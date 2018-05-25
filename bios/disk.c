/*
 * disk.c - disk routines
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*#define ENABLE_KDEBUG*/

#include "config.h"
#include "portab.h"
#include "gemerror.h"
#include "disk.h"
#include "asm.h"
#include "blkdev.h"
#include "kprint.h"
#include "xhdi.h"
#include "processor.h"
#include "natfeat.h"
#include "tosvars.h"
#include "mfp.h"
#include "ide.h"
#include "acsi.h"
#include "scsi.h"
#include "sd.h"

/*==== Defines ============================================================*/

#define REMOVABLE_PARTITIONS    1   /* minimum # partitions for removable unit */

extern LONG drvrem;                 /* bitmap of removable media drives */

/*==== Structures =========================================================*/
typedef struct {
    UBYTE fill0[4];
    UBYTE type;
    UBYTE fill5[3];
    ULONG start;        /* little-endian */
    ULONG size;         /* little-endian */
} PARTENTRY;

typedef struct {
    UBYTE filler[446];
    PARTENTRY entry[4];
    UWORD bootsig;
} MBR;

/*==== Global variables ===================================================*/

UNIT units[UNITSNUM];

/*==== Internal declarations ==============================================*/
static int atari_partition(UWORD unit,LONG *devices_available);
#if DETECT_NATIVE_FEATURES
static LONG natfeats_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen);
#endif
static LONG internal_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen);

/*
 * scans one unit and adds all found partitions
 */
static void disk_init_one(UWORD unit,LONG *devices_available)
{
    LONG bitmask, devs, rc;
    ULONG blocksize = SECTOR_SIZE;
    ULONG blocks = 0;
    ULONG device_flags;
    WORD shift;
    UNIT *punit = &units[unit];
    int i, n;
    char productname[40];

    punit->valid = 0;
    punit->features = 0;

#if DETECT_NATIVE_FEATURES
    /* First, determine if this unit is supported by NatFeats. */
    rc = natfeats_inquire(unit, NULL, &device_flags, productname, sizeof productname);
    if (rc == E_OK)
    {
        /* Our internal drivers will never be used for this unit */
        punit->features |= UNIT_NATFEATS;
        KINFO(("unit %d is managed by NatFeats: %s\n", unit, productname));
    }
    else
#endif
    {
        /* Try our internal drivers */
        for (i = 0; i <= HD_DETECT_RETRIES; i++)
        {
            rc = internal_inquire(unit, NULL, &device_flags, productname, sizeof productname);
            if (rc == 0)
                break;
        }
        if (rc) {
            KDEBUG(("disk_init_one(): internal_inquire(%d) returned %ld\n",unit,rc));
            return;
        }
        KINFO(("unit %d is managed by EmuTOS internal drivers: %s\n", unit, productname));
    }

    /* try to update with real capacity & blocksize */
    disk_get_capacity(unit, &blocks, &blocksize);
    shift = get_shift(blocksize);
    if (shift < 0) {    /* if blksize not a power of 2, ignore */
        KDEBUG(("disk_init_one(): invalid blocksize (%lu)\n",blocksize));
        return;
    }

    punit->valid = 1;
#if CONF_WITH_IDE
    punit->byteswap = 0;
#endif
    punit->size = blocks;
    punit->psshift = shift;
    punit->last_access = 0;
    punit->status = 0;

    if (device_flags & XH_TARGET_REMOVABLE)
        punit->features |= UNIT_REMOVABLE;

    /* scan for ATARI partitions on this harddrive */
    devs = *devices_available;  /* remember initial set */
    atari_partition(unit,devices_available);
    devs ^= *devices_available; /* which ones were allocated this time */

    /*
     * now ensure that we have a minimum number of logical devices
     * for a removable physical unit
     */
    if (device_flags&XH_TARGET_REMOVABLE) {
        for (i = n = 0, bitmask = 1L; i < BLKDEVNUM; i++, bitmask <<= 1)
            if (devs & bitmask)
                n++;    /* count allocated devices */
        for ( ; n < REMOVABLE_PARTITIONS; n++)
            add_partition(unit,devices_available,"BGM",0L,0L);
    }
}

/*
 * disk_init_all
 *
 * scans all interfaces and adds all found partitions to blkdev and drvbits
 *
 */
void disk_init_all(void)
{
    /* scan disk majors in the following order */
    static const int majors[] =
        {16, 18, 17, 19, 20, 22, 21, 23,    /* IDE primary/secondary */
         8, 9, 10, 11, 12, 13, 14, 15,      /* SCSI */
         0, 1, 2, 3, 4, 5, 6, 7,            /* ACSI */
         24, 25, 26, 27, 28, 29, 30, 31};   /* SD/MMC */
    int i;
    LONG devices_available = 0L;
    LONG bitmask;
    BLKDEV *b;

    /*
     * initialise bitmap of available devices
     * (A and B are already assigned to floppy disks)
     */
    for (i = 2, bitmask = 0x04L; i < BLKDEVNUM; i++, bitmask <<= 1)
        devices_available |= bitmask;

    /* scan for attached harddrives and their partitions */
    for(i = 0; i < ARRAY_SIZE(majors); i++) {
        UWORD unit = NUMFLOPPIES + majors[i];
        disk_init_one(unit,&devices_available);
        if (!devices_available) {
            KDEBUG(("disk_init_all(): maximum number of partitions reached!\n"));
            break;
        }
    }

    /* save bitmaps of drives associated with each physical unit.
     * these maps are not changed after booting.
     *
     * also save bitmap of removable drives
     */
    for (i = 0; i < UNITSNUM; i++)  /* initialise */
        units[i].drivemap = 0L;
    drvrem = 0UL;

    /* update bitmaps */
    for (i = 0, bitmask = 1L, b = blkdev; i < BLKDEVNUM; i++, bitmask <<= 1, b++) {
        if (b->flags&DEVICE_VALID) {
            units[b->unit].drivemap |= bitmask;
            if (units[b->unit].features&UNIT_REMOVABLE)
                drvrem |= bitmask;
        }
    }

#ifdef ENABLE_KDEBUG
    for (i = 0; i < UNITSNUM; i++) {
        int j;
        if (units[i].valid) {
            KDEBUG(("Phys drive %d => logical drive(s)",i));
            for (j = 0; j < BLKDEVNUM; j++)
                if (units[i].drivemap & (1L<<j))
                    KDEBUG((" %c",'A'+j));
            KDEBUG(("\n"));
        }
    }
    KDEBUG(("Removable logical drives:"));
    for (i = 0, bitmask = 1L; i < BLKDEVNUM; i++, bitmask <<= 1)
        if (drvrem & bitmask)
            KDEBUG((" %c",'A'+i));
    KDEBUG(("\n"));
#endif
}

/*
 * media change detection
 */
LONG disk_mediach(UWORD unit)
{
    UWORD major = unit - NUMFLOPPIES;
    LONG ret;
    WORD bus, reldev;

#if DETECT_NATIVE_FEATURES
    if (units[unit].features & UNIT_NATFEATS) {
        /* The NatFeats have no media change mechanism */
        return MEDIANOCHANGE;
    }
#endif

    /*
     * if less than half a second since last access, assume no mediachange
     */
    if (hz_200 < units[unit].last_access + CLOCKS_PER_SEC/2)
        return MEDIANOCHANGE;

    /* get bus and relative device */
    bus = GET_BUS(major);
    reldev = major - bus * DEVICES_PER_BUS;
    MAYBE_UNUSED(reldev);

    /* hardware access to device */
    switch(bus) {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        ret = acsi_ioctl(reldev,GET_MEDIACHANGE,NULL);
        break;
#endif /* CONF_WITH_ACSI */
#if CONF_WITH_SCSI
    case SCSI_BUS:
        ret = scsi_ioctl(reldev,GET_MEDIACHANGE,NULL);
        break;
#endif /* CONF_WITH_SCSI */
#if CONF_WITH_IDE
    case IDE_BUS:
        ret = ide_ioctl(reldev,GET_MEDIACHANGE,NULL);
        break;
#endif /* CONF_WITH_IDE */
#if CONF_WITH_SDMMC
    case SDMMC_BUS:
        ret = sd_ioctl(reldev,GET_MEDIACHANGE,NULL);
        break;
#endif /* CONF_WITH_SDMMC */
    default:
        ret = EUNDEV;
    }

    if (ret == MEDIACHANGE)
        disk_rescan(unit);

    KDEBUG(("disk_mediach(%d) returned %ld\n",unit,ret));
    return ret;
}

/*
 * rescan partitions on specified drive
 * this is used to handle media change on removable drives
 */
void disk_rescan(UWORD unit)
{
    int i;
    LONG devices_available, bitmask;

    /* determine available devices for rescan */
    devices_available = units[unit].drivemap;

    KDEBUG(("disk_rescan(%d):drivemap=0x%08lx\n",unit,devices_available));

    /* rescan (this clobbers 'devices_available') */
    disk_init_one(unit,&devices_available);

    /* now set the mediachange byte for the relevant devices */
    devices_available = units[unit].drivemap;
    for (i = 0, bitmask = 1L; i < BLKDEVNUM; i++, bitmask <<= 1)
        if (devices_available & bitmask)
            blkdev[i].mediachange = MEDIACHANGE;
}

/*
 * partition detection code
 *
 * atari part inspired by Linux 2.4.x kernel (file fs/partitions/atari.c)
 */

#include "string.h"
#include "atari_rootsec.h"

#define ICD_PARTS

/* check if a partition entry looks valid -- Atari format is assumed if at
   least one of the primary entries is ok this way */
static int VALID_PARTITION(struct partition_info *pi, unsigned long hdsiz)
{
    KDEBUG(("disk.c: checking if a partition is valid...\n"));
    KDEBUG(("        flag: %s\n", (pi->flg & 1) ? "OK" : "Failed" ));
    KDEBUG(("        partition start (%ld <= %ld): %s\n", pi->st, hdsiz, (pi->st <= hdsiz) ? "OK" : "Failed" ));
    KDEBUG(("        partition end (%ld <= %ld): %s\n", pi->st + pi->siz, hdsiz, (pi->st + pi->siz <= hdsiz) ? "OK" : "Failed" ));

    return ((pi->flg & 1) &&
        /* isalnum(pi->id[0]) && isalnum(pi->id[1]) && isalnum(pi->id[2]) && */
        pi->st <= hdsiz &&
        pi->st + pi->siz <= hdsiz);
}

static int OK_id(const char *s)
{
    /* for description of the following partition types see
     * the XHDI specification ver 1.30
     * http://toshyp.atari.org/en/010008.html#Recommended_20partition_20types
     */
    static const char supported_partition_types[][3] = {
        "GEM", /* GEMDOS partition < 16 MB */
        "BGM", /* GEMDOS partition > 16 MB */
        "LNX", /* Linux Ext2 partition, should be handled like 'RAW' */
        "SWP", /* Swap partition, should be handled like 'RAW' */
        "MIX", /* Minix partition, should be handled like 'RAW' */
        "UNX", /* ASV (Atari System V R4) partition, should be handled like 'RAW' */
        "QWA", /* QDOS partition, should be handled like 'RAW' */
        "MAC", /* MAC HFS partition, should be handled like 'RAW' */
        "F32", /* TOS-compatible FAT32 partition */
        "RAW", /* Partition type RAW */
    };

    int i;

    for (i = 0; i < ARRAY_SIZE(supported_partition_types); i++) {
        if (memcmp (s, supported_partition_types[i], 3) == 0)
            return TRUE;
    }

    return FALSE;
}

/*
 * determine whether we're looking at a partitioned
 * or a partitionless disk (like a floppy)
 *
 * returns the size in sectors if it appears to be partitionless
 * otherwise return 0
 */
static ULONG check_for_no_partitions(UBYTE *sect)
{
    struct fat16_bs *bs = (struct fat16_bs *)sect;
    ULONG size = 0UL;
    int i;

    if ((bs->media == 0xf8)
     && (bs->ext == 0x29)
     && (memcmp(bs->fstype,"FAT16   ",8) == 0)
     && (bs->sec[0] == 0)
     && (bs->sec[1] == 0)) {
        for (i = 3; i >= 0; i--)
            size = (size << 8) + bs->sec2[i];
    }

    return size;
}


#define MAXPHYSSECTSIZE 512
typedef union
{
    u8 sect[MAXPHYSSECTSIZE];
    struct rootsector rs;
    MBR mbr;
} PHYSSECT;

PHYSSECT physsect, physsect2;

#if CONF_WITH_IDE

/*
 * This function is only used during byteswap detection.
 * Subsequent byteswap will be performed by the IDE driver itself.
 */
static void byteswap(UBYTE *buffer, ULONG size)
{
    UWORD *p;

    for (p = (UWORD *)buffer; p < (UWORD *)(buffer+size); p++)
        swpw(*p);
}

static void maybe_fix_byteswap(UWORD unit, PHYSSECT *pphyssect)
{
    u8* sect = pphyssect->sect;
    MBR *mbr = &pphyssect->mbr;

    if (mbr->bootsig == 0xaa55)
    {
        KINFO(("DOS MBR byteswapped signature detected: enabling byteswap\n"));

        /* Fix loaded physical sector */
        byteswap(sect, SECTOR_SIZE);

        /* Enable byteswap in the IDE driver for subsequent access */
        units[unit].byteswap = 1;
    }
}

#endif /* CONF_WITH_IDE */

/*
 * scans for Atari partitions on unit and adds them to blkdev array
 *
 */
static int atari_partition(UWORD unit,LONG *devices_available)
{
    u8* sect = physsect.sect;
    struct rootsector *rs = &physsect.rs;
    struct partition_info *pi;
    MBR *mbr = &physsect.mbr;
    u32 extensect;
    u32 hd_size;
    int major = unit - NUMFLOPPIES;
#ifdef ICD_PARTS
    int part_fmt = 0; /* 0:unknown, 1:AHDI, 2:ICD/Supra */
#endif
    MAYBE_UNUSED(major);

    if (disk_rw(unit, RW_READ, 0, 1, sect))
        return -1;

    KINFO(("%cd%c: ","ashf????"[major>>3],'a'+(major&0x07)));

#if CONF_WITH_IDE
    /* IDE drives may be byteswapped if partitioned on foreign hardware */
    if (IS_IDE_DEVICE(major))
        maybe_fix_byteswap(unit, &physsect);
#endif /* CONF_WITH_IDE */

    /* check for DOS disk without partitions */
    if (mbr->bootsig == 0x55aa) {
        ULONG size = check_for_no_partitions(sect);
        if (size) {
            if (add_partition(unit,devices_available,"BGM",0UL,size) < 0)
                return -1;
            KINFO((" fake BGM\n"));
            return 1;
        }
    }

    /* check for DOS master boot record */
    if (mbr->bootsig == 0x55aa) {
        /* follow DOS PTBL */
        int i;

        /* offset to current extended boot record if != 0 */
        u32 extended_offs = 0;

        /* start sector of first(!) extended boot record;
         * this is required to traverse the list of linked extended partitions
         */
        u32 first_extended = 0;
        /* start sector of next extended boot record */
        u32 next_extended;

        do {
            /* start sector of next extended boot record, if present */
            next_extended = 0;

            KINFO((" MBR at %lu", extended_offs));

            for (i = 0; i < 4; i++) {
                u32 start, size;
                u8 type = mbr->entry[i].type;
                char pid[3];

                if (type == 0) {
                    KDEBUG((" empty partition entry ignored\n"));
                    continue;
                }

                pid[0] = 0;
                pid[1] = 'D';
                pid[2] = type;

                start = mbr->entry[i].start;    /* little-endian */
                swpl(start);

                size = mbr->entry[i].size;      /* little-endian */
                swpl(size);

                if (size == 0UL) {
                    KDEBUG((" entry for zero-length partition ignored\n"));
                    continue;
                }

                KDEBUG(("DOS partition detected: start=%lu, size=%lu, type=$%02x\n",
                        start, size, type));

                switch(type) {
                case 0x05:
                case 0x0f:
                    if (next_extended != 0) {
                        KDEBUG((" more than one extended partition: ignored, not yet supported\n"));
                    } else {
                        /* extended partition found, will read partition table later
                         * note that for the linked list of extended partitions,
                         * the start sector in this case is always relative to the first(!)
                         * extended boot record and not to the current one
                         */
                        next_extended = start + first_extended;
                    }
                    break;
                case 0x0b:
                case 0x0c:
                case 0x83:      /* any Linux partition, including ext2 */
                    /*
                     * note that FAT32 & Linux partitions occupy drive letters,
                     * but are not yet accessible to EmuTOS.  however, we allow
                     * access via XHDI for MiNT's benefit.
                     */
                    KDEBUG((" %s partition: not yet supported\n",(type==0x83)?"Linux":"FAT32"));
                    /* drop through */
                case 0x01:
                case 0x04:
                case 0x06:
                case 0x0e:
                    if (add_partition(unit,devices_available,pid,start+extended_offs,size) < 0)
                        return -1;
                    KINFO((" $%02x", type));
                    break;
                default:
                    KDEBUG((" unrecognised partition type: ignored\n"));
                    break;
                }
            }

            KINFO(("\n"));

            /* read next extended boot record */
            if (next_extended != 0) {
                /* save offset of first extended partition to later traverse linked list */
                if (first_extended == 0) {
                    first_extended = next_extended;
                }

                if (disk_rw(unit, RW_READ, next_extended, 1, sect)) {
                    extended_offs = next_extended = 0; /* could not read table */
                } else {
                    extended_offs = next_extended;
                }
            }

        } while (next_extended != 0);

        return 1;
    }

    hd_size = rs->hd_siz;

    /* Verify this is an Atari rootsector: */
    if (!VALID_PARTITION(&rs->part[0], hd_size) &&
        !VALID_PARTITION(&rs->part[1], hd_size) &&
        !VALID_PARTITION(&rs->part[2], hd_size) &&
        !VALID_PARTITION(&rs->part[3], hd_size)) {
        /*
         * if there's no valid primary partition, assume that no Atari
         * format partition table (there's no reliable magic or the like
         * :-()
         */
        KINFO((" Non-ATARI root sector\n"));
        return 0;
    }

    /*
     * circumvent bug in Hatari v1.7 & earlier: the ACSI Read Capacity
     * command, which we have just used by calling disk_get_capacity() in
     * disk_init_all() above, returns a value approximately 512 times too
     * small for the capacity.  this makes the value in units[].size
     * too small.
     *
     * if the value in units[].size is less than the disk size stored
     * in the partition table, we assume that we've encountered the bug.
     * we fix it by replacing units[].size with the partition table
     * value.
     */
    if (units[unit].size < hd_size) {
        KINFO(("Setting disk capacity from partition table value\n"));
        units[unit].size = hd_size;
    }

    pi = &rs->part[0];
    for (; pi < &rs->part[4]; pi++) {
        struct rootsector *xrs = &physsect2.rs;
        unsigned long partsect;

        if ( !(pi->flg & 1) )
            continue;
        /* active partition */
        if (memcmp (pi->id, "XGM", 3) != 0) {
            /* we don't care about other id's */
            if (add_partition(unit,devices_available,pi->id,pi->st,pi->siz) < 0)
                break;  /* max number of partitions reached */

            KINFO((" %c%c%c", pi->id[0], pi->id[1], pi->id[2]));
            continue;
        }
        /* extension partition */
#ifdef ICD_PARTS
        part_fmt = 1;
#endif
        KINFO((" XGM<"));
        partsect = extensect = pi->st;
        while (1) {
            if (disk_rw(unit, RW_READ, partsect, 1, physsect2.sect)) {
                KINFO((" block %ld read failed\n", partsect));
                return 0;
            }

            /* ++roman: sanity check: bit 0 of flg field must be set */
            if (!(xrs->part[0].flg & 1)) {
                KINFO(( "\nFirst sub-partition in extended partition is not valid!\n"));
                break;
            }

            if (add_partition(unit,devices_available,xrs->part[0].id,
                              partsect+xrs->part[0].st,xrs->part[0].siz) < 0)
                break;  /* max number of partitions reached */

            KINFO((" %c%c%c", xrs->part[0].id[0], xrs->part[0].id[1], xrs->part[0].id[2]));

            if (!(xrs->part[1].flg & 1)) {
                /* end of linked partition list */
                break;
            }
            if (memcmp( xrs->part[1].id, "XGM", 3 ) != 0) {
                KINFO(("\nID of extended partition is not XGM!\n"));
                break;
            }

            partsect = xrs->part[1].st + extensect;
        }
    }
#ifdef ICD_PARTS
    if ( part_fmt!=1 ) { /* no extended partitions -> test ICD-format */
        pi = &rs->icdpart[0];
        /* sanity check: no ICD format if first partition invalid */
        if (OK_id(pi->id)) {
            KINFO((" ICD<"));
            for (; pi < &rs->icdpart[8]; pi++) {
                /* accept only GEM,BGM,RAW,LNX,SWP partitions */
                if (!((pi->flg & 1) && OK_id(pi->id)))
                    continue;
                part_fmt = 2;
                if (add_partition(unit,devices_available,pi->id,pi->st,pi->siz) < 0)
                    break;  /* max number of partitions reached */
                KINFO((" %c%c%c", pi->id[0], pi->id[1], pi->id[2]));
            }
            KINFO((" >"));
        }
    }
#endif

    KINFO(("\n"));

    return 1;
}


/*=========================================================================*/

#if DETECT_NATIVE_FEATURES

/* Get unit information, using NatFeats only. */
static LONG natfeats_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen)
{
    UWORD major = unit - NUMFLOPPIES;

    if (!get_xhdi_nfid())
        return EUNDEV;

    return NFCall(get_xhdi_nfid() + XHINQTARGET2, (long)major, (long)0, (long)blocksize, (long)deviceflags, (long)productname, (long)stringlen);
}

#endif /* DETECT_NATIVE_FEATURES */

/* Get unit information, using our internal drivers only. */
static LONG internal_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen)
{
    UWORD major = unit - NUMFLOPPIES;
    LONG ret;
    WORD bus, reldev;
    BYTE name[40] = "Disk";
    ULONG flags = 0UL;
    MAYBE_UNUSED(reldev);
    MAYBE_UNUSED(ret);

    bus = GET_BUS(major);
    reldev = major - bus * DEVICES_PER_BUS;

    /*
     * hardware access to device
     *
     * note: we expect the xxx_ioctl() functions to physically access the
     * device, since internal_inquire() may be used to determine its presence
     */
    switch(bus) {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        ret = acsi_ioctl(reldev,GET_DISKNAME,name);
        break;
#endif /* CONF_WITH_ACSI */
#if CONF_WITH_SCSI
    case SCSI_BUS:
        ret = scsi_ioctl(reldev,GET_DISKNAME,name);
        break;
#endif /* CONF_WITH_SCSI */
#if CONF_WITH_IDE
    case IDE_BUS:
        ret = ide_ioctl(reldev,GET_DISKNAME,name);
        break;
#endif /* CONF_WITH_IDE */
#if CONF_WITH_SDMMC
    case SDMMC_BUS:
        ret = sd_ioctl(reldev,GET_DISKNAME,name);
        flags = XH_TARGET_REMOVABLE;    /* medium is removable */
        break;
#endif /* CONF_WITH_SDMMC */
    default:
        ret = EUNDEV;
    }

    /* if device doesn't exist, we're done */
    if (ret == EUNDEV)
        return ret;

    /* return values as requested */
    if (blocksize)
        *blocksize = SECTOR_SIZE;   /* standard physical sector size on HDD */
    if (deviceflags)
        *deviceflags = flags;
    if (productname)
        strlcpy(productname,name,stringlen);

    return 0;
}

#if CONF_WITH_XHDI
/* Get unit information, whatever low-level driver is used. */
LONG disk_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen)
{
#if DETECT_NATIVE_FEATURES
    if (units[unit].features & UNIT_NATFEATS) {
        return natfeats_inquire(unit, blocksize, deviceflags, productname, stringlen);
    }
#endif

    return internal_inquire(unit, blocksize, deviceflags, productname, stringlen);
}
#endif /* CONF_WITH_XHDI */

/* Get unit capacity */
LONG disk_get_capacity(UWORD unit, ULONG *blocks, ULONG *blocksize)
{
    UWORD major = unit - NUMFLOPPIES;
    LONG ret;
    ULONG info[2] = { 0UL, 512UL }; /* #sectors, sectorsize */
    WORD bus, reldev;
    MAYBE_UNUSED(reldev);
    MAYBE_UNUSED(ret);

#if DETECT_NATIVE_FEATURES
    if (units[unit].features & UNIT_NATFEATS) {
        return NFCall(get_xhdi_nfid() + XHGETCAPACITY, (long)major, (long)0, (long)blocks, (long)blocksize);
    }
#endif

    bus = GET_BUS(major);
    reldev = major - bus * DEVICES_PER_BUS;

    /* hardware access to device */
    switch(bus) {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        ret = acsi_ioctl(reldev,GET_DISKINFO,info);
        KDEBUG(("acsi_ioctl(%d) returned %ld\n", reldev, ret));
        if (ret < 0)
            return EUNDEV;
        break;
#endif /* CONF_WITH_ACSI */
#if CONF_WITH_SCSI
    case SCSI_BUS:
        ret = scsi_ioctl(reldev,GET_DISKINFO,info);
        KDEBUG(("scsi_ioctl(%d) returned %ld\n", reldev, ret));
        if (ret < 0)
            return ret;
        break;
#endif /* CONF_WITH_SCSI */
#if CONF_WITH_IDE
    case IDE_BUS:
        ret = ide_ioctl(reldev,GET_DISKINFO,info);
        KDEBUG(("ide_ioctl(%d) returned %ld\n", reldev, ret));
        if (ret < 0)
            return ret;
        break;
#endif /* CONF_WITH_IDE */
#if CONF_WITH_SDMMC
    case SDMMC_BUS:
        ret = sd_ioctl(reldev,GET_DISKINFO,info);
        KDEBUG(("sd_ioctl(%d) returned %ld\n", reldev, ret));
        if (ret < 0)
            return ret;
        break;
#endif /* CONF_WITH_SDMMC */
    default:
        return EUNDEV;
    }

    if (blocks)
        *blocks = info[0];
    if (blocksize)
        *blocksize = info[1];

    return 0;
}

/* Unit read/write */
LONG disk_rw(UWORD unit, UWORD rw, ULONG sector, UWORD count, UBYTE *buf)
{
    UWORD major = unit - NUMFLOPPIES;
    LONG ret;
    WORD bus, reldev;
    MAYBE_UNUSED(reldev);

#if DETECT_NATIVE_FEATURES
    if (units[unit].features & UNIT_NATFEATS) {
        ret = NFCall(get_xhdi_nfid() + XHREADWRITE, (long)major, (long)0, (long)rw, (long)sector, (long)count, buf);
        if (ret == EACCDN)      /* circumvent quirk in ARAnyM 1.0.2 and earlier */
            ret = EWRPRO;
        return ret;
    }
#endif

    bus = GET_BUS(major);
    reldev = major - bus * DEVICES_PER_BUS;

    /* hardware access to device */
    switch(bus) {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        ret = acsi_rw(rw, sector, count, buf, reldev);
        KDEBUG(("acsi_rw() returned %ld\n", ret));
        break;
#endif /* CONF_WITH_ACSI */
#if CONF_WITH_SCSI
    case SCSI_BUS:
        ret = scsi_rw(rw, sector, count, buf, reldev);
        KDEBUG(("scsi_rw() returned %ld\n", ret));
        break;
#endif /* CONF_WITH_SCSI */
#if CONF_WITH_IDE
    case IDE_BUS:
    {
        BOOL need_byteswap = units[unit].byteswap;
        ret = ide_rw(rw, sector, count, buf, reldev, need_byteswap);
        KDEBUG(("ide_rw() returned %ld\n", ret));
        break;
    }
#endif /* CONF_WITH_IDE */
#if CONF_WITH_SDMMC
    case SDMMC_BUS:
        ret = sd_rw(rw, sector, count, buf, reldev);
        KDEBUG(("sd_rw() returned %ld\n", ret));
        break;
#endif /* CONF_WITH_SDMMC */
    default:
        ret = EUNDEV;
    }

    return ret;
}

/*==== XBIOS functions ====================================================*/

LONG DMAread(LONG sector, WORD count, UBYTE *buf, WORD major)
{
    UWORD unit = NUMFLOPPIES + major;
    LONG rc;

    rc = disk_rw(unit, RW_READ, sector, count, buf);

    /* TOS invalidates the i-cache here, so be compatible */
    instruction_cache_kludge(buf,count<<units[unit].psshift);

    return rc;
}

LONG DMAwrite(LONG sector, WORD count, const UBYTE *buf, WORD major)
{
    UWORD unit = NUMFLOPPIES + major;

    return disk_rw(unit, RW_WRITE, sector, count, CONST_CAST(UBYTE *, buf));
}
