/*      GEMCTRL.C       5/14/84 - 08/22/85      Lee Jay Lorenzen        */
/*      GEM 2.0         11/06/85                Lowell Webster          */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */
/*      fix menu bar hang                       11/16/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2022 The EmuTOS development team
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

#include "emutos.h"
#include "struct.h"
#include "aesdefs.h"
#include "aesext.h"
#include "aesvars.h"
#include "obdefs.h"

#include "geminput.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemmnlib.h"
#include "geminit.h"
#include "gemgsxif.h"
#include "gemgrlib.h"
#include "gemoblib.h"
#include "gemobjop.h"
#include "gemasm.h"
#include "rectfunc.h"
#include "gemctrl.h"


#define THEDESK 3       /* MUST be the same value as DESKMENU in desk/desk_rsc.h */


/*
 * Global variables
 */
MOBLK   gl_ctwait;      /* MOBLK telling if menu bar is waiting */
                        /*  to be entered or exited by ctrl mgr */
BOOL    gl_ctmown;

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
    if (message)
        ap_sendmsg(appl_msg, message, owner, wh, m1, m2, m3, m4);

#if CONF_WITH_PCGEM
    /*
     * if close on a hotclose window, return immediately
     */
    if ((message == WM_CLOSED) && (D.w_win[wh].w_kind & HOTCLOSE))
    {
        button = 0;     /* we fake button up, otherwise a slow click will */
        return;         /*  cause us to stay in ctlmgr(), eating keys     */
    }
#endif

    /*
     * otherwise, wait for the button to come up
     */
    while(button & 0x0001)
        dsptch();
}


/*
 * handle WM_ARROWED message
 *
 * this function sends WM_ARROWED messages continuously whilst the left
 * button is held down.
 */
static void handle_arrow_msg(WORD w_handle, WORD gadget)
{
    WINDOW *pwin = &D.w_win[w_handle];
    AESPD *p = pwin->w_owner;
    WORD action;

    wm_update(END_UPDATE);      /* give up the screen */

    action = gl_wa[gadget - W_UPARROW];

    do
    {
        if (p->p_stat & WAITIN) /* send message now if he's waiting */
        {
            ap_sendmsg(appl_msg, WM_ARROWED, p, w_handle, action, 0, 0, 0);
        }
        else                    /* else make it the next to be processed */
        {
            if (p->p_msg.action < 0)    /* (if no pending message) */
            {
                p->p_msg.action = action;
                p->p_msg.wh = w_handle;
            }
        }
        dsptch();
    } while(button & 0x0001);

    wm_update(BEG_UPDATE);      /* take back the screen */
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
    GRECT   t, f;
    WINDOW  *pwin = &D.w_win[w_handle];
    WORD    x, y, w, h;
    WORD    wm, hm;
    WORD    elev_x, elev_y;
    WORD    kind;
    WORD    cpt, message;
    WORD    gadget, normal, selected;
#if !CONF_WITH_3D_OBJECTS
    BOOL    need_normal = FALSE;
#endif

    if (w_handle != gl_wtop)
    {
        perform_untop(gl_wtop);
        /*
         * went down on inactive window so tell owner to bring it to top
         */
        ct_msgup(WM_TOPPED, pwin->w_owner, w_handle, 0, 0, 0, 0);
        return;
    }

    message = 0;

    /*
     * went down on active window so handle control points
     */
    w_bldactive(w_handle);
    gadget = cpt = ob_find(gl_awind, W_BOX, MAX_DEPTH, mx, my);
#if CONF_WITH_3D_OBJECTS
    normal = gl_awind[gadget].ob_state & ~SELECTED;
    selected = normal | SELECTED;
#else
    normal = NORMAL;
    selected = SELECTED;
#endif
    w_getsize(WS_CURR, w_handle, &t);
    r_get(&t, &x, &y, &w, &h);
    kind = pwin->w_kind;

#if CONF_WITH_3D_OBJECTS
    /* since we animate gadgets, we must set clipping here */
    ob_actxywh(gl_awind, gadget, &f);
    gsx_sclip(&f);
#endif

    switch(cpt)
    {
    case W_CLOSER:
#if CONF_WITH_PCGEM
        if (kind & HOTCLOSE)
        {
            message = WM_CLOSED;
            break;
        }
        FALLTHROUGH;
#endif
    case W_FULLER:
        if (gr_watchbox(gl_awind, gadget, selected, normal))
        {
            message = (cpt == W_CLOSER) ? WM_CLOSED : WM_FULLED;
#if !CONF_WITH_3D_OBJECTS
            need_normal = TRUE;
#endif
        }
        break;
    case W_NAME:
        if (kind & MOVER)
        {
#if CONF_WITH_3D_OBJECTS
            ob_change(gl_awind, gadget, selected, TRUE);
            /* prevent the mover gadget from being moved completely offscreen */
            r_set(&f, 0, gl_hbox, gl_rscreen.g_w+w-gl_wbox-6-2*ADJ3DSTD, MAX_COORDINATE);
#else
            /* prevent the mover gadget from being moved completely offscreen */
            r_set(&f, 0, gl_hbox, gl_rscreen.g_w+w-gl_wbox-6, MAX_COORDINATE);
#endif
            gr_dragbox(w, h, x, y, &f, &x, &y);
            message = WM_MOVED;
        }
        break;
    case W_SIZER:
        if (kind & SIZER)
        {
#if CONF_WITH_3D_OBJECTS
            ob_change(gl_awind, gadget, selected, TRUE);
#endif
            w_getsize(WS_WORK, w_handle, &t);
            t.g_x -= x;
            t.g_y -= y;
            t.g_w -= w;
            t.g_h -= h;
            wm = gl_wchar;
            hm = gl_hchar;
            if (kind & (TGADGETS | HGADGETS))
                wm = gl_wbox * 4;
            if (kind & VGADGETS)
                hm = gl_hbox * 6;
            gr_rubwind(x, y, wm, hm, &t, &w, &h);
            message = WM_SIZED;
        }
        break;
    case W_HSLIDE:
    case W_VSLIDE:
        /*
         * because of the way W_ACTIVE is arranged, adding 1 to cpt
         * converts W_HSLIDE->W_HELEV, W_VSLIDE->W_VELEV
         */
        ob_offset(gl_awind, cpt+1, &elev_x, &elev_y);
        if (cpt == W_HSLIDE)
        {
            if ( !(mx < elev_x) )
                cpt += 1;
        }
        else
        {
            if ( !(my < elev_y) )
                cpt += 1;
        }
        FALLTHROUGH;
    case W_UPARROW:
    case W_DNARROW:
    case W_LFARROW:
    case W_RTARROW:
#if CONF_WITH_3D_OBJECTS
        if ((gadget != W_HSLIDE) && (gadget != W_VSLIDE))
            ob_change(gl_awind, gadget, selected, TRUE);
#endif
        handle_arrow_msg(w_handle, cpt);
#if CONF_WITH_3D_OBJECTS
        if ((gadget != W_HSLIDE) && (gadget != W_VSLIDE))
            ob_change(gl_awind, gadget, normal, TRUE);
#endif
        return;
        break;
    case W_HELEV:
    case W_VELEV:
#if CONF_WITH_3D_OBJECTS
        ob_change(gl_awind, gadget, selected, TRUE);
#endif
        message = (cpt == W_HELEV) ? WM_HSLID : WM_VSLID;
        /*
         * as noted above, subtracting 1 from cpt converts
         * W_HELEV->W_HSLIDE, W_VELEV->W_VSLIDE
         */
        x = gr_slidebox(gl_awind, cpt-1, cpt, (cpt == W_VELEV));
        break;
    }

#if !CONF_WITH_3D_OBJECTS
    if (need_normal)
#endif
        ob_change(gl_awind, gadget, normal, TRUE);

    ct_msgup(message, pwin->w_owner, w_handle, x, y, w, h);
}


static void hctl_rect(void)
{
    WORD    title, item;
    WORD    mesag;
    AESPD   *owner;
#if CONF_WITH_MENU_EXTENSION
    OBJECT *tree;
    WORD    treehi, treelo, parent;
#endif

    if ( gl_mntree )
    {
#if CONF_WITH_MENU_EXTENSION
        if (mn_do(&title, &item, &tree))
#else
        if ( mn_do(&title, &item) )
#endif
        {
            /* check system menu:   */
            if ((title == THEDESK) && (item >= gl_dafirst))
            {
                item -= gl_dafirst;
                mn_getownid(&owner,&item,item); /* get accessory owner & menu id */
                do_chg(gl_mntree, title, SELECTED, FALSE, TRUE, TRUE);

                if (gl_wtop >= 0)
                    perform_untop(gl_wtop);

                mesag = AC_OPEN;
#if CONF_WITH_MENU_EXTENSION
                treehi = treelo = parent = 0;
#endif
            }
            else
            {
                owner = gl_mnppd;
                mesag = MN_SELECTED;
#if CONF_WITH_MENU_EXTENSION
                treehi = HIWORD(tree);
                treelo = LOWORD(tree);
                parent = get_par(tree, item);
#endif
            }
            /*
             * application menu item has been selected so send it
             */
#if CONF_WITH_MENU_EXTENSION
            ct_msgup(mesag, owner, title, item, treehi, treelo, parent);
#else
            ct_msgup(mesag, owner, title, item, 0, 0, 0);
#endif
        }
    }
}


#if CONF_WITH_3D_OBJECTS
/*
 * handle messages caught by control manager
 *
 * we currently only handle redraws
 */
static void hctl_msg(WORD *msgbuf)
{
    if (msgbuf[0] == WM_REDRAW)
        w_redraw_desktop((GRECT *)&msgbuf[4]);
}
#endif


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
        gl_ctmown = TRUE;
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
    WORD    wh;
    WORD    msgbuf[8];

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
         * including fake key sent by mn_bar() [the menu bar handler]
         */
#if CONF_WITH_3D_OBJECTS
        ev_which = MU_KEYBD | MU_BUTTON | MU_MESAG;
#else
        ev_which = MU_KEYBD | MU_BUTTON;
#endif
        if (gl_mntree)                  /* only wait on bar when there  */
            ev_which |= MU_M1;          /* is a menu                    */

#if CONF_WITH_MENU_EXTENSION
        ev_which = ev_multi(ev_which, &gl_ctwait, &gl_ctwait, NULL,
                                0x0L, 0x0001ff01L, msgbuf, rets);
#else
        ev_which = ev_multi(ev_which, &gl_ctwait, &gl_ctwait,
                                0x0L, 0x0001ff01L, msgbuf, rets);
#endif

        wm_update(BEG_UPDATE);          /* take the screen */
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

#if CONF_WITH_3D_OBJECTS
        /* handle messages for e.g. depressed activators */
        if (ev_which & MU_MESAG)
            hctl_msg(msgbuf);
#endif

        wm_update(END_UPDATE);          /* give up the screen */
    }
}
