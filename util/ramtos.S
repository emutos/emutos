/*
 * ramtos.S - embedded ramtos.img
 *
 * Copyright (c) 2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

        .globl  _ramtos
        .globl  _end_ramtos

        SECTION_RODATA

_ramtos:
        .incbin "ramtos.img"
        .balign 4
_end_ramtos:
