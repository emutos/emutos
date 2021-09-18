.text
.cpu 68000

/* Warning: position-independent code only! */

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
	movl (%sp, 12), %d2     /* debug routine */
	lea stack_top, %sp

	/* Copy block list into safe memory. */

	lea blocks, %a1
1:
	movl (%a0)+, (%a1)+
	subq #1, %d0
	bneb 1b
	movl #0, (%a1)+

	/* Enable data writes. */

	andw #0x1fff, 0xfffff116

	/* Set up for copying. */

	lea blocks, %a0
	movl (%a0)+, %a1		/* a0 = pointer to pointer to current block */
	addl #32*1024, %a1		/* a1 = destination address (after first block) */
	movw #0x2700, %sr		/* interrupts off */

loop:
	movl (%a0)+, %a2		/* a2 = source block */

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
	movl (%a0), 0x80		/* write to trap #0 */
	trap #0

stack:
	ds 128
stack_top:

blocks:
	

