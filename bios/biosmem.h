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

#ifndef _BIOS_H
#define _BIOS_H

#include "portab.h"

void bmem_init(void);
void bmem_close(void);
void *balloc(long size);

#endif /* _BIOS_H */
