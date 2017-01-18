/*
 * memory.h - Memory functions
 *
 * Copyright (C) 2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MEMORY_H
#define MEMORY_H

/* Bit definitions for meminit_flags */

#define MEMINIT_BIT_FIRST_BOOT 0
/* If this bit is set, this means it is the first time that this EmuTOS
 * instance is run. This is always true for a cold boot from ROM. This is
 * always false for a reset. If EmuTOS lives in RAM, it is only true the first
 * time it is run. */

#ifndef ASM_SOURCE

#include "portab.h"

#if CONF_WITH_ADVANCED_CPU && !defined(__mcoldfire__)

BOOL detect_32bit_address_bus(void);

#endif /* CONF_WITH_ADVANCED_CPU && !defined(__mcoldfire__) */

#if CONF_WITH_TT_MMU

void set_ttram_refresh_rate(void);

#endif /* CONF_WITH_TT_MMU */

void ttram_detect(void);

#if CONF_WITH_ALT_RAM

void altram_init(void);

#endif /* CONF_WITH_ALT_RAM */

/* These flags will be set up early by meminit() */
extern UBYTE meminit_flags;
#define MEMINIT_FIRST_BOOT (1 << MEMINIT_BIT_FIRST_BOOT)

#endif /* ASM_SOURCE */

#endif /* MEMORY_H */
