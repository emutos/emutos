/*
 * vmouse.c - C part of the mouse stuff
 *
 * Copyright (c) 2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "gsxdef.h"
#include "gsxextrn.h"

#include "vdiconf.h"
#include "lineavars.h"
#include "asm.h"



extern long trap13(int, ...);

#define tickcal() (WORD)trap13(0x06)            /* ms between timer C calls */



/*
 * vex_butv
 *
 * This routine replaces the mouse button change vector with
 * the address of a user-supplied routine.  The previous value
 * is returned so that it also may be called when there is a
 * change in the mouse button status.
 *
 * Inputs:
 *    contrl[7], contrl[8] - pointer to user routine
 *
 * Outputs:
 *    contrl[9], contrl[10] - pointer to old routine
 *
 * Registers Modified:     a0
 */

void vex_butv()
{
    LONG * pointer;

    pointer = (LONG*)&CONTRL[9];
    *pointer = (LONG)user_but;
    (LONG*)user_but = *--pointer;
}



/*
 * vex_motv
 *
 * This routine replaces the mouse coordinate change vector with the address
 * of a user-supplied routine.  The previous value is returned so that it
 * also may be called when there is a change in the mouse coordinates.
 *
 *  Inputs:
 *     contrl[7], contrl[8] - pointer to user routine
 *
 *  Outputs:
 *     contrl[9], contrl[10] - pointer to old routine
 *
 *  Registers Modified:     a0
 */

void vex_motv()
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_mot;
    (LONG*)user_mot = *--pointer;
}



/*
 * vex_curv
 *
 * This routine replaces the mouse draw vector with the
 * address of a user-supplied routine.  The previous value
 * is returned so that it also may be called when the mouse
 * is to be drawn.
 *
 * Inputs:
 *    contrl[7], contrl[8] - pointer to user routine
 *
 * Outputs:
 *    contrl[9], contrl[10] - pointer to old routine
 *
 * Registers Modified:     a0
 */

void vex_curv()
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_cur;
    (LONG*)user_cur = *--pointer;
}



/*
 * vex_timv - exchange timer interrupt vector
 * 
 * entry:          new vector in CONTRL[7-8]
 * exit:           old vector in CONTRL[9-10]
 * destroys:       a0
 */

void vex_timv()
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];

    ints_off();

    *pointer = (LONG) tim_addr;
    (LONG*)tim_addr = *--pointer;

    ints_on();

    INTOUT[0] = tickcal();
}
