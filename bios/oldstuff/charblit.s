| ===========================================================================
| ==== charblit.s - very fast blitting of 8 bit width monospaced chars ======
| ===========================================================================

|	   Jim Shillington, Bell Northern Research   5/15/85


| ===========================================================================
|	This module contains the mono-spaced text blt for displays
|	with a consecutive video plane memory scheme.  It is heavily
|	optimized for the addressing modes of the 68000.  This code
|	can easily be modified to also support interleaved video
|	planes, like for the Atari.
| ===========================================================================



| ==== Public Routines ======================================================

	.text
	.global	_charblit


| ==== External Variables ===================================================

	.xdef	_DESTX		| x-coord of upper left corner of text rect
	.xdef	_DESTY		| y-coord of upper left corner of text rect
	.xdef	_DELY		| character height in scans
	.xdef	_FBASE		| base address of the font definition
	.xdef	_FWIDTH 	| width of the font definition
	.xdef	_WRT_MODE	| writing mode flag
	.xdef	_INTIN		| base address of INTIN array
	.xdef	_v_bas_ad	| base address of screen
	.xdef	_v_lin_wr	| # of bytes in a single scan line
	.xdef	_TEXT_FG	| foreground color for text
	.xdef	_v_planes	| # of video planes


| ==== External routines ====================================================

	.xdef	concat



| ==== Local constants ======================================================

	.equ	v_pl_dspl,	0x10000	| # of bytes between video planes



| ===========================================================================
|
|	_charblit
|
|	This routine performs text blt's on mono-spaced, 8-bit
|	wide fonts.  They can be arbitrarily tall.  This blt is
|	always byte-aligned and does not support clipping.
|
|	Inputs:
|	   _DESTX, _DESTY - x and y-coordinates of the character
|			    cell
|	   _DELY - character cell height
|	   _FBASE - base address of font definition
|	   _FWIDTH - width of font definition
|	   _WRT_MODE - current writing mode
|	   _TEXT_FG - text foreground color
|	   _v_bas_ad - base address of first video page
|	   _v_lin_wr - # of bytes in a single scan line
|	   _v_planes - # of video planes
|	   _character - character
|
|	Outputs:
|	   d0 - 0 = text blt could not be performed
|		1 = text blt was successfully performed
|
|	Registers Modified:
|
|	Author/Date:
|	   Jim Shillington, Bell Northern Research   5/15/85
|
|	Revisions:		None
|
| ===========================================================================

_charblit:
	movem.l d3-d7/a3-a6,-(a7)	| save registers used by C

	move.w	_DESTX,d0	| get x-coordinate of destination
	move.w	_DESTY,d1	| get y-coordinate of destination
	move.w	_DELY,d3	| get character cell height
	move.w	#1, d2		| get # of characters in string

|	Compute the starting address in the screen for the text extent
|	rectangle.  This is the address of the upper left corner of the
|	rectangle.

	jsr	concat		| compute offset into buffer
	tst.w	d0		| on an odd byte boundary?  (d0 = 8)
	beq	word_ok 	| nope - on an even byte boundary  (d0 = 0)
	addq.w	#1,d1		| yes - increment the buffer offset
word_ok:
	movea.l _v_bas_ad,a2	| get base address of the screen
	adda.l	d1,a2		| compute starting address within screen


|	Set up the rest of the parameters needed to draw the character

	movea.l _FBASE,a1	| get base address of font definition
	movea.b _character,a0	| get the character as byte value
	move.w	_v_planes,d0	| get # of planes
	subq.w	#1,d0		| convert it to counter for dbra loop
	move.w	_FWIDTH,d4	| get width of font definition
	move.w	_v_lin_wr,a3	| get # of bytes in a scan line
	suba.w	d2,a3		| get offset to start of next scan of string
	subq.w	#1,d3		| get # of scans - 1
	move.w	_TEXT_FG,d1	| get bit plane flags for text fgnd color

| ===========================================================================
|
|	At this point the register usage is as follows.
|
|	d0.w - plane loop counter
|	d1.w - text foreground color
|	d2.w - character loop counter
|	d3.w - scan loop counter
|	d4.w - width of font definition
|	d5.w - working copy of scan loop counter
|	d6.w - working copy of character loop counter
|	d7.w - index into jump table/scratch
|	a0.l - character string
|	a1.l - address of font definition
|	a2.l - starting address within current plane
|	a3.w - offset from end of text extent rectangle in one scan
|	       to start of text extent rectangle in next scan
|	a4.l - working copy of address of font definition
|	a5.l - working copy of address of character string
|	a6.l - working copy of starting address within current plane
|

|	Loop drawing all scans of every character to each video plane.
|	Each pass through the loop, set up the screen address and scan
|	loop counter and call the appropriate service routine.

mplan_lp:
	movea.l a2, a6		| get starting address for this plane
	move.w	d3, d5		| get scan loop counter
	movea.l a1, a4		| get base address of font definition
	move.w	_WRT_MODE, d7	| get the writing mode
	asl.w	#1, d7		| convert it to a word offset
	ror.w	#1, d1		| get next foreground bit plane flag
	bcc.b	bit_clr 	| write plane with 0's
	addq.w	#8, d7		| write it with 1's - use 2nd half of table
bit_clr:
	move.w	mon_mode(pc,d7.w),d7	| get offset from table to routine
	lea	mon_mode(pc,d7.w),a5	| get address of service routine
	jsr	(a5)		| call it
	adda.l	#v_pl_dspl,a2	| get starting address in next plane
	dbra	d0,mplan_lp	| loop until all planes drawn

|
|	Exit for successful completion.  Return a 1 in register d0.

blt_pass:
	moveq	#1,d0		| signal successful completion
mono_exit:
	movem.l (a7)+,d3-d7/a3-a6	| restore registers used by C
	rts


|	Exit for unsuccessful completion.  Return a 0 in register d0.

blt_fail:
	moveq	#0,d0		| signal unsuccessful completion
	bra	mono_exit	| restore registers and return



| ==== Jump Table for Drawing Text in the Different Writing Modes ===========

mon_mode:
	dc.w	mon_zer-mon_mode	| replace mode		0
	dc.w	mon_set1-mon_mode	| transparent mode	0
	dc.w	mon_xor-mon_mode	| exclusive or (xor)	0
	dc.w	mon_not1-mon_mode	| reverse transparent	0
	dc.w	mon_rep-mon_mode	| replace mode		1
	dc.w	mon_set0-mon_mode	| transparent mode	1
	dc.w	mon_xor-mon_mode	| exclusive or (xor)	1
	dc.w	mon_not0-mon_mode	| reverse transparent	1



| ===========================================================================
|
|	Replace Mode Text BLT -- Foreground Flag = 0
|
|	This routine performs a mono-spaced text blt in replace
|	mode for the case when the current plane should be written
|	with 0's to acheive the current foreground color.  This is
|	accomplished by clearing each target byte.
|
|	Inputs:
|	   d2.w - character loop count
|	   d5.w - scan loop count
|	   a3.w - offset to next scan line
|	   a6.l - starting address in plane
|
|	Outputs:	None
|
|	Registers Modified:	d5, d6, a6
|
|	Author/Date:
|	   Jim Shillington, Bell Northern Research   5/15/85
|
|	Revisions:	None
|
| ===========================================================================


|	Bump the screen pointer to the starting point for the text in the
|	next scan line.

mon_zer1:
	adda.w	a3,a6		| bump screen pointer to next scan line

mon_zer:

|	When that is done then loop to clear the next scan of the characters.

	move.b	d7,(a6)+	| clear byte for current character
	dbra	d5,mon_zer1	| loop until all scans cleared
	rts



| ===========================================================================
|
|	Replace Mode Text BLT -- Foreground Flag = 1
|
|	This routine performs a mono-spaced text blt in replace
|	mode for the case when the current plane should be written
|	with 1's to acheive the current foreground color.
|
|	Inputs:
|	   d2.w - character loop count
|	   d4.w - offset to next row in font definition
|	   d5.w - scan loop count
|	   a0.l - address of character string
|	   a4.l - address of font definition
|	   a3.w - offset to next scan line
|	   a6.l - starting address in plane
|
|	Outputs:	None
|
|	Registers Modified:	d5, d6, d7, a4, a5, a6
|
| ===========================================================================

|
|	Bump the screen pointer to the starting point for the text in the
|	next scan line and bump the font definition pointer to the start
|	of the next row of definitions.
|
mon_rep1:
	adda.w	a3,a6		| bump screen pointer to next scan line
	adda.w	d4,a4		| bump font definition pointer to next row

mon_rep:

|	writing the current scan of the character.
|	When that is done then loop to draw the next scan of the characters.

	move.w	(a0), d7	| get character code

	move.b	0(a4,d7.w),d7	| move scan byte from table to register
	not.b	d7		| invert scan byte to get reverse video
	move.b	d7,(a6)+	| move reverse video scan byte to screen

	dbra	d5,mon_rep1	| loop until all scans drawn
	rts



| ===========================================================================
|
|	Transparent Mode Text BLT -- Foreground Flag = 0
|
|	This routine performs a mono-spaced text blt in transparent
|	mode for the case when the current plane should be written
|	with 0's to acheive the current foreground color.
|
|	Inputs:
|	   d2.w - character loop count
|	   d4.w - offset to next row in font definition
|	   d5.w - scan loop count
|	   a0.l - address of character
|	   a4.l - address of font definition
|	   a3.w - offset to next scan line
|	   a6.l - starting address in plane
|
|	Outputs:	None
|
|	Registers Modified:	d5, d6, d7, a4, a5, a6
|
| ===========================================================================


|	Bump the screen pointer to the starting point for the text in the
|	next scan line and bump the font definition pointer to the start
|	of the next row of definitions.

mon_set0a:
	adda.w	a3,a6		| bump screen pointer to next scan line
	adda.w	d4,a4		| bump font definition pointer to next row
mon_set0:
|
|	writing the current scan of the character.
|	When that is done then loop to draw the next scan of the characters.
|
	move.w	(a0), d7	| get character code
	move.b	0(a4,d7.w),d7	| get current scan of character definition
	not.b	d7		| invert the bits
	and.b	d7,(a6)+	| clear all the bits that get foreground color
	dbra	d5,mon_set0a	| loop until all scans drawn
	rts



| ===========================================================================
|
|	Transparent Mode Text BLT -- Foreground Flag = 1
|
|	This routine performs a mono-spaced text blt in transparent
|	mode for the case when the current plane should be written
|	with 1's to acheive the current foreground color.
|
|	Inputs:
|	   d2.w - character loop count
|	   d4.w - offset to next row in font definition
|	   d5.w - scan loop count
|	   a0.l - address of character string
|	   a4.l - address of font definition
|	   a3.w - offset to next scan line
|	   a6.l - starting address in plane
|
|	Outputs:	None
|
|	Registers Modified:	d5, d6, d7, a1, a5, a6
|
| ===========================================================================

|
|	Bump the screen pointer to the starting point for the text in the
|	next scan line and bump the font definition pointer to the start
|	of the next row of definitions.
|
mon_set1a:
	adda.w	a3,a6		| bump screen pointer to next scan line
	adda.w	d4,a4		| bump font definition pointer to next row

mon_set1:
|	writing the current scan of the character.
|	When that is done then loop to draw the next scan of the characters.
|
	move.w	(a0), d7	| get character code
	move.b	0(a4,d7.w),d7	| get current scan of character definition
	or.b	d7,(a6)+	| set all the bits that get foreground color
	dbra	d5,mon_set1a	| loop until all scans drawn
	rts



| ===========================================================================
|
|	Reverse Transparent Mode Text BLT -- Foreground Flag = 0
|
|	This routine performs a mono-spaced text blt in reverse
|	transparent mode for the case when the current plane should
|	be written with 0's to acheive the current foreground color.
|	In this case, the bits that would normally be given the
|	background color are instead given the foreground color.
|
|	Inputs:
|	   d2.w - character loop count
|	   d4.w - offset to next row in font definition
|	   d5.w - scan loop count
|	   a0.l - address of character string
|	   a4.l - address of font definition
|	   a3.w - offset to next scan line
|	   a6.l - starting address in plane
|
|	Outputs:	None
|
|	Registers Modified:	d5, d6, d7, a4, a5, a6
|
| ===========================================================================

|
|	Bump the screen pointer to the starting point for the text in the
|	next scan line and bump the font definition pointer to the start
|	of the next row of definitions.
|
mon_not0a:
	adda.w	a3,a6		| bump screen pointer to next scan line
	adda.w	d4,a4		| bump font definition pointer to next row

mon_not0:
|	writing the current scan of the character.
|	When that is done then loop to draw the next scan of the characters.
|
	move.w	(a0), d7	| get character code
	move.b	0(a4,d7.w),d7	| get current scan of character definition
	and.b	d7,(a6)+	| give the background bits the foreground color
	dbra	d5,mon_not0a	| loop until all scans drawn
	rts



| ===========================================================================
|
|	Reverse Transparent Mode Text BLT -- Foreground Flag = 1
|
|	This routine performs a mono-spaced text blt in reverse
|	transparent mode for the case when the current plane should
|	be written with 1's to acheive the current foreground color.
|	In this case, the bits that would normally be given the
|	background color are instead given the foreground color.
|
|	Inputs:
|	   d2.w - character loop count
|	   d4.w - offset to next row in font definition
|	   d5.w - scan loop count
|	   a0.l - address of character string
|	   a4.l - address of font definition
|	   a3.w - offset to next scan line
|	   a6.l - starting address in plane
|
|	Outputs:	None
|
|	Registers Modified:	d5, d6, d7, a4, a5, a6
|
| ===========================================================================

|
|	Bump the screen pointer to the starting point for the text in the
|	next scan line and bump the font definition pointer to the start
|	of the next row of definitions.
|
mon_not1a:
	adda.w	a3,a6		| bump screen pointer to next scan line
	adda.w	d4,a4		| bump font definition pointer to next row

|	writing the current scan of the character.
|	When that is done then loop to draw the next scan of the characters.
|
mon_not1:
	move.w	(a0), d7	| get character code
	move.b	0(a4,d7.w),d7	| get current scan of character definition
	not.b	d7		| invert the bits
	or.b	d7,(a6)+	| give the background bits the foreground color
	dbra	d5,mon_not1a	| loop until all scans drawn
	rts



| ===========================================================================
|
|	Exclusive Or Mode Text BLT -- Foreground Flag = 0 or 1
|
|	This routine performs a mono-spaced text blt in exclusive
|	or mode.  The foreground color is not used in this opera-
|	tion.  The data from the text font definition is xor'ed
|	against the contents of each of the video planes.
|
|	Inputs:
|	   d2.w - character loop count
|	   d4.w - offset to next row in font definition
|	   d5.w - scan loop count
|	   a0.l - address of character
|	   a4.l - address of font definition
|	   a3.w - offset to next scan line
|	   a6.l - starting address in plane
|
|	Outputs:	None
|
|	Registers Modified:	d5, d6, d7, a4, a5, a6
|
| ===========================================================================

|
|	Bump the screen pointer to the starting point for the text in the
|	next scan line and bump the font definition pointer to the start
|	of the next row of definitions.
|
mon_xor1:
	adda.w	a3,a6		| bump screen pointer to next scan line
	adda.w	d4,a4		| bump font definition pointer to next row

mon_xor:
|	writing the current scan of the character.
|	When that is done then loop to draw the next scan of the characters.
|
	move.w	(a0), d7	| get next character code
	move.b	0(a4,d7.w),d7	| get current scan of character definition
	eor.b	d7,(a6)+	| invert all foreground bits in the plane
	dbra	d5,mon_xor1	| loop until all scans drawn
	rts



| ==== concat - convert x and y coordinates for screen =====================
|								  
|	This routine converts x and y coordinates into a physical 
|	offset to a word in the screen buffer and an index to the 
|	desired bit within that word.			     
|							     
|	input:			
|	   d0.w = x coordinate. 
|	   d1.w = y coordinate. 
|				
|	output: 					     
|	   d0.w = word index. (x mod 16)		     
|	   d1.w = physical offset -- (y*bytes_per_line)      
|					+ (x & xmask)>>xshift
|							     
|	destroys:	nothing 			     
|							     


|
|	Save the registers that get clobbered and convert the y-coordinate
|	into an offset to the start of the desired scan row.
|
concat:
	movem.w d2/d3,-(sp)	| save the registers that get clobbered
	mulu	_v_lin_wr,d1	| compute offset to scan row

|
|	Compute the bit offset into the desired word, save it, and remove
|	these bits from the x-coordinate.
|
	move.w	d0,d2		| save the x-coordinate for later
	andi.w	#0x000f,d0	| bit offset = x-coordinate mod 16
	andi.w	#0xfff0,d2	| clear bits for offset into word

|
|	Convert the adjusted x-coordinate to a word offset into the current
|	scan line.  If the planes are arranged in an interleaved fashion with
|	a word for each plane then shift the x-coordinate by a value contained
|	in the shift table.  If the planes are arranged as separate, consecu-
|	tive entities then divide the x-coordinate by 8 to get the number of
|	bytes.
|
	move.w	_v_planes,d3	| get number of planes
	move.b	shf_tab-1(pc,d3.w),d3	| get shift factor
	lsr.w	d3,d2		| convert x-coordinate to offset

|
|	Compute the offset to the desired word by adding the offset to the
|	start of the scan line to the offset within the scan line, restore
|	the clobbered registers, and exit.
|
	add.w	d2,d1		| compute total offset into screen buffer
	movem.w (sp)+,d2/d3	| restore the clobbered registers
	rts

| ===========================================================================
|	Shift Table for Computing Offsets into a Scan Line When 	
|	the Video Planes Are Interleaved				
|									
shf_tab:
	dc.b	3			| 1 plane
	dc.b	2			| 2 planes
	dc.b	0			| not used
	dc.b	1			| 4 planes

	.page

	.end


