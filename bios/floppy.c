/*
 * floppy.c - floppy routines
 *
 * Copyright (c) 2001-2012 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_FLOP 0

#include "config.h"
#include "portab.h"
#include "gemerror.h"
#include "floppy.h"
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
#include "xbiosbind.h"  /* random() */
#include "delay.h"
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
 * - boot-sector utilities: compute_cksum, intel format words
 * - xbios floprd, flopwr, flopver
 * - xbios flopfmt
 * - internal floprw, fropwtrack
 * - internal status, flopvbl
 * - low level dma and fdc registers access
 *
 */

/*
 * TODO and not implemented:
 * - mediach does not check the write-protect status
 * - on error, the geometry info should be reset to a sane state
 * - on error, should jump to critical error vector
 * - no 'virtual' disk B: mapped to disk A: when only one drive
 * - reserved or hidden sectors are not guaranteed to be handled correctly
 * - ... (search for 'TODO' in the rest of this file)
 * - the unique FDC track register is probably not handled correctly
 *   when using two drives
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

/* set/get intel words */
static UWORD compute_cksum(struct bs *buf);
static void setiword(UBYTE *addr, UWORD value);

static LONG flop_bootcheck(void);

/* floppy read/write */
static WORD floprw(LONG buf, WORD rw, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 

/* floppy write track */
static WORD flopwtrack(LONG buf, WORD dev, WORD track, WORD side, WORD track_size);

/* initialise a floppy for hdv_init */
static void flop_detect_drive(WORD dev);

#if CONF_WITH_FDC

/* called at start and end of a floppy access. */
static void floplock(WORD dev);
static void flopunlk(void);

/* select drive and side in the PSG port A */
static void select(WORD dev, WORD side);

/* sets the track on the current drive, returns 0 or error */
static WORD set_track(WORD track);

/* time to wait before aborting any FDC command.
 * this must be longer than the longest command.
 * seeking to the end with spin-up sequence may take more than 2 seconds.
 */
#define TIMEOUT 3000L /* in milliseconds */

/* access to dma and fdc registers */
static WORD get_dma_status(void);
static WORD get_fdc_reg(WORD reg);
static void set_fdc_reg(WORD reg, WORD value);
static void fdc_start_dma_read(WORD count);
static void fdc_start_dma_write(WORD count);

/* delay for fdc register access */
#define delay() delay_loop(loopcount_3_usec)

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
 * finfo[].cur_track contains a copy of the fdc track register for the current
 * drive, or -1 to indicate that the drive does not exist.
 *
 * finfo[].cur_density is the density (either DD or HD) being used to access
 * the current diskette.
 *
 * finfo[].last_access is set to the value of the 200 Hz counter at the end of
 * last fdc command. last_access can be used by mediach, a short time 
 * elapsed indicating that the floppy was not ejected.
 * 
 * finfo[].wp is set according to the corresponding bit in the fdc 
 * controller status reg. As soon as this value is different for
 * the drive, this means that the floppy has changed.
 *
 * finfo[].rate is the stepping rate set by Floprate() (or by 'seekrate' if
 * Floprate() has never been called).
 *
 * finfo[].actual_rate is the value to send to the 1772 chip to get the stepping
 * rate implied by finfo[].rate.  it differs from finfo[].rate for HD diskettes.
 * 
 * the flock variable in tosvars.s is used as following :
 * - floppy.c will set it before accessing to the DMA/FDC, and
 *   clear it at the end.
 * - acsi.c will set it before accessing to the DMA bus, and
 *   clear it at the end.
 * - flopvbl will do nothing when flock is set.
 *
 * deselected is not null when no drives are selected in PSG port A.
 * it is cleared in select() and set back in flopvbl() when the drives
 * are automatically deselected when the motor on bit is cleared in the
 * FDC status reg.
 */

static WORD cur_dev;
static UBYTE deselected;
static ULONG loopcount_3_usec;

/*
 * the following array maps the stepping rate as requested by Floprate()
 * to the (closest possible) value to send to the 1772 chip when an HD
 * diskette is loaded.
 */
static const WORD hd_steprate[] =
 { HD_STEPRATE_6MS, HD_STEPRATE_6MS, HD_STEPRATE_3MS, HD_STEPRATE_3MS };

#endif /* CONF_WITH_FDC */

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
  BYTE wp;           /* != 0 means write protected */
#endif
} finfo[NUMFLOPPIES];

#define IS_VALID_FLOPPY_DEVICE(dev) ((UWORD)(dev) < NUMFLOPPIES && devices[dev].valid)

/*==== hdv_init and hdv_boot ==============================================*/

static void flop_init(WORD dev)
{
    finfo[dev].rate = seekrate;
#if CONF_WITH_FDC
    finfo[dev].actual_rate = finfo[dev].rate;
    finfo[dev].cur_track = -1;
    finfo[dev].cur_density = DENSITY_DD;
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
    loopcount_3_usec = 3 * loopcount_1_msec / 1000;

    /* I'm unsure, so let flopvbl() do the work of figuring out. */
    deselected = 1;
#endif

    /* autodetect floppy drives */
    flop_detect_drive(0);
    flop_detect_drive(1);
}

static void flop_add_drive(WORD dev)
{
#if CONF_WITH_FDC
    /* FDC floppy device */
    finfo[dev].cur_track = 0;
#endif

    /* Physical block device */
    devices[dev].valid = 1;
    devices[dev].byteswap = 0;      /* floppies are never byteswapped */
    devices[dev].pssize = SECTOR_SIZE;  /* default */
    devices[dev].size = 0;          /* unknown size */
    devices[dev].last_access = 0;   /* never accessed */

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

#if DBG_FLOP
    kprintf("flop_detect_drive(%d)\n", dev);
#endif

#ifdef MACHINE_AMIGA
    if (amiga_flop_detect_drive(dev)) {
        flop_add_drive(dev);
        devices[dev].last_access = hz_200;
    }
    return;
#endif

#if CONF_WITH_FDC
    floplock(dev);
    select(dev, 0);
    set_fdc_reg(FDC_CS, FDC_RESTORE | finfo[cur_dev].actual_rate);
    if(timeout_gpip(TIMEOUT)) {
        /* timeout */
#if DBG_FLOP
        kprintf("flop_detect_drive(%d) timeout\n", dev);
#endif
        flopunlk();
        return;
    }
    status = get_fdc_reg(FDC_CS);
#if DBG_FLOP
    kprintf("status = 0x%02x\n", status);
#endif
    if(status & FDC_TRACK0) {
        /* got track0 signal, this means that a drive is connected */
#if DBG_FLOP
        kprintf("track0 signal got\n" );
#endif
        flop_add_drive(dev);
    } 
    flopunlk();
#endif
}


/*
 * flop_mediach - return mediachange status for floppy
 */
LONG flop_mediach(WORD dev)
{
    int unit;
    WORD err;
    struct bs *bootsec = (struct bs *) dskbufp;
#if DBG_FLOP
    kprintf("flop_mediach(%d)\n",dev);
#endif

    /* if less than half a second since last access, assume no mediachange */
    unit = blkdev[dev].unit;
    if (hz_200 < devices[unit].last_access + 100)
        return MEDIANOCHANGE;

    /* TODO, monitor write-protect status in flopvbl... */
    
#if DBG_FLOP
    kprintf("flop_mediach() read bootsec\n");
#endif
    /* for now, assume it is unsure and look at the serial number */
    /* read bootsector at track 0, side 0, sector 1 */
    err = floprw((LONG)bootsec,RW_READ,dev,1,0,0,1);
    if (err)        /* can't even read the bootsector */
        return MEDIACHANGE;

#if DBG_FLOP
    kprintf("flop_mediach() got bootsec, serial=0x%02x%02x%02x\n",
            bootsec->serial[0],bootsec->serial[1],bootsec->serial[2]);
#endif
    if (memcmp(bootsec->serial,blkdev[dev].serial,3))
        return MEDIACHANGE;

#if DBG_FLOP
    kprintf("flop_mediach() serial is unchanged\n");
#endif
    return MEDIAMAYCHANGE;
}


LONG flop_hdv_boot(void)
{
    LONG err;

    err = flop_bootcheck();
#if DBG_FLOP
    kprintf("flop_bootcheck returns %ld\n", err);
#endif
    if(err == 0) {
        /* if bootable, jump in it */
        regsafe_call(dskbufp);
    }
    return err;         /* may never be reached, if booting */
}
  

static LONG flop_bootcheck(void)
{
    struct bs *b = (struct bs *) dskbufp;
    WORD err;
    WORD cksum;

    if(nflops ==0) {
        return 2;    /* no drive */
    }
    if(bootdev >= nflops) {
        return 2;    /* couldn't load */
    }

    err = floprw((LONG)b, RW_READ, bootdev, 1, 0, 0, 1);
    if(err) {
        return 3;    /* unreadable */
    }
    cksum = compute_cksum(b);
    if(cksum == 0x1234) {
        return 0;    /* bootable */
    } else {
        return 4;    /* not valid boot sector */
    }
}
 
LONG floppy_rw(WORD rw, LONG buf, WORD cnt, LONG recnr, WORD spt, 
               WORD sides, WORD dev)
{
    WORD track;
    WORD side;
    WORD sect;
    WORD err;
    WORD bootsec = 0;

#if DBG_FLOP
    kprintf("floppy_rw(rw %d, buf 0x%lx, cnt %d, ", rw, buf, cnt);
    kprintf("recnr %ld, spt %d, sides %d, dev %d)\n", recnr, spt, sides, dev);
#endif

    if (!IS_VALID_FLOPPY_DEVICE(dev)) return EUNDEV;  /* unknown device */

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
#if CONF_WITH_ALT_RAM
        if ((void *)buf >= phystop) {
            /* The buffer provided by the user is outside the ST-RAM,
             * but floprw() needs to use the DMA.
             * We must use the intermediate _FRB buffer.
             */
            if (cookie_frb > 0) {
                /* do we really need proper FRB lock? (TODO) */
                if(rw & 1) {
                    /* writing */ 
                    memcpy((void *)cookie_frb, (void *)buf, SECTOR_SIZE);
                    err = floprw(cookie_frb, rw, dev, sect, track, side, 1);
                } else {
                    /* reading */
                    err = floprw(cookie_frb, rw, dev, sect, track, side, 1);
                    memcpy((void *)buf, (void *)cookie_frb, SECTOR_SIZE);
                }
                /* proper FRB unlock (TODO) */
            } else {
                err = -1;   /* problem: can't DMA to FastRAM */
            }
        } else
#endif
        {
            /* The buffer is in the ST-RAM, we can call floprw() directly */
            err = floprw(buf, rw, dev, sect, track, side, 1);
        }
        if(err) {
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
#if DBG_FLOP
            kprintf("switching density (current=%d)\n",f->cur_density);
#endif
            if (f->cur_density == DENSITY_DD)   /* retry with changed density */
                f->cur_density = DENSITY_HD;
            else f->cur_density = DENSITY_DD;
            bootsec = 0;                        /* avoid endless retries */
            continue;
        }
        buf += SECTOR_SIZE;
        recnr ++;
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
#define NUM_PROTOBT_ENTRIES (sizeof(protobt_data)/sizeof(struct _protobt))

void protobt(LONG buf, LONG serial, WORD type, WORD exec)
{
    WORD is_exec;
    struct bs *b = (struct bs *)buf;
    UWORD cksum;
  
    is_exec = (compute_cksum(b) == 0x1234);
  
    if (serial >= 0) {
        if (serial >= 0x01000000)   /* create a random serial */
            serial = Random();
        b->serial[0] = serial;      /* set it (reversed as per Atari TOS) */
        b->serial[1] = serial>>8;
        b->serial[2] = serial>>16;
    }
    
    if(type >= 0 && type < NUM_PROTOBT_ENTRIES) {
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
    cksum = 0x1234 - compute_cksum(b);
    b->cksum[0] = cksum>>8;
    b->cksum[1] = cksum;
    if (!exec)
        b->cksum[1]++;
}


/*==== boot-sector utilities ==============================================*/

static UWORD compute_cksum(struct bs *buf)
{
    int i;
    UWORD sum, *w;

    for (i = 0, sum = 0, w = (UWORD *)buf; i < SECTOR_SIZE/sizeof(UWORD); i++, w++)
        sum += *w;

    return sum;
}

static void setiword(UBYTE *addr, UWORD value)
{
    addr[0] = value;
    addr[1] = value >> 8;
}

/*==== xbios floprd, flopwr ===============================================*/


LONG floprd(LONG buf, LONG filler, WORD dev, 
            WORD sect, WORD track, WORD side, WORD count)
{
    return floprw(buf, RW_READ, dev, sect, track, side, count);
}


LONG flopwr(LONG buf, LONG filler, WORD dev, 
            WORD sect, WORD track, WORD side, WORD count)
{
    return floprw(buf, RW_WRITE, dev, sect, track, side, count);
}

/*==== xbios flopver ======================================================*/

/* TODO, in the case where both one sector cannot be read and another is
 * read wrong, what is the error return code? 
 */

LONG flopver(LONG buf, LONG filler, WORD dev, 
             WORD sect, WORD track, WORD side, WORD count)
{
    WORD i;
    WORD err;
    WORD outerr = 0;
    WORD *bad = (WORD *) buf;
    
    if(count <= 0) return 0;
    if(!IS_VALID_FLOPPY_DEVICE(dev)) return EUNDEV;  /* unknown disk */
    for(i = 0 ; i < count ; i++) {
        err = floprw((LONG) dskbufp, RW_READ, dev, sect, track, side, 1);
        if(err) {
            *bad++ = sect;
            outerr = err;
            continue;
        }
        sect ++;
    }
    
    if(outerr) {
        *bad = 0;
    }
    return outerr;
}


/*==== xbios flopfmt ======================================================*/

LONG flopfmt(LONG buf, WORD *skew, WORD dev, WORD spt,
             WORD track, WORD side, WORD interleave, 
             ULONG magic, WORD virgin)
{
    int i, j;
    WORD track_size, leader, offset;
    BYTE b1, b2;
    BYTE *s;
    LONG used, err;

#define APPEND(b, count) do { memset(s, b, count); s += count; } while(0)

    if(magic != 0x87654321UL) return 0;
    if(!IS_VALID_FLOPPY_DEVICE(dev)) return EUNDEV;  /* unknown disk */

    if ((spt >= 1) && (spt <= 10)) {
        track_size = TRACK_SIZE_DD;
        leader = LEADER_DD;
    } else if ((drivetype == HD_DRIVE) && (spt >= 13) && (spt <= 20)) {
        track_size = TRACK_SIZE_HD;
        leader = LEADER_HD;
    } else return EGENRL;     /* general error */

    /*
     * fixup interleave if not using skew table
     */
    if (interleave > 0)
        interleave = interleave % spt;
    if (interleave == 0)
        interleave = 1;

    s = (BYTE *)buf;

    /*
     * create the image in memory. 
     * track  ::= GAP1 record record ... record GAP5
     * record ::= GAP2 index GAP3 data GAP4
     */

    b1 = virgin >> 8;
    b2 = virgin;

    /* GAP1 + GAP2(part1) : 60/120 bytes 0x4E */  
    APPEND(0x4E, leader);
  
    for(i = 0, offset = -interleave, used = 0L; i < spt ; i++) {
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
        *s++ = 2; /* means sector of 512 bytes */
        *s++ = 0xf7;            /* generate 2 crc bytes */

        /* GAP3 */
        APPEND(0x4e, 22);
        APPEND(0x00, 12);

        /* data */
        APPEND(0xF5, 3);
        *s++ = 0xfb;            /* data address mark */
        for(j = 0; j < SECTOR_SIZE; j += 2) {
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
    if(err) return err;

    /* verify sectors and store bad sector numbers in buf */
    err = flopver(buf, 0L, dev, 1, track, side, spt);
    if(err) return EBADSF;
  
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

    if(!IS_VALID_FLOPPY_DEVICE(dev)) return EUNDEV;  /* unknown disk */
    old = finfo[dev].rate;
    if(rate >= 0 && rate <= 3) {
        finfo[dev].rate = rate;
        finfo[dev].actual_rate = rate;
    }
    return old;
}

/*==== internal floprw ====================================================*/

static WORD floprw(LONG buf, WORD rw, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count)
{
    WORD err;
#if CONF_WITH_FDC
    WORD retry;
    WORD status;
#endif

    if(!IS_VALID_FLOPPY_DEVICE(dev)) return EUNDEV;  /* unknown disk */

    rw &= RW_RW;    /* remove any extraneous bits */

    if((rw == RW_WRITE) && (track == 0) && (sect == 1) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }
  
#ifdef MACHINE_AMIGA
    err = amiga_floprw(buf, rw, dev, sect, track, side, count);
    devices[dev].last_access = hz_200;
#elif CONF_WITH_FDC
    floplock(dev);
    
    select(dev, side);
    err = set_track(track);
    if(err) {
        flopunlk();
        return err;
    }

    while (count--) {
      for(retry = 0; retry < 2 ; retry ++) {
        set_fdc_reg(FDC_SR, sect);
        set_dma_addr((ULONG) buf);
        if(rw == RW_READ) {
            fdc_start_dma_read(1);
            set_fdc_reg(FDC_CS, FDC_READ);
        } else {
            fdc_start_dma_write(1);
            set_fdc_reg(FDC_CS | DMA_WRBIT, FDC_WRITE);
        }
        if(timeout_gpip(TIMEOUT)) {
            /* timeout */
            err = EDRVNR;  /* drive not ready */
            flopunlk();
            return err;
        }
        status = get_dma_status();
        if(! (status & DMA_OK)) {
            /* DMA error, retry */
            err = EGENRL;  /* general error */
        } else {
            status = get_fdc_reg(FDC_CS);
            if((rw == RW_WRITE) && (status & FDC_WRI_PRO)) {
                err = EWRPRO;  /* write protect */
                /* no need to retry */
                break;
            } else if(status & FDC_RNF) {
                err = ESECNF;  /* sector not found */
            } else if(status & FDC_CRCERR) {
                err = E_CRC;   /* CRC error */
            } else if(status & FDC_LOSTDAT) {
                err = EDRVNR;  /* drive not ready */
            } else {
                err = 0;
                break;
            }
        }
      }
      //-- If there was an error, dont read any more sectors
      if (err) {
          break;
      }
      //-- Otherwise carry on sequentially
      buf += SECTOR_SIZE;
      sect++;
    }

    flopunlk();
#else
    err = EUNDEV;
#endif
    return err;
}

/*==== internal flopwtrack =================================================*/

static WORD flopwtrack(LONG buf, WORD dev, WORD track, WORD side, WORD track_size)
{
#if CONF_WITH_FDC
    WORD retry;
    WORD err;
    WORD status;
    
    if((track == 0) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }
  
    floplock(dev);
  
    select(dev, side);
    err = set_track(track);
    if(err) {
        flopunlk();
        return err;
    }

    for(retry = 0; retry < 2 ; retry ++) {
        set_dma_addr((ULONG) buf);
        fdc_start_dma_write((track_size + SECTOR_SIZE-1) / SECTOR_SIZE);
        set_fdc_reg(FDC_CS | DMA_WRBIT, FDC_WRITETR);
  
        if(timeout_gpip(TIMEOUT)) {
            /* timeout */
            err = EDRVNR;  /* drive not ready */
            flopunlk();
            return err;
        }
        status = get_dma_status();
        if(! (status & DMA_OK)) {
            /* DMA error, retry */
            err = EGENRL;  /* general error */
        } else {
            status = get_fdc_reg(FDC_CS);
            if(status & FDC_WRI_PRO) {
                err = EWRPRO;  /* write protect */
                /* no need to retry */
                break;
            } else if(status & FDC_LOSTDAT) {
                err = EDRVNR;  /* drive not ready */ 
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
        DMA->density = f->cur_density;
        f->actual_rate = (f->cur_density == DENSITY_HD) ? hd_steprate[f->rate] : f->rate;
    }
}

static void flopunlk(void)
{
    devices[cur_dev].last_access = hz_200;

    /* the VBL will deselect the drive when the motor is off */
    flock = 0;
}

void flopvbl(void)
{
    WORD status;
    BYTE b;
    WORD old_sr;

    /* don't do anything if the DMA circuitry is being used */
    if(flock) return;
    /* only do something every 8th VBL */
    if(frclock & 7) return;
   
    /* TODO - read the write-protect bit in the status register for
     * both drives
     */
 
    /* if no drives are selected, no need to deselect them */
    if(deselected) return;
    /* read FDC status register */
    status = get_fdc_reg(FDC_CS);
        
    /* if the bit motor on is not set, deselect both drives */
    if((status & FDC_MOTORON) == 0) {
        old_sr = set_sr(0x2700);
        
        PSG->control = PSG_PORT_A;
        b = PSG->control;
        b |= 6;
        PSG->data = b;

        deselected = 1;
        set_sr(old_sr);
    }
    
}

/*==== low level register access ==========================================*/


static void select(WORD dev, WORD side)
{
    WORD old_sr;
    BYTE a;
  
    old_sr = set_sr(0x2700);
    PSG->control = PSG_PORT_A;
    a = PSG->control;
    a &= 0xf8;
    if(dev == 0) {
        a |= 4;
    } else {
        a |= 2;
    }
    if(side == 0) {
        a |= 1;
    }  
    PSG->data = a;
    deselected = 0;
    set_sr(old_sr);
}

static WORD set_track(WORD track)
{
    if(track == finfo[cur_dev].cur_track) return 0;
  
    if(track == 0) {
        set_fdc_reg(FDC_CS, FDC_RESTORE | finfo[cur_dev].actual_rate);
    } else {
        set_fdc_reg(FDC_DR, track);
        set_fdc_reg(FDC_CS, FDC_SEEK | finfo[cur_dev].actual_rate);
    }
    if(timeout_gpip(TIMEOUT)) {
        /* cur_track is certainly wrong now */
        /* FIXME it shoud be reset using a Restore command */
        return E_SEEK;  /* seek error */
    } else {
        finfo[cur_dev].cur_track = track;
        return 0;
    }
}

static WORD get_dma_status(void)
{
    WORD ret;
    DMA->control = 0x90;
    ret = DMA->control;
    return ret;
}

static WORD get_fdc_reg(WORD reg)
{
    WORD ret;
    DMA->control = reg;
    delay();
    ret = DMA->data;
    delay();
    return ret;
}

static void set_fdc_reg(WORD reg, WORD value)
{
    DMA->control = reg;
    delay();
    DMA->data = value;
    delay();
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

#endif /* CONF_WITH_FDC */
