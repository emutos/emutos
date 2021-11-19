/*
 * gemmnext.c - support for AES 3.30 menu library extensions
 *
 * Copyright (C) 2021 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * this module provides minimal support for the new menu library functions
 * in AES 3.30: menu_popup(), menu_attach(), menu_istart(), menu_settings()
 *
 * the following restrictions apply, compared with the specifications
 * for AES 3.30:
 *  1.  menu_attach() may only attach submenus to the main menu bar.
 *      this means that there can only be one level of submenus from
 *      the main menu bar, and no submenus from popup menus.
 *  2.  the scroll flag is ignored.
 *
 * the only common programs that are known to use these functions under
 * plain TOS are Lattice C v5 & DevPac 3.10, both from HiSoft.  neither
 * use popups, submenus of submenus, or scrolling, so this implementation
 * should be adequate.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"

#if CONF_WITH_MENU_EXTENSION

#include "struct.h"
#include "obdefs.h"
#include "aesdefs.h"

#include "aesext.h"
#include "gemdos.h"
#include "gemlib.h"
#include "gemevlib.h"
#include "gemgrlib.h"
#include "gemgsxif.h"
#include "geminput.h"
#include "gemmnlib.h"
#include "gemmnext.h"
#include "gemoblib.h"
#include "gemwmlib.h"
#include "rectfunc.h"

static WORD keystate;

/*
 * values for 'settings' below
 */
#define INIT_DISPLAY_VALUE  200
#define INIT_DRAG_VALUE     10000
#define INIT_DELAY_VALUE    250
#define INIT_SPEED_VALUE    0
#define INIT_HEIGHT_VALUE   16
#define MIN_HEIGHT_VALUE    5

static MN_SET settings;

#define MENU_THICKNESS      1

/*
 * clamp the range of values for menu height
 */
static WORD clamp_height(WORD height)
{
    WORD max_height;

    if (height < MIN_HEIGHT_VALUE)
        return MIN_HEIGHT_VALUE;

    max_height = gl_rfull.g_h / gl_hchar;
    if (height > max_height)
        return max_height;

    return height;
}

/*
 * initialise menu library extension variables
 *
 * this must be called after workstation initialisation, since
 * clamp_height() uses gl_rfull which isn't set until that point
 */
void mnext_init(void)
{
    settings.display = INIT_DISPLAY_VALUE;
    settings.drag = INIT_DRAG_VALUE;
    settings.delay = INIT_DELAY_VALUE;
    settings.speed = INIT_SPEED_VALUE;
    settings.height = clamp_height(INIT_HEIGHT_VALUE);
}

/*
 * adjust the y position of an object so that it is all on-screen (if possible)
 */
static void clamp_ypos(OBJECT *obj)
{
    WORD y_limit;

    /* make sure that object doesn't extend past the bottom of the screen */
    y_limit = gl_rscreen.g_h - obj->ob_height;
    while(obj->ob_y > y_limit)
        obj->ob_y -= gl_hchar;

    /* even more importantly, it mustn't extend into the menu area */
    y_limit = gl_rfull.g_y;
    while(obj->ob_y < y_limit)
        obj->ob_y += gl_hchar;
}

/*
 * save or restore the screen underneath the popup
 *
 * if buf is NULL, we malloc a save area and copy to it
 * if buf is non-NULL, we copy from it and free the area
 */
static void *popup_blit(OBJECT *tree, void *buf)
{
    LONG bufsize;
    WORD pxyarray[8], *pts1, *pts2;
    FDB bufMFDB, screenMFDB, *psrc, *pdst;
    GRECT t;
    BOOL save = buf ? FALSE : TRUE;

    //do we need to set clip first?
    ob_actxywh(tree, ROOT, &t);
    t.g_x -= MENU_THICKNESS;
    t.g_y -= MENU_THICKNESS;
    t.g_w += 2 * MENU_THICKNESS + ADJ3DSTD;
    t.g_h += 2 * MENU_THICKNESS + ADJ3DSTD;

    rc_intersect(&gl_rfull, &t);
    gsx_sclip(&t);

    /* get save area */
    if (save)
    {
        bufsize = ((t.g_w + 15L) / 16L) * sizeof(WORD) * t.g_h * gl_nplanes;
        buf = dos_alloc_stram(bufsize);
        if (!buf)
        {
            gsx_sclip(&gl_rscreen);
            return NULL;
        }
    }

    /* set up the memory buffer MFDB */
    bufMFDB.fd_addr = buf;
    bufMFDB.fd_w = t.g_w;
    bufMFDB.fd_h = t.g_h;
    bufMFDB.fd_wdwidth = (t.g_w + 15L) / 16L;
    bufMFDB.fd_stand = 0;
    bufMFDB.fd_nplanes = gl_nplanes;
    bufMFDB.fd_r1 = bufMFDB.fd_r2 = bufMFDB.fd_r3 = 0;

    /* set up the screen MFDB */
    gsx_fix_screen(&screenMFDB);

    if (save)
    {
        psrc = &screenMFDB;
        pdst = &bufMFDB;
        pts1 = pxyarray;
        pts2 = pxyarray + 4;
    }
    else
    {
        psrc = &bufMFDB;
        pdst = &screenMFDB;
        pts1 = pxyarray + 4;
        pts2 = pxyarray;
    }

    pts1[0] = t.g_x;
    pts1[1] = t.g_y;
    pts1[2] = t.g_x + t.g_w - 1;
    pts1[3] = t.g_y + t.g_h - 1;
    pts2[0] = 0;
    pts2[1] = 0;
    pts2[2] = t.g_w - 1;
    pts2[3] = t.g_h - 1 ;

    /* do the blit */
    gsx_moff();
    vro_cpyfm(S_ONLY, pxyarray, psrc, pdst);
    gsx_mon();

    /* clean up */
    gsx_sclip(&gl_rscreen);
    if (save)
        return buf;

    dos_free(buf);

    return NULL;
}

/*
 * set up the mouse block
 */
static void setup_moblk(MOBLK *pmo, MENU *menu, WORD mx, WORD my)
{
    OBJECT *tree;
    WORD box, obj;

    tree = menu->mn_tree;
    box = menu->mn_menu;

    obj = ob_find(tree, box, 1, mx, my);

    if (obj < 0)        /* mouse isn't in popup */
    {
        pmo->m_out = 0; /* look for entry into enclosing box */
        rc_copy((GRECT *)&tree[box].ob_x, &pmo->m_gr);
    }
    else
    {
        pmo->m_out = 1; /* look for exit from item */
        ob_actxywh(tree, obj, &pmo->m_gr);
    }
}

/*
 * handle the popup
 */
static WORD handle_popup(MENU *menu)
{
    MOBLK m;
    OBJECT *tree;
    WORD mx, my, dummy, events;
    WORD obj, oldobj;
    WORD rets[6];

    tree = menu->mn_tree;
    obj = menu->mn_item;

    /* initially, mark start item as selected & draw entire box */
    if (!(tree[obj].ob_state & DISABLED))
        tree[obj].ob_state |= SELECTED;
    ob_draw(tree, menu->mn_menu, MAX_DEPTH);
    oldobj = obj;

    gr_mkstate(&mx, &my, &dummy, &dummy);

    while(1)
    {
        setup_moblk(&m, menu, mx, my);

        events = ev_multi(MU_M1|MU_BUTTON, &m, NULL, 0L, 0x0001ff01L, NULL, rets);

        mx = rets[0];
        my = rets[1];
        keystate = rets[3];

        /* find which object we're over (if any) */
        obj = ob_find(tree, menu->mn_menu, 1, mx, my);

        if (events & MU_M1)
        {
            /* mouse has moved, deselect previous selection */
            do_chg(tree, oldobj, SELECTED, FALSE, TRUE, FALSE);

            /* if mouse is still in popup, it must have entered a new item */
            if (obj >= 0)
            {
                do_chg(tree, obj, SELECTED, TRUE, TRUE, TRUE);
                oldobj = obj;
            }
        }

        if (events & MU_BUTTON)
            break;
    }

    /* deselect the selected object, for next time */
    if (obj)
        tree[obj].ob_state &= ~SELECTED;

    return obj;
}

/*
 * mn_popup: implements the menu_popup() function call
 */
WORD mn_popup(MENU *menu, WORD xpos, WORD ypos, MENU *mdata)
{
    OBJECT *tree, *obj;
    void *buf;
    WORD rets[4];
    WORD rc;
    WORD objnum = -1;

    wm_update(BEG_MCTRL);       /* take over the mouse */

// see bottom for comments about 'gl_btrue'
//    do {
//        gr_mkstate(&dummy, &dummy, &buttons, &dummy);
//    } while(buttons);           /* wait for button to come up */
    ev_button(1, 0x00ff, 0x0000, rets);

    /*
     * position the parent box: note that xpos/ypos are the location of
     * the *start* item, not the parent box, so to get the y position of
     * the parent, we first need to subtract the y offset of the start item.
     * then we must clamp the box to the screen area.
     */
    tree = menu->mn_tree;
    obj = tree + menu->mn_menu;
    obj->ob_x = xpos;
    obj->ob_y = ypos - tree[menu->mn_item].ob_y;
    clamp_ypos(obj);

    buf = popup_blit(obj, NULL);    /* save background */
    if (buf)
    {
        gsx_sclip(&gl_rfull);
        objnum = handle_popup(menu);/* sets 'keystate' */
        popup_blit(obj, buf);       /* restore saved background */
    }

    if (objnum >= 0)
    {
        mdata->mn_tree = menu->mn_tree; /* same as input, since we */
        mdata->mn_menu = menu->mn_menu; /*  don't allow submenus   */
        mdata->mn_item = objnum;
        mdata->mn_scroll = menu->mn_scroll;
        mdata->mn_keystate = keystate;  /* set by submenu_event() */
        rc = TRUE;
    }
    else
    {
        mdata->mn_keystate = 0;     /* TOS always sets mn_keystate */
        rc = FALSE;
    }

// there is a problem here somewhere with the button state.
// I think we should go back to the simple loop, but using 'gl_btrue' instead of 'button'
// and maybe gl_btrue should be volatile?

//    do {
//        gr_mkstate(&dummy, &dummy, &buttons, &dummy);
//    } while(buttons);           /* wait for button to come up */
//    while(button & 0x0001)      /* wait for button to come up */
//        ;
    ev_button(1, 0x00ff, 0x0000, rets);

    wm_update(END_MCTRL);       /* give back the mouse */

    return rc;
}

/*
 * mn_settings: implements the menu_settings() function call
 *
 * for a minimal implementation, none of these values are relevant
 */
void mn_settings(WORD flag, MN_SET *set)
{
    switch(flag)
    {
    case 0:             /* return current values */
        *set = settings;
        break;
    case 1:             /* set new values, ignoring negative values, like TOS */
        if (set->display >= 0L)
            settings.display = set->display;
        if (set->drag >= 0L)
            settings.drag = set->drag;
        if (set->delay >= 0L)
            settings.delay = set->delay;
        if (set->speed >= 0L)
            settings.speed = set->speed;
        if (set->height >= 0)
            settings.height = clamp_height(set->height);
        break;
    }
}
#endif
