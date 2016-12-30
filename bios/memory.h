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

#include "portab.h"

#if CONF_WITH_ADVANCED_CPU && !defined(__mcoldfire__)

BOOL detect_32bit_address_bus(void);

#endif /* CONF_WITH_ADVANCED_CPU && !defined(__mcoldfire__) */

#if CONF_WITH_TT_MMU

void init_tt_fastram(void);

#endif /* CONF_WITH_TT_MMU */

#endif /* MEMORY_H */
