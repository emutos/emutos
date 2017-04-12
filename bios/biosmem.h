/*
 *  biosmem.h - dumb bios-level memory management
 *
 * Copyright (C) 2002-2017 The EmuTOS development team
 *
 * Authors:
 *  LVL    Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOSMEM_H
#define BIOSMEM_H

#include "memdefs.h"

/*
 * sizes of disk buffers allocated via balloc_stram()
 */
#define DSKBUF_SECS     2
#define DSKBUF_SIZE     (DSKBUF_SECS * SECTOR_SIZE) /* pointed to by dskbufp */
#define FRB_SIZE        (64 * 1024UL)       /* pointed to by _FRB cookie */
#define FRB_SECS        (FRB_SIZE / SECTOR_SIZE)


/* Prototypes */
void bmem_init(void);
UBYTE *balloc_stram(ULONG size, BOOL top);

/* BIOS function */

void getmpb(MPB *mpb);

#endif /* BIOSMEM_H */
