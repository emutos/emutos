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
 
#include "kprint.h"

/*==== External declarations ==============================================*/

extern LONG ara_DMAread(LONG sector, WORD count, LONG buf, WORD dev);    /* in startup.S */


/*==== Introduction =======================================================*/

/*
 * This file contains all disk-related xbios routines.
 */
LONG DMAread(LONG sector, WORD count, LONG buf, WORD dev)
{
#if DBG_DISK
    kprintf("DMAread(%ld, %d, %ld, %d)\n", sector, count, buf, dev);
#endif
        if (dev >= 0 && dev <= 7) {
        /* ACSI */
    }
    else if (dev <= 15) {
        /* SCSI */
    }
    else if (dev <= 17) {
        /* IDE */
    }
    else if (dev <= 19) {
        /* Milan Secondary IDE channel */
    }
#if ARANYM_NATIVE_DISK
    else if (dev <= UNITSNUM) {
        /* virtual device through host access */
        return ara_DMAread(sector, count, buf, dev);
    }
#endif

    return EUNDEV;  /* unknown device */
}

LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev)
{
        return 0;
}

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
        return  memcmp (s, "GEM", 3) == 0 || memcmp (s, "BGM", 3) == 0 ||
                memcmp (s, "LNX", 3) == 0 || memcmp (s, "SWP", 3) == 0 ||
                memcmp (s, "RAW", 3) == 0 ;
}

#define MAXPHYSSECTSIZE 2048
int atari_partition(int bdev)
{
        int m_lim = BLKDEVNUM;
        int minor = 0;
        char sect[MAXPHYSSECTSIZE];
        struct rootsector *rs;
        struct partition_info *pi;
        u32 extensect;
        u32 hd_size;
#ifdef ICD_PARTS
        int part_fmt = 0; /* 0:unknown, 1:AHDI, 2:ICD/Supra */
#endif

    if (DMAread(0, 1, (long)sect, bdev))
        return -1;

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
                kprintf("Non-ATARI root sector\n");
                return 0;
        }

        pi = &rs->part[0];
        printk ("AHDI:");
        for (; pi < &rs->part[4] && minor < m_lim; minor++, pi++) {
                struct rootsector *xrs;
                char sect2[MAXPHYSSECTSIZE];
                unsigned long partsect;

                if ( !(pi->flg & 1) )
                        continue;
                /* active partition */
                if (memcmp (pi->id, "XGM", 3) != 0) {
                        /* we don't care about other id's */
                        add_partition(bdev, minor, pi->id, pi->st, pi->siz);
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

                        add_partition(bdev, minor, xrs->part[0].id,
                                   partsect + xrs->part[0].st,
                                   xrs->part[0].siz);

                        if (!(xrs->part[1].flg & 1)) {
                                /* end of linked partition list */
                                break;
                        }
                        if (memcmp( xrs->part[1].id, "XGM", 3 ) != 0) {
                                printk("\nID of extended partition is not XGM!\n");
                                break;
                        }

                        partsect = xrs->part[1].st + extensect;
                        minor++;
                        if (minor >= m_lim) {
                                printk( "\nMaximum number of partitions reached!\n" );
                                break;
                        }
                }
                printk(" >");
        }
#ifdef ICD_PARTS
        if ( part_fmt!=1 ) { /* no extended partitions -> test ICD-format */
                pi = &rs->icdpart[0];
                /* sanity check: no ICD format if first partition invalid */
                if (OK_id(pi->id)) {
                        printk(" ICD<");
                        for (; pi < &rs->icdpart[8] && minor < m_lim; minor++, pi++) {
                                /* accept only GEM,BGM,RAW,LNX,SWP partitions */
                                if (!((pi->flg & 1) && OK_id(pi->id)))
                                        continue;
                                part_fmt = 2;
                                add_partition(bdev, minor, pi->id, pi->st, pi->siz);
                        }
                        printk(" >");
                }
        }
#endif

        printk ("\n");

        return 1;
}
