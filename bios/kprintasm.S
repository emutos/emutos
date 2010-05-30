/*
 *  kprintasm.S - Native print asm routines
 *
 * Copyright (c) 2002 by Authors:
 *
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

        .global kprintf_init
        .global kprintf

        .global _printout_stonx
        
        .xdef   _kprintf
        .xdef   _native_print_kind

// ===========================================================================
// ==== TEXT segment =========================================================
// ===========================================================================

        .text

/*
 * void kprintf_init(void)
 * sets variable _native_print_kind:
 *   0: no native print detected
 *   1: STonX
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

#if STONX_NATIVE_PRINT + DETECT_NATIVE_PRINT > 1
#warning "please use only one STONX_ "
#warning "and DETECT_NATIVE_PRINT options"
#endif

kprintf_init:
#if STONX_NATIVE_PRINT
        move.w  #1,_native_print_kind
        rts
#endif
#if DETECT_NATIVE_PRINT
        // first, try STonX
        move.l  vec_linea,a0
        move.l  sp,a1
        move.l  #fail_stonx,vec_linea
        pea     msg_empty
        sub.l   #4,sp
        dc.w    0xa0ff
        dc.l    0
        move.l  a1,sp
        move.l  a0,vec_linea
        move.w  #1,_native_print_kind
        rts
fail_stonx:
        move.l  a1,sp
        move.l  a0,vec_linea
#endif  /* DETECT_NATIVE_PRINT */
        clr.w   _native_print_kind
        rts
        
msg_empty:
        .ascii  "\0"
        .even


// ==== STonX - Native print routine for debugging ====================
_printout_stonx:
        dc.w    0xa0ff     // Jump to native execution
        dc.l    0          // Printing subroutine
        rts


// ==== ASM glue for kprintf =================================================
kprintf:
        move.l  (sp)+,kprintfsave+16
#ifdef __mcoldfire__
        move.l  d0,kprintfsave
        move.l  d1,kprintfsave+4
        move.l  a0,kprintfsave+8
        move.l  a1,kprintfsave+12
#else
        movem.l d0-d1/a0-a1,kprintfsave
#endif
        jsr     _kprintf
#ifdef __mcoldfire__
        move.l  kprintfsave,d0
        move.l  kprintfsave+4,d1
        move.l  kprintfsave+8,a0
        move.l  kprintfsave+12,a1
#else
        movem.l kprintfsave,d0-d1/a0-a1
#endif
        move.l  kprintfsave+16,-(sp)
        rts

// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================
        .bss    
        
kprintfsave:    
        ds.l    5       // save zone for kprintf

