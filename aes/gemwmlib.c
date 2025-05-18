/*      GEMWMLIB.C      4/23/84 - 07/11/85      Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */
/*      fix wm_delete bug                       10/8/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2025 The EmuTOS development team
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
#include "gemlib.h"

#include "gemaplib.h"
#include "gemflag.h"
#include "gemoblib.h"
#include "gemwrect.h"
#include "geminit.h"
#include "gemfmlib.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemgsxif.h"
#include "gemobjop.h"
#include "gemctrl.h"
#include "gem_rsc.h"
#include "rectfunc.h"

#include "string.h"
#include "intmath.h"
#include "asm.h"

/*
 *  defines
 */
#define DROP_SHADOW_SIZE    2   /* size of drop shadow on windows */

GLOBAL WORD     gl_wtop;
GLOBAL OBJECT   *gl_awind;

static OBJECT *gl_newdesk;      /* current desktop background pattern */
static WORD gl_newroot;         /* current object within gl_newdesk   */

static OBJECT W_TREE[NUM_WIN];
static OBJECT W_ACTIVE[NUM_ELEM];


static const WORD gl_watype[NUM_ELEM] =
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
};

static const LONG gl_waspec[NUM_ELEM] =
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
};

#if CONF_WITH_3D_OBJECTS
static const WORD gl_3dflags[NUM_ELEM] =
{
    FL3DNONE,       /* W_BOX        */
    FL3DNONE,       /* W_TITLE      */
    FL3DACT,        /* W_CLOSER     */
    FL3DNONE,       /* W_NAME - may be altered by w_bldactive() */
    FL3DACT,        /* W_FULLER     */
    FL3DNONE,       /* W_INFO       */
    FL3DNONE,       /* W_DATA       */
    FL3DNONE,       /* W_WORK       */
    FL3DACT,        /* W_SIZER      */
    FL3DNONE,       /* W_VBAR       */
    FL3DACT,        /* W_UPARROW    */
    FL3DACT,        /* W_DNARROW    */
    FL3DNONE,       /* W_VSLIDE     */
    FL3DACT,        /* W_VELEV      */
    FL3DNONE,       /* W_HBAR       */
    FL3DACT,        /* W_LFARROW    */
    FL3DACT,        /* W_RTARROW    */
    FL3DNONE,       /* W_HSLIDE     */
    FL3DACT         /* W_HELEV      */
};

static const char empty_name[] = "";
#endif

#if CONF_WITH_WINDOW_COLOURS
/*
 * default TEDINFO colour words for window gadgets
 */
static WORD gl_wtcolor[NUM_ELEM];   /* when window is topped */
static WORD gl_wbcolor[NUM_ELEM];   /* when window is untopped */
#endif

static TEDINFO gl_aname;
static TEDINFO gl_ainfo;

/*
 * te_color defines for topped/untopped window names
 *
 * for both, border colour = 1, text colour = 1
 */
#define TOPPED_COLOR    0x11a1      /* opaque, fill pattern 2, fill colour 1 */
#define UNTOPPED_COLOR  0x1100      /* transparent, hollow, fill colour 0 */

/* initialisation values for gl_aname, gl_ainfo */
static const TEDINFO gl_asamp =
{
    NULL, NULL, NULL, IBM, 0, TE_LEFT, UNTOPPED_COLOR, 0, 1, 80, 80
};

static WORD wind_msg[8];

static OBJECT *gl_wtree;

#if CONF_WITH_3D_OBJECTS
/*
 * adjusted width & height of window elements
 */
static WORD adj_wbox;
static WORD adj_hbox;
#endif


static void w_nilit(WORD num, OBJECT olist[])
{
    while(num--)
    {
        olist[num].ob_next = olist[num].ob_head = olist[num].ob_tail = NIL;
    }
}


/*
 *  Routine to add a child object to a parent object.  The child
 *  is added at the end of the parent's current sibling list.
 *  It is also initialized.
 */
static void w_obadd(OBJECT olist[], WORD parent, WORD child)
{
    WORD lastkid;

    if ((parent != NIL) && (child != NIL))
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
    WINDOW *pwin;
    WORD i;
    MAYBE_UNUSED(i);

    pwin = &D.w_win[w_handle];
    pwin->w_owner = ppd;
    pwin->w_flags = VF_INUSE;
    pwin->w_kind = kind;
    pwin->w_pname = "";
    pwin->w_pinfo = "";
    pwin->w_hslide = pwin->w_vslide = 0;    /* slider at left/top   */
    pwin->w_hslsiz = pwin->w_vslsiz = -1;   /* use default size     */

#if CONF_WITH_WINDOW_COLOURS
    /* set default window object colours */
    for (i = 0; i < NUM_ELEM; i++)
    {
        pwin->w_tcolor[i] = gl_wtcolor[i];
        pwin->w_bcolor[i] = gl_wbcolor[i];
    }
#endif
}


static GRECT *w_getxptr(WORD which, WORD w_handle)
{
    /* FIXME: Probably remove the GRECT typecasts in this function */
    WINDOW *pwin = &D.w_win[w_handle];

    switch(which)
    {
    case WS_CURR:
    case WS_TRUE:
        return (GRECT *)&W_TREE[w_handle].ob_x;
    case WS_PREV:
        return &pwin->w_prev;
    case WS_WORK:
        return &pwin->w_work;
    case WS_FULL:
        return &pwin->w_full;
    }

    return NULL;
}


/* Get the size (x,y,w,h) of the window */
void w_getsize(WORD which, WORD w_handle, GRECT *pt)
{
    rc_copy(w_getxptr(which, w_handle), pt);
    if ((which == WS_TRUE) && pt->g_w && pt->g_h)
    {
        pt->g_w += DROP_SHADOW_SIZE;
        pt->g_h += DROP_SHADOW_SIZE;
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
    w_obadd(W_ACTIVE, parent, obj);
}


/*
 *  Walk the list and draw the parts of the window tree owned by this window
 */
static void do_walk(WORD wh, OBJECT *tree, WORD obj, WORD depth, GRECT *pc)
{
    ORECT   *po;
    GRECT   t;

    if (wh == NIL)
        return;

    /* clip to screen */
    if (pc)
        rc_intersect(&gl_rfull, pc);
    else
        pc = &gl_rfull;

    /* walk owner rectangle list */
    for (po = D.w_win[wh].w_rlist; po; po = po->o_link)
    {
        rc_copy(&po->o_gr, &t);
        /* intersect owner rectangle with clip rectangles */
        if (rc_intersect(pc, &t))
        {
            /* set clip and draw */
            gsx_sclip(&t);
            ob_draw(tree, obj, depth);
        }
    }
}


static WORD w_top(void)
{
    return (gl_wtop != NIL) ? gl_wtop : DESKWH;
}


void w_setactive(void)
{
    GRECT   d;
    WORD    wh;
    AESPD   *ppd;

    wh = w_top();
    w_getsize(WS_WORK, wh, &d);
    ppd = D.w_win[wh].w_owner;

    /* BUGFIX 2.1: don't chg own if null */
    if (ppd != NULL)
        ct_chgown(ppd, &d);
}


/*
 *  Draw the desktop background pattern underneath the current set of windows
 */
void w_drawdesk(GRECT *pc)
{
    OBJECT  *tree;
    WORD    depth;
    WORD    root;

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

    do_walk(DESKWH, tree, root, depth, pc);
}


static void w_cpwalk(WORD wh, WORD obj, WORD depth, BOOL usetrue)
{
    GRECT   c;

    /* start with window's true size as clip */
    if (usetrue)
        w_getsize(WS_TRUE, wh, &c);
    else
    {
        gsx_gclip(&c);      /* use global clip */
        c.g_w += DROP_SHADOW_SIZE;  /* add in drop shadow   */
        c.g_h += DROP_SHADOW_SIZE;
    }

    w_bldactive(wh);
    do_walk(wh, gl_awind, obj, depth, &c);
}


static void w_setcolor(WINDOW *pw, WORD gadget, BOOL istop)
{
#if CONF_WITH_WINDOW_COLOURS
    WORD color;
    OBJECT *obj;

    color = istop ? pw->w_tcolor[gadget] : pw->w_bcolor[gadget];

    obj = &W_ACTIVE[gadget];
    if ((obj->ob_type&0xff) == G_BOXTEXT)
        ((TEDINFO *)(obj->ob_spec))->te_color = color;
    else
        obj->ob_spec = (obj->ob_spec & 0xffff0000L) | (UWORD)color;
#endif
}


#if CONF_WITH_3D_OBJECTS
/*
 * main routines to build a window with 3D objects
 */
static void w_bldvbar(UWORD kind, BOOL istop, WINDOW *pw, WORD x, WORD y, WORD w, WORD h)
{
    WORD size, posn;

    w_setcolor(pw, W_VBAR, istop);
    w_adjust(W_DATA, W_VBAR, x, y, adj_wbox, h);

    y = 0;

    if (kind & UPARROW)
    {
        w_setcolor(pw, W_UPARROW, istop);
        w_adjust(W_VBAR, W_UPARROW, ADJ3DSTD, y+ADJ3DSTD, gl_wbox, gl_hbox);
        y += adj_hbox;      /* adjust y for VSLIDE */
        h -= adj_hbox;      /* adjust h for VSLIDE */
    }

    if (kind & DNARROW)
    {
        h -= adj_hbox;      /* adjust h for DNARROW, VSLIDE */
        w_setcolor(pw, W_DNARROW, istop);
        w_adjust(W_VBAR, W_DNARROW, ADJ3DSTD, y+h+ADJ3DSTD-1, gl_wbox, gl_hbox);
    }

    if (kind & VSLIDE)
    {
        w_setcolor(pw, W_VSLIDE, istop);
        w_adjust(W_VBAR, W_VSLIDE, 0, y, adj_wbox, h);
        if (pw->w_vslsiz == -1)
            size = gl_hbox;
        else size = max(gl_hbox, mul_div(h, pw->w_vslsiz, 1000));
        posn = mul_div(h-size, pw->w_vslide, 1000);
        w_setcolor(pw, W_VELEV, istop);
        w_adjust(W_VSLIDE, W_VELEV, ADJ3DSTD, posn+ADJ3DSTD, gl_wbox, size-2*ADJ3DSTD-1);
    }
}


static void w_bldhbar(UWORD kind, BOOL istop, WINDOW *pw, WORD x, WORD y, WORD w, WORD h)
{
    WORD size, posn;

    w_setcolor(pw, W_HBAR, istop);
    w_adjust(W_DATA, W_HBAR, x, y, w, adj_hbox);

    x = 0;

    if (kind & LFARROW)
    {
        w_setcolor(pw, W_LFARROW, istop);
        w_adjust(W_HBAR, W_LFARROW, x+ADJ3DSTD, ADJ3DSTD, gl_wbox, gl_hbox);
        x += adj_wbox;      /* adjust x for HSLIDE */
        w -= adj_wbox;      /* adjust w for HSLIDE */
    }

    if (kind & RTARROW)
    {
        w -= adj_wbox;      /* adjust w for RTARROW, HSLIDE */
        w_setcolor(pw, W_RTARROW, istop);
        w_adjust(W_HBAR, W_RTARROW, x+w-1+ADJ3DSTD, ADJ3DSTD, gl_wbox, gl_hbox);
    }

    if (kind & HSLIDE)
    {
        w_setcolor(pw, W_HSLIDE, istop);
        w_adjust(W_HBAR, W_HSLIDE, x, 0, w, adj_hbox);
        if (pw->w_hslsiz == -1)
            size = gl_wbox;
        else size = max(gl_wbox, mul_div(w, pw->w_hslsiz, 1000));
        posn = mul_div(w-size, pw->w_hslide, 1000);
        w_setcolor(pw, W_HELEV, istop);
        w_adjust(W_HSLIDE, W_HELEV, posn+ADJ3DSTD, ADJ3DSTD, size-2*ADJ3DSTD-1, gl_hbox);
    }
}


void w_bldactive(WORD w_handle)
{
    BOOL    istop, havevbar, havehbar;
    WORD    kind;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
    WORD    corner_x, corner_y;
#pragma GCC diagnostic pop
    GRECT   t;
    WORD    tempw;
    WINDOW  *pw;

    if (w_handle == NIL)
        return;

    pw = &D.w_win[w_handle];

    istop = (gl_wtop == w_handle);  /* set if it is on top */
    kind = pw->w_kind;              /* get the kind of window */
    w_nilit(NUM_ELEM, W_ACTIVE);

    /* start adding pieces & adjusting sizes */
    gl_aname.te_ptext = (kind & NAME) ? pw->w_pname : (char *)empty_name;
    gl_ainfo.te_ptext = pw->w_pinfo;
    gl_aname.te_just = TE_CNTR;

    w_getsize(WS_CURR, w_handle, &t);
    W_ACTIVE[W_BOX].ob_x = t.g_x;
    W_ACTIVE[W_BOX].ob_y = t.g_y;
    W_ACTIVE[W_BOX].ob_width = t.g_w;
    W_ACTIVE[W_BOX].ob_height = t.g_h;
    w_setcolor(pw, W_BOX, istop);

    /* do title area */
    t.g_x = t.g_y = 0;
    if (kind & TGADGETS)
    {
        w_setcolor(pw, W_TITLE, istop);
        w_adjust(W_BOX, W_TITLE, t.g_x, t.g_y, t.g_w, adj_hbox);
        tempw = t.g_w;
        if (kind & CLOSER)
        {
            w_setcolor(pw, W_CLOSER, istop);
            w_adjust(W_TITLE, W_CLOSER, t.g_x+ADJ3DSTD, t.g_y+ADJ3DSTD, gl_wbox, gl_hbox);
            t.g_x += adj_wbox;
            tempw -= adj_wbox;
        }
        if (kind & FULLER)
        {
            w_setcolor(pw, W_FULLER, istop);
            tempw -= adj_wbox;
            w_adjust(W_TITLE, W_FULLER, t.g_x+tempw+ADJ3DSTD, t.g_y+ADJ3DSTD, gl_wbox, gl_hbox);
        }
        if (kind & NAME)
        {
            w_setcolor(pw, W_NAME, istop);
            tempw -= 2 * ADJ3DSTD;
            w_adjust(W_TITLE, W_NAME, t.g_x+ADJ3DSTD, t.g_y+ADJ3DSTD, tempw, gl_hbox);
            W_ACTIVE[W_NAME].ob_state = istop ? NORMAL : DISABLED;
#if !CONF_WITH_WINDOW_COLOURS
            gl_aname.te_color = istop ? TOPPED_COLOR : UNTOPPED_COLOR;
#endif
        }
        else if (kind & MOVER)
        {
            /*
             * in TOS4, you may specify a MOVER gadget with no NAME gadget
             */
            tempw -= 2 * ADJ3DSTD;
            w_adjust(W_TITLE, W_NAME, t.g_x+ADJ3DSTD, t.g_y+ADJ3DSTD, tempw, gl_hbox);
            W_ACTIVE[W_NAME].ob_state = istop ? NORMAL : DISABLED;
            gl_aname.te_color = 0;
        }

        W_ACTIVE[W_NAME].ob_flags = (kind & NAME) ? FL3DACT : FL3DNONE;

        t.g_x = 0;
        t.g_y += adj_hbox;
        t.g_h -= adj_hbox;
    }

    /* do info area */
    if (kind & INFO)
    {
        w_setcolor(pw, W_INFO, istop);
        w_adjust(W_BOX, W_INFO, t.g_x, t.g_y, t.g_w, gl_hbox);
        t.g_y += gl_hbox;
        t.g_h -= gl_hbox;
    }

    /* do data area */
    w_adjust(W_BOX, W_DATA, t.g_x, t.g_y, t.g_w, t.g_h);

    /* determine whether we should display sizer & vertical/horizontal bars */
    havevbar = (kind & VGADGETS) ? TRUE : FALSE;
    havehbar = (kind & HGADGETS) ? TRUE : FALSE;

    /*
     * If we have a SIZER, we must draw a vertical bar and/or a horizontal
     * bar.  Atari TOS versions 1-3 always drew both, but TOS4 only draws
     * the one(s) containing other window elements.  If no other window
     * elements are present, it draws a vertical bar.  We do the same.
     */

    /* if we have both bars or a SIZER, we draw a corner element */
    corner_x = -1;          /* assume no corner (SIZER or dummy) */
    if ((havehbar && havevbar) || (kind & SIZER))
    {
        corner_x = t.g_w - gl_wbox - ADJ3DSTD;
        corner_y = t.g_h - gl_hbox - ADJ3DSTD;
    }
    /* if we have a SIZER but no bars, we draw a vertical bar */
    if ((!havevbar && !havehbar) && (kind & SIZER))
        havevbar = TRUE;

    /* do work area */
    t.g_x = t.g_y = 1;      /* allow a 1-pixel border */
    t.g_w -= 2;
    t.g_h -= 2;
    if (havevbar || (kind & SIZER))
        t.g_w -= (adj_wbox - 1);
    if (havehbar || (kind & SIZER))
        t.g_h -= (adj_hbox - 1);

    w_adjust(W_DATA, W_WORK, t.g_x, t.g_y, t.g_w, t.g_h);

    /* do vertical bar area */
    if (havevbar)
    {
        t.g_x += t.g_w;
        w_bldvbar(kind, istop, pw, t.g_x, 0, t.g_w+2, t.g_h+2);
    }

    /* do horizontal bar area */
    if (havehbar)
    {
        t.g_y += t.g_h;
        w_bldhbar(kind, istop, pw, 0, t.g_y, t.g_w+2, t.g_h+2);
    }

    /* do sizer area */
    if (corner_x >= 0)
    {
        w_setcolor(pw, W_SIZER, istop);
        w_adjust(W_DATA, W_SIZER, corner_x, corner_y, gl_wbox, gl_hbox);
        /* we only display the sizer indicator if we're really a SIZER */
        W_ACTIVE[W_SIZER].ob_spec &= 0x00ffffffL;   /* remove gadget char */
        if (kind & SIZER)
            W_ACTIVE[W_SIZER].ob_spec |= 0x06000000L;
    }
}
#else
/*
 * main routines to build a window with old-style (non-3D) objects
 */
static void w_bldvbar(UWORD kind, BOOL istop, WINDOW *pw, WORD x, WORD y, WORD w, WORD h)
{
    WORD size, posn;

    /* set window widget colours according to topped/untopped status */
    w_setcolor(pw, W_VBAR, istop);
    w_setcolor(pw, W_UPARROW, istop);
    w_setcolor(pw, W_DNARROW, istop);
    w_setcolor(pw, W_VSLIDE, istop);
    w_setcolor(pw, W_VELEV, istop);

    w_adjust(W_DATA, W_VBAR, x, y, gl_wbox, h);
    x = y = 0;
    if (istop)
    {
        if (kind & UPARROW)
        {
            w_adjust(W_VBAR, W_UPARROW, x, y, gl_wbox, gl_hbox);
            y += (gl_hbox - 1);
            h -= (gl_hbox - 1);
        }
        if (kind & DNARROW)
        {
            w -= (gl_wbox - 1);
            h -= (gl_hbox - 1);
            w_adjust(W_VBAR, W_DNARROW, x, y + h - 1, gl_wbox, gl_hbox);
        }
        if (kind & VSLIDE)
        {
            w_adjust(W_VBAR, W_VSLIDE, x, y, gl_wbox, h);

            if (pw->w_vslsiz == -1)
                size = gl_hbox;
            else size = max(gl_hbox, mul_div_round(h, pw->w_vslsiz, 1000));
            posn = mul_div_round(h-size, pw->w_vslide, 1000);
            w_adjust(W_VSLIDE, W_VELEV, 0, posn, gl_wbox, size);
        }
    }
}


static void w_bldhbar(UWORD kind, BOOL istop, WINDOW *pw, WORD x, WORD y, WORD w, WORD h)
{
    WORD size, posn;

    /* set window widget colours according to topped/untopped status */
    w_setcolor(pw, W_HBAR, istop);
    w_setcolor(pw, W_LFARROW, istop);
    w_setcolor(pw, W_RTARROW, istop);
    w_setcolor(pw, W_HSLIDE, istop);
    w_setcolor(pw, W_HELEV, istop);

    w_adjust(W_DATA, W_HBAR, x, y, w, gl_hbox);
    x = y = 0;
    if (istop)
    {
        if (kind & LFARROW)
        {
            w_adjust(W_HBAR, W_LFARROW, x, y, gl_wbox, gl_hbox);
            x += (gl_wbox - 1);
            w -= (gl_wbox - 1);
        }
        if (kind & RTARROW)
        {
            w -= (gl_wbox - 1);
            h -= (gl_hbox - 1);
            w_adjust(W_HBAR, W_RTARROW, x + w - 1, y, gl_wbox, gl_hbox);
        }
        if (kind & HSLIDE)
        {
            w_adjust(W_HBAR, W_HSLIDE, x, y, w, gl_hbox);

            if (pw->w_hslsiz == -1)
                size = gl_wbox;
            else size = max(gl_wbox, mul_div_round(w, pw->w_hslsiz, 1000));
            posn = mul_div_round(w-size, pw->w_hslide, 1000);
            w_adjust(W_HSLIDE, W_HELEV, posn, 0, size, gl_hbox);
        }
    }
}


void w_bldactive(WORD w_handle)
{
    BOOL    istop;
    WORD    kind;
    WORD    havevbar;
    WORD    havehbar;
    WORD    sizer_x, sizer_y;
    GRECT   t;
    WORD    tempw;
    WINDOW  *pw;

    if (w_handle == NIL)
        return;

    pw = &D.w_win[w_handle];

    istop = (gl_wtop == w_handle);  /* set if it is on top */
    kind = pw->w_kind;              /* get the kind of window */
    w_nilit(NUM_ELEM, W_ACTIVE);

    /* start adding pieces & adjusting sizes */
    gl_aname.te_ptext = pw->w_pname;
    gl_ainfo.te_ptext = pw->w_pinfo;
    gl_aname.te_just = TE_CNTR;

    w_getsize(WS_CURR, w_handle, &t);
    W_ACTIVE[W_BOX].ob_x = t.g_x;
    W_ACTIVE[W_BOX].ob_y = t.g_y;
    W_ACTIVE[W_BOX].ob_width = t.g_w;
    W_ACTIVE[W_BOX].ob_height = t.g_h;
    w_setcolor(pw, W_BOX, istop);

    /* do title area */
    t.g_x = t.g_y = 0;
    if (kind & (NAME|CLOSER|FULLER))
    {
        w_setcolor(pw, W_TITLE, istop);
        w_adjust(W_BOX, W_TITLE, t.g_x, t.g_y, t.g_w, gl_hbox);
        tempw = t.g_w;
        if (kind & CLOSER)
        {
            w_setcolor(pw, W_CLOSER, istop);
            if (istop)
            {
                w_adjust(W_TITLE, W_CLOSER, t.g_x, t.g_y, gl_wbox, gl_hbox);
                t.g_x += gl_wbox;
                tempw -= gl_wbox;
            }
        }
        if (kind & FULLER)
        {
            w_setcolor(pw, W_FULLER, istop);
            if (istop)
            {
                tempw -= gl_wbox;
                w_adjust(W_TITLE, W_FULLER, t.g_x+tempw, t.g_y, gl_wbox, gl_hbox);
            }
        }
        if (kind & NAME)
        {
            w_setcolor(pw, W_NAME, istop);
            w_adjust(W_TITLE, W_NAME, t.g_x, t.g_y, tempw, gl_hbox);
            W_ACTIVE[W_NAME].ob_state = istop ? NORMAL : DISABLED;
#if !CONF_WITH_WINDOW_COLOURS
            gl_aname.te_color = istop ? TOPPED_COLOR : UNTOPPED_COLOR;
#endif
        }
        t.g_x = 0;
        t.g_y += (gl_hbox - 1);
        t.g_h -= (gl_hbox - 1);
    }

    /* do info area */
    if (kind & INFO)
    {
        w_setcolor(pw, W_INFO, istop);
        w_adjust(W_BOX, W_INFO, t.g_x, t.g_y, t.g_w, gl_hbox);
        t.g_y += (gl_hbox - 1);
        t.g_h -= (gl_hbox - 1);
    }

    /* do data area */
    w_adjust(W_BOX, W_DATA, t.g_x, t.g_y, t.g_w, t.g_h);

    /* determine whether we should display sizer & vertical/horizontal bars */
    havevbar = kind & (UPARROW | DNARROW | VSLIDE | SIZER);
    havehbar = kind & (LFARROW | RTARROW | HSLIDE | SIZER);

    /* remember sizer area (if needed) */
    sizer_x = -1;           /* assume no sizer */
    if (havevbar && havehbar)
    {
        sizer_x = t.g_w - gl_wbox;
        sizer_y = t.g_h - gl_hbox;
    }

    /* do work area */
    t.g_x = t.g_y = 1;      /* allow a 1-pixel border */
    t.g_w -= 2;
    t.g_h -= 2;
    if (havevbar)
        t.g_w -= (gl_wbox - 1);
    if (havehbar)
        t.g_h -= (gl_hbox - 1);

    w_adjust(W_DATA, W_WORK, t.g_x, t.g_y, t.g_w, t.g_h);

    /* do vertical bar area */
    if (havevbar)
    {
        t.g_x += t.g_w;
        w_bldvbar(kind, istop, pw, t.g_x, 0, t.g_w+2, t.g_h+2);
    }

    /* do horizontal bar area */
    if (havehbar)
    {
        t.g_y += t.g_h;
        w_bldhbar(kind, istop, pw, 0, t.g_y, t.g_w+2, t.g_h+2);
    }

    /* do sizer area */
    if (sizer_x >= 0)
    {
        w_setcolor(pw, W_SIZER, istop);
        w_adjust(W_DATA, W_SIZER, sizer_x, sizer_y, gl_wbox, gl_hbox);
        /* we only display the sizer indicator if we're topped */
        W_ACTIVE[W_SIZER].ob_spec &= 0x00ffffffL;   /* remove gadget char */
        if (istop && (kind & SIZER))
            W_ACTIVE[W_SIZER].ob_spec |= 0x06000000L;
    }
}
#endif


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
    ap_rdwr(MU_SDMSG, towhom, 16, ap_msg);
}


/*
 *  Walk down ORECT list and accumulate the union of all the owner rectangles
 */
static WORD w_union(ORECT *po, GRECT *pt)
{
    if (!po)
        return FALSE;

    rc_copy(&po->o_gr, pt);

    po = po->o_link;
    while (po)
    {
        rc_union(&po->o_gr, pt);
        po = po->o_link;
    }

    return TRUE;
}


static void w_redraw(WORD w_handle, GRECT *pt)
{
    GRECT   t, d;
    AESPD   *ppd;

    ppd = D.w_win[w_handle].w_owner;

    /* make sure work rect and word rect intersect */
    rc_copy(pt, &t);
    w_getsize(WS_WORK, w_handle, &d);
    if (rc_intersect(&t, &d))
    {
        /* make sure window owns a rectangle */
        if (w_union(D.w_win[w_handle].w_rlist, &d))
        {
            /* intersect redraw rect with union of owner rects */
            if (rc_intersect(&d, &t))
                ap_sendmsg(wind_msg, WM_REDRAW, ppd, w_handle, t.g_x, t.g_y, t.g_w, t.g_h);
        }
    }
}


/*
 *  Routine to fix rectangles in preparation for a source to destination
 *  blit.  If the source is at -1, then the source and destination left
 *  fringes need to be realigned.
 */
static BOOL w_mvfix(GRECT *ps, GRECT *pd)
{
    WORD tmpsx;

    tmpsx = ps->g_x;
    rc_intersect(&gl_rfull, ps);
    if (tmpsx == -1)
    {
        pd->g_x++;
        pd->g_w--;
        return TRUE;
    }

    return FALSE;
}


/*
 *  Call to move top window.  This involves BLTing the window if none
 *  of it that is partially off the screen needs to be redrawn, else
 *  the whole desktop is just updated.  All uncovered portions of the
 *  desktop are redrawn by later calling w_update.
 */
static BOOL w_move(WORD w_handle, WORD *pstop, GRECT *prc)
{
    GRECT   s;      /* source */
    GRECT   d;      /* destination */
    GRECT   *pc;
    BOOL    sminus1, dminus1;

    w_getsize(WS_PREV, w_handle, &s);
    s.g_w += DROP_SHADOW_SIZE;
    s.g_h += DROP_SHADOW_SIZE;
    w_getsize(WS_TRUE, w_handle, &d);

    /* set flags for when part of the source is off the screen */
    if ( ((s.g_x+s.g_w > gl_width) && (d.g_x < s.g_x))  ||
         ((s.g_y+s.g_h > gl_height) && (d.g_y < s.g_y)) )
    {
        rc_union(&s, &d);
        *pstop = DESKWH;
    }
    else
    {
        *pstop = w_handle;
    }

    /* intersect with full screen and align fringes if -1 xpos */
    sminus1 = w_mvfix(&s, &d);
    dminus1 = w_mvfix(&d, &s);

    /* blit what we can */
    if (*pstop == w_handle)
    {
        gsx_sclip(&gl_rfull);
        bb_screen(s.g_x, s.g_y, d.g_x, d.g_y, s.g_w, s.g_h);
        /* cleanup left edge */
        if (sminus1 != dminus1)
        {
            if (dminus1)
                s.g_x--;
            if (sminus1)
            {
                d.g_x--;
                d.g_w = 1;
                gsx_sclip(&d);
                w_cpwalk(gl_wtop, 0, 0, FALSE);
            }
        }
        pc = &s;
    }
    else
    {
        pc = &d;
    }

    /* clean up the rest by returning clip rect */
    rc_copy(pc, prc);

    return (*pstop == w_handle);
}


/*
 *  Draw windows from top to bottom.  If top is 0, then start at the topmost
 *  window.  If bottom is 0, then start at the bottomost window.  For the
 *  first window drawn, just do the insides, since DRAW_CHANGE has already
 *  drawn the outside borders.
 */
void w_update(WORD bottom, GRECT *pt, WORD top, BOOL moved)
{
    WORD   i, ni;
    BOOL   done;

    /* limit to screen */
    rc_intersect(&gl_rfull, pt);
    gsx_moff();

    /* update windows from top to bottom */
    if (bottom == DESKWH)
        bottom = W_TREE[ROOT].ob_head;

    /* if there are windows */
    if (bottom != NIL)
    {
        /* start at the top */
        if (top == DESKWH)
            top = W_TREE[ROOT].ob_tail;
        /* draw windows from top to bottom */
        do {
            if (!(moved && (top == gl_wtop)))
            {
                /* set clip and draw a window's border  */
                gsx_sclip(pt);
                w_cpwalk(top, 0, MAX_DEPTH, FALSE); /* let appl. draw inside*/
                w_redraw(top, pt);
            }
            /* scan to find prev */
            i = bottom;
            done = (i == top);
            while (i != top)
            {
                ni = W_TREE[i].ob_next;
                if (ni == top)
                    top = i;
                else
                    i = ni;
            }
        }
        while(!done);
    }

    gsx_mon();
}


/*
 *  Draw the tree of windows given a major change in some window.  It
 *  may have been sized, moved, fulled, topped, or closed.  An attempt
 *  should be made to minimize the amount of redrawing of other windows
 *  that has to occur.  W_REDRAW() will actually issue window redraw
 *  requests based on the rectangle that needs to be cleaned up.
 */
static void draw_change(WORD w_handle, GRECT *pt)
{
    GRECT   c, pprev;
    GRECT   *pw;
    WORD    start, stop;
    BOOL    moved;
    WORD    oldtop, clrold, wasclr;

    wasclr = !(D.w_win[w_handle].w_flags & VF_BROKEN);

    /* save old size */
    w_getsize(WS_CURR, w_handle, &c);
    w_setsize(WS_PREV, w_handle, &c);

    /* set new sizes */
    w_setsize(WS_CURR, w_handle, pt);
    pw = w_getxptr(WS_WORK, w_handle);
    wm_calc(WC_WORK, D.w_win[w_handle].w_kind,
                pt->g_x, pt->g_y, pt->g_w, pt->g_h,
                &pw->g_x, &pw->g_y, &pw->g_w, &pw->g_h);

    /*
     * if this window isn't open, we're done (we must not attempt
     * to redraw the window tree).  this situation can occur when
     * a wind_set(WF_CURRXYWH) is done for a window that has been
     * created but not yet opened.
     */
    if ((D.w_win[w_handle].w_flags & VF_ISOPEN) == 0)
        return;

    /* update rectangle lists */
    everyobj(gl_wtree, ROOT, NIL, (EVERYOBJ_CALLBACK)newrect, 0, 0, MAX_DEPTH);

    /* remember oldtop & set new one */
    oldtop = gl_wtop;
    gl_wtop = W_TREE[ROOT].ob_tail;

    /* set ctrl rect and mouse owner */
    w_setactive();
    start = w_handle;       /* init. starting window */
    stop = DESKWH;          /* stop at the top */

    /* set flag to say we haven't moved the top window */
    moved = FALSE;

    /*
     * if same upper left corner, and not a zero size window,
     * then it's a size or top request; otherwise it's a move,
     * grow, open or close
     */
    if ((!rc_equal(&gl_rzero, pt)) && (pt->g_x == c.g_x) && (pt->g_y == c.g_y))
    {
        /* size or top request */
        if ((pt->g_w == c.g_w) && (pt->g_h == c.g_h))
        {
            /* handle top request (sizes of prev and current are the same) */
            /* return if this isn't a top request */
            if ((w_handle != W_TREE[ROOT].ob_tail) || (w_handle == oldtop))
                return;

            /* draw oldtop covered with deactivated borders */
            if (oldtop != NIL)
            {
                w_cpwalk(oldtop, 0, MAX_DEPTH, TRUE);
                clrold = !(D.w_win[oldtop].w_flags & VF_BROKEN);
            }
            else
                clrold = TRUE;

            /*
             * if oldtop isn't overlapped and new top was clear,
             * then just draw activated borders
             */
            if (clrold && wasclr)
            {
                w_cpwalk(gl_wtop, 0, MAX_DEPTH, TRUE);
                return;
            }
        }
        else
        {   /* handle size request */

            /* stop before current window if shrink was a pure subset */
            if ((pt->g_w <= c.g_w) && (pt->g_h <= c.g_h))
            {
                stop = w_handle;
                w_cpwalk(gl_wtop, 0, MAX_DEPTH, TRUE);
                moved = TRUE;
            }

            /* start at bottom if a shrink occurred */
            if ((pt->g_w < c.g_w) || (pt->g_h < c.g_h))
                start = DESKWH;

            /* update rect. is the union of two sizes + the drop shadow */
            c.g_w = max(pt->g_w, c.g_w) + DROP_SHADOW_SIZE;
            c.g_h = max(pt->g_h, c.g_h) + DROP_SHADOW_SIZE;
        }
    }
    else
    {
        /* move or grow or open or close */
        if (!(c.g_w && c.g_h) ||
            ((pt->g_x <= c.g_x) &&
             (pt->g_y <= c.g_y) &&
             (pt->g_x+pt->g_w >= c.g_x+c.g_w) &&
             (pt->g_y+pt->g_h >= c.g_y+c.g_h)) )
        {
            /* a grow that is a superset or an open */
            rc_copy(pt, &c);
        }
        else
        {
            /* move, close or shrink */

            /* do a move of top guy */
            if ((pt->g_w == c.g_w) && (pt->g_h == c.g_h) && (gl_wtop == w_handle))
            {
                moved = w_move(w_handle, &stop, &c);
                start = DESKWH;
            }

            /* check for a close */
            if (!(pt->g_w && pt->g_h))
                start = DESKWH;

            /* handle other moves and shrinks */
            if (start != DESKWH)
            {
                rc_union(pt, &c);
                if (!rc_equal(pt, &c))
                    start = DESKWH;
            }
        }
    }

    /* update gl_wtop after close, or open */
    if (oldtop != W_TREE[ROOT].ob_tail)
    {
        if (gl_wtop != NIL)
        {
            /* open or close with other windows open */
            w_getsize(WS_CURR, gl_wtop, pt);
            rc_union(pt, &c);
            /* if it was an open, then draw the old top guy covered */
            if ((oldtop != NIL) && (oldtop != w_handle))
            {
                /* only an open if prev size was zero */
                w_getsize(WS_PREV, gl_wtop, &pprev);
                if (rc_equal(&pprev, &gl_rzero))
                    w_cpwalk(oldtop, 0, MAX_DEPTH, TRUE);
            }
        }
    }

    /* account for drop shadow (BUGFIX in 2.1) */
    c.g_w += DROP_SHADOW_SIZE;
    c.g_h += DROP_SHADOW_SIZE;

    /* update the desktop background */
    if (start == DESKWH)
        w_drawdesk(&c);

    /* start the redrawing  */
    w_update(start, &c, stop, moved);
}


/*
 *  Walk down ORECT list looking for the next rect that still has
 *  size when clipped with the passed in clip rectangle
 */
static void w_owns(WINDOW *pwin, ORECT *po, GRECT *pt, GRECT *poutwds)
{
    while (po)
    {
        rc_copy(&po->o_gr, poutwds);
        pwin->w_rnext = po = po->o_link;
        if (rc_intersect(pt, poutwds))
            return;
    }

    poutwds->g_w = poutwds->g_h = 0;
}


/*
 *  (Re)initialize window manager internal variables, excluding window colours
 */
void wm_init(void)
{
    WORD    i;
    ORECT   *po;
    OBJECT  *tree;
    AESPD   *ppd;

    /* init default owner to be current process */
    ppd = rlr;

    /* init owner rectangles */
    or_start();

    /* init window extent objects */
    bzero(W_TREE, NUM_WIN * sizeof(OBJECT));
    w_nilit(NUM_WIN, W_TREE);
    for (i = 0; i < NUM_WIN; i++)
    {
        D.w_win[i].w_flags = 0x0;
        D.w_win[i].w_rlist = NULL;
        W_TREE[i].ob_type = G_IBOX;
    }
    W_TREE[ROOT].ob_type = G_BOX;
    tree = rs_trees[DESKTOP];
    W_TREE[ROOT].ob_spec = tree->ob_spec;

    /* init window element objects */
    bzero(W_ACTIVE, NUM_ELEM * sizeof(OBJECT));
    w_nilit(NUM_ELEM, W_ACTIVE);
    for (i = 0; i < NUM_ELEM; i++)
    {
        W_ACTIVE[i].ob_type = gl_watype[i];
        W_ACTIVE[i].ob_spec = gl_waspec[i];
#if CONF_WITH_3D_OBJECTS
        W_ACTIVE[i].ob_flags = gl_3dflags[i];
#endif
    }
    W_ACTIVE[ROOT].ob_state = SHADOWED;

    /* init rectangle list */
    D.w_win[0].w_rlist = po = get_orect();
    po->o_link = NULL;
    rc_copy(&gl_rfull, &po->o_gr);
    w_setup(ppd, DESKWH, NONE);
    w_setsize(WS_CURR, DESKWH, &gl_rscreen);
    w_setsize(WS_PREV, DESKWH, &gl_rscreen);
    w_setsize(WS_FULL, DESKWH, &gl_rfull);
    w_setsize(WS_WORK, DESKWH, &gl_rfull);

    /* init global variables */
    gl_wtop = NIL;
    gl_wtree = W_TREE;
    gl_awind = W_ACTIVE;
    gl_newdesk = NULL;

    /* init tedinfo parts of title and info lines */
    gl_aname = gl_asamp;
    gl_ainfo = gl_asamp;
    gl_aname.te_just = TE_CNTR;
    W_ACTIVE[W_NAME].ob_spec = (LONG)&gl_aname;
    W_ACTIVE[W_INFO].ob_spec = (LONG)&gl_ainfo;

#if CONF_WITH_3D_OBJECTS
    /* set up box width & height for window building */
    adj_wbox = gl_wbox + 2 * ADJ3DSTD;
    adj_hbox = gl_hbox + 2 * ADJ3DSTD;

    /*
     * the following mimics TOS4 behaviour and ensures that the work area
     * of a full-screen window is the same as TOS4.  this is necessary to
     * allow 4 columns of icons in a full-screen window in ST Low.
     */
    if (gl_width < 640)
    {
        adj_wbox--;
        if (gl_height >= 400)   /* e.g. 320 x 400 */
            adj_wbox--;
    }
#endif
}


/*
 *  Start the window manager up by initializing all internal variables
 */
void wm_start(void)
{
#if CONF_WITH_WINDOW_COLOURS
    WORD i;

    /*
    * Initialise default window gadget colours, as follows:
    *  W_NAME (topped)     0x11a1 (border black, text black, fill opaque, pattern 2)
    *  W_VSLIDE (topped)   0x1111 (border black, text black, fill transparent, pattern 1)
    *  W_HSLIDE (topped)   0x1111 (border black, text black, fill transparent, pattern 1)
    * For 3D object support only:
    *  W_VSLIDE (untopped) 0x1111 (border black, text black, fill transparent, pattern 1)
    *  W_HSLIDE (untopped) 0x1111 (border black, text black, fill transparent, pattern 1)
    * All others:          0x1101 (border black, text black, fill transparent, pattern 0)
    */
    for (i = 0; i < NUM_ELEM; i++)
    {
        gl_wtcolor[i] = gl_wbcolor[i] = 0x1101;
    }

    gl_wtcolor[W_NAME] |= 0xa0;
    gl_wtcolor[W_VSLIDE] |= 0x10;
    gl_wtcolor[W_HSLIDE] |= 0x10;
#if CONF_WITH_3D_OBJECTS
    gl_wbcolor[W_VSLIDE] |= 0x10;
    gl_wbcolor[W_HSLIDE] |= 0x10;
#endif
#endif

    wm_init();          /* initialise the remaining variables */
}


/*
 *  Allocates a window for the calling application of the appropriate
 *  size and returns a window handle
 */
WORD wm_create(WORD kind, GRECT *pt)
{
    WORD i;

    for (i = 0; (i < NUM_WIN) && (D.w_win[i].w_flags & VF_INUSE); i++)
        ;
    if (i < NUM_WIN)
    {
        w_setup(rlr, i, kind);
        w_setsize(WS_CURR, i, &gl_rzero);
        w_setsize(WS_PREV, i, &gl_rzero);
        w_setsize(WS_FULL, i, pt);
        return i;
    }

    return -1;
}


/*
 *  Return pointer to AES window structure for created window
 */
static WINDOW *get_pwin(WORD w_handle)
{
    WINDOW *pwin;

    if ((w_handle < 0) || (w_handle >= NUM_WIN))
        return NULL;

    pwin = &D.w_win[w_handle];

    if (!(pwin->w_flags & VF_INUSE))
        return NULL;

    return pwin;
}


/*
 *  Opens a window from a created but closed state
 */
BOOL wm_open(WORD w_handle, GRECT *pt)
{
    WINDOW *pwin;
    GRECT   t;

    pwin = get_pwin(w_handle);
    if (!pwin)                      /* window doesn't exist */
        return FALSE;
    if (pwin->w_flags & VF_ISOPEN)  /* window is already open */
        return FALSE;

    rc_copy(pt, &t);
    wm_update(BEG_UPDATE);

    pwin->w_flags |= VF_ISOPEN;
    w_obadd(W_TREE, ROOT, w_handle);
    draw_change(w_handle, &t);
    w_setsize(WS_PREV, w_handle, pt);

    wm_update(END_UPDATE);

    return TRUE;
}


/*
 *  Closes a window from an open state
 */
BOOL wm_close(WORD w_handle)
{
    WINDOW *pwin;
    GRECT   t;

    pwin = get_pwin(w_handle);
    if (!pwin)                          /* window doesn't exist */
        return FALSE;
    if (!(pwin->w_flags & VF_ISOPEN))   /* window isn't open */
        return FALSE;

    rc_copy(&gl_rzero, &t);
    wm_update(BEG_UPDATE);

    ob_delete(gl_wtree, w_handle);
    draw_change(w_handle, &t);
    pwin->w_flags &= ~VF_ISOPEN;

    wm_update(END_UPDATE);

    return TRUE;
}


/*
 *  Frees a window and its handle up for use by another application
 *  or by the same application
 */
BOOL wm_delete(WORD w_handle)
{
    WINDOW *pwin;

    pwin = get_pwin(w_handle);
    if (!pwin)                      /* window doesn't exist */
        return FALSE;
    if (pwin->w_flags & VF_ISOPEN)  /* window is open      */
        wm_close(w_handle);         /* - so close it first */

    newrect(gl_wtree, w_handle);      /* give back recs. */
    w_setsize(WS_CURR, w_handle, &gl_rscreen);
    w_setsize(WS_PREV, w_handle, &gl_rscreen);
    w_setsize(WS_FULL, w_handle, &gl_rfull);
    w_setsize(WS_WORK, w_handle, &gl_rfull);
    pwin->w_flags = 0;
    pwin->w_owner = NULL;

    return TRUE;
}


/*
 *  Gives information about the current window to the application that owns it
 *
 *  Note 1: the WF_COLOR and WF_DCOLOR modes were introduced in AES 3.30, and are
 *  not well-documented, so I'll add the documentation here:
 *
 *  wind_get(handle, WF_COLOR/WF_DCOLOR, &parm1, &parm2, &parm3, &parm4)
 *      input:  handle is the window handle (ignored for WF_DCOLOR)
 *              mode is WF_COLOR or WF_DCOLOR
 *              parm1 contains the number of the gadget (W_BOX etc)
 *      output: parm2 contains the obspec colour word when the window is topped
 *              parm3 contains the obspec colour word when the window is untopped
 *
 *  Note 2: the application program binding for wind_get(WF_COLOR/WF_DCOLOR) is
 *  a special case since there are three intin[] values, rather than the two
 *  used for all other wind_get() functions.
 */
BOOL wm_get(WORD w_handle, WORD w_field, WORD *poutwds, WORD *pinwds)
{
    WORD    which, gadget;
    GRECT   t;
    ORECT   *po;
    WINDOW  *pwin;
    MAYBE_UNUSED(gadget);

    pwin = get_pwin(w_handle);

    switch(w_field)
    {
    case WF_TOP:                    /* these don't require a valid handle */
    case WF_SCREEN:
    case WF_DCOLOR:
        break;
    default:
        if (!pwin)                  /* window doesn't exist */
            return FALSE;
    }

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
        poutwds[0] = pwin->w_hslide;
        break;
    case WF_VSLIDE:
        poutwds[0] = pwin->w_vslide;
        break;
    case WF_HSLSIZ:
        poutwds[0] = pwin->w_hslsiz;
        break;
    case WF_VSLSIZ:
        poutwds[0] = pwin->w_vslsiz;
        break;
    case WF_TOP:
        poutwds[0] = w_top();
        break;
    case WF_FIRSTXYWH:
    case WF_NEXTXYWH:
        w_getsize(WS_WORK, w_handle, &t);
        po = (w_field == WF_FIRSTXYWH) ? pwin->w_rlist : pwin->w_rnext;
        /* FIXME: GRECT typecasting again */
        w_owns(pwin, po, &t, (GRECT *)poutwds);
        break;
    case WF_SCREEN:
        gsx_mret((LONG *)poutwds, (LONG *)(poutwds+2));
        break;
#if CONF_WITH_WINDOW_COLOURS
    case WF_COLOR:
        gadget = pinwds[0];
        poutwds[1] = pwin->w_tcolor[gadget];
        poutwds[2] = pwin->w_bcolor[gadget];
        break;
    case WF_DCOLOR:
        gadget = pinwds[0];
        poutwds[1] = gl_wtcolor[gadget];
        poutwds[2] = gl_wbcolor[gadget];
        break;
#endif
    default:
        return FALSE;
    }

    if (which != -1)
        w_getsize(which, w_handle, (GRECT *)poutwds);

    return TRUE;
}


/*
 *  Routine to top a window and then make the right redraws happen
 */
static void wm_mktop(WORD w_handle)
{
    GRECT   t, p;

    ob_order(gl_wtree, w_handle, NIL);
    w_getsize(WS_PREV, w_handle, &p);
    w_getsize(WS_CURR, w_handle, &t);
    draw_change(w_handle, &t);
    w_setsize(WS_PREV, w_handle, &p);
}


/*
 *  Allows application to set the attributes of one of the windows that
 *  it currently owns.  Some of the information includes the name, and
 *  the scroll bar elevator positions.
 */

BOOL wm_set(WORD w_handle, WORD w_field, WORD *pinwds)
{
    BOOL    do_cpwalk = FALSE;
    BOOL    ret = TRUE;
    WORD    gadget = -1;
    WINDOW  *pwin;

    pwin = get_pwin(w_handle);

    if (w_field != WF_DCOLOR)   /* WF_DCOLOR doesn't require a valid handle */
    {
        if (!pwin)
            return FALSE;
    }

    wm_update(BEG_UPDATE);      /* grab the window sync */

    /*
     * validate input
     */
    switch(w_field)
    {
    case WF_HSLSIZ:
    case WF_VSLSIZ:
        if (pinwds[0] == -1)    /* means "use default size" */
            break;
        FALLTHROUGH;
    case WF_HSLIDE:
    case WF_VSLIDE:
        if (pinwds[0] < 1)
            pinwds[0] = 1;
        else if (pinwds[0] > 1000)
            pinwds[0] = 1000;
    }

    switch(w_field)
    {
    case WF_NAME:
        gl_aname.te_ptext = pwin->w_pname = *(char **)pinwds;
        if (pwin->w_flags & VF_ISOPEN)
        {
            gadget = W_NAME;
            do_cpwalk = TRUE;   /* update name applies to all open windows */
        }
        break;
    case WF_INFO:
        gl_ainfo.te_ptext = pwin->w_pinfo = *(char **)pinwds;
        if (pwin->w_flags & VF_ISOPEN)
        {
            gadget = W_INFO;
            do_cpwalk = TRUE;   /* update info line applies to all open windows */
        }
        break;
    case WF_CXYWH:
        draw_change(w_handle, (GRECT *)pinwds);
        break;
    case WF_TOP:
        if (w_handle != gl_wtop)
        {
            wm_mktop(w_handle);
        }
        break;
    case WF_NEWDESK:
        pwin->w_owner = rlr;
        gl_newdesk = *(OBJECT **) pinwds;
        gl_newroot = pinwds[2];
        break;
    case WF_HSLSIZ:
        if (pwin->w_hslsiz != pinwds[0])    /* size changed? */
        {
            pwin->w_hslsiz = pinwds[0];     /* yes, save new size */
            gadget = W_HSLIDE;              /* & redraw slider    */
        }
        break;
    case WF_VSLSIZ:
        if (pwin->w_vslsiz != pinwds[0])    /* size changed? */
        {
            pwin->w_vslsiz = pinwds[0];     /* yes, save new size */
            gadget = W_VSLIDE;              /* & redraw slider    */
        }
        break;
    case WF_HSLIDE:
        if (pwin->w_hslide != pinwds[0])    /* position changed? */
        {
            pwin->w_hslide = pinwds[0];     /* yes, save new posn */
            gadget = W_HSLIDE;              /* & redraw slider    */
        }
        break;
    case WF_VSLIDE:
        if (pwin->w_vslide != pinwds[0])    /* position changed? */
        {
            pwin->w_vslide = pinwds[0];     /* yes, save new posn */
            gadget = W_VSLIDE;              /* & redraw slider    */
        }
        break;
#if CONF_WITH_WINDOW_COLOURS
    case WF_COLOR:
        gadget = pinwds[0];
        if (pinwds[1] != -1)
            pwin->w_tcolor[gadget] = pinwds[1];
        if (pinwds[2] != -1)
            pwin->w_bcolor[gadget] = pinwds[2];
        do_cpwalk = TRUE;
        break;
    case WF_DCOLOR:
        gadget = pinwds[0];
        if (pinwds[1] != -1)
            gl_wtcolor[gadget] = pinwds[1];
        if (pinwds[2] != -1)
            gl_wbcolor[gadget] = pinwds[2];
        gadget = -1;            /* do not call w_cpwalk() in this case */
        break;
#endif
    default:
        ret = FALSE;
    }

    /*
     * if we've changed slider gadgets and they are visible, we must
     * update the window.  slider gadgets are always visible when 3D
     * object support is enabled; otherwise they are only visible if
     * the window is topped.
     */
#if !CONF_WITH_3D_OBJECTS
    if (w_handle == gl_wtop)
#endif
    {
        if ((gadget == W_HSLIDE) || (gadget == W_VSLIDE))
            do_cpwalk = TRUE;
    }

    if (do_cpwalk)
        w_cpwalk(w_handle, gadget, MAX_DEPTH, TRUE);

    wm_update(END_UPDATE);      /* give up the sync */

    return ret;
}


/*
 *  Given an x and y location, will figure out which window the mouse is in
 */

WORD wm_find(WORD x, WORD y)
{
    return ob_find(gl_wtree, 0, 2, x, y);
}


/*
 *  Locks or unlocks the current state of the window tree while an application
 *  is responding to a window update message in its message pipe, or is making
 *  some other direct screen update based on its current rectangle list.
 */
void wm_update(WORD beg_update)
{

    if (beg_update < 2)
    {
        if (beg_update)
        {
            if (!tak_flag(&wind_spb))
                ev_block(MU_MUTEX, (LONG)&wind_spb);

        }
        else
            unsync(&wind_spb);
    }
    else
    {
        beg_update -= 2;
        fm_own(beg_update);
    }
}


/*
 *  Given a width and height of a Work Area and the Kind of window desired,
 *  calculate the required window size including the Border Area.
 *  OR
 *  Given the width and height of a window including the Border Area and the
 *  Kind of window desired, calculate the result size of the window Work Area.
 */
void wm_calc(WORD wtype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
             WORD *px, WORD *py, WORD *pw, WORD *ph)
{
    WORD tb, bb, lb, rb;
#if CONF_WITH_3D_OBJECTS
    WORD havevbar, havehbar;
#endif

    tb = bb = rb = lb = 1;

#if CONF_WITH_3D_OBJECTS
    if (kind & TGADGETS)
        tb += (adj_hbox - 1);

    if (kind & INFO)
        tb += gl_hbox;

    /* determine how sizer & vertical/horizontal bars affect boundary */
    havevbar = kind & (VGADGETS|SIZER);
    havehbar = kind & (HGADGETS|SIZER);
    if (havevbar && havehbar)
    {
        /* the same as we do in w_bldactive() */
        havevbar &= ~SIZER;
        havehbar &= ~SIZER;
        if (!havevbar && !havehbar)
            havevbar |= SIZER;
    }

    if (havevbar)
        rb += (adj_wbox - 1);

    if (havehbar)
        bb += (adj_hbox - 1);
#else
    if (kind & (NAME|CLOSER|FULLER))
        tb += (gl_hbox - 1);

    if (kind & INFO)
        tb += (gl_hbox - 1);

    if (kind & (UPARROW|DNARROW|VSLIDE|SIZER))
        rb += (gl_wbox - 1);

    if (kind & (LFARROW|RTARROW|HSLIDE|SIZER))
        bb += (gl_hbox - 1);
#endif

    /* negate values to calc Border Area */
    if (wtype == WC_BORDER)
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
 *  This function deletes _ALL_ windows and clears all locks that have been
 *  done with wind_update()
 */
void wm_new(void)
{
    int wh;

    /* Remove locks: */
    while(ml_ocnt > 0)
        wm_update(END_MCTRL);
    while(wind_spb.sy_tas > 0)
        wm_update(END_UPDATE);

    /* Delete windows: */
    for (wh = 1; wh < NUM_WIN; wh++)
    {
        if (D.w_win[wh].w_flags & VF_ISOPEN)
            wm_close(wh);
        if (D.w_win[wh].w_flags & VF_INUSE)
            wm_delete(wh);
    }
}


#if CONF_WITH_3D_OBJECTS
/*
 * redraw selected portion of desktop
 *
 * this is used by the control manager to animate 3D objects
 */
void w_redraw_desktop(GRECT *pt)
{
    GRECT t, c;
    OBJECT *tree;
    WORD root;
    WORD curr[4];   /* current rectangle */

    t = *pt;

    if (gl_newdesk)
    {
        tree = gl_newdesk;
        root = gl_newroot;
    }
    else
    {
        tree = rs_trees[DESKTOP];
        root = ROOT;
    }

    /* prevent interference */
    gsx_moff();
    wm_update(BEG_UPDATE);

    wm_get(DESKWH, WF_FIRSTXYWH, curr, NULL);

    /* process until rectangle list is done */
    while(curr[2] && curr[3])
    {
        r_set(&c, curr[0], curr[1], curr[2], curr[3]);
        if (rc_intersect(&t, &c))
        {
            gsx_sclip(&c);
            ob_draw(tree, root, MAX_DEPTH);
        }
        wm_get(DESKWH, WF_NEXTXYWH, curr, NULL);
    }

    /* back to normal */
    wm_update(END_UPDATE);
    gsx_mon();
}
#endif
