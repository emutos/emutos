/*
 * aciavecs.S - exception handling for ikbd/midi acias.
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

// (following text is taken from the Atari Compendium, xbios(0x22)
//
// Kbdvbase() returns a pointer to a system structure KBDVECS which
// is defined as follows:
//
// typedef struct
// {
//   void (*midivec)( UBYTE data );  /* MIDI Input */
//   void (*vkbderr)( UBYTE data );  /* IKBD Error */
//   void (*vmiderr)( UBYTE data );  /* MIDI Error */
//   void (*statvec)(char *buf);     /* IKBD Status */
//   void (*mousevec)(char *buf);    /* IKBD Mouse */
//   void (*clockvec)(char *buf);    /* IKBD Clock */
//   void (*joyvec)(char *buf);      /* IKBD Joystick */
//   void (*midisys)( void );        /* Main MIDI Vector */
//   void (*ikbdsys)( void );        /* Main IKBD Vector */
// } KBDVECS;
//
//- midivec is called with the received data byte in d0.
//- If an overflow error occurred on either ACIA, vkbderr or vmiderr
//  will be called, as appropriate by midisys or ikbdsys with the
//  contents of the ACIA data register in d0.
//- statvec, mousevec, clockvec, and joyvec all are called with
//  the address of the packet in register a0.
//- midisys and ikbdsys are called by the MFP ACIA interrupt handler
//  when a character is ready to be read from either the midi or
//  keyboard ports.
//
// In addition to the documented features, it was deemed necessary to add
// the following undocumented features, located in memory immediately before
// and after the documented part of the KBDVECS structure:
//
// struct UNDOCUMENTED {
//   void (*kbdvec)( UBYTE data );   /* KBD Input, TOS >= 2.0 */
//   KBDVECS kbdvecs;
//   char ikbdstate;
//   char kbdlength;
// };
//
//- kbdvec (undocumented feature of TOS >= 2.0) is called with the
//  received data byte in d0.
//- The ikbdstate description in the Compendium is wrong. It should read:
//  "When the ikbdstate variable is non-zero, it means that a packet is
//  currently being retrieved. In that case, kbdlength represents the number
//  of remaining bytes to retrieve that are part of an IKBD packet."

/* Known differences between this and Atari TOS:
 *
 * a) Although this doesn't appear to be documented, the TOS setups the
 * following registers before entering vectors ikbdsys and midisys:
 *   a0: address of the iorec structure
 *   a1: address of the ACIA control register
 *   a2: address of the error routine (vkbderr or vmiderr)
 *   d2: value of the ACIA status register
 * Currently EmuTOS does not setup these registers.
 *
 * b) In EmuTOS the same buffer kbdbuf is used to accumulate the various IKBD
 * packets, whereas TOS uses five different buffers:
 *   statvec, mousevec (absolute), mousevec (relative), clockvec, joyvec.
 *
 * c) EmuTOS currently does not check errors at all. Atari TOS checks the
 * following errors:
 *  - if an interrupt request was received and no full byte is ready to be
 *    read, then TOS jumps to the error vector;
 *  - if a byte was read but an overrun occurred, then after having
 *    correctly processed the received byte, TOS reads the data register
 *    again, then jumps to the error vector.
 */

#include "asmdefs.h"

        .globl  _init_acia_vecs
        .globl  _int_acia

        .globl  _mousexvec
        .globl  _kbdvecs

        .globl  _ikbdiorec
        .globl  _midiiorec

        .globl  _call_mousevec


        .bss

// ==== IOREC BUFFERS ======================================================
// Table of input-output buffers for kbd in, midi in

ikbdibufbuf:    .ds.b   0x100
midiibufbuf:    .ds.b   0x80


// ==== IORECS =============================================================
// Table of input-output records for kbd in, midi in

_ikbdiorec:
ikbdibuf:       .ds.l   1
ikbdibufsz:     .ds.w   1
ikbdibufhd:     .ds.w   1
ikbdibuftl:     .ds.w   1
ikbdibuflo:     .ds.w   1
ikbdibufhi:     .ds.w   1
_midiiorec:
midiibuf:       .ds.l   1
midiibufsz:     .ds.w   1
midiibufhd:     .ds.w   1
midiibuftl:     .ds.w   1
midiibuflo:     .ds.w   1
midiibufhi:     .ds.w   1


// ==== KBDVBASE =============================================================
// This is now the table of routines for managing midi and keyboard data
// in packets from IKBD (shown by a0 and 4(sp))

_mousexvec:     .ds.l   1       // mouse-routine for additional buttons
kbdvec:         .ds.l   1       // must be right before _kbdvecs
_kbdvecs:
midivec:        .ds.l   1       // MIDI input
vkbderr:        .ds.l   1       // keyboard error
vmiderr:        .ds.l   1       // MIDI-Error
statvec:        .ds.l   1       // IKBD-Status
mousevec:       .ds.l   1       // mouse-routine
clockvec:       .ds.l   1       // time-routine
joyvec:         .ds.l   1       // joystick-routinee
midisys:        .ds.l   1       // MIDI-systemvector
ikbdsys:        .ds.l   1       // IKBD-systemvector
ikbdstate:      .ds.b   1       // action to take upon packet completion
kbdlength:      .ds.b   1       // number of bytes remaining in current packet
        .even



        .text

_init_acia_vecs:
        // initialize the kbdvecs structure
        lea     _just_rts,a0            // dummy vector
        lea     _kbdvecs,a1
        move.l  a0,_mousexvec-_kbdvecs(a1)
#ifdef __mcoldfire__
        move.l  #_kbdvec,d0
        move.l  d0,kbdvec-_kbdvecs(a1)
#else
        move.l  #_kbdvec,kbdvec-_kbdvecs(a1)
#endif
#ifdef __mcoldfire__
        move.l  #_midivec,d0
        move.l  d0,midivec-_kbdvecs(a1)
#else
        move.l  #_midivec,midivec-_kbdvecs(a1)
#endif
        move.l  a0,vkbderr-_kbdvecs(a1)
        move.l  a0,vmiderr-_kbdvecs(a1)
        move.l  a0,statvec-_kbdvecs(a1)
        move.l  a0,mousevec-_kbdvecs(a1)
#if CONF_WITH_IKBD_CLOCK
#ifdef __mcoldfire__
        move.l  #_clockvec,d0                        // in bios/clock.c
        move.l  d0,clockvec-_kbdvecs(a1)
#else
        move.l  #_clockvec,clockvec-_kbdvecs(a1)     // in bios/clock.c
#endif
#endif /* CONF_WITH_IKBD_CLOCK */
        move.l  a0,joyvec-_kbdvecs(a1)
#if CONF_WITH_MIDI_ACIA
#ifdef __mcoldfire__
        move.l  #_midisys,d0
        move.l  d0,midisys-_kbdvecs(a1)
#else
        move.l  #_midisys,midisys-_kbdvecs(a1)
#endif
#endif /* CONF_WITH_MIDI_ACIA */
#if CONF_WITH_IKBD_ACIA
#ifdef __mcoldfire__
        move.l  #_ikbdsys,d0
        move.l  d0,ikbdsys-_kbdvecs(a1)
#else
        move.l  #_ikbdsys,ikbdsys-_kbdvecs(a1)
#endif
#endif /* CONF_WITH_IKBD_ACIA */

        // initialize the iorecs
        pea     iorec_table_end-iorec_table
        pea     iorec_table
        pea     _ikbdiorec
        jsr     _memmove
        lea     12(sp),sp

        // initialize IKBD-related variables
        moveq.l #0,d0
        move.b  d0,ikbdstate
        move.b  d0,joybuf+1
        move.b  d0,joybuf+2

#if CONF_WITH_IKBD_ACIA || CONF_WITH_MIDI_ACIA
        // finally, setup the interrupt vector
        pea     _int_acia
        move.w  #6,-(sp)
        jsr     _mfpint
        addq.l  #6,sp
#endif /* CONF_WITH_IKBD_ACIA || CONF_WITH_MIDI_ACIA */

#ifdef MACHINE_AMIGA
        jsr     _amiga_init_keyboard_interrupt
#endif
        rts

        // FIXME: The iorec stuff should go into the .data segment (if
        // EmuTOS had a functional one) instead of copying the following to
        // the .bss segment at run time.
        SECTION_RODATA
        .even   // Mandatory in ELF section .rodata
iorec_table:
        //         sz, hd, tl,   lo, hi
        .dc.l   ikbdibufbuf
        .dc.w   0x100,  0,  0, 0x40, 0xC0
        .dc.l   midiibufbuf
        .dc.w    0x80,  0,  0, 0x20, 0x60
iorec_table_end:
        .text

#if CONF_WITH_IKBD_ACIA || CONF_WITH_MIDI_ACIA

// ==== Int 0x118 - midi/kbd interrupt routine ================
//

_int_acia:
        // save scratch regs
        /* LVL: Strictly speaking there is no specification telling what
         * registers can be freely used by the vectors of the kbdvecs struct.
         * Theoretically d0-d1/a0-a1 would be sufficient here.
         */
#ifdef __mcoldfire__
        lea     -32(sp),sp
        movem.l d0-d3/a0-a3,(sp)
#else
        movem.l d0-d3/a0-a3,-(sp)
#endif

int_acia_loop:
#if CONF_WITH_MIDI_ACIA
        move.l  midisys,a0
        jsr     (a0)
#endif
#if CONF_WITH_IKBD_ACIA
        move.l  ikbdsys,a0
        jsr     (a0)
#endif
        lea     0xfffffa00.w,a1
        btst.b  #4,0x1(a1)                // while still interrupting
        beq     int_acia_loop
        bclr.b  #6,0x11(a1)               // clear in service bit

        // restore scratch regs
#ifdef __mcoldfire__
        movem.l (sp),d0-d3/a0-a3
        lea     32(sp),sp
#else
        movem.l (sp)+,d0-d3/a0-a3
#endif
        rte

#endif /* CONF_WITH_IKBD_ACIA || CONF_WITH_MIDI_ACIA */

#if CONF_WITH_MIDI_ACIA

// ==== MIDI stuff ================
//

        .equ    midi_acia_stat, 0xfffffc04
        .equ    midi_acia_data, 0xfffffc06

_midisys:
        move.b  midi_acia_stat,d0
        bpl     midirts                 // not interrupting
        // TODO (?): check errors (buffer full ?)
        move.b  midi_acia_data,d0
        move.l  midivec,a0
        jmp     (a0)                    // stack is clean: no need to jsr.
midirts:
        rts

#endif /* CONF_WITH_MIDI_ACIA */

_midivec:
        // push byte data in d0 into midi iorec.
#ifdef __mcoldfire__
        moveq   #0,d1
#endif
        move.w  midiibuftl,d1
#ifdef __mcoldfire__
        addq.l  #1,d1
#else
        addq.w  #1,d1
#endif
        cmp.w   midiibufsz,d1
        blt     1f
        moveq   #0,d1
1:      cmp.w   midiibufhd,d1
        beq     1f
        movea.l midiibuf,a0             // must use ptr, user may have changed it
#ifdef __mcoldfire__
        move.b  d0,0(a0,d1.l)
#else
        move.b  d0,0(a0,d1.w)
#endif
        move.w  d1,midiibuftl
1:      rts

// ==== IKBD stuff ================
//
// Packets received from the IKBD are accumulated into the kbdbuf buffer.
// The packet header (F6 to FF) determines the packet length and the
// action to be taken once the packet has been received completely.
// During the reception of a packet, variable ikbdstate contains the
// action number, and variable kbdlength contains the number of bytes
// not received yet.
//
// action <--whole IKBD packet-->  Comment
// number    <-given to routine->
//
//  1     F6 a1 a2 a3 a4 a5 a6 a7 (miscellaneous, 7 bytes)
//  2     F7 0b xh xl yh yl       (absolute mouse)
//  3        F8 dx dy             (relative mouse, no button)
//  3        F9 dx dy             (relative mouse, button 1)
//  3        FA dx dy             (relative mouse, button 2)
//  3        FB dx dy             (relative mouse, both buttons)
//  4     FC yy MM dd hh mm ss    (date and time)
//  5     FD j0 j1                (both joysticks)
//  6        FE bj                (joystick 0)
//  7        FF bj                (joystick 1)
//
        .bss
        .even
kbdindex:
        .ds.w   1   // position of next byte in buffer
kbdbuf: .ds.b   8   // buffer where packets are being reconstructed
joybuf: .ds.b   3

        .text

_kbdvec:
        move.w  d0,-(sp)
        // call the C routine in ikbd.c to do the work.
        jsr     _kbd_int
        addq.l  #2,sp
just_rts:
        rts

#if CONF_WITH_IKBD_ACIA
        .equ    ikbd_acia_stat, 0xfffffc00
        .equ    ikbd_acia_data, 0xfffffc02

_ikbdsys:
        moveq   #0,d0
        move.b  ikbd_acia_stat,d1
        move.b  d1,d0
        bpl     just_rts                // not interrupting
        // TODO (?): check errors (buffer full ?)
        move.b  ikbd_acia_data,d0
        // Fall into ikbdraw
#endif /* CONF_WITH_IKBD_ACIA */

// Process a raw byte received from IKBD protocol.
// This routine is hardware independant.
// The byte to process is in d0.l
        .globl  ikbdraw
ikbdraw:
        tst.b   ikbdstate       // inside a multi-byte packet?
        bne     in_packet       // ikbdstate != 0 => go and add to the packet
        cmp.w   #0xf6,d0        // is byte a packet header?
        bcc     begin_packet    // byte >= 0xf6 => go begin receiving a packet
        move.l  kbdvec,a0       // ordinary key byte in d0. jump in vector
        jmp     (a0)            // stack is clean: no need to jsr.

begin_packet:
        move.b  d0,kbdbuf       // put the byte at beginning of the buffer
#ifdef __mcoldfire__
        lea     kbdindex,a0
        move.w  #1,(a0)         // next position in buffer is byte number 1
        sub.l   #0xf6,d0
        lea     ikbdstate,a0
        move.b  ikbdstate_table(pc,d0),(a0)
        lea     kbdlength,a0
        move.b  kbdlength_table(pc,d0),(a0)
#else
        move.w  #1,kbdindex     // next position in buffer is byte number 1
        sub.b   #0xf6,d0
        move.b  ikbdstate_table(pc,d0),ikbdstate
        move.b  kbdlength_table(pc,d0),kbdlength
#endif
        rts
ikbdstate_table:
        .dc.b   1, 2, 3, 3, 3, 3, 4, 5, 6, 7
kbdlength_table:
        .dc.b   7, 5, 2, 2, 2, 2, 6, 2, 1, 1
        .even

in_packet:
#ifdef __mcoldfire__
        moveq   #0,d1
#endif
        move.w  kbdindex,d1
        lea     kbdbuf,a0
#ifdef __mcoldfire__
        move.b  d0,0(a0,d1.l)
        moveq   #0,d0
        move.b  kbdlength,d0
        subq.l  #1,d0
        move.b  d0,kbdlength
        beq     got_packet
        moveq   #0,d0
        move.w  kbdindex,d0
        addq.l  #1,d0
        move.w  d0,kbdindex
#else
        move.b  d0,0(a0,d1.w)
        sub.b   #1,kbdlength
        beq     got_packet
        addq.w  #1,kbdindex
#endif
        rts

// now I've got a full packet in buffer kbdbuf.
got_packet:
        moveq.l #0,d0
        move.b  ikbdstate,d0
#ifdef __mcoldfire__
        asl.l   #2,d0
#else
        asl.w   #2,d0
#endif
        move.l  action_table-4(pc,d0),a1
        lea     kbdbuf,a0
        jmp     (a1)
action_table:
        .dc.l   kbd_status      // 1
        .dc.l   kbd_abs_mouse   // 2
        .dc.l   kbd_rel_mouse   // 3
#ifdef CONF_WITH_IKBD_CLOCK
        .dc.l   kbd_clock       // 4
#else
        .dc.l   0               // 4
#endif
        .dc.l   kbd_joys        // 5
        .dc.l   kbd_joy0        // 6
        .dc.l   kbd_joy1        // 7
kbd_status:
        addq.l  #1,a0
        move.l  statvec,a1
        bra     kbd_jump_vec
kbd_abs_mouse:
        addq.l  #1,a0
kbd_rel_mouse:
        move.l  mousevec,a1
        bra     kbd_jump_vec
#ifdef CONF_WITH_IKBD_CLOCK
kbd_clock:
        addq.l  #1,a0
        move.l  clockvec,a1
        bra     kbd_jump_vec
#endif

// Joystick support is special. The buffer passed to routine joyvec
// will, in all cases, contain (package header, joystick 0, joystick 1)
// so for each kind of package we copy this info into a separate joybuf.

kbd_joys:
        lea     joybuf,a1
        move.b  (a0)+,(a1)+
        move.b  (a0)+,(a1)+
        move.b  (a0),(a1)
        subq.l  #2,a1
        bra     joy_next
kbd_joy0:
        lea     joybuf,a1
        move.b  (a0)+,(a1)
        move.b  (a0),1(a1)
        bra     joy_next
kbd_joy1:
        lea     joybuf,a1
        move.b  (a0)+,(a1)
        move.b  (a0),2(a1)
joy_next:
        move.l  a1,a0
        move.l  joyvec,a1

kbd_jump_vec:
        move.l  a0,-(sp)
        jsr     (a1)
        addq.l  #4,sp
        clr.b   ikbdstate
        rts

/******************************************************************************/
/* Call mousevec from C                                                       */
/******************************************************************************/

_call_mousevec:
        move.l  4(sp),a0
        move.l  mousevec,a1
#ifdef __mcoldfire__
        lea     -44(sp),sp
        movem.l d2-d7/a2-a6,(sp)
#else
        movem.l d2-d7/a2-a6,-(sp)
#endif
        jsr     (a1)
#ifdef __mcoldfire__
        movem.l (sp),d2-d7/a2-a6
        lea     44(sp),sp
#else
        movem.l (sp)+,d2-d7/a2-a6
#endif
        rts

#if CONF_WITH_FLEXCAN || CONF_SERIAL_IKBD

/******************************************************************************/
/* Call ikbdraw from C                                                        */
/******************************************************************************/

        .globl  _call_ikbdraw
_call_ikbdraw:
        moveq   #0,d0
        move.b  5(sp),d0                // raw IKBD byte
        jbra    ikbdraw

#endif /* CONF_WITH_FLEXCAN || CONF_SERIAL_IKBD */

#ifdef MACHINE_AMIGA

/******************************************************************************/
/* Call joyvec from C                                                         */
/******************************************************************************/

        .globl  _call_joyvec
_call_joyvec:
        move.l  4(sp),a0
        move.l  joyvec,a1
#ifdef __mcoldfire__
        lea     -44(sp),sp
        movem.l d2-d7/a2-a6,(sp)
#else
        movem.l d2-d7/a2-a6,-(sp)
#endif
        jsr     (a1)
#ifdef __mcoldfire__
        movem.l (sp),d2-d7/a2-a6
        lea     44(sp),sp
#else
        movem.l (sp)+,d2-d7/a2-a6
#endif
        rts

#endif /* MACHINE_AMIGA */
