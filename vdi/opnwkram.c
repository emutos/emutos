/*
 * opnwkram.c - Open physical workstation
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "gsxdef.h"
#include "gsxextrn.h"
#include "lineavars.h"

/* fonts once set up in lineainit.c */
extern struct font_head fon8x16;
extern struct font_head fon8x8;
extern struct font_head fon6x6;

extern WORD SIZ_TAB_rom[];
extern WORD DEV_TAB_rom[];
extern WORD INQ_TAB_rom[];


/* Screen related variables */
extern UWORD v_planes;          // count of color planes
extern UWORD v_lin_wr;          // line wrap : bytes per line
extern UWORD v_hz_rez;          // screen horizontal resolution
extern UWORD v_vt_rez;          // screen vertical resolution
extern UWORD v_bytes_lin;       // width of line in bytes


/* OPEN_WORKSTATION: */
void v_opnwk()
{
    int i;

    /* We need to copy some initial table data from the ROM */
    for (i = 0; i < 12; i++) {
        SIZ_TAB[i] = SIZ_TAB_rom[i];
    }

    for (i = 0; i < 45; i++) {
        DEV_TAB[i] = DEV_TAB_rom[i];
        INQ_TAB[i] = INQ_TAB_rom[i];
    }

    /* Copy data from linea variables */
    DEV_TAB[0] = v_hz_rez-1;
    DEV_TAB[1] = v_vt_rez-1;
    INQ_TAB[4] = v_planes;

    /* Calculate colors allowed at one time */
    if (INQ_TAB[4] < 8)
        DEV_TAB[13] = 2<<(v_planes-1);
    else
        DEV_TAB[13] = 256;

    /* Set up the initial font ring */
    font_ring[1] = &fon8x16;

    cur_work = &virt_work;
    CONTRL[6] = virt_work.handle = 1;
    virt_work.next_work = NULLPTR;

    line_cw = -1;               /* invalidate current line width */

    text_init();                /* initialize the SIZ_TAB info */

    init_wk();

    /* Input must be initialized here and not in init_wk */

    loc_mode = 0;               /* default is request mode  */
    val_mode = 0;               /* default is request mode  */
    chc_mode = 0;               /* default is request mode  */
    str_mode = 0;               /* default is request mode  */


    /* mouse settings */
    HIDE_CNT = 1;               /* mouse is initially hidden */

    GCURX = DEV_TAB[0] / 2;     /* initialize the mouse to center */
    GCURY = DEV_TAB[1] / 2;

    gfx_init();                 /* go into graphics mode */
}
