/*
 * palmboot.S : PalmOS boot loader (bootstrap code)
 *
 * This is part of the danarun boot loader. It is used to reassemble the image
 * after loading into chunks on the PalmOS heap and takes care of launching the
 * image.  It will spit various useful numbers to the serial port as it runs.
 *
 * Copyright (C) 2021 David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

.text
.cpu 68000

/* Warning: position-independent code only! */

TX = 0xfffff906
UBAUD1 = 0xfffff902
USTCNT1 = 0xfffff900
PESEL = 0xfffff423
NIPR1 = 0xfffff90a

/* Concatenates all blocks in the list into one, following the first
 * block. All blocks must be in pointer order.
 *
 * On entry:
 *   sp+8 = pointer to list of blocks
 *   sp+4 = number of blocks
 *   sp+0 = return address
 */

entry:
	movl (%sp, 4), %a0		/* a0 = block address array */
	movl (%sp, 8), %d0		/* d0 = number of blocks */
	lea stack_top, %sp

	/* Copy block list into safe memory. */

	lea blocks, %a1
1:
	movl (%a0)+, (%a1)+
	subq #1, %d0
	bneb 1b
	movl #0, (%a1)+

	/* Initialise serial port. */

	andib #0xf, PESEL
	movw #0x226, UBAUD1
	movb #0xe8, TX
	movw #0xe100, USTCNT1
	movw #0, NIPR1

	/* Disable read-only memory. */

	andw #0x1fff, 0xfffff116

	/* Set up for copying. */

	lea blocks, %a0
	movl (%a0)+, %a1		/* a0 = pointer to pointer to current block */
	addl #32*1024, %a1		/* a1 = destination address (after first block) */
	movw #0x2700, %sr		/* interrupts off */

loop:
	movl (%a0)+, %a2		/* a2 = source block */

	movl %a2, %d7
	bsr hex32
	bsr space
	movl %a1, %d7
	bsr hex32
	bsr space
	bsr nl

	cmpl #0, %a2
	beqb finished

	movl #(32*1024)/4, %d1
1:
	movl (%a2)+, (%a1)+
	subl #1, %d1
	bneb 1b
	brab loop

finished:
	lea blocks, %a0
	
	movl (%a0), %d7
	bsr hex32
	bsr nl

	movl (%a0), 0x80		/* write to trap #0 */
	trap #0

hex32:
	movl %d7, %d5
	swap %d7
	lsrl #8, %d7
	bsr hex8
	movl %d5, %d7
	swap %d7
	bsr hex8
	movl %d5, %d7
	lsrl #8, %d7
	bsr hex8
	movl %d5, %d7
hex8:
	movb %d7, %d6
	lsr #4, %d7
	bsr hex4
	movb %d6, %d7
hex4:
	andb #0x0f, %d7
	addb #0x30, %d7
	cmpb #0x39, %d7
	blsb 1f
	addb #0x07, %d7
1:
putc:
	btstb #13-8, TX
	beqb putc
	movb %d7, TX+1
	rts

space:
	movb #32, %d7
	bra putc

nl:
	movb #13, %d7
	bsr putc
	movb #10, %d7
	bra putc
	
stack:
	ds 128
stack_top:

blocks:
	

