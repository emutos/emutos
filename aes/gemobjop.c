/*      GEMOBJOP.C      03/15/84 - 05/27/85     Gregg Morris            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */ 
/*      fix get_par                             11/12/87        mdf     */

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
#include "obdefs.h"
#include "taddr.h"
#include "kprint.h"


LONG obaddr(LONG tree, WORD obj, WORD fld_off)
{
        return( (tree + ((obj) * sizeof(OBJECT) + fld_off)) );
}


BYTE ob_sst(LONG tree, WORD obj, LONG *pspec, WORD *pstate, WORD *ptype,
            WORD *pflags, GRECT *pt, WORD *pth)
{
        WORD            th;
        OBJECT          *objptr = ((OBJECT *)tree) + obj;
        TEDINFO         *ted;

        pt->g_w = objptr->ob_width;
        pt->g_h = objptr->ob_height;
        *pflags = objptr->ob_flags;
        *pspec = objptr->ob_spec;
        if (objptr->ob_flags & INDIRECT)
          *pspec = *(LONG *)objptr->ob_spec;

        *pstate = objptr->ob_state;
        *ptype = objptr->ob_type & 0x00ff;
        th = 0;
        switch( *ptype )
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
                if ( objptr->ob_flags & EXIT)
                  th--;
                if ( objptr->ob_flags & DEFAULT)
                  th--;
                break;
        }
        if (th > 128)
          th -= 256;
        *pth = th;
        return *(BYTE *)pspec;  /* only useful for G_BOXCHAR */
}


void everyobj(LONG tree, WORD this, WORD last, void (*routine)(),
              WORD startx, WORD starty, WORD maxdep)
{
        register WORD   tmp1;
        register WORD   depth;
        WORD            x[8], y[8];
        OBJECT          *obj;

        x[0] = startx;
        y[0] = starty;
        depth = 1;
                                                /* non-recursive tree   */
                                                /*   traversal          */
child:
                                                /* see if we need to    */
                                                /*   to stop            */
        if ( this == last)
          return;
                                                /* do this object       */
        obj = ((OBJECT *)tree) + this;
        x[depth] = x[depth-1] + obj->ob_x;
        y[depth] = y[depth-1] + obj->ob_y;
        (*routine)(tree, this, x[depth], y[depth]);
                                                /* if this guy has kids */
                                                /*   then do them       */
        tmp1 = obj->ob_head;
        if ( tmp1 != NIL )
        {
          if ( !( obj->ob_flags & HIDETREE ) && 
                ( depth <= maxdep ) )
          {
            depth++;
            this = tmp1;
            goto child;
          }
        }
sibling:
                                                /* if this is the root  */
                                                /*   which has no parent*/
                                                /*   or it is the last  */
                                                /*   then stop else...  */
        obj = ((OBJECT *)tree) + this;
        tmp1 = obj->ob_next;
        if ( (tmp1 == last) ||
             (this == ROOT) )   
          return;
                                                /* if this obj. has a   */
                                                /*   sibling that is not*/
                                                /*   his parent, then   */
                                                /*   move to him and do */
                                                /*   him and his kids   */
        obj = ((OBJECT *)tree) + tmp1;
        if ( obj->ob_tail != this )
        {
          this = tmp1;
          goto child;
        }
                                                /* else move up to the  */
                                                /*   parent and finish  */
                                                /*   off his siblings   */ 
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
WORD get_par(LONG tree, WORD obj, WORD *pnobj)
{
    register WORD   pobj;
    register WORD   nobj;

    pobj = obj;
    nobj = NIL;
    if (obj == ROOT)
        pobj = NIL;
    else
    {
        do
        {
            obj = pobj;
            pobj = LWGET(OB_NEXT(obj));
            if (nobj == NIL)
                nobj = pobj;
        } while ( (pobj >= ROOT) && (LWGET(OB_TAIL(pobj)) != obj) );
    }
    *pnobj = nobj;
    return(pobj);
} /* get_par */



