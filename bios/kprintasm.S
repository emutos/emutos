/*
 *  kprintasm.S - Native print asm routines
 *
 * Copyright (c) 2002-2016 The EmuTOS development team:
 *
 * Authors:
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#if STONX_NATIVE_PRINT

        .globl  _stonx_kprintf_init
        .globl  _printout_stonx

        .extern _stonx_kprintf_available

// ===========================================================================
// ==== TEXT segment =========================================================
// ===========================================================================

        .text

/*
 * void stonx_kprintf_init(void)
 * sets variable _stonx_kprintf_available:
 *   0: no native print detected
 *   1: STonX native print detected
 *
 * Calling kprintf should only be done after:
 *   1/ the bss is cleared
 *   2/ this function is called
 * (calling kprintf before 1 may crash the machine with unexpected
 * exceptions; calling kprintf after 1/ and before 2/ will do nothing).
 *
 * Note: It is not possible to actually detect that something is printed
 * on the emulator. However the probability is high that emulators which
 * do not raise exceptions for these special opcodes also implement the
 * expected behaviour.
 */

        .equ vec_illegal, 0x10       // illegal exception vector
        .equ vec_linea, 0x28         // LineA exception vector

_stonx_kprintf_init:
        // first, try STonX
        move.l  vec_linea,a0
        move.l  sp,a1
        move.l  #fail_stonx,vec_linea
        pea     msg_empty
        sub.l   #4,sp
        .dc.w   0xa0ff
        .dc.l   0
        move.l  a1,sp
        move.l  a0,vec_linea
        move.w  #1,_stonx_kprintf_available
        rts
fail_stonx:
        move.l  a1,sp
        move.l  a0,vec_linea
        clr.w   _stonx_kprintf_available
        rts

msg_empty:
        .ascii  "\0"
        .even


// ==== STonX - Native print routine for debugging ====================
_printout_stonx:
        .dc.w   0xa0ff     // Jump to native execution
        .dc.l   0          // Printing subroutine
        rts

#endif /* STONX_NATIVE_PRINT */
