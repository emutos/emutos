/*      GEMGRLIB.C      4/11/84 - 09/20/85      Gregg Morris            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2017 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include "portab.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"

#include "gemevlib.h"
#include "gemgraf.h"
#include "gemwmlib.h"
#include "gemoblib.h"
#include "geminput.h"
#include "gemgsxif.h"
#include "gemgrlib.h"
#include "gemasm.h"
#include "gsx2.h"
#include "optimopt.h"
#include "rectfunc.h"

#include "intmath.h"

/*
 *  Routine to watch the mouse while the button is down and it stays
 *  inside/outside of the specified rectangle.  Return TRUE as long as
 *  the mouse is down.  Block until the mouse moves into or out of the
 *  specified rectangle.
 */
static WORD gr_stilldn(WORD out, WORD x, WORD y, WORD w, WORD h)
{
    WORD    rets[6];
    MOBLK   tmpmoblk;
    WORD    which;

    dsptch();

    tmpmoblk.m_out = out;
    tmpmoblk.m_gr.g_x = x;
    tmpmoblk.m_gr.g_y = y;
    tmpmoblk.m_gr.g_w = w;
    tmpmoblk.m_gr.g_h = h;

    which = ev_multi(MU_KEYBD | MU_BUTTON | MU_M1, &tmpmoblk,
                    NULL, 0x0L, 0x0001ff00L, 0x0L, rets);

    if (which & MU_BUTTON)
        return FALSE;

    return TRUE;
}


static void gr_setup(WORD color)
{
    gsx_sclip(&gl_rscreen);
    gsx_attr(FALSE, MD_XOR, color);
}


static void gr_clamp(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
                     WORD *pneww, WORD *pnewh)
{
    *pneww = max(xrat - xorigin + 1, wmin);
    *pnewh = max(yrat - yorigin + 1, hmin);
}


static void gr_scale(WORD xdist, WORD ydist, WORD *pcnt, WORD *pxstep, WORD *pystep)
{
    WORD i;
    WORD dist;


    gr_setup(BLACK);

    dist = (xdist + ydist) / 2;

    for (i = 0; dist; i++)
        dist /= 2;

    if ((*pcnt = i) != 0)
    {
        *pxstep = max(1, xdist / i);
        *pystep = max(1, ydist / i);
    }
    else
        *pxstep = *pystep = 1;
}


void gr_stepcalc(WORD orgw, WORD orgh, GRECT *pt, WORD *pcx, WORD *pcy,
                 WORD *pcnt, WORD *pxstep, WORD *pystep)
{
    *pcx = (pt->g_w/2) - (orgw/2);
    *pcy = (pt->g_h/2) - (orgh/2);

    gr_scale(*pcx, *pcy, pcnt, pxstep, pystep);

    *pcx += pt->g_x;
    *pcy += pt->g_y;
}


static void gr_xor(WORD clipped, WORD cnt, WORD cx, WORD cy, WORD cw, WORD ch,
                   WORD xstep, WORD ystep, WORD dowdht)
{
    GRECT tmprect;

    do
    {
        tmprect.g_x = cx;
        tmprect.g_y = cy;
        tmprect.g_w = cw;
        tmprect.g_h = ch;

        if (clipped)
            gsx_xcbox(&tmprect);
        else
            gsx_xbox(&tmprect);

        cx -= xstep;
        cy -= ystep;
        if (dowdht)
        {
            cw += (2 * xstep);
            ch += (2 * ystep);
        }
    } while(cnt--);
}


static void gr_draw(WORD have2box, GRECT *po, GRECT *poff)
{
    GRECT   t;

    gsx_xbox(po);
    if (have2box)
    {
        r_set(&t, po->g_x + poff->g_x, po->g_y + poff->g_y,
                  po->g_w + poff->g_w, po->g_h + poff->g_h);
        gsx_xbox(&t);
    }
}


static WORD gr_wait(GRECT *po, GRECT *poff)
{
    WORD have2box;
    WORD down;

    have2box = !rc_equal(&gl_rzero, poff);

    /* draw/erase old */
    gsx_moff();
    gr_draw(have2box, po, poff);
    gsx_mon();

    /* wait for change */
    down = gr_stilldn(TRUE, xrat, yrat, 1, 1);

    /* draw/erase old */
    gsx_moff();
    gr_draw(have2box, po, poff);
    gsx_mon();

    /* return exit event */
    return down;
}


/*
 *  Stretch the free corner of an XOR box(w,h) that is pinned at
 *  another corner, based on mouse movement until the button comes up.
 *  Also draw a second box offset from the stretching box.
 */
void gr_rubwind(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
                GRECT *poff, WORD *pwend, WORD *phend)
{
    WORD    down;
    GRECT   o;

    wm_update(TRUE);
    gr_setup(BLACK);

    r_set(&o, xorigin, yorigin, 0, 0);

    /* clamp size of rubber box to no smaller than wmin, hmin */
    do
    {
        gr_clamp(o.g_x, o.g_y, wmin, hmin, &o.g_w, &o.g_h);
        down = gr_wait(&o, poff);
    } while (down);

    *pwend = o.g_w;
    *phend = o.g_h;
    wm_update(FALSE);
}


/*
 *  Stretch the free corner of an XOR box(w,h) that is pinned at
 *  another corner, based on mouse movement until the button comes up.
 *  This is also called a rubber-band box.
 */
void gr_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
               WORD *pwend, WORD *phend)
{
    gr_rubwind(xorigin, yorigin, wmin, hmin, &gl_rzero, pwend, phend);
}


/*
 *  Drag a moving XOR box(w,h) that tracks relative to the mouse until the
 *  button comes up.  The starting x and y represent the location of the
 *  upper left hand corner of the rectangle relative to the mouse position.
 *  This relative distance should be maintained.  A constraining rectangle
 *  is also given.  The box should not be able to be dragged out of the
 *  constraining rectangle.
 */
void gr_dragbox(WORD w, WORD h, WORD sx, WORD sy, GRECT *pc,
                WORD *pdx, WORD *pdy)
{
    WORD    offx, offy, down;
    GRECT   o;

    wm_update(TRUE);
    gr_setup(BLACK);

    gr_clamp(sx+1, sy+1, 0, 0, &offx, &offy);
    r_set(&o, sx, sy, w, h);

    /* get box's x,y from mouse's x,y then constrain result */
    do
    {
        o.g_x = xrat - offx;
        o.g_y = yrat - offy;
        rc_constrain(pc, &o);
        down = gr_wait(&o, &gl_rzero);
    } while (down);

    *pdx = o.g_x;
    *pdy = o.g_y;
    wm_update(FALSE);
}


void gr_2box(WORD flag1, WORD cnt, GRECT *pt, WORD xstep, WORD ystep, WORD flag2)
{
    WORD i;

    gsx_moff();
    for (i = 0; i < 2; i++)
        gr_xor(flag1, cnt, pt->g_x, pt->g_y, pt->g_w, pt->g_h, xstep, ystep, flag2);
    gsx_mon();
}


/*
 *  Draw a moving XOR box(w,h) that moves from a point whose upper
 *  left corner is at src_x, src_y to a point at dst_x, dst_y
 */

void gr_movebox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty)
{
    WORD    signx, signy;
    WORD    cnt;
    WORD    xstep, ystep;
    GRECT   t;

    r_set(&t, srcx, srcy, w, h);

    signx = (srcx < dstx) ? -1 : 1;
    signy = (srcy < dsty) ? -1 : 1;

    gr_scale(signx*(srcx-dstx), signy*(srcy-dsty), &cnt, &xstep, &ystep);

    gr_2box(FALSE, cnt, &t, signx*xstep, signy*ystep, FALSE);
}


/*
 *  Draw a small box that moves from the origin x,y to a spot centered
 *  within the rectangle and then expands to match the size of the rectangle
 */
void gr_growbox(GRECT *po, GRECT *pt)
{
    WORD    cx, cy;
    WORD    cnt, xstep, ystep;

    gr_stepcalc(po->g_w, po->g_h, pt, &cx, &cy, &cnt, &xstep, &ystep);
    gr_movebox(po->g_w, po->g_h, po->g_x, po->g_y, cx, cy);
    po->g_x = cx;
    po->g_y = cy;
    gr_2box(TRUE, cnt, po, xstep, ystep, TRUE);
}


/*
 *  Draw a box that shrinks from the rectangle given down around a small
 *  box centered within the rectangle and then moves to the origin point
 */
void gr_shrinkbox(GRECT *po, GRECT *pt)
{
    WORD    cx, cy;
    WORD    cnt, xstep, ystep;

    gr_stepcalc(po->g_w, po->g_h, pt, &cx, &cy, &cnt, &xstep, &ystep);
    gr_2box(TRUE, cnt, pt, -xstep, -ystep, TRUE);
    gr_movebox(po->g_w, po->g_h, cx, cy, po->g_x, po->g_y);
}


WORD gr_watchbox(LONG tree, WORD obj, WORD instate, WORD outstate)
{
    WORD    out;
    WORD    state;
    GRECT   t;

    gsx_sclip(&gl_rscreen);
    ob_actxywh(tree, obj, &t);

    out = FALSE;
    do
    {
        state = (out) ? outstate : instate;
        ob_change(tree, obj, state, TRUE);
        out = !out;
    } while (gr_stilldn(out, t.g_x, t.g_y, t.g_w, t.g_h));

    return out;
}


WORD gr_slidebox(LONG tree, WORD parent, WORD obj, WORD isvert)
{
    GRECT   t, c;
    WORD    divnd, divis;

    ob_actxywh(tree, parent, &c);
    ob_relxywh(tree, obj, &t);
    gr_dragbox(t.g_w, t.g_h, t.g_x + c.g_x, t.g_y + c.g_y,
                &c, &t.g_x, &t.g_y);

    if (isvert)
    {
        divnd = t.g_y - c.g_y;
        divis = c.g_h - t.g_h;
    }
    else
    {
        divnd = t.g_x - c.g_x;
        divis = c.g_w - t.g_w;
    }

    if (divis)
        return mul_div(divnd, 1000, divis);
    else
        return 0;
}


void gr_mkstate(WORD *pmx, WORD *pmy, WORD *pmstat, WORD *pkstat)
{
    *pmx = xrat;
    *pmy = yrat;
    *pmstat = button;
    *pkstat = kstate;
}
