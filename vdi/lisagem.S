/*
 * lisagem.S - 
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "vdiconf.h"


/* Global declarations */

        .globl  _DINIT_G
        .globl  _EX_TIMV
        .globl  _GCHC_KEY
        .globl  _GCHR_KEY
        .globl  _GLOC_KEY
        .globl  _GSHIFT_S
        .globl  _INIT_G

        .globl  _MOV_CUR        // External routines used here
        .globl  _XFM_CRFM
        .globl  scrn_clip
        .globl  escfn2          // enter graphics mode
        .globl  escfn3          // exit graphics mode

/* External declarations */

        .xdef   _CONTRL     
        .xdef   _GCURX
        .xdef   _GCURY
        .xdef   _HIDE_CNT
        .xdef   _INTIN
        .xdef   _INTOUT
        .xdef   _TERM_CH
        .xdef   _v_bas_ad
        .xdef   _v_lin_wr
        .xdef   _v_planes
        .xdef   _X1     
        .xdef   _Y1
        .xdef   arrow_cdb
        .xdef   _cur_ms_stat
        .xdef   _tim_addr
        .xdef   _user_but
        .xdef   _user_cur
        .xdef   _user_mot


        .text



/*
 * just_rts - just return
 */
 
just_rts:
        rts
        
        

/*
 * _GSHIFT_S - GET SHIFT STATE 
 * entry:          none
 * exit:           CTL/SHIFT/ALT status in d0
 * destroys:       nothing
 */
 
_GSHIFT_S:
        move.w  #-1, -(sp)      // Ask BIOS for status of CTL-ALT-SHIFT
        move.w  #0xB, -(sp)
        trap    #13

        addq.l  #4, sp          // Clean up the stack

        andi.w  #0x000F, d0     // We only want the low 4 bits
        rts



/*
 * _EX_TIMV - exchange timer interrupt vector
 * 
 * entry:          new vector in CONTRL[7-8]
 * exit:           old vector in CONTRL[9-10]
 * destroys:       a0
 */

_EX_TIMV:
        move.l  _CONTRL,a0

        move    sr, -(sp)       // Disable interrupts
        ori.w   #0x0700, sr

        move.l  _tim_addr,18(a0)
        move.l  14(a0),_tim_addr

        move    (sp)+,sr

        move.w  #6,-(sp)        // Get tickcal (milliseconds per tick)
        trap    #13
        addq.l  #2,sp

        move.l  _INTOUT,a0
        move.w  d0,(a0)

        rts



/*
 * tick_int -  Timer interrupt routine 
 *
 * The etv_timer does point to this routine
 *
 *  entry:          none
 *  exit:           none
 *  destroys:       none
 */
 
tick_int:
        tst.b   in_proc                 // if we are still running, exit
        bne     timer_exit
        move.b  #1,in_proc

        movem.l d0-d7/a0-a6, -(sp)

        move.l  _tim_addr,a0            // Invoke user routine
        jsr     (a0)

        movem.l (sp)+, d0-d7/a0-a6
timer_exit:
        clr.b   in_proc                 // allow yet another trip through
        move.l  _tim_chain, -(sp)       // Call the old timer vector too
        rts



/*
 * _INIT_G - INITIALIZE GRAPHICS 
 *
 * entry:          none
 * exit:           none
 * destroys:       see _v_clrwk
 */

_INIT_G:
        clr.b   in_proc                 // no vblanks in process

        move.l  #just_rts,_tim_addr     // Tick points to rts

        move    sr, -(sp)               // Disable interrupts
        ori.w   #0x0700, sr

        pea     tick_int
        move.w  #0x100, -(sp)           // etv_timer
        move.w  #5, -(sp)               // call setexc via
        trap    #13                     // BIOS
        add.l   #8, sp                  // clean up stack

        move.l  d0, _tim_chain          // save old timer int vector

        move    (sp)+, sr               // Restore interrupt context

        bsr     vdimouse_init              // initialize mouse
                                        /* TODO: switch off cursor */
        bra     escfn2                  // clear screen



/*
 * _DINIT_G - deinitialize graphics
 *
 * entry:          none
 * exit:           none
 * destroys:       see _v_clrwk
 */

_DINIT_G:
        move.l  _tim_chain, -(sp)       // restore old timer int vector
        move.w  #0x100, -(sp)           // etv_timer
        move.w  #5, -(sp)               // call setexc via
        trap    #13                     // BIOS
        add.l   #8, sp                  // clean up stack

        bsr     vdimouse_exit           // de-initialize mouse
        bra     escfn3                  // clear screen
                                        // TODO: switch cursor on



/*
 * vdimouse_init - INITIALIZE MOUSE
 *
 * entry:          none
 * exit:           none
 * destroys:       a0
 */

vdimouse_init:
        /* Init _user_cur vector */
        lea.l   just_rts,a0             
        move.l  a0,_user_but            // just rts
        move.l  a0,_user_mot            // just rts
        move.l  #_MOV_CUR,_user_cur     // initialize user_cur vector

        /* Move in the default mouse form (presently the arrow) */
        move.l  _INTIN, -(sp)           // Save the intin pointer and make
        move.l  #arrow_cdb, _INTIN      // it point to the arrow
        bsr     _XFM_CRFM               // transform mouse
        move.l  (sp)+, _INTIN           // Restore INTIN pointer


#if 0 // is done in tick_int, like in TOS (MAD)
        /* Init mouse vbl interrupt */
        move.l  _vblqueue, a0           // get VBL-queue address
        move.l  #vb_draw, (a0)          // set GEM VBL-routine (in vdimouse.S)
#endif        
        rts



/*
 * vdimouse_exit - deinitialize/disable mouse
 *
 * Disables the mouse
 *
 * entry:          none
 * exit:           none
 * destroys:       a0
 */
 
vdimouse_exit:
        /* Init _user_cur vector */
        lea.l   just_rts,a0             
        move.l  a0,_user_but            // just rts
        move.l  a0,_user_mot            // just rts
        move.l  a0,_user_cur            // just rts
        rts



/*
 * _GCHC_KEY - get choice for choice input
 *
 * entry:  none
 * exit:   d0=0    nothing happened
 *         d0=1    choice value
 *         d0=2    button pressed
 *
 * TERM_CH         16 bit char info
 */

_GCHC_KEY:
        moveq   #1,d0
        move.w  d0, _TERM_CH
        rts



/*
 * _GCHR_KEY - get char for string input
 *
 * entry: none
 * exit:  d0=1     button pressed
 *        d0=0     nothing happened
 * 
 * TERM_CH         16 bit char info
 */

_GCHR_KEY:
        move.w  #2, -(sp)               // see if a character present at con
        move.w  #1, -(sp)
        trap    #13
        addq.l  #4, sp

        tst.w   d0
        beq     no_char

        move.w  #2, -(sp)               // get the character from console
        move.w  #2, -(sp)               // call Bconin
        trap    #13                     // BIOS
        addq.l  #4, sp

        move.l  d0, d1
        swap    d1
        lsl.w   #8, d1                  // scancode to bit 8-15
        or.w    d1, d0                  // asciicode to bit 0-7

        move.w  d0,_TERM_CH             // store char
        move.l  #1,d0
        rts
no_char:
        moveq.l #0,d0
        rts



/*
 * _GLOC_KEY - get locator key
 *
 * entry:  none
 * exit:   d0 = 0    - nothing
 *         d0 = 1    - button pressed
 *                     _TERM_CH = 16 bit char info
 *
 *         d0 = 2    - coordinate info
 *                     _X1 = new x
 *                     _Y1 = new y
 *         d0 = 4    - NOT IMPLIMENTED IN THIS VERSION
 */

_GLOC_KEY:
        move.b  _cur_ms_stat,d0
        move.b  d0,d1           // save state
        andi.b  #0xc0,d0        // mask out all but button status bits
        beq     key_stat        // nothing go check key stat

        move.b  d1, d0
        btst    #6, d0
        beq     notright

        move.w  #0x20, _TERM_CH // send terminator code for left key
        bra     clr_bt_stat
notright:
        move.w  #0x21,_TERM_CH  // send terminator code for left key

clr_bt_stat:
        andi.b  #0x23, d0                // clear mouse button status
        move.b  d0, _cur_ms_stat  // store cleared state
        move.w  #1, d0           // set button pressed flag
        rts

key_stat:
        move.w  #2, -(sp)  // see if a character present at con
        move.w  #1, -(sp)
        trap    #13
        addq.l  #4, sp

        tst.w   d0
        beq     mov_stat        // no char

        move.w  #2, -(sp)       // get the character from console
        move.w  #2, -(sp)       // call Bconin
        trap    #13             // BIOS
        addq.l  #4, sp
        
        move.l  d0, d1
        swap    d1
        lsl.w   #8, d1
        or.w    d1, d0
        move.w  d0, _TERM_CH     // store char
        move.l  #1, d0
        rts
mov_stat:
        bclr.l  #5,d1           // test and clear mouse motion flag 
        beq     no_chg

        move.b  d1, _cur_ms_stat  // save mouse stat
        move.w  _GCURX, _X1      // set _X1 = _GCURX
        move.w  _GCURY, _Y1      // set _Y1 = _GCURY
        move.l  #2,d0
        rts

no_chg:  
        move.l  #0,d0
        rts



                .bss

in_proc:        ds.b    1       // non-zero if my timer code is running




                .end
