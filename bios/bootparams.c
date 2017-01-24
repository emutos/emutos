/*
 * bootparams.c - ramtos boot parameters
 *
 * Copyright (C) 2017 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "bootparams.h"

#if EMUTOS_LIVES_IN_RAM

/*
 * All the variables defined here will be patched by ramtos loaders.
 * They need to be initialized to force them to live in the TEXT segment.
 * They also need to be defined in a source file different from the one
 * where they are used, otherwise GCC might be too smart by guessing their
 * value without trying to read the actual memory.
 */

#ifdef MACHINE_AMIGA

const ALTRAM_REGION altram_regions[MAX_ALTRAM_REGIONS] = { { NULL, 0 } };

#endif /* MACHINE_AMIGA */

#endif /* EMUTOS_LIVES_IN_RAM */
