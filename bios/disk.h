/*
 * disk.h - disk routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DISK_H
#define _DISK_H

#include "portab.h"
 
/* xbios functions */

extern LONG DMAread(LONG sector, WORD count, LONG buf, WORD dev);
extern LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev);

/* XHDI functions */

extern LONG XHInqTarget(UWORD major, UWORD minor, ULONG *blocksize,
                        ULONG *device_flags, char *product_name);
extern LONG XHGetCapacity(UWORD major, UWORD minor, ULONG *blocks,
                          ULONG *blocksize);

#define XHGETCAPACITY    14

/* partition detection */

int atari_partition(int bdev);

#endif /* _DISK_H */
