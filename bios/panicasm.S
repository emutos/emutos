/*
 * panicasm.S - support routines for panic debug messages
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"


        .text
  
        .global _panic
        .global _halt


/*
 * void panic(const char *fmt, ...);
 * save the registers, state and stack in 0x380, then
 * print the message on the console, and finally halt.
 * note: the stack dump will contain the attributes.
 * the exception number proc_enum will be zero.
 */
 
_panic:
        // supervisor mode, no interrupts
        move.w  #0x2700,sr
        // dump the registers including USP
        movem.l d0-d7/a0-a7,_proc_dregs
        move    usp,a0
        move.l  a0,_proc_usp
        // tell dopanic() that there are arguments following
        clr.l   _proc_enum
        // dump 16 words from the stack
        movem.l (sp),d0-d7
        movem.l d0-d7,_proc_stk
        // magic value
        move.l  #0x12345678,_proc_lives
        // at that time, we did not alter the stack, to we can jump to the 
        // C routine directly in kprint.c
        jmp     _dopanic

/* void halt(void)
 * simply halt the machine.
 */
_halt:
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        // first try 'STOP'
        stop    #0x2700
        // a normal 680x0 CPU would have halted already, but just in case:
#endif
        move.w  #0x2700,sr
forever:
        bra     forever


