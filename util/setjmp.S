/*
 * setjmp.S - EmuTOS own copy of an ANSI standard feature
 *
 * Copyright (C) 2002-2015 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

/*
 * structure of jmp_buf:
 *   0: return address
 *   4: twelve registers d2-d7/a2-a7
 */
        .globl  _setjmp
        .globl  _longjmp

        .text

_setjmp:
        move.l  4(sp),a0
        move.l  (sp),(a0)+
        movem.l d2-d7/a2-a7,(a0)
        moveq   #0,d0
        rts

_longjmp:
        move.l  4(sp),a0
        move.w  8(sp),d0
        beq     nonzero
        moveq   #1,d0
nonzero:
        move.l  (a0)+,d1
        movem.l (a0),d2-d7/a2-a7
        move.l  d1,(sp)
        rts
