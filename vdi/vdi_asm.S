/*
 * entry.S - Front end of the screen driver and mouse stuff
 *
 * Copyright 1999 Caldera, Inc. and Authors:
 * Copyright 2004 The EmuTOS development team
 * Copyright      Steve Cavender
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "vdi_asm.h"



/*
 * _GSX_ENTRY - front end of the GIOS for the screen driver
 */

        .xdef   _local_pb
        .xdef   _CONTRL
        .xdef   _INTIN
        .xdef   _PTSIN
        .xdef   _INTOUT
        .xdef   _PTSOUT
        .xdef   _flip_y
        .xdef   _screen

        .globl  _GSX_ENTRY


#define ptsin_size 512          // max. # of elements allowed for PTSIN array
#define ptsin_max  ptsin_size/2 // max. # of coordinate pairs for PTSIN array
#define W_1        2            // byte offset to first element of an array
#define W_3        6            // byte offset to third element of an array

        .bss
lcl_ptsin:      .ds.w   ptsin_size

        .text

_GSX_ENTRY:

        movem.l d1-d7/a0-a6,-(sp)       //Save the callers registers

/* Make a local copy of the array pointers in the user's parameter block. */

        movea.l d1,a2           // get address of user's parameter block
        lea     _local_pb, a1    // get address of local storage area
        movea.l (a2)+,a0        // copy address of user's CONTRL aray
        move.l  a0,(a1)+        // save address of user's CONTRL array
        move.l  (a2)+,(a1)+     // save address of user's INTIN  array
        lea     lcl_ptsin,a3    // local area used for PTSIN array address
        move.l  a3,(a1)+        // save address of local PTSIN array
        move.l  (a2)+,a4        // get address of user's PTSIN array
        move.l  (a2)+,(a1)+     // save address of user's INTOUT array
        move.l  (a2),(a1)       // save address of user's PTSOUT array

/* Save a copy of the user's CONTRL[1] and CONTRL[3] elements on the stack. */
/* This routine or the SCREEN DRIVER may change these values. */
        move.w  W_1(a0),d0      // get number of PTSIN entries from CONTRL[1]
        move.w  W_3(a0),d1      // get number of INTIN entries from CONTRL[3]

/* Validate the number of entries in user's PTSIN array. */
        move.w  d0,-(sp)        // save number of PTSIN entries from CONTRL[1]
        beq.b   size_intin      // for number of vertices = 0
        bpl.b   size_ptsin      // for number of vertices > 0
        clr.w   W_1(a0)         // for number of vertices < 0
//                              // set user's CONTRL[1] to 0
        bra.b   size_intin
size_ptsin:
        cmpi.w  #ptsin_max,d0   // # of user vertices > # of allowable vertices?
        ble.b   ptsin_ok        // for user vertices =< # of allowable vertices
        move.w  #ptsin_max,d0   // user vertices > # of allowable vertices
        move.w  d0,W_1(a0)      // put max # of vertices in user's CONTRL[1]
ptsin_ok:
        subq.l  #1,d0           // adjust coordinate count for loop 
save_ptsin:
        move.l  (a4)+,(a3)+     // save x,y coordinate pair
        dbra    d0,save_ptsin   // point to next x,y coordinate pair

/* Validate number of entries in user's INTIN array. */
size_intin:
        move.w  d1,-(sp)        // save number of INTIN entries from CONTRL[3]
        bpl.b   intin_ok        // for user entries > 0
        clr.w   W_3(a0)         // put 0 entries in user's CONTRL[3]

intin_ok:

/* Call screen which contains all the C routines for the SCREEN DRIVER. */
        jsr     _screen

/* Restore user's CONTRL[1] and CONTRL[3] array elements from the stack. */
        movea.l _CONTRL,a0      // get address of user's CONTRL array
        move.w  (sp)+,W_3(a0)   // original # of INTIN entries to CONTRL[3]
        move.w  (sp)+,W_1(a0)   // original # of PTSIN entries to CONTRL[1]

        movem.l (sp)+, d1-d7/a0-a6 // restore the caller's registers
        clr.l   d0
        move.w  _flip_y,d0      // set d0 to indicate magnitude/coordinate 

        rts



#define F_SAVRDY        0               // save buffer status flag: 0:empty  1:full
#define F_SAVWID        1               // saved line width        0:word   1:longword


        .text

        /* Global variables declared here */

        .globl  _mov_cur        // set's new x/y coordinates
        .globl  _mouse_int      // mouse interrupt routine
        .globl  _wheel_int      // wheel interrupt routine

        .globl  _cur_replace  
        .globl  _cur_display
        .globl  _vb_draw        // vertical blank mouse draw routine

        .globl  _mousex
        .globl  _mousey

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
        .xdef   _user_wheel     // user wheel vector

        .xdef   _DEV_TAB        // address of device table
        .xdef   _MAP_COL        // color map table

        .xdef   _old_statvec    // previous IKBD status packet vector


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
 * _wheel_int - Mouse wheel interrupt routine
 * entry:       a0 = address of IKBD status packet buffer from aciavecs.S
 * exit:        none                   
 * destroys:    nothing                
 */

_wheel_int:
        movem.l d0-d1/a0-a1,-(sp)        // Save only necessary registers

        // Could this be a wheel packet?
        lea     wheel_ikbd_packet(pc),a1
        moveq   #0,d0
        moveq   #5,d1
wheel_check:
        move.b  (a0)+,d0
        cmp.b   (a1)+,d0
        dbne    d1,wheel_check
        bne     wheel_exit

        move.b  (a0)+,d0
        sub.b   #0x59,d0
        bmi     wheel_exit
        cmp.b   #3,d0
        bhi     wheel_exit

        move.w  d0,d1
        lsr.b   #1,d0                    // Wheel number

        and.w   #1,d1                    // 0 - up,  1 - down
        add.w   d1,d1                    // 0 - up,  2 - down
        neg.w   d1                       // 0 - up, -2 - down
        addq.w  #1,d1                    // 1 - up, -1 - down

        movem.l d2-d7/a2-a6,-(sp)        // Save remaining register to be safe
        move.l  _user_wheel,a1
        jsr     (a1)
        movem.l (sp)+,d2-d7/a2-a6
                
        movem.l (sp)+,d0-d1/a0-a1
        rts                              // Packet has been dealt with

wheel_exit:
        movem.l (sp)+,d0-d1/a0-a1
        move.l  _old_statvec,-(sp)       // Go to old IKBD status vector
        rts

wheel_ikbd_packet:
        dc.b    0x05,0x00,0x00,0x00,0x00,0x00



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
