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



extern void mouse_int();    /* mouse interrupt routine */
extern void mov_cur();      // user button vector
extern void vb_draw();      // user button vector
extern struct param arrow_cdb;



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



/*
 * vdimouse_init - Initializes the mouse (VDI part)
 *
 * entry:          none
 * exit:           none
 */

void vdimouse_init(Vwk * vwk)
{
    WORD * pointer;             /* help for storing LONGs in INTIN */

    user_but = do_nothing;
    user_mot = do_nothing;
    user_cur = mov_cur;         /* initialize user_cur vector */

    /* Move in the default mouse form (presently the arrow) */
    pointer = INTIN;            /* save INTIN */
    INTIN = (WORD *)&arrow_cdb; /* it points to the arrow data */
    xfm_crfm();                 /* transform mouse */
    INTIN = pointer;            /* restore old value */

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



