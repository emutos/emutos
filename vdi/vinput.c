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

#define kbshift(a)   trap13(0x0B, a)	/* Get Drive Map            */
#define bconstat(a)  trap13(0x01,a)
#define bconin(a)    trap13(0x02,a)
#define bconout(a,b) trap13(0x03,a,b)



/*
 * gshift_s - get shift state
 *
 * returns:   CTL/SHIFT/ALT status
 */
 
WORD gshift_s()
{
    return (kbshift(-1) & 0x000f);
}



/*
 * GCHC_KEY - get choice for choice input
 *
 * returns:   0    nothing happened
 *            1    choice value
 *            2    button pressed
 */

WORD gchc_key()
{
    TERM_CH = 1;                /* 16 bit char info */
    return TERM_CH;
}



/*
 * gchr_key - get char for string input
 *
 * returns:  1     button pressed
 *           0     nothing happened
 * 
 * TERM_CH         16 bit char info
 */

WORD gchr_key()
{
    ULONG ch;

    if (bconstat(2)) {                  // see if a character present at con
        ch = bconin(2);
        TERM_CH = (WORD)
            (ch >> 8)|                  // scancode down to bit 8-15
            (ch & 0xff);                // asciicode to bit 0-7
        return 1;
    }
    return 0;
}
