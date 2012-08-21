/*      GEMCTRL.C       5/14/84 - 08/22/85      Lee Jay Lorenzen        */
/*      GEM 2.0         11/06/85                Lowell Webster          */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */ 
/*      fix menu bar hang                       11/16/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
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
#include "compat.h"
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

#define MBDOWN 0x0001
#define BELL 0x07                               /* bell                 */


/* Global variables: */
MOBLK    gl_ctwait;                             /* MOBLK telling if menu*/
                                                /*   bar is waiting     */
                                                /*   to be entered or   */
                                                /*   exited by ctrl mgr */
WORD     gl_ctmown;

WORD     appl_msg[8];
                                                /* used to convert from */
                                                /*   window object # to */
                                                /*   window message code*/
const WORD gl_wa[] =
{
        WA_UPLINE,
        WA_DNLINE,
        WA_UPPAGE,
        WA_DNPAGE,
        0x0,
        WA_LFLINE,
        WA_RTLINE,
        WA_LFPAGE,
        WA_RTPAGE
};



/* Local variable: */
static WORD             gl_tmpmoff;




/*
 * Send message and wait for the mouse button to come up
 */
static void ct_msgup(WORD message, PD *owner, WORD wh, WORD m1, WORD m2, WORD m3, WORD m4)
{
        if (message == NULL)
          return;

        ap_sendmsg(appl_msg, message, owner, wh, m1, m2, m3, m4);
                                                /* wait for button to   */
                                                /*   come up if not an  */
                                                /*   an arrowed message */
        if ( message != WM_ARROWED &&
           ( message != WM_CLOSED &&
           !(D.w_win[wh].w_kind & HOTCLOSE) ) )
        {
          while( (button & 0x0001) != 0x0 )
            dsptch();
        }
}


static void hctl_window(WORD w_handle, WORD mx, WORD my)
{
        GRECT           t, f, pt;
        WORD            x, y, w, h, ii;
        WORD            kind;
        register WORD   cpt, message;
        LONG            tree;
        OBJECT          *obj;
        
        message = 0;
        if ( (w_handle == gl_wtop) ||
             ( (D.w_win[w_handle].w_flags & VF_SUBWIN) &&
               (D.w_win[gl_wtop].w_flags & VF_SUBWIN) )  )
        {
                                                /* went down on active  */
                                                /*   window so handle   */
                                                /*   control points     */
          w_bldactive(w_handle);
          tree = gl_awind;
          cpt = ob_find(gl_awind, 0, 10, mx, my);
          w_getsize(WS_CURR, w_handle, &t);
          r_get(&t, &x, &y, &w, &h);
          kind = D.w_win[w_handle].w_kind;
          switch(cpt)
          {
            case W_CLOSER:
                if ( kind & HOTCLOSE )
                {
                  message = WM_CLOSED;
                  break;
                }
                                                /* else fall thru       */
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
                  r_set(&f, 0, gl_hbox, 10000, 10000);
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
                  gr_rubwind(x, y, 7 * gl_wbox, 7 * gl_hbox, &t, &w, &h);
                  message = WM_SIZED;
                }
                break;
            case W_HSLIDE:
            case W_VSLIDE:
                ob_actxywh(tree, cpt + 1, &pt);
                if ( cpt == W_HSLIDE )
                {
                                                /* APPLE change         */
                  /* anemic slidebars 
                    pt.g_y -= 2;
                    pt.g_h += 4;
                  */
                  if ( inside(mx, my, &pt) )
                  {
                    cpt = W_HELEV;
                    goto doelev;
                  }
                                                /* fix up for index     */
                                                /*   into gl_wa         */
                  if ( !(mx < pt.g_x) )
                    cpt += 1;
                }
                else
                {
                  /* anemic slidebars
                    pt.g_x -= 3;
                    pt.g_w += 6;
                  */
                  if ( inside(mx, my, &pt) )
                  {
                    cpt = W_VELEV;
                    goto doelev;
                  }
                  if ( !(my < pt.g_y) )
                    cpt += 1;
                }
                                                /* fall thru            */
            case W_UPARROW:
            case W_DNARROW:
            case W_LFARROW:
            case W_RTARROW:
                message = WM_ARROWED;
                break;
            case W_HELEV:
            case W_VELEV:
doelev:         message = (cpt == W_HELEV) ? WM_HSLID : WM_VSLID;
                ob_relxywh(tree, cpt - 1, &pt);
                obj = ((OBJECT *)tree) + cpt - 1;
                if ( message == WM_VSLID )
                {
                  obj->ob_x = pt.g_x;
                  obj->ob_width = pt.g_w;
                }
                else
                {
                  obj->ob_y = pt.g_y;
                  obj->ob_height = pt.g_h;
                }
                x = gr_slidebox(gl_awind, cpt - 1, cpt, (cpt == W_VELEV));
                                        /* slide is 1 less than elev    */
                break;
          }
          if (message == WM_ARROWED)
            x = gl_wa[cpt - W_UPARROW];
        }
        else
        {
          ct_msgup(WM_UNTOPPED, D.w_win[gl_wtop].w_owner, gl_wtop,
                        x, y, w, h);
          for(ii=0; ii<NUM_ACCS; ii++)
            dsptch();
                                                /* went down on inactive*/
                                                /*   window so tell ap. */
                                                /*   to bring it to top */

          message = WM_TOPPED;
        }
        ct_msgup(message, D.w_win[w_handle].w_owner, w_handle, 
                        x, y, w, h);
}


static void hctl_rect(void)
{
        WORD            title, item;
        register WORD   mesag;
        PD     *owner;
        
        if ( gl_mntree != 0x0L )
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

                if (gl_wtop >= 0 )
                {
                  WORD  ii;
                  ct_msgup(WM_UNTOPPED, D.w_win[gl_wtop].w_owner, gl_wtop,
                          0, 0, 0, 0);
                  for (ii=0; ii<NUM_ACCS; ii++)
                    dsptch();
                }
                
                mesag = AC_OPEN;
              }
              else
                item += gl_dabox;
            }
                                                /* application menu     */
                                                /*   item has been      */
                                                /*   selected so send it*/
            ct_msgup(mesag, owner, title, item, 0, 0, 0);
          }
        }
}


/*
*       Control change of ownership to this rectangle and this process.
*       Doing the control rectangle first is important.
*/
void ct_chgown(PD *mpd, GRECT *pr)
{
        set_ctrl(pr);
        if (!gl_ctmown)
          set_mown(mpd);
}


void ct_mouse(WORD grabit)
{
        
        if (grabit)
        {
          wm_update(TRUE);
          gl_ctmown = TRUE;
          gl_mowner = rlr;
          gsx_mfset(ad_armice);
          gl_tmpmoff = gl_moff; 
          if (gl_tmpmoff)
            ratinit();
        }
        else
        {
          if (gl_tmpmoff)
            gsx_moff();
          gl_moff = gl_tmpmoff;
          gsx_mfset((LONG)&gl_mouse[0]);
          gl_ctmown = FALSE;
          wm_update(FALSE);
        }
}


/*
*       Internal process context used to control the screen for use by
*       the menu manager, and the window manager.
*       This process never terminates and forms an integral part of
*       the system.
*/

void ctlmgr(void)
{
        register WORD   ev_which;
        WORD            rets[6];
        WORD            i, wh;

                                                /* set defaults for     */
                                                /*  multi wait          */
        gl_ctwait.m_out = FALSE;
        rc_copy(&gl_rmenu, (GRECT *)&gl_ctwait.m_x);
        while(TRUE)
        {
                                                /* fix up ctrl rect     */
          w_setactive();
                                                /* wait for something to*/
                                                /*   happen, keys need  */
                                                /*   to be eaten inc.   */
                                                /*   fake key sent by   */
                                                /*   or if button already*/
                                                /*   down, then let other*/
                                                /*   guys run then do it*/
          if (button)
          {
            for (i=0; i<(NUM_PDS*2); i++)
              dsptch();
            
            ev_which = MU_BUTTON;
            rets[0] = xrat;
            rets[1] = yrat;
          }
          else
          {
            ev_which = MU_KEYBD | MU_BUTTON;
            if ( gl_mntree != 0x0L )    /* only wait on bar when there  */
              ev_which |= MU_M1;        /* is a menu                    */
            ev_which = ev_multi(ev_which, &gl_ctwait, &gl_ctwait, 
                        0x0L, 0x0001ff01L, 0x0L, &rets[0]);
                                                /* grab screen sink     */
          }
          ct_mouse(TRUE);
                                                /* button down over area*/
                                                /*   ctrl mgr owns      */
                                                /* find out which wind. */
                                                /*   the mouse clicked  */
                                                /*   over and handle it */
          if (ev_which & MU_BUTTON)
          {
            wh = wm_find(rets[0], rets[1]);
            if (wh > 0)
               hctl_window( wh, rets[0], rets[1] );
          }
                                                /* mouse over menu bar  */
          if (ev_which & MU_M1)
            hctl_rect();
                                                /* give up screen sink  */
          ct_mouse(FALSE);
        }
}
