/*
 * dana2.S - AlphaSmart Dana specific assembler functions
 *
 * Copyright (C) 2013-2021 The EmuTOS development team
 *
 * Authors:
 *  DG   David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#define PFDATA 0xfffff429
#define PKDATA 0xfffff441
#define TSTAT1 0xfffff60a
#define URX1   0xfffff904
#define UTX1   0xfffff906

	.globl _dana_int_1
_dana_int_1:
	movml %d0-%d2/%a0-%a1, -(%sp)
	jsr _dana_ikbd_interrupt
	movml (%sp)+, %d0-%d2/%a0-%a1
	rte

	.globl _dana_int_4
_dana_int_4:
	movml %d0-%d2/%a0-%a1, -(%sp)

loop:
	movw URX1, %d0
	btstl #13, %d0
	beqb nodata

	movw %d0, -(%sp)
	jsr _dana_rs232_interrupt
	addql #2, %sp

	bra loop

nodata:
	movml (%sp)+, %d0-%d2/%a0-%a1
	rte

	.globl _dana_int_6
_dana_int_6:
	movml %d0/%a0, -(%sp)

	pea _dana_int_6_ret
	movw %sr, d0
	movw %d0, -(%sp)

	.extern _vector_5ms
	movl _vector_5ms, %a0
	jmp (%a0)
	| eventually returns to...
_dana_int_6_ret:
	andi #0, TSTAT1

	movml (%sp)+, %d0/%a0
	rte

	.globl _dana_set_lcd_contrast
_dana_set_lcd_contrast:
	moveb (5,%sp), %d0

	| Start sequence

	andib #~0x40, PFDATA
	orib #0x40, PKDATA
	.rept 8
		orib #0x08, PKDATA
	.endr
	andib #~0x08, PKDATA
	andib #~0x40, PKDATA

	moveb #0x80, %d1			| bit marker
1:
	andw %d1, %d0				| test bit
	beqb bit_is_zero
	orib #0x40, PFDATA		| set bit
	bra 2f
bit_is_zero:
	andib #~0x40, PFDATA		| clear bit
2:
	.rep 8
		orib #0x40, PKDATA
	.endr
	andib #~0x40, PKDATA

	lsrb #1, %d1
	bneb 1b

	.rept 8
		orib #0x08, PKDATA
	.endr
	andib #~0x08, PKDATA
	andib #~0x40, PKDATA

	rts

