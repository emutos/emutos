// ===========================================================================
// ==== vectors.s - default exception vectors
// ===========================================================================
//
// Copyright (c) 2001 Laurent Vogel.
//
// Authors:
//  LVL  Laurent Vogel
//
// This file is distributed under the GPL, version 2 or at your
// option any later version.  See doc/license.txt for details.

// Note: this scheme is designed to print the exception number
// for vectors 2 to 63 even if working on a 32bit address bus. 
// LVL.

	.xdef	_panic
	
	.xdef	_proc_lives
	.xdef	_proc_dregs	
	.xdef	_proc_aregs	
	.xdef	_proc_enum	
	.xdef	_proc_usp	
	.xdef	_proc_stk	
        
	.global	init_exc_vec
	.global	init_user_vec



//
// initialize the 62 exception vectors.
//
init_exc_vec:
	clr.l	_proc_lives
	lea	deflt_vec_table, a0
	lea 	8, a1
	move.l	#61, d0
set_vec:
	move.l	a0, (a1)+
	add.l	#2, a0
	dbra	d0, set_vec
	rts
//
// initialize the 192 user vectors.
//
init_user_vec:	
	lea 	user_vec, a0
	move.l	#191, d0
set_uvec:
	move.l	a0, (a1)+
	dbra	d0, set_uvec
	rts

deflt_vec_table:
	bsr.s	any_vec		// vector 2
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 5
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 10
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 15
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 20
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 25
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 30
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 35
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 40
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 45
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 50
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 55
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec		// vector 60
	bsr.s	any_vec
	bsr.s	any_vec
	bsr.s	any_vec

user_vec:
	pea	deflt_vec_table(pc)

// at this point, stack contains
// 0:exception vector address 4:sr 6:pc
any_vec:
	move.w	#0x2700, sr
	movem.l	d0-d7, _proc_dregs
	move.l	(sp)+, d0
	movem.l	a0-a7, _proc_aregs
	lea	deflt_vec_table(pc), a0
	sub.l	a0, d0
	lsr.l	#1, d0
	add.l	#1, d0
	move.l	d0, _proc_enum
	move	usp, a0
	move.l	a0, _proc_usp
	lea	_proc_stk, a0
	move.l	a7, a1
	move.l	(a1)+, (a0)+
	move.l	(a1)+, (a0)+
	move.l	(a1)+, (a0)+
	move.l	(a1)+, (a0)+
	move.l	(a1)+, (a0)+
	move.l	(a1)+, (a0)+
	move.l	(a1)+, (a0)+
	move.l	(a1)+, (a0)+
	move.l	#0x12345678, _proc_lives
	jsr	_panic
forever:
	bra	forever
