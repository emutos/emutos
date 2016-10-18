/*      DESKINS.C       09/22/84 - 06/19/85     Lee Lorenzen            */
/*      for 3.0         02/28/86                        MDF             */
/*      merge source    5/27/87  - 5/28/87              mdf             */

/*
 *      This module contains routines for three functions:
 *          . install devices
 *          . install application
 *          . remove desktop icons
 *
 *      Copyright 2002-2016 The EmuTOS development team
 *
 *      This software is licenced under the GNU Public License.
 *      Please see LICENSE.TXT for further information.
 *
 *                  Historical Copyright
 *      -------------------------------------------------------------
 *      GEM Desktop                                       Version 2.3
 *      Serial No.  XXXX-0000-654321              All Rights Reserved
 *      Copyright (C) 1985 - 1987               Digital Research Inc.
 *      -------------------------------------------------------------
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include <string.h>

#include "portab.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "gemdos.h"
#include "dos.h"
#include "optimize.h"

#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "deskbind.h"

#include "aesbind.h"
#include "deskglob.h"
#include "deskinf.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskdir.h"
#include "deskmain.h"
#include "icons.h"
#include "desk1.h"
#include "intmath.h"
#include "deskins.h"
#include "desksupp.h"
#include "kprint.h"


/*
 * used by insert_icon()
 */
static ICONBLK ib;


/*
 *  Routine to tell if an icon has an associated document type
 */
WORD is_installed(ANODE *pa)
{
    return ( !((*pa->a_pappl == '*') || (*pa->a_pappl == '?') ||
                   (*pa->a_pappl == '\0')  ) );
}


/*
 *  Check if icon grid position (x,y) is free
 */
static WORD grid_free(WORD x,WORD y)
{
    ANODE *pa;

    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if (pa->a_flags & AF_ISDESK)    /* icon on desktop? */
        {
            if ((x == pa->a_xspot/G.g_icw) && (y == pa->a_yspot/G.g_ich))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}


/*
 *  Routine to place disk icon
 *
 *  we start at the position of the sample icon, and scan rightwards for
 *  a free grid position, wrapping to subsequent lines as necessary.
 *  if no free position is available, we just return the input position.
 *
 *  Input:  position of sample icon
 *  Output: position of new icon
 */
static void ins_posdisk(WORD dx, WORD dy, WORD *pdx, WORD *pdy)
{
    WORD  xcnt, ycnt, xin, yin, x, y;

    xcnt = G.g_wdesk / G.g_icw;     /* number of grid positions */
    ycnt = G.g_hdesk / G.g_ich;

    xin = dx / G.g_icw;             /* input grid position */
    yin = dy / G.g_ich;

    for (x = xin, y = yin; y < ycnt; y++, x = 0)
    {
        for ( ; x < xcnt; x++)
        {
            if (grid_free(x,y))
            {
                *pdx = dx + (x - xin) * G.g_icw;
                *pdy = dy + (y - yin) * G.g_ich;
                return;
            }
        }
    }

    *pdx = dx;
    *pdy = dy;
}


/*
 *  Install icon for one drive
 *
 *  Returns -1 for error (couldn't allocate ANODE)
 */
static WORD install_drive(WORD drive)
{
    ANODE *pa;
    WORD x, y;

    /* find first available spot on desktop (before we alloc a new one) */
    ins_posdisk(0, 0, &x, &y);

    pa = app_alloc(FALSE);
    if (!pa)
        return -1;

    pa->a_flags = AF_ISCRYS | AF_ISDESK;
    pa->a_funkey = 0;
    pa->a_letter = 'A' + drive;
    pa->a_type = AT_ISDISK;
    pa->a_obid = 0;     /* fixed up when deskmain() calls app_blddesk() */
    sprintf(G.g_1text,"%s %c",ini_str(STDISK),pa->a_letter);
    scan_str(G.g_1text, &pa->a_pappl);  /* set up disk name */
    pa->a_pdata = "";                   /* point to empty string */
    pa->a_aicon = (drive > 1) ? IG_HARD : IG_FLOPPY;
    pa->a_dicon = NIL;
    snap_disk(x,y,&pa->a_xspot,&pa->a_yspot, 0, 0);

    return 0;
}


/*
 *  Install devices: installs an icon on the desktop for all
 *  devices that do not currently have an icon
 *
 *  Returns count of drives successfully installed
 */
WORD ins_devices(void)
{
    ULONG drivebits, mask;
    WORD drive, count;
    ANODE *pa;

    drivebits = dos_sdrv(dos_gdrv());   /* all current devices */

    /*
     * scan ANODEs and zero out the bits for installed drives
     */
    for (pa = G.g_ahead; pa; pa = pa->a_next)
        if (pa->a_type == AT_ISDISK)
            drivebits &= ~(1L<<(pa->a_letter-'A'));

    for (drive = 0, mask = 1, count = 0; drive < BLKDEVNUM; drive++, mask <<= 1)
    {
        if (drivebits&mask)
        {
            if (install_drive(drive) < 0)
                break;
            count++;
        }
    }

    return count;
}


/*
 * clear autorun flag on all ANODEs
 */
static void clear_all_autorun(void)
{
    ANODE *pa;

    for (pa = G.g_ahead; pa; pa = pa->a_next)
        pa->a_flags &= ~AF_AUTORUN;
}


/*
 * return pointer to start of last segment of path
 * (assumed to be the filename)
 */
BYTE *filename_start(BYTE *path)
{
    BYTE *start = path;

    while (*path)
        if (*path++ == '\\')
            start = path;

    return start;
}


/*
 * convert ascii to WORD
 *
 * stops at nul byte or first non-decimal character
 */
static WORD atow(BYTE *s)
{
    WORD n = 0;

    for ( ; *s; s++)
    {
        if ((*s >= '0') && (*s <= '9'))
            n = (n * 10) + *s - '0';
        else break;
    }

    return n;
}


/*
 * get function key
 *
 * checks for invalid or duplicate function key and
 * issues appropriate alert
 *
 * returns function key value (0 for none, -ve for error)
 */
static WORD get_funkey(OBJECT *tree,ANODE *pa,BOOL installed)
{
    ANODE *an;
    WORD funkey;
    BYTE fkey[3];

    inf_sget(tree,APFUNKEY,fkey);
    if (fkey[0] == '\0')                /* empty? */
        return 0;

    funkey = atow(fkey);
    if (installed && (funkey == pa->a_funkey))  /* unchanged? */
        return funkey;

    /*
     * this is a new ANODE, or the key has changed: validate it
     */
    if ((funkey < 1) || (funkey > 20))
    {
        fun_alert(1,STINVKEY);          /* invalid function key */
        return -1;
    }

    /*
     * we have a valid key, check against other ANODEs
     */
    for (an = G.g_ahead; an; an = an->a_next)
    {
        if (an == pa)
            continue;
        if (an->a_funkey == funkey)
        {
            if (fun_alert(1,STDUPKEY) != 1)     /* duplicate fun key */
                return -1;
            an->a_funkey = 0;
            break;
        }
    }

    return funkey;
}


/*
 * install application
 */
WORD ins_app(WORD curr)
{
    ANODE *pa;
    FNODE *pf;
    WNODE *pw;
    OBJECT *tree;
    WORD change = 0;    /* -ve means cancel, 0 means no change, +ve means change */
    WORD isapp, field, exitobj, funkey;
    BOOL installed;
    BYTE *pfname, *p, *q;
    BYTE name[LEN_ZFNAME];
    BYTE pathname[MAXPATHLEN];

    pa = i_find(G.g_cwin, curr, &pf, &isapp);
    if (!pa)
        return 0;

    installed = is_installed(pa);

    /*
     * first, get full path & name of application
     */
    if (installed)
    {
        p = pa->a_pappl;
        q = filename_start(p);
        pfname = q;
    }
    else
    {
        if (!isapp)     /* selected item appears to be a data file */
            return 0;
        pw = win_find(G.g_cwin);
        p = pw->w_path->p_spec;
        q = filename_start(p);
        pfname = pf->f_name;
    }
    strlcpy(pathname,p,q-p+1);  /* copy pathname including trailing backslash */

    /*
     * deselect all objects
     */
    tree = G.a_trees[ADINSAPP];
    deselect_all(tree);

    /*
     * fill in dialog
     */
    fmt_str(pfname, name);
    inf_sset(tree, APNAME, name);
    inf_sset(tree, APARGS, installed ? pa->a_pargs : "");
    inf_sset(tree, APDOCTYP, installed ? pa->a_pdata+2 : "");
    if (pa->a_funkey)
        sprintf(name, "%02d", pa->a_funkey);
    else name[0] = '\0';
    inf_sset(tree, APFUNKEY, installed ? name : "");

    field = pa->a_flags & AF_AUTORUN ? APAUTO : APNORM;
    tree[field].ob_state |= SELECTED;

    switch(pa->a_flags & (AF_ISCRYS|AF_ISPARM))
    {
    case AF_ISCRYS|AF_ISPARM:
        field = APGTP;
        break;
    case AF_ISCRYS:
        field = APGEM;
        break;
    case AF_ISPARM:
        field = APTTP;
        break;
    default:
        field = APTOS;
    }
    tree[field].ob_state |= SELECTED;

    field = (pa->a_flags&AF_APPDIR) ? APDEFAPP : APDEFWIN;
    tree[field].ob_state |= SELECTED;

    field = (pa->a_flags&AF_ISFULL) ? APPMFULL : APPMFILE;
    tree[field].ob_state |= SELECTED;

    show_hide(FMD_START, tree);
    do
    {
        exitobj = form_do(tree, APARGS);

        switch(exitobj&0x7fff)
        {
        case APINSTAL:      /* (re)install an application */
            if (!installed)
                pa = app_alloc(TRUE);
            if (!pa)
            {
                change = -1;        /* don't try any more */
                break;
            }

            funkey = get_funkey(tree,pa,installed);
            if (funkey < 0)
            {
                inf_sset(tree, APFUNKEY, "");
                tree[APINSTAL].ob_state &= ~SELECTED;
                if (!installed)
                    app_free(pa);
                draw_dial(tree);
                exitobj = -1;       /* request retry */
                break;
            }
            pa->a_funkey = funkey;  /* now we can store it */

            pa->a_flags = 0;
            field = inf_gindex(tree, APTOS, APGTP-APTOS+1);
            if (field & 1)
                pa->a_flags |= AF_ISPARM;
            if (field & 2)
                pa->a_flags |= AF_ISCRYS;
            if (tree[APDEFAPP].ob_state & SELECTED)
                pa->a_flags |= AF_APPDIR;
            if (tree[APPMFULL].ob_state & SELECTED)
                pa->a_flags |= AF_ISFULL;
            if (tree[APAUTO].ob_state & SELECTED)
            {
                clear_all_autorun();    /* only one autorun app is allowed */
                pa->a_flags |= AF_AUTORUN;
            }

            pa->a_letter = '\0';
            pa->a_type = AT_ISFILE;
            pa->a_obid = NIL;

            if (!installed)
            {
                strcat(pathname,pfname);    /* build full pathname */
                scan_str(pathname,&pa->a_pappl);
            }

            strcpy(name,"*.");
            inf_sget(tree,APDOCTYP,name+2);
            if (!installed || strcmp(name,pa->a_pdata)) /* doc type has changed */
                scan_str(name,&pa->a_pdata);

            inf_sget(tree,APARGS,name);
            scan_str(name,&pa->a_pargs);

            pa->a_aicon = IG_APPL;
            pa->a_dicon = IG_DOCU;
            pa->a_xspot = 0;
            pa->a_yspot = 0;
            change = 1;
            break;
        case APREMOVE:      /* remove an application */
            if (!installed)     /* mustn't remove default ANODE for app type */
                break;
            app_free(pa);
            change = 1;
            break;
        case APSKIP:        /* skip this application */
            break;
        case APCANCEL:      /* cancel further installs */
            change = -1;
            break;
        }
    } while(exitobj == -1);
    show_hide(FMD_FINISH, tree);

    return change;
}


 /*
 * insert icon into dialog
 */
static void insert_icon(OBJECT *tree, WORD obj, WORD nicon)
{
    OBJECT *objptr = tree + obj;
    ICONBLK *ibptr = &ib;

    memcpy(ibptr, &G.g_iblist[nicon], sizeof(ICONBLK));
    ibptr->ib_ptext = "";
    objptr->ob_type = G_ICON;
    objptr->ob_spec = (LONG)ibptr;
}


/*
 * install desktop icon
 *
 * returns:
 *  >0  icon has been changed
 *  0   icon not changed (user said skip)
 *  <0  icon not changed (user said cancel)
 */
static WORD install_desktop_icon(ANODE *pa)
{
    OBJECT *tree;
    BOOL icon_exists;
    WORD exitobj, x, y;
    WORD curr_icon, new_icon;
    WORD change = 0;
    BYTE curr_label[LEN_ZFNAME], new_label[LEN_ZFNAME], id[2];

    /* find first available spot on desktop (before we alloc a new one) */
    ins_posdisk(0, 0, &x, &y);

    icon_exists = pa ? TRUE : FALSE;

    /*
     * deselect all objects & hide printer button
     */
    tree = G.a_trees[ADINSDSK];
    deselect_all(tree);
    tree[ID_PRINT].ob_flags |= HIDETREE;

    /*
     * fill in dialog
     */
    if (!icon_exists)
    {
        pa = app_alloc(TRUE);
        if (!pa)
            return -1;              /* don't try any more */
        pa->a_flags = AF_ISDESK;
        pa->a_funkey = 0;
        pa->a_letter = '\0';
        pa->a_type = AT_ISDISK;
        pa->a_obid = 0;             /* filled in by app_blddesk() */
        pa->a_pappl = "";
        pa->a_pdata = "";
        pa->a_pargs = "";
        pa->a_aicon = IG_HARD;
        pa->a_dicon = NIL;
        snap_disk(x,y,&pa->a_xspot,&pa->a_yspot, 0, 0);
    }

    id[0] = id[1] = '\0';

    switch(pa->a_type)
    {
    case AT_ISFILE:
    case AT_ISFOLD:
        /* TODO, as per TOS2 (when we support files & folders on the desktop):
         *  disable ID_ID
         *  disable all the buttons
         */
        break;
    case AT_ISDISK:
        id[0] = pa->a_letter;
        /* drop thru */
    case AT_ISTRSH:
        inf_sset(tree, ID_ID, id);
        strcpy(curr_label, pa->a_pappl);
        inf_sset(tree, ID_LABEL, pa->a_pappl);
        if (pa->a_type == AT_ISDISK)
            tree[ID_DRIVE].ob_state |= SELECTED;
        else
            tree[ID_TRASH].ob_state |= SELECTED;
    }

    curr_icon = pa->a_aicon;
    if (curr_icon < 0)
        curr_icon = 0;
    else if (curr_icon > NUM_GEM_IBLKS-1)
        curr_icon = NUM_GEM_IBLKS - 1;
    new_icon = curr_icon;

    insert_icon(tree, ID_ICON, curr_icon);

    show_hide(FMD_START, tree);
    while(1)
    {
        exitobj = form_do(tree, ID_ID) & 0x7fff;

        switch(exitobj)
        {
        case ID_UP:             /* handle button up */
            new_icon = (curr_icon > 0) ? curr_icon-1 : 0;
            if (new_icon == IG_5RESV)   /* skip unused */
                new_icon = IG_TRASH;
            break;
        case ID_DOWN:           /* handle button down */
            new_icon = (curr_icon < NUM_GEM_IBLKS-1) ? curr_icon+1 : NUM_GEM_IBLKS-1;
            if (new_icon == IG_4RESV)   /* skip unused */
                new_icon = IG_APPL;
            break;
        case ID_OK:             /* (re)install an icon */
            if (inf_gindex(tree, ID_DRIVE, 3) == 0) /* only disks have a letter */
            {
                inf_sget(tree, ID_ID, id);
                pa->a_letter = id[0];
            }
            else
                pa->a_letter = '\0';
            switch(inf_gindex(tree, ID_DRIVE,3))
            {
            case 0:
                pa->a_type = AT_ISDISK;
                break;
            case 1:
                pa->a_type = AT_ISTRSH;
                break;
            }
            inf_sget(tree, ID_LABEL, new_label);
            if (strcmp(curr_label, new_label))      /* if label changed, */
                scan_str(new_label, &pa->a_pappl);  /* update it         */
            pa->a_aicon = curr_icon;
            change = 1;
            break;
        case ID_CNCL:           /* cancel further installs */
            change = -1;
            /* drop thru */
        case ID_SKIP:           /* skip this application */
            if (!icon_exists)       /* we allocated one */
                app_free(pa);       /* so we need to free it */
            break;
        }
        if ((exitobj != ID_UP) && (exitobj != ID_DOWN))
            break;
        tree[exitobj].ob_state &= ~SELECTED;

        if (new_icon != curr_icon)
        {
            curr_icon = new_icon;
            insert_icon(tree, ID_ICON, curr_icon);
        }
        draw_fld(tree, ID_IBOX);
    }
    show_hide(FMD_FINISH, tree);

    return change;
}


#if CONF_WITH_WINDOW_ICONS

#define NUM_EXTS    5
static const BYTE *exec_ext[NUM_EXTS] = { "TOS", "TTP", "PRG", "APP", "GTP" };

/*
 * test if file is executable, based on extension
 */
static BOOL is_executable(BYTE *filename)
{
    WORD i, n;

    n = strlen(filename);
    if (n < 5)          /* must be at least A.XYZ */
        return FALSE;

    filename += n - 3;  /* point to start of extension */

    for (i = 0; i < NUM_EXTS; i++)
        if (strcmp(filename,exec_ext[i]) == 0)
            return TRUE;

    return FALSE;
}


/*
 * set icon numbers into ANODE
 */
static void set_icon(ANODE *pa, WORD icon)
{
    pa->a_aicon = NIL;
    pa->a_dicon = icon;

    if (is_executable(pa->a_pdata))
    {
        pa->a_flags |= AF_ISEXEC;
        pa->a_aicon = icon;     /* this marks it as an executable file */
    }
}


/*
 * allocate ANODE for window icon
 */
static ANODE *allocate_window_anode(WORD type)
{
    ANODE *pa;

    pa = app_alloc(TRUE);
    if (!pa)
        return NULL;

    pa->a_flags = AF_WINDOW;
    pa->a_funkey = 0;
    pa->a_letter = '\0';
    pa->a_type = type;
    pa->a_obid = 0;
    pa->a_pappl = "";
    pa->a_pdata = "";
    pa->a_pargs = "";
    pa->a_aicon = NIL;
    pa->a_dicon = (type==AT_ISFOLD) ? IG_FOLDER : IG_DOCU;
    pa->a_xspot = 0;
    pa->a_yspot = 0;

    return pa;
}


/*
 * install window icon
 *
 * returns:
 *  >0  icon has been changed
 *  0   icon not changed (user said skip)
 *  <0  icon not changed (user said cancel)
 */
static WORD install_window_icon(FNODE *pf)
{
    BOOL identical;
    WORD edit_start, exitobj, dummy, type;
    WORD new_icon, curr_icon;
    WORD change = 0;
    OBJECT *tree;
    ANODE *pa;
    BYTE curr_name[LEN_ZFNAME], new_name[LEN_ZFNAME], temp[LEN_ZFNAME];

    /*
     * deselect all objects
     */
    tree = G.a_trees[ADINSWIN];
    deselect_all(tree);

    /*
     * initialise pointer to ANODE
     */
    pa = pf ? pf->f_pa : NULL;

    /*
     * fill in dialog
     */
    if (pf)
    {
        /*
         * handle existing FNODE:
         *  the name is fixed & not editable
         *  the file type (file or folder) is fixed & not changeable
         */
        strcpy(curr_name,pf->f_name);
        tree[IW_NAME].ob_flags &= ~EDITABLE;
        edit_start = 0;
        if (pa->a_type == AT_ISFILE)
            tree[IW_FILE].ob_state |= SELECTED;
        else
            tree[IW_FOLD].ob_state |= SELECTED;
        tree[IW_FILE].ob_state |= DISABLED;
        tree[IW_FOLD].ob_state |= DISABLED;
    }
    else
    {
        curr_name[0] = '\0';
        tree[IW_NAME].ob_flags |= EDITABLE;
        edit_start = IW_NAME;
        tree[IW_FILE].ob_state |= SELECTED;
        tree[IW_FILE].ob_state &= ~DISABLED;
        tree[IW_FOLD].ob_state &= ~DISABLED;
    }
    fmt_str(curr_name, temp);
    inf_sset(tree, IW_NAME, temp);

    curr_icon = pa ? pa->a_dicon : 0;
    if (curr_icon < 0)
        curr_icon = 0;
    else if (curr_icon > NUM_IBLKS-1)
        curr_icon = NUM_IBLKS - 1;
    new_icon = curr_icon;

    insert_icon(tree, IW_ICON, curr_icon);

    show_hide(FMD_START, tree);
    while(1)
    {
        exitobj = form_do(tree, edit_start) & 0x7fff;

        switch(exitobj)
        {
        case IW_UP:             /* handle button up */
            new_icon = (curr_icon > 0) ? curr_icon-1 : 0;
            if (new_icon == IG_5RESV)   /* skip unused */
                new_icon = IG_TRASH;
            break;
        case IW_DOWN:           /* handle button down */
            new_icon = (curr_icon < NUM_IBLKS-1) ? curr_icon+1 : NUM_IBLKS-1;
            if (new_icon == IG_4RESV)   /* skip unused */
                new_icon = IG_APPL;
            break;
        case IW_INST:           /* (re)install an icon */
        case IW_REMV:           /* remove an icon */
            inf_sget(tree, IW_NAME, temp);
            unfmt_str(temp, new_name);
            if (new_name[0] == '\0')    /* treat as skip */
                break;

            type = (tree[IW_FILE].ob_state & SELECTED) ? AT_ISFILE : AT_ISFOLD;
            if (!pf)
                pa = app_afind_by_name(type, "", new_name, &dummy);
            else pa = pf->f_pa;

            /* set flag if the names are exactly the same */
            identical = !strcmp(pa->a_pdata,new_name);

            if (exitobj == IW_INST)
            {
                if (!identical)
                {
                    pa = allocate_window_anode(type);
                    if (!pa)
                    {
                        change = -1;    /* cancel further installs */
                        break;
                    }
                    scan_str(new_name, &pa->a_pdata);
                    KDEBUG(("Installed window icon for %s\n",pa->a_pdata));
                }
                set_icon(pa, curr_icon);
            }
            else        /* IW_REMV */
            {
                if (!identical)
                {
                    fun_alert(1,STNOMTCH);  /* no matching file type */
                    break;
                }
                KDEBUG(("Removed window icon for %s\n",pa->a_pdata));
                app_free(pa);
            }
            change = 1;
            break;
        case IW_CNCL:           /* cancel further installs */
            change = -1;
            /* drop thru */
        case IW_SKIP:           /* skip this application */
            break;
        }
        if ((exitobj != IW_UP) && (exitobj != IW_DOWN))
            break;
        tree[exitobj].ob_state &= ~SELECTED;

        if (new_icon != curr_icon)
        {
            curr_icon = new_icon;
            insert_icon(tree, IW_ICON, curr_icon);
        }
        draw_fld(tree, IW_IBOX);
    }
    show_hide(FMD_FINISH, tree);

    return change;
}
#endif


/*
 * install icon (desktop or window)
 *
 * returns:
 *  >0      need to rebuild desktop
 *  0       no need to rebuild display
 *  <0      need to rebuild windows
 */
WORD ins_icon(WORD sobj)
{
    WORD rebuild = 0;
    WORD rc;
    ANODE *pa;
#if CONF_WITH_WINDOW_ICONS
    WNODE *pw;
    FNODE *pf;
#endif

    if (!sobj)              /* no icon selected */
    {
#if CONF_WITH_WINDOW_ICONS
        WORD icon_type;

        icon_type = fun_alert(1, STICNTYP);
        switch(icon_type)
        {
        case 1:         /* desktop */
            if (install_desktop_icon(NULL) > 0)
                return 1;
            break;
        case 2:         /* window */
            if (install_window_icon(NULL) > 0)
                return -1;
            break;
        default:        /* cancel */
            break;
        }
#else
        if (install_desktop_icon(NULL) > 0)
            return 1;
#endif
        return 0;
    }

    /*
     * handle one or more desktop icons
     */
    if ( (pa = i_find(0, sobj, NULL, NULL)) )
    {
        for ( ; sobj; sobj = win_isel(G.g_screen, DROOT, sobj))
        {
            pa = i_find(0, sobj, NULL, NULL);
            if (pa)
            {
                rc = install_desktop_icon(pa);
                if (rc > 0)
                    rebuild++;
                if (rc < 0)
                    break;
            }
        }
        return rebuild;
    }

#if CONF_WITH_WINDOW_ICONS
    /*
     * handle one or more window icons
     */
    pw = win_find(G.g_cwin);    /* get WNODE for current window */
    if (pw)
    {
        for (pf = pw->w_path->p_flist; pf; pf = pf->f_next)
        {
            if (pf->f_obid == NIL)
                continue;
            if (!(G.g_screen[pf->f_obid].ob_state & SELECTED))
                continue;
            rc = install_window_icon(pf);
            if (rc > 0)
                rebuild--;
            if (rc < 0)
                break;
        }
    }
#endif

    return rebuild;
}


/*
 * remove desktop icons
 */
WORD rmv_icon(WORD sobj)
{
    ANODE *pa;
    WORD icons_removed = 0;

    for ( ; sobj; sobj = win_isel(G.g_screen, DROOT, sobj))
    {
        pa = i_find(0,sobj,NULL,NULL);
        if (!pa)
            continue;
        if ((pa->a_type == AT_ISDISK) || (pa->a_type == AT_ISTRSH))
        {
            app_free(pa);
            icons_removed++;
        }
    }

    return icons_removed;
}
