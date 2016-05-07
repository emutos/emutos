/*
 * endgem.S - Last source of GEM AES/Desktop
 *
 * Copyright (c) 2016 by the EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

        .bss

// This label is referred by the GEM_MUPB structure
        .globl  __endgembss
__endgembss:
