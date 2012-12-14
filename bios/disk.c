/*
 * disk.c - disk routines
 *
 * Copyright (c) 2001-2012 EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_DISK 0

#include "config.h"
#include "portab.h"
#include "gemerror.h"
#include "disk.h"
#include "asm.h"
#include "blkdev.h"
#include "kprint.h"
#include "xhdi.h"

/*==== Internal declarations ==============================================*/
static int atari_partition(int xhdidev);

/*
 * disk_init
 *
 * Rescans all interfaces and adds all found partitions to blkdev and drvbits
 *
 */

void disk_init(void)
{
    /* scan disk targets in the following order */
    static const int targets[] =
        {16, 18, 17, 19, 20, 22, 21, 23,    /* IDE primary/secondary */
         8, 9, 10, 11, 12, 13, 14, 15,      /* SCSI */
         0, 1, 2, 3, 4, 5, 6, 7};           /* ACSI */
    int i;

    /* scan for attached harddrives and their partitions */
    for(i = 0; i < (sizeof(targets) / sizeof(targets[0])); i++) {
        ULONG blocksize;
        ULONG blocks;
        int major = targets[i];
        int minor = 0;
        int xbiosdev = major + 2;

        if (! XHInqTarget(major, minor, &blocksize, NULL, NULL)) {
            devices[xbiosdev].valid = 1;
            devices[xbiosdev].pssize = blocksize;

            if (! XHGetCapacity(major, minor, &blocks, NULL))
                devices[xbiosdev].size = blocks;
            else
                devices[xbiosdev].size = 0;

            /* scan for ATARI partitions on this harddrive */
            atari_partition(major);
        }
        else
            devices[xbiosdev].valid = 0;
    }
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
#if DBG_DISK
    kprintf("disk.c: checking if a partition is valid...\n");
    kprintf("        flag: %s\n", (pi->flg & 1) ? "OK" : "Failed" );
    kprintf("        partition start (%ld <= %ld): %s\n", pi->st, hdsiz, (pi->st <= hdsiz) ? "OK" : "Failed" );
    kprintf("        partition end (%ld <= %ld): %s\n", pi->st + pi->siz, hdsiz, (pi->st + pi->siz <= hdsiz) ? "OK" : "Failed" );
#endif
    return ((pi->flg & 1) &&
        // isalnum(pi->id[0]) && isalnum(pi->id[1]) && isalnum(pi->id[2]) &&
        pi->st <= hdsiz &&
        pi->st + pi->siz <= hdsiz);
}

static inline int OK_id(char *s)
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

#define MAXPHYSSECTSIZE 2048
union
{
    u8 sect[MAXPHYSSECTSIZE];
    struct rootsector rs;
} physsect, physsect2;
  

/*
 * scans for Atari partitions on 'xhdidev' and adds them to blkdev array
 *
 */
static int atari_partition(int xhdidev)
{
    u8* sect = physsect.sect;
    struct rootsector *rs = &physsect.rs;
    struct partition_info *pi;
    u32 extensect;
    u32 hd_size;
#ifdef ICD_PARTS
    int part_fmt = 0; /* 0:unknown, 1:AHDI, 2:ICD/Supra */
#endif
    int byteswap = 0;

    /* reset the sector buffer content */
    bzero(&physsect, sizeof(physsect));

    if (DMAread(0, 1, (long)&physsect, xhdidev))
        return -1;

    kprintf("%cd%c:","ash"[xhdidev>>3],'a'+(xhdidev&0x07));

    /* check for DOS byteswapped master boot record */
    if (sect[510] == 0xaa && sect[511] == 0x55) {
        int i;
        byteswap = 1;   /* byteswap required for whole disk!! */
#if DBG_DISK
        kprintf("DOS MBR byteswapped checksum detected: byteswap required!\n");
#endif
        /* swap bytes in the loaded boot sector */
        for(i=0; i<SECTOR_SIZE; i+=2) {
            int a = sect[i];
            sect[i] = sect[i+1];
            sect[i+1] = a;
        }
    }

    /* check for DOS master boot record */
    if (sect[510] == 0x55 && sect[511] == 0xaa) {
        /* follow DOS PTBL */
        int i;
        int offset = 446;
        for(i=0; i<4; i++, offset+=16) {
            u32 start, size;
            u8 type = sect[offset+4];
            char pid[3];

            pid[0] = 0;
            pid[1] = 'D';
            pid[2] = type;

            start = sect[offset+11];
            start <<= 8;
            start |= sect[offset+10];
            start <<= 8;
            start |= sect[offset+9];
            start <<= 8;
            start |= sect[offset+8];

            size = sect[offset+15];
            size <<= 8;
            size |= sect[offset+14];
            size <<= 8;
            size |= sect[offset+13];
            size <<= 8;
            size |= sect[offset+12];

            if (type == 0 || size == 0)
                continue;

#if DBG_DISK
            kprintf("DOS partition detected: start=%ld, size=%ld, type=$%02x\n",
                    start, size, type);
#endif
            if (type == 0x05 || type == 0x0f || type == 0x85) {
                kprintf(" extended partitions not yet supported ");
            }
            else {
                add_partition(xhdidev, pid, start, size, byteswap);
                kprintf(" $%02x", type);
            }
        }
    
        kprintf("\n");

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
        kprintf(" Non-ATARI root sector\n");
        return 0;
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
            if (add_partition(xhdidev, pi->id, pi->st, pi->siz, 0))
                break;  /* max number of partitions reached */
            kprintf(" %c%c%c", pi->id[0], pi->id[1], pi->id[2]);
            continue;
        }
        /* extension partition */
#ifdef ICD_PARTS
        part_fmt = 1;
#endif
        kprintf(" XGM<");
        partsect = extensect = pi->st;
        while (1) {
            /* reset the sector buffer content */
            bzero(&physsect2, sizeof(physsect2));

            if (DMAread(partsect, 1, (long)&physsect2, xhdidev)) {
                kprintf(" block %ld read failed\n", partsect);
                return 0;
            }

            /* ++roman: sanity check: bit 0 of flg field must be set */
            if (!(xrs->part[0].flg & 1)) {
                kprintf( "\nFirst sub-partition in extended partition is not valid!\n" );
                break;
            }

            if (add_partition(xhdidev, xrs->part[0].id,
                              partsect + xrs->part[0].st,
                              xrs->part[0].siz, 0))
                break;  /* max number of partitions reached */
            kprintf(" %c%c%c", xrs->part[0].id[0], xrs->part[0].id[1], xrs->part[0].id[2]);

            if (!(xrs->part[1].flg & 1)) {
                /* end of linked partition list */
                break;
            }
            if (memcmp( xrs->part[1].id, "XGM", 3 ) != 0) {
                kprintf("\nID of extended partition is not XGM!\n");
                break;
            }

            partsect = xrs->part[1].st + extensect;
        }
        kprintf(" >");
    }
#ifdef ICD_PARTS
    if ( part_fmt!=1 ) { /* no extended partitions -> test ICD-format */
        pi = &rs->icdpart[0];
        /* sanity check: no ICD format if first partition invalid */
        if (OK_id(pi->id)) {
            kprintf(" ICD<");
            for (; pi < &rs->icdpart[8]; pi++) {
                /* accept only GEM,BGM,RAW,LNX,SWP partitions */
                if (!((pi->flg & 1) && OK_id(pi->id)))
                    continue;
                part_fmt = 2;
                if (add_partition(xhdidev, pi->id, pi->st, pi->siz, 0))
                    break;  /* max number of partitions reached */
                kprintf(" %c%c%c", pi->id[0], pi->id[1], pi->id[2]);
            }
            kprintf(" >");
        }
    }
#endif

    kprintf("\n");

    return 1;
}


/*=========================================================================*/

LONG DMAread(LONG sector, WORD count, LONG buf, WORD dev)
{
    /* TODO: byteswap */
    return XHReadWrite(dev, 0, 0, sector, count, (void *)buf);
}

LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev)
{
    /* TODO: byteswap */
    return XHReadWrite(dev, 0, 1, sector, count, (void *)buf);
}

/*
vim:et:ts=4:sw=4:
*/
