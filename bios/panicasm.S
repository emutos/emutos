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

#include "asmdefs.h"


        .text
  
        .globl  _panic
        .globl  _halt


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
#ifdef __mcoldfire__
        move.l  a0,_proc_dregs+32
        lea     _proc_dregs,a0
        movem.l d0-d7,(a0)
        movem.l a1-a7,36(a0)
#else
        movem.l d0-d7/a0-a7,_proc_dregs
#endif
        move.l  usp,a0
        move.l  a0,_proc_usp
        // tell dopanic() that there are arguments following
        clr.l   _proc_enum
        // dump 16 words from the stack
        movem.l (sp),d0-d7
#ifdef __mcoldfire__
        lea     _proc_stk,a0
        movem.l d0-d7,(a0)
        // magic value
        lea     _proc_lives,a0
        move.l  #0x12345678,(a0)
#else
        movem.l d0-d7,_proc_stk
        // magic value
        move.l  #0x12345678,_proc_lives
#endif
        // at that time, we did not alter the stack, to we can jump to the 
        // C routine directly in kprint.c
        jmp     _dopanic

/* void halt(void)
 * simply halt the machine.
 */
_halt:
#if !USE_STOP_INSN_TO_FREE_HOST_CPU
        // disable the interrupts
        move.w  #0x2700,sr  
#endif
haltloop:
        // endless loop
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        stop    #0x2700
        // only an NMI could theroretically wake up the CPU here
#endif
        bra.s   haltloop
