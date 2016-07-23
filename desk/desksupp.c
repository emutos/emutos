/*      DESKSUPP.C      05/04/84 - 06/20/85     Lee Lorenzen            */
/*      for 3.0 (xm)    3/12/86  - 01/17/87     MDF                     */
/*      for 3.0                 11/13/87                mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
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

#include "config.h"
#include "portab.h"
#include "string.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "dos.h"
#include "gembind.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskbind.h"

#include "../aes/rectfunc.h"
#include "../aes/gemdos.h"
#include "aesbind.h"
#include "deskact.h"
#include "deskpro.h"
#include "deskinf.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskmain.h"
#include "deskglob.h"
#include "desk1.h"
#include "deskdir.h"
#include "desksupp.h"
#include "nls.h"
#include "scancode.h"
#include "kprint.h"


#if CONF_WITH_SHOW_FILE
/*
 *      declarations used by the show_file() code
 */
#define IOBUFSIZE   16384L
LONG get_deskwh(void);
static LONG linecount;
static WORD pagesize;
#endif


/*
 *  Clear out the selections for this particular window
 */
void desk_clear(WORD wh)
{
    WNODE *pw;
    GRECT c;
    WORD root = -1;

    /* get current size */
    wind_get(wh, WF_WXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h);

    if (wh)         /* not the desktop */
    {
        pw = win_find(wh);  /* find its tree of items */
        if (pw)
            root = pw->w_root;
    }
    else root = DROOT;

    /* clear all selections */
    act_allchg(wh, G.g_screen, root, 0, &gl_rfull, &c, SELECTED, FALSE, TRUE);
}


/*
 *  Verify window display by building a new view
 */
void desk_verify(WORD wh, WORD changed)
{
    WNODE *pw;
    WORD xc, yc, wc, hc;

    if (wh)
    {
        /* get current size */
        pw = win_find(wh);
        if (pw)
        {
            if (changed)
            {
                wind_get(wh, WF_WXYWH, &xc, &yc, &wc, &hc);
                win_bldview(pw, xc, yc, wc, hc);
            }
            G.g_croot = pw->w_root;
        }
    }
    else G.g_croot = 1;         /* DESK v1.2: The Desktop */

    G.g_cwin = wh;
    G.g_wlastsel = wh;
}



void do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc)
{
    GRECT clip_r, t;
    WNODE *pw;
    WORD root;

    clip_r.g_x = xc;
    clip_r.g_y = yc;
    clip_r.g_w = wc;
    clip_r.g_h = hc;
    root = 1;
    if (w_handle != 0)
    {
        pw = win_find(w_handle);
        if (pw)
            root = pw->w_root;
    }

    graf_mouse(M_OFF, NULL);

    wind_get(w_handle, WF_FIRSTXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
    while(t.g_w && t.g_h)
    {
        if (rc_intersect(&clip_r, &t))
            objc_draw(G.g_screen, root, MAX_DEPTH, t.g_x, t.g_y, t.g_w, t.g_h);
        wind_get(w_handle, WF_NEXTXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
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
    if (*py < G.g_ydesk)        /* ensure it's below menu bar */
        *py = G.g_ydesk;
}


void do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h)
{
    GRECT c;
    GRECT d;

    do_xyfix(&x, &y);
    get_xywh(G.g_screen, G.g_croot, &c.g_x, &c.g_y, &c.g_w, &c.g_h);

    /* Zooming box effect */
    get_xywh(G.g_screen, curr, &d.g_x, &d.g_y, &d.g_w, &d.g_h);
    d.g_x += c.g_x;
    d.g_y += c.g_y;
    graf_growbox(d.g_x, d.g_y, d.g_w, d.g_h, x, y, w, h);

    act_chg(G.g_cwin, G.g_screen, G.g_croot, curr, &c, SELECTED,
            FALSE, TRUE, TRUE);
    if (new_win)
        wind_open(wh, x, y, w, h);

    G.g_wlastsel = wh;
}


/*
 * returns TRUE iff window has grown
 */
WORD do_wfull(WORD wh)
{
    GRECT curr, prev, full;

    wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
    wind_get(wh, WF_PXYWH, &prev.g_x, &prev.g_y, &prev.g_w, &prev.g_h);
    wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);

    if (rc_equal(&curr, &full)) /* currently full, so shrink */
    {
        wind_set(wh, WF_CXYWH, prev.g_x, prev.g_y, prev.g_w, prev.g_h);
        graf_shrinkbox(prev.g_x, prev.g_y, prev.g_w, prev.g_h,
                        full.g_x, full.g_y, full.g_w, full.g_h);
        return 0;
    }

    graf_growbox(curr.g_x, curr.g_y, curr.g_w, curr.g_h,
                full.g_x, full.g_y, full.g_w, full.g_h);
    wind_set(wh, WF_CXYWH, full.g_x, full.g_y, full.g_w, full.g_h);

    return 1;
}


/*
 *  Open a directory, it may be the root or a subdirectory
 */
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv,
                BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt, WORD redraw)
{
    WORD ret;
    PNODE *tmp;

    /* convert to hourglass */
    graf_mouse(HGLASS, NULL);

    /* open a path node */
    tmp = pn_open(drv, ppath, pname, pext, F_SUBDIR);
    if (tmp == NULL)
    {
        graf_mouse(ARROW, NULL);
        return FALSE;
    }
    else
        pw->w_path = tmp;

    /* activate path by search and sort of directory */
    ret = pn_active(pw->w_path);
    if (ret != E_NOFILES)
    {
        /* some error condition */
    }

    /* set new name and info lines for window */
    win_sname(pw);
    win_sinfo(pw);
    wind_set(pw->w_id, WF_NAME, pw->w_name, 0, 0);
    wind_set(pw->w_id, WF_INFO, pw->w_info, 0, 0);

    /* do actual wind_open  */
    if (curr_icon)
    {
        do_wopen(new_win, pw->w_id, curr_icon,
                    pt->g_x, pt->g_y, pt->g_w, pt->g_h);
        if (new_win)
            win_top(pw);
    }

    /*
     * verify contents of window's object list
     * by building view and make it curr.
     */
    desk_verify(pw->w_id, TRUE);

    /* make it redraw */
    if (redraw && !new_win)
        fun_msg(WM_REDRAW, pw->w_id, pt->g_x, pt->g_y, pt->g_w, pt->g_h);

    graf_mouse(ARROW, NULL);

    return TRUE;
}


#if CONF_WITH_SHOW_FILE
/*
 *  helper functions for displaying a file
 */

/*
 *  get key from keyboard
 *
 *  if ASCII (1-255), returns value in low-order byte, 0 in high-order byte
 *  else returns scancode
 */
static WORD get_key(void)
{
    ULONG c;

    c = dos_rawcin();

    if (c & 0xff)           /* ASCII ? */
        c &= 0xff;          /* yes, just return the ASCII value */
    else
        c >>= 8;            /* convert to scancode */

    return (WORD)c;
}

/*
 *  check for flow control or quit (ctl-C/Q/q)
 *
 *  a +ve argument is the character to check
 *  a -ve argument means get the character from the console
 */
static WORD user_input(WORD c)
{
    if (c < 0)
        c = get_key();

    if ((c == CTL_C) || (c == 'Q') || (c == 'q') || (c == UNDO))    /* wants to quit */
        return -1;

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
 *  blank out line via VT52 escape sequence
 */
static void blank_line(void)
{
    dos_conout('\x1b');
    dos_conout('l');
}

/*
 *  clear screen via VT52 escape sequence
 */
static void clear_screen(void)
{
    dos_conout('\x1b');
    dos_conout('E');
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
    BYTE c, cprev = 0;
    BYTE *msg;

    n = len;
    while(n-- > 0)
    {
        if (dos_conis())
            if (user_input(-1))
                return 1;

        c = *s++;
        /* convert Un*x-style text to TOS-style */
        if ((c == '\n') && (cprev != '\r'))
            dos_conout('\r');
        dos_conout(c);
        if (c == '\n')
        {
            if (++linecount >= pagesize)
            {
                rsrc_gaddr(R_STRING,STMORE,(LONG *)&msg);
                dos_conws(msg);             /* "-More-" */
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
                    if (user_input(response))
                    {
                        dos_conout('\r');
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
    BYTE *msg;

    rc = dos_open(name,0);
    if (rc < 0L)
        return;

    handle = (WORD)rc;

    scr_width = G.g_wdesk;
    scr_height = G.g_ydesk + G.g_hdesk;

    /*
     * set up for text output
     */
    graf_mouse(M_OFF, NULL);
    menu_bar(G.a_trees[ADMENU],0);
    wind_update(BEG_UPDATE);
    form_dial(FMD_START, 0,0,0,0, 0,0,scr_width,scr_height);
    clear_screen();

    pagesize = get_deskwh() & 0xffff;
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
        rsrc_gaddr(R_STRING,(rc==0L)?STEOF:STFRE,(LONG *)&msg);
        blank_line();
        dos_conws(msg); /* "-End of file-" or "-File read error-" */
        dos_rawcin();
    }

    /*
     * switch back to normal desktop
     */
    clear_screen();     /* neatness */
    form_dial(FMD_FINISH, 0,0,0,0, 0,0,scr_width,scr_height);
    wind_update(END_UPDATE);
    menu_bar(G.a_trees[ADMENU],1);
    graf_mouse(M_ON, NULL);
}
#endif


/*
 *  Open an application
 */
static WORD do_aopen(ANODE *pa, WORD isapp, WORD curr, WORD drv,
                     BYTE *ppath, BYTE *pname)
{
    WORD ret, done;
    WORD isgraf, isparm, uninstalled;
    BYTE *pcmd, *ptail;

    done = FALSE;

    /* set flags */
    isgraf = pa->a_flags & AF_ISGRAF;
    isparm = pa->a_flags & AF_ISPARM;
    uninstalled = ( (*pa->a_pappl == '*') ||
                    (*pa->a_pappl == '?') ||
                    (*pa->a_pappl == '\0') );

    /* change current dir. to selected icon's */
    pro_chdir(drv, ppath);

    /*
     * see if application was selected directly or a
     * data file with an associated primary application
     */
    pcmd = ptail = NULL;
    G.g_cmd[0] = G.g_tail[1] = '\0';
    ret = TRUE;

    if (!uninstalled && !isapp)
    {
        /* an installed document */
        pcmd = pa->a_pappl;
        ptail = pname;
    }
    else
    {
        if (isapp)
        {       /* DOS-based app. has been selected */
            if (isparm)
            {
                pcmd = &G.g_cmd[0];
                ptail = &G.g_tail[1];
                ret = opn_appl(pname, "\0", pcmd, ptail);
            }
            else
                pcmd = pname;
        }
        else
        {       /* user double-clicked document */
#if CONF_WITH_SHOW_FILE
            ret = fun_alert(1, STSHOW, NULL);
            if (ret == 1)
            {
                char *iobuf = dos_alloc(IOBUFSIZE);
                if (iobuf)
                {
                    show_file(pname, IOBUFSIZE, iobuf);
                    dos_free((LONG)iobuf);
                }
            }
#else
            fun_alert(1, STNOAPPL, NULL);
#endif
            ret = FALSE;
        }
    }

    /* user wants to run another application */
    if (ret)
    {
        if ((pcmd != &G.g_cmd[0]) && (pcmd != NULL))
            strcpy(&G.g_cmd[0], pcmd);
        if ((ptail != &G.g_tail[1]) && (ptail != NULL))
            strcpy(&G.g_tail[1], ptail);
        done = pro_run(isgraf, 1, G.g_cwin, curr);
    }

    return done;
}


/*
 *  Open a disk
 */
static WORD do_dopen(WORD curr)
{
    WORD drv;
    WNODE *pw;
    ICONBLK *pib;

    pib = (ICONBLK *) get_iconblk_ptr(G.g_screen, curr);
    pw = win_alloc(curr);
    if (pw)
    {
        drv = (0x00ff & pib->ib_char);
        if (pro_chdir(drv, "") == 0)
            do_diropen(pw, TRUE, curr, drv, "", "*", "*",
                        (GRECT *)&G.g_screen[pw->w_root].ob_x, TRUE);
        else
            win_free(pw);
    }
    else
    {
        rsrc_gaddr(R_STRING, STNOWIND, &G.a_alert);
        form_alert(1, G.a_alert);
    }

    return FALSE;
}


/*
 *  Open a folder
 */
void do_fopen(WNODE *pw, WORD curr, WORD drv, BYTE *ppath, BYTE *pname,
              BYTE *pext, WORD redraw)
{
    GRECT t;
    WORD ok;
    BYTE *pnew;
    BYTE *pp;

    ok = TRUE;
    pnew = ppath;
    wind_get(pw->w_id, WF_WXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);

    if (pro_chdir(drv, "") < 0L)    /* drive (no longer) valid? */
    {
        true_closewnd(pw);
        return;
    }
    if (pro_chdir(drv, ppath) == 0) /* if directory exists, */
    {                               /* ensure all contents are displayed */
        pname = "*";
        pext  = "*";
    }

    pn_close(pw->w_path);

    ok = do_diropen(pw, FALSE, curr, drv, pnew, pname, pext, &t, redraw);
    if (!ok)
    {
        fun_alert(1, STDEEPPA, NULL);
        /* back up one level    */
        pp = last_separator(pnew);
        *pp = '\0';
        pro_chdir(drv,pnew);                  /* fixup current dir */
        do_diropen(pw, FALSE, curr, drv, pnew, pname, pext, &t, redraw);
    }
}


/*
 *  Open an icon
 */
WORD do_open(WORD curr)
{
    WORD done;
    ANODE *pa;
    WNODE *pw;
    FNODE *pf;
    WORD drv, isapp;
    BYTE path[LEN_ZPATH+1], name[LEN_ZNODE+1], ext[LEN_ZEXT+1];

    done = FALSE;

    pa = i_find(G.g_cwin, curr, &pf, &isapp);
    pw = win_find(G.g_cwin);
    if (!pw)
        return FALSE;
    if (pf)
        fpd_parse(&pw->w_path->p_spec[0],&drv,&path[0],&name[0],&ext[0]);

    if (pa)
    {
        switch(pa->a_type)
        {
        case AT_ISFILE:
            done = do_aopen(pa,isapp,curr,drv,&path[0],&pf->f_name[0]);
            break;
        case AT_ISFOLD:
            if (path[0] != '\0')
                strcat(&path[0], "\\");
            if ((strlen(path) + strlen(pf->f_name)) >= (LEN_ZPATH-3))
                fun_alert(1, STDEEPPA, NULL);
            else
            {
                strcat(&path[0], &pf->f_name[0]);
                pw->w_cvrow = 0;        /* reset slider */
                do_fopen(pw, curr, drv, &path[0], &name[0], &ext[0], TRUE);
            }
            break;
        case AT_ISDISK:
            do_dopen(curr);
            break;
        case AT_ISTRSH:
            form_alert(1, (LONG)ini_str(STNOOPEN));
            break;
        }
    }

    return done;
}


/*
 *  Get information on an icon
 */
WORD do_info(WORD curr)
{
    WORD ret, drive;
    ANODE *pa;
    WNODE *pw;
    FNODE *pf;

    pa = i_find(G.g_cwin, curr, &pf, NULL);
    pw = win_find(G.g_cwin);
    if (!pw)
        return FALSE;

    if (pa)
    {
        switch(pa->a_type)
        {
        case AT_ISFOLD:
            /* drop thru */
        case AT_ISFILE:
            ret = inf_file_folder(&pw->w_path->p_spec[0], pf);
            if (ret)
                fun_rebld(pw);
            break;
        case AT_ISDISK:
            drive = (get_iconblk_ptr(G.g_screen, curr)->ib_char) & 0xFF;
            inf_disk(drive);
            break;
        case AT_ISTRSH:
            fun_alert(1, STTRINFO, NULL);
            break;
        }
    }

    return FALSE;
}


/*
 *  Format the currently selected disk
 */
int do_format(WORD curr)
{
    WORD ret;
    WORD foundit;
    int done;
    BYTE *p;
    ANODE *pa;
    BYTE drive_letter;
    BOOL isgraf; /* The format program is a graphical one */

    done = 0;

    /*
     * check icon type (should be OK, since deskmain should have
     * disabled the menu item unless the selected icon is a floppy)
     */
    pa = i_find(G.g_cwin, curr, NULL, NULL);

    if (pa && (pa->a_type == AT_ISDISK) && (pa->a_aicon == IG_FLOPPY))
    {
        /*
         * it's a floppy disk: check if a formatter is available
         * we prefer a graphical one
         */
        strcpy(G.g_cmd, "FORMAT.PRG");
        isgraf = TRUE;
        foundit = shel_find(G.g_cmd);
        if (!foundit)
        {
            strcpy(G.g_cmd, "FORMAT.TTP");
            isgraf = FALSE;
            foundit = shel_find(G.g_cmd);
        }
        if (!foundit)
        {
            fun_alert(1, STNOFRMT, NULL);
            return 0;
        }

        /*
         * we have a formatter, ask user if we should run it
         */
        drive_letter = (get_iconblk_ptr(G.g_screen, curr)->ib_char) & 0xFF;
        ret = fun_alert(2, STFORMAT, &drive_letter);
        if (ret != 1)
            return 0;

        /*
         * user says OK
         */
        p = G.g_tail + 1;       /* set up command tail */
        *p++ = drive_letter;
        *p++ = ':';
        *p = '\0';

        pro_run(isgraf, 1, G.g_cwin, curr);
        done = 1;
    }

    return done;
}


/*
 *  Routine to check the all windows directory by doing a change
 *  disk/directory to it and redrawing the window
 */
void do_chkall(WORD redraw)
{
    WORD drv;
    BYTE path[LEN_ZPATH+1], name[LEN_ZNODE+1], ext[LEN_ZEXT+1];
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (pw->w_id)
        {
            fpd_parse(&pw->w_path->p_spec[0], &drv, &path[0],
                      &name[0], &ext[0]);
            do_fopen(pw, 0, drv, &path[0], &name[0], &ext[0], redraw);
        }
        else
        {
            desk_verify(0, TRUE);
        }
    }
}

/*
 *  Routine to re-read and redisplay the directory associated with
 *  the specified window
 */
void do_refresh(WNODE *pw)
{
    WORD drv;
    BYTE path[LEN_ZPATH+1], name[LEN_ZNODE+1], ext[LEN_ZEXT+1];

    if (!pw->w_id)      /* desktop */
        return;

    fpd_parse(pw->w_path->p_spec, &drv, path, name, ext);
    do_fopen(pw, 0, drv, path, name, ext, TRUE);
}
