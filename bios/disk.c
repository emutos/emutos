/*
 * disk.c - disk routines
 *
 * Copyright (c) 2001,2002 EmuTOS development team
 *
 * Authors:
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_DISK 0

#include "portab.h"
#include "gemerror.h"
#include "disk.h"
#include "asm.h"
#include "blkdev.h"
#include "config.h"
#include "kprint.h"
#include "xhdi.h"

/*==== External declarations ==============================================*/

/*
 * disk_init
 *
 * Rescans all interfaces and adds all found partitions to blkdev and drvbits
 *
 */

void disk_init(void)
{
        /* scan disk targets in the following order */
    int targets[] = {16, 18, 17, 19, 20, 22, 21, 23,    /* IDE primary/secondary */
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
 * inspired by Linux 2.4.x kernel (file fs/partitions/atari.c)
 */

#include "string.h"
#include "atari_rootsec.h"

#define printk  kprintf

#define ICD_PARTS

/* check if a partition entry looks valid -- Atari format is assumed if at
   least one of the primary entries is ok this way */
static int VALID_PARTITION(struct partition_info *pi, unsigned long hdsiz)
{
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
char sect[MAXPHYSSECTSIZE];
char sect2[MAXPHYSSECTSIZE];

/*
 * scans for Atari partitions on disk 'bdev' and adds them to blkdev array
 *
 */
int atari_partition(int bdev)
{
    struct rootsector *rs;
    struct partition_info *pi;
    u32 extensect;
    u32 hd_size;
#ifdef ICD_PARTS
    int part_fmt = 0; /* 0:unknown, 1:AHDI, 2:ICD/Supra */
#endif

    /* reset the sector buffer content */
    bzero(sect, sizeof(sect));

    if (DMAread(0, 1, (long)sect, bdev))
        return -1;

    printk("%cd%c:", (bdev >> 3) ? (((bdev >> 3) == 2) ? 'h' : 's') : 'a', (bdev & 7) + 'a');

    rs = (struct rootsector *)sect;
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
        struct rootsector *xrs;
        unsigned long partsect;

        if ( !(pi->flg & 1) )
            continue;
        /* active partition */
        if (memcmp (pi->id, "XGM", 3) != 0) {
            /* we don't care about other id's */
            if (add_partition(bdev, pi->id, pi->st, pi->siz))
                break;  /* max number of partitions reached */
            printk(" %c%c%c", pi->id[0], pi->id[1], pi->id[2]);
            continue;
        }
        /* extension partition */
#ifdef ICD_PARTS
        part_fmt = 1;
#endif
        printk(" XGM<");
        partsect = extensect = pi->st;
        while (1) {
            /* reset the sector buffer content */
            bzero(sect2, sizeof(sect2));

            if (DMAread(partsect, 1, (long)sect2, bdev)) {
                printk (" block %ld read failed\n", partsect);
                return 0;
            }
            xrs = (struct rootsector *)sect2;

            /* ++roman: sanity check: bit 0 of flg field must be set */
            if (!(xrs->part[0].flg & 1)) {
                printk( "\nFirst sub-partition in extended partition is not valid!\n" );
                break;
            }

            if (add_partition(bdev, xrs->part[0].id,
                              partsect + xrs->part[0].st,
                              xrs->part[0].siz))
                break;  /* max number of partitions reached */
            printk(" %c%c%c", xrs->part[0].id[0], xrs->part[0].id[1], xrs->part[0].id[2]);

            if (!(xrs->part[1].flg & 1)) {
                /* end of linked partition list */
                break;
            }
            if (memcmp( xrs->part[1].id, "XGM", 3 ) != 0) {
                printk("\nID of extended partition is not XGM!\n");
                break;
            }

            partsect = xrs->part[1].st + extensect;
        }
        printk(" >");
    }
#ifdef ICD_PARTS
    if ( part_fmt!=1 ) { /* no extended partitions -> test ICD-format */
        pi = &rs->icdpart[0];
        /* sanity check: no ICD format if first partition invalid */
        if (OK_id(pi->id)) {
            printk(" ICD<");
            for (; pi < &rs->icdpart[8]; pi++) {
                /* accept only GEM,BGM,RAW,LNX,SWP partitions */
                if (!((pi->flg & 1) && OK_id(pi->id)))
                    continue;
                part_fmt = 2;
                if (add_partition(bdev, pi->id, pi->st, pi->siz))
                    break;  /* max number of partitions reached */
                printk(" %c%c%c", pi->id[0], pi->id[1], pi->id[2]);
            }
            printk(" >");
        }
    }
#endif

    printk ("\n");

    return 1;
}


/*=========================================================================*/

LONG DMAread(LONG sector, WORD count, LONG buf, WORD dev)
{
    return XHReadWrite(dev, 0, 0, sector, count, (void *)buf);
}

LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev)
{
    return XHReadWrite(dev, 0, 1, sector, count, (void *)buf);
}
