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


                .globl  _CLEARMEM       // globl routines declared here
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
                .globl  vb_draw
                .globl  escfn2          // enter graphics mode
                .globl  escfn3          // exit graphics mode

                .globl  ms_buffer       // Global variables declared here
                .globl  delta_x
                .globl  delta_y

                .xdef   _CONTRL         // External variables used here
                .xdef   _GCURX
                .xdef   _GCURY
                .xdef   _HIDE_CNT
                .xdef   _INTIN
                .xdef   _INTOUT
                .xdef   _MOUSE_BT
                .xdef   _TERM_CH
                .xdef   _v_bas_ad
                .xdef   _v_lin_wr
                .xdef   _v_planes
                .xdef   _X1     
                .xdef   _Y1
                .xdef   arrow_cdb
                .xdef   cur_ms_stat
                .xdef   _draw_flag
                .xdef   _mouse_flag
                .xdef   _tim_addr
                .xdef   _user_but
                .xdef   _user_cur
                .xdef   _user_mot


                .text



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
 * _CLEARMEM - CLEAR SCREEN
 *
 * entry:          none
 * exit:           none
 * destroys:       d0/a0
 */

_CLEARMEM:
	move.l  _v_bas_ad,a0
        move.w  #32000/4,d0    // FIXME!!!
        moveq.l #0,d1

scr_loop:
	move.l  d1,(a0)+
        dbra    d0,scr_loop

just_rts:
	rts



/*
 * _INIT_G - INITIALIZE GRAPHICS 
 *
 * entry:          none
 * exit:           none
 * destroys:       see CLEARMEM
 */

_INIT_G:
        clr.b   in_proc			// no vblanks in process

        move.l  #just_rts,_tim_addr     // Tick points to rts

        move    sr, -(sp)               // Disable interrupts
        ori.w   #0x0700, sr

        pea     tick_int
        move.w	#0x100, -(sp)           // etv_timer
        move.w  #5, -(sp)               // call setexc via
        trap    #13                     // BIOS
        add.l   #8, sp                 // clean up stack

        move.l  d0, _tim_chain            // save old timer int vector

        move    (sp)+, sr               // Restore interrupt context

        bsr     mouse_init              // initialize mouse
        				/* TODO: switch off cursor */
        bra     escfn2                  // clear screen



/*
 * _EX_TIMV - exchange timer interrupt vector
 * 
 * entry:          new vector in CONTRL[7-8]
 * exit:           old vector in CONTRL[9-10]
 * destroys:       a0
 */

_EX_TIMV:
        move.l  _CONTRL,a0

        move    sr,-(sp)       // Disable interrupts
        ori.w   #0x0700,sr
        move.l  _tim_addr,18(a0)
        move.l  14(a0),_tim_addr
        move    (sp)+,sr

        move.w  #6,-(sp)       // Get number of milliseconds per tick
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
        move    sr, -(sp)               // Disable interrupts
        ori.w   #0x0700, sr

        move.l  _tim_addr,a0            // Invoke user routine
        jsr     (a0)

        move    (sp)+, SR               // Restore interrupt context

#if 0
// see if the mouse needs drawing

        bsr     vb_draw
        clr.b   in_proc         	//allow yet another trip through
#endif
        movem.l (sp)+, d0-d7/a0-a6
timer_exit:
        move.l  _tim_chain, -(sp) 	
        rts



/*
 * _DINIT_G - deinitialize graphics
 *
 * entry:          none
 * exit:           none
 * destroys:       see CLEARMEM
 */

_DINIT_G:
	move.l	_tim_chain, -(sp)       // restore old timer int vector
        move.w	#0x100, -(sp)           // etv_timer
        move.w  #5, -(sp)               // call setexc via
        trap    #13                     // BIOS
        add.l   #8, sp                  // clean up stack

        bsr     dinit_mouse             // de-initialize mouse
        bra     escfn3			// clear screen
                                        // TODO: switch cursor on



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
        move.w  #2, -(sp)		// see if a character present at con
        move.w  #1, -(sp)
        trap    #13
        addq.l  #4, sp

        tst.w   d0
        beq     no_char

        move.w  #2, -(sp)  		// get the character from Bconin
        trap    #13
        addq.l  #4, sp

        move.l  d0, d1
        swap    d1
        lsl.w   #8, d1			// scancode to bit 8-15
        or.w    d1, d0                  // asciicode to bit 0-7

        move.w  d0,_TERM_CH     	// store char
        move.l  #1,d0
        rts
no_char:
        moveq.l #0,d0
        rts


/*
 * mouse_init - INITIALIZE MOUSE
 *
 * entry:          none
 * exit:           none
 * destroys:       d0/a0-a1
 */

mouse_init:
        lea.l   user_init,a0    	// init user_but and user_mot to 
        move.l  a0,_user_but     	// just rts
        move.l  a0,_user_mot            // just rts
        move.l  #_MOV_CUR,_user_cur     // initialize user_cur vector

	/* Move in the default mouse form (presently the arrow) */

        move.l  _INTIN, -(sp)           // Save the intin pointer and make
        move.l  #arrow_cdb, _INTIN      // it point to the arrow

        bsr     _XFM_CRFM               // transform mouse

        move.l  (sp)+, _INTIN           // Restore INTIN pointer


	/* Init mouse interrupt variables */

        moveq.l #0,d0
        move.w  d0,cur_ms_stat  	// init cur_mouse_stat
        move.b  d0,_mouse_flag  	// interrupts can happen
        move.b  d0,_draw_flag   	// No action yet on vblank

	lea	_mdata, a0              // begin off mouse data (lineavars)
        move.w	d0, (a0)+               // set mouse x (_newx) to 0
        move.w	d0, (a0)+               // set mouse y (_newy) to 0
        move.b	d0, (a0)                // set draw flag (_draw_flag) to 0
	move.l	_vblqueue, a0           // get VBL-queue address
        move.l	#vb_draw, (a0)          // set GEM VBL-routine (in vdimouse.S)
        
        move    sr, -(sp)               // Disable interrupts
        ori.w   #0x0700, sr

        pea     int_mouse               // Get character vector for
	pea     arrow_cdb               // get arrow bitmap data
        move.w  #0x1, -(sp)             // enable mouse in relative mode
        move.w  #0x0, -(sp)             // Initmous
        trap    #14			// call XBIOS

        //move.l  d0, mouse_chain         // Set up chain before ints go on

        add.l   #12, sp                 // clean up stack

        move    (sp)+, sr               // Restore interrupt context

        rts


/*
 * user_init - Dummy function for vex* vectors
 *
 * vex_* vectors point to here initially, till VDI takes them over
 */

user_init:
        rts



/*
 * dinit_mouse - deinitialize mouse
 *
 * Disables the mouse
 *
 *  entry:          none   
 *  exit:           none   
 *  destroys:       nothing
 */
 
dinit_mouse:

        move    sr, -(sp)               // Disable interrupts
        ori.w   #0x0700, sr

        //move.l  d0, mouse_chain         // Set up chain before ints go on

        pea     int_mouse               // Get character vector for
	pea     arrow_cdb               // get arrow bitmap data
        move.w  #0x0, -(sp)             // disable mouse in relative mode
        move.w  #0x0, -(sp)             // Initmous
        trap    #14			// call XBIOS
        add.l   #12, sp                 // clean up stack

        move    (sp)+, sr               // Restore interrupt context
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
        move.b  cur_ms_stat,d0
        move.b  d0,d1           // save state
        andi.b  #0xc0,d0                // mask out all but button status bits
        cmpi.b  #0,d0           // see if mouse status changed
        beq     key_stat        // nothing go check key stat
        move.w  #0x20,_TERM_CH  // send terminator code for left key

clr_bt_stat:
        andi.b  #0x23,d0                // clear mouse button status
        move.b  d0,cur_ms_stat  // store cleared state
        move.w  #1,d0           // set button pressed flag
        rts

key_stat:
	move.w  #handle, -(sp)  // see if a character present at aux port
        move.w  #1, -(sp)
        trap    #13
        addq.l  #4, sp

        tst.w   d0
        beq     mov_stat        // no char

        move.w  #handle, -(sp)  // get the character
        move.w  #2, -(sp)       // from auxin
        trap    #13
        addq.l  #4, sp
        move.l  d0, d1
        swap    d1
        lsl.w   #8, d1
        or.w    d1, d0
        move.w  d0,_TERM_CH     // store char
        move.l  #1,d0
        rts
mov_stat:
        bclr.l  #5,d1           // test and clear mouse motion flag 
        beq     no_chg

        move.b  d1,cur_ms_stat  // save mouse stat
        move.w  _GCURX,_X1      // set _X1 = _GCURX
        move.w  _GCURY,_Y1      // set _Y1 = _GCURY
        move.l  #2,d0
        rts

no_chg:  
	move.l  #0,d0
        rts
        
        
        
/*
 * int_mouse - Mouse interrupt routine
 * entry:  8(sp) = address of mouse buffer
 * exit:           none                   
 * destroys:       nothing                
 */
 
int_mouse:
        tst.b   _mouse_flag     // If we are in a show/hide operation
        bne     ms_exit         // just exit.

        movem.l d0-d7/a0-a6, -(sp)      // save registers used
        move.b  (a0), d0                // see if we have a mouse packet
        move.b	d0, d1                  // save packet
        andi.b  #0xf8, d1               // mask out for ...
        cmp.b   #0xf8, d1               // relative packet header?
        bne     ms_done

	and.w   #3, d0          // isolate mouse buttons
        lsr.b   #1, d0          // left button pressed?
        bcc     no_left         // no
        bset    #1, d0          // set bit 0 for left button 

no_left:
        move.b  cur_ms_stat,d1  // get previous mouse state

        andi.w  #3,d1           // mask out state codes bits 6,7
        cmp.b   d1,d0           // has button state changed
        beq     xy_update       // no go test x,y change

        move.w  d1,-(sp)        // save previous mouse state
        move.l  _user_but,a1    // get user routine address
        move.w  sr, -(sp)       // Call user with interrupts off
        ori.w   #0x0700, sr
        jsr     (a1)

        move.w  (sp)+, sr       // restore interrupt context
        move.w  (sp)+, d1       // get back previous mouse button state
        move.w  d0,_MOUSE_BT

        eor.b   d0, d1          // compute which buttons have changed
        ror.b   #2, d1          // put deltas in bits 6 & 7
        or.b    d1, d0          // combine deltas and states

        move.b  d0, cur_ms_stat // store change in button stat

xy_update:
        move.b  1(a0), d0
        or.b    2(a0), d0
        bne     new_coord

        bclr.b  #5, cur_ms_stat // no motion clear motion status
        bra     ms_done         // no change done bye-bye

new_coord:
        bset.b  #5, cur_ms_stat // motion set motion status

        move.w  _GCURX, d0      // init d0 to _GCURX
        move.b  1(a0), d1

        ext.w   d1              // sign extend to word
        add.w   d1, d0          // d0 contains new x coordinate

        move.w  _GCURY,d1       // init d1 to _GCURY
        move.b  2(a0), d3
        ext.w   d3              // sign extend to word
        add.w   d3, d1          // d1 contains new x coordinate

        bsr     scrn_clip
        move.l  _user_mot,a1    // get user routine address
        move.w  sr, -(sp)       // Call user with interrupts off
        ori.w   #0x0700, sr
        jsr     (a1)            // call user to modify x,y
        move.w  (sp)+, SR       // restore interrupt context
        bsr     scrn_clip

        move.w  d0,_GCURX       // save new X location
        move.w  d1,_GCURY       // save new y location

        move.l  _user_cur,a1     // get user routine address
        move.w  SR, -(sp)       // Call user with interrupts off
        ori.w   #0x0700, sr
        jsr     (a1)            // call user to draw cursor
        move.w  (sp)+, sr       // restore interrupt context
//                                      // d0 contains new x position
//                                      // d1 contains new y position
ms_done:
        movem.l (sp)+,d0-d7/a0-a5       // restore old registers
        unlk    a6
ms_exit:
// FIXME: The following line was again not Atari compatible:
//      move.l  mouse_chain, -(sp)      // chain to next guy
        rts






                .bss
//_v_bas_ad     ds.l    1   // FIXME: Already declared in tosvars.S ?

in_proc:        ds.b    1       // non-zero if my timer code is running
ms_buffer:      ds.b    1       // mouse_buffer (header and button info)
delta_x:        ds.b    1       // change in x pos. 2's comp
delta_y:        ds.b    1       // change in y pos. 2's comp

// Clock and mouse device handles and names - not usable on the Atari...  - Thomas
#if 0

mouse_chain:    ds.l    1
clock_handle    ds.w    1
mouse_handle    ds.w    1

                .data
clock           .dc.b   'CLOCK:',0
mouse           .dc.b   'MOUSE:',0

#endif
                .end
