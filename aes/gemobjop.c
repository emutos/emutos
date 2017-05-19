/*      GEMOBJOP.C      03/15/84 - 05/27/85     Gregg Morris            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */
/*      fix get_par                             11/12/87        mdf     */

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

#include "config.h"
#include "portab.h"
#include "obdefs.h"
#include "kprint.h"
#include "gemobjop.h"


BYTE ob_sst(OBJECT *tree, WORD obj, LONG *pspec, WORD *pstate, WORD *ptype,
            WORD *pflags, GRECT *pt, WORD *pth)
{
    WORD    th;
    OBJECT  *objptr = tree + obj;
    TEDINFO *ted;

    pt->g_w = objptr->ob_width;
    pt->g_h = objptr->ob_height;
    *pflags = objptr->ob_flags;
    *pspec = objptr->ob_spec;
    if (objptr->ob_flags & INDIRECT)
        *pspec = *(LONG *)objptr->ob_spec;

    *pstate = objptr->ob_state;
    *ptype = objptr->ob_type & 0x00ff;
    th = 0;
    switch(*ptype)
    {
    case G_TITLE:
        th = 1;
        break;
    case G_TEXT:
    case G_BOXTEXT:
    case G_FTEXT:
    case G_FBOXTEXT:
        ted = (TEDINFO *)*pspec;
        th = ted->te_thickness;
        break;
    case G_BOX:
    case G_BOXCHAR:
    case G_IBOX:
        th = *(((BYTE *)pspec)+1);
        break;
    case G_BUTTON:
        th--;
        if (objptr->ob_flags & EXIT)
            th--;
        if (objptr->ob_flags & DEFAULT)
            th--;
        break;
    }

    if (th > 128)
        th -= 256;
    *pth = th;

    return *(BYTE *)pspec;  /* only useful for G_BOXCHAR */
}


void everyobj(OBJECT *tree, WORD this, WORD last, EVERYOBJ_CALLBACK routine,
              WORD startx, WORD starty, WORD maxdep)
{
    WORD    tmp1;
    WORD    depth;
    WORD    x[MAX_DEPTH+2], y[MAX_DEPTH+2];
    OBJECT  *obj;

    x[0] = startx;
    y[0] = starty;
    depth = 1;

    /*
     * non-recursive tree traversal
     */
child:
    /* see if we need to stop */
    if (this == last)
        return;

    /* do this object */
    obj = tree + this;
    x[depth] = x[depth-1] + obj->ob_x;
    y[depth] = y[depth-1] + obj->ob_y;
    (*routine)(tree, this, x[depth], y[depth]);

    /* if this guy has kids then do them */
    tmp1 = obj->ob_head;
    if (tmp1 != NIL)
    {
        if (!(obj->ob_flags & HIDETREE) && (depth <= maxdep))
        {
            depth++;
            this = tmp1;
            goto child;
        }
    }

sibling:
    /*
     * if this is the root (which has no parent),
     *  or it is the last then stop else...
     */
    obj = tree + this;
    tmp1 = obj->ob_next;
    if ((tmp1 == last) || (this == ROOT))
        return;
    /*
     * if this object has a sibling that is not his parent,
     * then move to him and do him and his kids
     */
    obj = tree + tmp1;
    if (obj->ob_tail != this)
    {
        this = tmp1;
        goto child;
    }
    /* else move up to the parent and finish off his siblings */
    depth--;
    this = tmp1;
    goto sibling;
}


/*
 * Routine that will find the parent of a given object.  The
 * idea is to walk to the end of our siblings and return
 * our parent.  If object is the root then return NIL as parent.
 * Also have this routine return the immediate next object of
 * this object.
 */
WORD get_par(OBJECT *tree, WORD obj, WORD *pnobj)
{
    WORD    pobj = NIL, nobj = NIL;
    OBJECT  *objptr, *pobjptr;

    if (obj != ROOT)
    {
        while(1)
        {
            objptr = tree + obj;
            pobj = objptr->ob_next;
            if (nobj == NIL)        /* first time */
                nobj = pobj;
            if (pobj < ROOT)
                break;
            pobjptr = tree + pobj;
            if ( pobjptr->ob_tail == obj )
                break;
            obj = pobj;
        }
    }

    *pnobj = nobj;
    return pobj;
}
