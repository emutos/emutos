/*
 * processor.S - Check on which processor we run
 *
 * Copyright (c) 1985 by Digital Research, Inc.
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"

// ==== References ===========================================================

        .global _frame_set              // Set long or shart frame

        .xdef   _longframe              // If not 0, use long stack frames


/*
 * _frame_set - Processor check and set _longframe
 *
 * First of all:  Are we executing on a 68000 or a 68010?
 * We find out by taking over the illegal instruction trap and then
 * executing a 'MOVE CCR,D0' which is not implemented on the 68000.
 * We will set a flag for use later on as we enter and exit the BDOS.
 * We also will set up a handler on the 68010 to convert 'MOVE SR,...'
 * to 'MOVE CCR,...' at execution time, so that we can support one
 * release of 68XXX software.
 */
 
_frame_set:
        
        movem.l d0-d7/a0-a6, -(sp)      // save registers
        move.l	sp, ii_stksav		// save the current sp

        clr.w	_longframe		// pre-clear processor flag

	move.l	0x10, d0                // Get old illegal instruction vector
	move.l	d0, ii_save             // save old vector
	move.l	d0, ii_old              // save it for use in new ii_handler

	move.l	#ii_tmp, 0x10		// exchange illegal instruction vector


//        jmp     ii_tmp
	move	ccr, d0			// So, can we execute this?

	move.w	#1,_longframe		// yes, flag that we're on a 68010
	move.l	#ii_handler, ii_save	// and set in our new handler


/*
 * ii_tmp - Temporary illegal instruction handler
 */

ii_tmp:
	move.l	ii_save, 0x10           // set new or old illegal vector

	movea.l	ii_stksav, sp		// restore sp
        movem.l (sp)+, d0-d7/a0-a6      // save registers

	rts


/*
 * ii_tmp - Temporary illegal instruction handler
 */

ii_handler:
	movem.l	d0/a0,-(sp)		// saving d0 and a0 while we test
	movea.l	10(sp),a0		// the offending address
	move.w	(a0),d0			// the offending instruction
	andi.w	#0xffc0,d0		// isolate move from SR opcode
	cmpi.w	#0x40c0,d0		// we don't care where "To" was
	bne	not_SR			// Not a MOVE SR,XX instruction

	ori.w	#0x200,(a0)		// make it MOVE CCR,XX
	movem.l	(sp)+,d0/a0
	rte

not_SR:
	movem.l	(sp)+,d0/a0
	move.l	ii_old,-(sp)		// let OEM handle it
	rts

// ==== Variables ============================================================
 
	.bss
	.even

ii_save:				// routine to set up as illegal
	.ds.l	1			// instruction handler
ii_old:
	.ds.l	1
ii_stksav:
	.ds.l	1			// at osinit: save sp here


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end




