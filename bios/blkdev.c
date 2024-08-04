/*
 * blkdev.c - BIOS block device functions
 *
 * Copyright (C) 2002-2024 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *  PES     Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "string.h"
#include "gemerror.h"
#include "asm.h"
#include "tosvars.h"
#include "ahdi.h"
#include "floppy.h"
#include "machine.h"
#include "disk.h"
#include "ikbd.h"
#include "blkdev.h"
#include "processor.h"
#include "acsi.h"
#include "scsi.h"
#include "ide.h"
#include "sd.h"
#include "scsidriv.h"
#include "biosext.h"
#include "biosmem.h"
#include "xhdi.h"
#include "intmath.h"


/*
 * undefine the following to enable booting from hard disk.
 * note that this is experimental and, as of march 2015,
 * will cause a crash if used with hard disk drivers ...
 */
#define DISABLE_HD_BOOT

/*
 * Global variables
 */

BLKDEV blkdev[BLKDEVNUM];

static PUN_INFO pun_info;

/*
 * Function prototypes
 */
static LONG blkdev_hdv_boot(void);
static void blkdev_hdv_init(void);
static LONG blkdev_mediach(WORD dev);
static LONG blkdev_rwabs(WORD rw, UBYTE *buf, WORD cnt, WORD recnr, WORD dev, LONG lrecnr);
static LONG bootcheck(void);
static void bus_init(void);
static WORD hd_boot_read(void);

/* get intel words */
static UWORD getiword(UBYTE *addr)
{
    UWORD value;
    value = MAKE_UWORD(addr[1], addr[0]);
    return value;
}

/*
 * compute word checksum
 */
UWORD compute_cksum(const UWORD *buf)
{
    int i;
    UWORD sum;
    const UWORD *p = buf;

    for (i = 0, sum = 0; i < SECTOR_SIZE/sizeof(UWORD); i++)
        sum += *p++;

    return sum;
}

/*
 * blkdevs_init - BIOS block drive initialization
 *
 * Called from bios.c, this routine will do necessary block device
 * initialization.
 */

void blkdev_init(void)
{
    /* initialize dskbufp for the block buffer */
    dskbufp = dskbuf;
    KDEBUG(("dskbufp = %p\n",dskbufp));

    /* setup booting related vectors */
    hdv_boot    = blkdev_hdv_boot;
    /* On TOS hdv_init points to the floppy detection and setup routine. */
    /* EmuTOS does not use hdv_init, but initializes it for compatibility. */
    hdv_init    = flop_hdv_init;

    /* setup general block device vectors */
    hdv_bpb     = blkdev_getbpb;
    hdv_rw      = blkdev_rwabs;
    hdv_mediach = blkdev_mediach;

    /* setting drvbits */
    blkdev_hdv_init();
}

/*
 * set up an AHDI-compatible PUN_INFO structure
 */
static void pun_info_setup(void)
{
    int i, physdev, old_physdev;
    BLKDEV *dev;
    LONG max_size;

    /*
     * initialize empty PUN_INFO
     */
    pun_info.puns = 0;
    pun_info.max_sect_siz = MAX_LOGSEC_SIZE;    /* temporarily, for blkdev_getbpb() */
    for (i = 0; i < PUN_MAXUNITS; i++)
    {
        pun_info.pun[i] = 0xff;
        pun_info.partition_start[i] = 0;
        pun_info.reserved[i] = 0;
    }

    /*
     * fill in PUN_INFO, using data from blkdev[]
     */
    old_physdev = -1;
    max_size = SECTOR_SIZE;     /* used for determining actual max logsec size */
    for (dev = blkdev, i = NUMFLOPPIES; dev < blkdev+BLKDEVNUM; dev++)
    {
        physdev = dev->unit - NUMFLOPPIES;
        if ((dev->flags & DEVICE_VALID) && (physdev >= 0))
        {
            if (physdev != old_physdev)
            {
                pun_info.puns++;        /* count physical devices */
                old_physdev = physdev;
            }
            blkdev_getbpb(i);           /* force boot sector scan & BPB rebuild */
            pun_info.pun[i] = physdev;
            pun_info.partition_start[i] = dev->start;
            if (dev->flags & GETBPB_ALLOWED)
                if (dev->bpb.recsiz > max_size)
                    max_size = dev->bpb.recsiz;
            if (++i >= PUN_MAXUNITS)    /* cannot store info for devices > P: */
                break;
        }
    }

    pun_info.cookie = 0x41484449L; /* AHDI */
    pun_info.cookie_ptr = &pun_info.cookie;
    pun_info.version_num = 0x300;      /* AHDI v3.00 */
    pun_info.max_sect_siz = (max_size > CONF_LOGSEC_SIZE) ? max_size : CONF_LOGSEC_SIZE;

    pun_ptr = &pun_info;

    KDEBUG(("PUN INFO: max sector size = %u\n",pun_info.max_sect_siz));
}

/*
 * blkdev_hdv_init
 *
 */

static void blkdev_hdv_init(void)
{
    /* Start with no drives. This matters for EmuTOS-RAM, because the system
     * variables are not automatically reinitialized. */
    drvbits = 0;

    /* Detect and initialize floppy drives */
    flop_hdv_init();

    /*
     * do bus initialisation, such as setting delay values
     */
    bus_init();

#if CONF_WITH_SCSI_DRIVER
    scsidriv_init();    /* detect all devices */
#endif

    disk_init_all();    /* Detect hard disk partitions */

#if CONF_WITH_XHDI
    init_XHDI_drvmap(); /* remember drives that we control */
#endif

    pun_info_setup();
}

/*
 * call bus initialisation routines
 */
static void bus_init(void)
{
#if CONF_WITH_ACSI
    acsi_init();
#endif

#if CONF_WITH_SCSI
    scsi_init();
#endif

#if CONF_WITH_IDE
    ide_init();
#endif

#if CONF_WITH_SDMMC
    sd_init();
#endif
}

/*
 * blkdev_boot - boot from device in 'bootdev'
 */
LONG blkdev_boot(void)
{
    KDEBUG(("drvbits = %08lx\n",drvbits));

    /*
     * if the user decided to skip hard drive boot, we set the
     * boot device to floppy A:
     */
    if (bootflags & BOOTFLAG_SKIP_HDD_BOOT)
        bootdev = FLOPPY_BOOTDEV;

    /*
     * if the user decided to skip AUTO programs, we don't
     * attempt to execute the bootsector
     */
    if (bootflags & BOOTFLAG_SKIP_AUTO_ACC)
        return 0;

#ifdef DISABLE_HD_BOOT
    if (bootdev >= NUMFLOPPIES) /* don't attempt to boot from hard disk */
        return 0;
#endif

    /*
     * execute the bootsector code (if present)
     */
    return blkdev_hdv_boot();
}

/*
 * blkdev_hdv_boot - BIOS boot vector
 */
static LONG blkdev_hdv_boot(void)
{
    LONG err;

    err = bootcheck();
    KDEBUG(("bootcheck() returns %ld\n",err));

    if (err == 0) {     /* if bootable, jump into it */
        invalidate_instruction_cache(dskbufp,SECTOR_SIZE);
        regsafe_call(dskbufp);
    }

    return err;         /* may never be reached, if booting */
}

static LONG bootcheck(void)
{
    WORD err;

    if ((bootdev < 0) || (bootdev >= BLKDEVNUM))
        return 2;   /* couldn't load */

    err = (bootdev < NUMFLOPPIES) ? flop_boot_read() : hd_boot_read();
    if (err)
        return 3;   /* unreadable */

    if (compute_cksum((const UWORD *)dskbufp) != 0x1234)
        return 4;   /* not valid boot sector */

#ifdef TARGET_FLOPPY
    if (!strncmp((const char*)(dskbufp + 2), "EmuTOS", 6))
    {
        /* Do not allow EmuTOS floppy to reload itself,
         * otherwise there will be an infinite loop. */
        return 4;   /* not valid boot sector */
    }
#endif

    return 0;       /* bootable */
}

/*
 * read boot sector from 'bootdev' into 'dskbufp'
 *
 * since the 'dskbufp' buffer is only 2 physical sectors in size, we
 * can't use blkdev_rwabs(), so we use disk_rw()
 */
static WORD hd_boot_read(void)
{
    BLKDEV *b = &blkdev[bootdev];

    return (WORD)disk_rw(b->unit,RW_READ,b->start,1,dskbufp);
}

/*
 * return next available device number in bitmap
 */
static int next_logical(LONG *devices_available)
{
    int logical;

    for (logical = 0; logical < BLKDEVNUM; logical++)
        if (*devices_available & (1L<<logical))
            return logical;

    return -1;
}

 /*
 * getbpb_allowed - check if partition type supports GetBPB
 */
static BOOL getbpb_allowed(char *id)
{
    /* check for Atari-style partitions */
    if ((strcmp(id,"BGM") == 0) || (strcmp(id,"GEM") == 0))
        return TRUE;

    /* check for certain DOS-style partitions */
    if ((id[0] == '\0') && (id[1] == 'D'))
    {
        switch(id[2])
        {
        case 0x01:
        case 0x04:
        case 0x06:
        case 0x0e:
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * Add a partition's details to the device's partition description.
 */
int add_partition(UWORD unit, LONG *devices_available, char id[], ULONG start, ULONG size)
{
    int logical;
    BLKDEV *b;

    logical = next_logical(devices_available);
    if (logical < 0) {
        KDEBUG(("add_partition(): maximum number of partitions reached!\n"));
        return -1;
    }
    b = blkdev + logical;

    if (id[0])
        KDEBUG((" %c=%c%c%c",'A'+logical,id[0],id[1],id[2]));
    else
        KDEBUG((" %c=$%02x",'A'+logical,id[2]));
    KDEBUG((",start=%ld,size=%ld\n",start,size));

    b->id[0] = id[0];
    b->id[1] = id[1];
    b->id[2] = id[2];
    b->id[3] = '\0';
    b->start = start;
    b->size  = size;

    b->flags = DEVICE_VALID;
    b->mediachange = MEDIANOCHANGE;
    b->unit  = unit;

    /* flag partitions that support GetBPB() */
    if (getbpb_allowed(b->id))
        b->flags |= GETBPB_ALLOWED;

    /* make just GEM/BGM partitions visible to applications */
/*
    if (strcmp(b->id, "GEM") == 0
        || strcmp(b->id, "BGM") == 0)
*/
        drvbits |= (1L << logical);

    *devices_available &= ~(1L << logical); /* mark device as used */

    return 0;
}


/*
 * blkdev_rwabs - BIOS block device read/write vector
 */

#define CNTMAX  0x7FFF  /* 16-bit MAXINT */

static LONG blkdev_rwabs(WORD rw, UBYTE *buf, WORD cnt, WORD recnr, WORD dev, LONG lrecnr)
{
    int retries = RWABS_RETRIES;
    int unit = dev;
    LONG lcount = cnt;
    LONG retval;
    WORD psshift;
    UBYTE *bufstart = buf;
    GEOMETRY *geo;

    KDEBUG(("rwabs(rw=%d, buf=%p, count=%ld, recnr=%u, dev=%d, lrecnr=%ld)\n",
            rw,buf,lcount,recnr,dev,lrecnr));

    /*
     * handle special undocumented feature in the floppy-only Rwabs() handler
     * installed by TOS: if the buffer pointer is NULL, the value in 'count'
     * is set as the current mediachange status (expected to be 0-2).
     * this feature is used by FastCopy III (and probably others).
     */
    if ((dev < NUMFLOPPIES) && (buf == NULL)) {
        blkdev[dev].mediachange = cnt;
        return 0L;
    }

    if (recnr != -1)            /* if long offset not used */
        lrecnr = (UWORD)recnr;  /* recnr as unsigned to enable 16-bit recn */

    if (rw & RW_NORETRIES)
        retries = 1;

    /*
     * are we accessing a physical unit or a logical device?
     */
    if (! (rw & RW_NOTRANSLATE)) {      /* logical */
        int sectors;
        if ((dev < 0 ) || (dev >= BLKDEVNUM) || !(blkdev[dev].flags&DEVICE_VALID))
            return EUNDEV;  /* unknown device */

        if (blkdev[dev].bpb.recsiz == 0)/* invalid BPB, e.g. FAT32 or ext2 */
            return ESECNF;              /* (this is an XHDI convention)    */

        /*
         * the following is used to ensure that altering logical
         * sector zero causes a media change to be detected
         */
        if (blkdev[dev].forcechange)
            return E_CHNG;
        if ((rw & RW_WRITE) && (lrecnr == 0))
            blkdev[dev].forcechange = TRUE;

        /* convert logical sectors to physical ones */
        sectors = blkdev[dev].bpb.recsiz >> units[blkdev[dev].unit].psshift;
        lcount *= sectors;
        lrecnr *= sectors;

        /* check if the count fits into this partition */
        if ((lrecnr < 0) || (blkdev[dev].size > 0
                             && (lrecnr + lcount) > blkdev[dev].size))
            return ESECNF;  /* sector not found */

        /* convert partition offset to absolute offset on a unit */
        lrecnr += blkdev[dev].start;

        /* convert logical drive to physical unit */
        unit = blkdev[dev].unit;

        KDEBUG(("rwabs translated: sector=%ld, count=%ld\n",lrecnr,lcount));

        if (! (rw & RW_NOMEDIACH)) {
            if (blkdev_mediach(dev) != MEDIANOCHANGE) {
                KDEBUG(("blkdev_rwabs(): media change detected\n"));
                return E_CHNG;
            }
        }

        /* In logical mode RW_NOBYTESWAP is not supported. */
        rw &= ~RW_NOBYTESWAP;
    }
    else {                              /* physical */
        if (unit < 0 || unit >= UNITSNUM || !units[unit].valid)
            return EUNDEV;  /* unknown device */

        /* check if the start & count values are valid for this unit */
        if ((lrecnr < 0) || (units[unit].size > 0
                             && (lrecnr + lcount) >= units[unit].size))
            return ESECNF;  /* sector not found */

        /*
         * the following is how I think this *ought* to work when a
         * physical unit is specified.  TOS may do it differently:
         * I haven't checked (RFB)
         */
        if (! (rw & RW_NOMEDIACH)) {
            if (units[unit].status&UNIT_CHANGED) {
                KDEBUG(("blkdev_rwabs(): media change detected\n"));
                units[unit].status &= ~UNIT_CHANGED;
                return E_CHNG;
            }
        }
    }

    psshift = units[unit].psshift;
    geo = &blkdev[unit].geometry;

    do {
        /* split the transfer to 15-bit count blocks (lowlevel functions take WORD count) */
        WORD scount = (lcount > CNTMAX) ? CNTMAX : lcount;
        do {        /* outer loop retries if critical event handler says we should */
            do {    /* inner loop automatically retries */
                retval = (unit<NUMFLOPPIES) ? floppy_rw(rw, buf, scount, lrecnr, geo->spt, geo->sides, unit)
                                            : disk_rw(unit, (rw & ~RW_NOTRANSLATE), lrecnr, scount, buf);
                if (retval == E_CHNG)       /* no automatic retry on media change */
                    break;
            } while((retval < 0) && (--retries > 0));
            if ((retval < 0L) && !(rw & RW_NOTRANSLATE))    /* only call etv_critic for logical requests */
                retval = call_etv_critic((WORD)retval,dev);
        } while(retval == CRITIC_RETRY_REQUEST);
        if (retval < 0)     /* error, retries exhausted */
            break;
        buf += (ULONG)scount << psshift;
        lrecnr += scount;
        lcount -= scount;
    } while(lcount > 0);

    /* TOS invalidates the i-cache here, so be compatible */
    if ((rw&RW_RW) == RW_READ)
        instruction_cache_kludge(bufstart,cnt<<psshift);

    if (retval == 0)
        units[unit].last_access = hz_200;

    if (retval == E_CHNG)
        if (unit >= NUMFLOPPIES)
            disk_rescan(unit);

    return retval;
}


/*
 * get_shift - get #bits to shift left to convert from blocksize to bytes
 *
 * returns -1 iff blocksize is not a power of 2
 */
WORD get_shift(ULONG blocksize)
{
    WORD i;
    ULONG n;

    for (i = 0, n = 1UL; i < 32; i++, n <<= 1)
        if (n == blocksize)
            return i;

    return -1;
}


/*
 * blkdev_getbpb - Get BIOS parameter block
 *
 * implement the Mediach flush as documented in Compendium
 */

LONG blkdev_getbpb(WORD dev)
{
    BLKDEV *bdev = blkdev + dev;
    struct bs *b;
    struct fat16_bs *b16;
    ULONG tmp, clsizb;
    LONG ret;
    UWORD reserved, recsiz;
    int n, unit;

    KDEBUG(("blkdev_getbpb(%d)\n",dev));

    if ((dev < 0 ) || (dev >= BLKDEVNUM) || !(bdev->flags&DEVICE_VALID))
    {
        KDEBUG(("device is invalid\n"));
        return 0L;  /* unknown device */
    }

    unit = bdev->unit;

    bdev->mediachange = MEDIANOCHANGE;      /* reset now */
    bdev->forcechange = FALSE;
    /*
     * set XHDI's "invalid BPB" indicator for non-floppy units
     * only, since they are the ones that might have a FAT32 or
     * ext2 filesystem.
     *
     * for floppies, we set the real size, since (a) we know it;
     * (b) it is used by blkdev_rwabs() to convert logical sector
     * numbers to physical; and (c) blkdev_rwabs() may be called
     * before blkdev_getbpb() actually reads the value.
     */
    bdev->bpb.recsiz = (unit < NUMFLOPPIES) ? SECTOR_SIZE : 0;

    /*
     * before we can build the BPB, we need to locate the bootsector.
     * if we're on a removable non-floppy unit, this may have moved
     * since last time, so we handle this first.
     */
    if ((unit >= NUMFLOPPIES) && (units[unit].features & UNIT_REMOVABLE))
        disk_mediach(unit);     /* check for change & rescan partitions if so */

    /* check if this device supports GetBPB() */
    if (!(bdev->flags & GETBPB_ALLOWED))
    {
        KDEBUG(("device does not support Getbpb()\n"));
        return 0L;              /* no can do */
    }

    /*
     * now we can read the bootsector using the physical mode.  for
     * floppies, we read additional sectors so that we can compute
     * checksums later on
     */
    n = (unit < NUMFLOPPIES) ? CHKSUM_SECTORS : 1;
    do {
        ret = blkdev_rwabs(RW_READ | RW_NOMEDIACH | RW_NOTRANSLATE,
                           dskbufp, n, -1, unit, bdev->start);
        if (ret < 0L)
            ret = call_etv_critic((WORD)ret,dev);
    } while(ret == CRITIC_RETRY_REQUEST);

    if (ret < 0L)
    {
        KDEBUG(("can't read boot sector\n"));
        return 0L;  /* error */
    }

    b = (struct bs *)dskbufp;
    b16 = (struct fat16_bs *)dskbufp;

    /* don't login a disk if the logical sector size is too large */
    recsiz = getiword(b->bps);
    if (recsiz > pun_info.max_sect_siz)
    {
        KDEBUG(("recsiz %u is too large (max recsiz = %u)\n",
                recsiz,pun_info.max_sect_siz));
        return 0L;
    }

    /* don't login a disk if the cluster size (in bytes) is invalid */
    clsizb = (ULONG)b->spc * recsiz;
    if ((clsizb == 0UL) || (clsizb > MAX_CLUSTER_SIZE))
    {
        KDEBUG(("invalid cluster size (%lu bytes): spc=%u, recsiz=%u\n",
                clsizb,b->spc,recsiz));
        return 0L;
    }

    /*
     * don't login a disk if the number of FATs is unsupported,
     * but for compatibility with TOS accept "zero" and then assume 2 FATs
     */
    if (b->fat == 0)
    {
        b->fat = 2;
    }
    if ((b->fat < MIN_FATS) || (b->fat > MAX_FATS))
    {
        KDEBUG(("invalid FAT count %u\n",b->fat));
        return 0L;
    }

    KDEBUG(("bootsector[dev=%d] = {\n  ...\n  res = %d;\n  hid = %d;\n}\n",
            dev,getiword(b->res),getiword(b->hid)));

    bdev->bpb.recsiz = recsiz;
    bdev->bpb.clsiz = b->spc;
    bdev->bpb.clsizb = clsizb;

    /*
     * determine the number of root directory sectors
     *
     * if the space required for the specified number of root directory
     * entries is not an exact number of logical sectors, we round up
     */
    tmp = getiword(b->dir);     /* root dir entries */
    if (bdev->bpb.recsiz != 0)
        bdev->bpb.rdlen = divu((tmp*32)+(bdev->bpb.recsiz-1),bdev->bpb.recsiz);
    else
        bdev->bpb.rdlen = 0;
    if (tmp*32 != bdev->bpb.rdlen*bdev->bpb.recsiz)
        KDEBUG(("root directory length has been rounded up\n"));

    bdev->bpb.fsiz = getiword(b->spf);

    /* the structure of the logical disk is assumed to be:
     * - bootsector
     * - other reserved sectors (if any)
     * - fats
     * - dir
     * - data clusters
     */
    reserved = getiword(b->res);
    if (reserved == 0)      /* should not happen */
        reserved = 1;       /* but if it does, Atari TOS assumes this */
    bdev->bpb.fatrec = reserved;
    /*
     * with 2 FATs, use 2nd FAT by default.
     * The code that flushes the FATs also assumes this.
     * When support for single FAT is disabled, assume 2 FATs like Atari TOS.
     */
    if (!CONF_WITH_1FAT_SUPPORT || (b->fat >= 2))
        bdev->bpb.fatrec += bdev->bpb.fsiz;
    bdev->bpb.datrec = bdev->bpb.fatrec + bdev->bpb.fsiz + bdev->bpb.rdlen;

    /*
     * determine number of clusters
     */
    tmp = getiword(b->sec);
    /*
     * a value of zero for total sectors should mean that we have a DOS-style
     * disk (512-byte logical sectors) >= 32MB, but it can also be due to an
     * invalid boot sector.  Atari TOS accepts a zero value & logs in the disk
     * (ending up with a negative value for number of clusters in the BPB).
     *
     * in the general case, EmuTOS must assume that zeros in b->sec means
     * that b->sec2 contains a valid value.  however, for floppies we can
     * assume that it's an invalid boot sector.  in this case, we arrange
     * to set a cluster count of zero.
     */
    if ((tmp == 0UL) && (unit >= NUMFLOPPIES))
        tmp = MAKE_ULONG(getiword(b16->sec2+2), getiword(b16->sec2));
    if (tmp < bdev->bpb.datrec)
        tmp = 0UL;
    else
        tmp = (tmp - bdev->bpb.datrec) / b->spc;
    if ((tmp > MAX_FAT16_CLUSTERS) || (bdev->bpb.fsiz == 0))
    {
        /* FAT32 - unsupported */
        KINFO(("Disk %c: is inaccessible (FAT32)\n",dev+'A'));
        bdev->bpb.recsiz = 0;               /* mark it for XHDI */
        return 0L;
    }
    bdev->bpb.numcl = tmp;

    /*
     * check for FAT12 or FAT16: according to Microsoft (who originated
     * the FAT format, after all), FAT type should be determined on the
     * basis of cluster count and nothing else
     */
    bdev->bpb.b_flags = 0;         /* FAT12 */
    if (bdev->bpb.numcl > MAX_FAT12_CLUSTERS)
        bdev->bpb.b_flags |= B_16;      /* FAT16 */
#if CONF_WITH_1FAT_SUPPORT
    if (b->fat < 2)
        bdev->bpb.b_flags |= B_1FAT;
#endif

    /* additional geometry info */
    bdev->geometry.sides = getiword(b->sides);
    bdev->geometry.spt = getiword(b->spt);
    memcpy(bdev->serial,b->serial,3);
    memcpy(bdev->serial2,b16->serial2,4);

    /* store checksums iff floppy drive */
    if (unit < NUMFLOPPIES)
        flop_checksum(unit, dskbufp);

    KDEBUG(("bpb[dev=%d] = {\n  recsiz = %u;\n  clsiz  = %d;\n",
            dev,bdev->bpb.recsiz,bdev->bpb.clsiz));
    KDEBUG(("  clsizb = %u;\n  rdlen  = %d;\n  fsiz   = %d;\n",
            bdev->bpb.clsizb,bdev->bpb.rdlen,bdev->bpb.fsiz));
    KDEBUG(("  fatrec = %d;\n  datrec = %d;\n  numcl  = %u;\n",
            bdev->bpb.fatrec,bdev->bpb.datrec,bdev->bpb.numcl));
    KDEBUG(("  bflags = %d;\n}\n",bdev->bpb.b_flags));

    return (LONG) &bdev->bpb;
}

/*
 * blkdev_mediach - BIOS media change vector
 */

static LONG blkdev_mediach(WORD dev)
{
    BLKDEV *b = &blkdev[dev];
    UWORD unit;
    LONG ret;

    if ((dev < 0 ) || (dev >= BLKDEVNUM) || !(b->flags&DEVICE_VALID))
        return EUNDEV;  /* unknown device */

    unit = b->unit;

    /* if we've already marked the drive as MEDIACHANGE, don't change it */
    if (b->mediachange == MEDIACHANGE)
        return b->mediachange;

    /*
     * if a mediachange has been forced, we return the same status
     * until Gepbpb() is called
     */
    if (blkdev[dev].forcechange)
        return MEDIACHANGE;

    do {
        ret = (dev<NUMFLOPPIES) ? flop_mediach(dev) : disk_mediach(unit);
        if (ret < 0L)
            ret = call_etv_critic((WORD)ret,dev);
    } while(ret == CRITIC_RETRY_REQUEST);
    if (ret < 0L)
        return ret;

    /* if media (may have) changed, mark physical unit */
    if (ret != MEDIANOCHANGE) {
        units[unit].status |= UNIT_CHANGED;
        b->mediachange = ret;
    }

    return b->mediachange;
}


/**
 * blkdev_drvmap - Read drive bitmap
 *
 * Returns a long containing a bit map of logical drives on the system,
 * with bit 0, the least significant bit, corresponding to drive A.
 * Note that if the BIOS supports logical drives A and B on a single
 * physical drive, it should return both bits set if a floppy drive is
 * present.
 */

LONG blkdev_drvmap(void)
{
    return(drvbits);
}



/*
 * blkdev_avail - Check drive availability
 *
 * Returns 0, if drive not available
 */

LONG blkdev_avail(WORD dev)
{
    return((1L << dev) & drvbits);
}
