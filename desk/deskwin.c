/*      DESKWIN.C       06/11/84 - 04/01/85             Lee Lorenzen    */
/*                      4/7/86                          MDF             */
/*      for 3.0         11/4/87 - 11/19/87                      mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
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

#include "emutos.h"
#include "string.h"
#include "obdefs.h"
#include "rectfunc.h"
#include "gsxdefs.h"
#include "gemdos.h"

#include "aesdefs.h"
#include "aesext.h"
#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "intmath.h"
#include "aesbind.h"
#include "deskobj.h"
#include "desksupp.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskmain.h"
#include "deskinf.h"


#define SPACE 0x20

#define WINDOW_STYLE (NAME | CLOSER | MOVER | FULLER | INFO | SIZER | \
                      UPARROW | DNARROW | VSLIDE | LFARROW | RTARROW | HSLIDE)

/*
 * Specify the initial ob_state for icon objects in a window.  If the
 * background colour is *not* configurable, it is always white, and
 * setting the ob_state to WHITEBAK is valid.  It reduces the number of
 * blits & rectangle fills, and provides (marginally) better performance.
 */
#ifdef CONF_WITH_BACKGROUNDS
#define INITIAL_ICON_STATE  0
#else
#define INITIAL_ICON_STATE  WHITEBAK
#endif

void win_view(void)
{
    switch(G.g_iview)
    {
    case V_TEXT:
        G.g_iwext = LEN_FNODE * gl_wchar;
        G.g_ihext = gl_hchar;
        /*
         * G.g_iwint defines the width of window space in front of
         * each text line.  this must be greater than zero to allow
         * for multiple item selection by "rubber-banding"
         *
         * note: the '-1' aligns the text on a byte boundary, allowing
         * the fast text output routine to be used if other conditions
         * are met (low-rez windows can rarely take advantage of this).
         */
        G.g_iwint = 2*gl_wchar - 1;
        G.g_ihint = 2;
        break;
    case V_ICON:
        G.g_iwext = G.g_wicon;
        G.g_ihext = G.g_hicon;
        G.g_iwint = MIN_WINT;
        G.g_ihint = MIN_HINT;
        break;
    }
    G.g_iwspc = G.g_iwext + G.g_iwint;
    G.g_ihspc = G.g_ihext + G.g_ihint;
#if CONF_WITH_SIZE_TO_FIT
    {
    WORD width, dummy;

    wind_calc(1, WINDOW_STYLE, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk,
                &dummy, &dummy, &width, &dummy);
    G.g_icols = max(1, width / G.g_iwspc);
    }
#endif
}


/*
 *  Start up by initializing global variables
 */
int win_start(void)
{
    WNODE *pw;
    WORD i;

    G.g_iview = START_VIEW;
    G.g_isort = START_SORT;
#if CONF_WITH_SIZE_TO_FIT
    G.g_ifit = TRUE;
#endif

    win_view();         /* uses G.g_iview */
    obj_init();         /* must be called *after* win_view(), because it uses */
                        /*  G.g_iwspc/G.g_ihspc which are set by win_view()   */

    G.g_wdesktop.w_flags = WN_DESKTOP;  /* mark as special pseudo-window */

    G.g_wfirst = G.g_wlist = dos_alloc_anyram(NUM_WNODES*sizeof(WNODE));
    if (!G.g_wlist)
        return -1;

    for (i = 0, pw = G.g_wlist; i < NUM_WNODES; i++, pw++)
    {
        pw->w_next = pw + 1;
        pw->w_id = 0;
    }
    (pw-1)->w_next = NULL;
    G.g_wcnt = 0x0;

    return 0;
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

    pt = (GRECT *) &G.g_cnxsave->cs_wnode[G.g_wcnt].x_save;

    wob = obj_walloc(pt->g_x, pt->g_y, pt->g_w, pt->g_h);
    if (wob)
    {
        G.g_wcnt++;
        pw = &G.g_wlist[wob-2];
        pw->w_flags = 0x0;
        pw->w_obid = obid;    /* if -ve, the complement of the drive letter */
        pw->w_root = wob;
        pw->w_cvcol = 0;
        pw->w_cvrow = 0x0;
        pw->w_pncol = (pt->g_w  - gl_wchar) / G.g_iwspc;
        pw->w_pnrow = (pt->g_h - gl_hchar) / G.g_ihspc;
        pw->w_vncol = 0;
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
 *
 *  Returns NULL if not found
 */
WNODE *win_find(WORD wh)
{
    WNODE *pw;

    if (wh == 0)            /* the desktop */
        return &G.g_wdesktop;

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


#if CONF_WITH_BOTTOMTOTOP
/*
 *  Find the WNODE for the bottom window; if none, return NULL
 */
WNODE *win_onbottom(void)
{
    WNODE *p, *last = NULL;

    for (p = G.g_wfirst; p; p = p->w_next)
        if (p->w_id)
            last = p;

    return last;
}
#endif


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
 *  Return count of open windows
 */
WORD win_count(void)
{
    return G.g_wcnt;
}


/*
 *  Calculate a bunch of parameters related to how many file objects
 *  will fit in a window
 *
 *  wfit/hfit are the number of items that will fit in the current window,
 *  horizontally (wfit) and vertically (hfit)
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
    for (pf = pwin->w_pnode.p_flist; pf; pf = pf->f_next)
    {
        pf->f_obid = NIL;
        cnt++;
    }

    /*
     * set window's virtual number of rows and columns: this is the number
     * of rows and columns that would be needed to display all the files.
     * if size-to-fit, this uses the current window width; otherwise it
     * assumes a maximum-width window for the current resolution.
     */
#if CONF_WITH_SIZE_TO_FIT
    if (G.g_ifit)
    {
        pwin->w_vncol = wfit;
        pwin->w_vnrow = (cnt + wfit - 1) / wfit;
    }
    else
    {
        pwin->w_vncol = min(cnt, G.g_icols);
        pwin->w_vnrow = (cnt + G.g_icols - 1) / G.g_icols;
    }
    if (pwin->w_vncol < 1)
        pwin->w_vncol = 1;
#else
    pwin->w_vnrow = (cnt + wfit - 1) / wfit;
#endif
    if (pwin->w_vnrow < 1)
        pwin->w_vnrow = 1;

    /*
     * set window's physical number of rows and columns: this is the number
     * of rows and columns that can be displayed in the current window.
     */
    pwin->w_pncol = wfit;
    pwin->w_pnrow = min(hfit, pwin->w_vnrow);

    /*
     * adjust the window's current virtual row and column numbers in
     * case we have expanded the window
     */
#if CONF_WITH_SIZE_TO_FIT
    w_space = min(wfit, pwin->w_vncol);
    while((pwin->w_vncol - pwin->w_cvcol) < w_space)
        pwin->w_cvcol--;
#endif
    w_space = min(hfit, pwin->w_vnrow);
    while((pwin->w_vnrow - pwin->w_cvrow) < w_space)
        pwin->w_cvrow--;

    /*
     * based on the window's current virtual upper left row
     * & column, calculate the start file
     */
#if CONF_WITH_SIZE_TO_FIT
    start = pwin->w_cvrow * pwin->w_vncol + pwin->w_cvcol;
#else
    start = pwin->w_cvrow * pwin->w_pncol;
#endif
    pf = pwin->w_pnode.p_flist;
    while (start-- && pf)
        pf = pf->f_next;
    *ppstart = pf;
}


/*
 *  Update two fields in the FNODE corresponding to a particular icon:
 *      the ptr to the ANODE
 *      the flag that indicates whether this is an application or not
 */
static void win_icalc(FNODE *pfnode, WNODE *pwin)
{
    pfnode->f_pa = app_afind_by_name((pfnode->f_attr&FA_SUBDIR) ? AT_ISFOLD : AT_ISFILE,
            AF_ISDESK|AF_VIEWER, pwin->w_pnode.p_spec, pfnode->f_name, &pfnode->f_isap);
}


/*
 *  Build an object tree of the list of files that are currently
 *  viewable in a window.  Next adjust root of tree to take into
 *  account the current view of the window.
 */
void win_bldview(WNODE *pwin, WORD x, WORD y, WORD w, WORD h)
{
    FNODE *pstart;
    ANODE *anode;
    WORD  obid, skipcnt;
    WORD  r_cnt, c_cnt;
    WORD  o_wfit, o_hfit;       /* object grid */
    WORD  i_index;
    WORD  xoff, yoff, wh, sl_size, sl_value;

    MAYBE_UNUSED(skipcnt);

    /* free all this window's kids and set size */
    obj_wfree(pwin->w_root, x, y, w, h);

    /* make pstart point at 1st file in current view */
    win_ocalc(pwin, w/G.g_iwspc, h/G.g_ihspc, &pstart);
#if CONF_WITH_SIZE_TO_FIT
    o_wfit = min(pwin->w_pncol + 1, pwin->w_vncol - pwin->w_cvcol);
#else
    o_wfit = pwin->w_pncol;
#endif
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
        si = &G.g_screeninfo[obid];
        si->fnptr = pstart;

        /* build object */
        obj = &G.g_screen[obid];
        obj->ob_state = INITIAL_ICON_STATE;
        if (pstart->f_selected)
            obj->ob_state |= SELECTED;
        obj->ob_flags = 0x00;
        switch(G.g_iview)
        {
        case V_TEXT:
            ub = &si->u.udef;
            obj->ob_type = G_USERDEF;
            obj->ob_spec = (LONG)ub;
            ub->ub_code = dr_code;
            ub->ub_parm = (LONG)pstart;
            win_icalc(pstart, pwin);
            break;
        case V_ICON:
            ib = &si->u.icon.block;
            obj->ob_type = G_ICON;
            win_icalc(pstart, pwin);
            anode = pstart->f_pa;
            if (anode)
                i_index = (pstart->f_isap) ? anode->a_aicon : anode->a_dicon;
            else
            {
                KDEBUG(("win_bldview(): NULL anode, using defaults\n"));
                if (pstart->f_attr&FA_SUBDIR)
                    i_index = IG_FOLDER;
                else
                    i_index = (pstart->f_isap) ? IG_APPL : IG_DOCU;
            }
            si->u.icon.index = i_index;
            obj->ob_spec = (LONG)ib;
            memcpy(ib, &G.g_iblist[i_index], sizeof(ICONBLK));
            ib->ib_ptext = pstart->f_name;
            if (anode)
                ib->ib_char |= anode->a_letter;
            break;
        }
        pstart = pstart->f_next;
        c_cnt++;
        if (c_cnt == o_wfit)
        {
            /* next row so skip next file in virt grid */
            r_cnt++;
            c_cnt = 0;
#if CONF_WITH_SIZE_TO_FIT
            skipcnt = pwin->w_vncol - o_wfit;
            while(skipcnt-- && pstart)
                pstart = pstart->f_next;
#endif
        }
    }

    /* set slider size & position */
    wh = pwin->w_id;

#if CONF_WITH_SIZE_TO_FIT
    sl_size = mul_div(pwin->w_pncol, 1000, pwin->w_vncol);
    wind_set(wh, WF_HSLSIZ, sl_size, 0, 0, 0);
    if (pwin->w_vncol > pwin->w_pncol)
        sl_value = mul_div(pwin->w_cvcol, 1000, pwin->w_vncol-pwin->w_pncol);
    else
        sl_value = 0;
    wind_set(wh, WF_HSLIDE, sl_value, 0, 0, 0);
#else
    wind_set(wh, WF_HSLSIZ, 1000, 0, 0, 0);
#endif

    sl_size = mul_div(pwin->w_pnrow, 1000, pwin->w_vnrow);
    wind_set(wh, WF_VSLSIZ, sl_size, 0, 0, 0);
    if (pwin->w_vnrow > pwin->w_pnrow)
        sl_value = mul_div(pwin->w_cvrow, 1000, pwin->w_vnrow-pwin->w_pnrow);
    else
        sl_value = 0;
    wind_set(wh, WF_VSLIDE, sl_value, 0, 0, 0);
}


/*
 *  Calculate the change in virtual row/column number required to
 *  display a new virtual row/column number, allowing for the
 *  height/width of the window
 */
static WORD win_delta(WNODE *pw, BOOL horizontal, WORD newcv)
{
    WORD delta;

    newcv = max(0, newcv);

#if CONF_WITH_SIZE_TO_FIT
    if (horizontal)
    {
        delta = min(pw->w_vncol - pw->w_pncol, newcv) - pw->w_cvcol;
    }
    else
#endif
    {
        delta = min(pw->w_vnrow - pw->w_pnrow, newcv) - pw->w_cvrow;
    }

    return delta;
}


/*
 *  Routine to blt the contents of a window based on a new current row
 *  or column
 */
static void win_blt(WNODE *pw, BOOL horizontal, WORD newcv)
{
    WORD  delcv;
    WORD  sx, sy, dx, dy, wblt, hblt, revblt, tmp;
    GRECT c, t;

    delcv = win_delta(pw, horizontal, newcv);
    if (!delcv)
        return;

#if CONF_WITH_SIZE_TO_FIT
    if (horizontal)
    {
        pw->w_cvcol += delcv;
    }
    else
#endif
    {
        pw->w_cvrow += delcv;
    }

    wind_get_grect(pw->w_id, WF_WXYWH, &c);
    win_bldview(pw, c.g_x, c.g_y, c.g_w, c.g_h);

    /* see if any part is off the screen */
    wind_get_grect(pw->w_id, WF_FIRSTXYWH, &t);
    if (rc_equal(&c, &t))
    {
        /* blt as much as we can, adjust clip & draw the rest */
        if ((revblt = (delcv < 0)) != 0)
            delcv = -delcv;
        if (pw->w_pnrow > delcv)
        {
            /* see how much there is, pretend blt up */
#if CONF_WITH_SIZE_TO_FIT
            if (horizontal)
            {
                sy = dy = 0;
                sx = delcv * G.g_iwspc;
                dx = 0;
                wblt = c.g_w - sx;
                hblt = c.g_h;
            }
            else
#endif
            {
                sx = dx = 0;
                sy = delcv * G.g_ihspc;
                dy = 0;
                wblt = c.g_w;
                hblt = c.g_h - sy;
            }
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
#if CONF_WITH_SIZE_TO_FIT
            if (horizontal)
            {
                if (!revblt)
                    c.g_x += wblt;
                c.g_w -= wblt;
            }
            else
#endif
            {
                if (!revblt)
                    c.g_y += hblt;
                c.g_h -= hblt;
            }
        }
    }

    do_wredraw(pw->w_id, &c);
}


#if CONF_WITH_SEARCH
/*
 *  Routine to update the window display so that it shows the
 *  specified file number (numbered sequentially from 0, in the
 *  current display sequence)
 */
void win_dispfile(WNODE *pw, WORD file)
{
    GRECT gr;
    WORD col, delcv;

    /*
     * adjust starting row of display
     */
#if CONF_WITH_SIZE_TO_FIT
    col = G.g_ifit ? pw->w_pncol : pw->w_vncol;
#else
    col = pw->w_pncol;
#endif
    delcv = win_delta(pw, FALSE, file/col); /* FALSE => calculate vertical delta */
    pw->w_cvrow += delcv;

    /*
     * adjust starting column of display to be as far left as possible,
     * while still displaying the item
     */
#if CONF_WITH_SIZE_TO_FIT
    pw->w_cvcol = max(0, file%col - pw->w_pncol + 1);
#else
    pw->w_cvcol = 0;
#endif

    wind_get_grect(pw->w_id, WF_WXYWH, &gr);
    win_bldview(pw, gr.g_x, gr.g_y, gr.g_w, gr.g_h);
    do_wredraw(pw->w_id, &gr);
}
#endif


/*
 *  Routine to change the current virtual row or column being viewed
 *  in the upper left corner based on a new slide amount
 */
void win_slide(WORD wh, BOOL horizontal, WORD sl_value)
{
    WNODE *pw;
    WORD  newcv;
    WORD  vn, pn, i, sls, sl_size;

    pw = win_find(wh);
    if (!pw)
        return;

#if CONF_WITH_SIZE_TO_FIT
    if (horizontal)
    {
        vn = pw->w_vncol;
        pn = pw->w_pncol;
        sls = WF_HSLSIZ;
    }
    else
#endif
    {
        vn = pw->w_vnrow;
        pn = pw->w_pnrow;
        sls = WF_VSLSIZ;
    }
    wind_get(wh, sls, &sl_size, &i, &i, &i);
    newcv = mul_div(sl_value, vn - pn, 1000);
    win_blt(pw, horizontal, newcv);
}


/*
 *  Routine to change the current virtual row or column being viewed
 *  in the upper left corner based on a new slide amount.
 */
void win_arrow(WORD wh, WORD arrow_type)
{
    WNODE *pw;
    WORD  newcv;
    BOOL  horizontal = FALSE;

    pw = win_find(wh);
    if (!pw)
        return;

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
#if CONF_WITH_SIZE_TO_FIT
    case WA_LFPAGE:
        newcv = pw->w_cvcol - pw->w_pncol;
        horizontal = TRUE;
        break;
    case WA_RTPAGE:
        newcv = pw->w_cvcol + pw->w_pncol;
        horizontal = TRUE;
        break;
    case WA_LFLINE:
        newcv = pw->w_cvcol - 1;
        horizontal = TRUE;
        break;
    case WA_RTLINE:
        newcv = pw->w_cvcol + 1;
        horizontal = TRUE;
        break;
#endif
    default:
        return;     /* ignore WA_LFPAGE, WA_RTPAGE, WA_LFLINE, WA_RTLINE */
    }
    win_blt(pw, horizontal, newcv);
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
            pw->w_pnode.p_flist = pn_sort(&pw->w_pnode);
        }
    }
}


/*
 *  Routine to build all existing windows again
 */
void win_bdall(void)
{
    GRECT clip;
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (pw->w_id != 0)
        {
            wind_get_grect(pw->w_id, WF_WXYWH, &clip);
            win_bldview(pw, clip.g_x, clip.g_y, clip.g_w, clip.g_h);
        }
    }
}


/*
 *  Routine to draw all existing windows
 */
void win_shwall(void)
{
    GRECT clip;
    WORD  wh;
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if ((wh=pw->w_id) != 0)     /* yes, assignment! */
        {
            wind_get_grect(wh, WF_WXYWH, &clip);
            fun_msg(WM_REDRAW, wh, clip.g_x, clip.g_y, clip.g_w, clip.g_h);
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
 *  Update the name line for a window
 */
void win_sname(WNODE *pw)
{
    char *psrc;
    char *pdst;

    psrc = pw->w_pnode.p_spec;
    pdst = pw->w_name;

    *pdst++ = ' ';
    while (*psrc)
        *pdst++ = *psrc++;
    *pdst++ = ' ';
    *pdst = '\0';
}


/*
 * Set the info line for a window
 *
 * Optionally, count selected items
 */
void win_sinfo(WNODE *pwin, BOOL check_selected)
{
    PNODE *pn;
    FNODE *fn;
    char *alert;
    WORD i, select_count = 0;
    LONG select_size = 0L;

    pn = &pwin->w_pnode;

    if (check_selected)
    {
        /* count selected FNODEs */
        for (i = 0, fn = pn->p_fbase; i < pn->p_count; i++, fn++)
        {
            if (fn->f_selected)
            {
                select_count++;
                select_size += fn->f_size;
            }
        }
    }

    /*
     * choose the appropriate string
     */
    if (select_count)
    {
        alert = desktop_str_addr(STINFST2);
    }
    else
    {
        alert = desktop_str_addr(STINFOST);
        select_count = pn->p_count;     /* so we can use common code below */
        select_size = pn->p_size;
    }

    sprintf(pwin->w_info, alert, select_size, select_count);
    wind_set(pwin->w_id, WF_INFO, pwin->w_info, 0, 0);
}
