/*
 * vdi_mouse.c
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
#include "xbiosbind.h"
#include "vdi_defs.h"
#include "tosvars.h"
#include "lineavars.h"



typedef struct Mcdb_ Mcdb;
struct Mcdb_ {
	WORD	xhot;
	WORD	yhot;
	WORD	planes;
	WORD	bg_col;
	WORD	fg_col;
	UWORD	mask[16];
	UWORD	data[16];
};

extern void mouse_int();    /* mouse interrupt routine */
extern void mov_cur();      // user button vector
extern void vb_draw();      // user button vector

/* global storage area for mouse form definition */
/* as long, as we use parts in assembler, we need these */
extern WORD m_pos_hx;	    // (cdb+0) Mouse hot spot - x coord
extern WORD m_pos_hy;       // (cdb+2) Mouse hot spot - y coord
extern WORD m_planes;       // (cdb+4) unused - Plane count for mouse pointer
extern WORD m_cdb_bg;       // (cdb+6) Mouse background color as pel value
extern WORD m_cdb_fg;       // (cdb+8) Mouse foreground color as pel value
extern UWORD mask_form;     // (cdb+10) Storage for mouse mask and cursor



/* Default Mouse Cursor Definition */
static Mcdb arrow_cdb = {
    1, 0, 1, 0, 1,
    /* background definition */
    {
        0xE000, //%1110000000000000
        0xF000, //%1111000000000000
        0xF800, //%1111100000000000
        0xFB00, //%1111110000000000
        0xFE00, //%1111111000000000
        0xFF00, //%1111111100000000
        0xFF80, //%1111111110000000
        0xFFB0, //%1111111111000000
        0xFE00, //%1111111000000000
        0xFE00, //%1111111000000000
        0xEF00, //%1110111100000000
        0x0F00, //%0000111100000000
        0x0780, //%0000011110000000
        0x0780, //%0000011110000000
        0x03B0, //%0000001111000000
        0x0000 //%0000000000000000
    },
    /* foreground definition */
    {
        0x4000, //%0100000000000000
        0x6000, //%0110000000000000
        0x7000, //%0111000000000000
        0x7800, //%0111100000000000
        0x7B00, //%0111110000000000
        0x7E00, //%0111111000000000
        0x7F00, //%0111111100000000
        0x7F80, //%0111111110000000
        0x7B00, //%0111110000000000
        0x6B00, //%0110110000000000
        0x4600, //%0100011000000000
        0x0600, //%0000011000000000
        0x0300, //%0000001100000000
        0x0300, //%0000001100000000
        0x0180, //%0000000110000000
        0x0000 //%0000000000000000
    }
};

/*
 * do_nothing - doesn't do much  :-)
 */

static void do_nothing()
{
}



/*
 * dis_cur - Displays the mouse cursor if the number of hide 
 *           operations has gone back to 0.
 *
 *  Decrement the counter for the number of hide operations performed.
 *  If this is not the last one then do nothing because the cursor
 *  should remain hidden.
 *
 *   Outputs:
 *      hide_cnt = hide_cnt - 1
 *      draw_flag = 0
 */

void dis_cur()
{
    mouse_flag += 1;            // disable mouse redrawing
    HIDE_CNT -= 1;              // decrement hide operations counter
    if (HIDE_CNT <= 0) {
        HIDE_CNT = 0;           // if hide counter < 0
        mousex = GCURX;         // get cursor x-coordinate
        mousey = GCURY;         // get cursor y-coordinate
        cur_display();          // display the cursor
        draw_flag = 0;          // disable vbl drawing routine
    }
    mouse_flag -= 1;            // re-enable mouse drawing
}



/*
 * hide_cur
 *
 * This routine hides the mouse cursor if it has not already
 * been hidden.
 *
 * Inputs:         None
 *
 * Outputs:
 *    hide_cnt = hide_cnt + 1
 *    draw_flag = 0
 */

void hide_cur()
{
    mouse_flag += 1;            /* disable mouse redrawing */

    /*
     * Increment the counter for the number of hide operations performed.
     * If this is the first one then remove the cursor from the screen.
     * If not then do nothing, because the cursor wasn't on the screen.
     */
    HIDE_CNT += 1;              // increment it
    if (HIDE_CNT == 1) {        // if cursor was not hidden...
        cur_replace();          // remove the cursor from screen
        draw_flag = 0;          // disable vbl drawing routine
    }

    mouse_flag -= 1;            /* re-enable mouse drawing */
}



/* LOCATOR_INPUT: */
void v_locator(Vwk * vwk)
{
    WORD i;
    WORD *pointer;

    *INTIN = 1;

    /* Set the initial locator position. */

    pointer = PTSIN;
    GCURX = *pointer++;
    GCURY = *pointer;

    if (loc_mode == 0) {
        dis_cur();
        while ((i = gloc_key()) != 1) { /* loop till some event */
            if (i == 4) {       /* keyboard cursor? */
                hide_cur();     /* turn cursor off */
                GCURX = X1;
                GCURY = Y1;
                dis_cur();      /* turn cursor on */
            }
        }
        *(INTOUT) = TERM_CH & 0x00ff;
        pointer = CONTRL;
        *(pointer + 4) = 1;
        *(pointer + 2) = 1;
        pointer = PTSOUT;
        *pointer++ = X1;
        *pointer = Y1;
        hide_cur();
    } else {
        i = gloc_key();
        pointer = CONTRL;
        *(pointer + 2) = 1;
        *(pointer + 4) = 0;
        switch (i) {
        case 0:
            *(pointer + 2) = 0;
            break;

        case 1:
            *(pointer + 2) = 0;
            *(pointer + 4) = 1;
            *(INTOUT) = TERM_CH & 0x00ff;
            break;

        case 2:
            pointer = PTSOUT;
            *pointer++ = X1;
            *pointer = Y1;
            break;

        case 3:
            *(pointer + 4) = 1;
            pointer = PTSOUT;
            *pointer++ = X1;
            *pointer = Y1;
            break;

        case 4:
            if (HIDE_CNT == 0) {
                hide_cur();
                pointer = PTSOUT;
                *pointer++ = GCURX = X1;
                *pointer = GCURY = Y1;
                dis_cur();
            } else {
                pointer = PTSOUT;
                *pointer++ = GCURX = X1;
                *pointer = GCURY = Y1;
            }
            break;

        }
    }
}



/*
 * v_show_c - show cursor
 */

void v_show_c(Vwk * vwk)
{
    if (!*INTIN && HIDE_CNT)
        HIDE_CNT = 1;           /* reset cursor to on */

    dis_cur();
}



/*
 * v_hide_c - hide cursor
 */

void v_hide_c(Vwk * vwk)
{
    hide_cur();
}



/*
 * vq_mouse - Query mouse position and button status
 */

void vq_mouse(Vwk * vwk)
{
    WORD *pointer;

    INTOUT[0] = MOUSE_BT;

    pointer = CONTRL;
    *(pointer + 4) = 1;
    *(pointer + 2) = 1;

    pointer = PTSOUT;
    *pointer++ = GCURX;
    *pointer = GCURY;
}



/* VALUATOR_INPUT: */
void v_valuator(Vwk * vwk)
{
}



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
 */

void vex_butv(Vwk * vwk)
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
 */

void vex_motv(Vwk * vwk)
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
 */

void vex_curv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_cur;
    (LONG*)user_cur = *--pointer;
}



static void set_mouse_form (Vwk * vwk, Mcdb * mcdb)
{
    int i;
    UWORD * gmdt;                /* global mouse definition table */
    UWORD * mask;
    UWORD * data;

    mouse_flag += 1;		/* disable updates while redefining cursor */

    /* save x-offset of mouse hot spot */
    m_pos_hx = mcdb->xhot & 0x000f;

    /* save y-offset of mouse hot spot */
    m_pos_hy = mcdb->yhot & 0x000f;

    /* is background color index too high? */
    if (mcdb->bg_col >= DEV_TAB[13]) {
        mcdb->bg_col = 1;		/* yes - default to 1 */
    }
    m_cdb_bg = MAP_COL[mcdb->bg_col];

    /* is forground color index too high? */
    if (mcdb->fg_col >= DEV_TAB[13]) {
        mcdb->fg_col = 1;		/* yes - default to 1 */
    }
    m_cdb_fg = MAP_COL[mcdb->fg_col];

    /*
     * Move the new mouse defintion into the global mouse cursor definition
     * table.  The values for the mouse mask and data are supplied as two
     * separate 16-word entities.  They must be stored as a single array
     * starting with the first word of the mask followed by the first word
     * of the data and so on.
     */

    /* copy the data to the global mouse definition table */
    gmdt = &mask_form;
    mask = mcdb->mask;
    data = mcdb->data;
    for (i = 15; i >= 0; i--) {
        *gmdt++ = *mask++;		/* get next word of mask */
        *gmdt++ = *data++;		/* get next word of data */
    }

    mouse_flag -= 1;			/* re-enable mouse drawing */
}



/*
 * xfm_crfm - Transforms user defined cursor to device specific format.
 *
 * Get the new values for the x and y-coordinates of the mouse hot
 * spot and the new color indices for the mouse mask and data.
 *
 * Inputs:
 *     intin[0] - x coordinate of hot spot
 *     intin[1] - y coordinate of hot spot
 *     intin[2] - reserved for future use. must be 1
 *     intin[3] - Mask color index
 *     intin[4] - Data color index
 *     intin[5-20]  - 16 words of cursor mask
 *     intin[21-36] - 16 words of cursor data
 *
 * Outputs:        None
 */
void xfm_crfm (Vwk * vwk)
{
    set_mouse_form(vwk, (Mcdb *)INTIN);
}



/*
 * vdimouse_init - Initializes the mouse (VDI part)
 *
 * entry:          none
 * exit:           none
 */

void vdimouse_init(Vwk * vwk)
{
    user_but = do_nothing;
    user_mot = do_nothing;
    user_cur = mov_cur;         /* initialize user_cur vector */

    /* Move in the default mouse form (presently the arrow) */
    set_mouse_form(vwk, &arrow_cdb);	/* transform mouse */

    MOUSE_BT = 0;               // clear the mouse button state
    cur_ms_stat = 0;            // clear the mouse status
    mouse_flag = 0;             // clear the mouse flag
    draw_flag = 0;              // clear the hide operations counter
    newx = 0;                   // set cursor x-coordinate to 0
    newy = 0;                   // set cursor y-coordinate to 0

    /* vblqueue points to start of vbl_list[] */
    *vblqueue = (LONG)vb_draw;   /* set GEM VBL-routine to vbl_list[0] */

    /* Initialize mouse via XBIOS in relative mode */
    Initmous(1, (LONG)&arrow_cdb, (LONG)mouse_int);
}



/*
 * vdimouse_exit - deinitialize/disable mouse
 */
 
void vdimouse_exit(Vwk * vwk)
{
    LONG * pointer;             /* help for storing LONGs in INTIN */

    user_but = do_nothing;
    user_mot = do_nothing;
    user_cur = do_nothing;

    pointer = vblqueue;         /* vblqueue points to start of vbl_list[] */
    *pointer = (LONG)vb_draw;   /* set GEM VBL-routine to vbl_list[0] */

    /* disable mouse via XBIOS */
    Initmous(0, 0, 0);
}



/*
 * vb_draw - moves mouse cursor, GEM VBL routine
 *
 * It removes the mouse cursor from its current location, if necessary,
 * * and redraws it at a new location.
 *
 *      Inputs:
 *         draw_flag - signals need to redraw cursor
 *         newx - new cursor x-coordinate
 *         newy - new cursor y-coordinate
 *         mouse_flag - cursor hide/show flag
 *
 *      Outputs:
 *         draw_flag is cleared
 *
 *      Registers Modified:     d0, d1
 *
 */

/* If we do not need to draw the cursor now then just exit. */

void vb_draw()
{
    WORD old_sr = set_sr(0x2700);  // disable interrupts
    if (draw_flag) {
        mousex = newx;          // get cursor x-coordinate
        mousey = newy;          // get cursor y-coordinate
        set_sr(old_sr);
        if (!mouse_flag) {
            cur_replace();              // remove the old cursor from the screen
            cur_display();              // redraw the cursor
        }
    } else
        set_sr(old_sr);

}
