/*
 * coma.S - GEMDOS command interpreter assembly language interface
 *
 * Copyright (c) 1999 by Caldera Inc.
 * Copyright (c) 1985, 2001 by Authors:
 *
 *  JSL  Jason S. Loveman
 *  SCC  Steven C. Cavender
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



	.text
	.globl	_in_term
	.globl	_rm_term
	.globl	_xbrkpt
	.globl	_cmain
	.globl	_div10
	.globl	_mod10
	.globl	_xoscall
//	.globl	_bios
	.globl	_xsetjmp
	.globl	_xlongjmp
	.globl	_exeflg
	.globl	_jb
	.globl	_devector					// SCC  22 Mar 85
	.globl	_super						// JSL  20 Mar 85
	.globl	_user						// JSL  20 Mar 85
	.globl	_emucon
_main:
_emucon:
      	move.l	#mystak,a5
	move.l	4(sp),-(a5)
	clr.l	-(a5)		// bogus return address
	move.l	a5,sp
	move.l	4(sp),a5
	move.l	0xc(a5),d0
	add.l	0x14(a5),d0
	add.l	0x1c(a5),d0
	add.l	#0x100,d0
	move.l	d0,-(sp)
	move.l	a5,-(sp)
	move.w	#0,-(sp)
	move.w	#0x4a,-(sp)
	trap	#1
	add.l	#12,sp

	clr.l	-(sp)		// toggle into sup mode	       
	move.w	#0x20,-(sp)	// to do BIOS calls	       
	trap	#1					       
	move.l	d0,2(sp)	// save SSP back in stack       
//
	move.l	#mycrit,-(sp)
	move	#0x101,-(sp)				       
	move	#5,-(sp)
	trap	#13		// set/get critical error
	add.l	#8,sp
	move.l	d0,ocrit
	jsr	_in_term				       
//
	move.w	#0x20,(sp)	// toggle back out to user mode 
	trap	#1					       
	addq.l	#6,sp					       
	jmp	_cmain	// leave basepage on stack
//
_devector:						       
	clr.l	-(sp)		// toggle into sup mode	       
	move.w	#0x20,-(sp)	// to do BIOS calls	       
	trap	#1					       
	move.l	d0,2(sp)	// save SSP back in stack       

	move.l	ocrit,-(sp)	// restore crit err vector      
	move.w	#0x101,-(sp)				       
	move.w	#5,-(sp)				       
	trap	#13					       
	add.l	#8,sp					       
//
	jsr	_rm_term				       
	move.w	#0x20,(sp)	// toggle back out to user mode 
	trap	#1					       
	addq.l	#6,sp					       
	rts						       
//
_in_term:						       
	move.l	#myterm,-(sp)				       
	move	#0x102,-(sp)				       
	move	#5,-(sp)				       
	trap	#13		// set/get terminate vector     
	add.l	#8,sp					       
	move.l	d0,oterm				       
	rts						       
//
_rm_term:						       
	move.l	oterm,-(sp)	// restore terminate vector     
	move.w	#0x102,-(sp)				       
	move.w	#5,-(sp)				       
	trap	#13					       
	add.l	#8,sp					       
	rts

//
_super: clr.l	-(sp)					       
	move	#0x20,-(sp)				       
	trap	#1					       
	addq.l	#6,sp					       
	move.l	d0,savess				       
	rts						       
//							       
_user:	move.l	savess,-(sp)				       
	move	#0x20,-(sp)				       
	trap	#1					       
	addq.l	#6,sp					       
	rts						       

myterm: cmp	#0,_exeflg	// is this my child's term
	beq	itsme

	rts

// I never terminate (need to distinguish 2nd level command.com (ie. batch)

itsme:
	andi.w	#0x5fff,sr	// return to user mode

	move	#1,-(sp)
	move.l	#_jb,-(sp)
	jsr	_xlongjmp
//
mycrit: move.l	#aris,a0
	jsr	bprt
	move	#2,-(sp)
	move	#2,-(sp)	// conin
	trap	#13
	addq.l	#4,sp
	and	#0x5f,d0 	// upcase
	cmp.b	#'A',d0
	beq	acrit

	cmp.b	#'R',d0
	beq	rcrit

	cmp.b	#'I',d0
	bne	mycrit

// ignore the failure, continue processing
icrit:	clr.l	d0
	rts

// abort the offending process
acrit:	move.w	4(sp),d0
	ext.l	d0
	rts

// retry the operation
rcrit:	move	#1,d0
	swap	d0
	rts
//
bprt:	clr.l	d0
	move.b	(a0)+,d0
	cmp.b	#0,d0
	beq	nomoch
	move.l	a0,-(sp)
	move	d0,-(sp)
	move	#2,-(sp)	// device handle
	move	#3,-(sp)	// conout function
	trap	#13
	addq.l	#6,sp
	move.l	(sp)+,a0
	jmp	bprt
nomoch: rts
//
_xbrkpt:	illegal
	rts
//
_xsetjmp: link	a6,#0
	move.l	8(a6),a0
	move.l	0(a6),(a0)+
	lea	8(a6),a1
	move.l	a1,(a0)+
	move.l	4(a6),(a0)
	clr.l	d0
	unlk	a6
	rts

_xlongjmp: link a6,#0
	move	12(a6),d0
	tst	d0
	bne	okrc
	move	#1,d0
okrc:	move.l	8(a6),a0
	move.l	(a0)+,a6
	move.l	(a0)+,a7
	move.l	(a0),-(sp)
	rts
//
//_bios:	move.l	(sp)+,biosav
//	trap	#13
//	move.l	biosav,-(sp)
//	rts

_div10: link	a6,#0
	move.l	8(a6),d0
	divu	#10,d0
	swap	d0
	clr	d0
	swap	d0
	unlk	a6
	rts

_mod10: link	a6,#0
	move.l	8(a6),d0
	divu	#10,d0
	clr	d0
	swap	d0
	unlk	a6
	rts
// call dosjr from within itself (or from linked-in shell)
_xoscall:
	move.l	(sp)+,retshell
	trap	#1
	move.l	retshell,-(sp)
	rts


	.data
aris:
	.dc.b	13,10
	.ascii "(A)bort, (R)etry, or (I)gnore ?"
        .dc.b	0

	.bss
	.even

savess: .ds.l	1					  
ocrit:	.ds.l	1
oterm:	.ds.l	1
biosav: .ds.l	1
retshell: .ds.l 1

	.ds.l	800		// Stack space, growing backwards			  
mystak: .ds.l	1               // Stack for command.prg
	.end


