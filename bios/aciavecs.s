| ===========================================================================
| ==== aciavecs.s - exception handling for ikbd/midi acias.
| ===========================================================================
|
| Copyright (c) 2001 Laurent Vogel.
|
| Authors:
|  LVL  Laurent Vogel
|
| This file is distributed under the GPL, version 2 or at your
| option any later version.  See doc/license.txt for details.

| (following text is taken from the Atari Compendium, xbios(0x22)
| 
| Kbdvbase() returns a pointer to a system structure KBDVECS which 
| is defined as follows: 
|
| typedef struct
| {
|   VOID (*midivec)( UBYTE data );  /* MIDI Input */
|   VOID (*vkbderr)( UBYTE data );  /* IKBD Error */
|   VOID (*vmiderr)( UBYTE data );  /* MIDI Error */
|   VOID (*statvec)(char *buf);     /* IKBD Status */
|   VOID (*mousevec)(char *buf);    /* IKBD Mouse */
|   VOID (*clockvec)(char *buf);    /* IKBD Clock */
|   VOID (*joyvec)(char *buf);      /* IKBD Joystick */
|   VOID (*midisys)( VOID );        /* Main MIDI Vector */
|   VOID (*ikbdsys)( VOID );        /* Main IKBD Vector */
|   char ikbdstate;                 /* See below */
| } KBDVECS;
|
|- midivec is called with the received data byte in d0. 
|- If an overflow error occurred on either ACIA, vkbderr or vmiderr 
|  will be called, as appropriate by midisys or ikbdsys with the 
|  contents of the ACIA data register in d0.
|- statvec, mousevec, clockvec, and joyvec all are called with 
|  the address of the packet in register A0.
|- midisys and ikbdsys are called by the MFP ACIA interrupt handler 
|  when a character is ready to be read from either the midi or 
|  keyboard ports.
|- ikbdstate is set to the number of bytes remaining to be read 
|  by the ikbdsys handler from a multiple-byte status packet.

        .equ    vec_acia, 0x118         | keyboard/Midi interrupt vector

	.global init_acia_vecs
		
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
	move.l	#int_acia,vec_acia
	| while we're at it, initialize the iorecs
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
	
	rts
	
| ==== Int 0x118 - exception for keyboard interrupt ================
|
| LVL: this is Martin's original one, which I'm keeping until the
| new one works.
int_acia:
        movem.l d0-d7/a0-a6,-(sp)
        move    sr,-(sp)                | save status register
        ori     #0x2700,sr              | turn off all interrupts
        jsr     _kbd_int                | call the C routine to do the work
        ori     #0x2300,sr              | turn on interrupts
        move    (sp)+,sr                | restore status register
        movem.l (sp)+,d0-d7/a0-a6
        rte

new_int_acia:
	| save regs (which ones, to be determined)
	movem.l d0-d2/a0-a2,-(sp)
	
int_acia_loop:
	move.l	midisys,a0
	jsr	(a0)
	move.l	ikbdsys,a0
	jsr	(a0)
	btst	#4,0xfffffa01		| while still interrupting
	beq	int_acia_loop
	bclr	#6,0xfffffa11		| clear in service bit
	
	| restore regs
	movem.l (sp)+,d0-d2/a0-a2
	rte

_midivec:
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
	bpl	just_rts		| not interrupting
	| TODO (?): check errors (buffer full ?)
	move.b	midi_acia_data,d0
	move.l	midivec,a0
	jmp	(a0)			| stack is clean: no need to jsr.
just_rts:
	rts

	.equ	ikbd_acia_stat, 0xfffffc00
	.equ	ikbd_acia_data, 0xfffffc02

_ikbdsys:
	move.b	ikbd_acia_stat,d0
	bpl	just_rts		| not interrupting
	| TODO
	rts

