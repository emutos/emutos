/*      DESKACT.C       06/11/84 - 09/05/85             Lee Lorenzen    */
/*      merged source   5/18/87 - 5/28/87               mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985 - 1987               Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "taddr.h"
#include "dos.h"
#include "gembind.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "infodef.h"
#include "desktop.h"
#include "deskbind.h"

#include "optimize.h"
#include "optimopt.h"
#include "rectfunc.h"
#include "aesbind.h"
#include "deskact.h"
#include "deskgraf.h"
#include "deskglob.h"
#include "deskmain.h"
#include "desksupp.h"


#define LEN_FNODE 45

#ifndef MAX_OBS
#define MAX_OBS 60
#endif


/* Prototypes: */
WORD act_chkobj(LONG tree, WORD root, WORD obj, WORD mx, WORD my, WORD w, WORD h);




static WORD gr_obfind(LONG tree, WORD root, WORD mx, WORD my)
{
        WORD            sobj;

        sobj = objc_find(tree, root, 2, mx, my);
        if ( (sobj != root) &&
             (sobj != NIL) )
          sobj = act_chkobj(tree, root, sobj, mx, my, 1, 1);
        return(sobj);
}


/*
*       Return TRUE as long as the mouse is down.  Block until the
*       mouse moves into or out of the specified rectangle.
*/
        WORD
gr_isdown(out, x, y, w, h, pmx, pmy, pbutton, pkstate)
        WORD            out, x, y, w, h;
        WORD            *pmx, *pmy, *pbutton, *pkstate;
{

        WORD            flags;
        UWORD           ev_which;
        WORD            kret, bret;

        flags = MU_BUTTON | MU_M1;
        ev_which = evnt_multi(flags, 0x01, 0xff, 0x00, out, x, y, w, h,
                              0, 0, 0, 0, 0, 0x0L, 0x0, 0x0, 
                              pmx, pmy, pbutton, pkstate, &kret, &bret);
        if ( ev_which & MU_BUTTON )
          return(FALSE);
        return(TRUE);
} /* gr_isdown */


        void
gr_accobs(tree, root, pnum, pxypts)
        LONG            tree;
        WORD            root;
        WORD            *pnum;
        WORD            *pxypts;
{
        WORD            i;
        OBJECT          *olist;
        WORD            obj;

        olist = (OBJECT *) LPOINTER(tree);

        i = 0;
        for(obj = olist[root].ob_head; obj > root; obj = olist[obj].ob_next) 
        {
          if (olist[obj].ob_state & SELECTED)
          {
            pxypts[i*2] = olist[root].ob_x + olist[obj].ob_x;
            pxypts[(i*2)+1] = olist[root].ob_y + olist[obj].ob_y;
            i++;
            if (i >= MAX_OBS)
              break;
          }
        }
        *pnum = i;
}


/*
*       Calculate the extent of the list of x,y points given.
*/
/* unused
        void
gr_extent(numpts, xylnpts, pt)
        WORD            numpts;
        WORD            *xylnpts;
        GRECT           *pt;
{
        WORD            i, j;
        WORD            lx, ly, gx, gy;

        lx = ly = 10000;
        gx = gy = 0;
        for (i=0; i<numpts; i++)
        {
          j = i * 2;
          if (xylnpts[j] < lx)
            lx = xylnpts[j];  
          if (xylnpts[j+1] < ly)
            ly = xylnpts[j+1];  
          if (xylnpts[j] > gx)
            gx = xylnpts[j];  
          if (xylnpts[j+1] > gy)
            gy = xylnpts[j+1];  
        }
        r_set(pt, lx, ly, gx-lx+1, gy-ly+1);
}
*/


#if 0
/*
*       Draw a list of polylines a number of times based on a list of
*       x,y object locations that are all relative to a certain x,y
*       offset.
*/
        VOID
gr_plns(x, y, numpts, xylnpts, numobs, xyobpts)
        WORD            x, y;
        WORD            numpts;
        WORD            *xylnpts;
        WORD            numobs;
        WORD            *xyobpts;
{
        WORD            i, j;

        graf_mouse(M_OFF, 0x0L);

        for(i=0; i<numobs; i++)
        {
          j = i * 2;
          gsx_pline(x + xyobpts[j], y + xyobpts[j+1], numpts, xylnpts);
        }
        graf_mouse(M_ON, 0x0L);
}


        WORD
gr_bwait(po, mx, my, numpts, xylnpts, numobs, xyobpts)
        GRECT           *po;
        WORD            mx, my;
        WORD            numpts;
        WORD            *xylnpts;
        WORD            numobs;
        WORD            *xyobpts;
{
        WORD            down;
        WORD            ret[4];
                                                /* draw old             */
        gr_plns(po->g_x, po->g_y, numpts, &xylnpts[0], numobs, &xyobpts[0]);
                                                /* wait for change      */
        down = gr_isdown(TRUE, mx, my, 2, 2, 
                                &ret[0], &ret[1], &ret[2], &ret[3]);
                                                /* erase old            */
        gr_plns(po->g_x, po->g_y, numpts, &xylnpts[0], numobs, &xyobpts[0]);
                                                /* return exit event    */
        return(down);
}
#endif


/*
*       This routine was formerly used to drag a list of polylines.
*       It has been heavily modified since the Copying metaphor has
*       changed: icon outlines are not dragged anymore. Instead, this
*       routine returns the x,y where the mouse button came up, indicating
*       the destination of the copy. numpts, xylnpts, numobs, & xyobpts
*       are no longer used.
*/
/*      VOID
gr_drgplns(in_mx, in_my, pc, numpts, xylnpts, numobs,
                 xyobpts, pdulx, pduly, pdwh, pdobj)
*/
void gr_drgplns(in_mx, in_my, pc, pdulx, pduly, pdwh, pdobj)
        WORD            in_mx, in_my;
        GRECT           *pc;
/*      WORD            numpts;
        WORD            *xylnpts;
        WORD            numobs;
        WORD            *xyobpts;
*/
        WORD            *pdulx, *pduly;
        WORD            *pdwh, *pdobj;
{
        LONG            tree, curr_tree;
        WNODE           *pw;
        WORD            root, state, curr_wh, curr_root, curr_sel, dst_wh;
        WORD            overwhite, l_mx, l_my;
/*      WORD            i, j, offx, offy;*/
        WORD            down, button, keystate, junk, ret[4];
        FNODE           *pf;
/*      GRECT           o, ln;*/
        ANODE           *pa;

        gsx_sclip(&gl_rscreen);
        graf_mouse(4, 0x0L);                    /* flat hand            */
/* BugFix       */
        l_mx = in_mx;
        l_my = in_my;
/*
        gsx_attr(FALSE, MD_XOR, BLACK);
*/
                                                /* figure out extent of */
                                                /*   single polygon     */
/*      gr_extent(numpts, &xylnpts[0], &ln);*/
                                                /* calc overall extent  */
                                                /*   for use as bounds  */
                                                /*   of motion          */
/*      gr_extent(numobs, &xyobpts[0], &o);
        o.g_w += ln.g_w;
        o.g_h += ln.g_h;
*/
/*      for (i = 0; i < numobs; i++)
        {
          j = i * 2;
          xyobpts[j] -= o.g_x;
          xyobpts[j+1] -= o.g_y;
        }
*/
/*      offx = l_mx - o.g_x;
        offy = l_my - o.g_y;
*/
        curr_wh = 0x0;
        curr_tree = 0x0L; 
        curr_root = 0; 
        curr_sel = 0;
        do
        {
/*        o.g_x = l_mx - offx;
          o.g_y = l_my - offy;
          rc_constrain(pc, &o);
          down = gr_bwait(&o, l_mx, l_my,numpts, &xylnpts[0],
                          numobs, &xyobpts[0]);
*/
          down = gr_isdown(TRUE, l_mx, l_my, 2, 2, 
                                &ret[0], &ret[1], &ret[2], &ret[3]);
          graf_mkstate(&l_mx, &l_my, &button, &keystate);
          dst_wh = wind_find(l_mx, l_my);
          tree = G.a_screen;
          root = DROOT;
          pw = win_find(dst_wh);
          if (pw)
          {
            tree = G.a_screen;
            root = pw->w_root;
          }
          else
            dst_wh = 0;
          *pdobj = gr_obfind(tree, root, l_mx, l_my);
          overwhite = (*pdobj == root) || (*pdobj == NIL);
          if ( (overwhite) || ((!overwhite) && (*pdobj != curr_sel)) )
          {
            if (curr_sel)
            {
              act_chg(curr_wh, curr_tree, curr_root, curr_sel, pc,
                        SELECTED, FALSE, TRUE, TRUE);
              curr_wh = 0x0;
              curr_tree = 0x0L;
              curr_root = 0x0;
              curr_sel = 0;
            }
            if (!overwhite)
            {
              state = LWGET(OB_STATE(*pdobj));
              if ( !(state & SELECTED) )
              {
                pa = i_find(dst_wh, *pdobj, &pf, &junk);
                if ( ((pa->a_type == AT_ISFOLD) ||
                      (pa->a_type == AT_ISDISK)) &&
                     !(pf->f_attr & F_FAKE) )
                {
                  curr_wh = dst_wh;
                  curr_tree = tree;
                  curr_root = root;
                  curr_sel = *pdobj;
                  act_chg(curr_wh, curr_tree, curr_root, curr_sel, pc,
                         SELECTED, TRUE, TRUE, TRUE);
                } /* if */
              } /* if !SELECTED */
            } /* if !overwhite */
          } /* if */
        } while (down);
        if (curr_sel)
            act_chg(curr_wh, curr_tree, curr_root, curr_sel, pc,
                        SELECTED, FALSE, TRUE, TRUE);
        *pdulx = l_mx;                          /* pass back dest. x,y  */
        *pduly = l_my;
        *pdwh = dst_wh;
        graf_mouse(ARROW, 0x0L);
} /* gr_drgplns */


/*
*       See if the bit at x,y in a rater form is on or off
*/
static WORD bit_on(WORD x, WORD y, LONG praster, WORD bwidth)
{
        WORD            windex;
        UWORD           tmpw;

        windex = (bwidth * y / 2) + (x / 16);
        tmpw = LWGET(praster + (windex * 2));
        tmpw = (tmpw >> (15 - (x % 16)) ) & 0x0001;
        return(tmpw);
} /* bit_on */


/*
*       Check to see which part of the object that the mouse has
*       been clicked over.  If the type of object is an icon, then use
*       the icon mask to determine if the icon was actually selected.
*       If the current view is by text strings then use the name
*       portion of the text string.
*/
WORD act_chkobj(LONG tree, WORD root, WORD obj, WORD mx, WORD my, WORD w, WORD h)
{
        OBJECT          *olist;
        ICONBLK         *ib;
        WORD            view, ox, oy;
        GRECT           t, m;

        olist = (OBJECT *) LPOINTER(tree);

        ox = olist[root].ob_x + olist[obj].ob_x;
        oy = olist[root].ob_y + olist[obj].ob_y;

        view = (root == DROOT) ? V_ICON : G.g_iview;
        switch( view )
        {
          case V_TEXT:
                r_set(&t, ox, oy, LEN_FNODE * gl_wchar, gl_hchar);
                r_set(&m, mx, my, w, h);
                if ( !rc_intersect(&t, &m) )
                  return(root);
                break;
          case V_ICON:
                r_set(&t, mx - ox, my - oy, w, h);
                r_set(&m, mx - ox, my - oy, w, h);
                ib = (ICONBLK *) &G.g_idlist[G.g_index[obj]];
                if ( !rc_intersect((GRECT *)&ib->ib_xtext, &t) )
                {
                  if ( !rc_intersect((GRECT *)&ib->ib_xicon, &m) )
                    return(root);
                  else
                  {
                    if ( !bit_on(m.g_x - ib->ib_xicon + (w / 2), 
                        m.g_y - ib->ib_yicon + (h / 2),
                        ib->ib_pmask, ib->ib_wicon/8) )
                    return(root);
                  }
                }
                break;
        }

        return(obj);
}


/*
*       Change a single objects state.
*
*       LONG            tree            * tree that holds item
*       WORD            obj             * object to affect    
*       UWORD           chgvalue        * bit value to change 
*       WORD            dochg           * set or reset value  
*       WORD            dodraw          * draw resulting chang
*       WORD            chkdisabled     * only if item enabled
*/
WORD act_chg(WORD wh, LONG tree, WORD root, WORD obj, GRECT *pc, UWORD chgvalue,
             WORD dochg, WORD dodraw, WORD chkdisabled)
{
        UWORD           curr_state;
        UWORD           old_state;
        OBJECT          *olist;
        GRECT           t;

        olist = (OBJECT *) LPOINTER(tree);
        old_state = curr_state = olist[obj].ob_state;
        if ( (chkdisabled) &&
             (curr_state & DISABLED) )
          return(FALSE);
                                                /* get object's extent  */
        rc_copy((GRECT *)&olist[obj].ob_x, &t);
        t.g_x += olist[root].ob_x;
        t.g_y += olist[root].ob_y;
                                                /* make change          */
        if ( dochg )
          curr_state |= chgvalue;
        else
          curr_state &= ~chgvalue;
                                                /* get it updated on    */
                                                /*   screen             */
        if ( old_state != curr_state )
        {
                                                /* change it without    */
                                                /*   drawing            */
          objc_change(tree, obj, 0, pc->g_x, pc->g_y, pc->g_w, pc->g_h,
                         curr_state, FALSE);
                                                /* clip to uncovered    */
                                                /*   portion desktop or */
                                                /*   window and the     */
                                                /*   object's extent    */
          if ( (dodraw) &&
               ( rc_intersect(pc, &t) ) )
          {
            do_wredraw(wh, t.g_x, t.g_y, t.g_w, t.g_h);
          }
        }
        return(TRUE);
}


/*
*       Change state of all objects partially intersecting the given rectangle
*       but allow one object to be excluded.
*/
void act_allchg(WORD wh, LONG tree, WORD root, WORD ex_obj, GRECT *pt, GRECT *pc, 
                WORD chgvalue, WORD dochg, WORD dodraw)
{
        WORD            obj, newstate;
        OBJECT          *olist;
        WORD            offx, offy;
        GRECT           o, a, w;

        olist = (OBJECT *) LPOINTER(tree);
        offx = olist[root].ob_x;
        offy = olist[root].ob_y;
                                                /* accumulate extent of */
                                                /*   change in this     */
                                                /*   rectangle          */
        a.g_w = a.g_h = 0;
/* BUGFIX 7/14/86 LKW   */
        rc_copy(pt, &w);
        rc_intersect(pc, &w);                   /* limit selection to   */
                                                /*    work area of window*/
/* */

        for(obj = olist[root].ob_head; obj > root; obj = olist[obj].ob_next) 
        {
          if (obj != ex_obj)
          {
            rc_copy((GRECT *)&olist[obj].ob_x, &o);
            o.g_x += offx;
/* BUGFIX 7/14/86 LKW   */
            o.g_y = o.g_y + offy + 1;
            o.g_h -= 1;
            if ( ( rc_intersect(&w, &o) ) &&
/* */
                 ( root != act_chkobj(tree,root,obj,o.g_x,o.g_y,o.g_w,o.g_h)))
            {
                                                /* make change          */
              newstate = olist[obj].ob_state;
              if ( dochg )
                newstate |= chgvalue;
              else
                newstate &= ~chgvalue;
              if (newstate != olist[obj].ob_state)
              {
                olist[obj].ob_state= newstate;
                rc_copy((GRECT *)&olist[obj].ob_x, &o);
                o.g_x += offx;
                o.g_y += offy;
                if (a.g_w)
                  rc_union(&o, &a);
                else
                  rc_copy(&o, &a);
              }
            }
          }
        }
        if ( ( dodraw ) &&
             ( rc_intersect(pc, &a) ) )
        {
          do_wredraw(wh, a.g_x, a.g_y, a.g_w, a.g_h);
        }
}


/*
*       Single click action on the specified tree of objects.
*/
void act_bsclick(WORD wh, LONG tree, WORD root, WORD mx, WORD my, WORD keystate,
                 GRECT *pc, WORD dclick)
{
        WORD            obj;
        WORD            shifted;
        WORD            state;
        OBJECT          *olist;

        shifted = (keystate & K_LSHIFT) || (keystate & K_RSHIFT);
        obj = gr_obfind(tree, root, mx, my);

        if ( (obj == root) ||
             (obj == NIL)  )
        {
          act_allchg(wh, tree, root, obj, &gl_rfull, pc,
                        SELECTED, FALSE, TRUE);
        }
        else
        {
          olist = (OBJECT *) LPOINTER(tree);
          state = olist[obj].ob_state;
          if ( !shifted )
          {
/* BugFix       */
            if ( dclick || !(state & SELECTED) )
            {
              act_allchg(wh, tree, root, obj, &gl_rfull, pc,
                         SELECTED, FALSE, TRUE);
              state |= SELECTED;
            }
/* */
          }
          else
          {
            if (state & SELECTED)
              state &= ~SELECTED;
            else
              state |= SELECTED;
          }  
          act_chg(wh, tree, root, obj, pc, SELECTED, 
                        state & SELECTED, TRUE, TRUE);
        }
}


/*
*       Button stayed down over the specified tree of objects.
*/
WORD act_bdown(WORD wh, LONG tree, WORD root, WORD *in_mx, WORD *in_my,
               WORD keystate, GRECT *pc, WORD *pdobj)
{
        WORD            sobj;
        WORD            numobs, button;
        OBJECT          *olist;
        WORD            dst_wh;
        WORD            l_mx, l_my, dulx, duly;
        WORD            numpts, *pxypts, view;
        GRECT           m;

        dst_wh = NIL;
        *pdobj = root;
        l_mx = *in_mx;
        l_my = *in_my;
        sobj = gr_obfind(tree, root, l_mx, l_my);
                        /* rubber box to enclose a group of icons       */
        if ( (sobj == root) || (sobj == NIL) )
        {
          r_set(&m, l_mx, l_my, 6, 6);
          graf_rubbox(m.g_x, m.g_y, 6, 6, &m.g_w, &m.g_h);
          act_allchg(wh, tree, root, sobj, &m, pc, SELECTED, TRUE, TRUE);
        } /* if */
        else
        {                                       /* drag icon(s)         */
          olist = (OBJECT *) LPOINTER(tree);
          if (olist[sobj].ob_state & SELECTED)
          {
            gr_accobs(tree, root, &numobs, &G.g_xyobpts[0]);
            if (numobs)
            {
              view = (root == DROOT) ? V_ICON : G.g_iview;
              if (view == V_ICON)
              {
                numpts = G.g_nmicon;
                pxypts = &G.g_xyicon[0];
              }
              else
              {
                numpts = G.g_nmtext;
                pxypts = &G.g_xytext[0];
              }
              gr_drgplns(l_mx, l_my, &gl_rfull, &dulx, &duly, &dst_wh, pdobj);
              if (dst_wh) 
              {
                                                /* cancel drag if it    */
                                                /*   ends up in same    */
                                                /*   window over itself */
                if ( (wh == dst_wh) && (*pdobj == sobj) )
                  dst_wh = NIL;
              } /* if (dst_wh */
              else
                dst_wh = NIL;
            } /* if numobs */
          } /* if SELECTED */
        } /* else */
        evnt_button(0x01, 0x01, 0x00, &l_mx, &l_my, &button, &keystate);
        *in_mx = dulx;                  /* pass back the dest. x,y      */
        *in_my = duly;
        return(dst_wh);
} /* act_bdown */

