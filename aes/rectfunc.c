/*
 *       Copyright 2002 The EmuTOS development team
 *
 *       This software is licenced under the GNU Public License.
 *       Please see LICENSE.TXT for further information.
 */

#include "config.h"
#include "portab.h"
#include "obdefs.h"

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
