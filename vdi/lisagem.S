/*
 * lisagem.S - VDI initialization and timer vector stuff
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "vdiconf.h"


/* Global declarations */

        .globl  _gfx_init
        .globl  _gfx_exit
        .globl  _vex_timv

        .globl  scrn_clip
        .globl  escfn2          // enter graphics mode
        .globl  escfn3          // exit graphics mode

/* External declarations */

        .xdef   _CONTRL     
        .xdef   _GCURX
        .xdef   _GCURY
        .xdef   _INTIN
        .xdef   _INTOUT
        .xdef   _TERM_CH
        .xdef   _v_lin_wr
        .xdef   _v_planes
        .xdef   _X1     
        .xdef   _Y1
        .xdef   arrow_cdb
        .xdef   _cur_ms_stat
        .xdef   _tim_addr

        .xdef   vdimouse_init
        .xdef   vdimouse_exit

        .text



/*
 * _gfx_init - Initialize graphics subsystem
 *
 * entry:          none
 * exit:           none
 * destroys:       see _v_clrwk
 */

_gfx_init:
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
 * _gfx_exit - deinitialize graphics subsystem
 *
 * entry:          none
 * exit:           none
 * destroys:       see _v_clrwk
 */

_gfx_exit:
        move.l  _tim_chain, -(sp)       // restore old timer int vector
        move.w  #0x100, -(sp)           // etv_timer
        move.w  #5, -(sp)               // call setexc via
        trap    #13                     // BIOS
        add.l   #8, sp                  // clean up stack

        bsr     vdimouse_exit           // de-initialize mouse
        bra     escfn3                  // clear screen
                                        // TODO: switch cursor on



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
        clr.b   in_proc                 // allow yet another trip through
timer_exit:
        move.l  _tim_chain, -(sp)       // Call the old timer vector too
        rts



just_rts:
        rts



                .bss

in_proc:        ds.b    1       // non-zero if my timer code is running




                .end
