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

#define TSTAT1 0xfffff60a
#define UTX1 0xfffff906

	.globl _dana_int_6
_dana_int_6:
	lea -8(%sp), %sp
	movml %d0/%a0, (%sp)

	pea _dana_int_6_ret
	movw %sr, d0
	movw %d0, -(%sp)

	.extern _vector_5ms
	movl _vector_5ms, %a0
	jmp (%a0)
	| eventually returns to...
_dana_int_6_ret:
	andi #0, TSTAT1

	movml (%sp), %d0/%a0
	lea 8(%sp), %sp
	rte

