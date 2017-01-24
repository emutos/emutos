/*
 * bootparams.h - ramtos boot parameters
 *
 * Copyright (C) 2011-2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IDE_H
#define IDE_H

#include "portab.h"

#ifdef MACHINE_AMIGA

/*
 * Alt-RAM regions can't be detected from hardware on warm boot. So the ramtos
 * loaders have to guess Alt-RAM regions detected by the previous OS, then
 * forward them to ramtos through this list.
 */

typedef struct
{
    UBYTE *address;
    ULONG size;
} ALTRAM_REGION;

#define MAX_ALTRAM_REGIONS 16 /* Arbitrary limit */

extern const ALTRAM_REGION altram_regions[MAX_ALTRAM_REGIONS];

#endif /* MACHINE_AMIGA */

#endif /* IDE_H */
