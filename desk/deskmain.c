/*      DESKTOP.C       05/04/84 - 09/05/85     Lee Lorenzen            */
/*      for 3.0         3/12/86  - 1/29/87      MDF                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2022 The EmuTOS development team
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

#include "emutos.h"
#include "string.h"

#include "xbiosbind.h"
#include "aesext.h"
#include "aesdefs.h"
#include "biosext.h"
#include "obdefs.h"
#include "rectfunc.h"
#include "gemdos.h"
#include "optimize.h"
#include "gsxdefs.h"
#include "intmath.h"

#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"

#include "biosdefs.h"
#include "nls.h"
#include "version.h"
#include "../obj/header.h"

#include "aesbind.h"
#include "desksupp.h"
#include "deskins.h"
#include "deskinf.h"
#include "deskdir.h"
#include "deskrsrc.h"
#include "deskfun.h"
#include "deskpro.h"
#include "deskact.h"
#include "deskobj.h"
#include "deskrez.h"
#include "deskmain.h"
#include "scancode.h"

/* structure pointed to by return value from Keytbl() */
typedef struct {
    UBYTE *normal;
    UBYTE *shift;
    UBYTE *caps;
} KEYTAB;

#define abs(x) ( (x) < 0 ? -(x) : (x) )
#define menu_text(tree,inum,ptext) (((tree)+(inum))->ob_spec = (LONG)(ptext))


#define CR      0x0d
#define ESC     0x1b


/* architectural */
#define MIN_DESKMENU_WIDTH  20  /* in characters, compatible with Atari TOS */


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
#define BACKGROUND_HAS_CHANGED  0x0004
#define SIZETOFIT_HAS_CHANGED   0x0008


/*
 * values in ob_flags for alignment of text objects
 * (note: internal only for the desktop, _not_ understood by the AES)
 *
 * these values must not conflict with "standard" ob_flags items
 */
#define CENTRE_ALIGNED  0x8000
#define RIGHT_ALIGNED   0x4000


GLOBAL char     gl_amstr[4];
GLOBAL char     gl_pmstr[4];

GLOBAL WORD     gl_apid;

/*
 * actual shortcuts currently in use
 *
 * for more information, see the comments in default_shortcuts[] below
 */
GLOBAL UBYTE    menu_shortcuts[NUM_SHORTCUTS];


/* forward declaration  */
static void    cnx_put(void);


/* BugFix       */
static WORD     ig_close;

/*
 * arrays used by men_update() to enable/disable menu items according
 * to the state of the desktop.  men_update() initially enables every
 * menu item, then counts types of icons.  based on the counts and other
 * factors, it disables certain menu items and then calls men_list()
 * (possibly several times).  each call to men_list() enables or disables
 * every item in a given array.
 *
 * detailed usage:
 *  there are three arrays of items to disable:
 *      ILL_NOWIN[]     disabled if there are no open windows
 *      ILL NOSEL[]     disabled if there are no icons selected
 *      ILL_MULTSEL[]   disabled if two or more icons are selected
 */
static const UBYTE ILL_NOSEL[] = { OPENITEM, DELTITEM, 0 };
static const UBYTE ILL_MULTSEL[] = { OPENITEM, 0 };
static const UBYTE ILL_NOWIN[] = {
    NFOLITEM, CLOSITEM, CLSWITEM,
#if CONF_WITH_SELECTALL
    SLCTITEM,
#endif
#if CONF_WITH_FILEMASK
    MASKITEM,
#endif
    0 };


/*
 * array that defines the mapping of shortcuts to menu items
 *
 * a value of 0 indicates that this menu item does not exist in this
 * binary: any shortcut in the corresponding entry of the menu_shortcuts[]
 * array will be ignored.
 *
 * WARNING: to save ROM space, we use a UBYTE instead of a WORD for the
 * menu item number, on the assumption that it will not exceed 255.
 *
 * NOTE: any change to this MUST be synchronised with changes to the
 * exclude_items[] array in tools/draftexc.c
 */
const UBYTE shortcut_mapping[NUM_SHORTCUTS] =
{
                /* 'File' menu */
    OPENITEM,
    SHOWITEM,
#if CONF_WITH_SEARCH
    SRCHITEM,
#else
    0,
#endif
    NFOLITEM,
    CLOSITEM,
    CLSWITEM,
#if CONF_WITH_BOTTOMTOTOP
    BTOPITEM,
#else
    0,
#endif
#if CONF_WITH_SELECTALL
    SLCTITEM,
#else
    0,
#endif
#if CONF_WITH_FILEMASK
    MASKITEM,
#else
    0,
#endif
    DELTITEM,
#if CONF_WITH_FORMAT
    FORMITEM,
#else
    0,
#endif
#if WITH_CLI
    CLIITEM,
#else
    0,
#endif
#if CONF_WITH_SHUTDOWN
    QUITITEM,
#else
    0,
#endif
                /* 'View' menu */
    ICONITEM,
    TEXTITEM,
    NAMEITEM,
    TYPEITEM,
    SIZEITEM,
    DATEITEM,
    NSRTITEM,
#if CONF_WITH_SIZE_TO_FIT
    FITITEM,
#else
    0,
#endif
#if CONF_WITH_BACKGROUNDS
    BACKGRND,
#else
    0,
#endif
                /* 'Options' menu */
    IICNITEM,
    IAPPITEM,
    IDSKITEM,
    RICNITEM,
    PREFITEM,
#if CONF_WITH_DESKTOP_CONFIG
    CONFITEM,
#else
    0,
#endif
    RESITEM,
#if CONF_WITH_READ_INF
    READITEM,
#else
    0,
#endif
    SAVEITEM,
#if CONF_WITH_BLITTER
    BLITITEM,
#else
    0,
#endif
#if CONF_WITH_CACHE_CONTROL
    CACHITEM,
#else
    0,
#endif
};


/*
 * array that defines the default keyboard shortcuts
 *
 * entries are uppercase characters, as displayed in the menu items; the actual
 * shortcut is the corresponding control character.  a value of 0 indicates no
 * shortcut for a specific item.
 */
static const UBYTE default_shortcuts[NUM_SHORTCUTS] =
{
            /* 'File' menu */
    'O',        /* OPENITEM */
    'I',        /* SHOWITEM */
    0x00,       /* SRCHITEM */
    'N',        /* NFOLITEM */
    'H',        /* CLOSITEM */
    'U',        /* CLSWITEM */
    'W',        /* BTOPITEM */
    'A',        /* SLCTITEM */
    0x00,       /* MASKITEM */
    'D',        /* DELTITEM */
    0x00,       /* FORMITEM */
    'Z',        /* CLIITEM */
    0x00,       /* QUITITEM */
            /* 'View' menu */
    0x00,       /* ICONITEM */
    0x00,       /* TEXTITEM */
    0x00,       /* NAMEITEM */
    0x00,       /* SIZEITEM */
    0x00,       /* TYPEITEM */
    0x00,       /* DATEITEM */
    0x00,       /* NSRTITEM */
    0x00,       /* FITITEM */
    0x00,       /* BACKGRND */
            /* 'Options' menu */
    0x00,       /* IICNITEM */
    0x00,       /* IAPPITEM */
    0x00,       /* IDSKITEM */
    0x00,       /* RICNITEM */
    0x00,       /* PREFITEM */
    0x00,       /* CONFITEM */
    'R',        /* RESITEM */
    0x00,       /* READITEM */
    'S',        /* SAVEITEM */
    0x00,       /* BLITITEM */
    0x00,       /* CACHITEM */
};


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
#define EGG_NOTES   23      /* number of notes to play */
static const UWORD freq[EGG_NOTES] =
{
        262, 349, 329, 293, 349, 392, 440, 392, 349, 329, 262, 293,
        349, 262, 262, 293, 330, 349, 465, 440, 392, 349, 698
};

static const UBYTE dura[EGG_NOTES] =
{
        4, 12, 4, 12, 4, 6, 2, 4, 4, 12, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 12, 4, 8, 4
};
#endif

/*
 * the ob_spec field of menu objects that represent separator lines
 * is set to point to an appropriate offset within this string.
 */
#define MAXLEN_SEPARATOR    40
static char separator[MAXLEN_SEPARATOR+1];

static int can_change_resolution;
#if !MPS_BLITTER_ALWAYS_ON
static int blitter_is_present;
#endif
#if CONF_WITH_CACHE_CONTROL
static int cache_is_present;
#endif
static char *desk_rs_strings;   /* see copy_menu_items() */

#if CONF_WITH_READ_INF
static BOOL restart_desktop;    /* TRUE iff we need to restart the desktop */
#endif


#if CONF_WITH_EASTER_EGG
/* array used by play_sound() routine */
static UBYTE snddat[16];        /* read later by interrupt handler */


/*
 *  play_sound()
 *
 *  This routine plays a sound:
 *      'frequency' is the frequency in Hz; must be > 0
 *      'duration' is the duration in ~250msec units: must be > 0 & < 32
 */
static void play_sound(UWORD frequency, UWORD duration)
{
    UWORD tp; /* 12 bit oscillation frequency setting value */

    tp = divu(125000L, frequency);
    snddat[0] = 0;      snddat[1] = LOBYTE(tp);     /* channel A pitch lo */
    snddat[2] = 1;      snddat[3] = HIBYTE(tp);     /* channel A pitch hi */
    snddat[4] = 7;      snddat[5] = 0xFE;
    snddat[6] = 8;      snddat[7] = 0x10;           /* amplitude: envelope */
    snddat[8] = 11;     snddat[9] = 0;              /* envelope lo */
    snddat[10] = 12;    snddat[11] = duration * 8;  /* envelope hi */
    snddat[12] = 13;    snddat[13] = 9;             /* envelope type */
    snddat[14] = 0xFF;  snddat[15] = 0;

    Dosound((LONG)snddat);
}
#endif


static void detect_features(void)
{
    can_change_resolution = rez_changeable();
#if !MPS_BLITTER_ALWAYS_ON
    blitter_is_present = Blitmode(-1) & 0x0002;
#endif
#if CONF_WITH_CACHE_CONTROL
    cache_is_present = cache_exists();
#endif
}


#if CONF_DEBUG_DESK_STACK
extern LONG deskstackbottom[]; /* defined in deskstart.S */

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


#if CONF_WITH_CACHE_CONTROL
/*
 * Routine to set cache on/off: must be Supexec'd because the set_cache()
 * function must run in supervisor state
 */
static void desktop_set_cache(void)
{
    set_cache(G.g_cache);
}
#endif


/*
 *  Routine to update all of the desktop windows
 */
static void desk_all(WORD flags)
{
    desk_busy_on();
    if (flags & SORT_HAS_CHANGED)
        win_srtall();
    if (flags & (VIEW_HAS_CHANGED|SORT_HAS_CHANGED|SIZETOFIT_HAS_CHANGED))
        win_bdall();
    win_shwall();
    desk_busy_off();
}


/*
 *  Enable/Disable the menu items in dlist
 */
static void men_list(OBJECT *mlist, const UBYTE *dlist, WORD enable)
{
    while (*dlist)
        menu_ienable(mlist, *dlist++, enable);
}


/*
 *  Based on currently selected icons & open windows, figure out which
 *  menu items should be enabled/disabled
 */
static void men_update(void)
{
    WORD item, napp, ndesk, nsel, ntrash, nwin;
    BOOL isapp;
    ANODE *appl;
    OBJECT *tree = desk_rs_trees[ADMENU];
    OBJECT *obj;

    /*
     * disable separator strings, enable remaining menu items
     */
    for (obj = tree+OPENITEM; ; obj++)
    {
        if (obj->ob_type == G_STRING)
        {
            if (*(char *)obj->ob_spec == '-')   /* must be a separator */
                obj->ob_state |= DISABLED;
            else
                obj->ob_state &= ~DISABLED;
        }
        if (obj->ob_flags & LASTOB)
            break;
    }

    /*
     * process all selected icons, counting types of icons: applications,
     * desktop icons, trash/printer icons, and selected icons.
     *
     * we handle the desktop "window" and real windows separately, since
     * in a real window there can be selected items that are not visible.
     */
    napp = ndesk = nsel = ntrash = 0;

    if (G.g_cwin == DESKWH)
    {
        for (item = 0; (item=win_isel(G.g_screen, G.g_croot, item)) != 0; nsel++)
        {
            appl = i_find(G.g_cwin, item, NULL, &isapp);
            if (!appl)
                continue;
            if (isapp)          /* count applications selected */
                napp++;
            switch(appl->a_type)
            {
#if CONF_WITH_PRINTER_ICON
            case AT_ISPRNT:                 /* Printer */
#endif
            case AT_ISTRSH:                 /* Trash */
                ntrash++;
                FALLTHROUGH;
            case AT_ISDISK:
                ndesk++;        /* count desktop icons selected */
                break;
            }
#if CONF_WITH_DESKTOP_SHORTCUTS
            /* allow "Remove icon" for icons on the desktop */
            if (appl->a_flags & AF_ISDESK)
                ndesk++;
#endif
        }
    }
    else    /* real window */
    {
        WNODE *pw = win_find(G.g_cwin);
        pn_count(pw, &nsel, &napp);
    }
    nwin = win_count();     /* number of open windows */

    /* disable "Delete" iff either trash or printer is selected */
    if (ntrash)
        menu_ienable(tree, DELTITEM, FALSE);

    /* disable "Install application" iff no applications are selected */
    if (!napp)
        menu_ienable(tree, IAPPITEM, FALSE);

    /* disable "Remove desktop icon" iff no desktop icons are selected */
    if (!ndesk)
        menu_ienable(tree, RICNITEM, FALSE);

    /* disable items based on number of selections */
    switch(nsel) {
    case 0:
        men_list(tree, ILL_NOSEL, FALSE);
        /*
         * like Atari TOS, 'Show' & 'Search' are only disabled if there
         * are neither open windows nor selected icons
         */
        if (!nwin)
        {
            menu_ienable(tree, SHOWITEM, FALSE);
#if CONF_WITH_SEARCH
            menu_ienable(tree, SRCHITEM, FALSE);
#endif
        }
        break;
    case 1:
        break;
    default:    /* more than one */
        men_list(tree, ILL_MULTSEL, FALSE);
    }

    /* disable items based on number of open windows */
    switch(nwin) {
    case 0:
        men_list(tree, ILL_NOWIN, FALSE);
        FALLTHROUGH;
#if CONF_WITH_BOTTOMTOTOP
    case 1:
        menu_ienable(tree, BTOPITEM, FALSE);
        break;
#endif
    /* The label below is necessary to avoid warning with FALLTHROUGH */
    default:    /* more than one */
        break;
    }

#if CONF_WITH_SHUTDOWN
    menu_ienable(tree, QUITITEM, can_shutdown());
#endif

    menu_ienable(tree, RESITEM, can_change_resolution);

#if MPS_BLITTER_ALWAYS_ON
    menu_ienable(tree, BLITITEM, FALSE); // Cannot disable blitter
    menu_icheck(tree, BLITITEM, TRUE);
#else
    #if CONF_WITH_BLITTER
    menu_ienable(tree, BLITITEM, blitter_is_present);
    menu_icheck(tree, BLITITEM, G.g_blitter);
    #endif
#endif

#if CONF_WITH_CACHE_CONTROL
    if (cache_is_present)
    {
        menu_ienable(tree, CACHITEM, TRUE);
        menu_icheck(tree, CACHITEM, G.g_cache);
    }
    else
    {
        menu_ienable(tree, CACHITEM, FALSE);
        menu_icheck(tree, CACHITEM, FALSE);
    }
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
        tree = desk_rs_trees[ADDINFO];
        /* draw the form        */
        start_dialog(tree);
        while(!done)
        {
            touchob = form_do(tree, 0);
            touchob &= 0x7fff;
            if (touchob == DEICON)
            {
#if CONF_WITH_EASTER_EGG
                int i;
                for (i = 0; i < EGG_NOTES; i++)
                {
                    play_sound(freq[i], dura[i]);
                    evnt_timer(dura[i]*64, 0);
                }
#endif
            }
            else
                done = TRUE;
        }
        obj = tree + DEOK;
        obj->ob_state = NORMAL;
        end_dialog(tree);
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
        if (pw || curr)
            done = do_open(pw, curr);
        break;
    case SHOWITEM:
        if (curr)
        {
            for ( ; curr; curr = win_isel(G.g_screen, G.g_croot, curr))
            {
                if (!do_info(curr))
                    break;
            }
            fun_rebld_marked(); /* rebuild any changed windows */
            break;
        }
        /*
         * here if there are no highlighted icons: display info for
         * the disk associated with the top window (just like TOS)
         */
        if (pw)
            inf_disk(pw->w_pnode.p_spec[0]);
        break;

#if CONF_WITH_SEARCH
    case SRCHITEM:
        if (pw || curr)
            fun_search(pw, curr);
        if (curr)
            desk_clear(DESKWH);     /* deselect desktop icon(s) */
        break;
#endif

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
#if CONF_WITH_BOTTOMTOTOP
    case BTOPITEM:
        pw = win_onbottom();
        if (pw)
        {
            GRECT gr;
            wind_get_grect(pw->w_id, WF_WXYWH, &gr);
            fun_msg(WM_TOPPED, pw->w_id, gr.g_x, gr.g_y, gr.g_w, gr.g_h);
        }
        break;
#endif
#if CONF_WITH_SELECTALL
    case SLCTITEM:
        if (pw)
            fun_selectall(pw);
        break;
#endif
#if CONF_WITH_FILEMASK
    case MASKITEM:
        if (pw)
            fun_mask(pw);
        break;
#endif
    case DELTITEM:
        if (pw || curr)
            fun_del(pw, curr);
        break;

#if CONF_WITH_FORMAT
    case FORMITEM:
        do_format();
        break;
#endif

#if WITH_CLI
    case CLIITEM:                         /* Start EmuCON */
        G.g_work[1] = '\0';
        done = pro_run(FALSE, DEF_CONSOLE, G.g_work, -1, -1);
        if (done && pw)
        {
            /*
             * set default directory according to path in topped window
             */
            char *p;

            strcpy(G.g_work, pw->w_pnode.p_spec);
            p = filename_start(G.g_work);
            *p = '\0';
            shel_wdef("", G.g_work);
        }
        break;
#endif

#if CONF_WITH_SHUTDOWN
    case QUITITEM:
        enable_ceh = FALSE; /* avoid possibility of useless form_alert()s */
        display_free_stack();
        shel_write(SHW_SHUTDOWN, FALSE, 2, NULL, NULL);
        done = TRUE;
        break;
#endif
    }

    return done;
}


static WORD do_viewmenu(WORD item)
{
    WORD new, rc = 0;
    OBJECT *menutree = desk_rs_trees[ADMENU];

    switch(item)
    {
    case ICONITEM:
    case TEXTITEM:
        new = item - ICONITEM;
        if (new == G.g_iview)
            break;
        menu_icheck(menutree, ICONITEM+G.g_iview, 0);
        menu_icheck(menutree, item, 1);
        G.g_iview = new;
        win_view();         /* uses G.g_iview */
        rc = VIEW_HAS_CHANGED;
        break;
    case NAMEITEM:
    case TYPEITEM:
    case SIZEITEM:
    case DATEITEM:
    case NSRTITEM:
        new = item - NAMEITEM;
        if (new == G.g_isort)
            break;
        menu_icheck(menutree, NAMEITEM+G.g_isort, 0);
        menu_icheck(menutree, item, 1);
        G.g_isort = new;
        rc = SORT_HAS_CHANGED;
        break;
#if CONF_WITH_SIZE_TO_FIT
    case FITITEM:
        G.g_ifit = G.g_ifit ? FALSE : TRUE;     /* flip size-to-fit mode */
        menu_icheck(menutree, FITITEM, G.g_ifit ? 1 : 0);
        rc = SIZETOFIT_HAS_CHANGED;
        break;
#endif
#if CONF_WITH_BACKGROUNDS
    case BACKGRND:
        if (inf_backgrounds())
            rc = BACKGROUND_HAS_CHANGED;
        break;
#endif
    }

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
        rebld = ins_app();
        if (rebld < 0)
        {
            win_bdall();    /* to refresh f_pa/f_isap in the FNODEs */
            win_shwall();
        }
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
        desk_busy_on();
        cnx_put();
        app_save(TRUE);
        desk_busy_off();
        break;
#if CONF_WITH_DESKTOP_CONFIG
    case CONFITEM:
        inf_conf();
        break;
#endif
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
                shel_write(SHW_RESCHNG,newmode,1,NULL,NULL);
#endif
#if CONF_WITH_ATARI_VIDEO
            else shel_write(SHW_RESCHNG,newres+2,0,NULL,NULL);
#endif
            done = TRUE;
        }
        break;
#if CONF_WITH_READ_INF
    case READITEM:
        if (app_read_inf())
        {
            restart_desktop = TRUE;
            done = TRUE;
        }
        break;
#endif
#if CONF_WITH_BLITTER && !MPS_BLITTER_ALWAYS_ON
    case BLITITEM:
        G.g_blitter = !G.g_blitter;
        menu_icheck(desk_rs_trees[ADMENU], BLITITEM, G.g_blitter);  /* flip blit mode */
        Blitmode(G.g_blitter);
        break;
#endif
#if CONF_WITH_CACHE_CONTROL
    case CACHITEM:
        G.g_cache = !G.g_cache;
        menu_icheck(desk_rs_trees[ADMENU], CACHITEM, G.g_cache);    /* flip cache mode */
        Supexec((LONG)desktop_set_cache);   /* uses G.g_cache */
        break;
#endif
    }

    return done;
}


static WORD hndl_button(WORD clicks, WORD mx, WORD my, WORD button, WORD keystate)
{
    WORD done, junk;
    GRECT c;
    WORD wh, dobj, dest_wh;

    done = FALSE;

    wh = wind_find(mx, my);

    if (wh != G.g_cwin)
        desk_clear_all();

    desk_verify(wh, FALSE);

    wind_get_grect(wh, WF_WXYWH, &c);

    if (clicks == 1)
    {
        WNODE *pw;

        act_bsclick(G.g_cwin, G.g_croot, mx, my,
                    keystate, &c, FALSE);
        graf_mkstate(&junk, &junk, &button, &junk);
        if (button & 0x0001)
        {
            dest_wh = act_bdown(G.g_cwin, G.g_croot, &mx, &my,
                                &keystate, &c, &dobj);
            if (dest_wh != NIL)
            {
                done = fun_drag(wh, dest_wh, 0, dobj, mx, my, keystate);
                desk_clear(wh);
            }
        }

        pw = win_find(wh);
        win_sinfo(pw, TRUE);
    }
    else
    {
        act_bsclick(G.g_cwin, G.g_croot, mx, my, keystate, &c, TRUE);
        done = do_filemenu(OPENITEM);
        /*
         * we wait for button up because, if the user keeps the button
         * down after the second click, the AES will continue to send
         * double-click events, causing these cosmetic problems:
         * (1) if the double-click is opening a disk/folder, multiple
         *     opens are done
         * (2) if the double-click launches a .TOS program and the eventual
         *     button up occurs in alpha mode, it is not seen by the AES.
         *     this leaves a residual value in the 'button' global which,
         *     on return to the desktop, causes the ctlmgr() function to
         *     continually call ct_mouse().  this calls gsx_mfset() which
         *     turns the mouse on and off, causing the mouse to blink.
         *     this only stops when a mouse click is done.
         */
        evnt_button(1, 0x01, 0x00, &junk, &junk, &junk, &junk);
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
        if (rc)             /* if anything has changed,                  */
            desk_all(rc);   /* rebuild/show all windows as appropriate   */
        break;
    case OPTNMENU:
        done = do_optnmenu(item);
        break;
    }

    menu_tnormal(desk_rs_trees[ADMENU], title, 1);

    return done;
}


/* Simulate WM_ARROWED using arrow keys */
static void kbd_arrow(WORD type)
{
    WORD wh;
    WORD dummy;
    WNODE *pw;

    wind_get(DESKWH, WF_TOP, &wh, &dummy, &dummy, &dummy);
    if (wh == DESKWH)
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
static WORD process_funkey(WORD funkey)
{
    ANODE *pa;
    char pathname[MAXPATHLEN];
    char *pfname;

    for (pa = G.g_ahead; pa; pa = pa->a_next)
        if (pa->a_funkey == funkey)
            break;

    if (pa)
    {
        pfname = filename_start(pa->a_pappl);
        /* copy pathname including trailing backslash */
        strlcpy(pathname,pa->a_pappl,pfname-pa->a_pappl+1);
        return do_aopen(pa,TRUE,-1,pathname,pfname,NULL);
    }

    return -1;
}


/*
 * check for function key & handle appropriately
 *
 * returns:
 *  -1  key not a function key, or matching function key not found
 *  0   non-graphics program launched
 *  1   graphics program launched
 */
static WORD check_function_key(WORD thechar)
{
    WORD funkey;

    if ((thechar >= FUNKEY_01) && (thechar <= FUNKEY_10))
        funkey = HIBYTE(thechar-FUNKEY_01) + 1;
    else if ((thechar >= FUNKEY_11) && (thechar <= FUNKEY_20))
        funkey = HIBYTE(thechar-FUNKEY_11) + 11;
    else return -1;

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

    /*
     * check for impossible drive characters here and just ignore them;
     * otherwise valid_drive() will issue an (ugly) error message
     */
    if ((drive < 'A') || (drive > 'Z')) /* ignore if not A-Z */
        return FALSE;

    if (!valid_drive(drive))
        return FALSE;

    do_dopen(-drive);       /* -ve indicates drive letter rather than obid */
    men_update();           /* must update menu items */
    return TRUE;
}


/*
 * lookup ascii shortcut
 *
 * if found, returns menu title & updates 'item'
 * otherwise returns -1
 */
static WORD lookup_ascii_shortcut(WORD ascii, WORD *itemptr)
{
    OBJECT *tree = desk_rs_trees[ADMENU];
    WORD i, n;

    if (ascii >= 0x20)      /* we only handle control characters */
        return -1;

    ascii |= 0x40;          /* convert to plain character */

    for (i = 0; i < NUM_SHORTCUTS; i++)
    {
        n = shortcut_mapping[i];
        if (!n)
            continue;
        if (ascii == menu_shortcuts[i])
        {
            if (tree[n].ob_state & DISABLED)    /* disabled - no match */
                break;
            *itemptr = n;
            if (n >= IICNITEM)
                return OPTNMENU;
            if (n >= ICONITEM)
                return VIEWMENU;
            return FILEMENU;
        }
    }

    return -1;
}


static WORD hndl_kbd(WORD thechar)
{
    WNODE *pw;
    WORD done, ascii;
    WORD title = -1, item;

    ascii = LOBYTE(thechar);
    if (ascii == CR)    /* deselect icons */
    {
        desk_clear_all();
        return FALSE;
    }
    if (ascii == ESC)   /* refresh window */
    {
        pw = win_ontop();
        if (pw)
            refresh_window(pw, TRUE);
        return FALSE;
    }

    if (check_arrow_key(thechar))
        return FALSE;

    done = check_function_key(thechar);
    if (done >= 0)
        return done;

    if (check_alt_letter_key(thechar))
        return FALSE;

    /* else normal ASCII character (ctl-Z etc) */
    title = lookup_ascii_shortcut(ascii,&item);

    /*
     * actually handle shortcuts
     */
    done = FALSE;
    if (title >= 0)
    {
        menu_tnormal(desk_rs_trees[ADMENU], title, 0);
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
    WORD            handle;

    done = change = menu = shrunk = FALSE;

    if ((G.g_rmsg[0] == WM_CLOSED) && ig_close)
    {
        ig_close = FALSE;
        return done;
    }

    handle = G.g_rmsg[3];

    switch(G.g_rmsg[0])
    {
    case MN_SELECTED:
        desk_verify(G.g_wlastsel, FALSE);
        done = hndl_menu(handle, G.g_rmsg[4]);
        break;
    case WM_REDRAW:
        menu = TRUE;
        if (handle)
        {
            do_wredraw(handle, (GRECT *)&G.g_rmsg[4]);
        }
        break;
    case WM_TOPPED:
        desk_clear(G.g_cwin);
        pw = win_find(handle);
        if (pw)
        {
            wind_set(handle, WF_TOP, 0, 0, 0, 0);
            win_top(pw);
            desk_verify(pw->w_id, FALSE);
            change = TRUE;
        }
        break;
    case WM_CLOSED:
        do_filemenu(CLOSITEM);
        break;
    case WM_FULLED:
        pw = win_find(handle);
        if (pw)
        {
            win_top(pw);
            do_wfull(handle);
            desk_verify(handle, TRUE);      /* build window, update w_pncol */
            change = TRUE;
        }
        break;
    case WM_ARROWED:
        win_arrow(handle, G.g_rmsg[4]);
        break;
#if CONF_WITH_SIZE_TO_FIT
    case WM_HSLID:
        win_slide(handle, TRUE, G.g_rmsg[4]);
        break;
#endif
    case WM_VSLID:
        win_slide(handle, FALSE, G.g_rmsg[4]);
        break;
    case WM_MOVED:
    case WM_SIZED:
        pw = win_find(handle);
        if (!pw)
            break;
        rc_copy((GRECT *)&G.g_rmsg[4], &gr);
        do_xyfix(&gr.g_x, &gr.g_y);
        wind_set_grect(handle, WF_CXYWH, &gr);
        if (G.g_rmsg[0] == WM_SIZED)
        {
            cols = pw->w_pncol;
            wind_get_grect(handle, WF_PXYWH, &gr);
            if ((G.g_rmsg[6] <= gr.g_w) && (G.g_rmsg[7] <= gr.g_h))
                shrunk = TRUE;
            desk_verify(handle, TRUE);      /* build window, update w_pncol */
        }
        else    /* WM_MOVED */
        {
            wind_get_grect(handle, WF_WXYWH, &gr);
            rc_copy(&gr, (GRECT *)(&G.g_screen[pw->w_root].ob_x));
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
        wind_get_grect(handle, WF_WXYWH, &gr);
        fun_msg(WM_REDRAW, handle, gr.g_x, gr.g_y, gr.g_w, gr.g_h);
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
    CSAVE *cnxsave = G.g_cnxsave;

    cnxsave->cs_view = G.g_iview;           /* V_ICON/V_TEXT */
    cnxsave->cs_sort = G.g_isort;           /* S_NAME etc */
    cnxsave->cs_confcpy = G.g_ccopypref;
    cnxsave->cs_confdel = G.g_cdelepref;
    cnxsave->cs_dblclick = G.g_cdclkpref;
    cnxsave->cs_confovwr = G.g_covwrpref;
    cnxsave->cs_timefmt = G.g_ctimeform;
    cnxsave->cs_datefmt = G.g_cdateform;
#if !MPS_BLITTER_ALWAYS_ON
    cnxsave->cs_blitter = TRUE;
#endif
#if CONF_WITH_CACHE_CONTROL
    cnxsave->cs_cache = G.g_cache;
#endif
#if CONF_WITH_DESKTOP_CONFIG
    cnxsave->cs_appdir = G.g_appdir;
    cnxsave->cs_fullpath = G.g_fullpath;
#endif
#if CONF_WITH_SIZE_TO_FIT
    cnxsave->cs_sizefit = G.g_ifit;
#endif

    /*
     * first, count the unused slots & initialise them
     */
    for (pw = G.g_wfirst, unused = NUM_WNODES; pw; pw = pw->w_next)
    {
        if (pw->w_id)
            unused--;
    }

    for (i = 0, pws = &cnxsave->cs_wnode[NUM_WNODES-1]; i < unused; i++, pws--)
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
        pws->hsl_save  = pw->w_cvcol;
        pws->vsl_save  = pw->w_cvrow;
        strcpy(pws->pth_save,pw->w_pnode.p_spec);
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
    CSAVE *cnxsave = G.g_cnxsave;

    do_viewmenu(ICONITEM + cnxsave->cs_view);
    do_viewmenu(NAMEITEM + cnxsave->cs_sort);
    G.g_ccopypref = cnxsave->cs_confcpy;
    G.g_cdelepref = cnxsave->cs_confdel;
    G.g_covwrpref = cnxsave->cs_confovwr;
    G.g_cdclkpref = cnxsave->cs_dblclick;
    G.g_ctimeform = cnxsave->cs_timefmt;
    G.g_cdateform = cnxsave->cs_datefmt;
#if !MPS_BLITTER_ALWAYS_ON
#if CONF_WITH_BLITTER
    G.g_blitter   = cnxsave->cs_blitter;
#else
    G.g_blitter   = FALSE; // If no blitter, don't tick
#endif
#endif
#if CONF_WITH_CACHE_CONTROL
    G.g_cache     = cnxsave->cs_cache;
    menu_icheck(desk_rs_trees[ADMENU], CACHITEM, G.g_cache);
#endif
#if CONF_WITH_DESKTOP_CONFIG
    G.g_appdir    = cnxsave->cs_appdir;
    G.g_fullpath  = cnxsave->cs_fullpath;
#endif
#if CONF_WITH_SIZE_TO_FIT
    G.g_ifit      = cnxsave->cs_sizefit;
    menu_icheck(desk_rs_trees[ADMENU], FITITEM, G.g_ifit ? 1 : 0);
#endif
    G.g_cdclkpref = evnt_dclick(G.g_cdclkpref, TRUE);

    /* DESKTOP v1.2: Remove 2-window limit; and cnx_open() inlined. */
    for (nw = 0; nw < NUM_WNODES; nw++)
    {
        pws = &cnxsave->cs_wnode[nw];

        /* Check for valid position */
        if (pws->x_save >= G.g_desk.g_w)
        {
            pws->x_save = G.g_desk.g_w/2;
        }
        if (pws->y_save >= G.g_desk.g_h)
        {
            pws->y_save = G.g_desk.g_h/2;
        }

        /* Check for valid width + height */
        if (pws->w_save <= 0 || pws->w_save > G.g_desk.g_w)
        {
            pws->w_save = G.g_desk.g_w;
        }
        if (pws->h_save <= 0 || pws->h_save > G.g_desk.g_h)
        {
            pws->h_save = G.g_desk.g_h;
        }

        if (pws->pth_save[0])
        {
            WORD obid = obj_get_obid(pws->pth_save[0]);

            if ((pw = win_alloc(obid)))
            {
                pw->w_cvcol = pws->hsl_save;
                pw->w_cvrow = pws->vsl_save;
                do_xyfix(&pws->x_save, &pws->y_save);
                if (!do_diropen(pw, TRUE, obid, pws->pth_save, (GRECT *)pws, TRUE))
                    win_free(pw);
            }
        }
    }
}


/*
 *  Change the sizes of the menus after translation, and fix up the
 *  separator lines
 *
 *  Note - the code below is based on the assumption that the width of
 *  the system font is CHAR_WIDTH pixels (documented as such in lineavars.h)
 */
static void adjust_menu(OBJECT *obj_array)
{
#define OBJ(i) (&obj_array[i])

    int i;  /* index in the menu bar */
    int j;  /* index in the array of pull downs */
    int width = (G.g_desk.g_w >> 3);    /* screen width in chars */
    int m;  /* max width of each set of menu items, needed for separator lines */
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
     * set up the separator string that will be pointed to for all separator lines
     */
    memset(separator, '-', MAXLEN_SEPARATOR);
    separator[MAXLEN_SEPARATOR] = '\0';

    /*
     * finally we can set ob_x and ob_width for the pulldown objects within the menu,
     * and set up the separator lines
     */
    j = OBJ(menu->ob_tail)->ob_head;
    m = MIN_DESKMENU_WIDTH - 1; /* make 'Desk' menu at least as wide as Atari TOS */
    for (i = mbar->ob_head, title = OBJ(i); i <= mbar->ob_tail; i++, title++, m = 0)
    {
        int k;
        OBJECT *dropbox = OBJ(j);
        OBJECT *item;

        /* find widest object under this menu heading */
        for (k = dropbox->ob_head, item = OBJ(k); k <= dropbox->ob_tail; k++, item++)
        {
            int l = strlen((char *)item->ob_spec);
            if (m < l)
                m = l;
        }
        /* force byte alignment for faster display */
        dropbox->ob_x = (mbar->ob_x + title->ob_x) & 0x00ff;

        /* set up separator lines */
        for (k = dropbox->ob_head, item = OBJ(k), m++; k <= dropbox->ob_tail; k++, item++)
        {
            if (*(char *)(item->ob_spec) == '-')
                item->ob_spec = (LONG)(separator+MAXLEN_SEPARATOR-m);
        }

        /* make sure the menu is not too far on the right of the screen */
        if (dropbox->ob_x + m >= width)
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

#if CONF_WITH_3D_OBJECTS
/*
 *  Perform any final position tweaks for 3D objects
 */
static void adjust_3d_positions(void)
{
    OBJECT *tree = desk_rs_trees[ADDESKCF];

    /*
     * adjust Desktop configuration dialog
     */
    tree[DCFUNPRV].ob_x -= 3 * ADJ3DSTD;    /* avoid button overlap */
    tree[DCMNUPRV].ob_x -= 3 * ADJ3DSTD;
}
#endif

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
 *  To allow centre- or right-alignment of text objects,
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
 *  Align dialog title: this is done dynamically to handle translated titles
 *
 *  If object 1 of a tree is a G_STRING and its y position equals
 *  one character height, we assume it's the title.
 *
 *  If CONF_WITH_ALT_DESKTOP_GRAPHICS is specified, titles are left-aligned
 *  & the ob_state is set up to generate an underline if the AES supports it.
 *  Otherwise titles are centre-aligned without underlining, like Atari TOS.
 */
void align_title(OBJECT *root)
{
    OBJECT *title;

    title = root + 1;

    if ((title->ob_type == G_STRING) && (title->ob_y == gl_hchar))
    {
#if CONF_WITH_ALT_DESKTOP_GRAPHICS
        title->ob_x = gl_wchar;
        title->ob_width = root->ob_width - (gl_wchar * 2);
        title->ob_state |= (0xFF00|WHITEBAK);
#else
        WORD len = strlen((char *)title->ob_spec) * gl_wchar;
        if (len > root->ob_width)
            len = root->ob_width;
        title->ob_x = (root->ob_width - len) / 2;
#endif
    }
}

/*
 *  copy menu item strings from ROM to RAM
 */
static WORD copy_menu_items(void)
{
    OBJECT *tree = desk_rs_trees[ADMENU];
    WORD dropdowns, item_root, item;
    WORD count, width, max_width, size = 0;
    char *p, *q;

    dropdowns = tree->ob_tail;

    /* calculate the amount of memory required */
    for (item_root = tree[ITEM_ROOT].ob_next; item_root != dropdowns;
                                                    item_root = tree[item_root].ob_next)
    {
        count = 0;
        max_width = 0;
        for (item = tree[item_root].ob_head; item != item_root; item = tree[item].ob_next)
        {
            q = (char *)tree[item].ob_spec;
            if (*q != '-')          /* not a separator */
            {
                count++;
                width = strlen(q);
                if (width > max_width)
                    max_width = width;
            }
        }
        max_width += 1 + SHORTCUT_SIZE;
        tree[item_root].ob_width = max_width;   /* remember for this menu */
        size += count * max_width;
    }

    /* allocate space for strings */
    desk_rs_strings = dos_alloc_anyram(size);
    if (!desk_rs_strings)
        return -1;

    /* copy strings to RAM, padding with spaces & adjusting object pointers */
    p = desk_rs_strings;
    for (item_root = tree[ITEM_ROOT].ob_next; item_root != dropdowns;
                                                    item_root = tree[item_root].ob_next)
    {
        width = tree[item_root].ob_width;
        for (item = tree[item_root].ob_head; item != item_root; item = tree[item].ob_next)
        {
            q = (char *)tree[item].ob_spec;
            if (*q != '-')      /* not a separator */
            {
                strcpy(p, q);   /* copy string, pad with spaces */
                for (q = p+strlen(p); q < p+width-1; )
                    *q++ = ' ';
                *q++ = '\0';
                tree[item].ob_spec = (LONG)p;
                p = q;
            }
            tree[item].ob_width = width;    /* width-adjust all items */
        }
    }

    return 0;
}


/*
 *  install menu shortcuts
 *
 *  the shortcut is inserted (as ^X) at the end of the menu item text,
 *  with one space following
 *
 *  note: when this is called, ob_width has already been converted to pixels
 */
void install_shortcuts(void)
{
    OBJECT *obj, *tree = desk_rs_trees[ADMENU];
    char *p, c;
    WORD i, n;

    for (i = 0; i < NUM_SHORTCUTS; i++)
    {
        n = shortcut_mapping[i];
        /*
         * check if menu item exists for this shortcut position; if not,
         * we must clear any associated shortcut to ensure that the
         * shortcut array matches the displayed menu items
         */
        if (!n)
        {
            menu_shortcuts[i] = 0x00;
            continue;
        }
        obj = tree + n;
        p = (char *)obj->ob_spec + (obj->ob_width+CHAR_WIDTH-1)/CHAR_WIDTH - SHORTCUT_SIZE;
        c = menu_shortcuts[i];
        if (c)  /* there is a shortcut, insert in menu item */
        {
            *p++ = '^';
            *p = c;
        }
        else    /* there is no shortcut, blank out any existing one */
        {
            *p++ = ' ';
            *p = ' ';
        }
    }
}


/*
 *  translate and fixup desktop objects
 */
static WORD desk_xlate_fix(void)
{
    OBJECT *tree = desk_rs_trees[ADDINFO];
    OBJECT *objlabel = &tree[DELABEL];
    OBJECT *objversion = &tree[DEVERSN];
    OBJECT *objyear = &tree[DECOPYRT];
    int i;

    /* translate strings in objects */
    xlate_obj_array(desk_rs_obj, RS_NOBS);

    /* copies menu item strings to RAM */
    if (copy_menu_items() < 0)
        return -1;

    /* insert the version number */
    objversion->ob_spec = (LONG) version;

    /* slightly adjust the about box for a timestamp build */
    if (version[1] != '.')
    {
        objlabel->ob_flags |= HIDETREE;   /* hide the word "Version" */
        objversion->ob_x = 0;             /* and enlarge the version object */
        objversion->ob_width = 40;
        objversion->ob_flags |= CENTRE_ALIGNED;
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

#if CONF_WITH_3D_OBJECTS
    adjust_3d_positions();
#endif

    /*
     * perform special object alignment - this must be done after
     * translation and coordinate fixing
     */
    align_objects(desk_rs_obj, RS_NOBS);

    return 0;
}


#if CONF_WITH_READ_INF
/*
 *  Close desktop windows
 */
void static close_desktop_windows(void)
{
    WNODE *pw;

    while( (pw=win_ontop()) )
    {
        wind_close(pw->w_id);           /* close the window */
        dos_free(pw->w_pnode.p_fbase);  /* free the fnodes */
        win_free(pw);                   /* free the wnode */
    }
}
#endif


/*
 *  Run desktop
 */
BOOL deskmain(void)
{
    WORD ii, done, flags;
    UWORD ev_which, mx, my, button, kstate, kret, bret;
    BOOL rc;
    OBJECT *menutree;

#if CONF_WITH_READ_INF
    restart_desktop = FALSE;
#endif

    /* remember start drive */
    G.g_stdrv = dos_gdrv();

    /* initialize libraries */
    gl_apid = appl_init();

    /* get GEM's gsx handle */
    gl_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);

    /* get desktop work area coordinates */
    wind_get_grect(DESKWH, WF_WXYWH, &G.g_desk);

    /* initialize mouse     */
    wind_update(BEG_UPDATE);
    desk_busy_on();
    wind_update(END_UPDATE);

    /* detect optional features */
    detect_features();

    /* allocate desktop context save area */
    G.g_cnxsave = dos_alloc_anyram(sizeof(CSAVE));
    if (!G.g_cnxsave)
    {
        KDEBUG(("insufficient memory for desktop prefs save area (need %ld bytes)\n",
                (LONG)sizeof(CSAVE)));
        nomem_alert();              /* infinite loop */
    }
    bzero(G.g_cnxsave, sizeof(CSAVE));

    /* initialize resources */
    if (desk_rs_init() < 0)         /* copies OBJECTs and TEDINFOs in ROM to RAM */
    {
        KDEBUG(("insufficient memory for desktop objects (need %ld bytes)\n",
                (LONG)RS_NOBS*sizeof(OBJECT)));
        nomem_alert();              /* infinite loop */
    }
    if (desk_xlate_fix() < 0)       /* translates & fixes desktop */
    {
        KDEBUG(("insufficient memory for desktop strings\n"));
        nomem_alert();          /* infinite loop */
    }

    /* initialize menus and dialogs */
    for (ii = 0; ii < RS_NTREE; ii++)
    {
        align_title(desk_rs_trees[ii]);
    }

    for (ii = 0; ii < RS_NBB; ii++) /* initialize bit images */
    {
        app_tran(ii);
    }

    /* These strings are used by dr_code.  We can't get to the
     * resource in dr_code because that would reenter AES, so we
     * save them here.
     */
    strcpy(gl_amstr, desktop_str_addr(STAM));
    strcpy(gl_pmstr, desktop_str_addr(STPM));

    /*
     * initialize the keyboard shortcut array (may be updated by app_start())
     */
    memcpy(menu_shortcuts, default_shortcuts, NUM_SHORTCUTS);

    /* Initialize icons and apps from memory, or EMUDESK.INF,
     * or builtin defaults
     */
    app_start();

    /*
     * install the keyboard shortcuts in the menu strings
     */
    install_shortcuts();

    /*
     * initialize windows: win_view() initialises g_iview, g_isort
     */
    if (win_start() < 0)
    {
        KDEBUG(("insufficient memory for desktop windows (need %ld bytes)\n",
                (LONG)NUM_WNODES*sizeof(WNODE)));
        nomem_alert();              /* infinite loop */
    }

    desk_verify(DESKWH, FALSE);     /* initialise g_croot, g_cwin, g_wlastsel  */

    /* establish menu items */
    menutree = desk_rs_trees[ADMENU];
    menu_icheck(menutree, ICONITEM+G.g_iview, 1);
    menu_icheck(menutree, NAMEITEM+G.g_isort, 1);
#if CONF_WITH_SIZE_TO_FIT
    menu_icheck(menutree, FITITEM, G.g_ifit);
#endif

    /* initialize desktop and its objects */
    app_blddesk();

    /* Take over the desktop */
    wind_set(DESKWH, WF_NEWDESK, G.g_screen, 1, 0);

    /* establish desktop's state from info found in app_start,
     * open windows
     */
    wind_update(BEG_UPDATE);
    cnx_get();
    wind_update(END_UPDATE);

#if CONF_WITH_BLITTER && !MPS_BLITTER_ALWAYS_ON
    /*
     * we now have the desired blitter state from EMUDESK.INF, so we can
     * call Blitmode() here.  note that we call it here, even if we've
     * called it in process_inf2() in geminit.c, because an auto-run
     * program may have changed the state in between.
     */
    if (blitter_is_present)
        Blitmode(G.g_blitter?1:0);
#endif

    men_update();

    /* menu is initialised - display menu bar & set mouse to arrow */
    wind_update(BEG_UPDATE);
    menu_bar(menutree, 1);
    desk_busy_off();
    wind_update(END_UPDATE);

    /* get ready for main loop */
    flags = MU_BUTTON | MU_MESAG | MU_KEYBD;
    done = FALSE;

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
            if (hndl_kbd(kret))
                done = TRUE;

        /* handle button down */
        if (ev_which & MU_BUTTON)
            if (hndl_button(bret, mx, my, button, kstate))
                done = TRUE;

        /* handle system message */
        while (ev_which & MU_MESAG)
        {
            if (done)
                break;
            if (hndl_msg())
                done = TRUE;
            /* use quick-out to clean out all messages */
            ev_which = evnt_multi(MU_MESAG | MU_TIMER, 0x02, 0x01, 0x01,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                G.g_rmsg, 0, 0,
                                &mx, &my, &button, &kstate, &kret, &bret);
        }

        /* free the screen      */
        wind_update(END_UPDATE);
    }

    /*
     * we are about to exit deskmain()
     *
     * if we are exiting because of a successful "Read INF file", we must
     * close all the windows & free all the memory we allocated above,
     * since we will re-enter deskmain() immediately.
     *
     * in all other cases, the desktop process will terminate and the
     * allocated memory will be automatically freed.  we must save the
     * desktop state in memory for when the desktop process restarts.
     */
#if CONF_WITH_READ_INF
    if (restart_desktop)
    {
        desk_busy_on();
        close_desktop_windows();    /* close our windows */
        dos_free(G.g_wlist);        /* free the windows */
        dos_free(G.g_screen);       /* the screen objects */
        dos_free(G.g_iblist);       /* the iconblks for the desktop icons */
        dos_free(G.g_alist);        /* the anodes */
        dos_free(G.g_atext);        /* the anode text buffer */
        dos_free(G.g_cnxsave);      /* the context save area */
        dos_free(desk_rs_obj);      /* the RAM copies of the Emudesk resource objects */
        dos_free(desk_rs_strings);  /* the RAM copies of the EmuDesk menu item strings */
        desk_busy_off();
        rc = FALSE;                 /* _deskstart will call us again immediately */
    }
    else
#endif
    {
        cnx_put();
        app_save(FALSE);
        rc = TRUE;                  /* _deskstart will terminate the process */
    }

    /* turn off the menu bar */
    menu_bar(NULL, 0);

    /* exit the gem AES */
    appl_exit();

    if (rc == FALSE)                /* restarting */
        wind_new();                 /* clean up AES windows */

    return rc;
}
