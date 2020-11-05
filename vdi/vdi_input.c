/*
 * vdi_input.c
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2019 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "biosbind.h"
#include "xbiosbind.h"
#include "vdi_defs.h"


static WORD gchc_key(void);
static WORD gchr_key(void);
static WORD gshift_s(void);


/*
 * CHOICE_INPUT: implements vrq_choice()/vsm_choice()
 *
 * These functions return the status of the logical 'choice' device.
 * The "GEM Programmer's Guide: VDI" indicates that these functions
 * are not required, and both Atari TOS and EmuTOS (using the original
 * imported DRI source) implement them as dummy functions.
 */
void vdi_v_choice(Vwk * vwk)
{
    gchc_key();
    INTOUT[0] = TERM_CH & 0x00ff;

}



/*
 * STRING_INPUT: implements vrq_string()/vsm_string()
 *
 * These functions return the status of the logical 'string' device,
 * which is the keyboard under TOS.
 *
 * vrq_string() operation in Atari TOS and EmuTOS
 * ----------------------------------------------
 * 1. This function reads characters from the keyboard until a carriage
 *    return is entered, or until the maximum number of characters has
 *    been read, and then returns.  The characters are returned in
 *    intout[]: each word in intout[] will contain zero in the high-order
 *    byte, and the ASCII character in the low-order byte.  The 'C'
 *    binding will copy the low-order bytes to a buffer.  If the call is
 *    terminated by a carriage return, the carriage return is NOT placed
 *    in intout[].
 * 2. The maximum number of characters may be specified as negative.  In
 *    this case, the maximum used will be the absolute value of that
 *    specified, and everything else will work the same as (1) above,
 *    except that the words in the intout[] array will contain extended
 *    keyboard codes: the scancode in the high-order byte and the ASCII
 *    code in the low-order byte.
 * 3. The 'echo' argument is ignored.
 * 4. Atari TOS bug: when the maximum is specified as negative, carriage
 *    returns do NOT terminate input; input is only terminated by the
 *    maximum number of characters being reached.
 *
 * vsm_string() operation in Atari TOS and EmuTOS
 * ----------------------------------------------
 * 1. On entry, this function checks if any keyboard input is pending;
 *    if not, it returns immediately.  Otherwise, it reads characters
 *    until there are no more, or until the maximum number of characters
 *    has been read.
 *    NOTE: carriage returns are treated like any other character, are
 *    included in intout[], and do NOT cause input termination.
 * 2. The maximum number of characters may be specified as negative, with
 *    the same results as described above for vrq_string().
 * 3. The 'echo' argument is ignored.
 */
void vdi_v_string(Vwk * vwk)
{
    WORD i, j, mask;

    mask = 0x00ff;
    j = INTIN[0];
    if (j < 0) {
        j = -j;
        mask = 0xffff;
    }
    if (!str_mode) {            /* Request mode */
        TERM_CH = 0;
        for (i = 0; (i < j) && ((TERM_CH & 0x00ff) != 0x000d); i++) {
            while (gchr_key() == 0);
            INTOUT[i] = TERM_CH = TERM_CH & mask;
        }
        if ((TERM_CH & 0x00ff) == 0x000d)
            --i;
        CONTRL[4] = i;
    } else {                    /* Sample mode */

        i = 0;
        while ((gchr_key() != 0) && (i < j))
            INTOUT[i++] = TERM_CH & mask;
        CONTRL[4] = i;
    }
}



/* Return Shift, Control, Alt State */
void vdi_vq_key_s(Vwk * vwk)
{
    INTOUT[0] = gshift_s();
}



/* SET_INPUT_MODE: */
void vdi_vsin_mode(Vwk * vwk)
{
    WORD i;

    INTOUT[0] = i = INTIN[1];
    i--;

    switch (INTIN[0]) {
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



/*
 * INQUIRE INPUT MODE: implements vqin_mode()
 *
 * This function is documented by Atari to return the mode value set
 * by vsin_mode() [this is either 1 (request mode) or 2 (sample mode)].
 * However, like all versions of Atari TOS, it actually returns the mode
 * value minus 1 (i.e. 0 or 1).
 */
void vdi_vqin_mode(Vwk * vwk)
{
    WORD *int_out;

    int_out = INTOUT;
    switch (INTIN[0]) {
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
static WORD gshift_s(void)
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
static WORD gchc_key(void)
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
static WORD gchr_key(void)
{
    ULONG ch;

    if (Bconstat(2)) {                  /* see if a character present at con */
        ch = Bconin(2);
        TERM_CH = (WORD)
            (ch >> 8)|                  /* scancode down to bit 8-15 */
            (ch & 0xff);                /* asciicode to bit 0-7 */
        return 1;
    }
    return 0;
}
