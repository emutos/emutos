/*      DESKACT.C       06/11/84 - 09/05/85             Lee Lorenzen    */
/*      merged source   5/18/87 - 5/28/87               mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
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

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "obdefs.h"
#include "rectfunc.h"
#include "gemdos.h"
#include "gsxdefs.h"

#include "aesdefs.h"
#include "aesext.h"
#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "aesbind.h"
#include "deskact.h"
#include "deskmain.h"
#include "desksupp.h"
#include "deskins.h"


/* defines */
#define SHRT_MIN    (-32767)    /* (would be in limits.h if it existed) */

/* Prototypes: */
static WORD act_chkobj(OBJECT *tree, WORD root, WORD obj, WORD mx, WORD my, WORD w, WORD h);




static WORD gr_obfind(OBJECT *tree, WORD root, WORD mx, WORD my)
{
    WORD sobj;

    sobj = objc_find(tree, root, 2, mx, my);
    if ((sobj != root) && (sobj != NIL))
        sobj = act_chkobj(tree, root, sobj, mx, my, 1, 1);

    return sobj;
}


/*
 *  Return TRUE as long as the mouse is down.  Block until the
 *  mouse moves into or out of the specified rectangle.
 */
static WORD gr_isdown(WORD out, WORD x, WORD y, WORD w, WORD h)
{
    UWORD ev_which;
    UWORD dummy;

    ev_which = evnt_multi(MU_BUTTON | MU_M1, 0x01, 0xff, 0x00, out, x, y, w, h,
                            0, 0, 0, 0, 0, NULL, 0x0, 0x0,
                            &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
    if (ev_which & MU_BUTTON)
        return FALSE;

    return TRUE;
}


/*
 * allocate & build list of (x,y) coordinates of selected objects
 *
 * returns pointer to allocated list, plus count of selected objects in pnum
 * (if there are no selected objects, returns NULL pointer)
 */
static Point *gr_accobs(OBJECT *tree, WORD root, WORD *pnum)
{
    WORD i, n;
    WORD obj;
    LONG maxpts;
    Point *pxypts;

    /* count points */
    for (obj = tree[root].ob_head, n = 0; obj > root; obj = tree[obj].ob_next)
        if (tree[obj].ob_state & SELECTED)
            n++;

    maxpts = dos_avail_anyram() / sizeof(Point);
    if (n > maxpts)
        n = maxpts;
    *pnum = n;

    if (n == 0)
        return NULL;

    pxypts = dos_alloc_anyram(n*sizeof(Point));

    i = 0;
    for (obj = tree[root].ob_head; obj > root; obj = tree[obj].ob_next)
    {
        if (tree[obj].ob_state & SELECTED)
        {
            pxypts[i].x = tree[root].ob_x + tree[obj].ob_x;
            pxypts[i].y = tree[root].ob_y + tree[obj].ob_y;
            i++;
            if (i >= n)
                break;
        }
    }

    return pxypts;
}


static void move_drvicon(OBJECT *tree, WORD root, WORD x, WORD y, Point *pts, WORD sxoff, WORD syoff)
{
    ANODE *an_disk;
    GRECT t;
    WORD objcnt;
    WORD obj;
    WORD oldx;
    WORD oldy;

    objcnt = 0;
    for (obj = tree[root].ob_head; obj > root; obj = tree[obj].ob_next)
    {
        if (tree[obj].ob_state & SELECTED)
        {
            oldx = tree[obj].ob_x;
            oldy = tree[obj].ob_y;

            snap_icon(x + pts[objcnt].x, y + pts[objcnt].y,
                                &tree[obj].ob_x, &tree[obj].ob_y, sxoff, syoff);

            an_disk = app_afind_by_id(obj);
            if (an_disk)        /* must be true, but just in case ... */
            {
                an_disk->a_xspot = tree[obj].ob_x;
                an_disk->a_yspot = tree[obj].ob_y;
            }
            t.g_x = oldx;
            t.g_y = oldy;
            t.g_w = G.g_wicon;
            t.g_h = G.g_hicon;
            do_wredraw(DESKWH, &t);
            t.g_x = tree[obj].ob_x;
            t.g_y = tree[obj].ob_y;
            do_wredraw(DESKWH, &t);
            ++objcnt;
        }
    }
}


/*
 *  Calculate the extent of the list of x,y points given
 */
static void gr_extent(WORD numpts, Point *xylnpts, GRECT *pt)
{
    WORD i;
    WORD lx, ly, gx, gy;

    lx = ly = MAX_COORDINATE;
    gx = gy = 0;
    for (i = 0; i < numpts; i++)
    {
        if (xylnpts[i].x < lx)
            lx = xylnpts[i].x;
        if (xylnpts[i].y < ly)
            ly = xylnpts[i].y;
        if (xylnpts[i].x > gx)
            gx = xylnpts[i].x;
        if (xylnpts[i].y > gy)
            gy = xylnpts[i].y;
    }
    r_set(pt, lx, ly, gx-lx+1, gy-ly+1);
}


/*
 *  Draw a list of polylines a number of times based on a list of
 *  x,y object locations that are all relative to a certain x,y offset
 */
static void gr_plns(WORD x, WORD y, WORD numpts, Point *xylnpts, WORD numobs,
                    Point *xyobpts)
{
    WORD i;

    graf_mouse(M_OFF, NULL);

    for (i = 0; i < numobs; i++)
    {
        gsx_pline(x + xyobpts[i].x, y + xyobpts[i].y, numpts, xylnpts);
    }
    graf_mouse(M_ON, NULL);
}


static WORD gr_bwait(GRECT *po, WORD mx, WORD my, WORD numpts, Point *xylnpts,
                     WORD numobs, Point *xyobpts)
{
    WORD  down;

    /*
     * Since the desktop and the AES currently share the same VDI work-
     * station, we have to reset the clipping and drawing mode here
     */
    gsx_sclip(&gl_rscreen);
    gsx_attr(FALSE, MD_XOR, BLACK);

    /* draw old */
    gr_plns(po->g_x, po->g_y, numpts, xylnpts, numobs, xyobpts);

    /* wait for change */
    down = gr_isdown(TRUE, mx, my, 2, 2);

    /* erase old */
    gr_plns(po->g_x, po->g_y, numpts, xylnpts, numobs, xyobpts);

    /* return exit event */
    return down;
}


static void gr_obalign(WORD numobs, WORD x, WORD y, Point *xyobpts)
{
    WORD i;

    for (i = 0; i < numobs; i++)
    {
        xyobpts[i].x -= x;
        xyobpts[i].y -= y;
    }
}


/*
 *  This routine is used to drag a list of polylines
 */
static void gr_drgplns(WORD in_mx, WORD in_my, GRECT *pc,
            WORD numpts, Point *xylnpts, WORD numobs, Point *xyobpts,
            WORD *pdulx, WORD *pduly, WORD *poffx, WORD *poffy, WORD *pdwh, WORD *pdobj)
{
    OBJECT *tree;
    WNODE  *pw;
    WORD   root, curr_wh, curr_root, curr_sel, dst_wh;
    WORD   l_mx, l_my;
    WORD   offx, offy;
    WORD   down, button, keystate;
    GRECT  o, ln;
    ANODE  *pa;
    OBJECT *obj;

    graf_mouse(FLATHAND, NULL);     /* flat hand */

    l_mx = in_mx;
    l_my = in_my;

    /* figure out extent of single polygon */
    gr_extent(numpts, xylnpts, &ln);

    /* calc overall extent for use as bounds of motion */
    gr_extent(numobs, xyobpts, &o);
    o.g_w += ln.g_w;
    o.g_h += ln.g_h;

    gr_obalign(numobs, o.g_x, o.g_y, xyobpts);

    offx = l_mx - o.g_x;
    offy = l_my - o.g_y;

    curr_wh = 0x0;
    curr_root = 0;
    curr_sel = 0;

    do
    {
        o.g_x = l_mx - offx;
        o.g_y = l_my - offy;
        rc_constrain(pc, &o);
        down = gr_bwait(&o, l_mx, l_my,numpts, xylnpts, numobs, xyobpts);

        graf_mkstate(&l_mx, &l_my, &button, &keystate);
        dst_wh = wind_find(l_mx, l_my);
        tree = G.g_screen;
        root = DROOT;
        if (dst_wh)
        {
            pw = win_find(dst_wh);
            if (pw)
            {
                tree = G.g_screen;
                root = pw->w_root;
            }
            else dst_wh = DESKWH;
        }

        *pdobj = gr_obfind(tree, root, l_mx, l_my);

/* The DESKTOP v1.2 test is rather less efficient here, but it is
 * the same test. */
        if ((*pdobj == root) || (*pdobj == NIL))
        {
            if (curr_sel)
            {
                act_chg(curr_wh, curr_root, curr_sel, pc, FALSE, TRUE);
                curr_wh = 0x0;
                curr_root = 0x0;
                curr_sel = 0;
                continue;
            }
        }

        if (*pdobj != curr_sel)
        {
            if (curr_sel)
            {
                act_chg(curr_wh, curr_root, curr_sel, pc, FALSE, TRUE);
                curr_wh = 0x0;
                curr_root = 0x0;
                curr_sel = 0;
            }
        }
        obj = tree + *pdobj;
        if (!(obj->ob_state & SELECTED))
        {
            pa = i_find(dst_wh, *pdobj, NULL, NULL);
            if (pa)
            {
                curr_wh = dst_wh;
                curr_root = root;
                curr_sel = *pdobj;
                act_chg(curr_wh, curr_root, curr_sel, pc, TRUE, TRUE);
            }
        }
    } while (down);

    if (curr_sel)
        act_chg(curr_wh, curr_root, curr_sel, pc, FALSE, TRUE);

    *pdulx = l_mx;              /* pass back dest. x,y  */
    *pduly = l_my;
    *poffx = offx;              /* and offsets */
    *poffy = offy;
    *pdwh = dst_wh;
    desk_busy_off();
}


/*
 *  Check to see if the mouse has been clicked over an object.
 *
 *  If the current view is by icon, then check the areas occupied by the
 *  icon graphic and the icon text to determine if the icon was selected.
 *
 *  If the current view is by text, then use the name portion of the text
 *  string.
 */
static WORD act_chkobj(OBJECT *tree, WORD root, WORD obj, WORD mx, WORD my, WORD w, WORD h)
{
    ICONBLK *ib;
    WORD    view, ox, oy, icon;
    GRECT   t, m;

    ox = tree[root].ob_x + tree[obj].ob_x;
    oy = tree[root].ob_y + tree[obj].ob_y;

    view = (root == DROOT) ? V_ICON : G.g_iview;
    switch( view )
    {
    case V_TEXT:
        r_set(&t, ox, oy, LEN_FNODE * gl_wchar, gl_hchar);
        r_set(&m, mx, my, w, h);
        if (!rc_intersect(&t, &m))
            return root;
        break;
    case V_ICON:
        r_set(&t, mx - ox, my - oy, w, h);
        r_set(&m, mx - ox, my - oy, w, h);
        icon = G.g_screeninfo[obj].u.icon.index;
        ib = &G.g_iblist[icon];
        if (!rc_intersect((GRECT *)&ib->ib_xtext, &t))
        {
            if (!rc_intersect((GRECT *)&ib->ib_xicon, &m))
                return root;
        }
        break;
    }

    return obj;
}


/*
 *  Change the SELECTED bit in a single screen object
 *
 *  Usage of (some) arguments:
 *      WORD   obj              * object to affect
 *      BOOL   set              * set or reset value
 *      BOOL   dodraw           * draw resulting change
 *
 *  We do not change the state if the item is disabled
 */
WORD act_chg(WORD wh, WORD root, WORD obj, GRECT *pc,
             BOOL set, BOOL dodraw)
{
    OBJECT *tree = G.g_screen;
    FNODE *fn;
    UWORD curr_state;
    UWORD old_state;
    GRECT t;

    old_state = curr_state = tree[obj].ob_state;
    if (curr_state & DISABLED)
        return FALSE;

    /* get object's extent */
    rc_copy((GRECT *)&tree[obj].ob_x, &t);
    t.g_x += tree[root].ob_x;
    t.g_y += tree[root].ob_y;

    /* make change */
    if (set)
        curr_state |= SELECTED;
    else
        curr_state &= ~SELECTED;

    /*
     * NOTE: since tree == G.g_screen, obj is a valid index
     * into G.g_screeninfo[]
     */
    fn = G.g_screeninfo[obj].fnptr;
    if (fn)
        fn->f_selected = (curr_state & SELECTED) ? TRUE : FALSE;

    /* get it updated on screen */
    if (old_state != curr_state)
    {
        tree[obj].ob_state = curr_state;
        /*
         * clip to uncovered portion of desktop or window
         * and the object's extent
         */
        if (dodraw && rc_intersect(pc, &t))
        {
            do_wredraw(wh, &t);
        }
    }

    return TRUE;
}


/*
 *  Change the SELECTED bit of multiple objects in a window
 *
 *  If setting the SELECTED bit, we set it in all objects partially
 *  intersecting the given rectangle, allowing one object to be excluded.
 *  We also set the 'selected' indicator in the corresponding FNODEs.
 *
 *  If clearing the SELECTED bit, we clear it in all objects associated
 *  with the window.  We also clear the 'selected' indicator in all
 *  the FNODEs associated with this window
 */
void act_allchg(WORD wh, WORD root, WORD ex_obj, GRECT *pt, GRECT *pc,
                BOOL set)
{
    OBJECT *tree = G.g_screen;
    FNODE *fn;
    WORD obj, newstate;
    WORD offx, offy;
    GRECT o, a, w;

    /* clearing selections: do all FNODEs, not just the visible ones */
    if (!set)
        pn_clear(win_find(wh));

    offx = tree[root].ob_x;
    offy = tree[root].ob_y;

    /* accumulate extent of change in this rectangle */
    a.g_w = a.g_h = 0;

    rc_copy(pt, &w);
    rc_intersect(pc, &w);   /* limit selection to work area of window */

    for (obj = tree[root].ob_head; obj > root; obj = tree[obj].ob_next)
    {
        if (obj != ex_obj)
        {
            rc_copy((GRECT *)&tree[obj].ob_x, &o);
            o.g_x += offx;
            o.g_y = o.g_y + offy + 1;
            o.g_h -= 1;
            if (rc_intersect(&w, &o) &&
                (root != act_chkobj(tree,root,obj,o.g_x,o.g_y,o.g_w,o.g_h)))
            {
                /*
                 * mark FNODE iff we're setting the SELECTED bit
                 *
                 * NOTE: since tree == G.g_screen, obj is a valid index
                 * into G.g_screeninfo[]
                 */
                fn = G.g_screeninfo[obj].fnptr;
                if (fn)
                    fn->f_selected = set ? TRUE : FALSE;
                /* make change */
                newstate = tree[obj].ob_state;
                if (set)
                    newstate |= SELECTED;
                else
                    newstate &= ~SELECTED;
                if (newstate != tree[obj].ob_state)
                {
                    tree[obj].ob_state= newstate;
                    rc_copy((GRECT *)&tree[obj].ob_x, &o);
                    o.g_x += offx;
                    o.g_y += offy;
                    if (a.g_w)
                        rc_union(&o, &a);
                    else
                        rc_copy(&o, &a);
                }
            }
            else
            {
                if (!set)
                    tree[obj].ob_state &= ~SELECTED;
            }
        }
    }

    if (rc_intersect(pc, &a))
    {
        do_wredraw(wh, &a);
    }
}


/*
 *  Single click action on the specified screen objects
 *
 *  Rules:
 *   1. a shift-click on no object within a window will leave the selection
 *      state of all objects unchanged
 *   2. a click on no object within a window OR a (shift-)click outside a
 *      window will deselect all objects
 *   3. a click on an object will deselect all objects & select the object
 *   4. a shift-click on an object will toggle the state of that object only
 */
void act_bsclick(WORD wh, WORD root, WORD mx, WORD my, WORD keystate,
                 GRECT *pc, BOOL dclick)
{
    OBJECT *tree = G.g_screen;
    WORD obj;
    WORD shifted;
    WORD state;

    shifted = (keystate & K_LSHIFT) || (keystate & K_RSHIFT);
    obj = gr_obfind(tree, root, mx, my);

    if ((obj == root) && shifted)   /* shift-click within window does nothing */
        return;

    if ((obj == root) || (obj == NIL))
    {
        /* deselect all objects */
        act_allchg(wh, root, obj, &gl_rfull, pc, FALSE);
    }
    else
    {
        state = tree[obj].ob_state;
        if (!shifted)
        {
            if (dclick || !(state & SELECTED))
            {
                /* deselect all objects & select one */
                act_allchg(wh, root, obj, &gl_rfull, pc, FALSE);
                state |= SELECTED;
            }
        }
        else
        {
            state ^= SELECTED;
        }
        act_chg(wh, root, obj, pc, state & SELECTED, TRUE);
    }
}


/*
 *  Button stayed down over the specified screen objects
 */
WORD act_bdown(WORD wh, WORD root, WORD *in_mx, WORD *in_my,
               WORD *keystate, GRECT *pc, WORD *pdobj)
{
    OBJECT *tree = G.g_screen;
    WORD sobj;
    WORD numobs, button;
    WORD dst_wh;
    WORD l_mx, l_my, l_mw, l_mh;
    WORD dulx = -1, duly = -1, offx = 0, offy = 0;
    WORD numpts, view;
    Point *obpts, *pxypts;
    GRECT m;

    dst_wh = NIL;
    *pdobj = root;
    l_mx = *in_mx;
    l_my = *in_my;
    sobj = gr_obfind(tree, root, l_mx, l_my);

    /* rubber box to enclose a group of icons */
    if ((sobj == root) || (sobj == NIL))
    {
        graf_rubbox(l_mx, l_my, SHRT_MIN, SHRT_MIN, &l_mw, &l_mh);
        if (l_mw < 0)       /* mouse moved leftwards */
        {
            l_mx += l_mw;   /* adjust origin and width */
            l_mw = -l_mw;
        }
        if (l_mh < 0)       /* mouse moved upwards */
        {
            l_my += l_mh;   /* adjust origin and height */
            l_mh = -l_mh;
        }
        r_set(&m, l_mx, l_my, l_mw, l_mh);
        act_allchg(wh, root, sobj, &m, pc, TRUE);
    }
    else
    {       /* drag icon(s) */
        if (tree[sobj].ob_state & SELECTED)
        {
            obpts = gr_accobs(tree, root, &numobs);
            if (numobs)
            {
                view = (root == DROOT) ? V_ICON : G.g_iview;
                if (view == V_ICON)
                {
                    numpts = NUM_ICON_POINTS;
                    pxypts = G.g_xyicon;
                }
                else
                {
                    numpts = NUM_TEXT_POINTS;
                    pxypts = G.g_xytext;
                }
                gr_drgplns(l_mx, l_my, &gl_rfull, numpts, pxypts, numobs, obpts,
                            &dulx, &duly, &offx, &offy, &dst_wh, pdobj);
                if (dst_wh)
                {
                    /* cancel drag if it ends up in same window over itself */
                    if ((wh == dst_wh) && (*pdobj == sobj))
                        dst_wh = NIL;
                }
                else
                {
                    if ((wh == DESKWH) && (*pdobj == root)) /* Dragging from desktop */
                    {
                        move_drvicon(tree, root, dulx, duly, obpts, offx, offy);
                        dst_wh = NIL;
                    }
                }
                dos_free(obpts);
            }
        }
    }

    evnt_button(0x01, 0x01, 0x00, &l_mx, &l_my, &button, keystate);

    /* return keystate to caller */
    *in_mx = dulx - offx;           /* pass back the adjusted dest. x,y */
    *in_my = duly - offy;
    return dst_wh;
}
