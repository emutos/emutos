/*
 * floppy.c - floppy routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_FLOP 0

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


/*==== Introduction =======================================================*/

/*
 * This file contains all floppy-related bios and xbios routines.
 * They are stacked by sections, function of a higher level calling 
 * functions of a lower level.
 *
 * sections in this file:
 * - private prototypes
 * - internal floppy status info
 * - floppy_init
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
 * - high density media not considered in flopfmt or protobt
 * - delay() should be based on some delay loop callibration
 * - reserved or hidden sectors are not guaranteed to be handled correctly
 * - ... (search for 'TODO' in the rest of this file)
 * - the unique FDC track register is probably not handled correctly
 *   when using two drives
 * - once mediach reported != 0, it should not report zero until a new
 *   getbpb is called.
 */


/*==== Internal defines ===================================================*/
 
#define SECT_SIZ 512
#define TRACK_SIZ 6250

/*==== Internal prototypes ==============================================*/

/* set/get intel words */
static void setiword(UBYTE *addr, UWORD value);

static LONG flop_bootcheck(void);

/* floppy read/write */
static WORD floprw(LONG buf, WORD rw, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 

/* floppy write track */
static WORD flopwtrack(LONG buf, WORD dev, WORD track, WORD side);

/* initialise a floppy for hdv_init */
static void flopini(WORD dev);

/* called at start and end of a floppy access. */
static void floplock(WORD dev);
static void flopunlk(WORD dev);

/* select drive and side in the PSG port A */
static void select(WORD dev, WORD side);

/* sets the track, returns 0 or error. rate is the step rate */
static WORD set_track(WORD track, WORD rate);

/* returns 1 if the timeout elapsed before the gpip changed */
static WORD timeout_gpip(LONG delay);  /* delay in milliseconds */
#define TIMEOUT 1500L   /* default one second and a half */

/* access to dma and fdc registers */
static WORD get_dma_status(void);
static WORD get_fdc_reg(WORD reg);
static void set_fdc_reg(WORD reg, WORD value);
static void set_dma_addr(ULONG addr);
static void fdc_start_dma_read(WORD count);
static void fdc_start_dma_write(WORD count);

/* delay for fdc register access */
static void delay(void);

/*==== Internal floppy status =============================================*/

/* cur_dev is the current drive, or -1 if none is current.
 * the fdc track register will reflect the position of the head of the
 * current drive. 'current' does not mean 'active', because the flopvbl
 * routine may deselect the drive in the PSG port A. The routine 
 * floplock(dev) will set the new current drive.
 *
 * cur_track contains a copy of the fdc track register for the current
 * drive, or -1 to indicate that the drive does not exist.
 *
 * last_access is set to the value of the 200 Hz counter at the end of
 * last fdc command. last_access can be used by mediach, a short time 
 * elapsed indicating that the floppy was not ejected.
 * 
 * finfo[].wp is set according to the corresponding bit in the fdc 
 * controller status reg. As soon as this value is different for
 * the drive, this means that the floppy has changed.
 *
 * finfo[].rate is the seek rate (TODO: unused)
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
static WORD cur_track;
static struct flop_info {
  WORD cur_track;
  WORD rate;
  BYTE wp;           /* != 0 means write protected */
} finfo[2];

static UBYTE deselected;

/*==== hdv_init and hdv_boot ==============================================*/

void flop_hdv_init(void)
{
    /* set floppy specific stuff */
    fverify = 0xff;
    seekrate = 3;

    nflops = 0;
    cur_dev = -1;

    /* I'm unsure, so let flopvbl() do the work of figuring out. */
    deselected = 1;

    finfo[0].cur_track = -1;
    finfo[0].rate = seekrate;
    finfo[1].cur_track = -1;
    finfo[1].rate = seekrate;
    flopini(0);
    flopini(1);
}

void floppy_init(void)
{
    /* set floppy specific stuff from floppy init */
    fverify = 0xff;
    seekrate = 3;

    /* I'm unsure, so let flopvbl() do the work or figuring out. */
    deselected = 1;
    //dskbufp = &diskbuf;
}



static void flopini(WORD dev)
{
    WORD status;
  
    blkdev[dev].valid = devices[dev].valid = 0;    /* disabled by default */

    floplock(dev);
    cur_track = -1;
    select(dev, 0);
    set_fdc_reg(FDC_CS, FDC_RESTORE);
    if(timeout_gpip(TIMEOUT)) {
        /* timeout */
        flopunlk(dev);
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
        cur_track = 0;
        nflops++;
        drvbits |= (1<<dev);

        /* init blkdev and device with default parameters */
        blkdev[dev].valid = 1;
        blkdev[dev].start = 0;
        blkdev[dev].size = 0;           /* unknown size */
        blkdev[dev].geometry.sides = 2; /* default geometry of 3.5" DD */
        blkdev[dev].geometry.spt = 9;
        blkdev[dev].unit = dev;
        devices[dev].valid = 1;
        devices[dev].pssize = 512;
        devices[dev].size = 0;          /* unknown size */
        devices[dev].last_access = 0;   /* never accessed */
    } 
    flopunlk(dev);
}


LONG flop_hdv_boot(void)
{
    LONG err;

    /* call hdv_boot using the pointer */
    err = flop_bootcheck();
    kprintf("hdv_boot returns %ld\n", err);
    if(err == 0) {
        /* if bootable, jump in it */
        regsafe_call(dskbufp);
    }
    return err;         /* may never be reached, if booting */
}
  

LONG flop_bootcheck(void)
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
    cksum = compute_cksum((LONG) b);
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

#if DBG_FLOP
    kprintf("floppy_rw(rw %d, buf 0x%lx, cnt %d, ", rw, buf, cnt);
    kprintf("recnr %ld, spt %d, sides %d, dev %d)\n", recnr, spt, sides, dev);
#endif

    if (dev < 0 || dev > 1) return EUNDEV;  /* unknown device */

    /* do the transfer one sector at a time. It is easier to implement,
     * but perhaps slower when using FastRAM, as the time spent in memcpying
     * the sector in memory may force us to wait for a complete
     * rotation of the floppy before reading the next sector.
     */
    
    while( --cnt >= 0) {
        sect = (recnr % spt) + 1;
        track = recnr / spt;
        if (sides == 1) {
            side = 0;
        } else {
            side = track % 2;
            track /= 2;
        }
        if (buf > 0x1000000L) {
            if (cookie_frb > 0) {
                /* do we really need proper FRB lock? (TODO) */
                if(rw & 1) {
                    /* writing */ 
                    memcpy((void *)cookie_frb, (void *)buf, SECT_SIZ);
                    err = floprw(cookie_frb, rw, dev, sect, track, side, 1);
                } else {
                    /* reading */
                    err = floprw(cookie_frb, rw, dev, sect, track, side, 1);
                    memcpy((void *)buf, (void *)cookie_frb, SECT_SIZ);
                }
                /* proper FRB unlock (TODO) */
            } else {
                err = -1;   /* problem: can't DMA to FastRAM */
            }
        } else {
            err = floprw(buf, rw, dev, sect, track, side, 1);
        }
        buf += SECT_SIZ;
        recnr ++;
        if(err) return (LONG) err;
    }
    return 0;
}

/*==== boot-sector: protobt =======================================*/

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
    { SECT_SIZ, 1, 1, 2,  64,  360, 252, 2, 9, 1, 0 },
    { SECT_SIZ, 2, 1, 2, 112,  720, 253, 2, 9, 2, 0 },
    { SECT_SIZ, 2, 1, 2, 112,  720, 248, 5, 9, 1, 0 },
    { SECT_SIZ, 2, 1, 2, 112, 1440, 249, 5, 9, 2, 0 },
};
  
void protobt(LONG buf, LONG serial, WORD type, WORD exec)
{
    WORD is_exec;
    struct bs *b = (struct bs *)buf;
    UWORD cksum;
  
    is_exec = (compute_cksum(buf) == 0x1234);
  
    if(serial < 0) {
        /* do not modify serial */
    } else {
        if(serial >= 0x1000000) {
            /* create a random serial */
            serial = Random();
        }
        /* set this serial */
        b->serial[0] = serial>>16;
        b->serial[1] = serial>>8;
        b->serial[2] = serial;
    }
    
    if(type >= 0 && type <= 3) {
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
  
    if(exec < 0) {
        /* keep in the same state */
        if(is_exec) {
            exec = 1;   /* executable */
        } else {
            exec = 0;   /* not executable */
        }
    }
    switch(exec) {
    case 0:
        cksum = compute_cksum(buf);
        if(cksum == 0x1234) {
            b->cksum[1]++;
        } 
        break;
    case 1:
        setiword(b->cksum, 0);
        cksum = compute_cksum(buf);
        setiword(b->cksum, 0x1234 - cksum);
        break;
    default:
        /* unknown */
        break;
    }
}


/*==== boot-sector utilities ==============================================*/

static void setiword(UBYTE *addr, UWORD value)
{
    addr[0] = value;
    addr[1] = value >> 8;
}

UWORD getiword(UBYTE *addr)
{
    UWORD value;
    value = (((UWORD)addr[1])<<8) + addr[0]; 
    return value;
}

/*==== xbios floprd, flopwr ===============================================*/


WORD floprd(LONG buf, LONG filler, WORD dev, 
            WORD sect, WORD track, WORD side, WORD count)
{
    return floprw(buf, RW_READ, dev, sect, track, side, count);
}


WORD flopwr(LONG buf, LONG filler, WORD dev, 
            WORD sect, WORD track, WORD side, WORD count)
{
    return floprw(buf, RW_WRITE, dev, sect, track, side, count);
}

/*==== xbios flopver ======================================================*/

/* TODO, in the case where both one sector cannot be read and another is
 * read wrong, what is the error return code? 
 */

WORD flopver(LONG buf, LONG filler, WORD dev, 
             WORD sect, WORD track, WORD side, WORD count)
{
    WORD i;
    WORD err;
    WORD outerr = 0;
    WORD *bad = (WORD *) buf;
    
    if(count <= 0) return 0;
    if(dev < 0 || dev > 1) return EUNDEV;  /* unknown disk */
    for(i = 0 ; i < count ; i++) {
        err = floprw((LONG) dskbufp, RW_READ, dev, sect, track, side, 1);
        if(err) {
            *bad++ = sect;
            outerr = err;
            continue;
        }
        if(memcmp((void *)buf, (void *)dskbufp, (long) SECT_SIZ)) {
            *bad++ = sect;
            outerr = -16;
        }
        sect ++;
    }
    
    if(outerr) {
        *bad = 0;
    }
    return outerr;
}


/*==== xbios flopfmt ======================================================*/

WORD flopfmt(LONG buf, LONG filler, WORD dev, WORD spt,
             WORD track, WORD side, WORD interleave, 
             ULONG magic, WORD virgin)
{
    int i, j;
    BYTE b1, b2;
    BYTE *s;
    BYTE *data;
    WORD *bad;
    WORD err;

// #define APPEND(b, count) do { int n=count; while(n--) *s++ = b; } while(0) 
#define APPEND(b, count) do { memset(s, b, count); s += count; } while(0)

    if(magic != 0x87654321UL) return 0;
    if(dev < 0 || dev > 1) return EUNDEV;  /* unknown disk */
    if(spt < 1 || spt > 10) return EGENRL;  /* general error */
    s = (BYTE *)buf;

    data = s; /* dummy, to avoid warning. data will be set in the loop */
  
    /*
     * sector interleave factor ignored, always 1.
     * create the image in memory. 
     * track  ::= GAP1 record record ... record GAP5
     * record ::= GAP2 index GAP3 data GAP4
     */

    b1 = virgin >> 8;
    b2 = virgin;

    /* GAP1 : 60 bytes 0x4E */  
    APPEND(0x4E, 60);
  
    for(i = 0 ; i < spt ; i++) {
        /* GAP2 */
        APPEND(0x00, 12);
        APPEND(0xF5, 3);

        /* index */
        *s++ = 0xfe;
        *s++ = track;
        *s++ = side;
        *s++ = i+1;
        *s++ = 2; /* means sector of 512 bytes */
        *s++ = 0xf7;

        /* GAP3 */
        APPEND(0x4e, 22);
        APPEND(0x00, 12);
        APPEND(0xF5, 3);
    
        /* data */
        *s++ = 0xfe;
        data = s; /* the content of a sector */
        for(j = 0 ; j < SECT_SIZ ; j += 2) {
            *s++ = b1; *s++ = b2;
        }
        *s++ = 0xf7;

        /* GAP4 */
        APPEND(0x4e, 40);
    }    

    /* GAP5 : all the rest to fill to size 6250 (size of a raw track) */  
    APPEND(0x4E, TRACK_SIZ - 60 - 614 * spt);
  
#undef APPEND

    /* write the buffer to track */
    err = flopwtrack(buf, dev, track, side);
    if(err) return err;

    /* verify sectors and store bad sector numbers in buf */
    bad = (WORD *)buf;
    for(i = 0 ; i < spt ; i++) {
        err = flopver((LONG)data, 0L, dev, i+1, track, side, 1);
        if(err) {
            *bad++ = i+1;
        }
    }
    *bad = 0;
    if(bad != (WORD *)buf) {
        return EBADSF;  /* bad sectors on format */
    }
  
    return 0;
}

/*==== xbios floprate ======================================================*/

/* sets the rate of the specified drive. 
 * rate meaning
 * 0   6ms
 * 1  12ms
 * 2   2ms
 * 3   3ms
 */

WORD floprate(WORD dev, WORD rate)
{
    WORD old;

    if(dev < 0 || dev > 1) return EUNDEV;  /* unknown disk */
    old = finfo[dev].rate;
    if(rate >= 0 && rate <= 3) {
        finfo[dev].rate = rate;
    }
    return old;
}

/*==== internal floprw ====================================================*/

static WORD floprw(LONG buf, WORD rw, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count)
{
    WORD retry;
    WORD err;
    WORD status;
    
    if(dev < 0 || dev > 1) return EUNDEV;  /* unknown disk */
    
    if((rw == RW_WRITE) && (track == 0) && (sect == 1) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }
  
    floplock(dev);
    
    select(dev, side);
    err = set_track(track, finfo[dev].rate);
    if(err) {
        flopunlk(dev);
        return err;
    }
    for(retry = 0; retry < 2 ; retry ++) {
        set_fdc_reg(FDC_SR, sect);
        set_dma_addr((ULONG) buf);
        if(rw == RW_READ) {
            fdc_start_dma_read(count);
            set_fdc_reg(FDC_CS, FDC_READ);
        } else {
            fdc_start_dma_write(count);
            set_fdc_reg(FDC_CS, FDC_WRITE);
        }
        if(timeout_gpip(TIMEOUT)) {
            /* timeout */
            err = EDRVNR;  /* drive not ready */
            flopunlk(dev);
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
    flopunlk(dev);
    return err;
}

/*==== internal flopwtrack =================================================*/

static WORD flopwtrack(LONG buf, WORD dev, WORD track, WORD side)
{
    WORD retry;
    WORD err;
    WORD status;
    
    if(dev < 0 || dev > 1) return EUNDEV;  /* unknown disk */
    
    if((track == 0) && (side == 0)) {
        /* TODO, maybe media changed ? */
    }
  
    floplock(dev);
  
    select(dev, side);
    err = set_track(track, finfo[dev].rate);
    if(err) {
        flopunlk(dev);
        return err;
    }
    for(retry = 0; retry < 2 ; retry ++) {
        set_dma_addr((ULONG) buf);
        fdc_start_dma_write((TRACK_SIZ + SECT_SIZ-1) / SECT_SIZ);
        set_fdc_reg(FDC_CS, FDC_WRITETR);
  
        if(timeout_gpip(TIMEOUT)) {
            /* timeout */
            err = EDRVNR;  /* drive not ready */
            flopunlk(dev);
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
    flopunlk(dev);
    return err;
}

/*==== internal status, flopvbl ===========================================*/


static void floplock(WORD dev)
{
    flock = 1;
    if(dev != cur_dev) {
        /* 
         * the FDC has only one track register for two units.
         * we need to save the current value, and switch 
         */
        if(cur_dev != -1) {
            finfo[cur_dev].cur_track = cur_track;
        }
        cur_dev = dev;
        cur_track = finfo[cur_dev].cur_track;
        /* TODO, what if the new device is not available? */
        set_fdc_reg(FDC_TR, cur_track);
    } 
}

static void flopunlk(WORD dev)
{
    devices[dev].last_access = hz_200;
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

static WORD set_track(WORD track, WORD rate)
{
    if(track == cur_track) return 0;
  
    if(track == 0) {
        set_fdc_reg(FDC_CS, FDC_RESTORE | (rate & 3));
    } else {
        set_fdc_reg(FDC_DR, track);
        set_fdc_reg(FDC_CS, FDC_SEEK | (rate & 3));
    }
    if(timeout_gpip(TIMEOUT)) {
        cur_track = -1;
        return E_SEEK;  /* seek error */
    } else {
        cur_track = track;
        return 0;
    }
}

/* returns 1 if the timeout (milliseconds) elapsed before gpip went low */
static WORD timeout_gpip(LONG delay)
{
    MFP *mfp = MFP_BASE;
    LONG next = hz_200 + delay/5;
    while(hz_200 < next) {
        if((mfp->gpip & 0x20) == 0) {
            return 0;
        }
    }
    return 1;
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

static void set_dma_addr(ULONG addr)
{
    DMA->addr_low = addr;
    DMA->addr_med = addr>>8;
    DMA->addr_high = addr>>16;
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


/* TODO - determine which delay is appropriate, and ensure
 * this delay is obtained regardless of the processor speed.
 */
static void delay(void)
{
    WORD delay = 30;
    while (--delay)
	    asm volatile (" nop ");
}
