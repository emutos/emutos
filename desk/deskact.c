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
#include "desk_rsc.h"
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
#include "desk1.h"


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
static WORD gr_isdown(WORD out, WORD x, WORD y, WORD w, WORD h,
                      WORD *pmx, WORD *pmy, WORD *pbutton, WORD *pkstate)
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


static void gr_accobs(LONG tree, WORD root, WORD *pnum, WORD *pxypts)
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



#ifdef DESK1

static void move_drvicon(LONG tree, WORD root, WORD x, WORD y, WORD *pts)
{
        ANODE *an_disk;
        WORD objcnt;
        OBJECT *tr;
        WORD obj;
        WORD oldx;
        WORD oldy;
        
        objcnt = 0;
        tr = (OBJECT *)tree;
        for (obj = tr[root].ob_head; obj > root; obj = tr[obj].ob_next)
        {
                if (tr[obj].ob_state & SELECTED)
                {
                        oldx = tr[obj].ob_x;
                        oldy = tr[obj].ob_y;

                        snap_disk(x + pts[2 * objcnt], y + pts[2 * objcnt + 1],
                                                &tr[obj].ob_x, &tr[obj].ob_y);

                        for (an_disk = G.g_ahead; an_disk; an_disk = an_disk->a_next)
                        {
                                if (an_disk->a_obid == obj)
                                {
                                        an_disk->a_xspot = tr[obj].ob_x;
                                        an_disk->a_yspot = tr[obj].ob_y;
                                }
                        }
                        do_wredraw(0, oldx, oldy, G.g_wicon, G.g_hicon);
                        do_wredraw(0, tr[obj].ob_x, tr[obj].ob_y, G.g_wicon, G.g_hicon);
                        ++objcnt;
                }
        }
        
}


/*
*       Calculate the extent of the list of x,y points given.
*/
static void gr_extent(WORD numpts, WORD *xylnpts, GRECT *pt)
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


/*
*       Draw a list of polylines a number of times based on a list of
*       x,y object locations that are all relative to a certain x,y
*       offset.
*/
static void gr_plns(WORD x, WORD y, WORD numpts, WORD *xylnpts, WORD numobs,
                    WORD *xyobpts)
{
        WORD    i, j;

        graf_mouse(M_OFF, 0x0L);

        for(i=0; i<numobs; i++)
        {
          j = i * 2;
          gsx_pline(x + xyobpts[j], y + xyobpts[j+1], numpts, xylnpts);
        }
        graf_mouse(M_ON, 0x0L);
}


static WORD gr_bwait(GRECT *po, WORD mx, WORD my, WORD numpts, WORD *xylnpts,
                     WORD numobs, WORD *xyobpts)
{
        WORD    down;
        WORD    ret[4];

        /* Since the desktop and the AES currently share the same VDI work-
         * station, we have to reset the clipping and drawing mode here */
        gsx_sclip(&gl_rscreen);
        gsx_attr(FALSE, MD_XOR, BLACK);
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


static void gr_obalign(WORD numobs, WORD x, WORD y, WORD *xyobpts)
{
        WORD i, j;
        
        for (i = 0; i < numobs; i++)
        {
          j = i * 2;
          xyobpts[j]   -= x;
          xyobpts[j+1] -= y;
        }
}

#endif



#ifdef DESK1
/*
*       This routine is used to drag a list of polylines.
*/
void gr_drgplns(WORD in_mx, WORD in_my, GRECT *pc, WORD numpts, 
                   WORD *xylnpts, WORD numobs, WORD *xyobpts, 
                   WORD *pdulx, WORD *pduly, WORD *pdwh, WORD *pdobj)
{
        LONG    tree, curr_tree;
        WNODE   *pw;
        WORD    root, state, curr_wh, curr_root, curr_sel, dst_wh;
        WORD    /* overwhite,*/ l_mx, l_my;
        WORD    offx, offy;
        WORD    down, button, keystate, junk;
        FNODE   *pf;
        GRECT   o, ln;
        ANODE   *pa;

        graf_mouse(4, 0x0L);                    /* flat hand            */

        l_mx = in_mx;
        l_my = in_my;
                                                /* figure out extent of */
                                                /*   single polygon     */
        gr_extent(numpts, &xylnpts[0], &ln);
                                                /* calc overall extent  */
                                                /*   for use as bounds  */
                                                /*   of motion          */
        gr_extent(numobs, &xyobpts[0], &o);
        o.g_w += ln.g_w;
        o.g_h += ln.g_h;

        gr_obalign(numobs, o.g_x, o.g_y, &xyobpts[0]);

        offx = l_mx - o.g_x;
        offy = l_my - o.g_y;

        curr_wh = 0x0;
        curr_tree = 0x0L; 
        curr_root = 0; 
        curr_sel = 0;

        do
        {
          o.g_x = l_mx - offx;
          o.g_y = l_my - offy;
          rc_constrain(pc, &o);
          down = gr_bwait(&o, l_mx, l_my,numpts, &xylnpts[0],
                          numobs, &xyobpts[0]);

          graf_mkstate(&l_mx, &l_my, &button, &keystate);
          dst_wh = wind_find(l_mx, l_my);
          tree = G.a_screen;
          root = DROOT;
          if (dst_wh)
          {
                pw = win_find(dst_wh);
                if (pw)
                {
                        tree = G.a_screen;
                        root = pw->w_root;
                }
                else dst_wh = 0;        
          }
                  
          *pdobj = gr_obfind(tree, root, l_mx, l_my);
/*        overwhite = (*pdobj == root) || (*pdobj == NIL);
          if ( (overwhite) || ((!overwhite) && (*pdobj != curr_sel)) )*/

/* The DESKTOP v1.2 test is rather less efficient here, but it is 
 * the same test. */
          if ((*pdobj == root) || (*pdobj == NIL))
          {
            if (curr_sel)
            {
              act_chg(curr_wh, curr_tree, curr_root, curr_sel, pc,
                        SELECTED, FALSE, TRUE, TRUE);
              curr_wh = 0x0;
              curr_tree = 0x0L;
              curr_root = 0x0;
              curr_sel = 0;
                  continue;
            }
          }
          
          if (*pdobj != curr_sel)
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
          }
          state = LWGET(OB_STATE(*pdobj)); /*state = tree[*pdobj].ob_state;*/
          if ( !(state & SELECTED) )
          {
                pa = i_find(dst_wh, *pdobj, &pf, &junk);
                if ((pa->a_type == AT_ISFOLD) ||
                    (pa->a_type == AT_ISDISK) ||
                    (pa->a_type == AT_ISTRSH))
                {
                  curr_wh = dst_wh;
                  curr_tree = tree;
                  curr_root = root;
                  curr_sel = *pdobj;
                  act_chg(curr_wh, curr_tree, curr_root, curr_sel, pc,
                          SELECTED, TRUE, TRUE, TRUE);
                } /* if */
              } /* if !SELECTED */
        } while (down);
        if (curr_sel)
            act_chg(curr_wh, curr_tree, curr_root, curr_sel, pc,
                        SELECTED, FALSE, TRUE, TRUE);
        *pdulx = l_mx;                          /* pass back dest. x,y  */
        *pduly = l_my;
        *pdwh = dst_wh;
        graf_mouse(ARROW, 0x0L);
} /* gr_drgplns */

#else /* DESK1 */

/*
*       This routine was formerly used to drag a list of polylines.
*       It has been heavily modified since the Copying metaphor has
*       changed: icon outlines are not dragged anymore. Instead, this
*       routine returns the x,y where the mouse button came up, indicating
*       the destination of the copy. numpts, xylnpts, numobs, & xyobpts
*       are no longer used.
*/
void gr_drgplns(WORD in_mx, WORD in_my, GRECT *pc, WORD *pdulx, WORD *pduly,
                WORD *pdwh, WORD *pdobj)
{
        LONG    tree, curr_tree;
        WNODE   *pw;
        WORD    root, state, curr_wh, curr_root, curr_sel, dst_wh;
        WORD    overwhite, l_mx, l_my;
        WORD    down, button, keystate, junk, ret[4];
        FNODE   *pf;
        ANODE   *pa;

        gsx_sclip(&gl_rscreen);
        graf_mouse(4, 0x0L);                    /* flat hand            */

        l_mx = in_mx;
        l_my = in_my;

        curr_wh = 0x0;
        curr_tree = 0x0L; 
        curr_root = 0; 
        curr_sel = 0;

        do
        {
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
#endif


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
#ifdef DESK1
              gr_drgplns(l_mx, l_my, &gl_rfull, numpts, pxypts, numobs,
                         G.g_xyobpts, &dulx, &duly, &dst_wh, pdobj);

#else
              gr_drgplns(l_mx, l_my, &gl_rfull, &dulx, &duly, &dst_wh, pdobj);
#endif
              if (dst_wh) 
              {
                                                /* cancel drag if it    */
                                                /*   ends up in same    */
                                                /*   window over itself */
                if ( (wh == dst_wh) && (*pdobj == sobj) )
                  dst_wh = NIL;
              } /* if (dst_wh */
              else
              {
#ifdef DESK1
                if (wh == 0 && (*pdobj == root)) // Dragging from desktop
                {
                  move_drvicon(tree, root, dulx, duly, G.g_xyobpts);
                  dst_wh = NIL;
                }
#else
                dst_wh = NIL;
#endif
              }
            } /* if numobs */
          } /* if SELECTED */
        } /* else */
        evnt_button(0x01, 0x01, 0x00, &l_mx, &l_my, &button, &keystate);
        *in_mx = dulx;                  /* pass back the dest. x,y      */
        *in_my = duly;
        return(dst_wh);
} /* act_bdown */

