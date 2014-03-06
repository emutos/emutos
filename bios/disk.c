/*
 * disk.c - disk routines
 *
 * Copyright (c) 2001-2014 The EmuTOS development team
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
#include "ide.h"
#include "acsi.h"

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

/*==== Internal declarations ==============================================*/
static int atari_partition(int major,LONG *devices_available);

/*
 * scans one unit and adds all found partitions
 */
static void disk_init_one(int major,LONG *devices_available)
{
    LONG bitmask, devs, rc;
    ULONG blocksize = SECTOR_SIZE;
    ULONG blocks = 0;
    ULONG device_flags;
    WORD shift;
    UNIT *unit = units + major + NUMFLOPPIES;
    int i, n, minor = 0;

    unit->valid = 0;

    rc = XHInqTarget(major, minor, NULL, &device_flags, NULL);
    if (rc) {
        KDEBUG(("disk_init_one(): XHInqTarget(%d) returned %ld\n",major,rc));
        return;
    }

    /* try to update with real capacity & blocksize */
    XHGetCapacity(major, minor, &blocks, &blocksize);
    shift = get_shift(blocksize);
    if (shift < 0) {    /* if blksize not a power of 2, ignore */
        KDEBUG(("disk_init_one(): invalid blocksize (%lu)\n",blocksize));
        return;
    }

    unit->valid = 1;
    unit->byteswap = 0;
    unit->size = blocks;
    unit->psshift = shift;
    unit->last_access = 0;
    unit->features = (device_flags&XH_TARGET_REMOVABLE) ? UNIT_REMOVABLE : 0;
    unit->status = 0;

    /* scan for ATARI partitions on this harddrive */
    devs = *devices_available;  /* remember initial set */
    atari_partition(major,devices_available);
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
            add_partition(major,devices_available,"BGM",0L,0L);
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
    for(i = 0; i < (sizeof(majors) / sizeof(majors[0])); i++) {
        disk_init_one(majors[i],&devices_available);
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
        if (b->valid) {
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
 * rescan partitions on specified drive
 * this is used to handle media change on removable drives
 */
void disk_rescan(int major)
{
    int i;
    int unit = major + NUMFLOPPIES;
    LONG devices_available, bitmask;

    /* determine available devices for rescan */
    devices_available = units[unit].drivemap;

    KDEBUG(("disk_rescan(%d):drivemap=0x%08lx\n",major,devices_available));

    /* rescan (this clobbers 'devices_available') */
    disk_init_one(major,&devices_available);

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
        // isalnum(pi->id[0]) && isalnum(pi->id[1]) && isalnum(pi->id[2]) &&
        pi->st <= hdsiz &&
        pi->st + pi->siz <= hdsiz);
}

static int OK_id(char *s)
{
    /* for description of the following partition types see
     * the XHDI specification ver 1.30
     */
    return  memcmp (s, "GEM", 3) == 0 || memcmp (s, "BGM", 3) == 0 ||
            memcmp (s, "LNX", 3) == 0 || memcmp (s, "SWP", 3) == 0 ||
            memcmp (s, "MIX", 3) == 0 || memcmp (s, "UNX", 3) == 0 ||
            memcmp (s, "QWA", 3) == 0 || memcmp (s, "MAC", 3) == 0 ||
            memcmp (s, "F32", 3) == 0 || memcmp (s, "RAW", 3) == 0;
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


#define MAXPHYSSECTSIZE 2048
union
{
    u8 sect[MAXPHYSSECTSIZE];
    struct rootsector rs;
    MBR mbr;
} physsect, physsect2;


/*
 * scans for Atari partitions on 'major' and adds them to blkdev array
 *
 */
static int atari_partition(int major,LONG *devices_available)
{
    u8* sect = physsect.sect;
    struct rootsector *rs = &physsect.rs;
    struct partition_info *pi;
    MBR *mbr = &physsect.mbr;
    u32 extensect;
    u32 hd_size;
    int unit = major + NUMFLOPPIES;
#ifdef ICD_PARTS
    int part_fmt = 0; /* 0:unknown, 1:AHDI, 2:ICD/Supra */
#endif

    /* reset the sector buffer content */
    bzero(&physsect, sizeof(physsect));

    if (DMAread(0, 1, (long)&physsect, major))
        return -1;

    KINFO(("%cd%c: ","ashf????"[major>>3],'a'+(major&0x07)));

    /* check for DOS byteswapped master boot record.
     * this is enabled on IDE units only,
     * because other media do not suffer of that problem.
     */
    if (IS_IDE_DEVICE(major) && mbr->bootsig == 0xaa55) {
        KINFO(("DOS MBR byteswapped signature detected: enabling byteswap\n"));
        units[unit].byteswap = 1; /* byteswap required for whole disk */
        /* swap bytes in the loaded boot sector */
        byteswap(sect,SECTOR_SIZE);
    }

    /* check for DOS disk without partitions */
    if (mbr->bootsig == 0x55aa) {
        ULONG size = check_for_no_partitions(sect);
        if (size) {
            if (add_partition(major,devices_available,"BGM",0UL,size) < 0)
                return -1;
            KINFO((" fake BGM\n"));
            return 1;
        }
    }

    /* check for DOS master boot record */
    if (mbr->bootsig == 0x55aa) {
        /* follow DOS PTBL */
        int i;

        KINFO((" MBR"));

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
                KDEBUG((" extended partition: ignored, not yet supported\n"));
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
                if (add_partition(major,devices_available,pid,start,size) < 0)
                    return -1;
                KINFO((" $%02x", type));
                break;
            default:
                KDEBUG((" unrecognised partition type: ignored\n"));
                break;
            }
        }

        KINFO(("\n"));

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
     * command, which we have just used by calling XHGetCapacity() in
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
            if (add_partition(major,devices_available,pi->id,pi->st,pi->siz) < 0)
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
            /* reset the sector buffer content */
            bzero(&physsect2, sizeof(physsect2));

            if (DMAread(partsect, 1, (long)&physsect2, major)) {
                KINFO((" block %ld read failed\n", partsect));
                return 0;
            }

            /* ++roman: sanity check: bit 0 of flg field must be set */
            if (!(xrs->part[0].flg & 1)) {
                KINFO(( "\nFirst sub-partition in extended partition is not valid!\n"));
                break;
            }

            if (add_partition(major,devices_available,xrs->part[0].id,
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
                if (add_partition(major,devices_available,pi->id,pi->st,pi->siz) < 0)
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

LONG disk_rw(UWORD unit, UWORD rw, ULONG sector, UWORD count, void *buf)
{
    UWORD major = unit - NUMFLOPPIES;
    LONG ret;
    WORD bus, reldev;
    MAYBE_UNUSED(reldev);

#if DETECT_NATIVE_FEATURES
    /* direct access to device */
    if (get_xhdi_nfid()) {
        ret = NFCall(get_xhdi_nfid() + XHREADWRITE, (long)major, (long)0, (long)rw, (long)sector, (long)count, buf);
        if (ret != EINVFN && ret != EUNDEV)
            return ret;
    }
#endif

    bus = GET_BUS(major);
    reldev = major - bus * DEVICES_PER_BUS;

    /* hardware access to device */
    switch(bus) {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        ret = acsi_rw(rw, sector, count, (LONG)buf, reldev);
        KDEBUG(("acsi_rw() returned %ld\n", ret));
        break;
#endif /* CONF_WITH_ACSI */
#if CONF_WITH_IDE
    case IDE_BUS:
    {
        BOOL need_byteswap = units[unit].byteswap;
        ret = ide_rw(rw, sector, count, (LONG)buf, reldev, need_byteswap);
        KDEBUG(("ide_rw() returned %ld\n", ret));
        break;
    }
#endif /* CONF_WITH_IDE */
#if CONF_WITH_SDMMC
    case SDMMC_BUS:
        ret = sd_rw(rw, sector, count, (LONG)buf, reldev);
        KDEBUG(("sd_rw() returned %ld\n", ret));
        break;
#endif /* CONF_WITH_SDMMC */
    default:
        ret = EUNDEV;
    }

    return ret;
}

/*=========================================================================*/

LONG DMAread(LONG sector, WORD count, LONG buf, WORD major)
{
    UWORD unit = NUMFLOPPIES + major;
    LONG rc;

    rc = disk_rw(unit, RW_READ, sector, count, (void *)buf);

    /* TOS invalidates the i-cache here, so be compatible */
    instruction_cache_kludge((void *)buf,count<<units[unit].psshift);

    return rc;
}

LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD major)
{
    UWORD unit = NUMFLOPPIES + major;

    return disk_rw(unit, RW_WRITE, sector, count, (void *)buf);
}

void byteswap(UBYTE *buffer, ULONG size)
{
    UWORD *p;

    for (p = (UWORD *)buffer; p < (UWORD *)(buffer+size); p++)
        swpw(*p);
}
