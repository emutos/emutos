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



        /* globl routines declared here */
        .globl  _mouse_int


        /* External routines used here */
        .xdef  scrn_clip

        /* External variables used here */
        .xdef   _GCURX          
        .xdef   _GCURY
        .xdef   _HIDE_CNT
        .xdef   _MOUSE_BT
        
        .xdef   _cur_ms_stat
        .xdef   _draw_flag
        .xdef   _mouse_flag
        .xdef   _user_but
        .xdef   _user_cur
        .xdef   _user_mot


        .text



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
        
        move.b  (a0), d0                // see if we have a mouse packet
        move.b  d0, d1                  // save packet
        and.b   #0xf8, d1               // mask out for ...
        cmp.b   #0xf8, d1               // relative mouse packet header?

        bne     ms_done

        and.w   #3, d0          // isolate mouse buttons
        lsr.b   #1, d0          // left button pressed?
        bcc     no_left         // no
        bset    #1, d0          // set bit 0 for left button 

no_left:
        move.b  _cur_ms_stat,d1  // get previous mouse state

        andi.w  #3,d1           // mask out state codes bits 6,7
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

        move.b  d0, _cur_ms_stat // store change in button stat

xy_update:
        move.b  1(a0), d0
        or.b    2(a0), d0
        bne     new_coord

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



        .end
