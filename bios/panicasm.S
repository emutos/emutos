/*
 * panicasm.S - support routines for panic debug messages
 *
 * Copyright (c) 2001-2016 The EmuTOS development team
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
        .globl  _kill_program
        .globl  _warm_reset
        .globl  _cold_reset


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
        // at this time, we have not altered the stack, so we can jump to the
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

/* void kill_program(void)
 * try to terminate via Pterm(); if Pterm() returns here (!),
 * we do a warm reset
 */
_kill_program:
        move.w  #-1,-(sp)               // error code for program killed by OS
        move.w  #0x4C,-(sp)             // Pterm
        trap    #1
        // drop thru into warm reset

/* void warm_reset(void)
 * Restart this OS
 */
_warm_reset:
        jsr     _flush_data_cache
        jmp     _main

/* void cold_reset(void)
 * Invalidate the RAM configuration and reset the computer to the ROM OS.
 * It is important to flush the data cache so the values of the magic variables
 * are really pushed to the RAM. Next OS may use different cache settings.
 */
_cold_reset:
        clr.l   resvalid        // Don't allow jump to reset vector in startup.S
        clr.l   resvector
#if CONF_DETECT_FIRST_BOOT_WITHOUT_MEMCONF
        clr.l   _warm_magic     // Next boot will be a first boot
#endif
#ifdef MACHINE_AMIGA
        clr.l   0x420           // Invalidate memvalid
        jsr     _flush_data_cache
        move.l  0x00fc0004,a0   // Get the entry adress from offset 4 in the ROM
        reset                   // The entry usually points just after reset
        jmp     (a0)
#elif defined(MACHINE_FIREBEE)
        // Do not invalidate memvalid, to keep the RAM settings
        jsr     _flush_data_cache
        move.l  0x00e00004,a0   // Get the entry adress from offset 4 in the ROM
        jmp     (a0)
#elif CONF_ATARI_HARDWARE
        clr.l   0x420           // Invalidate memvalid
        jsr     _flush_data_cache
        move.l  0x004,a0        // The 2 first longs are mapped to the ROM
        jmp     (a0)
#else
        jsr     _flush_data_cache
        // We don't know how to cold reset, so just warm reset this OS
        jmp     _main
#endif
