/*
 * linea.S - linea graphics stuff 
 *
 * Copyright (c) 2001 by Authors:
 *
 *  THO  Thomas Huth
 *  MAD  Martin Doering
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */


// ==== External Declarations ================================================

        .global _int_linea       	// entry for linea exception


// ==== Line-A handler ===============================
_int_linea:
	move.l  2(sp),a0
	move.l  a0,a1
	clr.l   d0
	move.w  (a0)+,d0
	move.l  a0,2(sp)
	and.w   #0xFFF,d0
	tst.w   d0
	bmi     wrong_linea
	cmp.w   linea_ents,d0
	bpl	wrong_linea
	lea     linea_vecs,a0
	lsl.w   #2,d0
	move.l  0(a0,d0),a0
	jsr     (a0)
linea_dispatch_pc:	
	rte

wrong_linea:
	move.w  d0,-(sp)
	sub.w   #2,a0
	move.l  a0,-(sp)
	pea	wrong_linea_msg
	jsr     _kprintf
	add.w	#10,sp
	rte
wrong_linea_msg:	
	.ascii  "pc=0x%08lx: Line-A call number 0x%03x out of bounds\n\0"
	.even



_linea_0:	
	lea	line_a_vars,a0  // get base address for line a variables
	move.l  a0,d0

	move.l  _font_ring,a1	// get pointer to the three system font headers
// BUG ? move.l  (a1),a1
	
	lea     linea_vecs,a2	// get pointer to table of the Line-A routines
	rts

//
// These are stubs for linea :
// the stub will print the pc of the caller, whether the function
// was called using the line a opcode, or directly via its address.
//

_linea_1:
	move.w	#1,d0
	bra	linea_stub



_linea_2:
	move.w	#2,d0
	bra	linea_stub



_linea_3:
	move.w	#3,d0
	bra	linea_stub



_linea_4:
	move.w	#4,d0
	bra	linea_stub



_linea_5:
	move.w	#5,d0
	bra	linea_stub



_linea_6:
	move.w	#6,d0
	bra	linea_stub



_linea_7:
	move.w	#7,d0
	bra	linea_stub



_linea_8:
	move.w	#8,d0
	bra	linea_stub



_linea_9:
	move.w	#9,d0
	bra	linea_stub



_linea_a:
	move.w	#0xa,d0
	bra	linea_stub



_linea_b:
	move.w	#0xb,d0
	bra	linea_stub



_linea_c:
	move.w	#0xc,d0
	bra	linea_stub



_linea_d:
	move.w	#0xd,d0
	bra	linea_stub



_linea_e:
	move.w	#0xe,d0
	bra	linea_stub



_linea_f:
	move.w	#0xf,d0
	bra	linea_stub



linea_stub:
	move.l  (sp),d1
	sub.l   #linea_dispatch_pc,d1
	and.l   #0xFFFFFF,d1
	bne	1f
	move.l  a1,a0
	bra     2f
1:	move.l  (sp),a0
2:	move.w  d0,-(sp)
	move.l  a0,-(sp)	
	pea	linea_stub_msg
	jsr	_kprintf
	add.w	#10,sp
	rts
linea_stub_msg:
	.ascii	"pc=0x%08lx: unimplemented Line-A call number 0x%03x\n\0"
	.even
	


linea_vecs:
	dc.l	_linea_0
	dc.l	_linea_1
	dc.l	_linea_2
	dc.l	_linea_3
	dc.l	_linea_4
	dc.l	_linea_5
	dc.l	_linea_6
	dc.l	_linea_7
	dc.l	_linea_8
	dc.l	_linea_9
	dc.l	_linea_a
	dc.l	_linea_b
	dc.l	_linea_c
	dc.l	_linea_d
	dc.l	_linea_e
	dc.l	_linea_f
linea_ents:
	dc.w    (linea_ents-linea_vecs)/4



// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

	.end
