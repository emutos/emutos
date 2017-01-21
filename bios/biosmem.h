/*
 *  biosmem.h - dumb bios-level memory management
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
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

/* Prototypes */
void bmem_init(void);
UBYTE *balloc(ULONG size);

/* BIOS function */

void getmpb(MPB *mpb);

#endif /* BIOSMEM_H */
