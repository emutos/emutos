/*      DESKTOP.C       05/04/84 - 09/05/85     Lee Lorenzen            */
/*      for 3.0         3/12/86  - 1/29/87      MDF                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2017 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "config.h"
#include <string.h>

#include "xbiosbind.h"
#include "portab.h"
#include "biosext.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "dos.h"
#include "gemdos.h"
#include "optimize.h"

#include "gembind.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskbind.h"

#include "../bios/screen.h"
#include "../bios/videl.h"
#include "nls.h"
#include "version.h"
#include "../bios/header.h"

#include "aesbind.h"
#include "desksupp.h"
#include "deskglob.h"
#include "deskins.h"
#include "deskinf.h"
#include "deskdir.h"
#include "deskrsrc.h"
#include "deskfun.h"
#include "deskpro.h"
#include "deskact.h"
#include "deskobj.h"
#include "deskrez.h"
#include "kprint.h"
#include "deskmain.h"
#include "scancode.h"

/* structure pointed to by return value from Keytbl() */
typedef struct {
    UBYTE *normal;
    UBYTE *shift;
    UBYTE *caps;
} KEYTAB;

#define abs(x) ( (x) < 0 ? -(x) : (x) )
#define menu_text(tree,inum,ptext) (((tree)+(inum))->ob_spec = ptext)


#define ESC     0x1b


/*
 * the following will always be true unless unlikely changes occur
 * in the desk menu
 */
#define TITLE_ROOT      (DESKMENU-1)
#define ITEM_ROOT       (ABOUITEM-1)


/*
 * flags for communication between do_viewmenu(), desk_all()
 */
#define VIEW_HAS_CHANGED    0x0001
#define SORT_HAS_CHANGED    0x0002


/*
 * values in ob_flags for alignment of text objects
 * (note: internal only for the desktop, _not_ understood by the AES)
 *
 * these values must not conflict with "standard" ob_flags items
 */
#define CENTRE_ALIGNED  0x8000
#define RIGHT_ALIGNED   0x4000


GLOBAL BYTE     gl_amstr[4];
GLOBAL BYTE     gl_pmstr[4];

GLOBAL WORD     gl_apid;

extern WORD     enable_ceh;     /* in gemdosif.S */

/* forward declaration  */
static void    cnx_put(void);


/* BugFix       */
static WORD     ig_close;

/*
 * arrays used by men_update() to enable/disable menu items according
 * to the state of the desktop.  men_update() initially enables every
 * menu item, then calls men_list() (possibly several times).  each
 * call to men_list() enables or disables every item in a given array.
 *
 * detailed usage:
 *  there is one array of items to enable:
 *      ILL_OPENWIN[]   enabled if there is an open window
 *  and many arrays of items to disable:
 *      ILL_ITEM[]      always disabled
 *      ILL_NOWIN[]     disabled if there are no open windows
 *      ILL NOSEL[]     disabled if there are no icons selected
 *      ILL_MULTSEL[]   disabled if two or more icons are selected
 *      ILL_FDSK[]      disabled if a floppy disk icon is selected
 *      ILL_HDSK[]      disabled if a hard disk icon is selected
 *      ILL_FILE[]      disabled if an executable file, or a non-executable
 *                       file with an associated application, is selected
 *      ILL_DOCU[]      disabled if a normal non-executable file is selected
 *      ILL_FOLD[]      disabled if a folder is selected
 *      ILL_TRASH[]     disabled if the trash can is selected
 */
static const BYTE     ILL_ITEM[] = {L1ITEM, L2ITEM, L3ITEM, L4ITEM, L5ITEM, 0};
static const BYTE     ILL_FILE[] = {IDSKITEM,RICNITEM,0};
static const BYTE     ILL_DOCU[] = {IDSKITEM,IAPPITEM,RICNITEM,0};
static const BYTE     ILL_FOLD[] = {IDSKITEM,IAPPITEM,RICNITEM,0};
static const BYTE     ILL_FDSK[] = {IAPPITEM,0};
static const BYTE     ILL_HDSK[] = {IAPPITEM,0};
static const BYTE     ILL_NOSEL[] = {OPENITEM,DELTITEM,
                                IAPPITEM,RICNITEM,0};
static const BYTE     ILL_MULTSEL[] = {OPENITEM, IDSKITEM, SHOWITEM, 0};
static const BYTE     ILL_TRASH[] = {OPENITEM,DELTITEM,IDSKITEM,
                                IAPPITEM,0};
static const BYTE     ILL_NOWIN[] = {NFOLITEM,CLOSITEM,CLSWITEM,0};
static const BYTE     ILL_OPENWIN[] = {SHOWITEM,NFOLITEM,CLOSITEM,CLSWITEM,ICONITEM,
                                NAMEITEM,DATEITEM,SIZEITEM,TYPEITEM,0};

/*
 * table to map the keyboard arrow character to the corresponding
 * value to put in msg[3] of the WM_ARROWED msg
 *
 * the table consists of pairs of values: { scancode, value }
 */
static const WORD arrow_table[] =
{
    SHIFT_ARROW_UP, WA_UPPAGE,
    SHIFT_ARROW_DOWN, WA_DNPAGE,
    ARROW_UP, WA_UPLINE,
    ARROW_DOWN, WA_DNLINE,
    ARROW_LEFT, WA_LFLINE,
    ARROW_RIGHT, WA_RTLINE,
    0
};


#if CONF_WITH_EASTER_EGG
/* easter egg */
static const WORD  freq[]=
{
        262, 349, 329, 293, 349, 392, 440, 392, 349, 329, 262, 293,
        349, 262, 262, 293, 330, 349, 465, 440, 392, 349, 698
};

static const WORD  dura[]=
{
        4, 12, 4, 12, 4, 6, 2, 4, 4, 12, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 12, 4, 8, 4
};
#endif


static LONG     ad_ptext;
static LONG     ad_picon;

static int can_change_resolution;

static void detect_features(void)
{
    can_change_resolution = rez_changeable();
}


#if CONF_DEBUG_DESK_STACK
extern LONG deskstackbottom[];

static void display_free_stack(void)
{
    LONG *p;

    for (p = deskstackbottom; ; p++)
        if (*p != STACK_MARKER)
            break;

    kprintf("Desktop stack has %ld bytes available\n",
            (p-deskstackbottom)*sizeof(LONG));
}
#else
#define display_free_stack()
#endif


/*
 *  Turn on the hour glass to signify a wait and turn it off when
 *  we're done
 */
static void desk_wait(WORD turnon)
{
    graf_mouse(turnon ? HGLASS : ARROW, NULL);
}


/*
 *  Routine to update all of the desktop windows
 */
static void desk_all(WORD flags)
{
    desk_wait(TRUE);
    if (flags & SORT_HAS_CHANGED)
        win_srtall();
    if (flags)          /* either sort or view has changed */
        win_bdall();
    win_shwall();
    desk_wait(FALSE);
}


/*
 *  Enable/Disable the menu items in dlist
 */
static void men_list(OBJECT *mlist, const BYTE *dlist, WORD enable)
{
    while (*dlist)
        menu_ienable(mlist, *dlist++, enable);
}


/*
 *  Based on current selected icons, figure out which menu items
 *  should be selected (deselected)
 */
static void men_update(void)
{
    WORD item, nsel, isapp;
    const BYTE *pvalue;
    ANODE *appl;
    OBJECT *tree = G.a_trees[ADMENU];

    pvalue = 0;

    /* enable all items */
    for (item = OPENITEM; item <= PREFITEM; item++)
        menu_ienable(tree, item, 1);

    /* disable some items */
    men_list(tree, ILL_ITEM, FALSE);

    nsel = 0;
    for (item = 0; (item=win_isel(G.g_screen, G.g_croot, item)) != 0; nsel++)
    {
        appl = i_find(G.g_cwin, item, NULL, &isapp);
        if (!appl)
            continue;
        switch(appl->a_type)
        {
        case AT_ISFILE:
            if (isapp || is_installed(appl))
                pvalue = ILL_FILE;
            else
                pvalue = ILL_DOCU;
            break;
        case AT_ISFOLD:
            pvalue = ILL_FOLD;
            break;
        case AT_ISDISK:
            pvalue = (appl->a_aicon == IG_FLOPPY) ? ILL_FDSK : ILL_HDSK;
            break;
        case AT_ISTRSH:                 /* Trash */
            pvalue = ILL_TRASH;
            break;
        }
        men_list(tree, pvalue, FALSE);       /* disable certain items */
#if CONF_WITH_DESKTOP_SHORTCUTS
        /* allow "Remove icon" for icons on the desktop */
        if (appl->a_flags & AF_ISDESK)
            menu_ienable(tree, RICNITEM, TRUE);
#endif
    }

    if (win_ontop())
        men_list(tree, ILL_OPENWIN, TRUE);
    else
        men_list(tree, ILL_NOWIN, FALSE);

    if (nsel != 1)
    {
        if (nsel)
            pvalue = ILL_MULTSEL;
        else
            pvalue = ILL_NOSEL;
        men_list(tree, pvalue, FALSE);
    }

#if !CONF_WITH_FORMAT
    menu_ienable(tree, FORMITEM, 0);
#endif

#if CONF_WITH_SHUTDOWN
    menu_ienable(tree, QUITITEM, can_shutdown());
#else
    menu_ienable(tree, QUITITEM, 0);
#endif

#if WITH_CLI == 0
    menu_ienable(tree, CLIITEM, 0);
#endif

}


static WORD do_deskmenu(WORD item)
{
    WORD done, touchob;
    OBJECT *tree, *obj;

    done = FALSE;
    switch(item)
    {
    case ABOUITEM:
        display_free_stack();
        tree = G.a_trees[ADDINFO];
        /* draw the form        */
        show_hide(FMD_START, tree);
        while(!done)
        {
            touchob = form_do(tree, 0);
            touchob &= 0x7fff;
            if (touchob == DEICON)
            {
#if CONF_WITH_EASTER_EGG
                int i;
                for (i = 0; i < 23; i++)
                {
                    sound(TRUE, freq[i], dura[i]);
                    evnt_timer(dura[i]*64, 0);
                }
#endif
            }
            else
                done = TRUE;
        }
        obj = tree + DEOK;
        obj->ob_state = NORMAL;
        show_hide(FMD_FINISH, tree);
        done = FALSE;
        break;
    }

    return done;
}


static WORD do_filemenu(WORD item)
{
    WORD done;
    WORD curr;
    WNODE *pw;

    done = FALSE;
    pw = win_ontop();

    curr = win_isel(G.g_screen, G.g_croot, 0);
    switch(item)
    {
    case OPENITEM:
        if (curr)
            done = do_open(curr);
        break;
    case SHOWITEM:
        if (curr)
            do_info(curr);
        else if (pw)
            inf_disk(pw->w_path->p_spec[0]);
        break;
    case NFOLITEM:
        if (pw)
            fun_mkdir(pw);
        break;
    case CLOSITEM:
        if (pw)
            fun_close(pw, CLOSE_FOLDER);
        break;
    case CLSWITEM:
        if (pw)
            fun_close(pw, CLOSE_WINDOW);
        break;
    case DELTITEM:
        if (curr)
            fun_del(curr);
        break;

#if CONF_WITH_FORMAT
    case FORMITEM:
        do_format();
        break;
#endif

#if WITH_CLI != 0
    case CLIITEM:                         /* Start EmuCON */
        G.g_tail[0] = G.g_tail[1] = 0;
        strcpy(G.g_cmd, "EMUCON");
        done = pro_run(FALSE, 1, -1, -1);
        break;
#endif

#if CONF_WITH_SHUTDOWN
    case QUITITEM:
        enable_ceh = FALSE; /* avoid possibility of useless form_alert()s */
        display_free_stack();
        pro_exit(G.g_cmd, G.g_tail);
        done = TRUE;
        break;
#endif
    }

    return done;
}


static WORD do_viewmenu(WORD item)
{
    WORD newview, newsort, rc = 0;
    LONG ptext;

    newview = G.g_iview;
    newsort = G.g_isort;
    switch(item)
    {
    case ICONITEM:
        newview = (G.g_iview == V_ICON) ? V_TEXT : V_ICON;
        break;
    case NAMEITEM:
        newsort = S_NAME;
        break;
    case DATEITEM:
        newsort = S_DATE;
        break;
    case SIZEITEM:
        newsort = S_SIZE;
        break;
    case TYPEITEM:
        newsort = S_TYPE;
        break;
    case NSRTITEM:
        newsort = S_NSRT;
        break;
    }

    if (newview != G.g_iview)
    {
        G.g_iview = newview;
        ptext = (newview == V_TEXT) ? ad_picon : ad_ptext;
        menu_text(G.a_trees[ADMENU], ICONITEM, ptext);
        rc |= VIEW_HAS_CHANGED;
    }
    if (newsort != G.g_isort)
    {
        menu_icheck(G.a_trees[ADMENU], G.g_csortitem, 0);
        G.g_csortitem = item;
        menu_icheck(G.a_trees[ADMENU], item, 1);
        rc |= SORT_HAS_CHANGED;
    }

    if (rc)
        win_view(newview, newsort);

    return rc;
}


static WORD do_optnmenu(WORD item)
{
    WORD done, rebld, curr;
    WORD newres, newmode;

    done = FALSE;
    rebld = FALSE;

    curr = win_isel(G.g_screen, G.g_croot, 0);

    switch(item)
    {
    case IDSKITEM:
        rebld = ins_devices();
        if (rebld)
        {
            app_blddesk();
        }
        break;
    case IAPPITEM:
        curr = 0;
        while( (curr = win_isel(G.g_screen, G.g_croot, curr)) )
        {
            WORD change;

            change = ins_app(curr);
            if (change < 0) /* user cancelled */
                break;
            if (change > 0) /* install or remove */
                rebld++;
        }
        if (rebld)
            desk_all(FALSE);
        break;
    case IICNITEM:
        rebld = ins_icon(curr);
        if (rebld > 0)
        {
            app_blddesk();
        }
#if CONF_WITH_WINDOW_ICONS
        else if (rebld < 0)
        {
            win_bdall();
            win_shwall();
        }
#endif
        break;
    case RICNITEM:
        if (curr)
            rebld = rmv_icon(curr);
        if (rebld)
        {
            app_blddesk();
        }
        break;
    case PREFITEM:
        if (inf_pref())
            desk_all(FALSE);
        break;
    case SAVEITEM:
        desk_wait(TRUE);
        cnx_put();
        app_save(TRUE);
        desk_wait(FALSE);
        break;
    case RESITEM:
        rebld = change_resolution(&newres,&newmode);
        if (rebld == 1)
        {
            if (FALSE)
                {
                    /* Dummy case for conditional compilation */
                }
#if CONF_WITH_VIDEL || defined(MACHINE_AMIGA)
            else if (newres == FALCON_REZ)
                shel_write(5,newmode,1,NULL,NULL);
#endif
#if CONF_WITH_ATARI_VIDEO
            else shel_write(5,newres+2,0,NULL,NULL);
#endif
            done = TRUE;
        }
        break;
    }

    return done;
}


static WORD hndl_button(WORD clicks, WORD mx, WORD my, WORD button, WORD keystate)
{
    WORD done, junk;
    GRECT c;
    WORD wh, dobj, dest_wh;
    WORD root;
    WNODE *wn;

    done = FALSE;

    wh = wind_find(mx, my);

    if (wh != G.g_cwin)
        desk_clear(G.g_cwin);

    desk_verify(wh, FALSE);

    wind_get_grect(wh, WF_WXYWH, &c);

    if (clicks == 1)
    {
        act_bsclick(G.g_cwin, G.g_screen, G.g_croot, mx, my,
                    keystate, &c, FALSE);
        graf_mkstate(&junk, &junk, &button, &junk);
        if (button & 0x0001)
        {
            dest_wh = act_bdown(G.g_cwin, G.g_screen, G.g_croot, &mx, &my,
                                &keystate, &c, &dobj);
            if (dest_wh != NIL)
            {
                root = 1;
                if (dest_wh != 0)
                {
                    wn = win_find(dest_wh);
                    if (wn)
                        root = wn->w_root;
                }
                fun_drag(wh, dest_wh, root, dobj, mx, my, keystate);
                desk_clear(wh);
            }
        }
    }
    else
    {
        act_bsclick(G.g_cwin, G.g_screen, G.g_croot, mx, my, keystate, &c, TRUE);
        done = do_filemenu(OPENITEM);
    }

    men_update();

    return done;
}


static WORD hndl_menu(WORD title, WORD item)
{
    WORD done, rc;

    done = FALSE;
    switch(title)
    {
    case DESKMENU:
        done = do_deskmenu(item);
        break;
    case FILEMENU:
        done = do_filemenu(item);
        break;
    case VIEWMENU:
        done = FALSE;
        rc = do_viewmenu(item);
        if (rc)             /* if sort and/or view has changed,  */
            desk_all(rc);   /* rebuild all windows appropriately */
        break;
    case OPTNMENU:
        done = do_optnmenu(item);
        break;
    }

    menu_tnormal(G.a_trees[ADMENU], title, 1);

    return done;
}


/* Simulate WM_ARROWED using arrow keys */
static void kbd_arrow(WORD type)
{
    WORD wh;
    WORD dummy;
    WNODE *pw;

    wind_get(0, WF_TOP, &wh, &dummy, &dummy, &dummy);
    if (!wh)
        return;

    pw = win_find(wh);
    if (!pw)
        return;

    desk_clear(wh);
    win_arrow(wh, type);
}


/*
 * check for arrow key & handle appropriately
 *
 * return TRUE iff matching key found & processed
 */
static BOOL check_arrow_key(WORD thechar)
{
    const WORD *p;

    for (p = arrow_table; *p; p += 2)
    {
        if (*p == thechar)
        {
            kbd_arrow(*(p+1));
            return TRUE;
        }
    }

    return FALSE;
}


/*
 * search ANODEs for matching function key and launch corresponding application
 */
static BOOL process_funkey(WORD funkey)
{
    ANODE *pa;
    BYTE pathname[MAXPATHLEN];
    BYTE *pfname;

    for (pa = G.g_ahead; pa; pa = pa->a_next)
        if (pa->a_funkey == funkey)
            break;

    if (pa)
    {
        pfname = filename_start(pa->a_pappl);
        /* copy pathname including trailing backslash */
        strlcpy(pathname,pa->a_pappl,pfname-pa->a_pappl+1);
        do_aopen(pa,1,-1,pathname,pfname);
        return TRUE;
    }

    return FALSE;
}


/*
 * check for function key & handle appropriately
 *
 * return TRUE iff matching key found & processed
 */
static BOOL check_function_key(WORD thechar)
{
    WORD funkey;

    if ((thechar >= FUNKEY_01) && (thechar <= FUNKEY_10))
        funkey = ((thechar-FUNKEY_01) >> 8) + 1;
    else if ((thechar >= FUNKEY_11) && (thechar <= FUNKEY_20))
        funkey = ((thechar-FUNKEY_11) >> 8) + 11;
    else return FALSE;

    return process_funkey(funkey);
}


/*
 * check for alt-A to alt-Z & handle appropriately
 *
 * return TRUE iff matching drive found & processed
 */
static BOOL check_alt_letter_key(WORD thechar)
{
    KEYTAB *keytab;
    WORD drive;

    if (LOBYTE(thechar))        /* not an alt key */
        return FALSE;

    keytab = (KEYTAB *)Keytbl(-1, -1, -1);
    drive = keytab->shift[HIBYTE(thechar)];

    if ((drive >= 'A') && (drive <= 'Z'))
    {
        ULONG drivebits = dos_sdrv(dos_gdrv()); /* all current devices */

        if (drivebits & (1L<<(drive-'A')))
        {
            do_dopen(-drive);   /* -ve indicates drive letter rather than obid */
            men_update();       /* must update menu items */
            return TRUE;
        }
    }

    return FALSE;
}


/*
 *  Scan desk menu for matching shortcut
 *
 *  Overview:
 *  A menu tree has two sides, sometimes referred to as BAR and DROPDOWNS.
 *  The BAR side contains objects corresponding to the menu titles; the
 *  DROPDOWNS side contains the menu item objects.
 *
 *  We scan down both sides of the menu tree at the same time.  On the
 *  items side, we scan each G_STRING menu item for the specified shortcut.
 *  If a match is found, the object numbers for title and item are
 *  returned.  If there is no match, -1 is returned.
 *
 *  Examples:
 *      to scan for ctl-X: set 'type' to '^', 'shortcut' to 'X'
 *      to scan for alt-Y: set 'type' to 0x07, 'shortcut' to 'Y'
 */
static WORD scan_menu(BYTE type, BYTE shortcut, WORD *itemptr)
{
    OBJECT *tree = G.a_trees[ADMENU];
    OBJECT *obj;
    BYTE *text, *p;
    WORD title_root, item_root, title, item;

    title_root = TITLE_ROOT;
    item_root = ITEM_ROOT;

    for (title = tree[title_root].ob_head; title != title_root;
                    title = tree[title].ob_next, item_root = tree[item_root].ob_next)
    {
        for (item = tree[item_root].ob_head; item != item_root; item = tree[item].ob_next)
        {
            obj = &tree[item];
            if (obj->ob_state & DISABLED)               /* ignore disabled items */
                continue;
            if ((obj->ob_type & 0x00ff) != G_STRING)    /* all items are strings */
                continue;
            text = (BYTE *)obj->ob_spec;
            p = strchr(text,type);                      /* look for marker */
            if (!p)
                continue;
            if (*(p+1) == shortcut)
            {
                *itemptr = item;
                return title;
            }
        }
    }

    return -1;
}


/*
 * lookup ascii shortcut
 *
 * if found, returns menu title & updates 'item'
 * otherwise returns -1
 */
static WORD lookup_ascii_shortcut(WORD ascii, WORD *itemptr)
{
    if (ascii >= 0x20)      /* we only handle control characters */
        return -1;

    return scan_menu('^', ascii|0x40, itemptr);
}


static WORD hndl_kbd(WORD thechar)
{
    WNODE *pw;
    WORD done = FALSE, ascii;
    WORD title = -1, item;

    ascii = thechar & 0x00ff;
    if (ascii == ESC)   /* refresh window */
    {
        pw = win_ontop();
        if (pw)
            do_refresh(pw);
        return done;
    }

    if (check_arrow_key(thechar))
        return done;

    if (check_function_key(thechar))
        return done;

    if (check_alt_letter_key(thechar))
        return done;

    /* else normal ASCII character (ctl-Z etc) */
    title = lookup_ascii_shortcut(ascii,&item);

    /*
     * actually handle shortcuts
     */
    if (title >= 0)
    {
        menu_tnormal(G.a_trees[ADMENU], title, 0);
        done = hndl_menu(title, item);
    }

    men_update();       /* clean up menu info   */

    return done;
}


WORD hndl_msg(void)
{
    WORD            done;
    WNODE           *pw;
    WORD            change, menu;
    GRECT           gr;
    WORD            cols, shrunk;

    done = change = menu = shrunk = FALSE;

    if ( G.g_rmsg[0] == WM_CLOSED && ig_close )
    {
        ig_close = FALSE;
        return done;
    }

    switch(G.g_rmsg[0])
    {
    case WM_TOPPED:
    case WM_CLOSED:
    case WM_FULLED:
    case WM_ARROWED:
    case WM_VSLID:
    case WM_HSLID:
    case WM_SIZED:
    case WM_MOVED:
        desk_clear(G.g_cwin);
        break;
    }

    switch(G.g_rmsg[0])
    {
    case MN_SELECTED:
        desk_verify(G.g_wlastsel, FALSE);
        done = hndl_menu(G.g_rmsg[3], G.g_rmsg[4]);
        break;
    case WM_REDRAW:
        menu = TRUE;
        if (G.g_rmsg[3])
        {
            do_wredraw(G.g_rmsg[3], G.g_rmsg[4], G.g_rmsg[5],
                            G.g_rmsg[6], G.g_rmsg[7]);
        }
        break;
    case WM_TOPPED:
        pw = win_find(G.g_rmsg[3]);
        if (pw)
        {
            wind_set(G.g_rmsg[3], WF_TOP, 0, 0, 0, 0);
            win_top(pw);
            desk_verify(pw->w_id, FALSE);
            G.g_wlastsel = pw->w_id;
            change = TRUE;
        }
        break;
    case WM_CLOSED:
        do_filemenu(CLOSITEM);
        break;
    case WM_FULLED:
        pw = win_find(G.g_rmsg[3]);
        if (pw)
        {
            cols = pw->w_pncol;
            win_top(pw);
            if (!do_wfull(G.g_rmsg[3]))
                shrunk = TRUE;
            desk_verify(G.g_rmsg[3], TRUE);   /* build window, update w_pncol */
            change = TRUE;
        }
        break;
    case WM_ARROWED:
        win_arrow(G.g_rmsg[3], G.g_rmsg[4]);
        break;
    case WM_VSLID:
        win_slide(G.g_rmsg[3], G.g_rmsg[4]);
        break;
    case WM_MOVED:
    case WM_SIZED:
        pw = win_find(G.g_rmsg[3]);
        if (!pw)
            break;
        gr.g_x = G.g_rmsg[4];
        gr.g_y = G.g_rmsg[5];
        gr.g_w = G.g_rmsg[6];
        gr.g_h = G.g_rmsg[7];
        do_xyfix(&gr.g_x, &gr.g_y);
        wind_set_grect(G.g_rmsg[3], WF_CXYWH, &gr);
        if (G.g_rmsg[0] == WM_SIZED)
        {
            cols = pw->w_pncol;
            wind_get_grect(G.g_rmsg[3], WF_PXYWH, &gr);
            if ((G.g_rmsg[6] <= gr.g_w) && (G.g_rmsg[7] <= gr.g_h))
                shrunk = TRUE;
            desk_verify(G.g_rmsg[3], TRUE);   /* build window, update w_pncol */
        }
        else    /* WM_MOVED */
        {
            wind_get_grect(G.g_rmsg[3],WF_WXYWH, &gr);
            r_set((GRECT *)(&G.g_screen[pw->w_root].ob_x), gr.g_x, gr.g_y, gr.g_w, gr.g_h);
        }
        change = TRUE;
        break;
    }

    /*
     * if our window has shrunk AND we're displaying a different number
     * of columns, we need to send a redraw message because the AES won't
     */
    if (shrunk && (pw->w_pncol != cols))
    {
        wind_get_grect(G.g_rmsg[3], WF_WXYWH, &gr);
        fun_msg(WM_REDRAW, G.g_rmsg[3], gr.g_x, gr.g_y, gr.g_w, gr.g_h);
    }

    if (change)
        cnx_put();

    G.g_rmsg[0] = 0;

    if (!menu)
        men_update();

    return done;
}


/*
 *  cnx_put(): in-memory save of desktop preferences and window information
 *
 *  we save info for the open windows; slot 0 corresponds to the bottom-most
 *  window, slot 1 to the next and so on.  if not all windows are open, the
 *  remaining slots are unused and initialised appropriately.
 */
static void cnx_put(void)
{
    WORD i, unused;
    WSAVE *pws;
    WNODE *pw;

    G.g_cnxsave.cs_view = (G.g_iview == V_ICON) ? 0 : 1;
    G.g_cnxsave.cs_sort = G.g_csortitem - NAMEITEM;
    G.g_cnxsave.cs_confcpy = G.g_ccopypref;
    G.g_cnxsave.cs_confdel = G.g_cdelepref;
    G.g_cnxsave.cs_dblclick = G.g_cdclkpref;
    G.g_cnxsave.cs_confovwr = G.g_covwrpref;
    G.g_cnxsave.cs_mnuclick = G.g_cmclkpref;
    G.g_cnxsave.cs_timefmt = G.g_ctimeform;
    G.g_cnxsave.cs_datefmt = G.g_cdateform;

    /*
     * first, count the unused slots & initialise them
     */
    for (pw = G.g_wfirst, unused = NUM_WNODES; pw; pw = pw->w_next)
    {
        if (pw->w_id)
            unused--;
    }

    for (i = 0, pws = &G.g_cnxsave.cs_wnode[NUM_WNODES-1]; i < unused; i++, pws--)
    {
        pws->pth_save[0] = 0;
    }

    /*
     * next, save the info for the open windows
     * (pws already points to the correct slot)
     */
    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (!pw->w_id)
            continue;
        wind_get(pw->w_id,WF_CXYWH,&pws->x_save,&pws->y_save,&pws->w_save,&pws->h_save);
        do_xyfix(&pws->x_save,&pws->y_save);
        pws->vsl_save  = pw->w_cvrow;
        strcpy(pws->pth_save,pw->w_path->p_spec);
        pws--;
    }
}


static void cnx_get(void)
{
    /* DESKTOP v1.2: This function is a lot more involved */
    /* because CNX_OPEN is no longer a separate function. */
    WORD nw;
    WSAVE *pws;
    WNODE *pw;

    G.g_iview = (G.g_cnxsave.cs_view == 0) ? V_TEXT : V_ICON;
    do_viewmenu(ICONITEM);
    do_viewmenu(NAMEITEM + G.g_cnxsave.cs_sort);
    G.g_ccopypref = G.g_cnxsave.cs_confcpy;
    G.g_cdelepref = G.g_cnxsave.cs_confdel;
    G.g_covwrpref = G.g_cnxsave.cs_confovwr;
    G.g_cdclkpref = G.g_cnxsave.cs_dblclick;
    G.g_cmclkpref = G.g_cnxsave.cs_mnuclick;
    G.g_ctimeform = G.g_cnxsave.cs_timefmt;
    G.g_cdateform = G.g_cnxsave.cs_datefmt;
    G.g_cdclkpref = evnt_dclick(G.g_cdclkpref, TRUE);
    G.g_cmclkpref = menu_click(G.g_cmclkpref, TRUE);

    /* DESKTOP v1.2: Remove 2-window limit; and cnx_open() inlined. */
    for (nw = 0; nw < NUM_WNODES; nw++)
    {
        pws = &G.g_cnxsave.cs_wnode[nw];

        /* Check for valid position */
        if (pws->x_save >= G.g_wdesk)
        {
            pws->x_save = G.g_wdesk/2;
        }
        if (pws->y_save >= G.g_hdesk)
        {
            pws->y_save = G.g_hdesk/2;
        }

        /* Check for valid width + height */
        if (pws->w_save <= 0 || pws->w_save > G.g_wdesk)
        {
            pws->w_save = G.g_wdesk/2;
        }
        if (pws->h_save <= 0 || pws->h_save > G.g_hdesk)
        {
            pws->h_save = G.g_hdesk/2;
        }

        if (pws->pth_save[0])
        {
            WORD obid = obj_get_obid(pws->pth_save[0]);

            if ((pw = win_alloc(obid)))
            {
                pw->w_cvrow = pws->vsl_save;
                do_xyfix(&pws->x_save, &pws->y_save);
                if (!do_diropen(pw, TRUE, obid, pws->pth_save, (GRECT *)pws, TRUE))
                    win_free(pw);
            }
        }
    }

    cnx_put();
}


/*  Counts the occurrences of c in str */
static int count_chars(char *str, char c)
{
    int count;

    count = 0;
    while(*str)
    {
        if (*str++ == c)
            count++;
    }

    return count;
}

/*
 *  The xlate_ functions below are also used by the GEM rsc in aes/gem_rsc.c
 */

/* Translates the strings in an OBJECT array */
void xlate_obj_array(OBJECT *obj_array, int nobj)
{
    OBJECT *obj;

    for (obj = obj_array; --nobj >= 0; obj++) {
        switch(obj->ob_type)
        {
        case G_TEXT:
        case G_BOXTEXT:
        case G_FTEXT:
        case G_FBOXTEXT:
            {
                BYTE **str = & ((TEDINFO *)obj->ob_spec)->te_ptmplt;
                *str = (BYTE *)gettext(*str);
            }
            break;
        case G_STRING:
        case G_BUTTON:
        case G_TITLE:
            obj->ob_spec = (LONG) gettext( (char *) obj->ob_spec);
            break;
        default:
            break;
        }
    }
}

/* Translates and fixes the TEDINFO strings */
void xlate_fix_tedinfo(TEDINFO *tedinfo, int nted)
{
    int i = 0;
    long len;
    int j;
    char *tedinfptr;

    /* translate strings in TEDINFO */
    for (i = 0; i < nted; i++)
    {
        TEDINFO *ted = &tedinfo[i];
        ted->te_ptmplt = (BYTE *)gettext(ted->te_ptmplt);
    }

    /* Fix TEDINFO strings: */
    len = 0;
    for (i = 0; i < nted; i++)
    {
        if (tedinfo[i].te_ptext == 0)
        {
            /* Count number of '_' in strings
             * ( +2 for @ at the beginning, and \0 at the end )
             */
            len += count_chars(tedinfo[i].te_ptmplt, '_') + 2;
        }
    }
    tedinfptr = dos_alloc_anyram(len);   /* Get memory */
    if (!tedinfptr)
    {
        KDEBUG(("insufficient memory for TEDINFO strings (need %ld bytes)\n",len));
        nomem_alert();          /* infinite loop */
    }

    for (i = 0; i < nted; i++)
    {
        if (tedinfo[i].te_ptext == 0)
        {
            tedinfo[i].te_ptext = tedinfptr;
            *tedinfptr++ = '@'; /* First character of uninitialized string */
            len = count_chars(tedinfo[i].te_ptmplt, '_');
            for (j = 0; j < len; j++)
            {
                *tedinfptr++ = '_';     /* Set other characters to '_' */
            }
            *tedinfptr++ = 0;   /* Final 0 */
        }
    }
}

/*
 *  Change the sizes of the menus after translation
 *
 *  Note - the code below is based on the assumption that the width of
 *  the system font is eight (documented as such in lineavars.h)
 */
#define CHAR_WIDTH 8
static void adjust_menu(OBJECT *obj_array)
{
#define OBJ(i) (&obj_array[i])

    int i;  /* index in the menu bar */
    int j;  /* index in the array of pull downs */
    int width = (G.g_wdesk >> 3);   /* screen width in chars */
    int n, x;
    OBJECT *menu = OBJ(0);
    OBJECT *mbar = OBJ(OBJ(menu->ob_head)->ob_head);
    OBJECT *title;

    /*
     * first, set ob_x & ob_width for all the menu headings, and
     * determine the required width of the (translated) menu bar.
     */
    for (i = mbar->ob_head, title = OBJ(i), x = 0; i <= mbar->ob_tail; i++, title++, x += n)
    {
        n = strlen((char *)title->ob_spec);
        title->ob_x = x;
        title->ob_width = n;
    }
    mbar->ob_width = x;

    /*
     * the menu bar cannot start at character offset 0 because, in that
     * case, the outline of the Desk dropdown box will not be drawn
     * properly. we assume that ob_x will be at least 1, so ob_width
     * must be less than the screen width.
     *
     * if the current ob_width is too great, we shrink each heading by
     * 1/2 character (we're assuming that each title has a space at the
     * end of the string).
     *
     * note that this code (and the subsequent menu bar ob_x adjustment)
     * was added to handle the French desktop menu bar which would
     * otherwise be slightly wider than the screen in ST Low.
     */
    if (mbar->ob_width >= width)
    {
        for (i = mbar->ob_head, title = OBJ(i), n = 0; i <= mbar->ob_tail; i++, title++, n++)
        {
            title->ob_x -= n/2;
            if (n&1)
            {
                title->ob_x--;
                title->ob_x |= (CHAR_WIDTH/2)<<8;
                title->ob_width--;
                title->ob_width |= (CHAR_WIDTH/2)<<8;
            }
        }
        mbar->ob_width -= n/2;
    }

    /*
     * if the menu bar is still too wide, we shorten it by truncating
     * the last heading.
     *
     * at the moment, this is a 'last resort' solution; however, if we
     * become desperate for code space, we could drop the '1/2 character'
     * adjustment above and use this as the only fixup (at the cost of
     * some ugliness in the French-language low-res display).
     */
    n = mbar->ob_width - width + 1;
    if (n > 0)
    {
        title--;
        title->ob_width -= n;
        mbar->ob_width -= n;
    }

    /*
     * next, if the menu bar is too far to the right, move it to the left,
     * but only as far as char posn 1
     */
    n = mbar->ob_x + mbar->ob_width - width;
    if (n > 0)
        mbar->ob_x = 1;

    /*
     * finally we can set ob_x and ob_width for the pulldown objects within the menu
     */
    j = OBJ(menu->ob_tail)->ob_head;
    for (i = mbar->ob_head, title = OBJ(i); i <= mbar->ob_tail; i++, title++)
    {
        int k, m;
        OBJECT *dropbox = OBJ(j);
        OBJECT *item;

        /* find widest object under this menu heading */
        for (k = dropbox->ob_head, item = OBJ(k), m = 0; k <= dropbox->ob_tail; k++, item++)
        {
            int l = strlen((char *)item->ob_spec);
            if (m < l)
                m = l;
        }
        dropbox->ob_x = mbar->ob_x + title->ob_x;

        /* make sure the menu is not too far on the right of the screen */
        if ((dropbox->ob_x&0x00ff) + m >= width)
        {
            dropbox->ob_x = width - m;
            m = (m-1) | ((CHAR_WIDTH-1)<<8);
        }

        for (k = dropbox->ob_head, item = OBJ(k); k <= dropbox->ob_tail; k++, item++)
            item->ob_width = m;
        dropbox->ob_width = m;

        j = dropbox->ob_next;
    }
    KDEBUG(("desktop menu bar: x=0x%04x, w=0x%04x\n",mbar->ob_x,mbar->ob_width));
#undef OBJ
}

/*
 *  Align text objects according to special values in ob_flags
 *
 *  Translations typically have a length different from the original
 *  English text.  In order to keep dialogs looking tidy in all
 *  languages, it is often useful to centre- or right-align text
 *  objects.  The AES does not provide an easy way of doing this
 *  (alignment in TEDINFO objects affects the text within the object,
 *  as well as object positioning).
 *
 *  To allow centre- or right-alignment alignment of text objects,
 *  we steal unused bits in ob_flags to indicate the required
 *  alignment.  Note that this does not cause any incompatibilities
 *  because this extra function is performed outside the AES, and
 *  only for the internal desktop resource.  Furthermore, we zero
 *  out the stolen bits after performing the alignment.
 *
 *  Also note that this aligns the *object*, not the text within
 *  the object.  It is perfectly reasonable (and common) to have
 *  left-aligned text within a right-aligned TEDINFO object.
 */
static void align_objects(OBJECT *obj_array, int nobj)
{
    OBJECT *obj;
    char *p;
    WORD len;       /* string length in pixels */

    for (obj = obj_array; --nobj >= 0; obj++)
    {
        switch(obj->ob_type)
        {
        case G_STRING:
        case G_TEXT:
        case G_FTEXT:
        case G_BOXTEXT:
        case G_FBOXTEXT:
            if (obj->ob_type == G_STRING)
                p = (char *)obj->ob_spec;
            else
                p = ((TEDINFO *)obj->ob_spec)->te_ptmplt;
            len = strlen(p) * gl_wchar;
            if (obj->ob_flags & CENTRE_ALIGNED)
            {
                obj->ob_x += (obj->ob_width-len)/2;
                if (obj->ob_x < 0)
                    obj->ob_x = 0;
                obj->ob_width = len;
            } else if (obj->ob_flags & RIGHT_ALIGNED)
            {
                obj->ob_x += obj->ob_width-len;
                if (obj->ob_x < 0)
                    obj->ob_x = 0;
                obj->ob_width = len;
            }
            obj->ob_flags &= ~(CENTRE_ALIGNED|RIGHT_ALIGNED);
            break;
        default:
            break;
        }
    }
}

/*
 *  Horizontally centre dialog title: this is done dynamically to
 *  handle translated titles.
 *
 *  If object 1 of a tree is a G_STRING and its y position equals
 *  one character height, we assume it's the title.
 */
void centre_title(OBJECT *root)
{
    OBJECT *title;
    WORD len;

    title = root + 1;

    if ((title->ob_type == G_STRING) && (title->ob_y == gl_hchar))
    {
        len = strlen((char *)title->ob_spec) * gl_wchar;
        if (len > root->ob_width)
            len = root->ob_width;
        title->ob_x = (root->ob_width - len) / 2;
    }
}

/*
 *  translate and fixup desktop objects
 */
static void desk_xlate_fix(void)
{
    OBJECT *tree = desk_rs_trees[ADDINFO];
    OBJECT *objlabel = &tree[DELABEL];
    OBJECT *objversion = &tree[DEVERSN];
    OBJECT *objyear = &tree[DECOPYRT];
    int i;

    /* translate strings in objects */
    xlate_obj_array(desk_rs_obj, RS_NOBS);

    /* insert the version number */
    objversion->ob_spec = (LONG) version;

    /* slightly adjust the about box for a timestamp build */
    if (version[1] != '.')
    {
        objlabel->ob_spec = (LONG) "";  /* remove the word "Version" */
        objversion->ob_x -= 6;          /* and move the start of the string */
    }

    /* insert the version number */
    objyear->ob_spec = (LONG) COPYRIGHT_YEAR;

    /* adjust the size and coordinates of menu items */
    adjust_menu(desk_rs_trees[ADMENU]);

    /* Fix objects coordinates: */
    for(i = 0 ; i < RS_NOBS ; i++)
    {
        rsrc_obfix(desk_rs_obj, i);
    }

    /* translate and fix TEDINFO strings */
    xlate_fix_tedinfo(desk_rs_tedinfo, RS_NTED);

    /*
     * perform special object alignment - this must be done after
     * translation and coordinate fixing
     */
    align_objects(desk_rs_obj, RS_NOBS);
}

/* Fake a rsrc_gaddr for the ROM desktop: */
WORD rsrc_gaddr_rom(WORD rstype, WORD rsid, void **paddr)
{
    switch(rstype)
    {
    case R_TREE:
        *paddr = desk_rs_trees[rsid];
        break;
    case R_BITBLK:
        *paddr = (void **)&desk_rs_bitblk[rsid];
        break;
    case R_STRING:
        *paddr = (void **)gettext( desk_rs_fstr[rsid] );
        break;
    default:
        KDEBUG(("rsrc_gaddr_rom(): unsupported resource type!\n"));
        return FALSE;
    }

    return TRUE;
}


WORD deskmain(void)
{
    WORD ii, done, flags;
    UWORD ev_which, mx, my, button, kstate, kret, bret;

    /* initialize libraries */
    gl_apid = appl_init();

    /* get GEM's gsx handle */
    gl_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);

    /* set clip to working desk and calc full */
    wind_get(0, WF_WXYWH, &G.g_xdesk, &G.g_ydesk, &G.g_wdesk, &G.g_hdesk);
    wind_calc(1, -1, G.g_xdesk,  G.g_ydesk,  G.g_wdesk,  G.g_hdesk,
                    &G.g_xfull, &G.g_yfull, &G.g_wfull, &G.g_hfull);

    /* initialize mouse     */
    wind_update(BEG_UPDATE);
    desk_wait(TRUE);
    wind_update(END_UPDATE);

    /* detect optional features */
    detect_features();

    /* initialize resources */
    desk_rs_init();                 /* copies ROM to RAM */
    desk_xlate_fix();               /* translates & fixes desktop */

    /* initialize menus and dialogs */
    for (ii = 0; ii < RS_NTREE; ii++)
    {
        rsrc_gaddr_rom(R_TREE, ii, (void **)&G.a_trees[ii]);
        centre_title(G.a_trees[ii]);
    }

    for (ii = 0; ii < RS_NBB; ii++) /* initialize bit images */
    {
        app_tran(ii);
    }

    rsrc_gaddr_rom(R_STRING, STASTEXT, (void **)&ad_ptext);
    rsrc_gaddr_rom(R_STRING, STASICON, (void **)&ad_picon);

    /* These strings are used by dr_code.  We can't get to the
     * resource in dr_code because that would reenter AES, so we
     * save them here.
     */
    strcpy(gl_amstr, ini_str(STAM));
    strcpy(gl_pmstr, ini_str(STPM));

    /* Initialize icons and apps from memory, or EMUDESK.INF,
     * or builtin defaults
     */
    app_start();

    /* initialize windows */
    win_start();

    /* initialize folders, paths, and drives */
    fpd_start();

    /* show menu */
    desk_verify(0, FALSE);                  /* should this be here  */
    wind_update(BEG_UPDATE);
    menu_bar(G.a_trees[ADMENU], 1);
    wind_update(END_UPDATE);

    /* establish menu items */
    G.g_iview = V_ICON;
    menu_text(G.a_trees[ADMENU], ICONITEM, ad_ptext);

    G.g_csortitem = NAMEITEM;
    menu_icheck(G.a_trees[ADMENU], G.g_csortitem, 1);

    menu_ienable(G.a_trees[ADMENU], RESITEM, can_change_resolution);

    /* initialize desktop and its objects */
    app_blddesk();

    /* Take over the desktop */
    wind_set(0, WF_NEWDESK, G.g_screen, 1, 0);

    /* set up current parms */
    desk_verify(0, FALSE);

    /* establish desktop's state from info found in app_start,
     * open windows
     */
    wind_update(BEG_UPDATE);
    cnx_get();
    wind_update(END_UPDATE);
    men_update();

    /* get ready for main loop */
    flags = MU_BUTTON | MU_MESAG | MU_KEYBD;
    done = FALSE;

    /* turn mouse on */
    desk_wait(FALSE);

    /* enable graphical critical error handler */
    enable_ceh = TRUE;

    /* loop handling user input until done */
    while(!done)
    {
        /* block for input */
        ev_which = evnt_multi(flags, 0x02, 0x01, 0x01,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                G.g_rmsg, 0, 0,
                                &mx, &my, &button, &kstate, &kret, &bret);

        /* grab the screen */
        wind_update(BEG_UPDATE);

        /* handle keybd message */
        if (ev_which & MU_KEYBD)
            done = hndl_kbd(kret);

        /* handle button down */
        if (ev_which & MU_BUTTON)
            done = hndl_button(bret, mx, my, button, kstate);

        /* handle system message */
        while (ev_which & MU_MESAG)
        {
            done = hndl_msg();
            /* use quick-out to clean out all messages */
            if (done)
                break;
            ev_which = evnt_multi(MU_MESAG | MU_TIMER, 0x02, 0x01, 0x01,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                G.g_rmsg, 0, 0,
                                &mx, &my, &button, &kstate, &kret, &bret);
        }

        /* free the screen      */
        wind_update(END_UPDATE);
    }

    /* save state in memory for when we come back to the desktop */
    cnx_put();
    app_save(FALSE);

    /* turn off the menu bar */
    menu_bar(NULL, 0);

    /* exit the gem AES */
    appl_exit();

    return TRUE;
}
