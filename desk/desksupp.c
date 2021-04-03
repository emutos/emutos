/*      DESKSUPP.C      05/04/84 - 06/20/85     Lee Lorenzen            */
/*      for 3.0 (xm)    3/12/86  - 01/17/87     MDF                     */
/*      for 3.0                 11/13/87                mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "string.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "gemdos.h"
#include "rectfunc.h"
#include "optimize.h"
#include "biosbind.h"
#include "biosdefs.h"
#include "xbiosbind.h"
#include "gemerror.h"
#include "cookie.h"

#include "aesdefs.h"
#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "aesbind.h"
#include "deskact.h"
#include "deskpro.h"
#include "deskinf.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskmain.h"
#include "deskdir.h"
#include "desksupp.h"
#include "deskins.h"
#include "deskobj.h"
#include "nls.h"
#include "scancode.h"
#include "biosext.h"
#include "lineavars.h"      /* for MOUSE_BT */

/* Needed to force media change */
#define MEDIACHANGE     0x02
#define READSEC         0x00

#if CONF_WITH_FORMAT
/*
 *      declarations used by the do_format() code
 */
#define MAXTRACK        80
#define FMTBUFLEN       12500       /* sufficient for HD */
#define VIRGIN          0xe5e5
#define RANDOM_SERIAL   0x01000000L
#define WRITESEC        0x03        /* no media change detection */
#define FA_VOL          0x08        /* volume label attribute */

static const WORD std_skewtab[] =
 { 1, 2, 3, 4, 5, 6, 7, 8, 9,
   1, 2, 3, 4, 5, 6, 7, 8, 9 };

static const WORD hd_skewtab[] =
 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };

#endif

#if CONF_WITH_SHOW_FILE || CONF_WITH_PRINTER_ICON
/*
 *      declarations used by show_file() or print_file()
 */
#define SETPRT_SERIAL   0x10    /* bit set by Setprt() to request serial o/p */
#define FF              0x0c    /* standard form feed code */
#define CHECK_COUNT     16      /* how often to check keyboard when printing */
#endif

#if CONF_WITH_SHOW_FILE
/*
 *      declarations used by show_file() only
 */
#define IOBUFSIZE   16384L
static LONG linecount;
static WORD pagesize;
#endif


/*
 *  Deselect all objects in specified tree
 */
void deselect_all(OBJECT *tree)
{
    OBJECT *obj = tree;
    do {
        obj->ob_state &= ~SELECTED;
    } while(!(obj++->ob_flags&LASTOB));
}


/*
 *  Clear out the selections for this particular window
 */
void desk_clear(WORD wh)
{
    WNODE *pw;
    GRECT c;
    WORD root = DROOT;

    if (wh != DESKWH)       /* not the desktop */
    {
        pw = win_find(wh);  /* find its tree of items */
        if (pw)
            root = pw->w_root;
    }

    /*
     * if 'root' is still DROOT, then either the 'window' is the desktop
     * (wh==DESKWH), or something is wrong with the window setup.  to handle
     * the latter case, we force the handle to the desktop anyway for safety.
     */
    if (root == DROOT)
        wh = DESKWH;

    /* get current size */
    wind_get_grect(wh, WF_WXYWH, &c);

    /* clear all selections */
    act_allchg(wh, root, 0, &gl_rfull, &c, FALSE);

    if (wh != DESKWH)           /* not the desktop */
    {
        win_sinfo(pw, TRUE);    /* may need to update info line */
    }
}


/*
 *  Clear out selections for all desktop windows
 *  (including the desktop itself)
 */
void desk_clear_all(void)
{
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
        if (pw->w_id)
            desk_clear(pw->w_id);

    desk_clear(DESKWH);
}


/*
 *  Verify window display by building a new view
 */
void desk_verify(WORD wh, WORD changed)
{
    WNODE *pw;
    GRECT clip;

    if (wh != DESKWH)
    {
        /* get current size */
        pw = win_find(wh);
        if (pw)
        {
            if (changed)
            {
                wind_get_grect(wh, WF_WXYWH, &clip);
                win_bldview(pw, &clip);
            }
            G.g_croot = pw->w_root;
        }
    }
    else G.g_croot = DROOT;     /* DESK v1.2: The Desktop */

    G.g_cwin = wh;
    G.g_wlastsel = wh;
}



void do_wredraw(WORD w_handle, GRECT *gptr)
{
    GRECT clip_r, t;
    WNODE *pw;
    WORD root;

    clip_r = *gptr;

    root = DROOT;
    if (w_handle != DESKWH)
    {
        pw = win_find(w_handle);
        if (pw)
            root = pw->w_root;
    }

    G.g_idt = Supexec((LONG)get_idt_cookie);    /* current _IDT for format_fnode() */

    graf_mouse(M_OFF, NULL);

    wind_get_grect(w_handle, WF_FIRSTXYWH, &t);
    while(t.g_w && t.g_h)
    {
        if (rc_intersect(&clip_r, &t))
            objc_draw(G.g_screen, root, MAX_DEPTH, t.g_x, t.g_y, t.g_w, t.g_h);
        wind_get_grect(w_handle, WF_NEXTXYWH, &t);
    }

    graf_mouse(M_ON, NULL);
}


/*
 *  Picks ob_x, ob_y, ob_width, ob_height fields out of object list
 */
static void get_xywh(OBJECT olist[], WORD obj, WORD *px, WORD *py,
                     WORD *pw, WORD *ph)
{
    *px = olist[obj].ob_x;
    *py = olist[obj].ob_y;
    *pw = olist[obj].ob_width;
    *ph = olist[obj].ob_height;
}


/*
 *  Returns ICONBLK pointer from object's ob_spec field
 */
static ICONBLK *get_iconblk_ptr(OBJECT olist[], WORD obj)
{
    return (ICONBLK *)olist[obj].ob_spec;
}


void do_xyfix(WORD *px, WORD *py)
{
    *px = (*px + 8) & 0xfff0;   /* horizontally align to nearest word boundary */
    if (*py < G.g_desk.g_y)     /* ensure it's below menu bar */
        *py = G.g_desk.g_y;
}


/*
 * open a window, normally corresponding to a disk drive icon on the desktop
 *
 * if curr <= 0, there is no 'source' screen object from which the new
 * object is coming, so we do not do the zoom effect & we do not try to
 * reset the object state.
 *
 * if curr > 0, there *is* a source object: we always do the zoom effect,
 * and change the object state, but we only redraw the object when we are
 * opening a new window.  otherwise, we must be showing the new data in
 * an existing window: the FNODE for the 'source' object has already been
 * freed via pn_close(), but would be needed for the redraw because:
 *  . in text mode, a redraw will cause the userdef code for text display
 *    to access the FNODE corresponding to the source object
 *  . in icon mode, a redraw will use the name from the FNODE as the icon
 *    name.
 * if we did allow a redraw, at best the display would show the wrong
 * values (or garbage) briefly; at worst, the desktop would crash.
 */
void do_wopen(WORD new_win, WORD wh, WORD curr, GRECT *pt)
{
    GRECT t;
    GRECT c, d;

    t = *pt;

    do_xyfix(&t.g_x, &t.g_y);

    if (curr > 0)
    {
        /*
         * get coordinates of current window & current object within window
         * and adjust object x/y to screen-relative values.  note that this
         * works ok even if the current window is the desktop pseudo-window.
         */
        get_xywh(G.g_screen, G.g_croot, &c.g_x, &c.g_y, &c.g_w, &c.g_h);
        get_xywh(G.g_screen, curr, &d.g_x, &d.g_y, &d.g_w, &d.g_h);
        d.g_x += c.g_x;     /* convert window to screen coordinates */
        d.g_y += c.g_y;

        graf_growbox_grect(&d, &t);
        act_chg(G.g_cwin, G.g_croot, curr, &gl_rfull, FALSE, new_win?TRUE:FALSE);
    }

    if (new_win)
        wind_open_grect(wh, &t);

    G.g_wlastsel = wh;
}


/*
 * implements FULLER widget
 */
void do_wfull(WORD wh)
{
    GRECT curr, prev, full;

    wind_get_grect(wh, WF_CXYWH, &curr);
    wind_get_grect(wh, WF_PXYWH, &prev);
    wind_get_grect(wh, WF_FXYWH, &full);

    if (rc_equal(&curr, &full)) /* currently full, so shrink */
    {
        wind_set_grect(wh, WF_CXYWH, &prev);
        graf_shrinkbox_grect(&prev, &full);
        return;
    }

    graf_growbox_grect(&curr, &full);
    wind_set_grect(wh, WF_CXYWH, &full);
}


/*
 *  test if specified file exists
 *
 *  returns ptr to DTA if it exists, else NULL
 */
static DTA *file_exists(char *path, char *name)
{
    DTA *dta;
    WORD rc;
    char fullname[MAXPATHLEN];

    desk_busy_on();     /* display busy, in case we're on a slow drive (floppy) */

    strcpy(fullname, path);
    if (name)
        strcpy(filename_start(fullname),name);
    dta = dos_gdta();
    dos_sdta(&G.g_wdta);
    rc = dos_sfirst(fullname, ALLFILES);
    dos_sdta(dta);

    desk_busy_off();

    return rc ? NULL : &G.g_wdta;
}


#if CONF_WITH_DESKTOP_SHORTCUTS
/*
 *  Prompt to Remove or Locate a shortcut
 */
void remove_locate_shortcut(WORD curr)
{
    WORD rc, button;
    ANODE *pa;
    char path[MAXPATHLEN];
    char fname[LEN_ZFNAME], *p;

    pa = app_afind_by_id(curr);
    if (!pa)        /* can't happen */
        return;

    rc = fun_alert_merge(1, STRMVLOC, filename_start(pa->a_pdata));
    switch(rc)
    {
    case 1:             /* Remove */
        app_free(pa);
        app_blddesk();
        break;
    case 2:             /* Locate */
        build_root_path(path, 'A'+G.g_stdrv);
        fname[0] = '\0';
        p = desktop_str_addr(STLOCATE);
        rc = fsel_exinput(path, fname, &button, p);
        if ((rc == 0) || (button == 0))
            break;
        p = filename_start(path);
        if (pa->a_type == AT_ISFILE)
            strcpy(p, fname);
        else
            *(p-1) = '\0';
        scan_str(path, &pa->a_pappl);
    }
}
#endif


/*
 *  Open a directory, it may be the root or a subdirectory
 *
 *  Note that we currently force the filename specification in the
 *  pathname to *.*
 */
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon,
                char *pathname, GRECT *pt, WORD redraw)
{
    WORD ret;

    /* convert to hourglass */
    desk_busy_on();

    /* open a path node */
    if (!pn_open(pathname, pw)) /* pathname is too long */
    {
        KDEBUG(("Pathname is too long\n"));
        desk_busy_off();
        return FALSE;
    }

    /* activate path by search and sort of directory */
    ret = pn_active(&pw->w_pnode, TRUE);
    if (ret < 0)    /* error reading directory */
    {
        KDEBUG(("Error reading directory %s\n",pathname));
        pn_close(&pw->w_pnode);
        desk_busy_off();
        return FALSE;
    }

    /* set new name and info lines for window */
    win_sname(pw);
    win_sinfo(pw, FALSE);
    wind_set(pw->w_id, WF_NAME, pw->w_name, 0, 0);

    /* do actual wind_open  */
    do_wopen(new_win, pw->w_id, curr_icon, pt);
    if (new_win)
        win_top(pw);

    /*
     * verify contents of window's object list
     * by building view and make it curr.
     */
    desk_verify(pw->w_id, TRUE);

    /* make it redraw */
    if (redraw && !new_win)
        fun_msg(WM_REDRAW, pw->w_id, pt->g_x, pt->g_y, pt->g_w, pt->g_h);

    desk_busy_off();

    return TRUE;
}


#if CONF_WITH_SHOW_FILE || CONF_WITH_PRINTER_ICON
/*
 *  helper functions for displaying or printing a file
 */

/*
 *  receive a character via the BIOS
 */
static LONG bios_conin(void)
{
    return Bconin(2);
}

/*
 *  test if character is available via the BIOS
 */
static WORD bios_conis(void)
{
    return (WORD) Bconstat(2);
}

/*
 *  send a character to the printer
 */
static WORD bios_prnout(WORD device, WORD ch)
{
    return Bconout(device, ch);
}

/*
 *  get key from keyboard, or equivalent mouse button
 *
 *  if ASCII (1-255), returns value in low-order byte, 0 in high-order byte
 *  else returns scancode
 */
static WORD get_key(void)
{
    ULONG c;

    while(!bios_conis())
    {
        if (MOUSE_BT & 0x0002)  /* right mouse button means quit */
            return 'Q';
        if (MOUSE_BT & 0x0001)  /* left mouse button means next page */
            return ' ';
    }

    c = bios_conin();

    if (c & 0xff)           /* ASCII ? */
        c &= 0xff;          /* yes, just return the ASCII value */
    else
        c >>= 8;            /* convert to scancode */

    return (WORD)c;
}

/*
 *  check for quit (ctl-C/Q/q/UNDO) and (optionally) handle flow control
 *
 *  a +ve argument is the character to check
 *  a -ve argument means get the character from the console
 */
static WORD user_input(WORD c, BOOL flow_control)
{
    if (c < 0)
        c = get_key();

    if ((c == CTL_C) || (c == 'Q') || (c == 'q') || (c == UNDO))    /* wants to quit */
        return -1;

    if (!flow_control)
        return 0;

    if (c == CTL_S)         /* user wants to pause */
    {
        while(1)
        {
            c = get_key();
            if (c == CTL_C)
                return -1;
            if (c == CTL_Q)
                break;
        }
    }

    return 0;
}

/*
 *  return TRUE iff user wants to quit printing this file
 */
static BOOL user_quit(void)
{
    if (bios_conis())
        if (user_input(-1, FALSE) < 0)  /* no flow control */
            return TRUE;

    return FALSE;
}

/*
 *  print a fixed-length buffer
 *
 *  returns +1 if user interrupt or quit
 *          0 otherwise
 */
static WORD print_buf(WORD device,const char *s,LONG len)
{
    WORD charcount = 0;
    char c;

    while(len-- > 0)
    {
        /* like Atari TOS, only check for user input 'occasionally' */
        if (++charcount > CHECK_COUNT)
        {
            charcount = 0;
            if (user_quit())
                return 1;
        }

        c = *s++;
        while(bios_prnout(device, c) == 0)
        {
            desk_busy_off();
            if (fun_alert(1, STPRTERR) != 1)    /* retry or cancel? */
                return 1;
            desk_busy_on();       /* we're busy again */
        }
    }

    return 0;
}

/*
 *  print a text file, allowing user cancel
 *
 *  returns FALSE iff user cancel
 */
BOOL print_file(char *name,LONG bufsize,char *iobuf)
{
    OBJECT *tree;
    LONG rc, n;
    WORD handle, device;

    rc = dos_open(name,0);
    if (rc < 0L)
    {
        form_error(2);  /* file not found */
        return TRUE;
    }

    handle = (WORD)rc;

    /* open dialog, set busy cursor */
    tree = desk_rs_trees[ADPRINT];
    set_tedinfo_name(tree, PRNAME, filename_start(name));
    start_dialog(tree);

    graf_mouse(HGLASS,NULL);    /* say we're busy */

    /* determine whether to use serial or parallel port */
    device = (Setprt(-1) & SETPRT_SERIAL) ? 1 : 0;

    while(1)
    {
        n = rc = dos_read(handle,bufsize,iobuf);
        if (rc <= 0L)
            break;
        rc = print_buf(device, iobuf, n);
        if (rc > 0L)
            break;
    }

    /* if not user cancel, do a form feed */
    if (rc <= 0L)
        bios_prnout(device, FF);

    dos_close(handle);

    /* close dialog, reset mouse cursor */
    end_dialog(tree);
    desk_busy_off();

    return (rc > 0L) ? FALSE : TRUE;
}
#endif


#if CONF_WITH_SHOW_FILE
/*
 *  helper functions for displaying a file
 */

/*
 *  send a character via the BIOS
 */
static void bios_conout(WORD ch)
{
    Bconout(2, ch);
}

/*
 *  send a string via the BIOS
 */
static void bios_conws(const char *s)
{
    while(*s)
        Bconout(2, *s++);
}

/*
 *  blank out line via VT52 escape sequence
 */
static void blank_line(void)
{
    bios_conout('\x1b');
    bios_conout('l');
}

/*
 *  clear screen via VT52 escape sequence
 */
static void clear_screen(void)
{
    bios_conout('\x1b');
    bios_conout('E');
}

/*
 *  display a fixed-length buffer with screen paging
 *
 *  returns +1 if user interrupt or quit
 *          0 otherwise
 */
static WORD show_buf(const char *s,LONG len)
{
    LONG n;
    WORD response;
    char c, cprev = 0;
    char *msg;

    n = len;
    while(n-- > 0)
    {
        if (bios_conis())
            if (user_input(-1, TRUE))
                return 1;

        c = *s++;
        /* convert Un*x-style text to TOS-style */
        if ((c == '\n') && (cprev != '\r'))
            bios_conout('\r');
        bios_conout(c);
        if (c == '\n')
        {
            if (++linecount >= pagesize)
            {
                msg = desktop_str_addr(STMORE);
                bios_conws(msg);            /* "-More-" */
                while(1)
                {
                    response = get_key();
                    if (response == '\r')   /* CR displays the next line */
                        break;
                    if (response == ' ')    /* space displays the next page */
                    {
                        linecount = 0L;
                        break;
                    }
                    if ((response == 'D') || (response == 'd') || (response == CTL_D))
                    {                       /* D, d, or ^D displays half a page */
                        linecount = pagesize / 2;
                        break;
                    }
                    if (user_input(response, TRUE))
                    {
                        bios_conout('\r');
                        return 1;
                    }
                }
                blank_line();               /* overwrite the pause msg */
            }
        }
        cprev = c;
    }

    return 0;
}

/*
 *  show a text file with pause at end of page & EOF
 */
static void show_file(char *name,LONG bufsize,char *iobuf)
{
    LONG rc, n;
    WORD handle, scr_width, scr_height;
    char *msg;

    rc = dos_open(name,0);
    if (rc < 0L)
    {
        form_error(2);  /* file not found */
        return;
    }

    handle = (WORD)rc;

    scr_width = G.g_desk.g_w;
    scr_height = G.g_desk.g_y + G.g_desk.g_h;

    /*
     * set up for text output
     */
    graf_mouse(M_OFF, NULL);
    menu_bar(desk_rs_trees[ADMENU],0);
    wind_update(BEG_UPDATE);
    form_dial(FMD_START, 0,0,0,0, 0,0,scr_width,scr_height);
    clear_screen();

    pagesize = (G.g_desk.g_y+G.g_desk.g_h)/gl_hchar - 1;
    linecount = 0L;

    while(1)
    {
        n = rc = dos_read(handle,bufsize,iobuf);
        if (rc <= 0L)
            break;
        rc = show_buf(iobuf,n);
        if (rc > 0L)
            break;
    }

    dos_close(handle);

    if (rc <= 0L)   /* not user quit */
    {
        bios_conout('\n');
        msg = desktop_str_addr((rc==0L)?STEOF:STFRE);
        blank_line();
        bios_conws(msg);    /* "-End of file-" or "-File read error-" */
        get_key();
    }

    /*
     * switch back to normal desktop
     */
    clear_screen();     /* neatness */
    form_dial(FMD_FINISH, 0,0,0,0, 0,0,scr_width,scr_height);
    wind_update(END_UPDATE);
    menu_bar(desk_rs_trees[ADMENU],1);
    graf_mouse(M_ON, NULL);
}
#endif


/*
 *  Open an application
 *
 *  This may be called via the Open item under the File menu, or by
 *  double-clicking an icon, or by pressing a function key, or by
 *  dropping a file on to a desktop shortcut for a program
 *
 *  returns TRUE iff shel_write() was issued successfully
 */
WORD do_aopen(ANODE *pa, BOOL isapp, WORD curr, char *pathname, char *pname, char *tail)
{
    WNODE *pw;
    WORD ret;
    WORD isgraf, isparm, installed_datafile;
    char *ptail, *p;
    char app_path[MAXPATHLEN];

#if CONF_WITH_DESKTOP_SHORTCUTS
    /*
     * if this is a desktop shortcut, first make sure that the file
     * exists.  then look for a corresponding installed application
     * and, if there is one, use its ANODE instead.
     *
     * note that app_afind_by_name() only updates 'isapp' if an
     * application is found, so it is safe to pass a pointer to it.
     */
    if (pa->a_flags & AF_ISDESK)
    {
        ANODE *tmp;

        if (!file_exists(pathname, pname))
        {
            remove_locate_shortcut(curr);
            return FALSE;
        }
        tmp = app_afind_by_name(AT_ISFILE, AF_ISDESK|AF_WINDOW|AF_VIEWER, pathname, pname, &isapp);
        if (tmp)
            pa = tmp;
    }
#endif

    /* set flags */
    isgraf = pa->a_flags & AF_ISCRYS;
    isparm = pa->a_flags & AF_ISPARM;
    installed_datafile = (is_installed(pa) && !isapp);
    pw = win_ontop();

    /*
     * update the current directory
     */
    if (is_installed(pa))
    {
        /*
         * if the application flags specify 'top window' and one exists,
         * we use it; otherwise we use the application directory
         */
        if (!(pa->a_flags & AF_APPDIR) && pw)
            p = pw->w_pnode.p_spec;
        else
            p = pa->a_pappl;
    }
    else
    {
        /*
         * if the desktop config flags specify 'top window' and one exists,
         * we use it; otherwise we use the application directory
         */
        if (!G.g_appdir && pw)
            p = pw->w_pnode.p_spec;
        else
            p = pathname;
    }
    strcpy(app_path, p);
    p = filename_start(app_path);
    *p = '\0';

    /* exit with error if we can't set the default directory */
    if (set_default_path(app_path) < 0)
    {
        fun_alert(1, STDEFDIR);
        return FALSE;
    }

    G.g_work[1] = '\0';
    ptail = G.g_work + 1;   /* arguments go here */

    /*
     * see if application was selected directly or via a
     * data file with an associated primary application
     */
    if (installed_datafile)
    {
        /*
         * the user has selected a file with an extension that matches
         * an installed application.
         *
         * first check that the application exists, since it may be being
         * invoked by an outdated entry for an installed application.
         */
        if (!file_exists(pa->a_pappl, NULL))
        {
            fun_alert_merge(1, STFILENF, filename_start(pa->a_pappl));
            return FALSE;
        }
        /*
         * set up to open the application, with a command tail based on
         * the application flags.
         */
        strcpy(ptail,pa->a_pargs);
        p = ptail + strlen(ptail);

        if (pa->a_flags & AF_ISFULL)
        {
            strcpy(p,pathname); /* build full path string */
            p = filename_start(p);
        }
        strcpy(p,pname);        /* the filename always goes on the end */
        return pro_run(isgraf, pa->a_pappl, G.g_work, G.g_cwin, curr);
    }

    /*
     * the file was selected directly, perhaps by dropping another file
     * on to it.  first, build full pathname for pro_run() or show_file()
     */
    strcpy(app_path, pathname);
    p = filename_start(app_path);
    strcpy(p, pname);

    /*
     * if the selected file is an application, run it
     */
    if (isapp)
    {
        ret = TRUE;
#if CONF_WITH_DESKTOP_SHORTCUTS
        if (tail)
        {
            /*
             * the user has dropped a file on to an application,
             * so we already know the tail to pass
             */
            strcpy(ptail, tail);
        } else
#endif
        if (isparm)
        {
            /*
             * the user has selected a .TTP or .GTP application,
             * so we need to prompt for the parameters to pass
             */
            ret = opn_appl(pname, ptail);
        }
        return ret ? pro_run(isgraf, app_path, G.g_work, G.g_cwin, curr) : FALSE;
    }

    /*
     * the user has selected a file which is not an application and
     * which does not have an extension that matches a 'normal' installed
     * application. if configured, we run the default viewer (if present).
     */
#if CONF_WITH_VIEWER_SUPPORT
    pa = app_afind_viewer();
    if (pa)
    {
        strcpy(ptail, app_path);
        KDEBUG(("Running default viewer %s: isgraf=%d, tail=%s\n",
                pa->a_pappl,pa->a_flags&AF_ISCRYS,ptail));
        return pro_run(pa->a_flags&AF_ISCRYS, pa->a_pappl, G.g_work, G.g_cwin, curr);
    }
#endif

    /*
     * the user has selected a file which is not an application and
     * which does not have an extension that matches any installed
     * application. if configured, we prompt for Show or Print.
     */
#if CONF_WITH_SHOW_FILE
    ret = fun_alert(1, STSHOW);
    if (ret != 3)   /* user said "Show" or "Print" */
    {
        char *iobuf = dos_alloc_anyram(IOBUFSIZE);
        if (iobuf)
        {
            if (ret == 1)
                show_file(app_path, IOBUFSIZE, iobuf);
            else
                print_file(app_path, IOBUFSIZE, iobuf);
            dos_free(iobuf);
        }
        else
            malloc_fail_alert();
    }
#else
    fun_alert(1, STNOAPPL);
#endif

    return FALSE;
}


/*
 *  Build root path for specified drive
 */
void build_root_path(char *path,WORD drive)
{
    char *p = path;

    *p++ = drive;
    *p++= ':';
    *p++ = '\\';
    *p = '\0';
}


/*
 *  Open a disk
 *
 *  if curr >= 0, it is a screen object id; the disk letter will be
 *  derived from the iconblk.
 *  if curr < 0, it is a negative disk letter
 */
WORD do_dopen(WORD curr)
{
    WORD drv;
    WNODE *pw;
    ICONBLK *pib;
    char path[10];

    if (curr >= 0)
    {
        pib = (ICONBLK *) get_iconblk_ptr(G.g_screen, curr);
        drv = pib->ib_char & 0x00ff;
    }
    else
    {
        drv = -curr;
        curr = obj_get_obid(drv);
    }

    if (!valid_drive(drv))
        return FALSE;

    pw = win_alloc(curr);
    if (pw)
    {
        build_root_path(path,drv);
        strcpy(path+3,"*.*");
        if (!do_diropen(pw, TRUE, curr, path, (GRECT *)&G.g_screen[pw->w_root].ob_x, TRUE))
        {
            win_free(pw);
            act_chg(DESKWH, DROOT, curr, &gl_rfull, FALSE, TRUE);
        }
    }
    else
    {
        fun_alert(1, STNOWIND);
    }

    return FALSE;
}


/*
 *  Open a folder
 *
 *  the folder is opened in a new window in two cases:
 *  1) desktop shortcuts are configured & the current window is the desktop, or
 *  2) 'allow_new_win' is TRUE and the Alt key is pressed
 */
void do_fopen(WNODE *pw, WORD curr, char *pathname, WORD allow_new_win)
{
    GRECT t;
    WORD junk, keystate, new_win = FALSE;
    char app_path[MAXPATHLEN];

    wind_get_grect(pw->w_id, WF_WXYWH, &t);

    build_root_path(app_path,pathname[0]);
    if (set_default_path(app_path) < 0L)    /* drive (no longer) valid? */
    {
        fun_close(pw, CLOSE_WINDOW);
        return;
    }

    if (strlen(pathname) >= LEN_ZPATH)
    {
        fun_alert(1, STDEEPPA);
        return;
    }

    strcpy(app_path, pathname);

#if CONF_WITH_DESKTOP_SHORTCUTS
    if (pw->w_flags & WN_DESKTOP)
    {
        /*
         * handle renamed target of shortcut
         */
        char *p = filename_start(app_path);
        *p = '\0';
        if (set_default_path(app_path) == EPTHNF)
        {
            remove_locate_shortcut(curr);
            return;
        }
        strcpy(p,"*.*");
        new_win = TRUE;
    }
    else
#endif
    if (allow_new_win)
    {
        graf_mkstate(&junk, &junk, &junk, &keystate);
        if ((keystate & MODE_SCA) == MODE_ALT)
            new_win = TRUE;
    }

    /*
     * if we are opening a folder on the desktop, or holding down only the Alt
     * key when opening a folder in a window, we need to create a new window
     */
    if (new_win)
    {
        pw = win_alloc(curr);
        if (!pw)
        {
            fun_alert(1, STNOWIND);
            return;
        }
        rc_copy((GRECT *)&G.g_screen[pw->w_root].ob_x,&t);
    }
    else
    {
        pn_close(&pw->w_pnode);
    }

    if (!do_diropen(pw, new_win, curr, app_path, &t, TRUE))
    {
        if (new_win)
            win_free(pw);
    }
}


/*
 *  Issue alert about the trash
 *
 *  The current name of the trash icon is obtained from the ANODE
 */
static void trash_alert(ANODE *pa)
{
    fun_alert_merge(1, STTRINFO, pa->a_pappl);
}


#if CONF_WITH_PRINTER_ICON
/*
 *  Issue alert about the printer
 *
 *  The current name of the printer icon is obtained from the ANODE
 */
static void printer_alert(ANODE *pa)
{
    fun_alert_merge(1, STPRINFO, pa->a_pappl);
}
#endif


/*
 *  Open an icon
 */
WORD do_open(WNODE *pwin, WORD curr)
{
    ANODE *pa;
    WNODE *pw;
    FNODE *pf;
    BOOL isapp;
    char pathname[MAXPATHLEN];
    char filename[LEN_ZFNAME];

    /*
     * if the icon is on the desktop, we get the ANODE from the item#;
     * otherwise, we must go via the filenodes, because the icon may
     * not be currently visible
     */
    if (G.g_cwin == DESKWH)
    {
        pa = i_find(DESKWH, curr, &pf, &isapp);
    }
    else
    {
        pf = pn_selected(pwin); /* get first selected file */
        if (!pf)
            return FALSE;
        pa = pf->f_pa;
        isapp = pf->f_isap;
    }
    if (!pa)
        return FALSE;

    switch(pa->a_type)
    {
    case AT_ISFILE:
    case AT_ISFOLD:
        pw = win_find(G.g_cwin);
        if (!pw)
            return FALSE;

#if CONF_WITH_DESKTOP_SHORTCUTS
        if (pa->a_flags & AF_ISDESK)
        {
            char *p = filename_start(pa->a_pappl);
            /* check for root folder */
            if ((pa->a_type == AT_ISFOLD) && (p == pa->a_pappl))
            {
                strcpy(pathname, pa->a_pappl);
                filename[0] = '\0';
            }
            else
            {
                strlcpy(pathname, pa->a_pappl, p-pa->a_pappl);
                strcpy(filename, p);
            }
            strcat(pathname, "\\*.*");
        }
        else
#endif
        {
            strcpy(pathname, pw->w_pnode.p_spec);
            strcpy(filename, pf->f_name);
        }

        if (pa->a_type == AT_ISFILE)
            return do_aopen(pa, isapp, curr, pathname, filename, NULL);

        /* handle opening a folder */
        if (add_one_level(pathname, filename))
        {
            pw->w_cvrow = 0;        /* reset slider */
            do_fopen(pw, curr, pathname, TRUE);
        }
        else
            fun_alert(1, STDEEPPA);
        break;
    case AT_ISDISK:
        do_dopen(curr);
        break;
    case AT_ISTRSH:
        trash_alert(pa);
        break;
#if CONF_WITH_PRINTER_ICON
    case AT_ISPRNT:
        printer_alert(pa);
        break;
#endif
    }

    return FALSE;
}


/*
 *  Get information on an icon
 *
 *  returns:
 *      0   cancel
 *          otherwise continue
 */
WORD do_info(WORD curr)
{
    WORD ret = 1, drive;
    ANODE *pa;
    WNODE *pw;
    FNODE fn, *pf;
    char pathname[MAXPATHLEN];
    char *pathptr;

    MAYBE_UNUSED(fn);
    MAYBE_UNUSED(pathname);

    pa = i_find(G.g_cwin, curr, &pf, NULL);
    if (!pa)
        return ret;

    switch(pa->a_type)
    {
    case AT_ISFOLD:
    case AT_ISFILE:
        pw = win_find(G.g_cwin);
        if (pw)
        {
#if CONF_WITH_DESKTOP_SHORTCUTS
            if (pa->a_flags & AF_ISDESK)
            {
                DTA *dta;

                dta = file_exists(pa->a_pappl, NULL);
                if (!dta)
                {
                    remove_locate_shortcut(curr);
                    break;
                }

                pf = &fn;
                memcpy(&pf->f_attr, &dta->d_attrib, 23);
                strcpy(pathname, pa->a_pappl);
                strcpy(filename_start(pathname),"*.*");
                pathptr = pathname;
            }
            else
#endif
            {
                pathptr = pw->w_pnode.p_spec;
            }
            ret = inf_file_folder(pathptr, pf);
            if (ret < 0)
                fun_mark_for_rebld(pathptr);
        }
        break;
    case AT_ISDISK:
        drive = LOBYTE(get_iconblk_ptr(G.g_screen, curr)->ib_char);
        ret = inf_disk(drive);
        break;
    case AT_ISTRSH:
        trash_alert(pa);
        break;
#if CONF_WITH_PRINTER_ICON
    case AT_ISPRNT:
        printer_alert(pa);
        break;
#endif
    }

    return ret;
}


#if CONF_WITH_FORMAT
/*
 *  Write boot sector
 */
static WORD write_boot(char *buf, WORD disktype, WORD drive)
{
    Protobt((LONG)buf, RANDOM_SERIAL, disktype, 0);
    *buf = 0xe9;        /* DOS compatibility */

    return Rwabs(WRITESEC, (LONG)buf, 1, 0, drive, 0);
}

/*
 *  Initialise starting sectors of floppy disk (boot sector, FATs, root dir)
 */
static WORD init_start(char *buf, WORD disktype, WORD drive, char *label)
{
    BPB *bpb;
    char *p;

    /*
     * write boot so we can do a Getbpb()
     */
    if (write_boot(buf, disktype, drive) < 0)
        return -1;

    bpb = (BPB *)Getbpb(drive);

    /*
     * write FATs
     */
    bzero(buf, bpb->fsiz*SECTOR_SIZE);
    buf[0] = 0xf9;
    buf[1] = 0xff;
    buf[2] = 0xff;
    if (Rwabs(WRITESEC, (LONG)buf, bpb->fsiz, 1, drive, 0))
        return -1;
    if (Rwabs(WRITESEC, (LONG)buf, bpb->fsiz, 1+bpb->fsiz, drive, 0))
        return -1;

    /*
     * write root dir, including label if present
     */
    bzero(buf, bpb->rdlen*SECTOR_SIZE);
    if (label[0])
    {
        memset(buf, ' ', 11);
        for (p = buf; *label; )
            *p++ = *label++;
        buf[11] = FA_VOL;
    }
    if (Rwabs(WRITESEC, (LONG)buf, bpb->rdlen, 1+bpb->fsiz*2, drive, 0))
        return -1;

    /*
     * rewrite boot to force mediachange to be set
     */
    if (write_boot(buf, disktype, drive) < 0)
        return -1;

    return 0;
}

/*
 *  Issue alert & return TRUE iff user wants to retry
 */
static BOOL retry_format(void)
{
    graf_mouse(ARROW,NULL);
    if (fun_alert(3, STFMTERR) == 2)
        return FALSE;
    graf_mouse(HGLASS,NULL);    /* say we're busy again */

    return TRUE;
}

/*
 *  Do the real formatting work
 */
static WORD format_floppy(OBJECT *tree, WORD max_width, WORD incr)
{
    char *buf, label[LEN_ZFNAME];
    const WORD *skewtab;
    WORD drive, numsides, disktype, spt, trackskew;
    WORD track, side, skewindex;
    WORD width, rc;

    drive = inf_gindex(tree, FMT_DRVA, 2);
    numsides = 2;       /* default to double sided */
    disktype = 3;       /* for Protobt() */
    spt = 9;
    skewtab = std_skewtab;
    trackskew = 2;

    switch(inf_gindex(tree, FMT_SS, 3))
    {
    case 0:             /* single sided */
        numsides = 1;
        disktype = 2;
        trackskew = 3;  /* skew between tracks */
        break;
    case 2:             /* high density */
        disktype = 4;
        spt = 18;
        skewtab = hd_skewtab;
        trackskew = 3;
    }

    buf = dos_alloc_stram(FMTBUFLEN);
    if (!buf)
    {
        malloc_fail_alert();
        return -1;
    }

    tree[FMT_BAR].ob_width = 0;
    tree[FMT_BAR].ob_spec = 0x00FF1121L;

    graf_mouse(HGLASS,NULL);    /* say we're busy */

    for (track = 0, rc = 0, skewindex = 0; (track < MAXTRACK) && !rc; track++)
    {
        for (side = 0; (side < numsides) && !rc; side++)
        {
            skewindex -= trackskew;
            if (skewindex < 0)
                skewindex += spt;

            while((rc=Flopfmt((LONG)buf, (LONG)&skewtab[skewindex],
                        drive, spt, track, side, -1, FLOPFMT_MAGIC, VIRGIN)))
            {
                if (!retry_format())
                    break;              /* rc will still be set */
            }
        }
        /* update progress bar */
        width = tree[FMT_BAR].ob_width + incr;
        if (width > max_width)
            width = max_width;
        tree[FMT_BAR].ob_width = width;
        draw_fld(tree,FMT_BAR);
    }

    inf_sget(tree, FMTLABEL, label);

    if (rc == 0)
        while((rc=init_start(buf, disktype, drive, label)))
        {
            if (!retry_format())
                break;                  /* rc will still be set */
        }

    graf_mouse(ARROW,NULL);     /* no longer busy */
    dos_free(buf);

    return rc;
}

/*
 *  Format a floppy disk
 */
void do_format(void)
{
    OBJECT *tree, *obj;
    LONG total, avail;
    WORD i, drivebits, drive;
    WORD exitobj, rc;
    WORD max_width, incr;

    tree = desk_rs_trees[ADFORMAT];

    /*
     * enable button(s) for existent drives, disable for non-existent
     */
    drivebits = dos_sdrv(dos_gdrv()) & 0x0003;  /* floppy devices */
    for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++, drivebits >>= 1)
    {
        if (drivebits & 0x0001)
        {
            obj->ob_state &= ~DISABLED;
        }
        else
        {
            obj->ob_state &= ~SELECTED;
            obj->ob_state |= DISABLED;
        }
    }

    /*
     * if a drive is currently selected, don't change it
     */
    drive = -1;
    for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++)
    {
        if (obj->ob_state & SELECTED)
        {
            drive = i;
            break;
        }
    }

    /*
     * if NO drive was previously selected, select the first enabled one
     */
    if (drive < 0)
    {
        for (i = 0, obj = &tree[FMT_DRVA]; i < 2; i++, obj++)
        {
            if (!(obj->ob_state & DISABLED))
            {
                drive = i;
                break;
            }
        }
        if (drive >= 0)
            obj->ob_state |= SELECTED;
    }

    /*
     * if there are no enabled drives, disallow OK
     */
    if (drive < 0)
        tree[FMT_OK].ob_state |= DISABLED;

    tree[FMT_CNCL].ob_state &= ~SELECTED;

    /*
     * adjust the initial default formatting option, hiding
     * the high density option if not available
     */
    if (Supexec((LONG)get_floppy_type) == 0)
    {
        if (tree[FMT_HD].ob_state & SELECTED)   /* first time */
        {
            tree[FMT_HD].ob_state &= ~SELECTED;
            tree[FMT_HD].ob_flags |= HIDETREE;
            tree[FMT_DS].ob_state |= SELECTED;
        }
    }

    /*
     * fix up the progress bar width, increment & fill pattern
     */
    incr = tree[FMT_BAR].ob_width / MAXTRACK;
    max_width = incr * MAXTRACK;
    tree[FMT_BAR].ob_width = max_width;
    tree[FMT_BAR].ob_spec = 0x00FF1101L;

    /*
     * do the actual work
     */
    do {
        inf_sset(tree, FMTLABEL, "");
        start_dialog(tree);
        exitobj = form_do(tree, FMTLABEL) & 0x7fff;
        if (exitobj == FMT_OK)
            rc = format_floppy(tree, max_width, incr);
        else
            rc = -1;
        end_dialog(tree);

        if (rc == 0)
        {
            drive = (tree[FMT_DRVA].ob_state & SELECTED) ? 0 : 1;
            refresh_drive('A'+drive);           /* update relevant windows */
            dos_space(drive + 1, &total, &avail);
            if (fun_alert_merge(2, STFMTINF, avail) == 2)
                rc = -1;
        }
        tree[FMT_BAR].ob_width = max_width;     /* reset to starting values */
        tree[FMT_BAR].ob_spec = 0x00FF1101L;
        tree[FMT_OK].ob_state &= ~SELECTED;
    } while (rc == 0);
}
#endif


/*
 *  Routine to re-read and redisplay the directory associated with
 *  the specified window
 */
void refresh_window(WNODE *pw, BOOL force_mediach)
{
    char drive;

    if (!pw->w_id)      /* desktop */
        return;

    /*
     * For floppy drives, allow a forced media change by a call
     * to Rwabs() with buffer == NULL.
     */
    if (force_mediach)
    {
        drive = pw->w_pnode.p_spec[0];
        if (drive == 'A' || drive == 'B')
        {
            KDEBUG(("Forcing media change for drive %c\n", drive));
            Rwabs(READSEC, (LONG)NULL, MEDIACHANGE, 0, drive - 'A', 0);
        }
    }

    /* make sure we don't open a new window */
    do_fopen(pw, 0, pw->w_pnode.p_spec, FALSE);
}


/*
 * Function called (after a format or delete) to redisplay any windows
 * associated with the specified drive letter
 */
void refresh_drive(WORD drive)
{
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (pw->w_id)
        {
            if (pw->w_pnode.p_spec[0] == drive)
            {
                fun_close(pw, CLOSE_TO_ROOT);   /* what Atari TOS does */
                refresh_window(pw, FALSE);
            }
        }
    }
}


/*
 *  Given an icon index, go find the ANODE which it represents
 *
 *  NOTE: normally, the anode for the default viewer is not included
 *  in the search.  The caller of this function may request its inclusion
 *  by negating the value of the window handle that is passed.
 *
 *  . returns ptr to corresponding FNODE via arg3
 *  . if checking a window (arg1 != 0), then return an indicator via arg4:
 *      TRUE if the matching ANODE indicates the item is an application,
 *      FALSE if it indicates the item is a data file for an application.
 *  . arg3 and/or arg4 may be NULL to bypass returning the corresponding value
 *
 *  returns NULL if no matching index
 */
ANODE *i_find(WORD wh, WORD item, FNODE **ppf, BOOL *pisapp)
{
    ANODE *pa;
    WNODE *pw;
    FNODE *pf;
    BOOL isapp;

    pa = (ANODE *) NULL;
    pf = (FNODE *) NULL;
    isapp = FALSE;

    if (wh == DESKWH)       /* On desktop? */
    {
        pa = app_afind_by_id(item);
        if (pa)
            if (pa->a_type == AT_ISFILE)
                isapp = (pa->a_aicon < 0) ? FALSE : TRUE;
    }
    else
    {
        WORD ignore = AF_ISDESK|AF_WINDOW|AF_VIEWER;
        if (wh < 0)
        {
            wh = -wh;
            ignore &= ~AF_VIEWER;   /* include default viewer anode */
        }
        pw = win_find(wh);
        if (pw)
        {
            if (item >= 0)
                pf = G.g_screeninfo[item].fnptr;
            if (pf)
            {
                pa = app_afind_by_name((pf->f_attr&FA_SUBDIR)?AT_ISFOLD:AT_ISFILE,
                                        ignore, pw->w_pnode.p_spec, pf->f_name, &isapp);
            }
        }
    }

    if (ppf)
        *ppf = pf;

    if (pisapp)
        *pisapp = isapp;

    return pa;
}


/*
 *  Routine to change the default drive and directory
 */
WORD set_default_path(char *path)
{
    dos_sdrv(path[0]-'A');

    return (WORD)dos_chdir(path);
}


/*
 *  Check if specified drive letter is valid
 *
 *  if it is, return TRUE
 *  else issue form_alert and return FALSE
 */
BOOL valid_drive(char drive)
{
    int drv = drive - 'A';
    char drvstr[2];

    drvstr[0] = drive;
    drvstr[1] = '\0';

    if ((drv >= 0) && (drv < BLKDEVNUM))
        if (dos_sdrv(dos_gdrv()) & (1L << drv))
            return TRUE;

    fun_alert_merge(1, STNODRIV, drvstr);

    return FALSE;
}


/*
 *  Issue 'out of memory' alert
 */
void malloc_fail_alert(void)
{
    fun_alert(1, STMAFAIL);
}


/*
 *  Change mouse to 'busy' indicator
 */
void desk_busy_on(void)
{
    graf_mouse(HGLASS, NULL);
}


/*
 *  Change mouse to 'not busy' indicator
 */
void desk_busy_off(void)
{
    graf_mouse(ARROW, NULL);
}
