/*
 * lineainit.c - linea graphics initialization
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */


#include "tosvars.h"
#include "lineavars.h"
#include "font.h"
#include "kprint.h"
//#include "country.h"
#include "screen.h"
#include "machine.h"

#define DBG_LINEA 0



/* Shift table for computing offsets into a scan line (interleaved planes) */
static const BYTE shft_tab [] =
{
    3,  /* 1 plane */
    2,  /* 2 planes */
    0,  /* not used */
    1   /* 4 planes */
};

BYTE shft_off;                  /* once computed Offset into a Scan Line */


/* Settings for the different video modes */
struct video_mode {
    UBYTE       planes;         // count of color planes (v_planes)
    UWORD       hz_rez;         // screen horizontal resolution (v_hz_rez)
    UWORD       vt_rez;         // screen vertical resolution (v_vt_rez)
    UBYTE       col_fg;         // color number of initial foreground color
};


static const struct video_mode video_mode[] = {
    {4, 320, 200, 15},        // 16 color mode
    {2, 640, 200, 3},         // 4 color mode
    {1, 640, 400, 1}          // monochrome mode
};



/*
 * cursconf - cursor configuration
 *
 * Arguments:
 *
 *   function =
 *   0 - switch off cursor
 *   1 - switch on cursor
 *   2 - blinking cursor
 *   3 - not blinking cursor
 *   4 - set cursor blink rate
 *   5 - get cursor blink rate
 *
 * Bits:
 *   M_CFLASH - cursor flash on
 *   M_CVIS   - cursor visibility on
 */

WORD cursconf(WORD function, WORD operand)
{
    switch (function) {
    case 0:
        cprintf("\033f");               /* set cursor unvisible */
        break;
    case 1:
        cprintf("\033e");               /* set cursor visible */
        break;
    case 2:
        v_stat_0 &= ~M_CFLASH;          /* unset cursor flash bit */
        break;
    case 3:
        v_stat_0 |= M_CFLASH;           /* set cursor flash bit */
        break;
    case 4:
        v_period = operand;             /* set cursor flash interval */
        break;
    case 5:
        return(v_period);               /* set cursor flash interval */
    }
    return(0);                          /* Hopefully never reached */
}



/*
 * linea_init - init linea variables
 */

void linea_init(void)
{
    WORD vmode;                 	/* video mode */

    vmode = (sshiftmod & 3);    	/* Get video mode from hardware */
#if DBG_LINEA
    kprintf("vmode : %d\n", vmode);
#endif

    /* set parameters for video mode */
    if (vmode > 2) {      		/* Mode 3 == unvalid for ST (MAD) */
        kprintf("video mode was: %d !\n", vmode);
        vmode=2;                	/* Falcon should be handled special? */
    }
    if (has_videl) {
        v_planes = get_videl_bpp();
        v_hz_rez = get_videl_width();
        v_vt_rez = get_videl_height();
    }
    else {
        v_planes = video_mode[vmode].planes;
        v_hz_rez = video_mode[vmode].hz_rez;
        v_vt_rez = video_mode[vmode].vt_rez;
    }
    v_lin_wr = v_hz_rez / 8 * v_planes;     /* bytes per line */
    /*v_pl_dspl = (long)v_hz_rez*(long)v_vt_rez/8;*/     /* bytes per plane */
    v_bytes_lin = v_lin_wr;       /* I think v_bytes_lin = v_lin_wr (joy) */

    v_col_fg = video_mode[vmode].col_fg;
    v_col_bg = 0;

    /* Calculate the shift offset by a value contained in the shift table
     * (used for screen modes that have the planes arranged in an
     *  interleaved fashion with a word for each plane). */
    if (v_planes <= 4)  shft_off = shft_tab[v_planes - 1];

#if DBG_LINEA
    kprintf("planes: %d\n", v_planes);
    kprintf("lin_wr: %d\n", v_lin_wr);
    kprintf("hz_rez: %d\n", v_hz_rez);
    kprintf("vt_rez: %d\n", v_vt_rez);
#endif
}
