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

extern struct font_head f8x16;

extern WORD SIZ_TAB_rom[];
extern WORD DEV_TAB_rom[];
extern WORD INQ_TAB_rom[];


/* OPEN_WORKSTATION: */
void v_opnwk()
{
        int i;

        /* We need to copy some initial table data from the ROM */
        for(i=0; i<12; i++)
        {
            SIZ_TAB[i] = SIZ_TAB_rom[i];
        }
        for(i=0; i<45; i++)
        {
            DEV_TAB[i] = DEV_TAB_rom[i];
            INQ_TAB[i] = INQ_TAB_rom[i];
        }

        /* Set up the initial font: */
        font_ring[1] = &f8x16;

        cur_work = &virt_work;
        CONTRL[6] = virt_work.handle = 1;
        virt_work.next_work = NULLPTR;

        line_cw = -1;                           /* invalidate current line width */

        text_init();                            /* initialize the SIZ_TAB info */

        init_wk();

        /* Input must be initialized here and not in init_wk */

        loc_mode = 0;                           /* default is request mode  */
        val_mode = 0;                           /* default is request mode  */
        chc_mode = 0;                           /* default is request mode  */
        str_mode = 0;                           /* default is request mode  */

        HIDE_CNT = 1;                           /* mouse is initially hidden */

        GCURX = DEV_TAB[0] / 2;         /* initialize the mouse to center */
        GCURY = DEV_TAB[1] / 2;

        INIT_G();                                       /* go into graphics mode */
}
