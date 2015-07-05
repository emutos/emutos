/*      DESKOBJ.C       06/11/84 - 02/08/85             Lee Lorenzen    */
/*      merge source    5/27/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2015 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "string.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskbind.h"

#include "optimopt.h"
#include "aesbind.h"
#include "deskglob.h"
#include "deskobj.h"
#include "kprint.h"


static const OBJECT gl_sampob[2] =
{
    { NIL, NIL, NIL, G_IBOX, NONE, NORMAL, 0x0L, 0, 0, 0, 0 },
    { NIL, NIL, NIL, G_BOX,  NONE, NORMAL, 0x00001100L, 0, 0, 0, 0 }
};


/*
 *  Initialize all objects in the G.g_screen[] array
 *
 *  . every object is marked as unused by setting ob_next to NIL
 *  . the root object is initialized as a G_IBOX object covering the
 *    entire screen
 *  . the objects that represent the desktop and desktop windows
 *    (objects 1->NUM_WNODES+1) are initialized as zero-size G_IBOX
 *    objects and made children of the root object
 */
void obj_init(void)
{
    WORD ii;
    OBJECT *obj;

    for (ii = 0, obj = G.g_screen; ii < NUM_SOBS; ii++, obj++)
    {
        obj->ob_head = NIL;
        obj->ob_next = NIL;
        obj->ob_tail = NIL;
    }

    memcpy(&G.g_screen[ROOT], &gl_sampob[0], sizeof(OBJECT));
    r_set((GRECT *)&G.g_screen[ROOT].ob_x, 0, 0, gl_width, gl_height);

    for (ii = 0, obj = G.g_screen+DROOT; ii < (NUM_WNODES+1); ii++, obj++)
    {
        memcpy(obj, &gl_sampob[1], sizeof(OBJECT));
        objc_add(G.g_screen, ROOT, DROOT+ii);
    }
}


/*
 *  Allocate a window object from the screen tree by looking for one
 *  with zero width and/or height, starting at object 2.
 *
 *  Returns number of object allocated, or 0 if no objects available
 */
WORD obj_walloc(WORD x, WORD y, WORD w, WORD h)
{
    WORD ii;
    OBJECT *obj;

    for (ii = DROOT+1, obj = G.g_screen+ii; ii < WOBS_START; ii++, obj++)
    {
        if (!(obj->ob_width && obj->ob_height))
        {
            r_set((GRECT *)&obj->ob_x, x, y, w, h);
            return ii;
        }
    }
    KDEBUG(("obj_walloc(): no window objects available\n"));

    return 0;
}


/*
 *  Reset a window object's x/y/w/h & free all its children
 * 
 *  The children are freed by NILing out the ob_next pointers of the
 *  children and the ob_head/ob_tail pointers of the parent.
 *
 *  The window object may be freed at the same time by setting the
 *  w/h function arguments to zeros; otherwise it remains allocated
 *  and may be re-used (this optimises the common situation of
 *  freeing and immediately reallocating).
 */
void obj_wfree(WORD obj, WORD x, WORD y, WORD w, WORD h)
{
    WORD ii, nxtob;

    r_set((GRECT *)&G.g_screen[obj].ob_x, x, y, w, h);
    for (ii = G.g_screen[obj].ob_head; ii > obj; ii = nxtob)
    {
        nxtob = G.g_screen[ii].ob_next;
        G.g_screen[ii].ob_next = NIL;
    }
    G.g_screen[obj].ob_head = G.g_screen[obj].ob_tail = NIL;
}


/*
 *  Find and allocate an item object at x/y/w/h.  The next free object
 *  is found by starting at object WOBS_START and looking for any
 *  object that is available (i.e. that has a next pointer of NIL).
 *
 *  Returns number of object allocated, or 0 if no objects available
 */
WORD obj_ialloc(WORD wparent, WORD x, WORD y, WORD w, WORD h)
{
    WORD ii;
    OBJECT *obj;

    for (ii = WOBS_START, obj = G.g_screen+ii; ii < NUM_SOBS; ii++, obj++)
    {
        if (obj->ob_next == NIL)
        {
            obj->ob_head = obj->ob_tail = NIL;
            objc_add(G.g_screen, wparent, ii);
            r_set((GRECT *)&obj->ob_x, x, y, w, h);
            return ii;
        }
    }
    KDEBUG(("obj_ialloc(): no item objects available\n"));

    return 0;
}
