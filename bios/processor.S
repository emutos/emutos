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

	.global _processor_init

        .xdef   _longframe              // If not 0, use long stack frames
	.global _mcpu
	.global _fputype

	.xdef	_detect_cpu		// in detect.S, taken from FreeMiNT
	.xdef	_detect_fpu

/*
 * void processor_init(void) - sets mcpu and fputype.
 * if the cpu is not a bare 68000, installs an illegal exception
 * handler to convert 'MOVE SR,...' to 'MOVE CCR,...' at execution time
 */

_processor_init:
	jsr	_detect_cpu
	move.l	d0,_mcpu
	beq	m68000
// this is a 68010 or later
	move.w	#1,_longframe
	move.l	0x10,ii_old
	move.l	#ii_handler,0x10
m68000:
	jsr	_detect_fpu
	move.l	d0,_fputype
	rts



/*
 * ii_handler - illegal instruction handler 
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

ii_old:
	ds.l	1

_mcpu:
	ds.l	1
_fputype:
	ds.l	1


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end




