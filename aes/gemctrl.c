/*	GEMCTRL.C	5/14/84 - 08/22/85	Lee Jay Lorenzen	*/
/*	GEM 2.0		11/06/85		Lowell Webster		*/
/*	merge High C vers. w. 2.2 & 3.0		8/19/87		mdf	*/ 
/*	fix menu bar hang			11/16/87	mdf	*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <portab.h>
#include <machine.h>
#include <struct.h>
#include <basepage.h>
#include <obdefs.h>
#include <taddr.h>
#include <gemlib.h>


#define FIXLATER	0

#if MULTIAPP
EXTERN PD	*fpdnm();
EXTERN WORD	sh_chmsg();
#endif

#define THEDESK 3

#define MBDOWN 0x0001
#define BELL 0x07				/* bell			*/

						/* in INPUT88.C		*/
EXTERN WORD	set_ctrl();
						/* in EVLIB.C		*/
EXTERN WORD	ev_multi();
						/* in WMLIB.C		*/
EXTERN WORD	w_getsize();
EXTERN WORD	w_bldactive();
EXTERN WORD	w_setactive();

#if FIXLATER
EXTERN WORD	mn_indextoid();
#endif
						/* in ASM.A86		*/
EXTERN WORD	dsptch();

EXTERN LONG	gl_mntree;
EXTERN PD	*gl_mnppd;
EXTERN WORD	gl_dabox;
EXTERN PD	*desk_ppd[];


EXTERN LONG	gl_awind;

EXTERN WORD	gl_wtop;
EXTERN WORD	gl_wbox;
EXTERN WORD	gl_hbox;
EXTERN GRECT	gl_rmenu;

EXTERN WORD	button, xrat, yrat;
EXTERN PD 	*gl_mowner;
EXTERN LONG	ad_armice, ad_mouse;
EXTERN WORD	gl_moff;


EXTERN THEGLO	D;

/* ---------- added for metaware compiler ------------- */
EXTERN VOID     ob_change();			/* in OBLIB.C		*/
EXTERN WORD     ob_find();
EXTERN VOID     ob_actxywh();	
EXTERN VOID     ob_relxywh();
EXTERN VOID     gr_dragbox();			/* in GRAF.C		*/
EXTERN VOID     gr_rubwind();
EXTERN WORD     gr_watchbox();
EXTERN WORD     gr_slidebox();
EXTERN WORD     mn_do();			/* in MNLIB.C		*/
EXTERN VOID     do_chg();
EXTERN VOID     set_mown();			/* in INPUT.C		*/
EXTERN VOID     wm_update();			/* in WMLIB.C 		*/
EXTERN WORD     wm_find();
EXTERN VOID     ap_sendmsg();
EXTERN VOID     gsx_mfset();			/* in GSXIF.C		*/
EXTERN VOID     ratinit();
EXTERN VOID     gsx_moff();
EXTERN VOID     rc_copy();			/* in OPTIMOPT.a86	*/  
EXTERN VOID     r_get();			
EXTERN VOID     r_set();
EXTERN WORD     inside();

/* ---------------------------------------------------- */

GLOBAL MOBLK	gl_ctwait;
GLOBAL WORD	gl_ctmown;

GLOBAL WORD	appl_msg[8];
						/* used to convert from	*/
						/*   window object # to	*/
						/*   window message code*/
GLOBAL WORD	gl_wa[] =
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


MLOCAL WORD		gl_tmpmoff;


/*
*	Send message and wait for the mouse button to come up
*/
	VOID
ct_msgup(message, owner, wh, m1, m2, m3, m4)
	WORD		message;
	PD		*owner;
	WORD		wh;
	WORD		m1, m2, m3, m4;
{
	if (message == NULL)
	  return;

	ap_sendmsg(appl_msg, message, owner, wh, m1, m2, m3, m4);
						/* wait for button to	*/
						/*   come up if not an	*/
						/*   an arrowed message	*/
	if ( message != WM_ARROWED &&
	   ( message != WM_CLOSED &&
	   !(D.w_win[wh].w_kind & HOTCLOSE) ) )
	{
	  while( (button & 0x0001) != 0x0 )
	    dsptch();
	}
}


	VOID
hctl_window(w_handle, mx, my)
	REG WORD	w_handle;
	WORD		mx, my;
{
	GRECT		t, f, pt;
	WORD		x, y, w, h, ii;
	WORD		kind;
	REG WORD	cpt, message;
	LONG		tree;
	
	message = 0;
	if ( (w_handle == gl_wtop) ||
	     ( (D.w_win[w_handle].w_flags & VF_SUBWIN) &&
	       (D.w_win[gl_wtop].w_flags & VF_SUBWIN) )  )
	{
						/* went down on active	*/
						/*   window so handle	*/
						/*   control points	*/
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
						/* else fall thru	*/
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
						/* APPLE change		*/
		  pt.g_y -= 2;
		  pt.g_h += 4;
		  if ( inside(mx, my, &pt) )
		  {
		    cpt = W_HELEV;
		    goto doelev;
		  }
						/* fix up for index	*/
						/*   into gl_wa		*/
		  if ( !(mx < pt.g_x) )
		    cpt += 1;
		}
		else
		{
		  pt.g_x -= 3;
		  pt.g_w += 6;
		  if ( inside(mx, my, &pt) )
		  {
		    cpt = W_VELEV;
		    goto doelev;
		  }
		  if ( !(my < pt.g_y) )
		    cpt += 1;
		}
						/* fall thru		*/
	    case W_UPARROW:
	    case W_DNARROW:
	    case W_LFARROW:
	    case W_RTARROW:
		message = WM_ARROWED;
		break;
	    case W_HELEV:
	    case W_VELEV:
doelev:		message = (cpt == W_HELEV) ? WM_HSLID : WM_VSLID;
		ob_relxywh(tree, cpt - 1, &pt);
		if ( message == WM_VSLID )
		{
		  pt.g_x += 3;		/* APPLE	*/
		  pt.g_w -= 6;
		  LWSET(OB_X(cpt - 1), pt.g_x);
		  LWSET(OB_WIDTH(cpt - 1), pt.g_w);
		}
		else
		{
		  pt.g_y += 2;		/* APPLE	*/
		  pt.g_h -= 4;
		  LWSET(OB_Y(cpt - 1), pt.g_y);
		  LWSET(OB_HEIGHT(cpt - 1), pt.g_h);
		}
		x = gr_slidebox(gl_awind, cpt - 1, cpt, (cpt == W_VELEV));
		if ( message == WM_VSLID )
		{
		  pt.g_x -= 3;
		  pt.g_w += 6;
		  LWSET(OB_X(cpt - 1), pt.g_x);
		  LWSET(OB_WIDTH(cpt - 1), pt.g_w);
		}
		else
		{
		  pt.g_y -= 2;
		  pt.g_h += 4;
		  LWSET(OB_Y(cpt - 1), pt.g_y);
		  LWSET(OB_HEIGHT(cpt - 1), pt.g_h);
		}
					/* slide is 1 less than elev	*/
		break;
	  }
	  if (message == WM_ARROWED)
	    x = gl_wa[cpt - W_UPARROW];
	}
	else
	{
#if SINGLAPP
	  ct_msgup(WM_UNTOPPED, D.w_win[gl_wtop].w_owner, gl_wtop,
			x, y, w, h);
          for(ii=0; ii<NUM_ACCS; ii++)
	    dsptch();
#endif
						/* went down on inactive*/
						/*   window so tell ap.	*/
						/*   to bring it to top	*/

	  message = WM_TOPPED;
	}
	ct_msgup(message, D.w_win[w_handle].w_owner, w_handle, 
			x, y, w, h);
}

	VOID
hctl_rect()
{
	WORD		title, item;
	REG WORD	mesag;
	REG PD		*owner;
#if SINGLAPP
        WORD		ii;
#endif
	
	if ( gl_mntree != 0x0L )
	{
	  mesag = 0;
	  if ( mn_do(&title, &item) )
	  {
	    owner = gl_mnppd;
	    mesag = MN_SELECTED;
						/* check system menu:	*/
	    if ( title == THEDESK )
	    {
	      if (item > 2)
	      {
		item -= 3;
	        owner = desk_ppd[item];
#if FIXLATER
	/* use w, new mnlib	 */
		item  = mn_indextoid(item);
#endif
	        do_chg(gl_mntree, title, SELECTED, FALSE, TRUE, TRUE);
#if SINGLAPP
		if (gl_wtop >= 0 )
		{
		  ct_msgup(WM_UNTOPPED, D.w_win[gl_wtop].w_owner, gl_wtop,
			  0, 0, 0, 0);
		  for (ii=0; ii<NUM_ACCS; ii++)
		    dsptch();
	        }
		
		mesag = AC_OPEN;
#endif
#if MULTIAPP
					/* release screen so apps can	*/
					/*  get it.			*/
		ct_mouse(FALSE);
	
		mesag = sh_chmsg(owner);
		  			/* get screen back to keep ctrl	*/
					/*  manager happy.		*/
		ct_mouse(TRUE);
#endif
	      }
	      else
		item += gl_dabox;
	    }
						/* application menu	*/
						/*   item has been 	*/
						/*   selected so send it*/
	    ct_msgup(mesag, owner, title, item, 0, 0, 0);
	  }
	}
}


/*
*	Control change of ownership to this rectangle and this process.
*	Doing the control rectangle first is important.
*/
	VOID
#if FIXLATER
ct_chgown(mpd, cpd, pr)
	PD		*mpd, *cpd;
#else
ct_chgown(mpd, pr)
	PD		*mpd;
#endif
	GRECT		*pr;
{
	set_ctrl(pr);
	if (!gl_ctmown)
#if FIXLATER
	  set_mown(mpd, cpd); 
#else
	  set_mown(mpd); 
#endif
}

        VOID
ct_mouse(grabit)
	WORD		grabit;
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
	  gsx_mfset(ad_mouse);
	  gl_ctmown = FALSE;
	  wm_update(FALSE);
	}
}

#if MULTIAPP
	VOID
hctl_mesag(pmbuf)
	WORD		pmbuf[];
{
	PD	*ppd;
	WORD	mesag;

	if (pmbuf[0] == CT_SWITCH)
	{
	  ppd = fpdnm(NULLPTR, pmbuf[3]);
	  if (ppd)
	  {
	    mesag = sh_chmsg(ppd);
	    ct_msgup(mesag, ppd, 0, 0, 0, 0, 0);
	  }
	}
}
#endif

/*
*	Internal process context used to control the screen for use by
*	the menu manager, and the window manager.
*	This process never terminates and forms an integral part of
*	the system.
*/

	VOID
ctlmgr()
{
	REG WORD	ev_which;
	WORD		rets[6];
	WORD		i, wh;
#if MULTIAPP
	WORD		cmsg[8];
#endif
						/* set defaults for 	*/
						/*  multi wait		*/
	gl_ctwait.m_out = FALSE;
	rc_copy(&gl_rmenu, &gl_ctwait.m_x);
	while(TRUE)
	{
						/* fix up ctrl rect	*/
	  w_setactive();
						/* wait for something to*/
						/*   happen, keys need	*/
						/*   to be eaten inc.	*/
						/*   fake key sent by	*/
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
	    if ( gl_mntree != 0x0L )	/* only wait on bar when there	*/
	      ev_which |= MU_M1;	/* is a menu 			*/
#if MULTIAPP
	    ev_which = ev_multi(ev_which | MU_MESAG, 
			&gl_ctwait, &gl_ctwait, 
			0x0L, 0x0001ff01L, ADDR(&cmsg[0]), &rets[0]);
#else
	    ev_which = ev_multi(ev_which, &gl_ctwait, &gl_ctwait, 
			0x0L, 0x0001ff01L, 0x0L, &rets[0]);
#endif
						/* grab screen sink	*/
	  }
	  ct_mouse(TRUE);
						/* button down over area*/
						/*   ctrl mgr owns	*/
						/* find out which wind.	*/
						/*   the mouse clicked  */
						/*   over and handle it	*/
	  if (ev_which & MU_BUTTON)
	  {
	    wh = wm_find(rets[0], rets[1]);
	    if (wh > 0)
      	       hctl_window( wh, rets[0], rets[1] );
	  }
						/* mouse over menu bar	*/
	  if (ev_which & MU_M1)
	    hctl_rect();
						/* give up screen sink	*/
	  ct_mouse(FALSE);
#if MULTIAPP
	  if (ev_which & MU_MESAG)
	    hctl_mesag(&cmsg[0]);
#endif
	}
}
