/*
 * EmuCON2 assembler routines
 *
 * Copyright (c) 2013 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifdef STANDALONE_CONSOLE
#define _coma_start _start
#else
#include "asmdefs.h"
#endif

        .globl  _coma_start
        .globl  _getwh
        .globl  _cmdmain

        .text
_coma_start:
        move.l  4(sp),a5                // a5 -> basepage
        lea     stack,sp
        move.l  12(a5),d0               // calc memory required
        add.l   20(a5),d0
        add.l   28(a5),d0
        addi.l  #256,d0
        move.l  d0,-(sp)                // Mshrink()
        move.l  a5,-(sp)
        move.w  #0,-(sp)
        move.w  #74,-(sp)
        trap    #1
        lea     12(sp),sp

        jsr     _cmdmain

        clr.l   -(sp)                   // Pterm0()
        trap    #1

_getwh:
        .dc.w   0xa000                  // lineA init
        move.l  -44(a0),d0              // v_cel_mx/v_cel_my
        rts

        .bss

        .ds.l   1024
stack:
