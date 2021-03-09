/*
 * vdi_esc.c - GSX escapes for the VDI screen driver
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "vdi_defs.h"
#include "lineavars.h"
#include "asm.h"
#include "bdosbind.h"


/* Local Constants */

#define ldri_escape             19      /* last DRI escape = 19. */


/*
 * do console raw i/o
 */
static LONG crawio(WORD ch)
{
    return Crawio(ch);
}


/*
 * write string to console
 */
static void cconws(char *string)
{
    Cconws(string);
}


/*
 * escfn0 - stub to simplify lookup table
 */
static void escfn0(Vwk * vwk)
{
}


/*
 * escfn1: vq_chcells() - returns the current number of rows and columns
 *
 * outputs:
 *   CONTRL[4] = 2 (# of integers returned)
 *   INTOUT[0] = number of rows
 *   INTOUT[1] = number of columns
 */
static void escfn1(Vwk * vwk)
{
    CONTRL[4] = 2;
    INTOUT[0] = v_cel_my + 1;
    INTOUT[1] = v_cel_mx + 1;
}


/*
 * escfn2: v_exit_cur() - exit alpha mode and enter graphics mode
 *
 * note: if the last thing a program does before entering graphics mode
 * is to write an Esc to the console, the first character of the sequence
 * below will be eaten (and ignored).  concretely, this is most likely
 * to happen when a .TOS/.TTP program exits to the desktop.
 *
 * to circumvent this, we send a harmless escape sequence before we hide
 * the alpha cursor.
 */
static void escfn2(Vwk * vwk)
{
    cconws("\033H\033f\033E");  /* home, hide alpha cursor, then clear-and-home */
}


/*
 * escfn3: v_enter_cur() - enter alpha mode and exit graphics mode
 *
 * note: we send a CR to reset the BDOS column counter (important for
 * tab expansion)
 */
static void escfn3(Vwk * vwk)
{
    cconws("\033E\033e\015");   /* clear-and-home, then show alpha cursor */
}


/*
 * escfn4: v_curup() - moves the alpha cursor up one line
 */
static void escfn4(Vwk * vwk)
{
    cconws("\033A");
}


/*
 * escfn5: v_curdown() - moves the alpha cursor down one line
 */
static void escfn5(Vwk * vwk)
{
    cconws("\033B");
}


/*
 * escfn6: v_curright() - moves the alpha cursor right one column
 */
static void escfn6(Vwk * vwk)
{
    cconws("\033C");
}


/*
 * escfn7: v_curleft() - moves the alpha cursor left one column
 */
static void escfn7(Vwk * vwk)
{
    cconws("\033D");
}


/*
 * escfn8: v_curhome() - moves the alpha cursor home
 */
static void escfn8(Vwk * vwk)
{
    cconws("\033H");
}


/*
 * escfn9: v_eeos() - clears screen from cursor position to end of screen
 */
static void escfn9(Vwk * vwk)
{
    cconws("\033J");
}


/*
 * escfn10: v_eeol() - clears screen from cursor position to end of line
 */
static void escfn10(Vwk * vwk)
{
    cconws("\033K");
}


/*
 * escfn11: vs_curaddress() - sets the cursor position
 *
 * The cursor will be displayed at the new location,
 * if it is not currently hidden.
 *
 * inputs:
 *   INTIN[0] = cursor row (1 - max_y_cell)
 *   INTIN[1] = cursor column (1 - max_x_cell)
 */
static void escfn11(Vwk * vwk)
{
    char out[5];

    /* send string via VT-52 terminal emulation */
    out[0] = '\033';
    out[1] = 'Y';
    out[2] = 0x20 + INTIN[0] - 1;   /* zero-based */
    out[3] = 0x20 + INTIN[1] - 1;
    out[4] = '\0';
    cconws(out);
}


/*
 * escfn12: v_curtext() - outputs cursor addressable alpha text
 *
 * The cursor will be displayed at the new location,
 * if it is not currently hidden.
 *
 * inputs:
 *   CONTRL[3] = character count
 *   INTIN = character array
 */
static void escfn12(Vwk * vwk)
{
    int cnt;
    WORD *chr;

    cnt = CONTRL[3];            /* get the character count */
    chr = INTIN;                /* address of the character array */

    while (cnt--) {
        crawio(*chr++);         /* raw i/o to standard input/output */
    }
}


/*
 * escfn13: v_rvon(): - switch to reverse video
 */
static void escfn13(Vwk * vwk)
{
    cconws("\033p");        /* enter reverse video */
}


/*
 * escfn14: v_rvoff() - switch to normal video
 */
static void escfn14(Vwk * vwk)
{
    cconws("\033q");        /* enter normal video */
}


/*
 * escfn15: vq_curaddress() - returns current row and column of the alpha cursor
 */
static void escfn15(Vwk * vwk)
{
    CONTRL[4] = 2;              /* 2 integers are returned */
    INTOUT[0] = v_cur_cy + 1;   /* row (starting at 1) */
    INTOUT[1] = v_cur_cx + 1;   /* column (starting at 1) */
}


/*
 * escfn16: vq_tabstatus() - returns the availability of a "tablet device"
 *
 * Atari TOS always returns 1, so do we
 *
 * outputs:
 *   CONTRL[4] = 1  (# of parameters returned)
 *   INTOUT[0] = 1  (device is available)
 */
static void escfn16(Vwk * vwk)
{
    CONTRL[4] = 1;              /* 1 integer is returned */
    INTOUT[0] = 1;              /* there is a mouse */
}


/*
 * escfn17: v_hardcopy() - output screen to printer
 *
 * This function is currently just a stub.
 */
static void escfn17(Vwk * vwk)
{
}


/*
 * escfn18: v_dspcur() - display the graphics cursor
 *
 * note: although Atari documentation specifies that this call
 * is v_dspcur(handle,x,y), where x/y are the coordinates where
 * the cursor should be displayed, Atari TOS apparently ignores
 * these values, as do we ...
 */
static void escfn18(Vwk * vwk)
{
    INTIN[0] = 0;           /* show regardless */
    vdi_v_show_c(vwk);      /* display the graphics cursor */
}


/*
 * escfn19: v_rmcur() - remove the graphics cursor
 */
static void escfn19(Vwk * vwk)
{
    vdi_v_hide_c(vwk);      /* hide the graphics cursor */
}


/*
 * function lookup table
 */
static void (* const esctbl[])(Vwk *) =
{
    escfn0,
    escfn1,
    escfn2,
    escfn3,
    escfn4,
    escfn5,
    escfn6,
    escfn7,
    escfn8,
    escfn9,
    escfn10,
    escfn11,
    escfn12,
    escfn13,
    escfn14,
    escfn15,
    escfn16,
    escfn17,
    escfn18,
    escfn19
};


/*
 * vdi_v_escape - this routine is called to decode the escape subfunctions
 *
 * The following inputs and outputs may be used by a subfunction:
 *
 * input:
 *   CONTRL[5] = escape function ID.
 *   CONTRL[6] = device handle.
 *   INTIN[]   = array of input parameters.
 *
 * output:
 *   CONTRL[2] = number of output vertices.
 *   CONTRL[4] = number of output parameters.
 *   INTOUT[]  = array of output parameters.
 *   PTSOUT[]  = array of output vertices.
 */
void vdi_v_escape(Vwk * vwk)
{
    UWORD escfun = CONTRL[5];

    KDEBUG(("VDI esc, subfunction %u called\n",escfun));

#if HAVE_BEZIER
    if (escfun == 99) {
        v_bez_qual(vwk);        /* set quality of bezier curves */
        return;
    }
#endif

    if (escfun > ldri_escape)
        return;
    (*esctbl[escfun])(vwk);
}


/*
 * called by v_opnwk() to enter graphics mode
 */
void esc_init(Vwk * vwk)
{
    escfn2(vwk);
}


/*
 * called by v_clswk() to exit graphics mode
 */
void esc_exit(Vwk * vwk)
{
    escfn3(vwk);
}
