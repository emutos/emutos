/*
 *       Copyright 2002-2015 The EmuTOS development team
 *
 *       This software is licenced under the GNU Public License.
 *       Please see LICENSE.TXT for further information.
 */

#include "config.h"
#include "portab.h"
#include "obdefs.h"
#include "rectfunc.h"


/*--------------------------------------*/
/*      inside                          */
/* determine if x,y is in rectangle     */
/*--------------------------------------*/
UWORD inside(WORD x, WORD y, GRECT *pt)
{
        if ( (x >= pt->g_x) && (y >= pt->g_y) &&
            (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h) )
                return(TRUE);
        else
                return(FALSE);
} /* inside */

/*----------------------------------------*/
/*      rc_constrain                      */
/* constrain one rectangle within another */
/*----------------------------------------*/
void rc_constrain(GRECT *pc, GRECT *pt)
{
        if (pt->g_x < pc->g_x)
            pt->g_x = pc->g_x;
        if (pt->g_y < pc->g_y)
            pt->g_y = pc->g_y;
        if ((pt->g_x + pt->g_w) > (pc->g_x + pc->g_w))
            pt->g_x = (pc->g_x + pc->g_w) - pt->g_w;
        if ((pt->g_y + pt->g_h) > (pc->g_y + pc->g_h))
            pt->g_y = (pc->g_y + pc->g_h) - pt->g_h;
}

/*--------------------------------------*/
/*      rc_copy                         */
/* copy source to destination rectangle */
/*--------------------------------------*/
void rc_copy(GRECT *psbox, GRECT *pdbox)
{
        pdbox->g_x = psbox->g_x;
        pdbox->g_y = psbox->g_y;
        pdbox->g_w = psbox->g_w;
        pdbox->g_h = psbox->g_h;
}

/*--------------------------------------*/
/*      rc_equal                        */
/* tests for two rectangles equal       */
/*--------------------------------------*/
WORD rc_equal(GRECT *p1, GRECT *p2)
{
        if ((p1->g_x != p2->g_x) ||
            (p1->g_y != p2->g_y) ||
            (p1->g_w != p2->g_w) ||
            (p1->g_h != p2->g_h))
                return(FALSE);
        return(TRUE);
}

/*-----------------------------------------*/
/*      rc_intersect                       */
/* find the intersection of two rectangles */
/*-----------------------------------------*/
WORD rc_intersect(GRECT *p1, GRECT *p2)
{
        WORD tx, ty, tw, th;

        tw = min(p2->g_x + p2->g_w, p1->g_x + p1->g_w);
        th = min(p2->g_y + p2->g_h, p1->g_y + p1->g_h);
        tx = max(p2->g_x, p1->g_x);
        ty = max(p2->g_y, p1->g_y);

        p2->g_x = tx;
        p2->g_y = ty;
        p2->g_w = tw - tx;
        p2->g_h = th - ty;

        return( (tw > tx) && (th > ty) );
}

/*----------------------------------*/
/*      rc_union                    */
/* find the union of two rectangles */
/*----------------------------------*/
void rc_union(GRECT *p1, GRECT *p2)
{
        WORD tx, ty, tw, th;

        tw = max(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
        th = max(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
        tx = min(p1->g_x, p2->g_x);
        ty = min(p1->g_y, p2->g_y);

        p2->g_x = tx;
        p2->g_y = ty;
        p2->g_w = tw - tx;
        p2->g_h = th - ty;
}

/*------------------------------*/
/*      min                     */
/* return min of two values     */
/*------------------------------*/
WORD min(WORD a, WORD b)
{
        return( (a < b) ? a : b );
}

/*------------------------------*/
/*      max                     */
/* return max of two values     */
/*------------------------------*/
WORD max(WORD a, WORD b)
{
        return( (a > b) ? a : b );
}
