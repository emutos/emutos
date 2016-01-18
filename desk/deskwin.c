/*      DESKWIN.C       06/11/84 - 04/01/85             Lee Lorenzen    */
/*                      4/7/86                          MDF             */
/*      for 3.0         11/4/87 - 11/19/87                      mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2015 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "string.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "dos.h"
#include "gembind.h"
#include "deskbind.h"
#include "rectfunc.h"
#include "intmath.h"
#include "aesbind.h"
#include "deskobj.h"
#include "desksupp.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskmain.h"
#include "deskglob.h"
#include "deskinf.h"
#include "kprint.h"


#define SPACE 0x20

#define HOTCLOSE 0x1000

#define WINDOW_STYLE (NAME | CLOSER | MOVER | FULLER | INFO | SIZER | \
                      UPARROW | DNARROW | VSLIDE | LFARROW | RTARROW | HSLIDE)
#define START_VIEW   V_ICON


void win_view(WORD vtype, WORD isort)
{
    G.g_iview = vtype;
    G.g_isort = isort;

    switch(vtype)
    {
    case V_TEXT:
        G.g_iwext = LEN_FNODE * gl_wchar;
        G.g_ihext = gl_hchar;
        /*
         * G.g_iwint defines the width of window space in front of
         * each text line.  this must be greater than zero to allow
         * for multiple item selection by "rubber-banding"
         */
        G.g_iwint = USE_WIDE_FORMAT() ? 2*gl_wchar+4 : gl_wchar+4;
        G.g_ihint = 2;
        G.g_num = G.g_nmtext;
        G.g_pxy = &G.g_xytext[0];
        break;
    case V_ICON:
        G.g_iwext = G.g_wicon;
        G.g_ihext = G.g_hicon;
        G.g_iwint = MIN_WINT;
        G.g_ihint = MIN_HINT;
        G.g_num = G.g_nmicon;
        G.g_pxy = &G.g_xyicon[0];
        break;
    }
    G.g_iwspc = G.g_iwext + G.g_iwint;
    G.g_ihspc = G.g_ihext + G.g_ihint;
}


/*
 *  Start up by initializing global variables
 */
void win_start(void)
{
    WNODE *pw;
    WORD i;

    win_view(START_VIEW, S_NAME);
    obj_init();         /* must be called *after* win_view(), because it uses */
                        /*  G.g_iwspc/G.g_ihspc which are set by win_view()   */

    for (i = 0, G.g_wfirst = pw = G.g_wlist; i < NUM_WNODES; i++, pw++)
    {
        pw->w_next = pw + 1;
        pw->w_id = 0;
    }
    (pw-1)->w_next = NULL;
    G.g_wcnt = 0x0;
}


/*
 *  Free a window node
 */
void win_free(WNODE *thewin)
{
    if (thewin->w_id != -1)
        wind_delete(thewin->w_id);

    G.g_wcnt--;
    thewin->w_id = 0;
    objc_order(G.g_screen, thewin->w_root, 1);
    obj_wfree( thewin->w_root, 0, 0, 0, 0 );
}


/*
 *  Allocate a window node
 */
WNODE *win_alloc(WORD obid)
{
    WNODE *pw;
    WORD  wob;
    GRECT *pt;

    if (G.g_wcnt == NUM_WNODES)
        return ((WNODE *) NULL);

    pt = (GRECT *) &G.g_cnxsave.cs_wnode[G.g_wcnt].x_save;

    wob = obj_walloc(pt->g_x, pt->g_y, pt->g_w, pt->g_h);
    if (wob)
    {
        G.g_wcnt++;
        pw = &G.g_wlist[wob-2];
        pw->w_flags = 0x0;
        pw->w_obid = obid;    /* DESKTOP v1.2 */
        pw->w_root = wob;
        pw->w_cvrow = 0x0;
        pw->w_pncol = (pt->g_w  - gl_wchar) / G.g_iwspc;
        pw->w_pnrow = (pt->g_h - gl_hchar) / G.g_ihspc;
        pw->w_vnrow = 0x0;
        pw->w_id = wind_create(WINDOW_STYLE, G.g_xdesk, G.g_ydesk,
                                 G.g_wdesk, G.g_hdesk);
        if (pw->w_id != -1)
        {
            return pw;
        }
        win_free(pw);   /* decrement G.g_wcnt & call obj_wfree() */
    }

    return NULL;
}


/*
 *  Find the WNODE that has this id
 */
WNODE *win_find(WORD wh)
{
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (pw->w_id == wh)
            return pw;
    }

    return NULL;
}


/*
 *  Bring a window node to the top of the window list
 */
void win_top(WNODE *thewin)
{
    WNODE *p, *prev, *next;

    objc_order(G.g_screen, thewin->w_root, NIL);

    /*
     * find the node that points to this one
     */
    for (prev = NULL, p = G.g_wfirst; p; prev = p, p = p->w_next)
        if (p == thewin)
            break;
    if (!prev)                      /* already at front of chain */
        return;
    next = thewin->w_next;          /* remember old next ptr */

    /*
     * move it to the front
     */
    thewin->w_next = G.g_wfirst;    /* old first is now next */
    G.g_wfirst = thewin;            /* this node is now first */
    prev->w_next = next;            /* close up chain */
}


/*
 *  Find out if the window node on top has size; if it does, then it
 *  is the currently active window.  If not, then no window is on
 *  top, so return NULL.
 */
WNODE *win_ontop(void)
{
    WORD wob;

    wob = G.g_screen[ROOT].ob_tail;
    if (G.g_screen[wob].ob_width && G.g_screen[wob].ob_height)
        return &G.g_wlist[wob-2];
    else
        return NULL;
}


/*
 *  Calculate a bunch of parameters related to how many file objects
 *  will fit in a window
 */
static void win_ocalc(WNODE *pwin, WORD wfit, WORD hfit, FNODE **ppstart)
{
    FNODE *pf;
    WORD  start, cnt, w_space;

    if (wfit < 1)       /* this happens when displaying as text */
        wfit = 1;
    if (hfit < 1)
        hfit = 1;

    /*
     * zero out obid ptrs in flist and count up
     * number of files in virtual file space
     */
    cnt = 0;
    for (pf = pwin->w_path->p_flist; pf; pf = pf->f_next)
    {
        pf->f_obid = NIL;
        cnt++;
    }

    /* set windows virtual number of rows */
    pwin->w_vnrow = (cnt + wfit - 1) / wfit;
    if (pwin->w_vnrow < 1)
        pwin->w_vnrow = 1;

    /* backup cvrow & cvcol to account for more space in wind.*/
    pwin->w_pncol = wfit;
    w_space = pwin->w_pnrow = min(hfit, pwin->w_vnrow);
    while((pwin->w_vnrow - pwin->w_cvrow) < w_space)
        pwin->w_cvrow--;

    /*
     * based on the window's current virtual upper left row
     * & column, calculate the start and stop files
     */
    start = pwin->w_cvrow * pwin->w_pncol;
    pf = pwin->w_path->p_flist;
    while (start-- && pf)
        pf = pf->f_next;
    *ppstart = pf;
}


/*
 *  Calculate a bunch of parameters dealing with a particular icon
 */
static void win_icalc(FNODE *pfnode)
{
    pfnode->f_pa = app_afind(FALSE, (pfnode->f_attr&F_SUBDIR) ? AT_ISFOLD : AT_ISFILE,
                            -1, &pfnode->f_name[0], &pfnode->f_isap);
}


/*
 *  Build an object tree of the list of files that are currently
 *  viewable in a window.  Next adjust root of tree to take into
 *  account the current view of the window.
 */
void win_bldview(WNODE *pwin, WORD x, WORD y, WORD w, WORD h)
{
    FNODE *pstart;
    WORD  obid;
    WORD  r_cnt, c_cnt;
    WORD  o_wfit, o_hfit;       /* object grid */
    WORD  i_index;
    WORD  xoff, yoff, wh, sl_size, sl_value;

    /* free all this window's kids and set size */
    obj_wfree(pwin->w_root, x, y, w, h);

    /* make pstart point at 1st file in current view */
    win_ocalc(pwin, w/G.g_iwspc, h/G.g_ihspc, &pstart);
    o_wfit = pwin->w_pncol;
    o_hfit = min(pwin->w_pnrow + 1, pwin->w_vnrow - pwin->w_cvrow);
    r_cnt = c_cnt = 0;
    while ((c_cnt < o_wfit) && (r_cnt < o_hfit) && pstart)
    {
        OBJECT *obj;
        SCREENINFO *si;
        USERBLK *ub;
        ICONBLK *ib;

        /* calc offset */
        yoff = r_cnt * G.g_ihspc;
        xoff = c_cnt * G.g_iwspc;

        /* allocate object */
        obid = obj_ialloc(pwin->w_root, xoff + G.g_iwint, yoff + G.g_ihint,
                                 G.g_iwext, G.g_ihext);
        if (!obid)          /* can't allocate item object */
        {
            KDEBUG(("win_bldview(): can't create window item object\n"));
            break;
        }

        /* remember it          */
        pstart->f_obid = obid;

        /* build object */
        obj = &G.g_screen[obid];
        obj->ob_state = WHITEBAK /*| DRAW3D*/;
        obj->ob_flags = 0x00;
        si = &G.g_screeninfo[obid];
        switch(G.g_iview)
        {
        case V_TEXT:
            ub = &si->udef;
            obj->ob_type = G_USERDEF;
            obj->ob_spec = (LONG)ub;
            ub->ub_code = &dr_code;
            ub->ub_parm = (LONG)&pstart->f_junk;
            win_icalc(pstart);
            break;
        case V_ICON:
            ib = &si->icon.block;
            obj->ob_type = G_ICON;
            win_icalc(pstart);
            i_index = (pstart->f_isap) ? pstart->f_pa->a_aicon :
                                            pstart->f_pa->a_dicon;
            si->icon.index = i_index;
            obj->ob_spec = (LONG)ib;
            memcpy(ib, &G.g_iblist[i_index], sizeof(ICONBLK));
            ib->ib_ptext = pstart->f_name;
            ib->ib_char |= (0x00ff & pstart->f_pa->a_letter);
            break;
        }
        pstart = pstart->f_next;
        c_cnt++;
        if (c_cnt == o_wfit)
        {
            /* next row so skip next file in virt grid */
            r_cnt++;
            c_cnt = 0;
        }
    }

    /* set slider size & position */
    wh = pwin->w_id;
    wind_set(wh, WF_HSLSIZ, 1000, 0, 0, 0);
    sl_size = mul_div(pwin->w_pnrow, 1000, pwin->w_vnrow);
    wind_set(wh, WF_VSLSIZ, sl_size, 0, 0, 0);

    if (pwin->w_vnrow > pwin->w_pnrow)
        sl_value = mul_div(pwin->w_cvrow, 1000, pwin->w_vnrow-pwin->w_pnrow);
    else
        sl_value = 0;
    wind_set(wh, WF_VSLIDE, sl_value, 0, 0, 0);
}


/*
 *  Routine to blt the contents of a window based on a new current row
 */
static void win_blt(WNODE *pw, WORD newcv)
{
    WORD  delcv, pn;
    WORD  sx, sy, dx, dy, wblt, hblt, revblt, tmp;
    GRECT c, t;

    newcv = max(0, newcv);
    newcv = min(pw->w_vnrow - pw->w_pnrow, newcv);
    pn = pw->w_pnrow;
    delcv = newcv - pw->w_cvrow;
    pw->w_cvrow += delcv;
    if (!delcv)
        return;

    wind_get(pw->w_id, WF_WXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h);
    win_bldview(pw, c.g_x, c.g_y, c.g_w, c.g_h);

    /* see if any part is off the screen */
    wind_get(pw->w_id, WF_FIRSTXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
    if (rc_equal(&c, &t))
    {
        /* blt as much as we can, adjust clip & draw the rest */
        if ((revblt = (delcv < 0)) != 0)
            delcv = -delcv;
        if (pn > delcv)
        {
            /* see how much there is, pretend blt up */
            sx = dx = 0;
            sy = delcv * G.g_ihspc;
            dy = 0;
            wblt = c.g_w;
            hblt = c.g_h - sy;
            if (revblt)
            {
                tmp = sx;
                sx = dx;
                dx = tmp;
                tmp = sy;
                sy = dy;
                dy = tmp;
            }
            gsx_sclip(&c);
            bb_screen(S_ONLY, sx+c.g_x, sy+c.g_y, dx+c.g_x, dy+c.g_y,
                        wblt, hblt);

            if (!revblt)
                c.g_y += hblt;
            c.g_h -= hblt;
        }
    }

    do_wredraw(pw->w_id, c.g_x, c.g_y, c.g_w, c.g_h);
}


/*
 *  Routine to change the current virtual row being viewed
 *  in the upper left corner based on a new slide amount
 */
void win_slide(WORD wh, WORD sl_value)
{
    WNODE *pw;
    WORD  newcv;
    WORD  vn, pn, i, sls, sl_size;

    pw = win_find(wh);

    vn = pw->w_vnrow;
    pn = pw->w_pnrow;
    sls = WF_VSLSIZ;
    wind_get(wh, sls, &sl_size, &i, &i, &i);
    newcv = mul_div(sl_value, vn - pn, 1000);
    win_blt(pw, newcv);
}


/*
 *  Routine to change the current virtual row being viewed
 *  in the upper left corner based on a new slide amount.
 */
void win_arrow(WORD wh, WORD arrow_type)
{
    WNODE *pw;
    WORD  newcv;

    newcv = 0;
    pw = win_find(wh);

    switch(arrow_type)
    {
    case WA_UPPAGE:
        newcv = pw->w_cvrow - pw->w_pnrow;
        break;
    case WA_DNPAGE:
        newcv = pw->w_cvrow + pw->w_pnrow;
        break;
    case WA_UPLINE:
        newcv = pw->w_cvrow - 1;
        break;
    case WA_DNLINE:
        newcv = pw->w_cvrow + 1;
        break;
    }
    win_blt(pw, newcv);
}


/*
 *  Routine to sort all existing windows again
 */
void win_srtall(void)
{
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (pw->w_id != 0)
        {
            pw->w_cvrow = 0;        /* reset slider         */
            pw->w_path->p_flist = pn_sort(pw->w_path);
        }
    }
}


/*
 *  Routine to build all existing windows again
 */
void win_bdall(void)
{
    WORD  xc, yc, wc, hc;
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (pw->w_id != 0)
        {
            wind_get(pw->w_id, WF_WXYWH, &xc, &yc, &wc, &hc);
            win_bldview(pw, xc, yc, wc, hc);
        }
    }
}


/*
 *  Routine to draw all existing windows
 */
void win_shwall(void)
{
    WORD  xc, yc, wc, hc;
    WORD  wh;
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if ((wh=pw->w_id) != 0)     /* yes, assignment! */
        {
            wind_get(wh, WF_WXYWH, &xc, &yc, &wc, &hc);
            fun_msg(WM_REDRAW, wh, xc, yc, wc, hc);
        }
    }
}


/*
 *  Return the next icon that was selected after the current icon
 */
WORD win_isel(OBJECT olist[], WORD root, WORD curr)
{
    if (!curr)
        curr = olist[root].ob_head;
    else
        curr = olist[curr].ob_next;

    while(curr > root)
    {
        if ( olist[curr].ob_state & SELECTED )
            return curr;
        curr = olist[curr].ob_next;
    }

    return 0;
}


/*
 *  Return pointer to this icon's name.  It will always be an icon that
 *  is on the desktop.
 */
BYTE *win_iname(WORD curr)
{
    ICONBLK *pib;
    BYTE    *ptext;

    assert(G.g_screen[curr].ob_type == G_ICON);

    pib = (ICONBLK *)G.g_screen[curr].ob_spec;
    ptext = pib->ib_ptext;

    return ptext;
}


/*
 *  Set the name and information lines of a particular window
 */
void win_sname(WNODE *pw)
{
    BYTE *psrc;
    BYTE *pdst;

    psrc = pw->w_path->p_spec;
    pdst = pw->w_name;

    *pdst++ = ' ';
    while (*psrc && (*psrc != '*'))
        *pdst++ = *psrc++;
    *pdst++ = ' ';
    *pdst = '\0';
}


/* Added for DESKTOP v1.2 */
void win_sinfo(WNODE *pwin)
{
    PNODE *pn;

    pn = pwin->w_path;
    rsrc_gaddr(R_STRING, STINFOST, (LONG *)&G.a_alert);
    strlencpy(G.g_1text, (char *)G.a_alert);

    sprintf(pwin->w_info, G.g_1text, pn->p_size, pn->p_count);
}
