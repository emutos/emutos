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
static int atari_partition(int xhdidev,LONG *devices_available);

/*
 * disk_init_all
 *
 * Rescans all interfaces and adds all found partitions to blkdev and drvbits
 *
 */

void disk_init_all(void)
{
    /* scan disk targets in the following order */
    static const int targets[] =
        {16, 18, 17, 19, 20, 22, 21, 23,    /* IDE primary/secondary */
         8, 9, 10, 11, 12, 13, 14, 15,      /* SCSI */
         0, 1, 2, 3, 4, 5, 6, 7,            /* ACSI */
         24, 25, 26, 27, 28, 29, 30, 31};   /* SD/MMC */
    int i;
    LONG devices_available = 0L;
    LONG bitmask;

    /*
     * initialise bitmap of available devices
     * (A and B are already assigned to floppy disks)
     */
    for (i = 2, bitmask = 0x04L; i < BLKDEVNUM; i++, bitmask <<= 1)
        devices_available |= bitmask;

    /* scan for attached harddrives and their partitions */
    for(i = 0; i < (sizeof(targets) / sizeof(targets[0])); i++) {
        disk_init_one(targets[i],&devices_available);
        if (!devices_available) {
            KDEBUG(("disk_init_all(): maximum number of partitions reached!\n"));
            break;
        }
    }
}

void disk_init_one(int major,LONG *devices_available)
{
    LONG rc;
    ULONG blocksize;
    ULONG blocks;
    WORD shift;
    int xbiosdev = major + NUMFLOPPIES;
    int minor = 0;

    devices[xbiosdev].valid = 0;

    rc = XHInqTarget(major, minor, NULL, NULL, NULL);
    if (rc) {
        KDEBUG(("disk_init_one(): XHInqTarget(%d) returned %ld\n",major,rc));
        return;
    }

    blocks = 0;
    blocksize = SECTOR_SIZE;

    /* try to update with real capacity & blocksize */
    XHGetCapacity(major, minor, &blocks, &blocksize);
    shift = get_shift(blocksize);
    if (shift < 0) {    /* if blksize not a power of 2, ignore */
        KDEBUG(("disk_init_one(): invalid blocksize (%lu)\n",blocksize));
        return;
    }

    devices[xbiosdev].valid = 1;
    devices[xbiosdev].byteswap = 0;
    devices[xbiosdev].size = blocks;
    devices[xbiosdev].psshift = shift;
    devices[xbiosdev].last_access = 0;

    /* scan for ATARI partitions on this harddrive */
    atari_partition(major,devices_available);
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
 * scans for Atari partitions on 'xhdidev' and adds them to blkdev array
 *
 */
static int atari_partition(int xhdidev,LONG *devices_available)
{
    u8* sect = physsect.sect;
    struct rootsector *rs = &physsect.rs;
    struct partition_info *pi;
    MBR *mbr = &physsect.mbr;
    u32 extensect;
    u32 hd_size;
    int xbiosdev = xhdidev + NUMFLOPPIES;
#ifdef ICD_PARTS
    int part_fmt = 0; /* 0:unknown, 1:AHDI, 2:ICD/Supra */
#endif

    /* reset the sector buffer content */
    bzero(&physsect, sizeof(physsect));

    if (DMAread(0, 1, (long)&physsect, xhdidev))
        return -1;

    KINFO(("%cd%c: ","ashf????"[xhdidev>>3],'a'+(xhdidev&0x07)));

    /* check for DOS byteswapped master boot record.
     * this is enabled on IDE devices only,
     * because other media do not suffer of that problem.
     */
    if (IS_IDE_DEVICE(xhdidev) && mbr->bootsig == 0xaa55) {
        KINFO(("DOS MBR byteswapped signature detected: enabling byteswap\n"));
        devices[xbiosdev].byteswap = 1; /* byteswap required for whole disk */
        /* swap bytes in the loaded boot sector */
        byteswap(sect,SECTOR_SIZE);
    }

    /* check for DOS disk without partitions */
    if (mbr->bootsig == 0x55aa) {
        ULONG size = check_for_no_partitions(sect);
        if (size) {
            if (add_partition(xhdidev,devices_available,"BGM",0UL,size) < 0)
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

            if (size == 0) {
                KDEBUG((" entry for zero-length partition ignored\n"));
                continue;
            }

            KDEBUG(("DOS partition detected: start=%lu, size=%lu, type=$%02x\n",
                    start, size, type));

            if (type == 0x05 || type == 0x0f || type == 0x85) {
                KDEBUG((" extended partitions not yet supported\n"));
            }
            else {
                if (add_partition(xhdidev,devices_available,pid,start,size) < 0)
                    return -1;
                KINFO((" $%02x", type));
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
     * small for the capacity.  this makes the value in devices[].size
     * too small.
     *
     * if the value in devices[].size is less than the disk size stored
     * in the partition table, we assume that we've encountered the bug.
     * we fix it by replacing devices[].size with the partition table
     * value.
     */
    if (devices[xbiosdev].size < hd_size) {
        KINFO(("Setting disk capacity from partition table value\n"));
        devices[xbiosdev].size = hd_size;
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
            if (add_partition(xhdidev,devices_available,pi->id,pi->st,pi->siz) < 0)
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

            if (DMAread(partsect, 1, (long)&physsect2, xhdidev)) {
                KINFO((" block %ld read failed\n", partsect));
                return 0;
            }

            /* ++roman: sanity check: bit 0 of flg field must be set */
            if (!(xrs->part[0].flg & 1)) {
                KINFO(( "\nFirst sub-partition in extended partition is not valid!\n"));
                break;
            }

            if (add_partition(xhdidev,devices_available,xrs->part[0].id,
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
                if (add_partition(xhdidev,devices_available,pi->id,pi->st,pi->siz) < 0)
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

LONG DMAread(LONG sector, WORD count, LONG buf, WORD dev)
{
LONG rc;

    rc = XHReadWrite(dev, 0, 0, sector, count, (void *)buf);

    /* TOS invalidates the i-cache here, so be compatible */
    instruction_cache_kludge((void *)buf,count<<devices[dev+NUMFLOPPIES].psshift);

    return rc;
}

LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev)
{
    return XHReadWrite(dev, 0, 1, sector, count, (void *)buf);
}

void byteswap(UBYTE *buffer, ULONG size)
{
    UWORD *p;

    for (p = (UWORD *)buffer; p < (UWORD *)(buffer+size); p++)
        swpw(*p);
}

/*
vim:et:ts=4:sw=4:
*/
