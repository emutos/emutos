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

#define DBG_DISK 1

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
    else if (dev <= UNITSNUM) {
        /* virtual device through host access */
        return ara_DMAread(sector, count, buf, dev);
    }

    return EUNDEV;  /* unknown device */
}

LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev)
{
	return 0;
}
