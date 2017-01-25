/*      DESKOBJ.C       06/11/84 - 02/08/85             Lee Lorenzen    */
/*      merge source    5/27/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
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
#include "gemdos.h"

#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskbind.h"

#include "aesbind.h"
#include "deskglob.h"
#include "deskobj.h"
#include "gembind.h"
#include "kprint.h"


static const OBJECT gl_sampob[2] =
{
    { NIL, NIL, NIL, G_IBOX, NONE, NORMAL, 0x0L, 0, 0, 0, 0 },
    { NIL, NIL, NIL, G_BOX,  NONE, NORMAL, 0x00001100L, 0, 0, 0, 0 }
};


/*
 *  Allocate memory for screen objects
 *
 *  We try to allocate sufficient memory so that, with all windows open
 *  and the desktop full of icons, everything can be displayed
 *
 *  Returns the number of entries in the object array
 */
static WORD sob_malloc(void)
{
    LONG mem, limit, num_obs;
    BYTE *p;

    /* We need to calculate how many objects can be displayed in a
     * maximum-sized window.  Because icons take less display space,
     * this is the same as the number of icons that can be displayed
     * in such a window.  There is no single wind_get() call to get
     * the workarea size for a maximum-size window; we would need to
     * create one, do a wind_get(), and then delete it, which is
     * time-consuming.
     *
     * Instead, the following apparently-crude approximation provides
     * accurate values for all ST & Falcon resolutions, and should be
     * adequate for other resolutions too.  We use gl_wchar as a proxy
     * for the scroll-bar width, and gl_hchar as a proxy for the title
     * bar/info line height.  Rounding down takes care of the rest :-).
     */
    num_obs = (NUM_WNODES+1) * ((G.g_wdesk-gl_wchar)/G.g_iwspc) * ((G.g_hdesk-gl_hchar)/G.g_ihspc);

    /* In case we're memory-constrained, we limit ourselves to 5% of
     * available memory, or MIN_WOBS, whichever is greater.  In practice,
     * this should not be a restriction.
     */
    mem = dos_avail_anyram();
    limit = mem / (20*(sizeof(OBJECT)+sizeof(SCREENINFO)));
    if (limit < MIN_WOBS)
        limit = MIN_WOBS;
    if (limit < num_obs)
        num_obs = limit;

    num_obs += WOBS_START;      /* allow for root, desktop, windows */
    mem = num_obs * (sizeof(OBJECT)+sizeof(SCREENINFO));

    p = dos_alloc_anyram(mem);
    if (!p)
    {
        KDEBUG(("insufficient memory for %ld screen objects\n",num_obs));
        nomem_alert();          /* halt and catch fire */
    }

    G.g_screen = (OBJECT *)p;
    G.g_screeninfo = (SCREENINFO *)(G.g_screen+num_obs);

    return num_obs;
}


/*
 *  Initialize all objects in the G.g_screen[] array
 *
 *  . every non-item object is initialized by setting the links to NIL
 *  . all item objects are chained from g_screenfree
 *  . the root object is initialized as a G_IBOX object covering the
 *    entire screen
 *  . the objects that represent the desktop and desktop windows
 *    (objects 1->NUM_WNODES+1) are initialized as zero-size G_IBOX
 *    objects and made children of the root object
 */
void obj_init(void)
{
    WORD ii, num_sobs;
    OBJECT *obj;

    num_sobs = sob_malloc();
    KDEBUG(("obj_init(): allocated %d screen objects\n",num_sobs));

    /* initialize non-item objects */
    for (ii = 0, obj = G.g_screen; ii < WOBS_START; ii++, obj++)
    {
        obj->ob_head = NIL;
        obj->ob_next = NIL;
        obj->ob_tail = NIL;
    }

    /* put all item objects on the free chain */
    for ( ; ii < num_sobs-1; ii++, obj++)
    {
        obj->ob_next = ii + 1;
    }
    obj->ob_next = NIL;         /* last object marker */
    G.g_screenfree = WOBS_START;

    G.g_screen[ROOT] = gl_sampob[0];
    r_set((GRECT *)&G.g_screen[ROOT].ob_x, 0, 0, gl_width, gl_height);

    for (ii = 0, obj = G.g_screen+DROOT; ii < (NUM_WNODES+1); ii++, obj++)
    {
        *obj = gl_sampob[1];
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
 *  The children (if any) are freed by moving them to the head of
 *  the free chain and NILing out the ob_head/ob_tail pointers of
 *  the parent.
 *
 *  The window object may be freed at the same time by setting the
 *  w/h function arguments to zeros; otherwise it remains allocated
 *  and may be re-used (this optimises the common situation of
 *  freeing and immediately reallocating).
 */
void obj_wfree(WORD obj, WORD x, WORD y, WORD w, WORD h)
{
    OBJECT *window = &G.g_screen[obj];
    OBJECT *item;
    WORD ii, oldfree;

    r_set((GRECT *)&window->ob_x, x, y, w, h);

    if (window->ob_head >= WOBS_START)      /* there are children */
    {
        oldfree = G.g_screenfree;
        G.g_screenfree = window->ob_head;   /* update start of free chain */
        for (ii = window->ob_head; ; ii = item->ob_next)
        {
            item = &G.g_screen[ii];
            if (item->ob_next < WOBS_START)
            {
                item->ob_next = oldfree;    /* link to previous chain */
                break;
            }
        }
    }

    window->ob_head = window->ob_tail = NIL;
}


/*
 *  Find and allocate an item object at x/y/w/h; the next free object
 *  is taken from the free chain.
 *
 *  Returns number of object allocated, or 0 if no objects available
 */
WORD obj_ialloc(WORD wparent, WORD x, WORD y, WORD w, WORD h)
{
    WORD objnum;
    OBJECT *obj;

    objnum = G.g_screenfree;

    if (objnum >= WOBS_START)
    {
        obj = G.g_screen + objnum;
        G.g_screenfree = obj->ob_next;
        obj->ob_next = obj->ob_head = obj->ob_tail = NIL;
        objc_add(G.g_screen, wparent, objnum);
        r_set((GRECT *)&obj->ob_x, x, y, w, h);
        return objnum;
    }

    KDEBUG(("obj_ialloc(): no item objects available\n"));

    return 0;
}
