/*      GEMMNLIB.C      04/26/84 - 08/14/86     Lowell Webster          */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */
/*      fix mn_bar -- bar too wide              11/19/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2019 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "struct.h"
#include "aesdefs.h"
#include "aesext.h"
#include "aesvars.h"
#include "gemdos.h"
#include "obdefs.h"
#include "gemlib.h"

#include "gemgsxif.h"
#include "gemevlib.h"
#include "gemoblib.h"
#include "gemwmlib.h"
#include "gemgraf.h"
#include "geminput.h"
#include "gemsuper.h"
#include "gemctrl.h"
#include "gempd.h"
#include "rectfunc.h"
#include "gemmnlib.h"
#include "gemmnext.h"
#include "geminit.h"


#define THESCREEN 0
#define THEBAR 1
#define THEACTIVE 2

/*** STATE DEFINITIONS FOR menu_state ***********************************/

#define INBAR   1       /* mouse position       outside menu bar & mo dn */
                        /* multi wait           mo up | in menu bar     */
                        /* moves                -> 5  ,  ->2            */

#define OUTTITLE 2      /* mouse position       over title && mo dn     */
                        /* multiwait            mo up | out title rect  */
                        /* moves                -> 5  , ->1 ->2  ->3    */

#define OUTITEM 3       /* mouse position       over item && mo dn      */
                        /* multi wait           mo up | out item rect   */
                        /* moves                -> 5  , ->1 ->2 ->3 ->4 */

#define INBARECT 4      /* mouse position       out menu rect && bar && mo dn*/
                        /* multi wait   mo up | in menu rect | in menu bar */
                        /* moves        -> 5  , -> 3         , -> 2     */


GLOBAL OBJECT   *gl_mntree;
GLOBAL AESPD    *gl_mnppd;

static AESPD    *desk_ppd[NUM_ACCS];
static WORD     acc_display[NUM_ACCS];

GLOBAL WORD     gl_dafirst;     /* object # of first DA entry */


static WORD menu_sub(OBJECT *tree, WORD ititle)
{
    WORD    themenus, imenu;
    WORD    i;

    themenus = (tree+THESCREEN)->ob_tail;

    /* correlate title # to menu subtree # */
    imenu = (tree+themenus)->ob_head;
    for (i = ititle-THEACTIVE; i > 1; i--)
    {
        imenu = (tree+imenu)->ob_next;
    }

    return imenu;
}


static void menu_fixup(void)
{
    WORD    themenus, dabox, ob;
    WORD    i, cnt, st, height;
    OBJECT  *tree;

    if ((tree=gl_mntree) == NULL)
        return;

    /*
     * add the objects describing the DAs to the menu tree
     */
    themenus = tree[THESCREEN].ob_tail;
    dabox = tree[themenus].ob_head;
    tree[dabox].ob_head = tree[dabox].ob_tail = NIL;
    gl_dafirst = dabox + 3;

    cnt = (D.g_accreg) ? (2 + D.g_accreg) : 1;

    for (i = 1, st = 0, height = 0; i <= cnt; i++)
    {
        ob = dabox + i;
        ob_add(tree, dabox, ob);
        if (i > 2)      /* the DAs come after the accessory separator line */
        {
            for ( ; st < NUM_ACCS; st++)
                if (D.g_acctitle[st])
                    break;
            if (st >= NUM_ACCS)     /* should not happen */
                break;
            tree[ob].ob_spec = (LONG)D.g_acctitle[st++];
        }
        height += gl_hchar;
    }

    tree[dabox].ob_height = height;
}


/*
 *  Change a mouse-wait rectangle based on an object's size
 */
static void rect_change(OBJECT *tree, MOBLK *prmob, WORD iob, BOOL x)
{
    ob_actxywh(tree, iob, &prmob->m_gr);
    prmob->m_out = x;
}



/*
 *  Routine to change the state of a particular object.  The change in
 *  state will not occur if the object is disabled and the chkdisabled
 *  parameter is set.  The object will be drawn with its new state only
 *  if the dodraw parameter is set.
 */
BOOL do_chg(OBJECT *tree, WORD iitem, UWORD chgvalue,
             WORD dochg, WORD dodraw, WORD chkdisabled)
/* tree:         tree that holds item */
/* iitem:        item to affect       */
/* chgvalue:     bit value to change  */
/* dochg:        set or reset value   */
/* dodraw:       draw resulting change*/
/* chkdisabled:  only if item enabled */
{
    UWORD   curr_state;
    OBJECT  *obj;

    obj = tree + iitem;
    curr_state = obj->ob_state;
    if (chkdisabled && (curr_state & DISABLED) )
        return FALSE;

    if (dochg)
        curr_state |= chgvalue;
    else
        curr_state &= ~chgvalue;

    if (dodraw)
        gsx_sclip(&gl_rzero);

    ob_change(tree, iitem, curr_state, dodraw);
    return TRUE;
}


/*
 *  Test if an item number has changed
 */
static BOOL item_changed(WORD last_item, WORD cur_item)
{
    if (last_item == NIL)
        return FALSE;

    if (last_item == cur_item)
        return FALSE;

    return TRUE;
}

/*
 *  Set or reset the SELECTED flag of an item iff the item number has changed
 */
static BOOL menu_select(OBJECT *tree, WORD last_item, WORD cur_item, WORD setit)
{
    if (item_changed(last_item, cur_item))
    {
        return do_chg(tree, last_item, SELECTED, setit, TRUE, TRUE);
    }

    return FALSE;
}


/*
 *  Routine to save or restore the portion of the screen underneath a
 *  menu tree.  This involves BLTing out and back the data that was
 *  underneath the menu before it was pulled down.
 */
static void menu_sr(WORD saveit, OBJECT *tree, WORD imenu)
{
    GRECT   t;

    /* do the blit to save or restore */
    gsx_sclip(&gl_rzero);
    ob_actxywh(tree, imenu, &t);
    t.g_x -= MENU_THICKNESS;
    t.g_w += 2 * MENU_THICKNESS;
    t.g_h += 2 * MENU_THICKNESS;
    if (saveit)
        bb_save(&t);
    else
        bb_restore(&t);
}


/*
 *  Routine to pull a menu down.  This involves saving the data
 *  underneath the menu and drawing in the proper menu sub-tree.
 */
static WORD menu_down(OBJECT *tree, WORD ititle)
{
    WORD    imenu;

    imenu = menu_sub(tree, ititle);

    /* draw title selected */
    if (do_chg(tree, ititle, SELECTED, TRUE, TRUE, TRUE))
    {
        /* save area underneath the menu */
        menu_sr(TRUE, tree, imenu);
        /* draw all items in menu */
        ob_draw(tree, imenu, MAX_DEPTH);
    }

    return imenu;
}


WORD mn_do(WORD *ptitle, WORD *pitem)
{
    OBJECT  *tree;
    LONG    buparm;
    WORD    mnu_flags, done, main_rect;
    WORD    cur_menu, cur_item, last_item;
    WORD    cur_title, last_title;
    UWORD   ev_which;
    MOBLK   p1mor, p2mor;
    WORD    menu_state;
    BOOL    leave_flag;
    WORD    rets[6];
    OBJECT  *obj;
#if CONF_WITH_MENU_EXTENSION
    OBJECT  *smtree;
#endif

    /*
     * initially wait to go into the active part of the bar,
     * or for the button state to change, or to go out of the
     * bar when nothing is down
     */
    menu_state = INBAR;

    done = FALSE;
    buparm = 0x00010101L;
    cur_title = cur_menu = cur_item = NIL;
    tree = gl_mntree;

    ct_mouse(TRUE);

    while (!done)
    {
        mnu_flags = MU_BUTTON | MU_M1;

        switch(menu_state)
        {
        case INBAR:
            /* secondary wait for mouse to leave THEBAR */
            mnu_flags |= MU_M2;
            rect_change(tree, &p2mor, THEBAR, TRUE);
            main_rect = THEACTIVE;
            leave_flag = FALSE;
            break;
        case INBARECT:
            /* secondary wait for mouse to enter cur_menu */
            mnu_flags |= MU_M2;
            rect_change(tree, &p2mor, cur_menu, FALSE);
            main_rect = THEACTIVE;
            leave_flag = FALSE;
            break;
        case OUTITEM:
            main_rect = cur_item;
            buparm = (button & 0x0001) ? 0x00010100L : 0x00010101L;
            leave_flag = TRUE;
            break;
        default:    /* OUTTITLE */
            main_rect = cur_title;
            leave_flag = TRUE;
            break;
        }

        /* set up primary mouse rectangle wait according to 'main_rect' set above */
        rect_change(tree, &p1mor, main_rect, leave_flag);

        /*
         * at this point the mouse rectangle waits are as follows:
         * 1) INBAR
         *      primary: wait for mouse to enter THEACTIVE
         *      secondary: wait for mouse to leave INBAR
         * 2) INBARECT:
         *      primary: wait for mouse to enter THEACTIVE
         *      secondary: wait for mouse to enter cur_menu
         * 3) OUTITEM:
         *      primary: wait for mouse to leave cur_item
         *      secondary: unused
         * 4) OUTTITLE:
         *      primary: wait for mouse to leave cur_title
         *      secondary: unused
         */

        /* wait for something */
#if CONF_WITH_MENU_EXTENSION
        ev_which = ev_multi(mnu_flags, &p1mor, &p2mor, NULL, 0x0L, buparm, NULL, rets);
#else
        ev_which = ev_multi(mnu_flags, &p1mor, &p2mor, 0x0L, buparm, NULL, rets);
#endif

        /*
         * if it's a button. first check if we are still in the initial state (INBAR).
         * if so, the user is just holding the button down in the bar, and we stay in
         * the loop, doing nothing, not even checking where the mouse is.
         *
         * otherwise, check if we're in a title.  if not, exit this loop (and
         * subsequently the function).
         *
         * if we are in a title, we flip the button state that we will wait for on
         * the next time around, and continue with menu processing.
         */
        if (ev_which & MU_BUTTON)
        {
            if (menu_state == INBAR)
                continue;
            if (menu_state != OUTTITLE)
                break;
            buparm ^= 0x00000001;
        }

        /* do menus */

        /* save old values      */
        last_title = cur_title;
        last_item = cur_item;

        /* see if mouse cursor is over the bar  */
        cur_title = ob_find(tree, THEACTIVE, 1, rets[0], rets[1]);
        if ((cur_title != NIL) && (cur_title != THEACTIVE))
        {
            menu_state = OUTTITLE;
            cur_item = NIL;
        }
        else
        {
            cur_title = last_title;
            /* if menu never shown, nothing selected */
            if (cur_menu == NIL)
                cur_title = NIL;
            /* if nothing selected, get out */
            if (cur_title == NIL)
            {
                done = TRUE;
            }
            else
            {
                cur_item = ob_find(tree, cur_menu, 1, rets[0], rets[1]);
                if (cur_item != NIL)
                    menu_state = OUTITEM;
                else
                {
                    obj = tree + cur_title;
                    if (obj->ob_state & DISABLED)
                    {
                        cur_title = NIL;
                        done = TRUE;
                    }
                    else
                        menu_state = INBARECT;
                }
            }
        }

#if CONF_WITH_MENU_EXTENSION
        /* remove old submenu if appropriate */
        if (item_changed(last_item, cur_item))
        {
            if (tree[last_item].ob_flags & SUBMENU)
            {
                if (smtree)
                    undisplay_submenu(tree, last_item);
            }
        }
#endif

        /* unhilite old item */
        menu_select(tree, last_item, cur_item, FALSE);
        /* unhilite old title & pull up old menu */
        if (menu_select(tree, last_title, cur_title, FALSE))
            menu_sr(FALSE, tree, cur_menu);
        /* hilite new title & pull down new menu */
        if (menu_select(tree, cur_title, last_title, TRUE))
        {
            cur_menu = menu_down(tree, cur_title);
        }
        /* hilite new item */
        menu_select(tree, cur_item, last_item, TRUE);

#if CONF_WITH_MENU_EXTENSION
        /* display submenu if appropriate */
        if (item_changed(cur_item, last_item))
        {
            if (tree[cur_item].ob_flags & SUBMENU)
            {
                smtree = display_submenu(tree, cur_item);
            }
        }
#endif
    }

    /* decide what should be cleaned up and returned */
    done = FALSE;
    if (cur_title != NIL)
    {
        menu_sr(FALSE, tree, cur_menu);
        if ((cur_item != NIL) && do_chg(tree, cur_item, SELECTED, FALSE, FALSE, TRUE))
        {
            /* only return TRUE when item is enabled and is not NIL */
            *ptitle = cur_title;
            *pitem = cur_item;
            done = TRUE;
        }
        else
            do_chg(tree, cur_title, SELECTED, FALSE, TRUE, TRUE);
    }

    ct_mouse(FALSE);

    return done;
}


/*
 *  Routine to display the menu bar.  Clipping is turned completely
 *  off so that this operation will be as fast as possible.  The
 *  global variable gl_mntree is also set or reset.
 */
void mn_bar(OBJECT *tree, WORD showit)
{
    AESPD   *p;
    OBJECT  *obj;

    p = fpdnm(NULL, rlr->p_pid);

    if (showit)
    {
        gl_mnppd = p;
        gl_mntree = tree;
        menu_fixup();
        obj = tree + 1;
        obj->ob_width = gl_width - obj->ob_x;
        ob_actxywh(tree, THEACTIVE, &gl_ctwait.m_gr);
        gsx_sclip(&gl_rzero);
        ob_draw(tree, THEBAR, MAX_DEPTH);
        /* ensure the separator line is drawn in black in replace mode */
        gsx_attr(FALSE, MD_REPLACE, BLACK);
        gsx_cline(0, gl_hbox - 1, gl_width - 1, gl_hbox - 1);
    }
    else
    {
        gl_mntree = NULL;
        rc_copy(&gl_rmenu, &gl_ctwait.m_gr);
    }

    /* make ctlmgr fix up the size of rect it's waiting for by sending fake key */
    post_keybd(ctl_pd->p_cda, 0x0000);
}


/*
 *  Routine to cleanup some menu stuff:
 *  - tell all DAs that the currently running application is about to terminate
 *  . free up the submenu array in the AESPD if necessary
 */
void mn_cleanup(void)
{
    WORD i;

    for (i = 0; i < NUM_ACCS; i++)
    {
        if (desk_ppd[i])
            ap_sendmsg(appl_msg, AC_CLOSE, desk_ppd[i], i, 0, 0, 0, 0);
    }

#if CONF_WITH_MENU_EXTENSION
    if (gl_submenu)
    {
        dos_free(gl_submenu);
        gl_submenu = NULL;
        gl_submenu_hwm = NULL;
    }
#endif
}


/*
 *  Routine to build lookup table for displayslot->menuid conversion.
 *  On completion, acc_display[n] will contain the menu_id for the
 *  nth accessory slot as displayed on the desktop.
 */
static void build_menuid_lookup(void)
{
    WORD i, slot;

    for (i = 0, slot = 0; i < NUM_ACCS; i++)
    {
        if (D.g_acctitle[i])
        {
            acc_display[slot++] = i;
        }
    }

    for ( ; slot < NUM_ACCS; slot++)
    {
        acc_display[slot++] = -1;
    }
}


/*
 *  Routine to register a desk accessory item on the menu bar.  The
 *  return value is the object index of the menu item that was added.
 */
WORD mn_register(WORD pid, char *pstr)
{
    WORD    openda;

    /* add desk accessory if room */
    if ((pid >= 0) && (D.g_accreg < NUM_ACCS))
    {
        D.g_accreg++;
        for (openda = 0; openda < NUM_ACCS; openda++)
        {
            if (!D.g_acctitle[openda])
                  break;
        }
        if (openda >= NUM_ACCS)         /* shouldn't happen */
        {
            KDEBUG(("g_acctitle[] has no free slots!\n"));
            openda = NUM_ACCS - 1;      /* kludge - fixup, it might survive */
        }
        desk_ppd[openda] = rlr;
        D.g_acctitle[openda] = pstr;    /* save pointer, like Atari TOS */

        menu_fixup();
        build_menuid_lookup();
        return openda;
    }
    else
        return -1;
}

/*
 *  Routine to reset all variables related to menu registration
 */
void mn_init(void)
{
    WORD i;

    for (i = 0; i < NUM_ACCS; i++)
    {
        desk_ppd[i] = NULL;
        D.g_acctitle[i] = NULL;
    }

    D.g_accreg = 0;
}

/*
 *  Routine to get the owner and menu id of the DA corresponding
 *  to the desktop display item number
 */
void mn_getownid(AESPD **owner,WORD *id,WORD item)
{
    WORD n;

    n = acc_display[item];              /* get menu_id */
    if ((n >= 0) && (n < NUM_ACCS))     /* paranoia */
    {
        *id = n;
        *owner = desk_ppd[n];
    }
}
