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

#ifndef DISK_H
#define DISK_H

#include "portab.h"
 
/* xbios functions */

extern LONG DMAread(LONG sector, WORD count, LONG buf, WORD dev);
extern LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD dev);

/* partition detection */

void disk_init(void);
int atari_partition(int bdev);

#endif /* DISK_H */
