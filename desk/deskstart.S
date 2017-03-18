/*
 * deskstart.S - assembler startup code of the desktop
 *
 * Copyright (C) 1987 Digital Research Inc.
 *               2001 Lineo, Inc.
 *               2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#define DESKSTACKSIZE   2048    /* in bytes: must be multiple of 4 */

        .globl  _deskstart
        .globl  _gem
#if CONF_DEBUG_DESK_STACK
        .globl  _deskstackbottom
#endif

        .extern _deskmain

        .text

_deskstart:

        movea.l a7,a5
        movea.l #deskustack,a7
        movea.l 4(a5),a5

        move.l  0xC(a5),d0
        add.l   0x14(a5),d0
        add.l   0x1C(a5),d0
        add.l   #0x100,d0
        move.l  d0,-(a7)
        move.l  a5,-(a7)
        clr.w   -(a7)
        move.w  #0x4A,-(a7)
        trap    #1                      // Mshrink
        lea     12(a7),a7

#if CONF_DEBUG_DESK_STACK
        move.l  #STACK_MARKER,d0        // fill stack with marker bytes
        lea     _deskstackbottom,a0
        lea     deskustack,a1
fill:
        move.l  d0,(a0)+
        cmpa.l  a0,a1
        bge.b   fill
#endif

        jsr     _deskmain               // Run the desktop

        clr.w   -(sp)
        trap    #1                      // Pterm


_gem:
        move.l  4(sp),d1
        move.w  #0xc8,d0
//
// GCC only uses d0-d1/a0-a1 for work registers, so expects d2/a2 to
// be preserved across the trap.  However, someone could hook into
// the trap and not preserve them, so we must do it ourselves.
//
// Note: when saving/restoring two registers on the stack, using move.l
// instead of movem.l takes the same code space, and is faster for both
// 68000 & 68030, as well as being ColdFire-compatible.
//
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        trap    #2
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts


.bss



// Space for the Stack:
_deskstackbottom:
        .ds.b   DESKSTACKSIZE
deskustack:
