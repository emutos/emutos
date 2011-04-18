/*      GEMOBLIB.C      03/15/84 - 05/27/85     Gregg Morris            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */ 

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

#include "portab.h"
#include "machine.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "taddr.h"
#include "gemlib.h"
#include "funcdef.h"

#include "geminit.h"
#include "gemgsxif.h"
#include "gemobjop.h"
#include "gemgraf.h"
#include "optimopt.h"
#include "rectfunc.h"

#include "string.h"
#include "kprint.h"     // just for debugging

                                                /* in GSXBIND.C         */
#define g_vsf_color( x )          gsx_1code(S_FILL_COLOR, x)


GLOBAL LONG     ad_tmpstr;
GLOBAL LONG     ad_rawstr;
GLOBAL LONG     ad_fmtstr;
GLOBAL LONG     ad_edblk;
GLOBAL LONG     ad_bi;
GLOBAL LONG     ad_ib;

GLOBAL TEDINFO  edblk;
GLOBAL BITBLK   bi;
GLOBAL ICONBLK  ib;



/*
*       Routine to find the x,y offset of a particular object relative
*       to the physical screen.  This involves accumulating the offsets
*       of all the objects parents up to and including the root.
*/
void ob_offset(LONG tree, WORD obj, WORD *pxoff, WORD *pyoff)
{
        WORD            junk;
        LONG            px;

        *pxoff = *pyoff = 0;
        do
        {
                                                /* have our parent--    */
                                                /*  add in his x, y     */
          *pxoff += LWGET(px = OB_X(obj));
          *pyoff += LWGET(px + 0x02L);
          obj = get_par(tree, obj, &junk);
        } while ( obj != NIL );
}


/************************************************************************/
/* o b _ r e l x y w h                                                  */
/************************************************************************/
void ob_relxywh(LONG tree, WORD obj, GRECT *pt)
{
        LWCOPY(ADDR(pt), OB_X(obj), sizeof(GRECT)/2);
} /* ob_relxywh */


/************************************************************************/
/* o b _ a c t x y w h                                                  */
/************************************************************************/
void ob_actxywh(LONG tree, WORD obj, GRECT *pt)
{
        LONG            pw;

        ob_offset(tree, obj, &pt->g_x, &pt->g_y);
        pt->g_w = LWGET(pw = OB_WIDTH(obj));    
        pt->g_h = LWGET(pw + 0x02L);    
} /* ob_actxywh */



void ob_setxywh(LONG tree, WORD obj, GRECT *pt)
{
        LWCOPY(OB_X(obj), ADDR(pt), sizeof(GRECT)/2);
}



/*
*       Routine to take an unformatted raw string and based on a
*       template string build a formatted string.
*/
void ob_format(WORD just, BYTE *raw_str, BYTE *tmpl_str, BYTE *fmt_str)
{
        register BYTE   *pfbeg, *ptbeg, *prbeg;
        BYTE                    *ptend, *prend;
        register WORD   inc, ptlen, prlen;

        if (*raw_str == '@')
          *raw_str = NULL;

        pfbeg = fmt_str;
        ptbeg = tmpl_str;
        prbeg = raw_str;

        ptlen = strlen(tmpl_str);
        prlen = strlen(raw_str);

        inc = 1;
        pfbeg[ptlen] = NULL;
        if (just == TE_RIGHT)
        {
          inc = -1;
          pfbeg = pfbeg + ptlen - 1;
          ptbeg = ptbeg + ptlen - 1;
          prbeg = prbeg + prlen - 1;
        }

        ptend = ptbeg + (inc * ptlen);
        prend = prbeg + (inc * prlen);

        while( ptbeg != ptend )
        {
          if ( *ptbeg != '_')
            *pfbeg = *ptbeg;
          else
          {
            if ( prbeg != prend )
            {
              *pfbeg = *prbeg;
              prbeg += inc;
            }
            else
              *pfbeg = '_';
          } /* else */
          pfbeg += inc;
          ptbeg += inc;
        } /* while */
} /* ob_format */



#if MC68K
static WORD far_call(WORD (*fcode)(), LONG fdata)
{
        return (*fcode)(fdata);
}
#endif


/*
*       Routine to load up and call a user defined object draw or change 
*       routine.
*/

WORD ob_user(LONG tree, WORD obj, GRECT *pt, LONG spec, WORD curr_state, WORD new_state)
{
        PARMBLK         pb;

        pb.pb_tree = tree;
        pb.pb_obj = obj;
        pb.pb_prevstate = curr_state;
        pb.pb_currstate = new_state;
        rc_copy(pt, (GRECT *)&pb.pb_x);  /* FIXME: Ugly typecasting */
        gsx_gclip((GRECT *)&pb.pb_xc);   /* FIXME: dito */
        pb.pb_parm = LLGET(spec+4);
        return(  far_call( (WORD(*)())LLGET(spec), ADDR(&pb)) );
}



/*
*       Routine to draw an object from an object tree.
*/
void  just_draw(LONG tree, WORD obj, WORD sx, WORD sy)
{
        WORD            bcol, tcol, ipat, icol, tmode, th;
        WORD            state, obtype, len, flags;
        LONG            spec;
        WORD            tmpx, tmpy, tmpth;
        BYTE            ch;
        GRECT           t, c;

        ch = ob_sst(tree, obj, &spec, &state, &obtype, &flags, &t, &th);

        if ( (flags & HIDETREE) ||
             (spec == -1L) )
          return;

        t.g_x = sx;
        t.g_y = sy;
                                                /* do trivial reject    */
                                                /*  with full extent    */
                                                /*  including, outline, */
                                                /*  shadow, & thickness */
        if (gl_wclip && gl_hclip)
        {
          rc_copy(&t, &c);
          if (state & OUTLINED)
            gr_inside(&c, -3);
          else
            gr_inside(&c, ((th < 0) ? (3 * th) : (-3 * th)) );
        
          if ( !(gsx_chkclip(&c)) )
            return;
        }
                                                /* for all tedinfo      */
                                                /*   types get copy of  */
                                                /*   ted and crack the  */
                                                /*   color word and set */
                                                /*   the text color     */
        if ( obtype != G_STRING )
        {
          tmpth = (th < 0) ? 0 : th;
          tmode = MD_REPLACE;
          tcol = BLACK;
          switch( obtype )
          {
            case G_BOXTEXT:
            case G_FBOXTEXT:
            case G_TEXT:
            case G_FTEXT:
                  LBCOPY(ad_edblk, spec, sizeof(TEDINFO));
                  gr_crack(edblk.te_color, &bcol,&tcol, &ipat, &icol, &tmode);
                break;
          }
                                                /* for all box types    */
                                                /*   crack the color    */
                                                /*   if not ted and     */
                                                /*   draw the box with  */
                                                /*   border             */
          switch( obtype )
          {
            case G_BOX:
            case G_BOXCHAR:
            case G_IBOX:
                gr_crack(LLOWD(spec), &bcol, &tcol, &ipat, &icol, &tmode);
            case G_BUTTON:
                if (obtype == G_BUTTON)
                {
                  bcol = BLACK;
                  ipat = IP_HOLLOW;
                  icol = WHITE;
                }
            case G_BOXTEXT:
            case G_FBOXTEXT:
                                                /* draw box's border    */
                if ( th != 0 )
                {
                  gsx_attr(FALSE, MD_REPLACE, bcol);
                  gr_box(t.g_x, t.g_y, t.g_w, t.g_h, th);
                }
                                                /* draw filled box      */
                if (obtype != G_IBOX)
                {
                  gr_inside(&t, tmpth);
                  gr_rect(icol, ipat, &t); 
                  gr_inside(&t, -tmpth);
                }
                break;
          }
          gsx_attr(TRUE, tmode, tcol);
                                                /* do whats left for    */
                                                /*   all the other types*/
          switch( obtype )
          {
            case G_FTEXT:
            case G_FBOXTEXT:
                strcpy(D.g_rawstr, (char *) edblk.te_ptext);
                strcpy(D.g_tmpstr, (char *) edblk.te_ptmplt);
                ob_format(edblk.te_just, &D.g_rawstr[0], &D.g_tmpstr[0], 
                        &D.g_fmtstr[0]);
                                                /* fall thru to gr_gtext*/
            case G_BOXCHAR:
                edblk.te_ptext = ad_fmtstr;
                if (obtype == G_BOXCHAR)
                {
                  D.g_fmtstr[0] = ch;
                  D.g_fmtstr[1] = NULL;
                  edblk.te_just = TE_CNTR;
                  edblk.te_font = IBM;
                }
                                                /* fall thru to gr_gtext*/
            case G_TEXT:
            case G_BOXTEXT:
                gr_inside(&t, tmpth);
                gr_gtext(edblk.te_just, edblk.te_font, edblk.te_ptext, 
                                &t);
                gr_inside(&t, -tmpth);
                break;
            case G_IMAGE:
                LBCOPY(ad_bi, spec, sizeof(BITBLK));
                gsx_blt(bi.bi_pdata, bi.bi_x, bi.bi_y, bi.bi_wb,
                                0x0L, t.g_x, t.g_y, gl_width/8, bi.bi_wb * 8,
                                bi.bi_hl, MD_TRANS, bi.bi_color, WHITE);
                break;
            case G_ICON:
                LBCOPY(ad_ib, spec, sizeof(ICONBLK));
                ib.ib_xicon += t.g_x;
                ib.ib_yicon += t.g_y; 
                ib.ib_xtext += t.g_x;
                ib.ib_ytext += t.g_y; 
                gr_gicon(state, ib.ib_pmask, ib.ib_pdata, ib.ib_ptext,
                  ib.ib_char, ib.ib_xchar, ib.ib_ychar,
                  (GRECT *)&ib.ib_xicon, (GRECT *)&ib.ib_xtext);  /* FIXME: Ugly typecasting */
                state &= ~SELECTED;
                break;
            case G_USERDEF:
                state = ob_user(tree, obj, &t, spec, state, state);
                break;
          } /* switch type */
        }
        if ( (obtype == G_STRING) ||
             (obtype == G_TITLE) ||
             (obtype == G_BUTTON) )
        {
          len = LBWMOV((WORD *)ad_intin, (BYTE *)spec);
          if (len)
          { 
            gsx_attr(TRUE, MD_TRANS, BLACK);
            tmpy = t.g_y + ((t.g_h-gl_hchar)/2);
            if (obtype == G_BUTTON)
              tmpx = t.g_x + ((t.g_w-(len*gl_wchar))/2);
            else
              tmpx = t.g_x;
            gsx_tblt(IBM, tmpx, tmpy, len);
          }
        }
        if (state)
        {
          if ( state & OUTLINED )
          {
              gsx_attr(FALSE, MD_REPLACE, BLACK);
              gr_box(t.g_x-3, t.g_y-3, t.g_w+6, t.g_h+6, 1);
              gsx_attr(FALSE, MD_REPLACE, WHITE);
              gr_box(t.g_x-2, t.g_y-2, t.g_w+4, t.g_h+4, 2);
          }

          if (th > 0)
            gr_inside(&t, th);
          else 
            th = -th;

          if ( (state & SHADOWED) && th )
          {
            g_vsf_color(bcol);
            bb_fill(MD_REPLACE, FIS_SOLID, 0, t.g_x, t.g_y+t.g_h+th,
                                t.g_w + th, 2*th);
            bb_fill(MD_REPLACE, FIS_SOLID, 0, t.g_x+t.g_w+th, t.g_y, 
                        2*th, t.g_h+(3*th));
          }
          if ( state & CHECKED )
          {
            gsx_attr(TRUE, MD_TRANS, BLACK);
            intin[0] = 0x08;                            /* a check mark */
            gsx_tblt(IBM, t.g_x+2, t.g_y, 1);
          }
          if ( state & CROSSED )
          {
            gsx_attr(FALSE, MD_TRANS, WHITE);
            gsx_cline(t.g_x, t.g_y, t.g_x+t.g_w-1, t.g_y+t.g_h-1);
            gsx_cline(t.g_x, t.g_y+t.g_h-1, t.g_x+t.g_w-1, t.g_y);
          }
          if ( state & DISABLED )
          {
            g_vsf_color(WHITE);
            bb_fill(MD_TRANS, FIS_PATTERN, IP_4PATT, t.g_x, t.g_y,
                         t.g_w, t.g_h);
          }
          if (state & SELECTED)
          {
            bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, t.g_x,t.g_y, t.g_w, t.g_h);
          }
        }
} /* just_draw */


/*
 *      Object draw routine that walks tree and draws appropriate objects.
 */
void ob_draw(LONG tree, WORD obj, WORD depth)
{
        WORD            last, pobj;
        WORD            sx, sy;

        pobj = get_par(tree, obj, &last);

        //kprintf("It does not work without this kprintf...\n");
        if (pobj != NIL)
          ob_offset(tree, pobj, &sx, &sy);
        else
          sx = sy = 0;

        gsx_moff();
        everyobj(tree, obj, last, (void(*)())just_draw, sx, sy, depth);
        gsx_mon();
}




/*
*       Routine to find the object that is previous to us in the
*       tree.  The idea is we get our parent and then walk down
*       his list of children until we find a sibling who points to
*       us.  If we are the first child or we have no parent then
*       return NIL.
*/
WORD get_prev(LONG tree, WORD parent, WORD obj)
{
        register WORD   nobj, pobj;

        pobj = LWGET(OB_HEAD(parent));
        if (pobj != obj)
        {
          while (TRUE)
          {
            nobj = LWGET(OB_NEXT(pobj));
            if (nobj != obj)
              pobj = nobj;
            else
              return(pobj);
          }
        }
        else
          return(NIL);
} /* get_prev */



/*
*       Routine to find out which object a certain mx,my value is
*       over.  Since each parent object contains its children the
*       idea is to walk down the tree, limited by the depth parameter,
*       and find the last object the mx,my location was over.
*/

/************************************************************************/
/* o b _ f i n d                                                        */
/************************************************************************/
WORD ob_find(LONG tree, WORD currobj, WORD depth, WORD mx, WORD my)
{
        WORD            lastfound;
        WORD            dosibs, done, junk;
        GRECT           t, o;
        WORD            parent, childobj, flags;

        lastfound = NIL;

        if (currobj == 0)
          r_set(&o, 0, 0, 0, 0);
        else
        {
          parent = get_par(tree, currobj, &junk);
          ob_actxywh(tree, parent, &o);
        }
        
        done = FALSE;
        dosibs = FALSE;

        while( !done )
        {
                                                /* if inside this obj,  */
                                                /*   might be inside a  */
                                                /*   child, so check    */
          ob_relxywh(tree, currobj, &t);
          t.g_x += o.g_x;
          t.g_y += o.g_y;

          flags = LWGET(OB_FLAGS(currobj));
          if ( (inside(mx, my, &t)) &&
               (!(flags & HIDETREE)) )
          {
            lastfound = currobj;

            childobj = LWGET(OB_TAIL(currobj));
            if ( (childobj != NIL) && depth)
            {
              currobj = childobj;
              depth--;
              o.g_x = t.g_x;
              o.g_y = t.g_y;
              dosibs = TRUE;
            }
            else
              done = TRUE;
          }
          else
          {
            if ( (dosibs) &&
                 (lastfound != NIL) )
            {
                currobj = get_prev(tree, lastfound, currobj);
                if (currobj == NIL)
                  done = TRUE;
            }
            else
              done = TRUE;
          }
        }
                                                /* if no one was found  */
                                                /*   this will return   */
                                                /*   NIL                */
        return(lastfound);
} /* ob_find */


/*
*       Routine to add a child object to a parent object.  The child
*       is added at the end of the parent's current sibling list.
*       It is also initialized.
*/
void ob_add(LONG tree, WORD parent, WORD child)
{
        register WORD   lastkid;
        register LONG   ptail;

        if ( (parent != NIL) &&
             (child != NIL) )
        {
                                                /* initialize child     */
          LWSET(OB_NEXT(child), parent);

          lastkid = LWGET( ptail = OB_TAIL(parent) );
          if (lastkid == NIL)
                                                /* this is parent's 1st */
                                                /*   kid, so both head  */
                                                /*   and tail pt to it  */
            LWSET(OB_HEAD(parent), child);
          else
                                                /* add kid to end of    */
                                                /*   kid list           */
            LWSET(OB_NEXT(lastkid), child);
          LWSET(ptail, child);
        }
} /* ob_add */


/*
*       Routine to delete an object from the tree.
*/
void ob_delete(LONG tree, WORD obj)
{
        register WORD   parent;
        WORD            prev, nextsib;
        register LONG   ptail, phead;

        if (obj != ROOT)
          parent = get_par(tree, obj, &nextsib);
        else
          return;

        if ( LWGET(phead = OB_HEAD(parent)) == obj )
        {
                                                /* this is head child   */
                                                /*   in list            */
          if ( LWGET(ptail = OB_TAIL(parent)) == obj)
          {
                                                /* this is only child   */
                                                /*   in list, so fix    */
                                                /*   head & tail ptrs   */
            nextsib = NIL;
            LWSET(ptail, NIL);
          }
                                                /*   move head ptr to   */
                                                /*   next child in list */
          LWSET(phead, nextsib);
        }
        else
        {
                                                /* it's somewhere else, */
                                                /*   so move pnext      */
                                                /*   around it          */
          prev = get_prev(tree, parent, obj);
          LWSET(OB_NEXT(prev), nextsib);
          if ( LWGET(ptail = OB_TAIL(parent)) == obj)
                                                /* this is last child   */
                                                /*   in list, so move   */
                                                /*   tail ptr to prev   */
                                                /*   child in list      */
            LWSET(ptail, prev);
        }
} /* ob_delete */


/*
*       Routine to change the order of an object relative to its
*       siblings in the tree.  0 is the head of the list and NIL
*       is the tail of the list.
*/
void ob_order(LONG tree, WORD mov_obj, WORD new_pos)
{
        register WORD   parent;
        WORD            chg_obj, ii, junk;
        register LONG   phead, pnext, pmove;

        if (mov_obj != ROOT)
          parent = get_par(tree, mov_obj, &junk);
        else
          return;

        ob_delete(tree, mov_obj);
        chg_obj = LWGET(phead = OB_HEAD(parent));
        pmove = OB_NEXT(mov_obj);
        if (new_pos == 0)
        {
                                                /* put mov_obj at head  */
                                                /*   of list            */
          LWSET(pmove, chg_obj);
          LWSET(phead, mov_obj);
        }
        else
        {
                                                /* find new_pos         */
          if (new_pos == NIL)
            chg_obj = LWGET(OB_TAIL(parent));
          else
          {
            for (ii = 1; ii < new_pos; ii++)
              chg_obj = LWGET(OB_NEXT(chg_obj));
          } /* else */
                                                /* now add mov_obj      */
                                                /*   after chg_obj      */
          LWSET(pmove, LWGET(pnext = OB_NEXT(chg_obj)));
          LWSET(pnext, mov_obj);
        }
        if (LWGET(pmove) == parent)
          LWSET(OB_TAIL(parent), mov_obj);
} /* ob_order */



/************************************************************************/
/* o b _ e d i t                                                        */
/************************************************************************/
/* see OBED.C                                                           */

/*
*       Routine to change the state of an object and redraw that
*       object using the current clip rectangle.
*/
void ob_change(LONG tree, WORD obj, UWORD new_state, WORD redraw)
{
        WORD            flags, obtype, th;
        GRECT           t;
        UWORD           curr_state;
        LONG            spec;

        ob_sst(tree, obj, &spec, (WORD*)&curr_state, &obtype, &flags, &t, &th);
        
        if ( (curr_state == new_state) ||
             (spec == -1L) )
          return;       

        LWSET(OB_STATE(obj), new_state);

        if (redraw)
        {
          ob_offset(tree, obj, &t.g_x, &t.g_y);

          gsx_moff();

          th = (th < 0) ? 0 : th;

          if ( obtype == G_USERDEF )
          {
            ob_user(tree, obj, &t, spec, curr_state, new_state);
            redraw = FALSE;
          }
          else
          {
            if ( (obtype != G_ICON) &&
               ((new_state ^ curr_state) & SELECTED) )
            {
              bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, t.g_x+th, t.g_y+th,
                        t.g_w-(2*th), t.g_h-(2*th));
              redraw = FALSE;
            }
          }

          if (redraw) {
              just_draw(tree, obj, t.g_x, t.g_y);
          }

          gsx_mon();
        }
        return;
} /* ob_change */


UWORD ob_fs(LONG tree, WORD ob, WORD *pflag)
{
        *pflag = LWGET(OB_FLAGS(ob));
        return( LWGET(OB_STATE(ob)) );
}

