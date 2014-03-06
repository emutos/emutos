/*
 * disk.h - disk routines
 *
 * Copyright (c) 2001-2014 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef DISK_H
#define DISK_H

#include "portab.h"

/* physical disk functions */

LONG disk_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen);
LONG disk_get_capacity(UWORD unit, ULONG *blocks, ULONG *blocksize);
LONG disk_rw(UWORD unit, UWORD rw, ULONG sector, UWORD count, void *buf);

/* xbios functions */

extern LONG DMAread(LONG sector, WORD count, LONG buf, WORD major);
extern LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD major);

/* partition detection */

void disk_init_all(void);
void disk_rescan(int major);
void byteswap(UBYTE *buffer, ULONG size);

#endif /* DISK_H */
