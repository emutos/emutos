/*
 * mousedrv.S - BIOS level mouse driver
 *
 * Copyright (c) 1999 Caldera, Inc.
 * Copyright (c) 2001 by Authors of the EmuTOS development team:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "vdiconf.h"



        /* globl routines declared here */
        .globl  vdimouse_init
        .globl  vdimouse_exit

        .globl  _mouse_int      // mouse interrupt routine

        .globl  _vex_butv       // exchange button change vector
        .globl  _vex_motv       // exchange mouse movement vector
        .globl  _vex_curv       // exchange cursor change vector

        /* Global variables declared here */
        .globl  _GCHC_KEY
        .globl  _GCHR_KEY
        .globl  _GLOC_KEY
        .globl  _GSHIFT_S


        /* External routines used here */
        .xdef   _DEV_TAB        // address of device table
        .xdef   _xfm_crfm
        .xdef   vb_draw

        /* External variables used here */
        .xdef   _CONTRL         // address of CONTRL array
        .xdef   _INTIN          // address of INTIN array

        .xdef   _GCURX          // graphics cursor x-coordinate
        .xdef   _GCURY          // graphics cursor y-coordinate
        .xdef   _HIDE_CNT       // graphics cursor hide count
        .xdef   _MOUSE_BT       // mouse button state
        
        .xdef   _cur_ms_stat    // current mouse status
        .xdef   _draw_flag      // Non-zero means draw mouse form on vblank
        .xdef   _mouse_flag     // Non-zero if mouse ints disabled
        .xdef   _newx           // New mouse x position
        .xdef   _newy           // New mouse y position
        .xdef   _user_but       // user button vector
        .xdef   _user_mot       // user motion vector
        .xdef   _user_cur       // user cursor vector




        .text



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
        move.l  #_mov_cur,_user_cur     // initialize user_cur vector


        /* Move in the default mouse form (presently the arrow) */
        move.l  _INTIN, -(sp)           // Save the intin pointer and make
        move.l  #arrow_cdb, _INTIN      // it point to the arrow
        bsr     _xfm_crfm               // transform mouse
        move.l  (sp)+, _INTIN           // restore INTIN pointer

        clr.l   d0
        move.w  d0, _MOUSE_BT           // clear the mouse button state

        move.b  d0, _cur_ms_stat        // clear the mouse status
        move.b  d0, _mouse_flag         // clear the mouse flag
        move.b  d0, _draw_flag          // clear the hide operations counter
        move.w  d0, _newx               // set cursor x-coordinate to 0
        move.w  d0, _newx               // set cursor y-coordinate to 0

        /* Init mouse vbl interrupt */
        move.l  _vblqueue, a0           // get VBL-queue address
        move.l  #vb_draw, (a0)          // set GEM VBL-routine
        
        /* Initialize mouse via XBIOS */
        pea     _mouse_int              // mouse interrupt routine
        pea     arrow_cdb               // mouse parameter block
        move.w  #1, -(sp)               // enable mouse in relative mode
        move.w  #0, -(sp)               // Initmous
        trap    #14                     // call XBIOS
        add.l   #12, sp                 // clean up the stack

just_rts:
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

        /* Init mouse vbl interrupt */
        move.l  _vblqueue, a0           // get VBL-queue address
        move.l  #0, (a0)                // unset GEM VBL-routine
        
        /* Initialize mouse via XBIOS */
        move.l  #-1, -(sp)              // no mouse interrupt routine
        move.l  #-1, -(sp)              // no mouse parameter block
        move.w  #0, -(sp)               // enable mouse in relative mode
        move.w  #0, -(sp)               // Initmous
        trap    #14                     // call XBIOS
        add.l   #12, sp                 // clean up the stack

        rts



/*
 * scrn_clip - This routine clips the x and y-coordinates of the cursor
 *
 * position to the limits of the screen.
 *
 *      Inputs:
 *         d0 = x coordinate
 *         d1 = y coordinate
 *
 *      Outputs:
 *         d0 = clipped to screen x
 *         d1 = clipped to screen y
 *
 *      Registers Modified: a0
 */

/*
 *      If the new x-coordinate is less than 0 then the cursor will be
 *      placed off the left side of the screen so set its value to 0.
 */
scrn_clip:
        lea.l   _DEV_TAB,a0     // get address of device table
        tst.w   d0              // is new x-coordinate < 0?
        bge     cl_xmax         // nope - check if greater than maximum value
        clr.w   d0              // yes - set it to 0
        bra     cl_y            // now clip the y-coordinate

/*
 *      If the new x-coordinate is greater than the maximum value then the
 *      cursor will be placed off the right side of the screen so set it
 *      to the maximum value.
 */
cl_xmax:
        cmp.w   (a0),d0         // is new x-coordinate > maximum value?
        ble     cl_y            // nope - go clip the y-coordinate
        move.w  (a0),d0         // yes - set it to the maximum value
/*
 *      If the new y-coordinate is less than 0 then the cursor will be
 *      placed off the top side of the screen so set its value to 0.
 */
cl_y:
        tst.w   d1              // is new y-coordinate < 0?
        bge     cl_ymax         // nope - check if greater than maximum value
        clr.w   d1              // yes - set it to 0
        rts

/*
 *      If the new x-coordinate is greater than the maximum value then the
 *      cursor will be placed off the right side of the screen so set it
 *      to the maximum value.
 */
cl_ymax:
        cmp.w   2(a0),d1        // is new y-coordinate > maximum value?
        ble     cl_done         // nope - done
        move.w  2(a0),d1        // yes - set it to the maximum value
cl_done:
        rts



/*
 * _mouse_int - Mouse interrupt routine
 * entry:       a0 = address of mouse buffer from aciavecs.S
 * exit:        none                   
 * destroys:    nothing                
 */
 
_mouse_int:
        tst.b   _mouse_flag     // If we are in a show/hide operation
        bne     ms_exit         // just exit.

        movem.l d0-d7/a0-a6, -(sp)      // save registers used

        move.b  (a0), d0        // see if we have a mouse packet
        move.b  d0, d1          // save packet
        and.b   #0xf8, d1       // mask out for ...
        cmp.b   #0xf8, d1       // relative mouse packet header?

        bne     ms_done

        and.w   #3, d0          // isolate mouse buttons
        lsr.b   #1, d0          // left button pressed?
        bcc     no_left         // no
        bset    #1, d0          // set bit 0 for left button 

no_left:
        move.b  _cur_ms_stat,d1 // get previous mouse state

        and.w   #3,d1           // mask out state codes bits 6,7
        cmp.b   d1,d0           // has button state changed
        beq     xy_update       // no go test x,y change

        move.w  d1,-(sp)        // save previous mouse state
        move.l  _user_but,a1    // get user routine address
        jsr     (a1)

        move.w  (sp)+, d1       // get back previous mouse button state
        move.w  d0,_MOUSE_BT

        eor.b   d0, d1          // compute which buttons have changed
        ror.b   #2, d1          // put deltas in bits 6 & 7
        or.b    d1, d0          // combine deltas and states

        move.b  d0,_cur_ms_stat // store change in button stat

xy_update:
        move.b  1(a0), d0       // get new relative x pos
        or.b    2(a0), d0       // get new relative y pos
        bne     new_coord       // just check for movement

        bclr.b  #5, _cur_ms_stat // no motion clear motion status
        bra     ms_done         // no change done bye-bye

new_coord:
        bset.b  #5, _cur_ms_stat // motion set motion status

        move.w  _GCURX, d0      // set d0 to _GCURX
        move.b  1(a0), d1

        ext.w   d1              // sign extend to word
        add.w   d1, d0          // d0 contains new x coordinate

        move.w  _GCURY,d1       // init d1 to _GCURY
        move.b  2(a0), d3
        ext.w   d3              // sign extend to word
        add.w   d3, d1          // d1 contains new x coordinate

        bsr     scrn_clip

        move.l  _user_mot,a1    // get user routine address
        jsr     (a1)            // call user to modify x,y

        bsr     scrn_clip

        move.w  d0,_GCURX       // save new X location
        move.w  d1,_GCURY       // save new y location

        move.l  _user_cur,a1    // get user routine address
        jsr     (a1)            // call user to draw cursor

                                // d0 contains new x position
                                // d1 contains new y position
ms_done:
        movem.l (sp)+,d0-d7/a0-a6       // restore old registers
ms_exit:
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



/*      
 * mov_cur - moves the mouse cursor to its new location
 *           unless the cursor is currently hidden.
 *
 * Inputs:
 *    d0 = new x-coordinate for mouse cursor
 *    d1 = new y-coordinate for mouse cursor
 *
 * Outputs:        None
 *
 * Registers Modified:     None
 */

_mov_cur:
        tst.w   _HIDE_CNT       // is the cursor hidden?
        bne     mov_cur1        // yes - don't draw it now

/*
 * The cursor is not currently hidden.  Save the new coordinates for the
 * cursor, set a flag to signal the need to redraw it, and (if we have
 * no timer or vertical blank tick) then call the routine that will
 * remove the cursor from its current location and redraw it in the new
 * one.  This is a critical region so all interrupts must be turned off.
 */
        move.w  sr,-(sp)        // save current value of status register
        ori.w   #0x0700,sr      // mask off all interrupts
        
        move.w  d0,_newx        // save new cursor x-coordinate
        move.w  d1,_newy        // save new cursor y-coordinate
        bset.b  #0,_draw_flag   // set the 'draw cursor' flag

        move.w  (sp)+,sr        // restore the value of the status register
mov_cur1:
        rts



/*
 *     _vex_butv
 *
 *      This routine replaces the mouse button change vector with
 *      the address of a user-supplied routine.  The previous value
 *      is returned so that it also may be called when there is a
 *      change in the mouse button status.
 *
 *      Inputs:
 *         contrl[7], contrl[8] - pointer to user routine
 *
 *      Outputs:
 *         contrl[9], contrl[10] - pointer to old routine
 *
 *      Registers Modified:     a0
 */

_vex_butv:
        move.l  _CONTRL,a0      // get address of CONTRL array
        move.l  _user_but,18(a0) // store old address in CONTRL
        move.l  14(a0),_user_but // store new routine address
        rts



/*
 *    _vex_motv
 *
 *      This routine replaces the mouse coordinate change vector
 *      with the address of a user-supplied routine.  The previous
 *      value is returned so that it also may be called when there
 *      is a change in the mouse coordinates.
 *
 *      Inputs:
 *         contrl[7], contrl[8] - pointer to user routine
 *
 *      Outputs:
 *         contrl[9], contrl[10] - pointer to old routine
 *
 *      Registers Modified:     a0
 */

_vex_motv:
        move.l  _CONTRL,a0      // get address of CONTRL array
        move.l  _user_mot,18(a0) // store old address in CONTRL
        move.l  14(a0),_user_mot // store new routine address
        rts



/*
 *    _vex_curv
 *
 *      This routine replaces the mouse draw vector with the
 *      address of a user-supplied routine.  The previous value
 *      is returned so that it also may be called when the mouse
 *      is to be drawn.
 *
 *      Inputs:
 *         contrl[7], contrl[8] - pointer to user routine
 *
 *      Outputs:
 *         contrl[9], contrl[10] - pointer to old routine
 *
 *      Registers Modified:     a0
 */

_vex_curv:
        move.l  _CONTRL,a0      // get address of CONTRL array
        move.l  _user_cur,18(a0) // store old address in CONTRL
        move.l  14(a0),_user_cur // store new routine address
        rts



        .end
