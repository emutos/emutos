/*
 * vdi_inp.c - Pointer related input stuff
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



extern long trap13(int, ...);
extern void s68(int *);
extern void s68l(long *);

#define swp68(x) s68(&x)
#define swp68l(x) s68l(&x)
#define kbshift(a) trap13(0x0B, a)            /* Get Drive Map            */



/*
 * _GSHIFT_S - GET SHIFT STATE 
 * entry:          none
 * exit:           CTL/SHIFT/ALT status in d0
 * destroys:       nothing
 */
 
WORD GSHIFT_S()
{
    return (kbshift(-1) & 0x000f);
}



/*
 * _GCHC_KEY - get choice for choice input
 *
 * returns:   0    nothing happened
 *            1    choice value
 *            2    button pressed
 */

WORD GCHC_KEY()
{
    TERM_CH = 1;                /* 16 bit char info */
    return TERM_CH;
}
