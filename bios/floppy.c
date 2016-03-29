/*
 * floppy.c - floppy routines
 *
 * Copyright (c) 2001-2015 The EmuTOS development team
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
 * - internal floprw, fropwtrack
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

/*==== Internal prototypes ==============================================*/

/* set intel words */
static void setiword(UBYTE *addr, UWORD value);

/* floppy read/write */
static WORD floprw(UBYTE *buf, WORD rw, WORD dev,
                   WORD sect, WORD track, WORD side, WORD count);

/* floppy write track */
static WORD flopwtrack(UBYTE *buf, WORD dev, WORD track, WORD side, WORD track_size);

/* initialise a floppy for hdv_init */
static void flop_detect_drive(WORD dev);

#if CONF_WITH_FDC

/* called at start and end of a floppy access. */
static void floplock(WORD dev);
static void flopunlk(void);

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
#define DESELECT_TIMEOUT (3*CLOCKS_PER_SEC) /* in ticks */

/* access to dma and fdc registers */
static WORD get_dma_status(void);
static WORD get_fdc_reg(WORD reg);
static void set_fdc_reg(WORD reg, WORD value);
static void fdc_start_dma_read(WORD count);
static void fdc_start_dma_write(WORD count);
static void falcon_wait(void);

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

/*==== Internal floppy status =============================================*/

/* cur_dev is the current drive, or -1 if none is current.
 * the fdc track register will reflect the position of the head of the
 * current drive. 'current' does not mean 'active', because the flopvbl
 * routine may deselect the drive in the PSG port A. The routine
 * floplock(dev) will set the new current drive.
 *
 * drivetype is the type of drive; this is derived from the first byte of the
 * _FDC cookie.  note that TOS appears to assume that if you have two drives,
 * they are both of the same type.  so EmuTOS does the same.
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
 * the flock variable in tosvars.s is used as following :
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

static UBYTE drivetype;
#define DD_DRIVE    0x00
#define HD_DRIVE    0x01

static struct flop_info {
  WORD rate;        /* rate selected via Floprate() */
  WORD actual_rate; /* value to send to 1772 controller */
  BYTE cur_density;
#define DENSITY_DD  0x00
#define DENSITY_HD  0x03
#if CONF_WITH_FDC
  WORD cur_track;
  UBYTE wpstatus;       /* current write protect status */
  UBYTE wplatch;        /* latched copy of wpstatus */
#endif
} finfo[NUMFLOPPIES];

#define IS_VALID_FLOPPY_DEVICE(dev) ((UWORD)(dev) < NUMFLOPPIES && units[dev].valid)

/*==== hdv_init and hdv_boot ==============================================*/

static void flop_init(WORD dev)
{
    finfo[dev].rate = seekrate;
#if CONF_WITH_FDC
    finfo[dev].actual_rate = finfo[dev].rate;
    finfo[dev].cur_density = DENSITY_DD;
    finfo[dev].cur_track = -1;
    finfo[dev].wpstatus = 0;
    finfo[dev].wplatch = 0;
#endif
}

void flop_hdv_init(void)
{
    /* set floppy specific stuff */
    fverify = 0xff;
    seekrate = DD_STEPRATE_3MS;

    /* by default, there is no floppy drive */
    nflops = 0;
    flop_init(0);
    flop_init(1);

#if CONF_WITH_FDC
    cur_dev = -1;
    drivetype = (cookie_fdc >> 24) ? HD_DRIVE : DD_DRIVE;
    loopcount_fdc = loopcount_1_msec / 20;  /* 50 usec */
    deselect_time = 0UL;
#endif

    /* autodetect floppy drives */
    flop_detect_drive(0);
    flop_detect_drive(1);
}

/*
 * read boot sector from 'bootdev' into 'dskbufp'
 */
WORD flop_boot_read(void)
{
    return floprw(dskbufp,RW_READ,bootdev,1,0,0,1);
}

static void flop_add_drive(WORD dev)
{
#if CONF_WITH_FDC
    /* FDC floppy device */
    finfo[dev].cur_track = 0;
#endif

    /* Physical block device */
    units[dev].valid = 1;
    units[dev].byteswap = 0;        /* floppies are never byteswapped */
    units[dev].psshift = get_shift(SECTOR_SIZE);
    units[dev].size = 0;            /* unknown size */
    units[dev].last_access = 0;     /* never accessed */
    units[dev].features = UNIT_REMOVABLE;

    /* Logical block device */
    blkdev[dev].valid = 1;
    blkdev[dev].mediachange = MEDIACHANGE;
    blkdev[dev].start = 0;
    blkdev[dev].size = 0;           /* unknown size */
    blkdev[dev].geometry.sides = 2; /* default geometry of 3.5" DD */
    blkdev[dev].geometry.spt = 9;
    blkdev[dev].unit = dev;

    /* OS variables */
    nflops++;
    drvbits |= (1 << dev);
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

    KDEBUG(("flop_mediach(%d)\n",dev));

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
     * the current status was set, as was the latch.  we might have
     * inserted a WP diskette in an empty drive, or removed a WP diskette
     * from a drive, or even replaced one WP diskette by another.  we
     * attempt to read the boot sector to check the diskette serial number
     */
    if (floprw((UBYTE *)bootsec,RW_READ,dev,1,0,0,1) != 0) {
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
    WORD track;
    WORD side;
    WORD sect;
    WORD err;
    WORD bootsec = 0;

    KDEBUG(("floppy_rw(rw %d, buf 0x%lx, cnt %d, recnr %ld, spt %d, sides %d, dev %d)\n",
            rw,(ULONG)buf,cnt,recnr,spt,sides,dev));

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown device */

    /* set flag if reading boot sector */
    if ((cnt == 1) && (recnr == blkdev[dev].start)
     && (rw & RW_NOTRANSLATE) && (rw & RW_NOMEDIACH))
        bootsec = 1;

    /* do the transfer one sector at a time. It is easier to implement,
     * but perhaps slower when using FastRAM, as the time spent in memcpying
     * the sector in memory may force us to wait for a complete
     * rotation of the floppy before reading the next sector.
     */

    while (cnt > 0) {
        sect = (recnr % spt) + 1;
        track = recnr / spt;
        if (sides == 1) {
            side = 0;
        } else {
            side = track % 2;
            track /= 2;
        }
#if CONF_WITH_FRB
        if (buf >= (UBYTE *)phystop) {
            /* The buffer provided by the user is outside the ST-RAM,
             * but floprw() needs to use the DMA.
             * We must use the intermediate _FRB buffer.
             */
            if (cookie_frb) {
                if ((rw & RW_RW) == RW_WRITE)
                    memcpy(cookie_frb, buf, SECTOR_SIZE);
                err = floprw(cookie_frb, rw, dev, sect, track, side, 1);
                if ((rw & RW_RW) == RW_READ)
                    memcpy(buf, cookie_frb, SECTOR_SIZE);
            } else {
                err = -1;   /* problem: can't DMA to FastRAM */
            }
        } else
#endif
        {
            /* The buffer is in the ST-RAM, we can call floprw() directly */
            err = floprw(buf, rw, dev, sect, track, side, 1);
        }
        if (err) {
            struct flop_info *f;
            if (drivetype == DD_DRIVE)          /* DD only, so no retry */
                return err;
            if (!bootsec)                       /* not reading boot sector */
                return err;
            /* we now switch density and retry.  note that 'spt' will
             * certainly be wrong for the calculation of 'sect' and
             * 'track' above; however, since we're reading the very
             * first sector of the diskette, this doesn't matter.
             */
            f = &finfo[dev];
            KDEBUG(("switching density (current=%d)\n",f->cur_density));

            if (f->cur_density == DENSITY_DD)   /* retry with changed density */
                f->cur_density = DENSITY_HD;
            else f->cur_density = DENSITY_DD;
            bootsec = 0;                        /* avoid endless retries */
            continue;
        }
        buf += SECTOR_SIZE;
        recnr++;
        cnt--;
    }

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
    b->cksum[0] = cksum>>8;
    b->cksum[1] = cksum;
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
    return floprw(buf, RW_READ, dev, sect, track, side, count);
}


LONG flopwr(const UBYTE *buf, LONG filler, WORD dev,
            WORD sect, WORD track, WORD side, WORD count)
{
    return floprw(CONST_CAST(UBYTE *, buf), RW_WRITE, dev, sect, track, side, count);
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
 */

LONG flopver(WORD *buf, LONG filler, WORD dev,
             WORD sect, WORD track, WORD side, WORD count)
{
    WORD i, err;
    LONG rc = 0L;
    WORD *bad = (WORD *)buf;

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown disk */

    for (i = 0; i < count; i++, sect++) {
        err = floprw(dskbufp, RW_READ, dev, sect, track, side, 1);
        if (err) {
            *bad++ = sect ? sect : -1;
            if ((err != EREADF) && (err != ESECNF))
                rc = err;
        }
    }

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
    WORD track_size, leader, offset;
    BYTE b1, b2;
    UBYTE *s;
    LONG used, err;

#define APPEND(b, count) do { memset(s, b, count); s += count; } while(0)

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;          /* unknown disk */

    if (magic != 0x87654321UL)
        return EBADSF;          /* just like TOS4 */

    if ((spt >= 1) && (spt <= 10)) {
        track_size = TRACK_SIZE_DD;
        leader = LEADER_DD;
    } else if ((drivetype == HD_DRIVE) && (spt >= 13) && (spt <= 20)) {
        track_size = TRACK_SIZE_HD;
        leader = LEADER_HD;
    } else return EBADSF;       /* consistent, at least :-) */

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
    err = flopwtrack(buf, dev, track, side, track_size);
    if (err)
        return err;

    /* verify sectors and store bad sector numbers in buf */
    err = flopver((WORD *)buf, 0L, dev, 1, track, side, spt);
    if (err || (*(WORD *)buf != 0))
        return EBADSF;

    return 0;
}

/*==== xbios floprate ======================================================*/

/* sets the stepping rate of the specified drive.
 * rate meaning
 * 0   6ms
 * 1  12ms
 * 2   2ms
 * 3   3ms
 */

LONG floprate(WORD dev, WORD rate)
{
    WORD old;

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown disk */

    old = finfo[dev].rate;
    if (rate >= 0 && rate <= 3) {
        finfo[dev].rate = rate;
        finfo[dev].actual_rate = rate;
    }

    return old;
}

/*==== internal floprw ====================================================*/

static WORD floprw(UBYTE *buf, WORD rw, WORD dev,
                   WORD sect, WORD track, WORD side, WORD count)
{
    WORD err;
#if CONF_WITH_FDC
    WORD retry;
    WORD status;
    WORD cmd;
    LONG buflen = (LONG)count * SECTOR_SIZE;
#endif

    if (!IS_VALID_FLOPPY_DEVICE(dev))
        return EUNDEV;  /* unknown disk */

    rw &= RW_RW;    /* remove any extraneous bits */

    if ((rw == RW_WRITE) && (track == 0) && (sect == 1) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }

#ifdef MACHINE_AMIGA
    err = amiga_floprw(buf, rw, dev, sect, track, side, count);
    units[dev].last_access = hz_200;
#elif CONF_WITH_FDC
    /* flush data cache here so that memory is current */
    if (rw == RW_WRITE)
        flush_data_cache(buf,buflen);

    floplock(dev);

    select(dev, side);
    err = set_track(track);
    if (err) {
        flopunlk();
        return err;
    }

    while(count--) {
      for (retry = 0; retry < 2; retry++) {
        set_fdc_reg(FDC_SR, sect);
        set_dma_addr(buf);
        if (rw == RW_READ) {
            fdc_start_dma_read(1);
            cmd = FDC_READ;
        } else {
            fdc_start_dma_write(1);
            if (cookie_mch == MCH_FALCON)
                falcon_wait();
            cmd = FDC_WRITE;
        }
        if (flopcmd(cmd) < 0) {     /* timeout */
            err = EDRVNR;           /* drive not ready */
            break;                  /* no retry */
        }
        status = get_dma_status();
        if (!(status & DMA_OK)) {   /* DMA error, retry */
            err = EGENRL;           /* general error */
        } else {
            status = get_fdc_reg(FDC_CS);
            if ((rw == RW_WRITE) && (status & FDC_WRI_PRO)) {
                err = EWRPRO;       /* write protect */
                break;              /* no retry */
            } else if (status & FDC_RNF) {
                err = ESECNF;       /* sector not found */
            } else if (status & FDC_CRCERR) {
                err = E_CRC;        /* CRC error */
            } else if (status & FDC_LOSTDAT) {
                err = EDRVNR;       /* drive not ready */
            } else {
                err = 0;
                break;
            }
        }
      }
      //-- If there was an error, don't read any more sectors
      if (err)
          break;
      //-- Otherwise carry on sequentially
      buf += SECTOR_SIZE;
      sect++;
    }

    flopunlk();

    /* invalidate data cache if we've read into memory */
    if (rw == RW_READ)
        invalidate_data_cache(buf,buflen);
#else
    err = EUNDEV;
#endif
    return err;
}

/*==== internal flopwtrack =================================================*/

static WORD flopwtrack(UBYTE *buf, WORD dev, WORD track, WORD side, WORD track_size)
{
#if CONF_WITH_FDC
    WORD retry;
    WORD err;
    WORD status;

    if ((track == 0) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }

    /* flush cache here so that track image is pushed to memory */
    flush_data_cache(buf,track_size);

    floplock(dev);

    select(dev, side);
    err = set_track(track);
    if (err) {
        flopunlk();
        return err;
    }

    for (retry = 0; retry < 2; retry++) {
        set_dma_addr(buf);
        fdc_start_dma_write((track_size + SECTOR_SIZE-1) / SECTOR_SIZE);
        if (cookie_mch == MCH_FALCON)
            falcon_wait();
        if (flopcmd(FDC_WRITETR) < 0) { /* timeout */
            err = EDRVNR;               /* drive not ready */
            break;
        }
        status = get_dma_status();
        if (!(status & DMA_OK)) {       /* DMA error, retry */
            err = EGENRL;               /* general error */
        } else {
            status = get_fdc_reg(FDC_CS);
            if (status & FDC_WRI_PRO) {
                err = EWRPRO;           /* write protect */
                break;                  /* no retry */
            } else if (status & FDC_LOSTDAT) {
                err = EDRVNR;           /* drive not ready */
            } else {
                err = 0;
                break;
            }
        }
    }
    flopunlk();

    return err;
#else
    return EUNDEV;
#endif
}

#if CONF_WITH_FDC

/*==== internal status, flopvbl ===========================================*/


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

    /* for HD drives, always set density & update actual step rate */
    if (drivetype == HD_DRIVE) {
        DMA->modectl = f->cur_density;
        f->actual_rate = (f->cur_density == DENSITY_HD) ? hd_steprate[f->rate] : f->rate;
    }
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
}

/*
 * seek to the specified track on the current device
 * (does nothing if already at that track)
 */
static WORD set_track(WORD track)
{
    struct flop_info *fi = &finfo[cur_dev];
    WORD cmd;

    if (track == fi->cur_track)
        return 0;

    if (track == 0) {
        cmd = FDC_RESTORE;
    } else {
        set_fdc_reg(FDC_DR, track);
        cmd = FDC_SEEK;
    }

    if (flopcmd(cmd | fi->actual_rate) < 0) {   /* timeout */
        if (cmd == FDC_SEEK)
            flopcmd(FDC_RESTORE | fi->actual_rate); /* attempt to restore */
        fi->cur_track = 0;                          /* assume we did */
        return E_SEEK;  /* seek error */
    }

    fi->cur_track = track;
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

static WORD get_dma_status(void)
{
    WORD ret;

    DMA->control = DMA_SCREG | DMA_FDC;
    ret = DMA->control;

    return ret;
}

static WORD get_fdc_reg(WORD reg)
{
    WORD ret;

    DMA->control = reg;
    fdc_delay();
    ret = DMA->data;
    fdc_delay();

    return ret;
}

static void set_fdc_reg(WORD reg, WORD value)
{
    DMA->control = reg;
    fdc_delay();
    DMA->data = value;
    fdc_delay();
}

/* the fdc_start_dma_*() functions toggle the dma write bit, to
 * signal the DMA to clear its internal buffers (16 bytes in input,
 * 32 bytes in output). This is done just before issuing the
 * command to the fdc, after all fdc registers have been set.
 */

static void fdc_start_dma_read(WORD count)
{
    DMA->control = DMA_SCREG | DMA_FDC;
    DMA->control = DMA_SCREG | DMA_FDC | DMA_WRBIT;
    DMA->control = DMA_SCREG | DMA_FDC;
    DMA->data = count;
}

static void fdc_start_dma_write(WORD count)
{
    DMA->control = DMA_SCREG | DMA_FDC | DMA_WRBIT;
    DMA->control = DMA_SCREG | DMA_FDC;
    DMA->control = DMA_SCREG | DMA_FDC | DMA_WRBIT;
    DMA->data = count;
}

/*
 * during write sector and write track sequences, just before sending
 * the write command, Falcon TOS (TOS4) loops waiting for a bit in a
 * Falcon-only register (at $860f) to become zero.  at a guess, this
 * bit is set to zero when the DMA has been cleared, but who knows?
 * we just do the same.
 */
static void falcon_wait(void)
{
    while(DMA->modectl&DMA_MCBIT3)
        ;
}

#endif /* CONF_WITH_FDC */
