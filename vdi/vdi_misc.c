/*
 * vdi_misc.c - everything, what does not fit in elsewhere
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "asm.h"
#include "biosbind.h"
#include "tosvars.h"
#include "vdi_defs.h"
#include "lineavars.h"



BOOL in_proc;                   /* flag, if we are still running */

/*
 * arb_corner - copy and sort (arbitrate) the corners
 *
 * raster (ll, ur) format is desired.
 */
void arb_corner(WORD * corners)
{
    /* Local declarations. */
    WORD *xy1, *xy2;

    /* Fix the x coordinate values, if necessary. */
    xy1 = corners;
    xy2 = corners + 2;
    if (*xy1 > *xy2) {
        WORD temp = *xy1;
        *xy1 = *xy2;
        *xy2 = temp;
    }

    /* Fix the y coordinate values, if necessary. */
    xy1++;
    xy2++;
    if (*xy1 > *xy2) {
        WORD temp = *xy1;
        *xy1 = *xy2;
        *xy2 = temp;
    }
}



/*
 * arb_corner - copy and sort (arbitrate) the corners
 *
 * traditional (ll, ur) format is desired.
 */
void arb_corner_llur(WORD * corners)
{
    /* Local declarations. */
    WORD temp;
    WORD *xy1, *xy2;

    /* Fix the x coordinate values, if necessary. */
    xy1 = corners;
    xy2 = corners + 2;
    if (*xy1 > *xy2) {
        temp = *xy1;
        *xy1 = *xy2;
        *xy2 = temp;
    }

    /* Fix the y coordinate values, if necessary. */
    xy1++;
    xy2++;
    if (*xy1 < *xy2) {
        temp = *xy1;
        *xy1 = *xy2;
        *xy2 = temp;
    }
}



WORD Isqrt(ULONG x)
{
    ULONG s1, s2;

    if (x < 2)
        return x;

    s1 = x;
    s2 = 2;
    do {
        s1 /= 2;
        s2 *= 2;
    } while (s1 > s2);

    s2 = (s1 + (s2 / 2)) / 2;

    do {
        s1 = s2;
        s2 = (x / s1 + s1) / 2;
    } while (s1 > s2);

    return (WORD)s1;
}



/*
 * tick_int -  VDI Timer interrupt routine
 *
 * The etv_timer does point to this routine
 */
 
void tick_int(int u)
{
    if (!in_proc) {
        in_proc = 1;                    // set flag, that we are running
        // MAD: evtl. registers to stack
        (*tim_addr)(u);                    // call the timer vector
        // and back from stack
    }
    in_proc = 0;                        // allow yet another trip through
    // MAD: evtl. registers to stack
    (*tim_chain)(u);                       // call the old timer vector too
    // and back from stack
}



/*
 * vex_timv - exchange timer interrupt vector
 * 
 * entry:          new vector in CONTRL[7-8]
 * exit:           old vector in CONTRL[9-10]
 */

void vex_timv(Vwk * vwk)
{
    WORD old_sr;
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];

    old_sr = set_sr(0x2700);

    *pointer = (LONG) tim_addr;
    (LONG*)tim_addr = *--pointer;

    set_sr(old_sr);

    INTOUT[0] = (WORD)Tickcal();        /* ms between timer C calls */
}



/*
 * do_nothing - doesn't do much  :-)
 */

static void do_nothing_int(int u)
{
    (void)u;
}



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
