/*
 * blkdev.c - BIOS block device functions
 *
 * Copyright (c) 2002 by Authors:
 *
 *  MAD     Martin Doering
 *  joy     Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#define DBG_BLKDEV 0

#include "portab.h"
#include "bios.h"
#include "gemerror.h"
#include "kprint.h"
#include "tosvars.h"
#include "floppy.h"
#include "disk.h"
#include "blkdev.h"

/*
 * Global variables
 */

BLKDEV blkdev[BLKDEVNUM];     /* enough for now? */
UNIT devices[UNITSNUM];

static BYTE diskbuf[2*512];      /* buffer for 2 sectors */


/*
 * blkdevs_init - BIOS block drive initialization
 *
 * Called from bios.c, this routine will do necessary block device
 * initialization.
 */

void blkdev_init(void)
{
    /* set the block buffer pointer to reserved memory */
    dskbufp = &diskbuf;
#if DBG_BLKDEV
    kprintf("diskbuf = %08lx\n", dskbufp);
#endif

    /* setup booting related vectors */
    hdv_boot    = blkdev_hdv_boot;
    hdv_init    = 0; // blkdev_hdv_init;

    /* setup general block device vectors */
    hdv_bpb     = blkdev_getbpb;
    hdv_rw      = blkdev_rwabs;
    hdv_mediach = blkdev_mediach;

    /* floppy initialisation */
    floppy_init();

    /* harddisk initialisation */
    // disk_init();

    /* setting drvbits */
    blkdev_hdv_init();
}



/*
 * blkdev_hdv_init
 *
 */

void blkdev_hdv_init(void)
{
    drvbits = 0;

    /* call the real */
    flop_hdv_init();

    /* hacked init values for my drive */
    blkdev[2].valid = 1;
    blkdev[2].start = 4;
    blkdev[2].size = 65519;
    blkdev[2].unit = 22;
    devices[blkdev[2].unit].valid = 1;
    devices[blkdev[2].unit].pssize = 512;
    devices[blkdev[2].unit].size = 100000;
    drvbits |= (1<<2);
}


/*
 * blkdev_hdv_boot - BIOS boot vector
 */

LONG blkdev_hdv_boot(void)
{
    return(flop_hdv_boot());
}


/*
 * blkdev_rwabs - BIOS block device read/write vector
 */

#define CNTMAX  0x7FFF  /* 16-bit MAXINT */

LONG blkdev_rwabs(WORD rw, LONG buf, WORD cnt, WORD recnr, WORD dev, LONG lrecnr)
{
    int retries = RWABS_RETRIES;
    int unit = dev;
    LONG lcount = cnt;
    LONG retval;

#if DBG_BLKDEV
    kprintf("rwabs(rw=%d, buf=%ld, count=%ld, recnr=%u, dev=%d, lrecnr=%ld)\n", rw, buf, lcount, recnr, dev, lrecnr);
#endif

    if (recnr != -1)            /* if long offset not used */
        lrecnr = (UWORD)recnr;  /* recnr as unsigned to enable 16-bit recn */

    if (! (rw & RW_NOTRANSLATE)) {
        int sectors;
        if ((dev < 0 ) || (dev >= BLKDEVNUM) || !blkdev[dev].valid)
            return EUNDEV;  /* unknown device */

        /* convert logical sectors to physical ones */
        sectors = blkdev[dev].bpb.recsiz / devices[blkdev[dev].unit].pssize;
        lcount *= sectors;
        lrecnr *= sectors;

        /* check if the count does fit to this partition */
#if 0   /* requires the size of partition which is unknown for a floppy */
        if ((lrecnr < 0) || ((lrecnr + lcount) >= blkdev[dev].size))
            return ESECNF;  /* sector not found */
#endif

        /* convert partition offset to absolute offset on a unit */
        lrecnr += blkdev[dev].start;

        /* convert logical drive to physical unit */
        unit = blkdev[dev].unit;

#if DBG_BLKDEV
        kprintf("rwabs translated: sector=%ld, count=%ld\n", lrecnr, lcount);
#endif
    }
    else {
        if ((unit < 0) || unit >= UNITSNUM || !devices[unit].valid)
            return EUNDEV;  /* unknown device */

        /* check if the start & count values are valid for this device */
#if 0   /* requires the size of disk device which is unknown for a floppy */
        if ((lrecnr < 0) || ((lrecnr + lcount) >= devices[unit].size))   
            return ESECNF;  /* sector not found */
#endif
    }

    if (rw & RW_NORETRIES)
        retries = 1;

    if (! (rw & RW_NOMEDIACH)) {
        /* check physMediach() for this drive */
    }

    rw &= RW_RW;

    do {
        /* split the transfer to 15-bit count blocks (lowlevel functions take WORD count) */
        int scount = (lcount > CNTMAX) ? CNTMAX : lcount;
        lcount -= CNTMAX;
        do {
            if (unit < 2) {
                retval = floppy_rw(rw, buf, scount, lrecnr,
                                   blkdev[unit].geometry.spt,
                                   blkdev[unit].geometry.sides, unit);
            }
            else {
                retval = rw ? DMAwrite(lrecnr, scount, buf, unit-2)
                            : DMAread(lrecnr, scount, buf, unit-2);
            }
        } while((retval < 0) && (--retries > 0));
    } while(lcount > 0);

    return retval;
}


/*
 * blkdev_getbpb - Get BIOS parameter block
 *
 * implement the Mediach flush as documented in Compendium
 */

LONG blkdev_getbpb(WORD dev)
{
    struct bs *b;
    LONG tmp;
    WORD err;
  
#if DBG_BLKDEV
    kprintf("getbpb(%d)\n", dev);
#endif

    if ((dev < 0 ) || (dev >= BLKDEVNUM) || !blkdev[dev].valid)
        return 0;  /* unknown device */

    /* read bootsector using the physical mode */
    err = blkdev_rwabs(RW_READ | RW_NOTRANSLATE, (LONG) dskbufp, 1, -1,
                       blkdev[dev].unit, blkdev[dev].start);
    if (err) return 0;

    b = (struct bs *)dskbufp;
#if DBG_BLKDEV
    kprintf("bootsector[dev = %d] = {\n  ...\n", dev);
    kprintf("  res = %d;\n", getiword(b->res));
    kprintf("  hid = %d;\n", getiword(b->hid));
    kprintf("}\n");
#endif
  
    /* TODO
     * check if the parameters are sane and set reasonable defaults if not
     */
  
    blkdev[dev].bpb.recsiz = getiword(b->bps);
    blkdev[dev].bpb.clsiz = b->spc;
    blkdev[dev].bpb.clsizb = blkdev[dev].bpb.clsiz * blkdev[dev].bpb.recsiz;
    tmp = getiword(b->dir);
    blkdev[dev].bpb.rdlen = (tmp * 32) / blkdev[dev].bpb.recsiz;
    blkdev[dev].bpb.fsiz = getiword(b->spf);

    /* the structure of the logical disk is assumed to be:
     * - bootsector
     * - fats
     * - dir
     * - data clusters
     * TODO: understand what to do with reserved or hidden sectors.
     */

    blkdev[dev].bpb.fatrec = 1 + blkdev[dev].bpb.fsiz; 
    blkdev[dev].bpb.datrec = blkdev[dev].bpb.fatrec + blkdev[dev].bpb.fsiz 
                           + blkdev[dev].bpb.rdlen;
    blkdev[dev].bpb.numcl = (getiword(b->sec) - blkdev[dev].bpb.datrec) / b->spc;
    blkdev[dev].bpb.b_flags = (blkdev[dev].bpb.numcl > 0xff7) ? B_16 : 0;

    /* additional geometry info */
    blkdev[dev].geometry.sides = getiword(b->sides);
    blkdev[dev].geometry.spt = getiword(b->spt);
    blkdev[dev].serial[0] = b->serial[0];
    blkdev[dev].serial[1] = b->serial[1];
    blkdev[dev].serial[2] = b->serial[2];
  
#if DBG_BLKDEV
    kprintf("bpb[dev = %d] = {\n", dev);
    kprintf("  recsiz = %d;\n", blkdev[dev].bpb.recsiz);
    kprintf("  clsiz  = %d;\n", blkdev[dev].bpb.clsiz);
    kprintf("  clsizb = %d;\n", blkdev[dev].bpb.clsizb);
    kprintf("  rdlen  = %d;\n", blkdev[dev].bpb.rdlen);
    kprintf("  fsiz   = %d;\n", blkdev[dev].bpb.fsiz);
    kprintf("  fatrec = %d;\n", blkdev[dev].bpb.fatrec);
    kprintf("  datrec = %d;\n", blkdev[dev].bpb.datrec);
    kprintf("  numcl  = %d;\n", blkdev[dev].bpb.numcl);
    kprintf("  bflags = %d;\n", blkdev[dev].bpb.b_flags);
    kprintf("}\n");
#endif

    return (LONG) &blkdev[dev].bpb;
}


/*
 * blkdev_mediach - dummy BIOS media change vector
 */

LONG blkdev_mediach(WORD dev)
{
    int unit;
    if ((dev < 0 ) || (dev >= BLKDEVNUM) || !blkdev[dev].valid)
        return EUNDEV;  /* unknown device */

    unit = blkdev[dev].unit;
    if (hz_200 < devices[unit].last_access + 100) {
        /* less than half a second since last access, assume no mediachange */
        return 0; /* definitely not changed */
    }
    else {
        WORD err;
        struct bs *bs = (struct bs *) dskbufp;
        /* TODO, monitor write-protect status in flopvbl... */
    
        /* for now, assume it is unsure and look at the serial number */
        /* read bootsector using the physical mode */
        err = blkdev_rwabs(RW_READ | RW_NOTRANSLATE, (LONG) dskbufp, 1, -1,
                           unit, blkdev[dev].start);
        if (err) {
            /* can't even read the bootsector, what do we do ??? */
            return err;
        }
        if ( (bs->serial[0] != blkdev[dev].serial[0])
            || (bs->serial[1] != blkdev[dev].serial[1])
            || (bs->serial[2] != blkdev[dev].serial[2]) ) {
            return 2; /* definitely changed */
        }
        else {
            return 1; /* unsure */
        }
    }
}

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
