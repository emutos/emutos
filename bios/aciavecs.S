// ===========================================================================
// ==== aciavecs.s - exception handling for ikbd/midi acias.
// ===========================================================================
//
// Copyright (c) 2001 Laurent Vogel.
//
// Authors:
//  LVL  Laurent Vogel
//
// This file is distributed under the GPL, version 2 or at your
// option any later version.  See doc/license.txt for details.

// (following text is taken from the Atari Compendium, xbios(0x22)
// 
// Kbdvbase() returns a pointer to a system structure KBDVECS which 
// is defined as follows: 
//
// typedef struct
// {
//   VOID (*midivec)( UBYTE data );  /* MIDI Input */
//   VOID (*vkbderr)( UBYTE data );  /* IKBD Error */
//   VOID (*vmiderr)( UBYTE data );  /* MIDI Error */
//   VOID (*statvec)(char *buf);     /* IKBD Status */
//   VOID (*mousevec)(char *buf);    /* IKBD Mouse */
//   VOID (*clockvec)(char *buf);    /* IKBD Clock */
//   VOID (*joyvec)(char *buf);      /* IKBD Joystick */
//   VOID (*midisys)( VOID );        /* Main MIDI Vector */
//   VOID (*ikbdsys)( VOID );        /* Main IKBD Vector */
// } KBDVECS;
//
//- midivec is called with the received data byte in d0. 
//- If an overflow error occurred on either ACIA, vkbderr or vmiderr 
//  will be called, as appropriate by midisys or ikbdsys with the 
//  contents of the ACIA data register in d0.
//- statvec, mousevec, clockvec, and joyvec all are called with 
//  the address of the packet in register A0.
//- midisys and ikbdsys are called by the MFP ACIA interrupt handler 
//  when a character is ready to be read from either the midi or 
//  keyboard ports.
//
// LVL:	The following implementation is DIFFERENT from that of TOS.
// And, right now, if is widely UNTESTED.
//

        .equ    vec_acia, 0x118         // keyboard/Midi interrupt vector

	.global init_acia_vecs
	.global _int_acia
		
init_acia_vecs:
	move.l	#_midivec,midivec
	move.l	#_vkbderr,vkbderr
	move.l	#_vmiderr,vmiderr
	move.l	#_statvec,statvec
	move.l	#_mousevec,mousevec
	move.l	#_clockvec,clockvec
	move.l	#_joyvec,joyvec
	move.l	#_midisys,midisys
	move.l	#_ikbdsys,ikbdsys
	move.l	#_int_acia,vec_acia
	// while we're at it, initialize the iorecs
	move.l	#rs232ibufbuf,rs232ibuf
	move.w	#0x100,rs232ibufsz
	move.w	#0,rs232ibufhd
	move.w	#0,rs232ibuftl
	move.w	#0x40,rs232ibuflo
	move.w	#0xC0,rs232ibufhi
	move.l	#rs232obufbuf,rs232obuf
	move.w	#0x100,rs232obufsz
	move.w	#0,rs232obufhd
	move.w	#0,rs232obuftl
	move.w	#0x40,rs232obuflo
	move.w	#0xC0,rs232obufhi
	move.l	#ikbdibufbuf,ikbdibuf
	move.w	#0x100,ikbdibufsz
	move.w	#0,ikbdibufhd
	move.w	#0,ikbdibuftl
	move.w	#0x40,ikbdibuflo
	move.w	#0xC0,ikbdibufhi
	move.l	#midiibufbuf,midiibuf
	move.w	#0x80,midiibufsz
	move.w	#0,midiibufhd
	move.w	#0,midiibuftl
	move.w	#0x20,midiibuflo
	move.w	#0x60,midiibufhi
	move.b	#0,kbdbuf
	rts
	
// ==== Int 0x118 - midi/kbd interrupt routine ================
//

_int_acia:
	// save scratch regs
	movem.l d0-d1/a0-a1,-(sp)
	
int_acia_loop:
	move.l	midisys,a0
	jsr	(a0)
	move.l	ikbdsys,a0
	jsr	(a0)
	btst	#4,0xfffffa01		// while still interrupting
	beq	int_acia_loop
	bclr	#6,0xfffffa11		// clear in service bit
	
	// restore scratch regs
	movem.l (sp)+,d0-d1/a0-a1
	rte

_midivec:
	// push byte data in d0 into midi iorec.
	move.w	midiibuftl,d1
	add.w	#1,d1
	cmp.w	midiibufsz,d1
	blt	1f
	move.l	#0,d1
1:	cmp.w	midiibufhd,d1
	beq 	1f
	lea	midiibufbuf,a0
	move.b	d0,0(a0,d1.w)
	move.w	d1,midiibuftl
1:	rts
	
_vkbderr:
_vmiderr:
_statvec:
_mousevec:
_clockvec:
_joyvec:
	rts
	
	.equ	midi_acia_stat, 0xfffffc04
	.equ	midi_acia_data, 0xfffffc06
	
_midisys:
	move.b	midi_acia_stat,d0
	bpl	just_rts		// not interrupting
	// TODO (?): check errors (buffer full ?)
	move.b	midi_acia_data,d0
	move.l	midivec,a0
	jmp	(a0)			// stack is clean: no need to jsr.
just_rts:
	rts

	.equ	ikbd_acia_stat, 0xfffffc00
	.equ	ikbd_acia_data, 0xfffffc02

_ikbdsys:
	move.l	#0,d0
	move.b	ikbd_acia_stat,d1
	move.b	d1,d0
	bpl	just_rts		// not interrupting
	// TODO (?): check errors (buffer full ?)
	move.b	ikbd_acia_data,d0
	
//	movem.l d0/d1/a0/a1,-(sp)
//	bra 1f
//2:	.ascii  "IKBD data = 0x%02x\n\0"
//	.even
//1:	move.w	d0,-(sp)
//	pea 	2b
//	jsr 	_kprintf
//	add.w 	#6,sp
//	movem.l (sp)+,d0/d1/a0/a1

	tst.b	kbdbuf
	bne	in_packet	// kbdbuf[0] != 0, we are receiving a packet
	cmp.w	#0xf6,d0
	blt	key_event	// byte < 0xf6, a key press or release event
	sub.b	#0xf6,d0
	move.b	kbd_length_table(pc,d0),kbdlength
	move.b	#1,kbdindex
	move.b	d0,kbdbuf
	rts
kbd_length_table:
	dc.b	8, 6, 3, 3, 3, 3, 7, 3, 2, 2

key_event:
	move.w	d0,-(sp)
	// call the C routine in newkbc.c to do the work.
	jsr 	_kbd_int
	add.w   #2,sp
	rts
	
in_packet:
	move.l	#0,d1
	move.b	kbdindex,d1
	lea	kbdbuf,a0
	move.b	d0,(a0,d1)
	add.b	#1,d1
	cmp.b	kbdlength,d1
	bge	got_packet
	move.b	d1,kbdindex
	rts
// now I've got a packet in buffer kbdbuf, of length kbdlength.
got_packet:
	move.l	#0,d0
	move.b	(a0),d0
	add.w	d0,d0
	add.w	d0,d0
	move.l	packet_switch(pc,d0),a0
	jmp	(a0)
packet_switch:
	dc.l	kbd_status
	dc.l	kbd_abs_mouse
	dc.l	kbd_rel_mouse
	dc.l	kbd_rel_mouse
	dc.l	kbd_rel_mouse
	dc.l	kbd_rel_mouse
	dc.l	kbd_clock
	dc.l	kbd_joys
	dc.l	kbd_joy0
	dc.l	kbd_joy1
kbd_status:
	lea	kbdbuf+1,a0
	move.l	statvec,a1
	bra	kbd_jump_vec
kbd_abs_mouse:
	lea	kbdbuf+1,a0
	move.l	mousevec,a1
	bra	kbd_jump_vec
kbd_rel_mouse:
	lea	kbdbuf,a0
	move.l	mousevec,a1
	bra	kbd_jump_vec
kbd_clock:
	lea	kbdbuf+1,a0
	move.l	clockvec,a1
	bra	kbd_jump_vec
kbd_joys:
	lea	kbdbuf,a0
	move.l	joyvec,a1
	bra	kbd_jump_vec
kbd_joy0:
	lea	kbdbuf,a0
	move.l	joyvec,a1
	bra	kbd_jump_vec
kbd_joy1:
	lea	kbdbuf,a0
	move.b	1(a0),2(a0)
	clr.b	1(a0)			// ???
	move.l	joyvec,a1
	bra	kbd_jump_vec

kbd_jump_vec:		
	move.l	a0,-(sp)
	jsr	(a1)
	add.w	#4,sp
	clr.b	kbdbuf
	rts
		
