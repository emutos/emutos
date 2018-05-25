/*      GEMOBLIB.C      03/15/84 - 05/27/85     Gregg Morris            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

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

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"
#include "funcdef.h"

#include "geminit.h"
#include "gemgsxif.h"
#include "gemobjop.h"
#include "gemgraf.h"
#include "optimopt.h"
#include "rectfunc.h"
#include "gemoblib.h"

#include "string.h"
#include "kprint.h"

                                                /* in GSXBIND.C         */
#define g_vsf_color( x )          gsx_1code(S_FILL_COLOR, x)


/*
 *  Routine to find the x,y offset of a particular object relative
 *  to the physical screen.  This involves accumulating the offsets
 *  of all the object's parents up to and including the root.
 */
void ob_offset(OBJECT *tree, WORD obj, WORD *pxoff, WORD *pyoff)
{
    WORD   junk;
    OBJECT *treeptr = tree;

    *pxoff = *pyoff = 0;
    do
    {
        *pxoff += (treeptr+obj)->ob_x;  /* add in current object's offsets */
        *pyoff += (treeptr+obj)->ob_y;
        obj = get_par(tree, obj, &junk);/* then get parent */
    } while (obj != NIL);
}


/*
 *  ob_relxywh: fill GRECT with x/y/w/h from object (relative x/y)
 */
void ob_relxywh(OBJECT *tree, WORD obj, GRECT *pt)
{
    OBJECT *objptr = tree + obj;

    memcpy(pt, &objptr->ob_x, sizeof(GRECT));
}


/*
 *  ob_actxywh: fill GRECT with x/y/w/h of object (absolute x/y)
 */
void ob_actxywh(OBJECT *tree, WORD obj, GRECT *pt)
{
    OBJECT *objptr = tree + obj;

    ob_offset(tree, obj, &pt->g_x, &pt->g_y);
    pt->g_w = objptr->ob_width;
    pt->g_h = objptr->ob_height;
}


/*
 * ob_setxywh: copy values from GRECT into object
 */
void ob_setxywh(OBJECT *tree, WORD obj, GRECT *pt)
{
    OBJECT *objptr = tree + obj;

    memcpy(&objptr->ob_x, pt, sizeof(GRECT));
}


/*
 *  Routine to take an unformatted raw string and, based on a
 *  template string, build a formatted string.
 */
void ob_format(WORD just, BYTE *raw_str, BYTE *tmpl_str, BYTE *fmt_str)
{
    BYTE *pfbeg, *ptbeg, *prbeg;
    BYTE *ptend, *prend;
    WORD inc, ptlen, prlen;

    if (*raw_str == '@')
        *raw_str = '\0';

    pfbeg = fmt_str;
    ptbeg = tmpl_str;
    prbeg = raw_str;

    ptlen = strlen(tmpl_str);
    prlen = strlen(raw_str);

    inc = 1;
    pfbeg[ptlen] = '\0';
    if (just == TE_RIGHT)
    {
        inc = -1;
        pfbeg = pfbeg + ptlen - 1;
        ptbeg = ptbeg + ptlen - 1;
        prbeg = prbeg + prlen - 1;
    }

    ptend = ptbeg + (inc * ptlen);
    prend = prbeg + (inc * prlen);

    while(ptbeg != ptend)
    {
        if (*ptbeg != '_')
            *pfbeg = *ptbeg;
        else
        {
            if (prbeg != prend)
            {
                *pfbeg = *prbeg;
                prbeg += inc;
            }
            else
                *pfbeg = '_';
        }

        pfbeg += inc;
        ptbeg += inc;
    }
}


static __inline__ WORD call_usercode(USERBLK *ub, PARMBLK *pb)
{
    register WORD (*ub_code)(PARMBLK *parmblock) __asm__("a0") = ub->ub_code;
    register WORD retvalue __asm__("d0");

    __asm__ volatile
    (
        "move.l  %2,-(sp)\n\t"
        "jsr     (%1)\n\t"
        "addq.l  #4,sp\n\t"
    : "=r"(retvalue)
    : "a"(ub_code), "g"(pb)
    : "d1", "d2", "a1", "a2", "memory", "cc"
    );
    return retvalue;
}


/*
 *  Routine to load up and call a user-defined object draw or change
 *  routine.
 */
static WORD ob_user(OBJECT *tree, WORD obj, GRECT *pt, LONG spec,
                    WORD curr_state, WORD new_state)
{
    PARMBLK pb;
    USERBLK *ub = (USERBLK *)spec;

    pb.pb_tree = tree;
    pb.pb_obj = obj;
    pb.pb_prevstate = curr_state;
    pb.pb_currstate = new_state;
    rc_copy(pt, (GRECT *)&pb.pb_x);     /* FIXME: Ugly typecasting */
    gsx_gclip((GRECT *)&pb.pb_xc);      /* FIXME: ditto */
    pb.pb_parm = ub->ub_parm;

    return call_usercode(ub, &pb);
}


/*
 *  Routine to draw an object from an object tree.
 */
static void just_draw(OBJECT *tree, WORD obj, WORD sx, WORD sy)
{
    WORD bcol, tcol, ipat, icol, tmode, th;
    WORD state, obtype, len, flags;
    LONG spec;
    WORD tmpx, tmpy, tmpth;
    BYTE ch;
    GRECT t, c;
    TEDINFO edblk;
    BITBLK bi;
    ICONBLK ib;

    ch = ob_sst(tree, obj, &spec, &state, &obtype, &flags, &t, &th);

    if ((flags & HIDETREE) || (spec == -1L))
        return;

    t.g_x = sx;
    t.g_y = sy;

    /*
     * do trivial reject with full extent including outline, shadow,
     * & thickness
     */
    if (gl_wclip && gl_hclip)
    {
        rc_copy(&t, &c);
        if (state & OUTLINED)
            gr_inside(&c, -3);
        else
            gr_inside(&c, ((th < 0) ? (3 * th) : (-3 * th)) );

        if (!(gsx_chkclip(&c)))
            return;
    }

    /*
     * handle all non-string types
     */
    if (obtype != G_STRING)
    {
        tmpth = (th < 0) ? 0 : th;
        tmode = MD_REPLACE;
        tcol = BLACK;

        /*
         * for all tedinfo types, get copy of ted, crack the
         * color word and set the text color
         */
        switch(obtype)
        {
        case G_BOXTEXT:
        case G_FBOXTEXT:
        case G_TEXT:
        case G_FTEXT:
            memcpy(&edblk, (TEDINFO *)spec, sizeof(TEDINFO));
            gr_crack(edblk.te_color, &bcol,&tcol, &ipat, &icol, &tmode);
            break;
        }

        /*
         * handle box types: if not ted, crack the color; for all
         * box types, draw the box with border
         */
        switch(obtype)
        {
        case G_BOX:
        case G_BOXCHAR:
        case G_IBOX:
            gr_crack((UWORD)spec, &bcol, &tcol, &ipat, &icol, &tmode);
            /* drop thru */
        case G_BUTTON:
            if (obtype == G_BUTTON)
            {
                bcol = BLACK;
                ipat = IP_HOLLOW;
                icol = WHITE;
            }
            /* drop thru */
        case G_BOXTEXT:
        case G_FBOXTEXT:
            /* draw box's border */
            if (th != 0)
            {
                gsx_attr(FALSE, MD_REPLACE, bcol);
                gr_box(t.g_x, t.g_y, t.g_w, t.g_h, th);
            }
            /* draw filled box */
            if (obtype != G_IBOX)
            {
                gr_inside(&t, tmpth);
                gr_rect(icol, ipat, &t);
                gr_inside(&t, -tmpth);
            }
            break;
        }

        gsx_attr(TRUE, tmode, tcol);

        /*
         * do what's left for all types
         */
        switch( obtype )
        {
        case G_FTEXT:
        case G_FBOXTEXT:
            strcpy(D.g_rawstr, edblk.te_ptext);
            strcpy(D.g_tmpstr, edblk.te_ptmplt);
            ob_format(edblk.te_just, D.g_rawstr, D.g_tmpstr, D.g_fmtstr);
            /* drop thru to gr_gtext */
        case G_BOXCHAR:
            edblk.te_ptext = D.g_fmtstr;
            if (obtype == G_BOXCHAR)
            {
                D.g_fmtstr[0] = ch;
                D.g_fmtstr[1] = '\0';
                edblk.te_just = TE_CNTR;
                edblk.te_font = IBM;
            }
            /* drop thru to gr_gtext */
        case G_TEXT:
        case G_BOXTEXT:
            gr_inside(&t, tmpth);
            gr_gtext(edblk.te_just, edblk.te_font, edblk.te_ptext, &t);
            gr_inside(&t, -tmpth);
            break;
        case G_IMAGE:
            bi = *((BITBLK *)spec);
            gsx_blt((void *)bi.bi_pdata, bi.bi_x, bi.bi_y, bi.bi_wb,
                    NULL, t.g_x, t.g_y, gl_width/8, bi.bi_wb * 8,
                    bi.bi_hl, MD_TRANS, bi.bi_color, WHITE);
            break;
        case G_ICON:
            ib = *((ICONBLK *)spec);
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

    /*
     * draw text for string/title/button
     */
    if ((obtype == G_STRING) ||
        (obtype == G_TITLE) ||
        (obtype == G_BUTTON))
    {
        len = expand_string(intin, (BYTE *)spec);
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

    /*
     * handle changes in appearance due to object state
     */
    if (state)
    {
        if (state & OUTLINED)
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

        if ((state & SHADOWED) && th)
        {
            g_vsf_color(bcol);
            bb_fill(MD_REPLACE, FIS_SOLID, 0, t.g_x, t.g_y+t.g_h+th,
                    t.g_w + th, 2*th);
            bb_fill(MD_REPLACE, FIS_SOLID, 0, t.g_x+t.g_w+th, t.g_y,
                    2*th, t.g_h+(3*th));
        }

        if (state & CHECKED)
        {
            gsx_attr(TRUE, MD_TRANS, BLACK);
            intin[0] = 0x08;                            /* a check mark */
            gsx_tblt(IBM, t.g_x+2, t.g_y, 1);
        }

        if (state & CROSSED)
        {
            gsx_attr(FALSE, MD_TRANS, WHITE);
            gsx_cline(t.g_x, t.g_y, t.g_x+t.g_w-1, t.g_y+t.g_h-1);
            gsx_cline(t.g_x, t.g_y+t.g_h-1, t.g_x+t.g_w-1, t.g_y);
        }

        if (state & DISABLED)
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
 *  Object draw routine that walks tree and draws appropriate objects.
 */
void ob_draw(OBJECT *tree, WORD obj, WORD depth)
{
    WORD last, pobj;
    WORD sx, sy;

    pobj = get_par(tree, obj, &last);

    if (pobj != NIL)
        ob_offset(tree, pobj, &sx, &sy);
    else
        sx = sy = 0;

    gsx_moff();
    everyobj(tree, obj, last, just_draw, sx, sy, depth);
    gsx_mon();
}


/*
 *  Routine to find the object that is previous to us in the
 *  tree.  The idea is we get our parent and then walk down
 *  his list of children until we find a sibling who points to
 *  us.  If we are the first child or we have no parent then
 *  return NIL.
 */
static WORD get_prev(OBJECT *tree, WORD parent, WORD obj)
{
    WORD nobj, pobj;
    OBJECT *treeptr = tree;

    pobj = (treeptr+parent)->ob_head;
    if (pobj == obj)
        return NIL;

    while(TRUE)
    {
        nobj = (treeptr+pobj)->ob_next;
        if (nobj == obj)        /* next object is us,         */
            return pobj;        /*  so return previous object */

        if (nobj == parent)     /* next object is the parent,  */
            return NIL;         /*  so we're not in this chain */

        pobj = nobj;
    }
}


/*
 *  ob_find: routine to find out which object a certain mx,my value is over
 *
 *  Since each parent object contains its children the idea is to
 *  walk down the tree, limited by the depth parameter, and find
 *  the last object the mx,my location was over.
 */
WORD ob_find(OBJECT *tree, WORD currobj, WORD depth, WORD mx, WORD my)
{
    WORD lastfound;
    WORD dosibs, done, junk;
    GRECT t, o;
    WORD parent, childobj, flags;
    OBJECT *objptr;

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

    while(!done)
    {
        /*
         * if inside this obj, might be inside a child, so check
         */
        ob_relxywh(tree, currobj, &t);
        t.g_x += o.g_x;
        t.g_y += o.g_y;

        objptr = tree + currobj;
        flags = objptr->ob_flags;
        if ( (inside(mx, my, &t)) && (!(flags & HIDETREE)) )
        {
            lastfound = currobj;

            childobj = objptr->ob_tail;
            if ((childobj != NIL) && depth)
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
            if (dosibs && (lastfound != NIL))
            {
                currobj = get_prev(tree, lastfound, currobj);
                if (currobj == NIL)
                    done = TRUE;
            }
            else
                done = TRUE;
        }
    }

    /*
     * if no one was found, this will return NIL
     */
    return lastfound;
}


/*
 *  Routine to add a child object to a parent object.  The child
 *  is added at the end of the parent's current sibling list.
 *  It is also initialized.
 */
void ob_add(OBJECT *tree, WORD parent, WORD child)
{
    WORD lastkid;
    OBJECT *treeptr = tree;
    OBJECT *parentptr;

    if ((parent != NIL) && (child != NIL))
    {
        parentptr = treeptr + parent;

        /* initialize child */
        (treeptr+child)->ob_next = parent;

        lastkid = parentptr->ob_tail;
        if (lastkid == NIL)             /* this is parent's 1st kid, so */
            parentptr->ob_head = child; /* both head and tail point to it  */
        else
            (treeptr+lastkid)->ob_next = child; /* add kid to end of kid list */
        parentptr->ob_tail = child;
    }
}


/*
 *  Routine to delete an object from the tree.
 */
WORD ob_delete(OBJECT *tree, WORD obj)
{
    WORD parent;
    WORD prev, nextsib;
    OBJECT *treeptr = tree;
    OBJECT *parentptr, *prevptr;

    if (obj == ROOT)
        return 0;           /* can't delete the root object! */

    parent = get_par(tree, obj, &nextsib);

    parentptr = treeptr + parent;

    if (parentptr->ob_head == obj)      /* this is head child in list */
    {
        if (parentptr->ob_tail == obj)
        {
            /*
             * this is the only child in the list,
             * so fix head & tail ptrs
             */
            nextsib = NIL;
            parentptr->ob_tail = NIL;
        }

        /* move head ptr to next child in list */
        parentptr->ob_head = nextsib;
    }
    else                /* it's somewhere else, so move pnext around it */
    {
        prev = get_prev(tree, parent, obj);
        if (prev == NIL)  /* object not found */
        {
            KINFO(("objc_delete() failed: object %d not found\n",obj));
            return 0;
        }

        prevptr = treeptr + prev;
        prevptr->ob_next = nextsib;
        if (parentptr->ob_tail == obj)  /* this is last child in list, so move */
            parentptr->ob_tail = prev;  /*  tail ptr to previous child in list */
    }

    return 1;
}


/*
 *  Routine to change the order of an object relative to its
 *  siblings in the tree.  0 is the head of the list and NIL
 *  is the tail of the list.
 */
void ob_order(OBJECT *tree, WORD mov_obj, WORD new_pos)
{
    WORD parent;
    WORD chg_obj, ii, junk;
    OBJECT *treeptr = tree;
    OBJECT *parentptr, *movptr, *chgptr;

    if (mov_obj == ROOT)
        return;

    parent = get_par(tree, mov_obj, &junk);
    parentptr = treeptr + parent;
    movptr = treeptr + mov_obj;

    ob_delete(tree, mov_obj);
    chg_obj = parentptr->ob_head;
    if (new_pos == 0)
    {
          movptr->ob_next = chg_obj;    /* put mov_obj at head of list */
          parentptr->ob_head = mov_obj;
    }
    else
    {
        /* find new_pos */
        if (new_pos == NIL)
            chg_obj = parentptr->ob_tail;
        else
        {
            for (ii = 1; ii < new_pos; ii++)
            {
                chgptr = treeptr + chg_obj;
                chg_obj = chgptr->ob_next;
            }
        }

        /* now add mov_obj after chg_obj */
        chgptr = treeptr + chg_obj;
        movptr->ob_next = chgptr->ob_next;
        chgptr->ob_next = mov_obj;
    }

    if (movptr->ob_next == parent)
        parentptr->ob_tail = mov_obj;
}


/*
 *  Routine to change the state of an object and redraw that
 *  object using the current clip rectangle.
 */
void ob_change(OBJECT *tree, WORD obj, UWORD new_state, WORD redraw)
{
    WORD flags, obtype, th;
    GRECT t;
    UWORD curr_state;
    LONG spec;
    OBJECT *objptr;

    ob_sst(tree, obj, &spec, (WORD*)&curr_state, &obtype, &flags, &t, &th);

    if ((curr_state == new_state) || (spec == -1L))
        return;

    objptr = tree + obj;
    objptr->ob_state = new_state;

    if (redraw)
    {
        ob_offset(tree, obj, &t.g_x, &t.g_y);

        gsx_moff();

        th = (th < 0) ? 0 : th;

        if (obtype == G_USERDEF)
        {
            ob_user(tree, obj, &t, spec, curr_state, new_state);
            redraw = FALSE;
        }
        else
        {
            if ((obtype != G_ICON) &&
               ((new_state ^ curr_state) & SELECTED) )
            {
                bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, t.g_x+th, t.g_y+th,
                        t.g_w-(2*th), t.g_h-(2*th));
                redraw = FALSE;
            }
        }

        if (redraw)
            just_draw(tree, obj, t.g_x, t.g_y);

        gsx_mon();
    }
}


UWORD ob_fs(OBJECT *tree, WORD obj, WORD *pflag)
{
    OBJECT *objptr = tree + obj;

    *pflag = objptr->ob_flags;
    return objptr->ob_state;
}
