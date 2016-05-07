/*
 * vdi_misc.c - everything, what does not fit in elsewhere
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "asm.h"
#include "biosbind.h"
#include "../bios/tosvars.h"
#include "vdi_defs.h"
#include "../bios/lineavars.h"



/* timer related vectors (linea variables in bios/lineavars.S) */

extern void     (*tim_addr)(int);       // timer interrupt vector
extern void     (*tim_chain)(int);      // timer interrupt vector save



static BOOL in_proc;                   /* flag, if we are still running */



/*
 * arb_corner - copy and sort (arbitrate) the corners
 *
 * raster (ll, ur) format is desired.
 */
void arb_corner(Rect * rect)
{
    /* Fix the x coordinate values, if necessary. */
    if (rect->x1 > rect->x2) {
        WORD temp = rect->x1;
        rect->x1 = rect->x2;
        rect->x2 = temp;
    }

    /* Fix the y coordinate values, if necessary. */
    if (rect->y1 > rect->y2) {
        WORD temp = rect->y1;
        rect->y1 = rect->y2;
        rect->y2 = temp;
    }
}



/*
 * arb_line - copy and sort (arbitrate) the lines coordinates
 *
 * traditional (ll, ur) format is desired.
 */
void arb_line(Line * line)
{
    /* Fix the x coordinate values, if necessary. */
    if (line->x1 > line->x2) {
        WORD temp = line->x1;
        line->x1 = line->x2;
        line->x2 = temp;
    }

    /* Fix the y coordinate values, if necessary. */
    if (line->y1 < line->y2) {
        WORD temp = line->y1;
        line->y1 = line->y2;
        line->y2 = temp;
    }
}



/*
 * tick_int -  VDI Timer interrupt routine
 *
 * The etv_timer does point to this routine
 */

static void tick_int(int u)
{
    if (!in_proc) {
        in_proc = 1;                    // set flag, that we are running
                                        // MAD: evtl. registers to stack
        (*tim_addr)(u);                 // call the timer vector
                                        // and back from stack
    }
    in_proc = 0;                        // allow yet another trip through
                                        // MAD: evtl. registers to stack
    (*tim_chain)(u);                    // call the old timer vector too
                                        // and back from stack
}



/*
 * vdi_vex_timv - exchange timer interrupt vector
 *
 * entry:          new vector in CONTRL[7-8]
 * exit:           old vector in CONTRL[9-10]
 */
void vdi_vex_timv(Vwk * vwk)
{
    WORD old_sr;
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];

    old_sr = set_sr(0x2700);

    *pointer = (LONG) tim_addr;
    tim_addr = (void (*)(int)) *--pointer;

    set_sr(old_sr);

    INTOUT[0] = (WORD)Tickcal();        /* ms between timer C calls */
    CONTRL[4] = 1;
}



/*
 * do_nothing - doesn't do much  :-)
 */

static void do_nothing_int(int u)
{
    (void)u;
}



/*
 * timer_init - initialize the timer
 *
 * initially set timer vector to dummy, save old vector
 */

void timer_init(Vwk * vwk)
{
    WORD old_sr;

    in_proc = 0;                        // no vblanks in process

    /* Now initialize the lower level things */
    tim_addr = do_nothing_int;          // tick points to rts

    old_sr = set_sr(0x2700);            // disable interrupts
    tim_chain = (void(*)(int))          // save old vector
        Setexc(0x100, (long)tick_int);  // set etv_timer to tick_int
    set_sr(old_sr);                     // enable interrupts

}



/*
 * timer_exit - de-initialize the time
 *
 * reactivate the old saved vector
 */

void timer_exit(Vwk * vwk)
{
    WORD old_sr;

    old_sr = set_sr(0x2700);            // disable interrupts
    Setexc(0x100, (long)tim_chain);     // set etv_timer to tick_int
    set_sr(old_sr);                     // enable interrupts
}



UWORD * get_start_addr(const WORD x, const WORD y)
{
    UBYTE * addr;

    /* init adress counter */
    addr = v_bas_ad;                    /* start of screen */
    addr += (x&0xfff0)>>shift_offset[v_planes]; /* add x coordinate part of addr */
    addr += (LONG)y * v_lin_wr;         /* add y coordinate part of addr */

    return (UWORD*)addr;
}
