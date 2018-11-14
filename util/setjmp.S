/*
 * setjmp.S - EmuTOS's own copy of an ANSI standard feature
 *
 * Copyright (C) 2002-2018 The EmuTOS development team
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
 *
 * Note that even if the return address of setjmp() is on the stack,
 * it must be saved separately because it will be immediately popped out
 * by the caller just after the call.
 */
        .globl  _setjmp
        .globl  _longjmp

        .text

_setjmp:
        move.l  4(sp),a0                // jmp_buf
        move.l  (sp),(a0)+              // save setjmp() return address
        movem.l d2-d7/a2-a7,(a0)        // save setjmp() caller registers
        moveq   #0,d0                   // return from setjmp()
        rts

_longjmp:
        move.l  4(sp),a0                // jmp_buf
        move.w  8(sp),d0                // requested setjmp() return value
        jne     nonzero
        moveq   #1,d0                   // if was 0, force it to 1
nonzero:
        move.l  (a0)+,d1                // get setjmp() return address
        movem.l (a0),d2-d7/a2-a7        // restore setjmp() caller registers
        move.l  d1,(sp)                 // force return address
        rts                             // return to setjmp() caller
