/*
 * danaboot.S - AlphaSmart Dana decompressing RAM booter
 *
 * Copyright (C) 2021 David Given
 *
 * Authors:
 *  DG    David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

// Warning: position-independent code only!

.cpu 68000

entry:
	lea entry, %sp

	lea entry, %a0
	addl relocate_addr, %a0

	pea data_start
	jmp (%a0)

relocate_addr:
	dc.l relocate - entry
	
data_start:
.incbin "emutos.img"
data_end:

	.align 2
relocate:
	movl (%sp)+, a2			// source
	lea 0x2140, %a1			// destination
	movw #(data_end-data_start)/4 - 1, %d0 // number of words
1:
	movl (%a2)+, (%a1)+
	dbf %d0, 1b
	jmp 0x2140

