/*
 * endvdi.S - Last source of VDI
 *
 * Copyright (c) 2016 by the EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

        .bss

// This label is referred by the OSHEADER
        .globl  __endvdibss
__endvdibss:
