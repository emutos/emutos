/*
 * vdi_asm.S - Front end of the screen driver and mouse stuff
 *
 * Copyright 1999 Caldera, Inc. and Authors:
 * Copyright 2004-2024 The EmuTOS development team
 * Copyright      Steve Cavender
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"
#include "vdiext.h"

        .extern _local_pb
        .extern _CONTRL
        .extern _flip_y
        .extern _screen
        .extern _vdishare               //see vdi_fill.c

#define lcl_ptsin _vdishare

/*
 * _vditrap - VDI trap dispatcher
 *
 * This functions checks for a trap-2 VDI call and jumps to the VDI dispatcher
 * then. It also provides a simple vq_gdos call and the "d0=-1" query function
 * that returns the dispatcher's address.
 *
 * Note that this function does not care about AES calls since the AES later
 * hooks into the trap-2 vector itself.
 */
        .globl  _vditrap
_vditrap:
        cmp.w   #0x73,d0                // VDI call?
        jne     no_vdi
        jsr     _GSX_ENTRY
        rte

no_vdi:
        cmp.w   #0xffff,d0              // query for jsr:able address?
        jeq     query

/*
 * Just like Atari TOS, we leave d0 unchanged, indicating the absence of
 * GDOS if this is called with -2 in the low-order word of d0.
 */
        rte

query:
        move.l  #_GSX_ENTRY,d0          // Return address of VDI dispatcher
        rte

/*
 * _GSX_ENTRY - front end of the GIOS for the screen driver
 */

#define W_1        2            // byte offset to first element of an array


        .text

        .globl  _GSX_ENTRY
_GSX_ENTRY:
#ifdef __mcoldfire__
        lea     -56(sp),sp
        movem.l d1-d7/a0-a6,(sp)        //Save the callers registers
#else
        movem.l d1-d7/a0-a6,-(sp)       //Save the callers registers
#endif

/* Make a local copy of the array pointers in the user's parameter block. */

        movea.l d1,a2           // get address of user's parameter block
        lea     _local_pb, a1    // get address of local storage area
        movea.l (a2)+,a0        // copy address of user's CONTRL aray
        move.l  a0,(a1)+        // save address of user's CONTRL array
        move.l  (a2)+,(a1)+     // save address of user's INTIN  array
        lea     lcl_ptsin,a3    // shared area used for local copy of PTSIN array
        move.l  a3,(a1)+        // save address of local PTSIN array
        move.l  (a2)+,a4        // get address of user's PTSIN array
        move.l  (a2)+,(a1)+     // save address of user's INTOUT array
        move.l  (a2),(a1)       // save address of user's PTSOUT array

/* Save a copy of the user's CONTRL[1] element on the stack, in case we change it. */
#ifdef __mcoldfire__
        moveq   #0,d0
#endif
        move.w  2(a0),d0        // get number of PTSIN entries from CONTRL[1]

/* Validate the number of entries in user's PTSIN array. */
        move.w  d0,-(sp)        // save number of PTSIN entries from CONTRL[1]
        jeq     no_ptsin        // for number of vertices = 0
size_ptsin:
        cmpi.w  #MAX_VERTICES,d0  // # of user vertices > # of allowable vertices?
        jle     ptsin_ok        // for user vertices =< # of allowable vertices
        move.w  #MAX_VERTICES,d0  // user vertices > # of allowable vertices
        move.w  d0,2(a0)        // put max # of vertices in user's CONTRL[1]
ptsin_ok:
        subq.l  #1,d0           // adjust coordinate count for loop
save_ptsin:
        move.l  (a4)+,(a3)+     // save x,y coordinate pair
#ifdef __mcoldfire__
        subq.l  #1,d0
        jpl     save_ptsin      // point to next x,y coordinate pair
#else
        dbra    d0,save_ptsin   // point to next x,y coordinate pair
#endif

no_ptsin:
/* Call screen which contains all the C routines for the SCREEN DRIVER. */
        jsr     _screen

/* Restore user's CONTRL[1] element from the stack. */
        movea.l _CONTRL,a0      // get address of user's CONTRL array
        move.w  (sp)+,2(a0)     // original # of PTSIN entries to CONTRL[1]

#ifdef __mcoldfire__
        movem.l (sp), d1-d7/a0-a6  // restore the caller's registers
        lea     56(sp), sp
#else
        movem.l (sp)+, d1-d7/a0-a6 // restore the caller's registers
#endif
        moveq   #0,d0
        move.w  _flip_y,d0      // set d0 to indicate magnitude/coordinate

        rts

#define F_SAVRDY        0               // save buffer status flag: 0:empty  1:full
#define F_SAVWID        1               // saved line width        0:word   1:longword

        .text

        /* Global variables declared here */

        .globl  _mov_cur        // set's new x/y coordinates
        .globl  _mouse_int      // mouse interrupt routine
#if CONF_WITH_EXTENDED_MOUSE
        .globl  _wheel_int      // wheel interrupt routine
        .globl  _call_user_but  // call the _user_but vector from C
        .globl  _call_user_wheel// call the _user_wheel vector from C
#endif

        /* External variables used here */

        .extern _GCURX          // graphics cursor x-coordinate
        .extern _GCURY          // graphics cursor y-coordinate
        .extern _HIDE_CNT       // graphics cursor hide count
        .extern _MOUSE_BT       // mouse button state

        .extern _cur_ms_stat    // current mouse status
        .extern _draw_flag      // Non-zero means draw mouse form on vblank
        .extern _newx           // New mouse x position
        .extern _newy           // New mouse y position

        .extern _user_but       // user button vector
        .extern _user_mot       // user motion vector
        .extern _user_cur       // user cursor vector
#if CONF_WITH_EXTENDED_MOUSE
        .extern _user_wheel     // user wheel vector
        .extern _old_statvec    // previous IKBD status packet vector
#endif

        .extern _DEV_TAB        // address of device table

        .text

/*
 * scrn_clip - This routine clips the x and y-coordinates of the cursor
 *             position to the limits of the screen.
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
        jge     cl_xmax         // nope - check if greater than maximum value
        clr.w   d0              // yes - set it to 0
        jra     cl_y            // now clip the y-coordinate

/*
 *      If the new x-coordinate is greater than the maximum value then the
 *      cursor will be placed off the right side of the screen so set it
 *      to the maximum value.
 */
cl_xmax:
        cmp.w   (a0),d0         // is new x-coordinate > maximum value?
        jle     cl_y            // nope - go clip the y-coordinate
        move.w  (a0),d0         // yes - set it to the maximum value
/*
 *      If the new y-coordinate is less than 0 then the cursor will be
 *      placed off the top of the screen so set its value to 0.
 */
cl_y:
        tst.w   d1              // is new y-coordinate < 0?
        jge     cl_ymax         // nope - check if greater than maximum value
        clr.w   d1              // yes - set it to 0
        rts

/*
 *      If the new y-coordinate is greater than the maximum value then the
 *      cursor will be placed off the bottom of the screen so set it
 *      to the maximum value.
 */
cl_ymax:
        cmp.w   2(a0),d1        // is new y-coordinate > maximum value?
        jle     cl_done         // nope - done
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
#ifdef __mcoldfire__
        lea     -60(sp),sp
        movem.l d0-d7/a0-a6, (sp)       // save registers used
#else
        movem.l d0-d7/a0-a6, -(sp)      // save registers used
#endif

        move.b  (a0), d0        // see if we have a mouse packet
        move.b  d0, d1          // save packet
#ifdef __mcoldfire__
        and.l   #0xf8, d1       // mask out for ...
#else
        and.b   #0xf8, d1       // mask out for ...
#endif
        cmp.b   #0xf8, d1       // relative mouse packet header?

        jne     ms_done

#ifdef __mcoldfire__
        and.l   #3, d0          // isolate mouse buttons
        lsr.l   #1, d0          // left button pressed?
#else
        and.w   #3, d0          // isolate mouse buttons
        lsr.b   #1, d0          // left button pressed?
#endif
        jcc     no_left         // no
        bset    #1, d0          // set bit 0 for left button

no_left:
        move.b  _cur_ms_stat,d1 // get previous mouse state

#ifdef __mcoldfire__
        and.l   #3,d1           // mask out state codes bits 6,7
#else
        and.w   #3,d1           // mask out state codes bits 6,7
#endif
        cmp.b   d1,d0           // has button state changed
        jeq     xy_update       // no go test x,y change

        lea     _MOUSE_BT,a1
        move.w  (a1),d2
#ifdef __mcoldfire__
        andi.l  #0xfffffffc,d2
        or.l    d2,d0           // keep additional mouse button states
#else
        andi.w  #0xfffc,d2
        or.w    d2,d0           // keep additional mouse button states
#endif
        move.w  d0,(a1)

        move.w  d1,-(sp)        // save previous mouse state
        move.l  _user_but,a1    // get user routine address
        jsr     (a1)

        move.w  (sp)+, d1       // get back previous mouse button state

#ifdef __mcoldfire__
        and.l   #0xffff,d0
        and.l   #0xffff,d1
        eor.l   d0, d1          // compute which buttons have changed
        lsl.l   #6, d1          // put deltas in bits 6 & 7
        or.l    d1, d0          // combine deltas and states
#else
        eor.b   d0, d1          // compute which buttons have changed
        ror.b   #2, d1          // put deltas in bits 6 & 7
        or.b    d1, d0          // combine deltas and states
#endif

        move.b  d0,_cur_ms_stat // store change in button stat

xy_update:
#ifdef __mcoldfire__
        tst.b   1(a0)           // get new relative x pos
        jne     new_coord       // just check for movement
        tst.b   2(a0)           // get new relative y pos
        jne     new_coord       // just check for movement
#else
        move.b  1(a0), d0       // get new relative x pos
        or.b    2(a0), d0       // get new relative y pos
        jne     new_coord       // just check for movement
#endif

#ifdef __mcoldfire__
        lea     _cur_ms_stat, a1
        bclr.b  #5, (a1)         // no motion clear motion status
#else
        bclr.b  #5, _cur_ms_stat // no motion clear motion status
#endif
        jra     ms_done         // no change done bye-bye

new_coord:
#ifdef __mcoldfire__
        lea     _cur_ms_stat, a1
        bset.b  #5, (a1)         // motion set motion status
#else
        bset.b  #5, _cur_ms_stat // motion set motion status
#endif

        move.w  _GCURX, d0      // set d0 to _GCURX
#ifdef __mcoldfire__
        ext.l   d0
#endif
        move.b  1(a0), d1

        ext.w   d1              // sign extend to word
#ifdef __mcoldfire__
        ext.l   d1
        add.l   d1, d0          // d0 contains new x coordinate
#else
        add.w   d1, d0          // d0 contains new x coordinate
#endif

        move.w  _GCURY,d1       // init d1 to _GCURY
#ifdef __mcoldfire__
        ext.l   d1
#endif
        move.b  2(a0), d3
        ext.w   d3              // sign extend to word
#ifdef __mcoldfire__
        ext.l   d3
        add.l   d3, d1          // d1 contains new x coordinate
#else
        add.w   d3, d1          // d1 contains new x coordinate
#endif

        jbsr    scrn_clip

        move.l  _user_mot,a1    // get user routine address
        jsr     (a1)            // call user to modify x,y

        jbsr    scrn_clip

        move.w  d0,_GCURX       // save new X location
        move.w  d1,_GCURY       // save new y location

        move.l  _user_cur,a1    // get user routine address
        jsr     (a1)            // call user to draw cursor

                                // d0 contains new x position
                                // d1 contains new y position
ms_done:
#ifdef __mcoldfire__
        movem.l (sp),d0-d7/a0-a6        // restore old registers
        lea     60(sp),sp
#else
        movem.l (sp)+,d0-d7/a0-a6       // restore old registers
#endif
ms_exit:
        rts

#if CONF_WITH_EXTENDED_MOUSE

/*
 * _wheel_int - Mouse wheel interrupt routine
 * entry:       a0 = address of IKBD status packet buffer from aciavecs.S
 * exit:        none
 * destroys:    nothing
 */

_wheel_int:
#ifdef __mcoldfire__
        lea     -16(sp),sp
        movem.l d0-d1/a0-a1,(sp)         // Save only necessary registers
#else
        movem.l d0-d1/a0-a1,-(sp)        // Save only necessary registers
#endif

        // Could this be a wheel packet?
        lea     wheel_ikbd_packet(pc),a1
        moveq   #0,d0
        moveq   #5,d1
wheel_check:
        move.b  (a0)+,d0
        cmp.b   (a1)+,d0
#ifdef __mcoldfire__
        jne      wheel_exit
        subq.l   #1,d1
        jpl      wheel_check
#else
        dbne    d1,wheel_check
        jne     wheel_exit
#endif

#ifdef __mcoldfire__
        moveq   #0,d0
        move.b  (a0)+,d0
        sub.l   #0x59,d0
#else
        move.b  (a0)+,d0
        sub.b   #0x59,d0
#endif
        jmi     wheel_exit
        cmp.b   #3,d0
        jhi     wheel_exit

        move.w  d0,d1
#ifdef __mcoldfire__
        lsr.l   #1,d0                    // Wheel number

        and.l   #1,d1                    // 0 - up,  1 - down
        add.l   d1,d1                    // 0 - up,  2 - down
//      neg.l   d1                       // 0 - up, -2 - down
//      addq.l  #1,d1                    // 1 - up, -1 - down
        subq.l  #1,d1                    // -1 - up, 1 - down
#else
        lsr.b   #1,d0                    // Wheel number

        and.w   #1,d1                    // 0 - up,  1 - down
        add.w   d1,d1                    // 0 - up,  2 - down
//      neg.w   d1                       // 0 - up, -2 - down
//      addq.w  #1,d1                    // 1 - up, -1 - down
        subq.w  #1,d1                    // -1 - up, 1 - down
#endif

#ifdef __mcoldfire__
        lea     -44(sp),sp
        movem.l d2-d7/a2-a6,(sp)         // Save remaining register to be safe
#else
        movem.l d2-d7/a2-a6,-(sp)        // Save remaining register to be safe
#endif
        move.l  _user_wheel,a1
        jsr     (a1)
#ifdef __mcoldfire__
        movem.l (sp),d2-d7/a2-a6
        lea     44(sp),sp
#else
        movem.l (sp)+,d2-d7/a2-a6
#endif

#ifdef __mcoldfire__
        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
#else
        movem.l (sp)+,d0-d1/a0-a1
#endif
        rts                              // Packet has been dealt with

wheel_exit:
#ifdef __mcoldfire__
        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
#else
        movem.l (sp)+,d0-d1/a0-a1
#endif
        move.l  _old_statvec,-(sp)       // Go to old IKBD status vector
        rts

wheel_ikbd_packet:
        .dc.b   0x05,0x00,0x00,0x00,0x00,0x00

#endif /* CONF_WITH_EXTENDED_MOUSE */

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
        jne     mov_cur1        // yes - don't draw it now

/*
 * The cursor is not currently hidden.  Save the new coordinates for the
 * cursor and set a flag to signal the need to redraw it.  The cursor will
 * actually be drawn by vb_draw() (in vdi_mouse.c) which has been added to
 * the VBL handler list.  This is a critical region so all interrupts must
 * be turned off.
 */
#ifdef __mcoldfire__
        move.l  d3,-(sp)
        move.l  d2,-(sp)
        move.w  sr,d2           // save current value of status register
        move.w  d2,d3
        ori.l   #0x0700,d3      // mask off all interrupts
        move.w  d3,sr
#else
        move.w  sr,-(sp)        // save current value of status register
        ori.w   #0x0700,sr      // mask off all interrupts
#endif

        move.w  d0,_newx        // save new cursor x-coordinate
        move.w  d1,_newy        // save new cursor y-coordinate

#ifdef __mcoldfire__
        moveq   #0,d3
        bset.b  d3,_draw_flag   // set the 'draw cursor' flag
        move.w  d2,sr           // restore the value of the status register
        move.l  (sp)+,d2
        move.l  (sp)+,d3
#else
        bset.b  #0,_draw_flag   // set the 'draw cursor' flag
        move.w  (sp)+,sr        // restore the value of the status register
#endif
mov_cur1:
        rts

#if CONF_WITH_EXTENDED_MOUSE

/*
 * Call the _user_but vector from C.
 */
_call_user_but:
        move.w  4(sp),d0
        move.l  _user_but,a0
        jmp     (a0)

/*
 * Call the _user_wheel vector from C.
 */
_call_user_wheel:
        move.w  4(sp),d0
        move.w  6(sp),d1
        move.l  _user_wheel,a0
        jmp     (a0)

#endif /* CONF_WITH_EXTENDED_MOUSE */
