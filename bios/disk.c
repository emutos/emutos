/*
 * disk.c - disk routines
 *
 * Copyright (c) 2001 EmuTOS development team
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
#include "acsi.h"
#include "kprint.h"

/*==== External declarations ==============================================*/

extern LONG ara_XHDI(WORD function, ...);    /* in startup.S */
extern int native_print_kind;   /* hack to detect if aranym is present */


/*
 * partition detection code
 *
 * inspired by Linux 2.4.x kernel (file fs/partitions/atari.c)
 */

#include "ctype.h"
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


static LONG scsi_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev)
{
    /* implement SCSI here */
    return EUNDEV;
}

static LONG ide_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev)
{
    /* implement IDE here */
    return EUNDEV;
}

static LONG dma_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev)
{
#if DBG_DISK
    kprintf("XBIOS DMA%s(%ld, %d, 0x%08lx, %d)\n", 
            rw ? "write" : "read", sector, count, buf, dev);
#endif

    if (dev >= 0 && dev < 8) {
        return acsi_rw(rw, sector, count, buf, dev);
    }
    else if (dev < 16) {
        return scsi_rw(rw, sector, count, buf, dev);
    }
    else if (dev < 24) {
        return ide_rw(rw, sector, count, buf, dev);
    }
    return EUNDEV;  /* unknown device */
}


LONG DMAread(LONG sector, WORD count, LONG buf, WORD dev)
{
#if ARANYM_NATIVE_DISK
    /* direct access to host device */
    if (native_print_kind == 2)
        return ara_XHDI(XHREADWRITE, dev, 0, 0, sector, count, buf);
#endif
    return dma_rw(0, sector, count, buf, dev);
}

LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev)
{
#if ARANYM_NATIVE_DISK
    /* direct access to host device */
    if (native_print_kind == 2)
        return ara_XHDI(XHREADWRITE, dev, 0, 1, sector, count, buf);
#endif
    return dma_rw(1, sector, count, buf, dev);
}


/*
 * XHDI implementation
 */

LONG XHInqTarget(UWORD major, UWORD minor, ULONG *blocksize,
                 ULONG *device_flags, char *product_name)
{
    int retval;
    if (minor != 0)
        return EUNDEV;

    memset(sect, 0, sizeof(sect));
    memset(sect2, 0xff, sizeof(sect));
    retval = DMAread(0, 1, (long)sect, major);
    if (! retval) {
        if (blocksize) {
            /* TODO could add some heuristic here based on difference 
             * between sect and sect2 contents 
             *   DMAread(0, 1, (long)sect2, major);
             */
            *blocksize = 512; 
        }
        if (device_flags)
            *device_flags = 0;  /* not implemented yet */
        if (product_name)
            strcpy(product_name, "Generic Disk");
    }

    return retval;
}

LONG XHGetCapacity(UWORD major, UWORD minor, ULONG *blocks, ULONG *blocksize)
{
#if ARANYM_NATIVE_DISK
    if (native_print_kind == 2) {
    /* direct access to host device */
        return ara_XHDI(XHGETCAPACITY, major, minor, blocks, blocksize);
    }
#endif
    /* TODO could read the blocks from Atari root sector */
    return EINVFN;
}
