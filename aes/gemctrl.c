/*      GEMCTRL.C       5/14/84 - 08/22/85      Lee Jay Lorenzen        */
/*      GEM 2.0         11/06/85                Lowell Webster          */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */
/*      fix menu bar hang                       11/16/87        mdf     */

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

#include "geminput.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemgraf.h"
#include "gemmnlib.h"
#include "geminit.h"
#include "gemgsxif.h"
#include "gemgrlib.h"
#include "gemoblib.h"
#include "gemasm.h"
#include "optimopt.h"
#include "rectfunc.h"
#include "gemctrl.h"


#define THEDESK 3


/*
 * Global variables
 */
MOBLK   gl_ctwait;      /* MOBLK telling if menu bar is waiting */
                        /*  to be entered or exited by ctrl mgr */
WORD    gl_ctmown;

WORD    appl_msg[8];

/*
 * Local variables
 */
static WORD gl_tmpmoff;

/*
 * array used to convert from window object # to window message code
 *
 * the first entry maps to the up arrow object
 */
static const WORD gl_wa[] =
{
        WA_UPLINE,      /* W_UPARROW */
        WA_DNLINE,      /* W_DNARROW */
        WA_UPPAGE,      /* W_VSLIDE */
        WA_DNPAGE,      /* W_VELEV */
        0x0,            /* W_HBAR */
        WA_LFLINE,      /* W_LFARROW */
        WA_RTLINE,      /* W_RTARROW */
        WA_LFPAGE,      /* W_HSLIDE */
        WA_RTPAGE       /* W_HELEV */
};


/*
 * Send message and wait for the mouse button to come up
 */
static void ct_msgup(WORD message, AESPD *owner, WORD wh, WORD m1, WORD m2, WORD m3, WORD m4)
{
    if (message == 0)
        return;

    ap_sendmsg(appl_msg, message, owner, wh, m1, m2, m3, m4);

    /*
     * wait for button to come up if not an arrowed message
     */
    if ( message != WM_ARROWED &&
       ( message != WM_CLOSED && !(D.w_win[wh].w_kind & HOTCLOSE) ) )
    {
        while( (button & 0x0001) != 0x0 )
            dsptch();
    }
}


/*
 * perform window untopping: send WM_UNTOPPED message to window owner,
 * wait for button up, then dispatch any waiting gem processes
 */
static void perform_untop(WORD wh)
{
    WORD i;

    /* send msg, wait for button up */
    ct_msgup(WM_UNTOPPED, D.w_win[wh].w_owner, wh, 0, 0, 0, 0);

    for (i = 0; i < num_accs; i++)
        dsptch();
}


static void hctl_window(WORD w_handle, WORD mx, WORD my)
{
    GRECT   t, f, pt;
    WINDOW  *pwin = &D.w_win[w_handle];
    WORD    x, y, w, h;
    WORD    wm, hm;
    WORD    kind;
    WORD    cpt, message;
    OBJECT  *tree;

    message = 0;
    x = y = w = h = 0;

    if (w_handle == gl_wtop)
    {
        /*
         * went down on active window so handle control points
         */
        w_bldactive(w_handle);
        tree = gl_awind;
        cpt = ob_find(gl_awind, 0, 10, mx, my);
        w_getsize(WS_CURR, w_handle, &t);
        r_get(&t, &x, &y, &w, &h);
        kind = pwin->w_kind;
        switch(cpt)
        {
        case W_CLOSER:
            if ( kind & HOTCLOSE )
            {
                message = WM_CLOSED;
                break;
            }
            /* else fall thru */
        case W_FULLER:
            if ( gr_watchbox(gl_awind, cpt, SELECTED, NORMAL) )
            {
                message = (cpt == W_CLOSER) ? WM_CLOSED : WM_FULLED;
                ob_change(gl_awind, cpt, NORMAL, TRUE);
            }
            break;
        case W_NAME:
            if ( kind & MOVER )
            {
                /* prevent the mover gadget from being moved completely offscreen */
                r_set(&f, 0, gl_hbox, gl_rscreen.g_w + w - gl_wbox - 6, MAX_COORDINATE);
                gr_dragbox(w, h, x, y, &f, &x, &y);
                message = WM_MOVED;
            }
            break;
        case W_SIZER:
            if (kind & SIZER)
            {
                w_getsize(WS_WORK, w_handle, &t);
                t.g_x -= x;
                t.g_y -= y;
                t.g_w -= w;
                t.g_h -= h;
                wm = gl_wchar;
                hm = gl_hchar;
                if (kind & (LFARROW | RTARROW | HSLIDE))
                    wm = gl_wbox * 7;
                if (kind & (UPARROW | DNARROW | VSLIDE))
                    hm = gl_hbox * 7;
                gr_rubwind(x, y, wm, hm, &t, &w, &h);
                message = WM_SIZED;
            }
            break;
        case W_HSLIDE:
        case W_VSLIDE:
            ob_actxywh(tree, cpt + 1, &pt);
            if (inside(mx, my, &pt))
            {
                cpt = (cpt==W_HSLIDE) ? W_HELEV : W_VELEV;
                goto doelev;
            }

            /* fix up cpt for index into gl_wa[] */
            if (cpt == W_HSLIDE)
            {
                if ( !(mx < pt.g_x) )
                    cpt += 1;
            }
            else
            {
                if ( !(my < pt.g_y) )
                    cpt += 1;
            }
            /* fall thru */
        case W_UPARROW:
        case W_DNARROW:
        case W_LFARROW:
        case W_RTARROW:
            message = WM_ARROWED;
            x = gl_wa[cpt - W_UPARROW];
            break;
        case W_HELEV:
        case W_VELEV:
doelev:     message = (cpt == W_HELEV) ? WM_HSLID : WM_VSLID;
            x = gr_slidebox(gl_awind, cpt - 1, cpt, (cpt == W_VELEV));
            /* slide is 1 less than elev    */
            break;
        }
    }
    else
    {
        perform_untop(gl_wtop);
        /*
         * went down on inactive window so tell ap. to bring it to top
         */
        message = WM_TOPPED;
    }
    ct_msgup(message, pwin->w_owner, w_handle, x, y, w, h);
}


static void hctl_rect(void)
{
    WORD    title, item;
    WORD    mesag;
    AESPD   *owner;

    if ( gl_mntree )
    {
        mesag = 0;
        if ( mn_do(&title, &item) )
        {
            owner = gl_mnppd;
            mesag = MN_SELECTED;
            /* check system menu:   */
            if ( title == THEDESK )
            {
                if (item > 2)
                {
                    item -= 3;
                    mn_getownid(&owner,&item,item); /* get accessory owner & menu id */
                    do_chg(gl_mntree, title, SELECTED, FALSE, TRUE, TRUE);

                    if (gl_wtop >= 0)
                        perform_untop(gl_wtop);

                    mesag = AC_OPEN;
                }
                else
                    item += gl_dabox;
            }
            /*
             * application menu item has been selected so send it
             */
            ct_msgup(mesag, owner, title, item, 0, 0, 0);
        }
    }
}


/*
 * Control change of ownership to this rectangle and this process.
 * Doing the control rectangle first is important.
 */
void ct_chgown(AESPD *mpd, GRECT *pr)
{
    set_ctrl(pr);
    if (!gl_ctmown)
        set_mown(mpd);
}


void ct_mouse(WORD grabit)
{
    if (grabit)
    {
        wm_update(BEG_UPDATE);
        gl_ctmown = TRUE;
        gl_mowner = rlr;
        set_mouse_to_arrow();
        gl_tmpmoff = gl_moff;
        if (gl_tmpmoff)
            ratinit();
    }
    else
    {
        if (gl_tmpmoff)
            gsx_moff();
        gl_moff = gl_tmpmoff;
        gsx_mfset(&gl_mouse);
        gl_ctmown = FALSE;
        wm_update(END_UPDATE);
    }
}


/*
 * Internal process context used to control the screen for use by
 * the menu manager, and the window manager.
 * This process never terminates and forms an integral part of
 * the system.
 */
void ctlmgr(void)
{
    WORD    ev_which;
    WORD    rets[6];
    WORD    i, wh;

    /*
     * set defaults for multi wait
     */
    gl_ctwait.m_out = FALSE;
    rc_copy(&gl_rmenu, &gl_ctwait.m_gr);
    while(TRUE)
    {
        /* fix up ctrl rect */
        w_setactive();
        /*
         * wait for something to happen, keys need to be eaten
         * inc. fake key sent by ... or if button already down,
         * then let other guys run then do it
         */
        if (button)
        {
            for (i = 0; i < (totpds*2); i++)
                dsptch();

            ev_which = MU_BUTTON;
            rets[0] = xrat;
            rets[1] = yrat;
        }
        else
        {
            ev_which = MU_KEYBD | MU_BUTTON;
            if ( gl_mntree )            /* only wait on bar when there  */
                ev_which |= MU_M1;      /* is a menu                    */
            ev_which = ev_multi(ev_which, &gl_ctwait, &gl_ctwait,
                                0x0L, 0x0001ff01L, NULL, rets);
        }

        ct_mouse(TRUE);                 /* grab screen sink     */
        /*
         * button down over area ctrl mgr owns.  find out which
         * window the mouse clicked over and handle it
         */
        if (ev_which & MU_BUTTON)
        {
            wh = wm_find(rets[0], rets[1]);
            if (wh > 0)
                hctl_window( wh, rets[0], rets[1] );
        }
                                        /* mouse over menu bar  */
        if (ev_which & MU_M1)
            hctl_rect();
        ct_mouse(FALSE);                /* give up screen sink  */
    }
}
