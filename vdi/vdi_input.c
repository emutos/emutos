/*
 * vdi_input.c
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "biosbind.h"
#include "xbiosbind.h"
#include "lineavars.h"
#include "vdi_defs.h"



/* CHOICE_INPUT: */
void v_choice(Vwk * vwk)
{
    WORD i;

    if (chc_mode == 0) {
        *(CONTRL + 4) = 1;
        while (gchc_key() != 1);
        *(INTOUT) = TERM_CH & 0x00ff;
    } else {
        i = gchc_key();
        *(CONTRL + 4) = i;
        if (i == 1)
            *(INTOUT) = TERM_CH & 0x00ff;
        else if (i == 2)
            *(INTOUT + 1) = TERM_CH & 0x00ff;
    }
}



/* STRING_INPUT: */
void v_string(Vwk * vwk)
{
    WORD i, j, mask;

    mask = 0x00ff;
    j = *INTIN;
    if (j < 0) {
        j = -j;
        mask = 0xffff;
    }
    if (!str_mode) {            /* Request mode */
        TERM_CH = 0;
        for (i = 0; (i < j) && ((TERM_CH & 0x00ff) != 0x000d); i++) {
            while (gchr_key() == 0);
            *(INTOUT + i) = TERM_CH = TERM_CH & mask;
        }
        if ((TERM_CH & 0x00ff) == 0x000d)
            --i;
        *(CONTRL + 4) = i;
    } else {                    /* Sample mode */

        i = 0;
        while ((gchr_key() != 0) && (i < j))
            *(INTOUT + i++) = TERM_CH & mask;
        *(CONTRL + 4) = i;
    }
}



/* Return Shift, Control, Alt State */
void vq_key_s(Vwk * vwk)
{
    CONTRL[4] = 1;
    INTOUT[0] = gshift_s();
}



/* SET_INPUT_MODE: */
void vsin_mode(Vwk * vwk)
{
    WORD i, *int_in;

    CONTRL[4] = 1;

    int_in = INTIN;
    *INTOUT = i = *(int_in + 1);
    i--;
    switch (*(int_in)) {
    case 0:
        break;

    case 1:                     /* locator */
        loc_mode = i;
        break;

    case 2:                     /* valuator */
        val_mode = i;
        break;

    case 3:                     /* choice */
        chc_mode = i;
        break;

    case 4:                     /* string */
        str_mode = i;
        break;
    }
}



/* INQUIRE INPUT MODE: */
void vqi_mode(Vwk * vwk)
{
    WORD *int_out;

    *(CONTRL + 4) = 1;

    int_out = INTOUT;
    switch (*(INTIN)) {
    case 0:
        break;

    case 1:                     /* locator */
        *int_out = loc_mode;
        break;

    case 2:                     /* valuator */
        *int_out = val_mode;
        break;

    case 3:                     /* choice */
        *int_out = chc_mode;
        break;

    case 4:                     /* string */
        *int_out = str_mode;
        break;
    }
}



/*
 * gshift_s - get shift state
 *
 * returns:   CTL/SHIFT/ALT status
 */
 
WORD gshift_s()
{
    return (Kbshift(-1) & 0x000f);
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

    if (Bconstat(2)) {                  // see if a character present at con
        ch = Bconin(2);
        TERM_CH = (WORD)
            (ch >> 8)|                  // scancode down to bit 8-15
            (ch & 0xff);                // asciicode to bit 0-7
        return 1;
    }
    return 0;
}



/*
 * gloc_key - get locator key
 *
 * returns:  0    - nothing
 *           1    - button pressed
 *                  TERM_CH = 16 bit char info
 *
 *           2    - coordinate info
 *                     X1 = new x
 *                     Y1 = new y
 *           4    - NOT IMPLIMENTED IN THIS VERSION
 *
 * The variable cur_ms_stat holds the bitmap of mouse status since the last
 * interrupt. The bits are
 *
 * 0 - 0x01 Left mouse button status  (0=up)
 * 1 - 0x02 Right mouse button status (0=up)
 * 2 - 0x04 Reserved
 * 3 - 0x08 Reserved
 * 4 - 0x10 Reserved
 * 5 - 0x20 Mouse move flag (1=moved)
 * 6 - 0x40 Right mouse button status flag (0=hasn't changed)
 * 7 - 0x80 Left mouse button status flag  (0=hasn't changed)
 */

WORD gloc_key()
{
    WORD retval;
    ULONG ch;

    if (cur_ms_stat & 0xc0) {           // some button status bits set?
        if (cur_ms_stat & 0x40)         // if bit 6 set
            TERM_CH = 0x21;             // send terminator code for left key
        else
            TERM_CH = 0x20;             // send terminator code for right key
        cur_ms_stat &= 0x23;            // clear mouse button status (bit 6/7)
        retval = 1;                     // set button pressed flag
    } else {                            // check key stat
        if (Bconstat(2)) {              // see if a character present at con
            ch = Bconin(2);
            TERM_CH = (WORD)
                (ch >> 8)|              // scancode down to bit 8-15
                (ch & 0xff);            // asciicode to bit 0-7
            retval = 1;                 // set button pressed flag
        } else {
            if (cur_ms_stat & 0x20) {   // if bit #5 set ...
                cur_ms_stat |= ~0x20;   // clear bit 5
                X1 = GCURX;             // set _X1 = _GCURX
                Y1 = GCURY;             // set _Y1 = _GCURY
                retval = 2;
            } else {
                retval = 0;
            }
        }
    }
    return retval;
}



