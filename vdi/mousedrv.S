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
        .globl  _mov_cur        // set's new x/y coordinates
        .globl  _mouse_int      // mouse interrupt routine

        /* Global variables declared here */

        /* External routines used here */
        .xdef   _DEV_TAB        // address of device table
        .xdef   _xfm_crfm
        .xdef   _vb_draw
        .xdef   _user_but       // user button vector
        .xdef   _user_mot       // user motion vector
        .xdef   _user_cur       // user cursor vector

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




        .text


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





        .end
