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

#define DBG_FLOP 1

#include "portab.h"
#include "floppy.h"
#include "dma.h"
#include "fdc.h"
#include "psg.h"
#include "mfp.h"
#include "asm.h"
#include "tosvars.h"
 
#include "kprint.h"

/*==== External declarations ==============================================*/

extern LONG random(VOID);    /* in xbios.c */


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
 * - boot-sector: getbpb, protobt 
 * - boot-sector utilities: compute_cksum, intel format words
 * - bios rwabs
 * - bios mediach
 * - xbios floprd, flopwr, flopver
 * - xbios flopfmt
 * - internal floprw
 * - internal status, flopvbl
 * - low level dma and fdc registers access
 *
 */
 
/*==== Internal declarations ==============================================*/
 
struct bs {
  /*   0 */  UBYTE bra[2];
  /*   2 */  UBYTE loader[6];
  /*   8 */  UBYTE serial[3];
  /*   b */  UBYTE bps[2];    /* bytes per sector */
  /*   d */  UBYTE spc;       /* sectors per cluster */
  /*   e */  UBYTE res[2];    /* number of reserved sectors */
  /*  10 */  UBYTE fat;       /* number of FATs */
  /*  11 */  UBYTE dir[2];    /* number of DIR root entries */
  /*  13 */  UBYTE sec[2];    /* total number of sectors */
  /*  15 */  UBYTE media;     /* media descriptor */
  /*  16 */  UBYTE spf[2];    /* sectors per FAT */
  /*  18 */  UBYTE spt[2];    /* sectors per track */
  /*  1a */  UBYTE sides[2];  /* number of sides */
  /*  1c */  UBYTE hid[2];    /* number of hidden sectors */
  /*  1e */  UBYTE data[0x1e0];
  /* 1fe */  UBYTE cksum[2];
};

/*==== Internal prototypes ==============================================*/

/* set/get intel words */
static VOID setiword(UBYTE *addr, UWORD value);
static UWORD getiword(UBYTE *addr);

/* floppy read/write */
static WORD floprw(LONG buf, WORD rw, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 

/* initialise a floppy for hdv_init */
static void flopini(WORD dev);

/* called at start and end of a floppy access. */
static void floplock(WORD dev);
static void flopunlk(void);

/* select in the PSG port A*/
static void select(WORD dev, WORD side);

/* sets the track, returns 0 or error. */
static WORD set_track(WORD track);

/* returns 1 if the timeout elapsed before the gpip changed */
#define TIMEOUT 0x50000   /* arbitrary value */
static WORD timeout_gpip(LONG delay);

/* access to dma and fdc registers */
static WORD get_dma_status(void);
static WORD get_fdc_reg(WORD reg);
static void set_fdc_reg(WORD reg, WORD value);
static void set_dma_addr(ULONG addr);
static void fdc_start_dma_read(WORD count);
static void fdc_start_dma_write(WORD count);

/* delay for fdc register access */
static void delay(void);

/* memcmp */
static WORD memcmp(void* a, void* b, LONG n);

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
 * the finfo structure contains the geometry as read from the 
 * bootsector at last get_bpb. finfo[].sides == -1 if the geometry
 * is invalid.
 *
 * last_access is set to the value of the 200 Hz counter at the end of
 * last fdc command. When time elapsed since last_access is greater than
 * .5 seconds, and if no command is running, the flopvbl interrupt 
 * routine will deselect the drive (which will turn the motor off).
 * last_access is also used by mediach, a short time elapsed indicating 
 * that the floppy was not ejected.
 * 
 * the flock variable in tosvars.s is used as following :
 * - floppy.c will set it before accessing to the DMA/FDC, and
 *   clear it at the end.
 * - flopvbl will do nothing when flock is set.
 */

static WORD cur_dev;
static WORD cur_track;
static struct flop_info {
  WORD cur_track;
  WORD spt;          /* number of sectors per track */
  WORD sides;        /* number of sides, or -1 if geometry not inited */
  BYTE serial[3];    /* the serial number taken from the bootsector */
  BYTE wp;           /* TODO, write protected */
} finfo[2];
static LONG last_access;

extern WORD flock;

static struct bpb flop_bpb[2];

/*==== floppy_init (called by startup) ====================================*/

void floppy_init(void)
{
  hdv_boot = flop_hdv_boot;
  hdv_init = flop_hdv_init;
  hdv_bpb =  flop_getbpb;
  hdv_rw = flop_rwabs;
  hdv_mediach = flop_mediach;
  fverify = 0xff;
  seekrate = 3;
  dskbufp = (LONG) diskbuf;
}

/*==== hdv_init and hdv_boot ==============================================*/

VOID flop_hdv_init(VOID)
{
  nflops = 0;
  cur_dev = -1;
  finfo[0].cur_track = -1;
  finfo[0].sides = -1;
  finfo[1].cur_track = -1;
  finfo[0].sides = -1;
  flopini(0);
  flopini(1);  
}

static void flopini(WORD dev)
{
  WORD status;
  
  floplock(dev);
  cur_track = -1;
  select(dev, 0);
  set_fdc_reg(FDC_CS, FDC_RESTORE);
  if(timeout_gpip(TIMEOUT)) {
    /* timeout */
    flopunlk();
    return;
  }
  status = get_fdc_reg(FDC_CS);
  kprintf("status = 0x%02x\n", status);
  if(status & FDC_TRACK0) {
    /* got track0 signal, this means that a drive is connected */
    kprintf("track0 signal got\n" );
    cur_track = 0;
    nflops++;
    drvbits |= (1<<dev);
  } else {
  }
  flopunlk();
}


VOID do_hdv_boot(VOID)
{
  LONG err;

  /* call hdv_boot using the pointer */
  err = hdv_boot();
  kprintf("hdv_boot returns %ld\n", err);
  if(err == 0) {
    /* if bootable, jump in it */
    ((VOID (*)(VOID))dskbufp)();
  }
}
  

LONG flop_hdv_boot(VOID)
{
  struct bs *b = (struct bs *) dskbufp;
  WORD err;
  WORD cksum; 
  
  /* call hdv_init using the pointer */
  hdv_init();
  
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
 

/*==== boot-sector: getbpb, protobt =======================================*/

LONG flop_getbpb(WORD dev)
{
  struct bs *b;
  LONG tmp;
  WORD err;
  
  if(dev < 0 || dev > 1) return 0;
  
  /* read bootsector */
  err = floprw((LONG) dskbufp, RW_READ, dev, 1, 0, 0, 1);
  if(err) {
    /* TODO */ 
    finfo[dev].sides = -1;
    return 0;
  }
  {
    WORD i;
    UBYTE *bb = (UBYTE *)dskbufp;
    kprintf("bootsector: \n");
    for(i = 0 ; i < 128 ; i++) {
      kprintf("%02x ", bb[i]);
      if((i & 15) == 15) kprintf("\n");
    }
  }
  
  b = (struct bs *)dskbufp;
  flop_bpb[dev].recsiz = getiword(b->bps);
  flop_bpb[dev].clsiz = b->spc;
  flop_bpb[dev].clsizb = flop_bpb[dev].clsiz * flop_bpb[dev].recsiz;
  tmp = getiword(b->dir);
  flop_bpb[dev].rdlen = (tmp * 32) / flop_bpb[dev].recsiz;
  flop_bpb[dev].fsiz = getiword(b->spf);
  /* TODO: formulas below are FALSE if reserved sectors != 0 */
  flop_bpb[dev].fatrec = 1 + flop_bpb[dev].fsiz; 
  flop_bpb[dev].datrec = 1 + 2*flop_bpb[dev].fsiz + flop_bpb[dev].rdlen;
  flop_bpb[dev].numcl = (getiword(b->sec) - flop_bpb[dev].datrec) / b->spc;
  flop_bpb[dev].b_flags = 0;   /* assume floppies are always in FAT12 */
  
  /* additional geometry info */
  finfo[dev].sides = getiword(b->sides);
  finfo[dev].spt = getiword(b->spt);
  finfo[dev].serial[0] = b->serial[0];
  finfo[dev].serial[1] = b->serial[1];
  finfo[dev].serial[2] = b->serial[2];
  
  return (LONG) &flop_bpb[dev];
}

VOID protobt(LONG buf, LONG serial, WORD type, WORD exec)
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
      serial = random();
    }
    /* set this serial */
    b->serial[0] = serial>>16;
    b->serial[1] = serial>>8;
    b->serial[2] = serial;
  }
    
  switch(type) {   /* this is ugly */
  case 0:
    setiword(b->bps, 512);
    b->spc = 1;
    setiword(b->res, 1);
    b->fat = 2;
    setiword(b->dir, 64);
    setiword(b->sec, 360);
    b->media = 252;
    setiword(b->spf, 2);
    setiword(b->spt, 9);
    setiword(b->sides, 1);
    setiword(b->hid, 0); 
    break;
  case 1:
    setiword(b->bps, 512);
    b->spc = 2;
    setiword(b->res, 1);
    b->fat = 2;
    setiword(b->dir, 112);
    setiword(b->sec, 720);
    b->media = 253;
    setiword(b->spf, 2);
    setiword(b->spt, 9);
    setiword(b->sides, 2);
    setiword(b->hid, 0); 
    break;
  case 2:
    setiword(b->bps, 512);
    b->spc = 2;
    setiword(b->res, 1);
    b->fat = 2;
    setiword(b->dir, 112);
    setiword(b->sec, 720);
    b->media = 248;
    setiword(b->spf, 5);
    setiword(b->spt, 9);
    setiword(b->sides, 1);
    setiword(b->hid, 0); 
    break;
  case 3:
    setiword(b->bps, 512);
    b->spc = 2;
    setiword(b->res, 1);
    b->fat = 2;
    setiword(b->dir, 112);
    setiword(b->sec, 1440);
    b->media = 249;
    setiword(b->spf, 5);
    setiword(b->spt, 9);
    setiword(b->sides, 2);
    setiword(b->hid, 0); 
    break;
  default:
    /* do not alter type */
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
  }
}


/*==== boot-sector utilities ==============================================*/

/* compute_cksum is also used for booting DMA, hence not static. */
UWORD compute_cksum(LONG buf)
{
  UWORD sum = 0;
  UWORD tmp;
  UBYTE *b = (UBYTE *)buf;
  WORD i;
  for(i = 0 ; i < 256 ; i++) {
    tmp = *b++ << 8;
    tmp += *b++;
    sum += tmp;
  }
  return sum;
}

static void setiword(UBYTE *addr, UWORD value)
{
  addr[0] = value;
  addr[1] = value >> 8;
}

static UWORD getiword(UBYTE *addr)
{
  UWORD value;
  kprintf("getiword addr = %lx: ", (LONG) addr - dskbufp);
  value = (((UWORD)addr[1])<<8) + addr[0]; 
  kprintf("value = %x\n", value);
  return value;
}

/*==== bios rwabs =========================================================*/

LONG flop_rwabs(WORD rw, LONG buf, WORD cnt, WORD recnr, WORD dev)
{
  WORD track;
  WORD side;
  WORD sect;
  WORD err;
  
  if(dev < 0 || dev > 1) return -15;
    
  if(finfo[dev].sides == -1) {
    /* no geometry. Should we call get_bpb ??? TODO */
    return -1;
  }
  
  /* TODO mediach ignored */
  rw &= RW_RW_MASK;
  
  while( --cnt >= 0) {
    sect = (recnr % finfo[dev].spt) + 1;
    track = recnr / finfo[dev].spt;
    if(finfo[dev].sides == 1) {
      side = 0;
    } else {
      side = track % 2;
      track /= 2;
    }
    err = floprw(buf, rw, dev, sect, track, side, 1);
    buf += 512;
    recnr ++;
    if(err) return (LONG) err;
  }
  return 0;
}

/*==== bios mediach =======================================================*/


LONG flop_mediach(WORD dev)
{
  if(dev < 0 || dev > 1) return -15;
  /* TODO */
  return 1; /* unsure */ 
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


WORD flopver(LONG buf, LONG filler, WORD dev, 
             WORD sect, WORD track, WORD side, WORD count)
{
  WORD i;
  WORD err;
  WORD outerr = 0;
  WORD *out = (WORD *) buf;
  
  if(count <= 0) return 0;
  if(dev < 0 || dev > 1) return -15;
  for(i = 0 ; i < count ; i++) {
    err = floprw((LONG) dskbufp, RW_READ, dev, sect, track, side, 1);
    if(err) {
      *out++ = sect;
      outerr = err;
      continue;
    }
    if(memcmp((void *)buf, (void *)dskbufp, (long) flop_bpb[dev].recsiz)) {
      *out++ = sect;
      outerr = -16;
    }
    sect ++;
  }
    
  if(outerr) {
    *out = 0;
  }
  return outerr;
}


/*==== xbios flopfmt ======================================================*/


WORD flopfmt(LONG buf, LONG filler, WORD dev, WORD spt,
             WORD track, WORD side, WORD interleave, 
             ULONG magic, WORD virgin)
{
  if(magic != 0x87654321UL) return 0;
  if(dev < 0 || dev > 1) return -15;
  
  /* TODO */
  
  return -15;
}


/*==== internal floprw ====================================================*/

WORD floprw(LONG buf, WORD rw, WORD dev, 
            WORD sect, WORD track, WORD side, WORD count)
{
  WORD retry;
  WORD err;
  WORD status;
  
  if(dev < 0 || dev > 1) return -15;
  
  if((rw == RW_WRITE) && (track == 0) && (sect == 1) && (side == 0)) {
    /* maybe media changed ? */
  }
  
  floplock(dev);
  
  select(dev, side);
  err = set_track(track);
  if(err) {
    flopunlk();
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
      err = -2;
      flopunlk();
      return err;
    }
    status = get_dma_status();
    if(! (status & DMA_OK)) {
      /* DMA error, retry */
    } else {
      status = get_fdc_reg(FDC_CS);
      if((rw == RW_WRITE) && (status & FDC_WRI_PRO)) {
        err = -13;
        /* no need to retry */
        break;
      } else if(status & FDC_RNF) {
        err = -8;
      } else if(status & FDC_CRCERR) {
        err = -4;
      } else if(status & FDC_LOSTDAT) {
        err = -2;
      } else {
        err = 0;
        break;
      }
    }
  }  
  flopunlk();
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
  } 
}

static void flopunlk(void)
{
  last_access = hz_200;
  flock = 0;
}

void flopvbl(void)
{
  if(flock) return;
  /* TODO, the TOS flopvbl does:
     - only every 8th interrupt
     - deselects drives only when motor is not on
     - checks if floppies are write protected
  */
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
  set_sr(old_sr);
}

static WORD set_track(WORD track)
{
  if(track == cur_track) return 0;
  
  if(track == 0) {
    set_fdc_reg(FDC_CS, FDC_RESTORE);
  } else {
    set_fdc_reg(FDC_DR, track);
    set_fdc_reg(FDC_CS, FDC_SEEK);
  }
  if(timeout_gpip(TIMEOUT)) {
    cur_track = -1;
    return -1;
  } else {
    cur_track = track;
    return 0;
  }
}

/* returns 1 if the timeout elapsed before the event occurred */
static WORD timeout_gpip(LONG timeout)
{
  MFP *mfp = MFP_BASE;
  while(--timeout >= 0) {
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
  DMA->addr_high = addr>>16;
  DMA->addr_med = addr>>8;
  DMA->addr_low = addr;
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

static void delay(void)
{
  WORD delay = 30;
  while(--delay);
}

static WORD memcmp(void* aa, void* bb, LONG n)
{
  UBYTE * a = aa;
  UBYTE * b = bb;
  while((--n >= 0) && (*a++ == *b++));
  if(n < 0) return 0;
  --a;
  --b;
  if(*a < *b) return -1;
  return 1;
}


