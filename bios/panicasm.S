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
        move.w  #0x2700, sr
        movem.l d0-d7, _proc_dregs
        movem.l a0-a7, _proc_aregs
        clr.l   _proc_enum
        move    usp, a0
        move.l  a0, _proc_usp
        lea     _proc_stk, a0
        move.l  a7, a1
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  #0x12345678, _proc_lives
        // at that time, we did not use the stack, to we may jump to the 
        // C routine directly in kprint.c
        jmp     _dopanic

/* void halt(void)
 * simply halt the machine.
 */
_halt:
  // first try 'STOP'
  stop    #0x2700
  // a normal 680x0 CPU would have halted already, but just in case:
forever:
  bra     forever


