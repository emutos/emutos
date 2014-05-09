/*      GEMWMLIB.C      4/23/84 - 07/11/85      Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */
/*      fix wm_delete bug                       10/8/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
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

#include "gempd.h"
#include "gemaplib.h"
#include "gemflag.h"
#include "gemoblib.h"
#include "gemwrect.h"
#include "gemmnlib.h"
#include "geminit.h"
#include "gemgraf.h"
#include "gemfmlib.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemgsxif.h"
#include "gemobjop.h"
#include "gemctrl.h"
#include "gem_rsc.h"
#include "gsx2.h"
#include "rectfunc.h"
#include "optimopt.h"
#include "optimize.h"
#include "gemwmlib.h"
#include "kprint.h"

#include "string.h"
#include "intmath.h"

#define DESKWH  0x0

#define NUM_MWIN NUM_WIN



GLOBAL LONG     gl_newdesk;                     /* current desktop back-*/
                                                /* ground pattern.      */
GLOBAL WORD     gl_newroot;                     /* current object w/in  */
                                                /* gl_newdesk.          */
GLOBAL LONG     desk_tree[NUM_PDS];             /* list of object trees */
                                                /* for the desktop back-*/
                                                /* ground pattern.      */
static WORD     desk_root[NUM_PDS];             /* starting object to   */
                                                /* draw within desk_tree.*/

GLOBAL OBJECT   W_TREE[NUM_MWIN];
GLOBAL OBJECT   W_ACTIVE[NUM_ELEM];


GLOBAL const WORD gl_watype[NUM_ELEM] =
{
        G_IBOX,         /* W_BOX        */
        G_BOX,          /* W_TITLE      */
        G_BOXCHAR,      /* W_CLOSER     */
        G_BOXTEXT,      /* W_NAME       */
        G_BOXCHAR,      /* W_FULLER     */
        G_BOXTEXT,      /* W_INFO       */
        G_IBOX,         /* W_DATA       */
        G_IBOX,         /* W_WORK       */
        G_BOXCHAR,      /* W_SIZER      */
        G_BOX,          /* W_VBAR       */
        G_BOXCHAR,      /* W_UPARROW    */
        G_BOXCHAR,      /* W_DNARROW    */
        G_BOX,          /* W_VSLIDE     */
        G_BOX,          /* W_VELEV      */
        G_BOX,          /* W_HBAR       */
        G_BOXCHAR,      /* W_LFARROW    */
        G_BOXCHAR,      /* W_RTARROW    */
        G_BOX,          /* W_HSLIDE     */
        G_BOX           /* W_HELEV      */
} ;

GLOBAL const LONG gl_waspec[NUM_ELEM] =
{
        0x00011101L,    /* W_BOX        */
        0x00011101L,    /* W_TITLE      */
        0x05011101L,    /* W_CLOSER     */
        0x0L,           /* W_NAME       */
        0x07011101L,    /* W_FULLER     */
        0x0L,           /* W_INFO       */
        0x00001101L,    /* W_DATA       */
        0x00001101L,    /* W_WORK       */
        0x06011101L,    /* W_SIZER      */
        0x00011101L,    /* W_VBAR       */
        0x01011101L,    /* W_UPARROW    */
        0x02011101L,    /* W_DNARROW    */
        0x00011111L,    /* W_VSLIDE     */
        0x00011101L,    /* W_VELEV      */
        0x00011101L,    /* W_HBAR       */
        0x04011101L,    /* W_LFARROW    */
        0x03011101L,    /* W_RTARROW    */
        0x00011111L,    /* W_HSLIDE     */
        0x00011101L     /* W_HELEV      */
} ;

GLOBAL TEDINFO  gl_aname;
GLOBAL TEDINFO  gl_ainfo;

GLOBAL const TEDINFO gl_asamp =
{
        0x0L, 0x0L, 0x0L, IBM, MD_REPLACE, TE_LEFT, SYS_FG, 0x0, 1, 80, 80
};


GLOBAL WORD     wind_msg[8];

GLOBAL WORD     gl_wtop;
GLOBAL LONG     gl_awind;

static LONG     gl_wtree;



void w_nilit(WORD num, OBJECT olist[])
{
        while( num-- )
        {
          olist[num].ob_next = olist[num].ob_head = olist[num].ob_tail = NIL;
        }
}


/*
*       Routine to add a child object to a parent object.  The child
*       is added at the end of the parent's current sibling list.
*       It is also initialized.
*/
static void w_obadd(OBJECT olist[], WORD parent, WORD child)
{
        register WORD   lastkid;

        if ( (parent != NIL) &&
             (child != NIL) )
        {
          olist[child].ob_next = parent;

          lastkid = olist[parent].ob_tail;
          if (lastkid == NIL)
            olist[parent].ob_head = child;
          else
            olist[lastkid].ob_next = child;
          olist[parent].ob_tail = child;
        }
}



static void w_setup(AESPD *ppd, WORD w_handle, WORD kind)
{
        register WINDOW *pwin;

        pwin = &D.w_win[w_handle];
        pwin->w_owner = ppd;
        pwin->w_flags = VF_INUSE;
        pwin->w_kind = kind;
        pwin->w_pname = "";
        pwin->w_pinfo = "";
        pwin->w_hslide = pwin->w_vslide = 0;    /* slider at left/top   */
        pwin->w_hslsiz = pwin->w_vslsiz = -1;   /* use default size     */
}



static GRECT *w_getxptr(WORD which, WORD w_handle)
{
        /* FIXME: Probably remove the GRECT typecasts in this function */

        switch(which)
        {
          case WS_CURR:
          case WS_TRUE:
                return( (GRECT *)&W_TREE[w_handle].ob_x );
          case WS_PREV:
                return( (GRECT *)&D.w_win[w_handle].w_xprev );
          case WS_WORK:
                return( (GRECT *)&D.w_win[w_handle].w_xwork );
          case WS_FULL:
                return( (GRECT *)&D.w_win[w_handle].w_xfull );
        }

        return 0;
}



/* Get the size (x,y,w,h) of the window */
void w_getsize(WORD which, WORD w_handle, GRECT *pt)
{
        rc_copy(w_getxptr(which, w_handle), pt);
        if ( (which == WS_TRUE) && pt->g_w && pt->g_h)
        {
          pt->g_w += 2;
          pt->g_h += 2;
        }
}



static void w_setsize(WORD which, WORD w_handle, GRECT *pt)
{
        rc_copy(pt, w_getxptr(which, w_handle));
}


static void w_adjust( WORD parent, WORD obj, WORD x, WORD y,  WORD w, WORD h)
{
        W_ACTIVE[obj].ob_x = x;
        W_ACTIVE[obj].ob_y = y;
        W_ACTIVE[obj].ob_width = w;
        W_ACTIVE[obj].ob_height = h;

        W_ACTIVE[obj].ob_head = W_ACTIVE[obj].ob_tail = NIL;
        w_obadd(&W_ACTIVE[ROOT], parent, obj);
}


static void w_hvassign(WORD isvert, WORD parent, WORD obj, WORD vx, WORD vy,
                       WORD hx, WORD hy, WORD w, WORD h)
{
        if ( isvert )
          w_adjust(parent, obj, vx, vy, gl_wbox, h);
        else
          w_adjust(parent, obj, hx, hy, w, gl_hbox);
}


/*
*       Walk the list and draw the parts of the window tree owned by
*       this window.
*/

static void do_walk(WORD wh, LONG tree, WORD obj, WORD depth, GRECT *pc)
{
        register ORECT  *po;
        GRECT           t;

        if ( wh == NIL )
          return;
                                                /* clip to screen       */
        if (pc)
          rc_intersect(&gl_rfull, pc);
        else
          pc = &gl_rfull;
                                                /* walk owner rect list */
        for(po=D.w_win[wh].w_rlist; po; po=po->o_link)
        {
          t.g_x = po->o_x;
          t.g_y = po->o_y;
          t.g_w = po->o_w;
          t.g_h = po->o_h;
                                                /* intersect owner rect */
                                                /*   with clip rect's   */
          if ( rc_intersect(pc, &t) )
          {
                                                /*  set clip and draw   */
            gsx_sclip(&t);
            ob_draw(tree, obj, depth);
          }
        }
}


/*
*       Draw the desktop background pattern underneath the current
*       set of windows.
*/

void w_drawdesk(GRECT *pc)
{
        register LONG   tree;
        register WORD   depth;
        register WORD   root;
        GRECT           pt;

        rc_copy(pc, &pt);
        if (gl_newdesk)
        {
          tree = gl_newdesk;
          depth = MAX_DEPTH;
          root = gl_newroot;
        }
        else
        {
          tree = gl_wtree;
          depth = 0;
          root = ROOT;
        }
                                                /* account for drop     */
                                                /*   shadow             */
                                                /* BUGFIX in 2.1        */
        pt.g_w += 2;
        pt.g_h += 2;

        do_walk(DESKWH, tree, root, depth, pc);
}


static void w_cpwalk(WORD wh, WORD obj, WORD depth, WORD usetrue)
{
        GRECT           c;
                                                /* start with window's  */
                                                /*   true size as clip  */
        if ( usetrue )
          w_getsize(WS_TRUE, wh, &c);
        else
        {
                                                /* use global clip      */
          gsx_gclip(&c);
                                                /* add in drop shadow   */
          c.g_w += 2;
          c.g_h += 2;
        }
        w_bldactive(wh);
        do_walk(wh, gl_awind, obj, depth, &c);
}


/*
*       Build an active window and draw the all parts of it but clip
*       these parts with the owner rectangles and the passed in
*       clip rectangle.
*/

static void w_clipdraw(WORD wh, WORD obj, WORD depth, WORD usetrue)
{
        WORD            i;

        if ( (usetrue == 2) ||
             (usetrue == 0) )
        {
          for(i=W_TREE[ROOT].ob_head; i>ROOT; i=W_TREE[i].ob_next)
          {
            if ( (i != wh) &&
                 (D.w_win[i].w_owner == D.w_win[wh].w_owner) &&
                 (D.w_win[i].w_flags & VF_SUBWIN) &&
                 (D.w_win[wh].w_flags & VF_SUBWIN) )
              w_cpwalk(i, obj, depth, TRUE);
          }
        }
                                                /* build active tree    */
        w_cpwalk(wh, obj, depth, usetrue);
}


static void  w_strchg(WORD w_handle, WORD obj, BYTE *pstring)
{
        if ( obj == W_NAME )
          gl_aname.te_ptext = D.w_win[w_handle].w_pname = pstring;
        else
          gl_ainfo.te_ptext = D.w_win[w_handle].w_pinfo = pstring;

        w_clipdraw(w_handle, obj, MAX_DEPTH, 1);
}


static void w_barcalc(WORD isvert, WORD space, WORD sl_value, WORD sl_size,
                      WORD min_sld, GRECT *ptv, GRECT *pth)
{
        if (sl_size == -1)
          sl_size = min_sld;
        else
          sl_size = max(min_sld, mul_div(sl_size, space, 1000) );

        sl_value = mul_div(space - sl_size, sl_value, 1000);
#if 0 /* anemic slidebars */
        if (isvert)
          r_set(ptv, 3, sl_value, gl_wbox-6, sl_size);
        else
          r_set(pth, sl_value, 2, sl_size, gl_hbox-4);
#else
        if (isvert)
          r_set(ptv, 0, sl_value, gl_wbox, sl_size);
        else
          r_set(pth, sl_value, 0, sl_size, gl_hbox);
#endif
}


static void w_bldbar(UWORD kind, WORD istop, WORD w_bar, WORD sl_value,
                     WORD sl_size, WORD x, WORD y, WORD w, WORD h)
{
        WORD            isvert, obj;
        UWORD           upcmp, dncmp, slcmp;
        register WORD   w_up;
        WORD            w_dn, w_slide, space, min_sld;

        isvert = (w_bar == W_VBAR);
        if ( isvert )
        {
          upcmp = UPARROW;
          dncmp = DNARROW;
          slcmp = VSLIDE;
          w_up = W_UPARROW;
          w_dn = W_DNARROW;
          w_slide = W_VSLIDE;
          min_sld = gl_hbox;
        }
        else
        {
          upcmp = LFARROW;
          dncmp = RTARROW;
          slcmp = HSLIDE;
          w_up = W_LFARROW;
          w_dn = W_RTARROW;
          w_slide = W_HSLIDE;
          min_sld = gl_wbox;
        }

        w_hvassign(isvert, W_DATA, w_bar, x, y, x, y, w, h);
        x = y = 0;
        if ( istop )
        {
          if (kind & upcmp)
          {
            w_adjust(w_bar, w_up, x, y, gl_wbox, gl_hbox);
            if ( isvert )
            {
              y += (gl_hbox - 1);
              h -= (gl_hbox - 1);
            }
            else
            {
              x += (gl_wbox - 1);
              w -= (gl_wbox - 1);
            }
          }
          if ( kind & dncmp )
          {
            w -= (gl_wbox - 1);
            h -= (gl_hbox - 1);
            w_hvassign(isvert, w_bar, w_dn, x, y + h - 1,
                        x + w - 1, y, gl_wbox, gl_hbox);
          }
          if ( kind & slcmp )
          {
            w_hvassign(isvert, w_bar, w_slide, x, y, x, y, w, h);
            space = (isvert) ? h : w;

            w_barcalc(isvert, space, sl_value, sl_size, min_sld,
                  (GRECT *)&W_ACTIVE[W_VELEV].ob_x, (GRECT *)&W_ACTIVE[W_HELEV].ob_x);

            obj = (isvert) ? W_VELEV : W_HELEV;
            W_ACTIVE[obj].ob_head = W_ACTIVE[obj].ob_tail = NIL;
            w_obadd(&W_ACTIVE[ROOT], w_slide, obj);
          }
        }
}


static WORD w_top(void)
{
        return( (gl_wtop != NIL) ? gl_wtop : DESKWH );
}


void w_setactive(void)
{
        GRECT           d;
        register WORD   wh;
        AESPD           *ppd;

        wh = w_top();
        w_getsize(WS_WORK, wh, &d);
        ppd = D.w_win[wh].w_owner;
                                                /* BUGFIX 2.1           */
                                                /*  don't chg own if null*/
        if (ppd != NULLPTR)
          ct_chgown(ppd, &d);
}


void w_bldactive(WORD w_handle)
{
        WORD            istop, issub;
        register WORD   kind;
        register WORD   havevbar;
        register WORD   havehbar;
        GRECT           t;
        register WORD   tempw;
        WORD            offx, offy;
        WINDOW          *pw;

        if (w_handle == NIL)
          return;

        pw = &D.w_win[w_handle];
                                                /* set if it is on top  */
        istop = (gl_wtop == w_handle);
                                                /* get the kind of windo*/
        kind = pw->w_kind;
        w_nilit(NUM_ELEM, &W_ACTIVE[0]);
                                                /* start adding pieces  */
                                                /*   & adjusting sizes  */
        gl_aname.te_ptext = pw->w_pname;
        gl_ainfo.te_ptext = pw->w_pinfo;
        gl_aname.te_just = TE_CNTR;
        issub = ( (pw->w_flags & VF_SUBWIN) &&
                  (D.w_win[gl_wtop].w_flags & VF_SUBWIN) );
        w_getsize(WS_CURR, w_handle, &t);
        rc_copy(&t, (GRECT *)&W_ACTIVE[W_BOX].ob_x); /* FIXME: typecast */
        offx = t.g_x;
        offy = t.g_y;
                                                /* do title area        */
        t.g_x = t.g_y = 0;
        if ( kind & (NAME | CLOSER | FULLER) )
        {
          w_adjust(W_BOX, W_TITLE, t.g_x, t.g_y, t.g_w, gl_hbox);
          tempw = t.g_w;
          if ( (kind & CLOSER) &&
               ( istop || issub ) )
          {
            w_adjust(W_TITLE, W_CLOSER, t.g_x, t.g_y, gl_wbox, gl_hbox);
            t.g_x += gl_wbox;
            tempw -= gl_wbox;
          }
          if ( (kind & FULLER) &&
               ( istop || issub ) )
          {
            tempw -= gl_wbox;
            w_adjust(W_TITLE, W_FULLER, t.g_x + tempw, t.g_y,
                        gl_wbox, gl_hbox);
          }
          if ( kind & NAME )
          {
            w_adjust(W_TITLE, W_NAME, t.g_x, t.g_y, tempw, gl_hbox);
            W_ACTIVE[W_NAME].ob_state = (istop || issub) ? NORMAL : DISABLED;

            /* uncomment following line to disable pattern in window title */
            gl_aname.te_color = (istop && (!issub)) ? WTS_FG : WTN_FG;
          }
          t.g_x = 0;
          t.g_y += (gl_hbox - 1);
          t.g_h -= (gl_hbox - 1);
        }
                                                /* do info area         */
        if ( kind & INFO )
        {
          w_adjust(W_BOX, W_INFO, t.g_x, t.g_y, t.g_w, gl_hbox);
          t.g_y += (gl_hbox - 1);
          t.g_h -= (gl_hbox - 1);
        }
                                                /* do data area         */
        w_adjust(W_BOX, W_DATA, t.g_x, t.g_y, t.g_w, t.g_h);
                                                /* do work area         */
        t.g_x++;
        t.g_y++;
        t.g_w -= 2;
        t.g_h -= 2;
        havevbar = kind & (UPARROW | DNARROW | VSLIDE | SIZER);
        havehbar = kind & (LFARROW | RTARROW | HSLIDE | SIZER);
        if ( havevbar )
          t.g_w -= (gl_wbox - 1);
        if ( havehbar )
          t.g_h -= (gl_hbox - 1);

        t.g_x += offx;
        t.g_y += offy;

        t.g_x = t.g_y = 1;
        w_adjust(W_DATA, W_WORK, t.g_x, t.g_y, t.g_w, t.g_h);
                                                /* do vert. area        */
        if ( havevbar )
        {
          t.g_x += t.g_w;
          w_bldbar(kind, istop || issub, W_VBAR, pw->w_vslide,
                        pw->w_vslsiz, t.g_x, 0,
                        t.g_w + 2, t.g_h + 2);
        }
                                                /* do horiz area        */
        if ( havehbar )
        {
          t.g_y += t.g_h;
          w_bldbar(kind, istop || issub, W_HBAR, pw->w_hslide,
                        pw->w_hslsiz, 0, t.g_y,
                        t.g_w + 2, t.g_h + 2);
        }
                                                /* do sizer area        */
        if ( (havevbar) &&
             (havehbar) )
        {
          w_adjust(W_DATA, W_SIZER, t.g_x, t.g_y, gl_wbox, gl_hbox);
          W_ACTIVE[W_SIZER].ob_spec =
                (istop && (kind & SIZER)) ? 0x06011100L: 0x00011100L;
        }
}


void ap_sendmsg(WORD ap_msg[], WORD type, AESPD *towhom,
                WORD w3, WORD w4, WORD w5, WORD w6, WORD w7)
{
        ap_msg[0] = type;
        ap_msg[1] = rlr->p_pid;
        ap_msg[2] = 0;
        ap_msg[3] = w3;
        ap_msg[4] = w4;
        ap_msg[5] = w5;
        ap_msg[6] = w6;
        ap_msg[7] = w7;
        ap_rdwr(MU_SDMSG, towhom, 16, (LONG)&ap_msg[0]);
}



/*
*       Walk down ORECT list and accumulate the union of all the owner
*       rectangles.
*/
static WORD w_union(ORECT *po, GRECT *pt)
{
        if (!po)
          return(FALSE);

        pt->g_x = po->o_x;
        pt->g_y = po->o_y;
        pt->g_w = po->o_w;
        pt->g_h = po->o_h;

        po = po->o_link;
        while (po)
        {
          rc_union((GRECT *)&po->o_x, pt);  /* FIXME: typecast */
          po = po->o_link;
        }
        return(TRUE);
}



static void w_redraw(WORD w_handle, GRECT *pt)
{
        GRECT           t, d;
        AESPD           *ppd;

        ppd = D.w_win[w_handle].w_owner;
                                                /* make sure work rect  */
                                                /*   and word rect      */
                                                /*   intersect          */
        rc_copy(pt, &t);
        w_getsize(WS_WORK, w_handle, &d);
        if ( rc_intersect(&t, &d) )
        {
                                                /* make sure window has */
                                                /*   owns a rectangle   */
          if ( w_union(D.w_win[w_handle].w_rlist, &d) )
          {
                                                /* intersect redraw     */
                                                /*   rect with union    */
                                                /*   of owner rects     */
            if ( rc_intersect(&d, &t) )
              ap_sendmsg(wind_msg, WM_REDRAW, ppd,
                        w_handle, t.g_x, t.g_y, t.g_w, t.g_h);
          }
        }
}


/*
*       Routine to fix rectangles in preparation for a source to
*       destination blt.  If the source is at -1, then the source
*       and destination left fringes need to be realigned.
*/
static WORD w_mvfix(GRECT *ps, GRECT *pd)
{
        register WORD   tmpsx;

        tmpsx = ps->g_x;
        rc_intersect(&gl_rfull, ps);
        if (tmpsx == -1)
        {
          pd->g_x++;
          pd->g_w--;
          return(TRUE);
        }
        return(FALSE);
}


/*
*       Call to move top window.  This involves BLTing the window if
*       none of it that is partially off the screen needs to be redraw,
*       else the whole desktop to just updated.  All uncovered portions
*       of the desktop are redrawn by later by calling w_update.
*/
static WORD w_move(WORD w_handle, WORD *pstop, GRECT *prc)
{
        GRECT           s;                      /* source               */
        GRECT           d;                      /* destination          */
        register GRECT  *pc;
        register WORD   sminus1, dminus1;

        w_getsize(WS_PREV, w_handle, &s);
        s.g_w += 2;
        s.g_h += 2;
        w_getsize(WS_TRUE, w_handle, &d);
                                                /* set flags for when   */
                                                /*   part of the source */
                                                /*   is off the screen  */
        if ( ( (s.g_x + s.g_w > gl_width) && (d.g_x < s.g_x) )  ||
             ( (s.g_y + s.g_h > gl_height) && (d.g_y < s.g_y) )   )
        {
          rc_union(&s, &d);
          *pstop = DESKWH;
        }
        else
        {
          *pstop = w_handle;
        }
                                                /* intersect with full  */
                                                /*   screen and align   */
                                                /*   fringes if -1 xpos */
        sminus1 = w_mvfix(&s, &d);
        dminus1 = w_mvfix(&d, &s);
                                                /* blit what we can     */
        if ( *pstop == w_handle )
        {
          gsx_sclip(&gl_rfull);
          bb_screen(S_ONLY, s.g_x, s.g_y, d.g_x, d.g_y, s.g_w, s.g_h);
                                                /* cleanup left edge    */
          if (sminus1 != dminus1)
          {
            if (dminus1)
              s.g_x--;
            if (sminus1)
            {
              d.g_x--;
              d.g_w = 1;
              gsx_sclip(&d);
              w_clipdraw(gl_wtop, 0, 0, 0);
            }
          }
          pc = &s;
        }
        else
        {
          pc = &d;
        }
                                                /* clean up the rest    */
                                                /*   by returning       */
                                                /*   clip rect          */
        rc_copy(pc, prc);
        return( (*pstop == w_handle) );
}


/*
 * Draw windows from top to bottom.  If top is 0, then start at the topmost
 * window.  If bottom is 0, then start at the bottomost window.  For the
 * first window drawn, just do the insides, since DRAW_CHANGE has already
 * drawn the outside borders.
 */

void w_update(WORD bottom, GRECT *pt, WORD top, WORD moved, WORD usetrue)
{
    WORD   i, ni;
    WORD   done;

    /* limit to screen */
    rc_intersect(&gl_rfull, pt);
    gsx_moff();
    /* update windows from top to bottom */
    if (bottom == DESKWH)
        bottom = W_TREE[ROOT].ob_head;
    /* if there are windows */
    if (bottom != NIL) {
        /* start at the top     */
        if (top == DESKWH)
            top = W_TREE[ROOT].ob_tail;
        /* draw windows from    */
        /*   top to bottom      */
        do {
            if ( !((moved) && (top == gl_wtop)) ) {
                /* set clip and draw a window's border  */
                gsx_sclip(pt);
                /* CHANGED 1/10/86 LKW  */
                /* w_clipdraw(top, 0, MAX_DEPTH, 2); !* from FALSE to 2      *! */
                //w_clipdraw(top, 0, MAX_DEPTH, usetrue);
//                kprintf("=== WIND_OPEN1 \n");
                w_cpwalk(top, 0, MAX_DEPTH, usetrue);   /* let appl. draw inside*/
                w_redraw(top, pt);
//                kprintf("=== WIND_OPEN2 \n");
            }
            /* scan to find prev    */
            i = bottom;
            done = (i == top);
            //kprintf("=== WIND_OPEN i = %x\n", i);
            while (i != top)
            {
                ni = W_TREE[i].ob_next;
                if (ni == top)
                    top = i;
                else
                    i = ni;
            }
        }
        while( !done );
    }
    gsx_mon();
}


static void w_setmen(WORD pid)
{
        WORD            npid;

        npid = menu_tree[pid] ? pid : 0;
        if ( gl_mntree != menu_tree[npid] )
          mn_bar(menu_tree[npid], TRUE, npid);

        npid = desk_tree[pid] ? pid : 0;
        if (gl_newdesk != desk_tree[npid] )
        {
          gl_newdesk = desk_tree[npid];
          gl_newroot = desk_root[npid];
          w_drawdesk(&gl_rscreen);
        }
}


/*
*       Routine to draw menu of top most window as the current menu bar.
*/
static void w_menufix(void)
{
        WORD            pid;

        pid = D.w_win[w_top()].w_owner->p_pid;
        w_setmen(pid);
}


/*
*       Draw the tree of windows given a major change in the some
*       window.  It may have been sized, moved, fulled, topped, or closed.
*       An attempt should be made to minimize the amount of
*       redrawing of other windows that has to occur.  W_REDRAW()
*       will actually issue window redraw requests based on
*       the rectangle that needs to be cleaned up.
*/

static void draw_change(WORD w_handle, GRECT *pt)
{
        GRECT           c, pprev;
        register GRECT  *pw;
        register WORD   start;
        WORD            stop, moved;
        WORD            oldtop, clrold, diffbord, wasclr;

        wasclr = !(D.w_win[w_handle].w_flags & VF_BROKEN);
                                                /* save old size        */
        w_getsize(WS_CURR, w_handle, &c);
        w_setsize(WS_PREV, w_handle, &c);
                                                /* set new size's       */
        w_setsize(WS_CURR, w_handle, pt);
        pw = (GRECT *) w_getxptr(WS_WORK, w_handle);
        wm_calc(WC_WORK, D.w_win[w_handle].w_kind,
                        pt->g_x, pt->g_y, pt->g_w, pt->g_h,
                        &pw->g_x, &pw->g_y, &pw->g_w, &pw->g_h);
                                                /* update rect. lists   */
        everyobj(gl_wtree, ROOT, NIL, (void(*)())newrect, 0, 0, MAX_DEPTH);
                                                /* remember oldtop      */
        oldtop = gl_wtop;
        gl_wtop = W_TREE[ROOT].ob_tail;
                                                /* if new top then      */
                                                /*   change men         */
        if (gl_wtop != oldtop)
          w_menufix();
                                                /* set ctrl rect and    */
                                                /*   mouse owner        */
        w_setactive();
                                                /* init. starting window*/
        start = w_handle;
                                                /* stop at the top      */
        stop = DESKWH;
                                                /* set flag to say we   */
                                                /*   haven't moved      */
                                                /*   the top window     */
        moved = FALSE;
                                                /* if same upper left   */
                                                /*   corner and not     */
                                                /*   zero size window   */
                                                /*   then its a size or */
                                                /*   top request, else  */
                                                /*   its a move, grow,  */
                                                /*   open or close.     */
        if ( (!rc_equal(&gl_rzero, pt)) &&
              (pt->g_x == c.g_x) &&
              (pt->g_y == c.g_y) )
        {
                                                /* size or top request  */
          if ( (pt->g_w == c.g_w) && (pt->g_h == c.g_h) )
          {
                                                /* sizes of prev and    */
                                                /*  current are the same*/
                                                /*  so its a top request*/

                                                /* return if this isn't */
                                                /*   a top request      */
            if ( (w_handle != W_TREE[ROOT].ob_tail) ||
                 (w_handle == oldtop) )

              return;
                                                /* say when borders will*/
                                                /*   change             */
            diffbord = !( (D.w_win[oldtop].w_flags & VF_SUBWIN) &&
                          (D.w_win[gl_wtop].w_flags & VF_SUBWIN) );
                                                /* draw oldtop covered  */
                                                /*   with deactivated   */
                                                /*   borders            */
            if (oldtop != NIL)
            {
              if (diffbord)
                w_clipdraw(oldtop, 0, MAX_DEPTH, 2);
              clrold = !(D.w_win[oldtop].w_flags & VF_BROKEN);
            }
            else
              clrold = TRUE;
                                                /* if oldtop isn't      */
                                                /*   overlapped and new */
                                                /*   top was clear then */
                                                /*   just draw activated*/
                                                /*   borders            */
            if ( (clrold) &&
                 (wasclr) )
            {
              w_clipdraw(gl_wtop, 0, MAX_DEPTH, 1);
              return;
            }
          }
          else
                                                /* size change          */
          {
                                                /* stop before current  */
                                                /*   window if shrink   */
                                                /*   was a pure subset  */
            if ( (pt->g_w <= c.g_w) && (pt->g_h <= c.g_h) )
            {
              stop = w_handle;
              w_clipdraw(gl_wtop, 0, MAX_DEPTH, 2);
              moved = TRUE;
            }
                                                /* start at bottom if   */
                                                /*   a shrink occurred  */
            if ( (pt->g_w < c.g_w) || (pt->g_h < c.g_h) )
              start = DESKWH;
                                                /* update rect. is the  */
                                                /*   union of two sizes */
                                                /*   + the drop shadow  */
            c.g_w = max(pt->g_w, c.g_w) + 2;
            c.g_h = max(pt->g_h, c.g_h) + 2;
          }
        }
        else
        {
                                                /* move or grow or open */
                                                /*   or close           */
          if ( !(c.g_w && c.g_h) ||
                ( (pt->g_x <= c.g_x) &&
                  (pt->g_y <= c.g_y) &&
                  (pt->g_x+pt->g_w >= c.g_x+c.g_w) &&
                  (pt->g_y+pt->g_h >= c.g_y+c.g_h)))
          {
                                                /* a grow that is a     */
                                                /*  superset or an open */
            rc_copy(pt, &c);
          }
          else
          {
                                                /* move, close or shrink*/
                                                /* do a move of top guy */
            if ( (pt->g_w == c.g_w) &&
                 (pt->g_h == c.g_h) &&
                 (gl_wtop == w_handle) )
            {
              moved = w_move(w_handle, &stop, &c);
              start = DESKWH;
            }
                                                /* check for a close    */
            if ( !(pt->g_w && pt->g_h) )
              start = DESKWH;
                                                /* handle other moves   */
                                                /*   and shrinks        */
            if ( start != DESKWH )
            {
              rc_union(pt, &c);
              if ( !rc_equal(pt, &c) )
                start = DESKWH;
            }
          }
        }
                                                /* update gl_wtop       */
                                                /*   after close,       */
                                                /*   or open            */
        if ( oldtop != W_TREE[ROOT].ob_tail )
        {
          if (gl_wtop != NIL)
          {
                                                /* open or close with   */
                                                /*   other windows open */
            w_getsize(WS_CURR, gl_wtop, pt);
            rc_union(pt, &c);
                                                /* if it was an open    */
                                                /*   then draw the      */
                                                /*   old top guy        */
                                                /*   covered            */
            if ( (oldtop != NIL ) &&
                 (oldtop != w_handle) )
            {
                                                /* BUGFIX 2/20/86 LKW   */
                                                /* only an open if prev */
                                                /*  size was zero.      */
              w_getsize(WS_PREV, gl_wtop, &pprev);
              if (rc_equal(&pprev, &gl_rzero))
                w_clipdraw(oldtop, 0, MAX_DEPTH, 2);    /* */
            }
          }
        }
        c.g_w += 2;                             /* account for drop shadow*/
        c.g_h += 2;                             /* BUGFIX in 2.1        */

                                                /* update the desktop   */
                                                /*   background         */
        if (start == DESKWH)
          w_drawdesk(&c);

                                                /* start the redrawing  */
        w_update(start, &c, stop, moved, TRUE);
}


/*
*       Walk down ORECT list looking for the next rect that still has
*       size when clipped with the passed in clip rectangle.
*/
static void w_owns(WORD w_handle, ORECT *po, GRECT *pt, WORD *poutwds)
{
        while (po)
        {
          poutwds[0] = po->o_x;
          poutwds[1] = po->o_y;
          poutwds[2] = po->o_w;
          poutwds[3] = po->o_h;
          D.w_win[w_handle].w_rnext = po = po->o_link;
          /* FIXME: GRECT typecasting again */
          if ( (rc_intersect(pt, (GRECT *)&poutwds[0])) &&
               (rc_intersect(&gl_rfull, (GRECT *)&poutwds[0]))  )
            return;
        }
        poutwds[2] = poutwds[3] = 0;
}




/*
*       Start the window manager up by initializing internal variables.
*/
void wm_start(void)
{
        register WORD   i;
        register ORECT  *po;
        register OBJECT *tree;
        AESPD           *ppd;

#if 0
                                                /* init default owner   */
                                                /*  to be screen mgr.   */
        ppd = fpdnm(NULLPTR, SCR_MGR);
#else
                                                /* init default owner   */
                                                /* to be current process*/
        ppd = rlr;
#endif
                                                /* init owner rects.    */
        or_start();
                                                /* init window extent   */
                                                /*   objects            */
        memset(&W_TREE[ROOT], 0, NUM_MWIN * sizeof(OBJECT));
        w_nilit(NUM_MWIN, &W_TREE[ROOT]);
        for(i=0; i<NUM_MWIN; i++)
        {
          D.w_win[i].w_flags = 0x0;
          D.w_win[i].w_rlist = (ORECT *) 0x0;
          W_TREE[i].ob_type = G_IBOX;
        }
        W_TREE[ROOT].ob_type = G_BOX;
        tree = rs_trees[DESKTOP];
        W_TREE[ROOT].ob_spec = tree->ob_spec;
                                                /* init window element  */
                                                /*   objects            */
        memset(&W_ACTIVE[ROOT], 0, NUM_ELEM * sizeof(OBJECT));
        w_nilit(NUM_ELEM, &W_ACTIVE[0]);
        for(i=0; i<NUM_ELEM; i++)
        {
          W_ACTIVE[i].ob_type = gl_watype[i];
          W_ACTIVE[i].ob_spec = gl_waspec[i];
        }
        W_ACTIVE[ROOT].ob_state = SHADOWED;
                                                /* init rect. list      */
        D.w_win[0].w_rlist = po = get_orect();
        po->o_link = (ORECT *) 0x0;
        po->o_x = XFULL;
        po->o_y = YFULL;
        po->o_w = WFULL;
        po->o_h = HFULL;
        w_setup(ppd, DESKWH, NONE);
        w_setsize(WS_CURR, DESKWH, &gl_rscreen);
        w_setsize(WS_PREV, DESKWH, &gl_rscreen);
        w_setsize(WS_FULL, DESKWH, &gl_rfull);
        w_setsize(WS_WORK, DESKWH, &gl_rfull);
                                                /* init global vars     */
        gl_wtop = NIL;
        gl_wtree = (LONG)&W_TREE[ROOT];
        gl_awind = (LONG)&W_ACTIVE[0];
        gl_newdesk = 0x0L;
                                                /* init tedinfo parts   */
                                                /*   of title and info  */
                                                /*   lines              */
        memcpy(&gl_aname, &gl_asamp, sizeof(TEDINFO));
        memcpy(&gl_ainfo, &gl_asamp, sizeof(TEDINFO));
        gl_aname.te_just = TE_CNTR;
        W_ACTIVE[W_NAME].ob_spec = (LONG)&gl_aname;
        W_ACTIVE[W_INFO].ob_spec = (LONG)&gl_ainfo;
}


/*
*       Allocates a window for the calling application of the appropriate
*       size and returns a window handle.
*
*/

WORD wm_create(WORD kind, GRECT *pt)
{
        register WORD   i;

        for(i=0; i<NUM_WIN && (D.w_win[i].w_flags & VF_INUSE); i++);
        if ( i < NUM_WIN )
        {
          w_setup(rlr, i, kind);
          w_setsize(WS_CURR, i, &gl_rzero);
          w_setsize(WS_PREV, i, &gl_rzero);
          w_setsize(WS_FULL, i, pt);
          return(i);
        }
        return(-1);
}


/*
*       Opens or closes a window.
*/
static void wm_opcl(WORD wh, GRECT *pt, WORD isadd)
{
        GRECT           t;

        rc_copy(pt, &t);
        wm_update(TRUE);
        if (isadd)
        {
          D.w_win[wh].w_flags |= VF_INTREE;
          w_obadd(&W_TREE[ROOT], ROOT, wh);
        }
        else
        {
          ob_delete(gl_wtree, wh);
          D.w_win[wh].w_flags &= ~VF_INTREE;
        }
        draw_change(wh, &t);
        if (isadd)
          w_setsize(WS_PREV, wh, pt);
        wm_update(FALSE);
}


/*
*       Opens a window from a created but closed state.
*/
void wm_open(WORD w_handle, GRECT *pt)
{
        wm_opcl(w_handle, pt, TRUE);
}


/*
*       Closes a window from an open state.
*/

void wm_close(WORD w_handle)
{
        wm_opcl(w_handle, &gl_rzero, FALSE);
}


/*
*       Frees a window and its handle up for use by
*       by another application or by the same application.
*/

void wm_delete(WORD w_handle)
{
        newrect(gl_wtree, w_handle);            /* give back recs.      */
        w_setsize(WS_CURR, w_handle, &gl_rscreen);
        w_setsize(WS_PREV, w_handle, &gl_rscreen);
        w_setsize(WS_FULL, w_handle, &gl_rfull);
        w_setsize(WS_WORK, w_handle, &gl_rfull);
        D.w_win[w_handle].w_flags = 0x0;        /*&= ~VF_INUSE; */
        D.w_win[w_handle].w_owner = NULLPTR;
}


/*
*       Gives information about the current window to the application
*       that owns it.
*/
void wm_get(WORD w_handle, WORD w_field, WORD *poutwds)
{
        register WORD   which;
        GRECT           t;
        register ORECT  *po;

        which = -1;
        switch(w_field)
        {
          case WF_WXYWH:
                which = WS_WORK;
                break;
          case WF_CXYWH:
                which = WS_CURR;
                break;
          case WF_PXYWH:
                which = WS_PREV;
                break;
          case WF_FXYWH:
                which = WS_FULL;
                break;
          case WF_HSLIDE:
                poutwds[0] = D.w_win[w_handle].w_hslide;
                break;
          case WF_VSLIDE:
                poutwds[0] = D.w_win[w_handle].w_vslide;
                break;
          case WF_HSLSIZ:
                poutwds[0] = D.w_win[w_handle].w_hslsiz;
                break;
          case WF_VSLSIZ:
                poutwds[0] = D.w_win[w_handle].w_vslsiz;
                break;
          case WF_TOP:
                poutwds[0] = w_top();
                break;
          case WF_FIRSTXYWH:
          case WF_NEXTXYWH:
                w_getsize(WS_WORK, w_handle, &t);
                po = (w_field == WF_FIRSTXYWH) ? D.w_win[w_handle].w_rlist :
                                                 D.w_win[w_handle].w_rnext;
                w_owns(w_handle, po, &t, &poutwds[0]);
                break;
          case WF_SCREEN:
                gsx_mret((LONG *)&poutwds[0], (LONG *)&poutwds[2]);
                break;
          case WF_TATTRB:
                poutwds[0] = D.w_win[w_handle].w_flags >> 3;
                break;
        }
        if (which != -1)
          w_getsize(which, w_handle, (GRECT *)&poutwds[0]);
}


static WORD wm_gsizes(WORD w_field, WORD *psl, WORD *psz)
{
        if ( (w_field == WF_HSLSIZ) ||
             (w_field == WF_HSLIDE) )
        {
          *psl = W_ACTIVE[W_HELEV].ob_x;
          *psz = W_ACTIVE[W_HELEV].ob_width;
          return(W_HBAR);
        }
        if ( (w_field == WF_VSLSIZ) ||
             (w_field == WF_VSLIDE) )
        {
          *psl = W_ACTIVE[W_VELEV].ob_y;
          *psz = W_ACTIVE[W_VELEV].ob_height;
          return(W_VBAR);
        }
        return(0);
}


/*
*       Routine to top a window and then make the right redraws happen
*/
static void wm_mktop(WORD w_handle)
{
        GRECT           t, p;

        if ( w_handle != gl_wtop )
        {
          ob_order(gl_wtree, w_handle, NIL);
          w_getsize(WS_PREV, w_handle, &p);
          w_getsize(WS_CURR, w_handle, &t);
          draw_change(w_handle, &t);
          w_setsize(WS_PREV, w_handle, &p);
        }
}


/*
*       Allows application to set the attributes of
*       one of the windows that it currently owns.  Some of the
*       information includes the name, and the scroll bar elevator
*       positions.
*/

void wm_set(WORD w_handle, WORD w_field, WORD *pinwds)
{
        WORD            which, liketop, i;
        register WORD   wbar;
        WORD            osl, osz, nsl, nsz;
        GRECT           t;
        WINDOW          *pwin;

        osl = osz = nsl = nsz = 0;
        which = -1;
                                                /* grab the window sync */
        wm_update(TRUE);
        pwin = &D.w_win[w_handle];
        wbar = wm_gsizes(w_field, &osl, &osz);
        if (wbar)
        {
          pinwds[0] = max(-1, pinwds[0]);
          pinwds[0] = min(1000, pinwds[0]);
        }
        liketop = ( ( w_handle == gl_wtop ) ||
                    ( pwin->w_flags & VF_SUBWIN ) );
        switch(w_field)
        {
          case WF_NAME:
                which = W_NAME;
                break;
          case WF_INFO:
                which = W_INFO;
                break;
          case WF_SIZTOP:
                ob_order(gl_wtree, w_handle, NIL);
                                                /* fall thru    */
          case WF_CXYWH:
                draw_change(w_handle, (GRECT *)&pinwds[0]);
                break;
          case WF_TOP:
                if (w_handle != gl_wtop)
                {
                  for(i=W_TREE[ROOT].ob_head; i>ROOT; i=W_TREE[i].ob_next)
                  {
                    if ( (i != w_handle) &&
                         (D.w_win[i].w_owner == rlr) &&
                         (D.w_win[i].w_flags & VF_SUBWIN) &&
                         (pwin->w_flags & VF_SUBWIN) )
                      wm_mktop(i);
                  }
                  wm_mktop(w_handle);
                }
                break;
          case WF_NEWDESK:
                pwin->w_owner = rlr;
                desk_tree[rlr->p_pid] = gl_newdesk = *(LONG *) &pinwds[0];
                desk_root[rlr->p_pid] = gl_newroot = pinwds[2];
                break;
          case WF_HSLSIZ:
                pwin->w_hslsiz = pinwds[0];
                break;
          case WF_VSLSIZ:
                pwin->w_vslsiz = pinwds[0];
                break;
          case WF_HSLIDE:
                pwin->w_hslide = pinwds[0];
                break;
          case WF_VSLIDE:
                pwin->w_vslide = pinwds[0];
                break;
          case WF_TATTRB:
                if (pinwds[0] & WA_SUBWIN)
                  pwin->w_flags |= VF_SUBWIN;
                else
                  pwin->w_flags &= ~VF_SUBWIN;
                if (pinwds[0] & WA_KEEPWIN)
                  pwin->w_flags |= VF_KEEPWIN;
                else
                  pwin->w_flags &= ~VF_KEEPWIN;
                break;
        }
        if ( (wbar) &&
             (liketop) )
        {
          w_bldactive(w_handle);
          wm_gsizes(w_field, &nsl, &nsz);
          if ( (osl != nsl) ||
               (osz != nsz) ||
               (pwin->w_flags & VF_SUBWIN) )
          {
            w_getsize(WS_TRUE, w_handle, &t);
            do_walk(w_handle, gl_awind, wbar + 3, MAX_DEPTH, &t);
          }
        }
        if (which != -1)
          w_strchg(w_handle, which, *(BYTE **)pinwds);
                                                /* give up the sync     */
        wm_update(FALSE);
}


/*
*       Given an x and y location this call will figure out which window
*       the mouse is in.
*/

WORD wm_find(WORD x, WORD y)
{
        return( ob_find(gl_wtree, 0, 2, x, y) );
}


/*
*       Locks or unlocks the current state of the window tree while an
*       application is responding to a window update message in his message
*       pipe or is making some other direct screen update based on his current
*       rectangle list.
*/
void wm_update(WORD beg_update)
{

        if ( beg_update < 2)
        {
          if ( beg_update )
          {
            if ( !tak_flag(&wind_spb) )
                    ev_block(MU_MUTEX, (LONG)&wind_spb);

          }
          else
            unsync(&wind_spb);
        }
        else
        {
          beg_update -= 2;
          fm_own( beg_update );
        }
}


/*
*       Given a width and height of a Work Area and the Kind of window
*       desired calculate the required window size including the
*       Border Area.  or...  Given the width and height of a window
*       including the Border Area and the Kind of window desired, calculate
*       the result size of the window Work Area.
*/
void wm_calc(WORD wtype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
             WORD *px, WORD *py, WORD *pw, WORD *ph)
{
        register WORD   tb, bb, lb, rb;

        tb = bb = rb = lb = 1;

        if ( kind & (NAME | CLOSER | FULLER) )
          tb += (gl_hbox - 1);
        if ( kind & INFO )
          tb += (gl_hbox - 1);

        if ( kind & (UPARROW | DNARROW | VSLIDE | SIZER) )
          rb += (gl_wbox - 1);
        if ( kind & (LFARROW | RTARROW | HSLIDE | SIZER) )
          bb += (gl_hbox - 1);
                                                /* negate values to calc*/
                                                /*   Border Area        */
        if ( wtype == WC_BORDER )
        {
          lb = -lb;
          tb = -tb;
          rb = -rb;
          bb = -bb;
        }
        *px = x + lb;
        *py = y + tb;
        *pw = w - lb - rb;
        *ph = h - tb - bb;
}


/*
 * This function deletes _ALL_ windows and clears all locks that have been
 * done with wind_update()
 */
void wm_new(void)
{
    int wh;

    /* Remove locks: */
    while(ml_ocnt > 0)
        wm_update(2);                   /* END_MCTRL */
    while(wind_spb.sy_tas > 0)
        wm_update(0);                   /* END_UPDATE */

    /* Delete windows: */
    for(wh = 1; wh < NUM_WIN; wh++)
    {
        if(D.w_win[wh].w_flags & VF_INTREE)  wm_close(wh);
        if(D.w_win[wh].w_flags & VF_INUSE)   wm_delete(wh);
    }
}
