/*
 * deskstart.S - assembler startup code of the desktop
 *
 * Copyright (C) 1987 Digital Research Inc.
 *               2001 Lineo, Inc.
 *               2002-2024 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

/* Desktop stack use can be estimated using CONF_DEBUG_DESK_STACK. */
#define DESKSTACKSIZE   2560    /* in bytes: must be multiple of 4 */

        .globl  _deskstart
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
        jge     fill
#endif

restart:
        jsr     _deskmain               // Run the desktop
        tst.w   d0
        jeq     restart                 // handle read of .INF file

        clr.w   -(sp)
        trap    #1                      // Pterm

.bss

// Space for the Stack:
_deskstackbottom:
        .ds.b   DESKSTACKSIZE
deskustack:
