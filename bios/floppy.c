/*
 * floppy.c - floppy routines
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "gemerror.h"
#include "floppy.h"
#include "disk.h"
#include "dma.h"
#include "fdc.h"
#include "psg.h"
#include "mfp.h"
#include "asm.h"
#include "tosvars.h"
#include "machine.h"
#include "blkdev.h"
#include "string.h"
#include "kprint.h"
#include "xbiosbind.h"  /* Random() */
#include "delay.h"
#include "processor.h"
#include "cookie.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif


/*==== Introduction =======================================================*/

/*
 * This file contains all floppy-related bios and xbios routines.
 * They are stacked by sections, function of a higher level calling
 * functions of a lower level.
 *
 * sections in this file:
 * - private prototypes
 * - internal floppy status info
 * - disk initializations: hdv_init, hdv_boot
 * - boot-sector: protobt
 * - boot-sector utilities: intel format words
 * - xbios floprd, flopwr, flopver
 * - xbios flopfmt
 * - internal flopio, fropwtrack
 * - internal status, flopvbl
 * - low level dma and fdc registers access
 *
 */

/*
 * TODO and not implemented:
 * - on error, the geometry info should be reset to a sane state
 * - on error, should jump to critical error vector
 * - no 'virtual' disk B: mapped to disk A: when only one drive
 */

/*==== Internal defines ===================================================*/

#define TRACK_SIZE_DD      6250
#define TRACK_SIZE_HD      12500
#define LEADER_DD          60
#define LEADER_HD          120

/*
 * stepping rates
 */
#define DD_STEPRATE_6MS    0        /* double density */
#define DD_STEPRATE_12MS   1
#define DD_STEPRATE_2MS    2
#define DD_STEPRATE_3MS    3
#define HD_STEPRATE_3MS    0        /* high density */
#define HD_STEPRATE_6MS    1

/*
 * structure to track floppy drive state
 */
struct flop_info {
    WORD rate;          /* rate selected via Floprate() */
    WORD actual_rate;   /* value to send to 1772 controller */
    BYTE drive_type;
#define DD_DRIVE    0x00
#define HD_DRIVE    0x01
    BYTE cur_density;
#define DENSITY_DD  0x00
#define DENSITY_HD  0x03
#if CONF_WITH_FDC
    WORD cur_track;
    UBYTE wpstatus;     /* current write protect status */
    UBYTE wplatch;      /* latched copy of wpstatus */
#endif
};

/*
 * retry counts
 */
#define IO_RETRIES  2   /* actually the total number of tries */

/*==== Internal prototypes ==============================================*/

/* set intel words */
static void setiword(UBYTE *addr, UWORD value);

/* floppy read/write */
static WORD flopio(UBYTE *buf, WORD rw, WORD dev,
                   WORD sect, WORD track, WORD side, WORD count);
static WORD flopio_ver(UBYTE *buf, WORD rw, WORD dev,
                   WORD sect, WORD track, WORD side, WORD count);

/* floppy write track */
static WORD flopwtrack(UBYTE *buf, WORD dev, WORD track, WORD side,
                    WORD track_size, WORD density);

/* initialise a floppy for hdv_init */
static void flop_detect_drive(WORD dev);

#if CONF_WITH_FDC

/* called at start and end of a floppy access. */
static void floplock(WORD dev);
static void flopunlk(void);

/* set density on HD drives */
static void set_density(struct flop_info *f);
static void switch_density(WORD dev);

/* issue command to current drive */
static WORD flopcmd(WORD cmd);

/* select/deselect drive and side in the PSG port A */
static UBYTE set_psg_porta(UBYTE n);
static UBYTE convert_drive_and_side(WORD dev, WORD side);
static void select(WORD dev, WORD side);
static void deselect(void);

/* sets the track on the current drive, returns 0 or error */
static WORD set_track(WORD track);

/* called by flopunlk() to make FDC status available to flopvbl() */
static void dummy_seek(void);

/* time (in ticks) to wait before aborting any FDC command; this
 * must be longer than the time required for the longest command.
 * the time is greater if the motor is off and therefore spinup
 * is required.
 */
#define MOTORON_TIMEOUT  (3*CLOCKS_PER_SEC/2)   /* 1.5 seconds */
#define MOTOROFF_TIMEOUT (3*CLOCKS_PER_SEC)     /* 3.0 seconds */

/* deselection timeout (see "handling of drive deselection" below) */
#define DESELECT_TIMEOUT (5*CLOCKS_PER_SEC)     /* 5.0 seconds */

/* access to dma and fdc registers */
static WORD decode_error(void);
static WORD get_dma_status(void);
static WORD get_fdc_reg(WORD reg);
static void set_fdc_reg(WORD reg, WORD value);
static void fdc_start_dma_read(WORD count);
static void fdc_start_dma_write(WORD count);

/*
 * fdc_delay() is for worst-case 1772 access (in MFM mode)
 *
 * the maximum delay required is 32 usec; this is between a write to the
 * command register and a read of status bits 1-7 (see the WD177X-00
 * Floppy Disk Formatter/Controller manual, p1-16).  Atari TOS uses a
 * value of 52-78 usec for this (because of timer resolution issues).
 * EmuTOS uses 50 usec (see flop_hdv_init()).
 */
#define fdc_delay() delay_loop(loopcount_fdc)

/*
 * the following delay is used between toggling dma out.  in Atari TOS 3
 * & TOS 4, the delay is provided by an instruction sequence which takes
 * about 5usec on a Falcon.  EmuTOS uses 5usec (see flop_hdv_init()).
 */
#define toggle_delay() delay_loop(loopcount_toggle)


/*==== Internal floppy status =============================================*/

/* cur_dev is the current drive, or -1 if none is current.
 * the fdc track register will reflect the position of the head of the
 * current drive. 'current' does not mean 'active', because the flopvbl
 * routine may deselect the drive in the PSG port A. The routine
 * floplock(dev) will set the new current drive.
 *
 * finfo[] is an array of structures, one entry per floppy drive.  structure
 * members are as follows:
 *
 * finfo[].rate is the stepping rate set by Floprate() (or by 'seekrate' if
 * Floprate() has never been called).
 *
 * finfo[].actual_rate is the value to send to the 1772 chip to get the stepping
 * rate implied by finfo[].rate.  it differs from finfo[].rate for HD diskettes.
 *
 * finfo[].drive_type is the type of drive; this is derived from the first byte
 * of the _FDC cookie.  in two-drive systems, both drives are currently assumed
 * to be the same.
 *
 * finfo[].cur_density is the density (either DD or HD) being used to access
 * the current diskette.
 *
 * finfo[].cur_track contains a copy of the fdc track register for the current
 * drive, or -1 to indicate that the drive does not exist.
 *
 * finfo[].wpstatus and finfo[].wplatch are used by flopvbl() to help in
 * detecting a diskette change.  finfo[].wpstatus contains the most recent
 * state of the write-protect status bit.  finfo[].wplatch is a latched copy
 * of wpstatus.  see "handling of media change" below for more details.
 *
 * the flock system variable is used as following :
 * - floppy.c will set it before accessing to the DMA/FDC, and
 *   clear it at the end.
 * - acsi.c will set it before accessing to the DMA bus, and
 *   clear it at the end.
 * - flopvbl will do nothing when flock is set.
 */

/*==== Handling of drive deselection ======================================*/

/* when there is a diskette in the currently-selected drive, the FDC will
 * automatically turn off the motor after 10 revolutions if the device
 * remains idle.  this clears the FDC_MOTORON status bit, which is then
 * detected in flopvbl() which deselects the drive.  however, this method
 * does not work when no diskette is present, since there are no diskette
 * revolutions to be detected.
 *
 * to handle this situation, the local variable deselect_time stores the
 * earliest time (in ticks) to deselect the currently-selected drive.  when
 * this time is reached, flopvbl() deselects all drives.  a special value
 * of zero in deselect_time indicates that no drive is currently selected.
 *
 * deselect_time is set in flopunlk(), cleared in deselect(), and checked
 * in flopvbl().
 */

static WORD cur_dev;
static ULONG deselect_time;
static ULONG loopcount_fdc;
static ULONG loopcount_toggle;

/* the following is updated by flopvbl(), and used by flopcmd().  it is
 * non-zero iff the motor on bit is set in the floppy status byte.
 */
static WORD motor_on;

/*
 * the following array maps the stepping rate as requested by Floprate()
 * to the (closest possible) value to send to the 1772 chip when an HD
 * diskette is loaded.
 */
static const WORD hd_steprate[] =
 { HD_STEPRATE_6MS, HD_STEPRATE_6MS, HD_STEPRATE_3MS, HD_STEPRATE_3MS };

#endif /* CONF_WITH_FDC */

/*==== Handling of media change ===========================================*/

/* when Write Protect is set in the FDC status register, it indicates one
 * of two things: either there is no diskette inserted, or the inserted
 * diskette is write-protected.
 *
 * thus if we insert or eject a non-write-protected diskette, we can
 * detect it by the transition from non-WP to WP.  however, if we insert
 * or eject a write-protected diskette, WP is set before & after, so
 * there is no transition.  there is no temporary glitch in the WP line
 * during the insert or eject, contrary to some documentation.
 *
 * so we can use the status of the WP line only as a partial clue to media
 * change.  if WP changes, then there is a media change; but if it is set
 * and does not change, the media may have changed (this is presumably why
 * that status exists as a possible return from Mediach()).
 */

static struct flop_info finfo[NUMFLOPPIES];

#define IS_VALID_FLOPPY_DEVICE(dev) ((UWORD)(dev) < NUMFLOPPIES && units[dev].valid)

/*==== hdv_init and hdv_boot ==============================================*/

static void flop_init(WORD dev)
{
    struct flop_info *f = &finfo[dev];

    f->rate = seekrate;
#if CONF_WITH_FDC
    /* actual_rate is set by set_density() which is called by floplock() */
    f->drive_type = (cookie_fdc >> 24) ? HD_DRIVE : DD_DRIVE;
    f->cur_density = (f->drive_type == HD_DRIVE) ? DENSITY_HD : DENSITY_DD;
    f->cur_track = -1;
    f->wpstatus = 0;
    f->wplatch = 0;
#endif
}

void flop_hdv_init(void)
{
    /* set floppy specific stuff */
    fverify = 0xff;
    seekrate = DD_STEPRATE_3MS;

#if CONF_WITH_FDC
    cur_dev = -1;
    loopcount_fdc = loopcount_1_msec / 20;  /* 50 usec */
    loopcount_toggle = loopcount_1_msec / 200;  /* 5 usec */
    deselect_time = 0UL;
#endif

    /* by default, there is no floppy drive */
    nflops = 0;
#ifdef MACHINE_AMIGA
    amiga_floppy_init();
#endif
    flop_init(0);
    flop_init(1);

    /* autodetect floppy drives */
    flop_detect_drive(0);
    flop_detect_drive(1);

#if CONF_WITH_FDC
    /*
     * if drive 1 doesn't exist, the attempt to detect it will leave the
     * motor line on, causing drive 0 to spin until it's accessed, even
     * if it contains a diskette. to avoid that, we reselect drive 0 now.
     */
    if (!units[1].valid)
    {
        floplock(0);
        select(0,0);
        flopunlk();
    }
#endif
}

/*
 * read boot sector from 'bootdev' into 'dskbufp'
 */
WORD flop_boot_read(void)
{
    return flopio(dskbufp,RW_READ,bootdev,1,0,0,1);
}

static void flop_add_drive(WORD dev)
{
    UNIT *u = &units[dev];
    BLKDEV *b = &blkdev[dev];

#if CONF_WITH_FDC
    /* FDC floppy device */
    finfo[dev].cur_track = 0;
#endif

    /* Physical block device */
    u->valid = 1;
#if CONF_WITH_IDE
    u->byteswap = 0;            /* floppies are never byteswapped */
#endif
    u->psshift = get_shift(SECTOR_SIZE);
    u->size = 0;                /* unknown size */
    u->last_access = 0;         /* never accessed */
    u->features = UNIT_REMOVABLE;

    /* Logical block device */
    b->flags = DEVICE_VALID | GETBPB_ALLOWED;
    b->mediachange = MEDIACHANGE;
    b->start = 0;
    b->size = 0;                /* unknown size */
    b->geometry.sides = 2;      /* default geometry of 3.5" DD */
    b->geometry.spt = 9;
    b->unit = dev;
    b->bpb.recsiz = SECTOR_SIZE;

    /* OS variables */
    nflops++;
    drvbits |= (1L << dev);
}

static void flop_detect_drive(WORD dev)
{
    WORD status;

    MAYBE_UNUSED(status);
    MAYBE_UNUSED(flop_add_drive);

    KDEBUG(("flop_detect_drive(%d)\n",dev));

#ifdef MACHINE_AMIGA
    if (amiga_flop_detect_drive(dev)) {
        flop_add_drive(dev);
        units[dev].last_access = hz_200;
    }
    return;
#endif

#if CONF_WITH_FDC
    floplock(dev);

    select(dev, 0);
    if (flopcmd(FDC_RESTORE | FDC_HBIT | finfo[dev].actual_rate) < 0) {
        KDEBUG(("flop_detect_drive(%d) timeout\n",dev));
    } else {
        status = get_fdc_reg(FDC_CS);
        KDEBUG(("status = 0x%02x\n",status));
        if (status & FDC_TRACK0) {
            /* got track0 signal, this means that a drive is connected */
            KDEBUG(("track0 signal got\n"));
            flop_add_drive(dev);
        }
    }

    flopunlk();
#endif
}


/*
 * flop_mediach - return mediachange status for floppy
 */
LONG flop_mediach(WORD dev)
{
    struct fat16_bs *bootsec = (struct fat16_bs *) dskbufp;
    int unit;

    KDEBUG(("flop_mediach(%d)\n",dev));

#ifdef MACHINE_AMIGA
    return amiga_flop_mediach(dev);
#endif

#if CONF_WITH_FDC
    {
    struct flop_info *fi = &finfo[dev];

    /*
     * if the latch has not been set since we reset it last time, status
     * can never have been set, so there has been no diskette eject (and
     * the current diskette is not write-protected, fwiw)
     */
    if (!fi->wplatch) {
        KDEBUG(("flop_mediach(): no media change detected by flopvbl()\n"));
        return MEDIANOCHANGE;
    }

    /*
     * the latch has been set, so we have a possible or definite diskette
     * change.  we clear the latch & will report a change of some kind.
     */
    fi->wplatch = FALSE;

    /*
     * if the current status is clear, then we must have gone from a WP
     * diskette or empty drive to a non-WP diskette at some time since the
     * last call to flop_mediach(), so there must have been a diskette
     * load.  we report a definite mediachange.
     */
    if (!fi->wpstatus) {
        KDEBUG(("flop_mediach(): media change detected by flopvbl()\n"));
        return MEDIACHANGE;
    }

    }
#endif

    /*
     * if less than half a second since last access, assume no mediachange
     */
    unit = blkdev[dev].unit;
    if (hz_200 < units[unit].last_access + CLOCKS_PER_SEC/2)
        return MEDIANOCHANGE;

    /*
     * the current status was set, as was the latch.  we might have
     * inserted a WP diskette in an empty drive, or removed a WP diskette
     * from a drive, or even replaced one WP diskette by another.  we
     * attempt to read the boot sector to check the diskette serial number
     */
    if (flopio((UBYTE *)bootsec,RW_READ,dev,1,0,0,1) != 0) {
        KDEBUG(("flop_mediach(): can't read boot sector => media change\n"));
        return MEDIACHANGE;
    }

    KDEBUG(("flop_mediach() got bootsec, serial=0x%02x%02x%02x, serial2=0x%02x%02x%02x%02x\n",
            bootsec->serial[0],bootsec->serial[1],bootsec->serial[2],
            bootsec->serial2[0],bootsec->serial2[1],bootsec->serial2[2],bootsec->serial2[3]));

    if (memcmp(bootsec->serial,blkdev[dev].serial,3)
     || memcmp(bootsec->serial2,blkdev[dev].serial2,4)) {
        KDEBUG(("flop_mediach(): serial number change => media change\n"));
        return MEDIACHANGE;
    }

    /*
     * the serial number has not changed, but there could be two diskettes
     * with the same serial number, or the diskette could have been ejected,
     * modified on another system, and replaced.  so we have to say "maybe".
     */
    KDEBUG(("flop_mediach(): serial number unchanged => maybe media change\n"));

    return MEDIAMAYCHANGE;
}


LONG floppy_rw(WORD rw, UBYTE *buf, WORD cnt, LONG recnr, WORD spt,
               WORD sides, WORD dev)
{
    WORD start_trkside, end_trkside;
    WORD start_relsec, end_relsec;
    WORD track, side, numsecs, err;

    KDEBUG(("floppy_rw(): rw=%d, buf=%p, dev=%d, recnr=%ld, cnt=%d, spt=%d, sides=%d\n",
            rw,buf,dev,recnr,cnt,spt,sides));

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown device */

    /*
     * we divide the i/o into 3 sections, each of which may be empty:
     *  1. the sectors from the starting sector to the end of the starting track/side
     *  2. 0 or more entire track/sides
     *  3. the sectors from the start of the last track/side to the ending sector
     */
    start_trkside = recnr / spt;
    start_relsec = recnr - (spt * start_trkside);   /* zero-based sector number */

    recnr += cnt - 1;
    end_trkside = recnr / spt;
    end_relsec = recnr - (spt * end_trkside);

    KDEBUG(("floppy_rw(): start=%d/%d, end=%d/%d\n",
            start_trkside,start_relsec,end_trkside,end_relsec));

    /*
     * section 1: the sectors at the end of the first track/side
     */
    if (start_trkside < end_trkside) {
        if (sides == 1) {
            track = start_trkside;
            side = 0;
        } else {
            track = start_trkside >> 1;
            side = start_trkside & 0x0001;
        }
        numsecs = spt - start_relsec;
        KDEBUG(("floppy_rw() #1: track=%d, side=%d, start=%d, count=%d\n",
                track,side,start_relsec+1,numsecs));
        err = flopio_ver(buf, rw, dev, start_relsec+1, track, side, numsecs);
        if (err)
            return err;
        buf += SECTOR_SIZE * numsecs;
        start_trkside++;
        start_relsec = 0;
    }

    /*
     * section 2: zero or more entire track/sides
     */
    for ( ; start_trkside < end_trkside; start_trkside++) {
        if (sides == 1) {
            track = start_trkside;
            side = 0;
        } else {
            track = start_trkside >> 1;
            side = start_trkside & 0x0001;
        }
        KDEBUG(("floppy_rw() #2: track=%d, side=%d, start=%d, count=%d\n",
                track,side,1,spt));
        err = flopio_ver(buf, rw, dev, 1, track, side, spt);
        if (err)
            return err;
        buf += SECTOR_SIZE * spt;
    }

    /*
     * section 3: the sectors at the start of the last (or only) track/side
     */
    if (sides == 1) {
        track = start_trkside;
        side = 0;
    } else {
        track = start_trkside >> 1;
        side = start_trkside & 0x0001;
    }
    numsecs = end_relsec - start_relsec + 1;
    KDEBUG(("floppy_rw() #3: track=%d, side=%d, start=%d, count=%d\n",
            track,side,start_relsec+1,numsecs));
    err = flopio_ver(buf, rw, dev, start_relsec+1, track, side, numsecs);
    if (err)
        return err;

    return 0;
}

/*==== boot-sector: protobt =======================================*/
/*
 * note that (as in Falcon or TT TOS) you are allowed to create
 * a boot sector for a device type that is not on your system.
 */

struct _protobt {
    WORD bps;
    UBYTE spc;
    WORD res;
    UBYTE fat;
    WORD dir;
    WORD sec;
    UBYTE media;
    WORD spf;
    WORD spt;
    WORD sides;
    WORD hid;
};

static const struct _protobt protobt_data[] = {
    { SECTOR_SIZE, 1, 1, 2,  64,  360, 0xfc, 2, 9, 1, 0 },
    { SECTOR_SIZE, 2, 1, 2, 112,  720, 0xfd, 2, 9, 2, 0 },
    { SECTOR_SIZE, 2, 1, 2, 112,  720, 0xf9, 5, 9, 1, 0 },
    { SECTOR_SIZE, 2, 1, 2, 112, 1440, 0xf9, 5, 9, 2, 0 },
    { SECTOR_SIZE, 2, 1, 2, 224, 2880, 0xf0, 5, 18, 2, 0 }, /* for HD floppy */
    { SECTOR_SIZE, 2, 1, 2, 224, 5760, 0xf0, 10, 36, 2, 0 } /* for ED floppy */
};
#define NUM_PROTOBT_ENTRIES ARRAY_SIZE(protobt_data)

void protobt(UBYTE *buf, LONG serial, WORD type, WORD exec)
{
    WORD is_exec;
    struct bs *b = (struct bs *)buf;
    UWORD cksum;

    is_exec = (compute_cksum((const UWORD *)b) == 0x1234);

    if (serial >= 0) {
        if (serial >= 0x01000000)   /* create a random serial */
            serial = Random();
        b->serial[0] = serial;      /* set it (reversed as per Atari TOS) */
        b->serial[1] = serial>>8;
        b->serial[2] = serial>>16;
    }

    if (type >= 0 && type < NUM_PROTOBT_ENTRIES) {
        const struct _protobt *bt = &protobt_data[type];

        setiword(b->bps, bt->bps);
        b->spc = bt->spc;
        setiword(b->res, bt->res);
        b->fat = bt->fat;
        setiword(b->dir, bt->dir);
        setiword(b->sec, bt->sec);
        b->media = bt->media;
        setiword(b->spf, bt->spf);
        setiword(b->spt, bt->spt);
        setiword(b->sides, bt->sides);
        setiword(b->hid, bt->hid);
    }

    /*
     * Falcon & TT TOS 'exec' flag compatibility
     *  1. if 'exec' is negative, leave the executable *status* unchanged
     *  2. if 'exec' is zero, make it non-executable (checksum 0x1235)
     *  3. if 'exec' is greater than zero, make it executable (checksum 0x1234)
     *
     * note that a new checksum is calculated & stored every time.  also,
     * although the Protobt() specification is that a checksum of 0x1234
     * means executable & anything else means non-executable, both Falcon
     * & TT TOS always set a checksum of 0x1235 for non-executable, so we
     * do it as well for hyper-compatibility :-).
     */
    if (exec < 0)       /* keep in the same state */
        exec = is_exec ? 1 : 0;

    b->cksum[0] = b->cksum[1] = 0;
    cksum = 0x1234 - compute_cksum((const UWORD *)b);
    b->cksum[0] = HIBYTE(cksum);
    b->cksum[1] = LOBYTE(cksum);
    if (!exec)
        b->cksum[1]++;
}


/*==== boot-sector utilities ==============================================*/

static void setiword(UBYTE *addr, UWORD value)
{
    addr[0] = value;
    addr[1] = value >> 8;
}

/*==== xbios floprd, flopwr ===============================================*/


LONG floprd(UBYTE *buf, LONG filler, WORD dev,
            WORD sect, WORD track, WORD side, WORD count)
{
    return flopio(buf, RW_READ, dev, sect, track, side, count);
}


LONG flopwr(const UBYTE *buf, LONG filler, WORD dev,
            WORD sect, WORD track, WORD side, WORD count)
{
    return flopio(CONST_CAST(UBYTE *, buf), RW_WRITE, dev, sect, track, side, count);
}

/*==== xbios flopver ======================================================*/

/*
 * Flopver() is clearly not used very much, since both the Compendium
 * and Compute's "Complete TOS Reference" describe it incorrectly.
 * Atari's "Hitchhiker's Guide to the BIOS" has the most accurate
 * description, but it still leaves some things to the imagination.
 *
 * Unfortunately the actual behaviour of TOS does not help us much,
 * since the implementation of Flopver() in all versions of TOS has
 * several problems:
 *      the drive number is not checked
 *      a count of 0 or less causes an unending loop
 *      the bad sector list isn't terminated when an "unexpected" error occurs
 *
 * Also there is a somewhat theoretical hole in the design: a Flopver()
 * that includes sector 0 on a normal track should report an error, but
 * there is no way for the caller to distinguish between an entry of zero
 * in the bad sector list and the end of the list.  So, for example, if
 * you choose a starting sector of 0 and a count of 20 for a disk with
 * sectors 1-9 on that track, you will apparently have no bad sectors ...
 *
 * The following implementation follows the Hitchhiker's Guide with
 * additional interpretations:
 *      invalid disk number:
 *          return EUNDEV
 *      zero or negative count:
 *          return OK (no sectors to verify)
 *      error return code from floppy read:
 *          add sector to table of bad sectors; if sector zero was
 *          bad, we record it as -1
 *
 * Error return codes from floppy read are divided into two types:
 *      expected errors: EREADF or ESECNF
 *      unexpected errors: all others
 * If no unexpected errors occur, the return code from Flopver() is zero;
 * otherwise it's the return code from the last unexpected error.
 *
 * Two important points to remember: the buffer must be WORD aligned and
 * at least 1KB in length (actually 2*SECTOR_SIZE in this implementation).
 */

LONG flopver(WORD *buf, LONG filler, WORD dev,
             WORD sect, WORD track, WORD side, WORD count)
{
    LONG rc = 0L;
    WORD *bad = buf;
#if defined(MACHINE_AMIGA) || CONF_WITH_FDC
    WORD i, err;
    UBYTE *diskbuf = (UBYTE *)buf + SECTOR_SIZE;
#endif
#if CONF_WITH_FDC
    WORD retry;
    BOOL density_ok;
#endif

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown disk */

#ifdef MACHINE_AMIGA
    for (i = 0; i < count; i++, sect++) {
        err = amiga_floprw(diskbuf, RW_READ, dev, sect, track, side, 1);
        if (err) {
            *bad++ = sect ? sect : -1;
            if ((err != EREADF) && (err != ESECNF))
                rc = err;
        }
    }

    units[dev].last_access = hz_200;

#elif CONF_WITH_FDC

#if CONF_WITH_FRB
    if (!IS_STRAM_POINTER(buf)) {
        /*
         * the buffer provided by the user is outside ST-RAM, so we must
         * use the intermediate _FRB buffer
         */
        diskbuf = get_frb_cookie();
        if (!diskbuf) {
            KDEBUG(("flopver() error: can't DMA to Alt-RAM\n"));
            return EGENRL;
        }
    }
#endif

    floplock(dev);

    select(dev, side);
    rc = set_track(track);
    if (rc) {
        flopunlk();
        return rc;
    }

    /*
     * if the drive is double-density, then we know the density setting
     * (none required) is OK.  otherwise, we don't know at this point.
     */
    density_ok = (finfo[dev].drive_type==DD_DRIVE) ? TRUE : FALSE;

    for (i = 0; i < count; i++, sect++) {
        for (retry = 0; retry < IO_RETRIES; retry++) {
            set_fdc_reg(FDC_SR, sect);
            set_dma_addr(diskbuf);
            fdc_start_dma_read(1);
            if (flopcmd(FDC_READ) < 0) {/* timeout */
                err = EDRVNR;           /* drive not ready */
                break;                  /* no retry */
            }
            err = decode_error();
            if (err == 0)
                break;
            if ((err == ESECNF) && !density_ok) {   /* density _may_ be wrong */
                switch_density(dev);
                density_ok = TRUE;
                retry = 0;              /* allow retries after density switch */
                err = 0;
            }
        }
        if (err) {
            *bad++ = sect ? sect : -1;
            if ((err != EREADF) && (err != ESECNF))
                rc = err;
        }
    }

    flopunlk();

#else
    rc = EUNDEV;
#endif

    /* we always terminate the list of bad sectors! */
    *bad = 0;

    return rc;
}

/*==== xbios flopfmt ======================================================*/

LONG flopfmt(UBYTE *buf, WORD *skew, WORD dev, WORD spt,
             WORD track, WORD side, WORD interleave,
             ULONG magic, WORD virgin)
{
    int i, j;
    WORD density, track_size, leader, offset;
    BYTE b1, b2;
    UBYTE *s;
    LONG used, err;

#define APPEND(b, count) do { memset(s, b, count); s += count; } while(0)

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;          /* unknown disk */

    if (magic != 0x87654321UL)
        return EBADSF;          /* just like TOS4 */

    density = DENSITY_DD;       /* default density */
    switch(finfo[dev].drive_type) {
    case HD_DRIVE:
        if ((spt >= 13) && (spt <= 20)) {
            density = DENSITY_HD;
            track_size = TRACK_SIZE_HD;
            leader = LEADER_HD;
            break;
        }
        /* else drop thru */
    case DD_DRIVE:
        if ((spt >= 1) && (spt <= 10)) {
            track_size = TRACK_SIZE_DD;
            leader = LEADER_DD;
            break;
        }
    default:
        return EBADSF;          /* consistent, at least :-) */
    }

#if CONF_WITH_FRB
    if (!IS_STRAM_POINTER(buf)) {
        /*
         * the buffer provided by the user is outside ST-RAM, but flopwtrack()
         * needs to use DMA, so we must use the intermediate _FRB buffer
         */
        buf = get_frb_cookie();
        if (!buf) {
            KDEBUG(("flopfmt() error: can't DMA from Alt-RAM\n"));
            return EGENRL;
        }
    }
#endif

    /*
     * fixup interleave if not using skew table
     */
    if (interleave > 0)
        interleave = interleave % spt;
    if (interleave == 0)
        interleave = 1;

    s = buf;

    /*
     * create the image in memory.
     * track  ::= GAP1 record record ... record GAP5
     * record ::= GAP2 index GAP3 data GAP4
     */

    b1 = virgin >> 8;
    b2 = virgin;

    /* GAP1 + GAP2(part1) : 60/120 bytes 0x4E */
    APPEND(0x4E, leader);

    for (i = 0, offset = -interleave, used = 0L; i < spt ; i++) {
        /* GAP2 (part2) */
        APPEND(0x00, 12);

        /* id */
        APPEND(0xF5, 3);
        *s++ = 0xfe;            /* id address mark */
        *s++ = track;
        *s++ = side;
        if (interleave > 0) {   /* using interleave factor */
            offset = (offset+interleave) % spt;
            while(used&(1L<<offset))    /* skip offsets already used */
                offset = (offset+1) % spt;
            *s++ = offset + 1;  /* sector number from 'interleave' */
            used |= (1L<<offset);
        } else *s++ = *skew++;  /* sector number from skew array */
        *s++ = 2;               /* means sector of 512 bytes */
        *s++ = 0xf7;            /* generate 2 crc bytes */

        /* GAP3 */
        APPEND(0x4e, 22);
        APPEND(0x00, 12);

        /* data */
        APPEND(0xF5, 3);
        *s++ = 0xfb;            /* data address mark */
        for (j = 0; j < SECTOR_SIZE; j += 2) {
            *s++ = b1; *s++ = b2;
        }
        *s++ = 0xf7;            /* generate 2 crc bytes */

        /* GAP4 + GAP2(part1) */
        APPEND(0x4e, 40);
    }

    /* GAP5 : all the rest to fill raw track */
    APPEND(0x4E, track_size - leader - 614 * spt);

#undef APPEND

    /* write the buffer to track */
    err = flopwtrack(buf, dev, track, side, track_size, density);
    if (err)
        return err;

    /* verify sectors and store bad sector numbers in buf */
    err = flopver((WORD *)buf, 0L, dev, 1, track, side, spt);
    if (err)
        return err;
    if (*(WORD *)buf != 0)
        return EBADSF;

    return 0;
}

/*==== xbios floprate ======================================================*/

/*
 * sets the stepping rate of the specified drive:
 *  rate meaning
 *   0     6ms
 *   1    12ms
 *   2     2ms
 *   3     3ms
 * returns the previous value; to obtain the current value without
 * changing it, use a rate of -1
 *
 * as of TOS 3, the following additional, undocumented, special values
 * may be used for rate:
 *  -2: mark the specified drive as an HD drive (always returns 0)
 *  -3: mark the specified drive as a DD drive (always returns 0)
 *  -4: query the type of the specified drive (returns -1 for HD, 0 for DD)
 * the main purpose of this would seem to be to allow a mix of drive
 * types on e.g. a TT, via a small user program.
 */
LONG floprate(WORD dev, WORD rate)
{
    WORD old = 0;
    struct flop_info *f;

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown disk */

    f = &finfo[dev];

#if CONF_WITH_BIOS_EXTENSIONS
    if (rate == -4)
        old = (f->drive_type == HD_DRIVE) ? -1 : 0;
    else if (rate == -3)
        f->drive_type = DD_DRIVE;
    else if (rate == -2)
        f->drive_type = HD_DRIVE;
    else
#endif
    {
        old = f->rate;
        if (rate >= 0 && rate <= 3)
            f->rate = rate;     /* actual_rate is set via floplock() */
    }

    return old;
}

/*==== internal flopio ====================================================*/

static WORD flopio(UBYTE *userbuf, WORD rw, WORD dev,
                   WORD sect, WORD track, WORD side, WORD count)
{
    WORD err;
#if CONF_WITH_FDC
    WORD retry;
    WORD cmd;
    BOOL density_ok;
    UBYTE *tmpbuf = NULL, *iobufptr;
#endif

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown disk */

    rw &= RW_RW;    /* remove any extraneous bits */

    if ((rw == RW_WRITE) && (track == 0) && (sect == 1) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }

#ifdef MACHINE_AMIGA
    err = amiga_floprw(userbuf, rw, dev, sect, track, side, count);
    units[dev].last_access = hz_200;
#elif CONF_WITH_FDC
    floplock(dev);

    select(dev, side);
    err = set_track(track);
    if (err) {
        flopunlk();
        return err;
    }

    /*
     * if the drive is double-density, then we know the density setting
     * (none required) is OK.  otherwise, we don't know at this point.
     */
    density_ok = (finfo[dev].drive_type==DD_DRIVE) ? TRUE : FALSE;

    /*
     * the floppy hardware requires that the buffer be word-aligned and
     * located in ST-RAM.  if it isn't, we get an intermediate buffer.
     * since we only do one sector at a time, we don't care about the
     * buffer size.
     */
    if (IS_ODD_POINTER(userbuf) || !IS_STRAM_POINTER(userbuf)) {
        tmpbuf = dskbufp;
    }

    /*
     * if writing, we need to flush the data cache first, so that the
     * backing memory is current.  if we're not using a temporary buffer,
     * we can do it just once for efficiency.
     */
    if (rw && !tmpbuf)
        flush_data_cache(userbuf, (LONG)count * SECTOR_SIZE);

    while(count--) {
        iobufptr = tmpbuf ? tmpbuf : userbuf;
        if (rw && tmpbuf) {
            memcpy(tmpbuf, userbuf, SECTOR_SIZE);
            flush_data_cache(tmpbuf, SECTOR_SIZE);
        }

        for (retry = 0; retry < IO_RETRIES; retry++) {
            set_fdc_reg(FDC_SR, sect);
            set_dma_addr(iobufptr);
            if (rw == RW_READ) {
                fdc_start_dma_read(1);
                cmd = FDC_READ;
            } else {
                fdc_start_dma_write(1);
                cmd = FDC_WRITE;
            }
            if (flopcmd(cmd) < 0) {     /* timeout */
                err = EDRVNR;           /* drive not ready */
                break;                  /* no retry */
            }
            err = decode_error();
            if ((err == 0) || (err == EWRPRO))
                break;
            if ((err == ESECNF) && !density_ok) {   /* density _may_ be wrong */
                switch_density(dev);
                density_ok = TRUE;
                retry = 0;              /* allow retries after density switch */
                err = 0;
            }
        }
        /* If there was an error, don't read any more sectors */
        if (err)
            break;

        /*
         * if reading, invalidate data cache (this is low-cost, so
         * it's ok to do it on a sector-by-sector basis), then copy
         * from temporary buffer if necessary.
         */
        if (!rw) {
            invalidate_data_cache(iobufptr, SECTOR_SIZE);
            if (tmpbuf)
                memcpy(userbuf, tmpbuf, SECTOR_SIZE);
        }

        /* Otherwise carry on sequentially */
        userbuf += SECTOR_SIZE;
        sect++;
    }

    flopunlk();

#else
    err = EUNDEV;
#endif
    return err;
}

/*==== internal flopio_ver =================================================*/

/*
 * performs flopio() with optional verification when writing
 */
static WORD flopio_ver(UBYTE *buf, WORD rw, WORD dev, WORD sect, WORD track, WORD side, WORD count)
{
    WORD err;

    err = flopio(buf, rw, dev, sect, track, side, count);

    if ((rw & RW_WRITE) && (err == 0) && fverify)
    {
        err = flopver((WORD *)dskbufp, 0L, dev, sect, track, side, count);
        /*
         * flopver() returns 0 if it only encounters EREADF/ESECNF, but
         * the buffer will contain one or more non-zero sector numbers.
         * so in this case we set an error code.
         */
        if ((err == 0) && (*(WORD *)dskbufp != 0))
            err = EBADSF;
    }

    return err;
}

/*==== internal flopwtrack =================================================*/

static WORD flopwtrack(UBYTE *userbuf, WORD dev, WORD track, WORD side, WORD track_size, WORD density)
{
#if CONF_WITH_FDC
    WORD err;
    struct flop_info *f = &finfo[dev];

    if ((track == 0) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }

    /* flush cache here so that track image is pushed to memory */
    flush_data_cache(userbuf,track_size);

    f->cur_density = density;   /* used by floplock() */
    floplock(dev);

    select(dev, side);
    err = set_track(track);
    if (err) {
        flopunlk();
        return err;
    }

    set_dma_addr(userbuf);
    fdc_start_dma_write((track_size + SECTOR_SIZE-1) / SECTOR_SIZE);

    if (flopcmd(FDC_WRITETR) < 0) {     /* timeout: */
        err = EDRVNR;                   /* drive not ready */
    } else {
        err = decode_error();
    }

    flopunlk();

    return err;
#else
    return EUNDEV;
#endif
}

#if CONF_WITH_FDC

/*==== internal status, flopvbl ===========================================*/

/*
 * set density & update step rate
 */
static void set_density(struct flop_info *f)
{
    if (has_modectl)
        DMA->modectl = f->cur_density;

    f->actual_rate = (f->cur_density == DENSITY_HD) ? hd_steprate[f->rate] : f->rate;
}

/*
 * switch density / step rate (HD drives only)
 */
static void switch_density(WORD dev)
{
    struct flop_info *f = &finfo[dev];

    if (f->drive_type != HD_DRIVE)
        return;

    KDEBUG(("switching density (current=%d)\n",f->cur_density));
    f->cur_density = (f->cur_density==DENSITY_DD) ? DENSITY_HD : DENSITY_DD;
    set_density(f);
}

static void floplock(WORD dev)
{
struct flop_info *f = &finfo[dev];

    /* prevent the VBL from accessing the FDC */
    flock = 1;

    if (dev != cur_dev) {
        cur_dev = dev;

        /*
         * the FDC has only one track register for two units.
         * we need to save the current value, and switch
         */
        if (f->cur_track >= 0) {
            set_fdc_reg(FDC_TR, f->cur_track);
        }
    }

    /* set density (if applicable) */
    set_density(f);
}

static void flopunlk(void)
{
    LONG now;

    /*
     * issue a seek to the current track on the current device.
     *
     * because a seek is a type I floppy command, this ensures that
     * the status subsequently fetched by flopvbl() is valid, allowing
     * flopvbl() to handle media change properly.
     */
    dummy_seek();

    now = hz_200;       /* only fetch it once */
    if (cur_dev >= 0)
        units[cur_dev].last_access = now;
    deselect_time = now + DESELECT_TIMEOUT;

    /* the VBL will deselect the drive subsequently */
    flock = 0;
}

/*
 * flopvbl() performs two functions:
 *  . detects a change of diskette
 *  . turns off the drive motor when it times out
 */
void flopvbl(void)
{
    WORD n, status;
    UBYTE a, old_a, wp;

    /* if there are no floppies, do nothing */
    if (nflops == 0)
        return;

    /* don't do anything if the DMA circuitry is being used */
    if (flock)
        return;

    /* only do something every 8th VBL */
    if (frclock & 7)
        return;

    /*
     * handle diskette change
     * we check one floppy every 8 VBLs, like Atari TOS
     */
    if ((nflops == 2)           /* if there are 2 floppies and */
     && (frclock & 0x0008))     /* it's an odd multiple of 8,  */
        n = 1;                  /* check floppy 1              */
    else n = 0;

    /*
     * we read the write-protect bit in the status register for the
     * appropriate floppy drive, to help in detecting media change.
     * for more info, see the comments at the beginning of this module.
     */
    a = convert_drive_and_side(n,0);
    old_a = set_psg_porta(a);   /* remember previous drive/side bits */

    status = get_fdc_reg(FDC_CS);
    motor_on = status & FDC_MOTORON;    /* remember for flopcmd()'s use */

    wp = status & FDC_WRI_PRO;
    finfo[n].wpstatus = wp;
    finfo[n].wplatch |= wp;

    set_psg_porta(old_a);       /* reselect previous drive/side */

    /* if no drives are selected, no need to deselect them */
    if (deselect_time == 0UL)
        return;

    /*
     * we deselect the drives for two reasons:
     * 1. it's been a while since any drive has been accessed.  this
     *    handles the case of an empty drive that has been accessed,
     *    leaving the motor on (the FDC never times out, because it
     *    never sees any indexes)
     * 2. the motor is off (due to the FDC timing out after 10
     *    revolutions)
     */
    if ((hz_200 > deselect_time) || ((status & FDC_MOTORON) == 0)) {
        deselect();
        motor_on = 0;
    }
}

/*==== low level register access ==========================================*/

 /*
 * sets the floppy-drive related bits of PSG port A
 * returns the previous value of those bits
 */
static UBYTE set_psg_porta(UBYTE n)
{
    WORD old_sr;
    UBYTE a, old_a;

    old_sr = set_sr(0x2700);
    PSG->control = PSG_PORT_A;
    old_a = PSG->control;

    a = (old_a & 0xf8) | (n & 0x07);/* remove old drive & side bits, insert new */

    PSG->data = a;
    set_sr(old_sr);

    return old_a & 0x07;
}

 /*
 * convert floppy drive/side to PSG port A bits
 */
static UBYTE convert_drive_and_side(WORD dev, WORD side)
{
    UBYTE n;

    n = 0x07;       /* default is deselect both floppies, set side = 0 */
    switch(dev) {
    case 0:
        n &= ~2;        /* select floppy 0 */
        break;
    case 1:
        n &= ~4;        /* select floppy 1 */
        break;
    }
    if (side != 0)
        n &= ~1;        /* set side = 1 */

    return n;
}

/*
 * deselect floppies
 */
static void deselect(void)
{
    select(-1,0);
    deselect_time = 0UL;
}

/*
 * select floppy drive and side
 *
 * note: an invalid drive (e.g. -1) will leave both floppies deselected
 */
static void select(WORD dev, WORD side)
{
    UBYTE n;

    n = convert_drive_and_side(dev,side);
    set_psg_porta(n);
}

/*
 * seek to the current track on the current device
 */
static void dummy_seek(void)
{
    struct flop_info *fi;
    WORD track;

    /* if there's no current device, do nothing */
    if (cur_dev < 0)
        return;

    fi = &finfo[cur_dev];
    track = fi->cur_track;

    /* if there's no current track, do nothing */
    if (track < 0)
        return;

    set_fdc_reg(FDC_DR,track);
    flopcmd(FDC_SEEK | fi->actual_rate);    /* ignore any errors here */

    get_fdc_reg(FDC_CS);                    /* resets IRQ */
}

 /*
 * restore head to track 0 on specified device
 */
static void restore(struct flop_info *fi)
{
    if (flopcmd(FDC_RESTORE | fi->actual_rate) == 0)
        if (get_fdc_reg(FDC_CS) & FDC_TRACK0)
            fi->cur_track = 0;
}

/*
 * seek to the specified track on the current device
 * (does nothing if already at that track)
 *
 * returns  0       ok
 *          E_SEEK  seek error
 */
static WORD set_track(WORD track)
{
    struct flop_info *fi = &finfo[cur_dev];

    if (track == fi->cur_track)
        return 0;

    set_fdc_reg(FDC_DR, track);
    if (flopcmd(FDC_SEEK | fi->actual_rate) < 0)    /* timeout */
    {
        restore(fi);
        return E_SEEK;
    }

    fi->cur_track = track;          /* all ok */
    return 0;
}

/*
 * send command to fdc
 *
 * returns   0  ok
 *          -1  timeout
 */
static WORD flopcmd(WORD cmd)
{
    LONG timeout;
    WORD reg;

    /* set timeout based on motor-on status from flopvbl() */
    timeout = motor_on ? MOTORON_TIMEOUT : MOTOROFF_TIMEOUT;

    switch(cmd&0xf0) {
    case FDC_WRITE:             /* Write Sector */
    case (FDC_WRITE|FDC_MBIT):  /* Write Multiple Sector */
    case FDC_WRITETR:           /* Write Track */
        reg = FDC_CS | DMA_WRBIT;
        break;
    default:
        reg = FDC_CS;
    }
    set_fdc_reg(reg, cmd);

    if (timeout_gpip(timeout)) {
        set_fdc_reg(FDC_CS,FDC_IRUPT);  /* Force Interrupt */
        fdc_delay();                    /* allow it to complete */
        return -1;
    }

    return 0;
}

/*
 * decode DMA/FDC error status
 */
static WORD decode_error(void)
{
    WORD status;

    status = get_dma_status();
    if (!(status & DMA_OK))     /* DMA error */
        return EGENRL;

    status = get_fdc_reg(FDC_CS);
    if (status & FDC_RNF)       /* can be symptom of wrong density */
        return ESECNF;
    if (status & FDC_WRI_PRO)
        return EWRPRO;          /* write protect */
    if (status & FDC_CRCERR)
        return E_CRC;           /* CRC error */
    if (status & FDC_LOSTDAT)
        return EDRVNR;          /* drive not ready */

    return 0;
}

static WORD get_dma_status(void)
{
    WORD ret;

    DMA->control = DMA_SCREG | DMA_FLOPPY;
    ret = DMA->control;

    return ret;
}

static WORD get_fdc_reg(WORD reg)
{
    DMA->control = reg;
    fdc_delay();
    return DMA->data;
}

static void set_fdc_reg(WORD reg, WORD value)
{
    DMA->control = reg;
    fdc_delay();
    DMA->data = value;
}

/* the fdc_start_dma_*() functions toggle the dma write bit, to
 * signal the DMA to clear its internal buffers (16 bytes in input,
 * 32 bytes in output). This is done just before issuing the
 * command to the fdc, after all fdc registers have been set.
 */

static void fdc_start_dma_read(WORD count)
{
    DMA->control = DMA_SCREG | DMA_FLOPPY | DMA_WRBIT;
    toggle_delay();
    DMA->control = DMA_SCREG | DMA_FLOPPY;
    toggle_delay();
    DMA->data = count;
}

static void fdc_start_dma_write(WORD count)
{
    DMA->control = DMA_SCREG | DMA_FLOPPY;
    toggle_delay();
    DMA->control = DMA_SCREG | DMA_FLOPPY | DMA_WRBIT;
    toggle_delay();
    DMA->data = count;

/*
 * during write sector and write track sequences, just before sending
 * the write command, Falcon TOS (TOS4) loops waiting for bit 3 in the
 * "mode control" register (at $860f) to become zero.  we do the same.
 */
    if (cookie_mch == MCH_FALCON)
        while(DMA->modectl&DMA_MCBIT3)
            ;
}

#endif /* CONF_WITH_FDC */
