/*
 *  biosmem.h - dumb bios-level memory management
 *
 * Copyright (c) 2002 by
 *
 * Authors:
 *  LVL    Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BIOSMEM_H
#define _BIOSMEM_H

#include "portab.h"
#include "bios.h"

void bmem_init(void);
void *balloc(long size);

/* BIOS function */

void getmpb(MPB *mpb);

#endif /* _BIOSMEM_H */
