/*
 * blkdev.c - BIOS block device functions
 *
 * Copyright (c) 2002-2011 The EmuTOS development team
 * 
 * Authors:
 *  MAD     Martin Doering
 *  joy     Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#define DBG_BLKDEV 0

#include "config.h"
#include "portab.h"
#include "string.h"
#include "gemerror.h"
#include "kprint.h"
#include "tosvars.h"
#include "floppy.h"
#include "disk.h"
#include "ikbd.h"
#include "blkdev.h"

/*
 * Global variables
 */

BLKDEV blkdev[BLKDEVNUM];
int blkdevnum;
UNIT devices[UNITSNUM];

static BYTE diskbuf[2*512];      /* buffer for 2 sectors */

PUN_INFO pun_info;

/* get intel words */
static UWORD getiword(UBYTE *addr)
{
    UWORD value;
    value = (((UWORD)addr[1])<<8) + addr[0]; 
    return value;
}

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
    kprintf("diskbuf = %08lx\n", (long)dskbufp);
#endif

    /* setup booting related vectors */
    hdv_boot    = blkdev_hdv_boot;
    hdv_init    = 0; // blkdev_hdv_init;

    /* setup general block device vectors */
    hdv_bpb     = blkdev_getbpb;
    hdv_rw      = blkdev_rwabs;
    hdv_mediach = blkdev_mediach;

#if CONF_WITH_FLOPPY
    /* floppy initialisation */
    floppy_init();
#endif

    /* setting drvbits */
    blkdev_hdv_init();
}

/* currently the only valid information in the PUN_INFO is the max_sect_siz */
/* which is exactly what FreeMiNT was missing and was complaining about... */
void pun_info_setup(void)
{
    int i;
    BPB *bpb;

    /* set PUN_INFO */
    pun_info.puns = blkdevnum;
    pun_info.max_sect_siz = 512;

    /* floppy A: */
    pun_info.pun[0] = 0;    /* FIXME */
    pun_info.partition_start[0] = 0;
    
    /* floppy B: */
    pun_info.pun[1] = 0;    /* FIXME */
    pun_info.partition_start[1] = 0;
    
    /* disks C: - P: */
    for(i = 2; i < 16; i++) {
        pun_info.pun[i] = 0;    /* FIXME */
        pun_info.partition_start[i] = 0;    /* FIXME */

        bpb = (BPB *)blkdev_getbpb(i);
        if (bpb != NULL && bpb->recsiz > (int)pun_info.max_sect_siz) {
            pun_info.max_sect_siz = bpb->recsiz;
        }
    }

    pun_info.cookie = 0x41484449L; /* AHDI */
    pun_info.cookie_ptr = &pun_info.cookie;
    pun_info.version_num = 0x300;      /* AHDI v3.00 */

    pun_ptr = (LONG)&pun_info;

#if DBG_BLKDEV
    kprintf("PUN INFO: max sector size = %d\n", pun_info.max_sect_siz);
#endif
}

/*
 * blkdev_hdv_init
 *
 */

void blkdev_hdv_init(void)
{
#if CONF_WITH_FLOPPY
    /* Detect and initialize floppy drives */
    flop_hdv_init();
#endif

    blkdevnum = 2; /* Start hard disk partitions at C: */
    disk_init(); /* Detect hard disk partitions */

    pun_info_setup();
}


/*
 * blkdev_hdv_boot - BIOS boot vector
 */

LONG blkdev_hdv_boot(void)
{
#if DBG_BLKDEV
    kprintf("drvbits = %08lx\n", drvbits);
#endif
    /* boot eventually from a block device (floppy or harddisk) */

    /* the actual boot device or better the order of several boot devices
       should be configurable like for example in PC CMOS SETUP:
       e.g. first CD-ROM, then IDE primary, then SCSI and floppy at last.
       As we don't have such configuration yet there's a hardcoded order
       for now: if C: exists use it as the boot device, otherwise boot from A:
    */

    /* boot hard drive only if user does not hold the Alternate key down */
    if ((kbshift(-1) & 0x08) == 0) {
        if (blkdev_avail(2)) {  /* if drive C: is available */
            bootdev = 2;        /* make it the boot drive */
            return 0;           /* don't actually boot from the boot device
                                   as there is most probably a harddisk driver
                                   installed and that would require complete
                                   lowlevel IDE/ACSI/SCSI emulation. Luckily
                                   EmuTOS got its own internal hdd driver,
                                   so we don't need to execute the boot sector
                                   code and boot the drive at all! :-) */
        }
    }

#if CONF_WITH_FLOPPY
    /* otherwise try to boot from floppy A: */
    bootdev = 0;
    return(flop_hdv_boot());
#else
    return EUNDEV;
#endif
}

/*
 * Add a partitions details to the devices partition description.
 */
int add_partition(int dev, char id[], ULONG start, ULONG size, int byteswap)
{
    if (blkdevnum == BLKDEVNUM) {
        kprintf("Maximum number of partitions reached!\n");
        return -1;
    }
#if DBG_BLKDEV
    kprintf(" %c=%c%c%c, start=%ld, size=%ld\n", 'A'+blkdevnum, id[0],
                                                id[1], id[2], start, size);
#endif

    blkdev[blkdevnum].id[0] = id[0];
    blkdev[blkdevnum].id[1] = id[1];
    blkdev[blkdevnum].id[2] = id[2];
    blkdev[blkdevnum].id[3] = '\0';
    blkdev[blkdevnum].start = start;
    blkdev[blkdevnum].size  = size;

    blkdev[blkdevnum].unit  = dev + 2;
    blkdev[blkdevnum].valid = 1;
    blkdev[blkdevnum].byteswap = byteswap;

    /* make just GEM/BGM partitions visible to applications */
/*
    if (strcmp(blkdev[blkdevnum].id, "GEM") == 0
        || strcmp(blkdev[blkdevnum].id, "BGM") == 0)
*/
        drvbits |= (1L << blkdevnum);

    blkdevnum++;

    return 0;
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
        if ((lrecnr < 0) || (blkdev[dev].size > 0
                             && (lrecnr + lcount) >= blkdev[dev].size))
            return ESECNF;  /* sector not found */

        /* convert partition offset to absolute offset on a unit */
        lrecnr += blkdev[dev].start;

        /* convert logical drive to physical unit */
        unit = blkdev[dev].unit;

#if DBG_BLKDEV
        kprintf("rwabs translated: sector=%ld, count=%ld\n", lrecnr, lcount);
#endif
    }
    else {
        if (unit < 0 || unit >= UNITSNUM || !devices[unit].valid)
            return EUNDEV;  /* unknown device */

        /* check if the start & count values are valid for this device */
        if ((lrecnr < 0) || (devices[unit].size > 0
                             && (lrecnr + lcount) >= devices[unit].size))
            return ESECNF;  /* sector not found */
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
#if CONF_WITH_FLOPPY
                retval = floppy_rw(rw, buf, scount, lrecnr,
                                   blkdev[unit].geometry.spt,
                                   blkdev[unit].geometry.sides, unit);
#else
                retval = EUNDEV;
#endif
            }
            else {
                retval = rw ? DMAwrite(lrecnr, scount, buf, unit-2)
                            : DMAread(lrecnr, scount, buf, unit-2);
            }
        } while((retval < 0) && (--retries > 0));
        buf += scount;
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
    struct fat16_bs *b16;
    ULONG tmp;
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
    b16 = (struct fat16_bs *)dskbufp;
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
    if (blkdev[dev].bpb.recsiz != 0)
        blkdev[dev].bpb.rdlen = (tmp * 32) / blkdev[dev].bpb.recsiz;
    else
        blkdev[dev].bpb.rdlen = 0;
    blkdev[dev].bpb.fsiz = getiword(b->spf);

    /* the structure of the logical disk is assumed to be:
     * - bootsector
     * - other reserved sectors (if any)
     * - fats
     * - dir
     * - data clusters
     */

    blkdev[dev].bpb.fatrec = getiword(b->res) + blkdev[dev].bpb.fsiz; 
    blkdev[dev].bpb.datrec = blkdev[dev].bpb.fatrec + blkdev[dev].bpb.fsiz 
                           + blkdev[dev].bpb.rdlen;
    if (b->spc != 0) {
        tmp = getiword(b->sec);
        /* handle DOS-style disks (512-byte logical sectors) >= 32MB */
        if (tmp == 0)
            tmp = ((ULONG)getiword(b16->sec2+2)<<16) + getiword(b16->sec2);
        blkdev[dev].bpb.numcl = (tmp - blkdev[dev].bpb.datrec) / b->spc;
    }
    else
        blkdev[dev].bpb.numcl = 0;

    /* Check for FAT12 or FAT16 */
    if (b16->ext == 0x29 && !memcmp(b16->fstype, "FAT12   ", 8)) {
        /* Explicit FAT12 */
        blkdev[dev].bpb.b_flags = 0;
    }
    else if (b16->ext == 0x29 && !memcmp(b16->fstype, "FAT16   ", 8)) {
        /* Explicit FAT16 */
        blkdev[dev].bpb.b_flags = B_16;
    }
    else {
        /* We have to guess the filesystem type */
        if (b->media == 0xf8) {
            /* Assume that hard disks use FAT16 */
            blkdev[dev].bpb.b_flags = B_16;
        }
        else {
            /* This is probably a floppy using FAT12 */
            blkdev[dev].bpb.b_flags = 0;
        }
    }

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

    if (dev >= 2) return 0;  /* for harddrives return "not changed" for now */

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



/**
 * blkdev_drvmap - Read drive bitmap
 *
 * Returns a long containing a bit map of logical drives on  the system,
 * with bit 0, the least significant bit, corresponding to drive A.
 * Note that if the BIOS supports logical drives A and B on a single
 * physical drive, it should return both bits set if a floppy is present.
 */

LONG blkdev_drvmap(void)
{
    return(drvbits);
}



/*
 * blkdev_avail - Read drive bitmapCheck drive availability
 *
 * Returns 0, if drive not available
 */

LONG blkdev_avail(WORD dev)
{
    return((1L << dev) & drvbits);
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

/*
vim:et:ts=4:sw=4:
*/
