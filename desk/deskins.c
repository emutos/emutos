/*      DESKINS.C       09/22/84 - 06/19/85     Lee Lorenzen            */
/*      for 3.0         02/28/86                        MDF             */
/*      merge source    5/27/87  - 5/28/87              mdf             */

/*
 *      This module contains routines for three functions:
 *          . install devices
 *          . install application
 *          . remove desktop icons
 *
 *      Copyright 2002-2020 The EmuTOS development team
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

#include "emutos.h"
#include "string.h"
#include "obdefs.h"
#include "gemdos.h"
#include "optimize.h"

#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "aesbind.h"
#include "deskinf.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskdir.h"
#include "deskmain.h"
#include "icons.h"
#include "intmath.h"
#include "deskins.h"
#include "desksupp.h"


/*
 * used by insert_icon()
 */
static ICONBLK ib;


#ifdef ENABLE_KDEBUG
static void anode_dump(char *msg)
{
    ANODE *pa;

    kprintf("%s:\n",msg);
    for (pa = G.g_ahead; pa; pa = pa->a_next)
        kprintf("  flags=0x%04x,type=%d,aicon=%d,dicon=%d,appl=%s,data=%s\n",
                pa->a_flags,pa->a_type,pa->a_aicon,pa->a_dicon,pa->a_pappl,pa->a_pdata);
}
#endif


/*
 *  Routine to tell if an icon has an associated document type
 */
WORD is_installed(ANODE *pa)
{
#if CONF_WITH_DESKTOP_SHORTCUTS
    if (pa->a_flags & AF_ISDESK)
        return FALSE;
#endif
    if ((*pa->a_pappl == '*') || (*pa->a_pappl == '?') || (*pa->a_pappl == '\0'))
        return FALSE;
    return TRUE;
}


#if CONF_WITH_VIEWER_SUPPORT
/*
 *  Routine to tell if an anode is suitable as an installed viewer
 */
BOOL is_viewer(ANODE *pa)
{
    char *p;
    WORD found, component;

    /*
     * must be a file with non-negative icon numbers
     */
    if ((pa->a_type != AT_ISFILE) || (pa->a_aicon < 0) || (pa->a_dicon < 0))
        return FALSE;

    /*
     * pdata must be an all-wildcard spec: we make sure both components
     * of the filename are wildcard-only
     */
    for (found = 0, component = 1, p = pa->a_pdata; *p; p++)
    {
        if ((*p == '*') || (*p == '?'))
            found |= component;
        else if (*p == '.')
            component <<= 1;
    }

    return (found==3) ? TRUE : FALSE;
}
#endif


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
 *  Align icon on a grid
 *
 *  input:
 *      x/y: mouse cursor location
 *      sxoff/syoff: offset of cursor from upper left of target icon position
 *  returns:
 *      px/py: 'snapped' upper left of target icon position
 */
void snap_icon(WORD x, WORD y, WORD *px, WORD *py, WORD sxoff, WORD syoff)
{
    WORD xgrid, ygrid, icw, ich;
    WORD columns, rows, spare_pixels;

    /*
     * Determine:
     * (1) the number of columns that can fit, and
     * (2) the total number of spare pixels that should be distributed
     *     evenly between the columns
     * Then determine the x grid position corresponding to x/sxoff
     * and convert it to pixels
     */
    icw  = G.g_icw;
    columns = G.g_wdesk / icw;
    spare_pixels = G.g_wdesk - (columns * icw);

    xgrid = (x - sxoff + (icw / 2)) / icw;  /* x grid position */
    xgrid = min(xgrid, columns-1);          /* clamp it for safety */
    *px = (xgrid * icw) + (spare_pixels / columns);

    /*
     * Determine:
     * (1) the number of rows that can fit, and
     * (2) the total number of spare pixels that should be distributed
     *     evenly between the rows
     * Then determine the y grid position corresponding to y/syoff
     * and convert it to pixels
     */
    ich = G.g_ich;
    rows = G.g_hdesk / ich;
    spare_pixels = G.g_hdesk - (rows * ich);

    y -= G.g_ydesk;
    ygrid  = (y - syoff + (ich / 2)) / ich; /* y grid position */
    ygrid = min(ygrid, rows-1);             /* clamp it for safety */
    *py = (ygrid * ich) + (spare_pixels / rows);
    *py += G.g_ydesk;
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

    pa = app_alloc();
    if (!pa)
        return -1;

    pa->a_flags = AF_ISDESK;
    pa->a_funkey = 0;
    pa->a_letter = 'A' + drive;
    pa->a_type = AT_ISDISK;
    pa->a_obid = 0;     /* fixed up when deskmain() calls app_blddesk() */
    sprintf(G.g_work,"%s %c",desktop_str_addr(STDISK),pa->a_letter);
    scan_str(G.g_work, &pa->a_pappl);   /* set up disk name */
    pa->a_pdata = "";                   /* point to empty string */
    pa->a_aicon = (drive > 1) ? IG_HARD : IG_FLOPPY;
    pa->a_dicon = NIL;
    snap_icon(x,y,&pa->a_xspot,&pa->a_yspot, 0, 0);

    return 0;
}


/*
 *  Remove ANODE for one drive
 *
 *  Returns -1 for error (couldn't find ANODE)
 */
static WORD remove_drive(WORD drive)
{
    ANODE *pa;
    WORD letter = drive + 'A';

    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if (pa->a_type == AT_ISDISK)
        {
            if (pa->a_letter == letter)
            {
                app_free(pa);
                return 0;
            }
        }
    }

    return -1;
}


/*
 *  Install devices: removes desktop icons for non-existent devices, then
 *  installs an icon on the desktop for existing devices without an icon
 *
 *  Returns TRUE iff any changes were made, else FALSE
 */
BOOL ins_devices(void)
{
    ULONG current, installed, drivebits, mask;
    WORD drive;
    BOOL change = FALSE;
    ANODE *pa;

    current = dos_sdrv(dos_gdrv());     /* all current devices */

    /*
     * scan ANODEs and build bitmask of installed devices
     */
    for (pa = G.g_ahead, installed = 0UL; pa; pa = pa->a_next)
        if (pa->a_type == AT_ISDISK)
            installed |= 1L << (pa->a_letter - 'A');

    KDEBUG(("current=0x%08lx, installed=0x%08lx\n",current,installed));

    /*
     * remove icons for non-existent devices
     */
    drivebits = installed & ~current;

    for (drive = 0, mask = 1L; drive < BLKDEVNUM; drive++, mask <<= 1)
    {
        if (drivebits & mask)
        {
            KDEBUG(("removing ANODE for device %c\n",drive+'A'));
            if (remove_drive(drive) == 0)
                change = TRUE;
        }
    }

    /*
     * install icons for devices without one
     */
    drivebits = current & ~installed;

    for (drive = 0, mask = 1L; drive < BLKDEVNUM; drive++, mask <<= 1)
    {
        if (drivebits & mask)
        {
            KDEBUG(("installing ANODE for device %c\n",drive+'A'));
            if (install_drive(drive) == 0)
                change = TRUE;
        }
    }

    return change;
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


#if CONF_WITH_VIEWER_SUPPORT
/*
 * remove all ANODEs that have the viewer flag set: we expect there
 * will be a maximum of 1, but we handle any number
 */
static void remove_all_viewers(void)
{
    ANODE *pa, *next;

    for (pa = G.g_ahead; pa; pa = next)
    {
        next = pa->a_next;      /* remember in case we free below */
        if (pa->a_flags & AF_VIEWER)
        {
            KDEBUG(("removing existing default viewer %s\n",pa->a_pappl));
            app_free(pa);
        }
    }
}
#endif


/*
 * convert ascii to WORD
 *
 * stops at nul byte or first non-decimal character
 */
static WORD atow(char *s)
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
    char fkey[3];

    inf_sget(tree,APFUNKEY,fkey);
    if (fkey[0] == '\0')                /* empty? */
        return 0;

    funkey = atow(fkey);
    if (installed && (funkey == pa->a_funkey))  /* unchanged? */
        return funkey;

    /*
     * this is a new ANODE, or the key has changed: validate it
     */
    if ((funkey < FIRST_FUNKEY) || (funkey > LAST_FUNKEY))
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
    BOOL installed, viewer;
    char *pfname, *p, *q;
    char name[LEN_ZFNAME];
    char pathname[MAXPATHLEN];

    /*
     * is this a valid anode (-ve handle => include check for default viewer) ?
     * if so, see if this is the default viewer
     */
    pa = i_find(-G.g_cwin, curr, &pf, &isapp);
    if (!pa)
        return 0;
    viewer = (pa->a_flags & AF_VIEWER) ? TRUE : FALSE;

#if CONF_WITH_DESKTOP_SHORTCUTS
    /*
     * here we handle the case of installing an application identified by
     * a desktop shortcut.  we need to determine if this application is
     * already installed, i.e. if there already exists a non-shortcut ANODE
     * for this application.
     *
     * if so, we change the ANODE pointer to point to it, and continue as
     * though the user has selected the application in a desktop window.
     *
     * if not, we handle first-time installation later below.
     */
    if (G.g_cwin == DESKWH) /* we're on the desktop, so this is a shortcut icon */
    {
        ANODE *temppa;

        strcpy(pathname,pa->a_pdata);   /* get path for app_afind_by_name() */
        p = filename_start(pathname);
        *p = '\0';
        temppa = app_afind_by_name(AT_ISFILE,AF_ISDESK|AF_WINDOW,pathname,pa->a_pappl,&isapp);
        if (temppa)
        {
            if (strcmp(temppa->a_pappl,pa->a_pdata) == 0)
            {
                pa = temppa;
                KDEBUG(("Found installed app anode for desktop shortcut\n"));
            }
        }
    }
#endif

    installed = is_installed(pa) || viewer; /* the viewer is always installed */

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
#if CONF_WITH_DESKTOP_SHORTCUTS
        /*
         * handle install application for a desktop shortcut when there
         * is no existing 'install application' anode
         */
        if (pa->a_flags & AF_ISDESK)
        {
            p = pa->a_pdata;
            q = filename_start(p);
            pfname = pa->a_pappl;
        }
        else
#endif
        {
            pw = win_find(G.g_cwin);
            p = pw->w_pnode.p_spec;
            q = filename_start(p);
            pfname = pf->f_name;
        }
    }
    strlcpy(pathname,p,q-p+1);  /* copy pathname including trailing backslash */

    /*
     * deselect all objects
     */
    tree = desk_rs_trees[ADINSAPP];
    deselect_all(tree);

    /*
     * fill in dialog
     */
    set_tedinfo_name(tree, APNAME, pfname);
    inf_sset(tree, APARGS, installed ? pa->a_pargs : "");
    inf_sset(tree, APDOCTYP, installed ? pa->a_pdata+2 : "");
    if (pa->a_funkey)
        sprintf(name, "%u ", pa->a_funkey); /* inf_sset() will truncate if necessary */
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

    start_dialog(tree);
    do
    {
        exitobj = form_do(tree, APARGS);

        switch(exitobj&0x7fff)
        {
        case APINSTAL:      /* (re)install an application */
            if (!installed)
                pa = app_alloc();
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
            if (name[2] == '*')         /* prevent a badly-formed wildcard spec */
                name[3] = '\0';
            if (!installed || strcmp(name,pa->a_pdata)) /* doc type has changed */
                scan_str(name,&pa->a_pdata);

            inf_sget(tree,APARGS,name);
            scan_str(name,&pa->a_pargs);

#if CONF_WITH_VIEWER_SUPPORT
            if (is_viewer(pa))
            {
                remove_all_viewers();       /* remove any existing default viewer(s) */
                pa->a_flags |= AF_VIEWER;   /* mark app as default viewer */
                KDEBUG(("adding new default viewer %s\n",pa->a_pappl));
            }
#endif

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
    end_dialog(tree);

    return change;
}


 /*
 * insert icon into dialog
 */
static void insert_icon(OBJECT *tree, WORD obj, WORD nicon)
{
    OBJECT *objptr = tree + obj;

    ib = G.g_iblist[nicon];
    ib.ib_ptext = "";
    objptr->ob_type = G_ICON;
    objptr->ob_spec = (LONG)&ib;
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
    WORD start_fld = ID_ID;
    WORD change = 0;
    char curr_label[LEN_ZFNAME], new_label[LEN_ZFNAME], id[2];

    /* find first available spot on desktop (before we alloc a new one) */
    ins_posdisk(0, 0, &x, &y);

    icon_exists = pa ? TRUE : FALSE;

    /*
     * deselect all objects & hide printer button if not supported
     */
    tree = desk_rs_trees[ADINSDSK];
    deselect_all(tree);
#if !CONF_WITH_PRINTER_ICON
    tree[ID_PRINT].ob_flags |= HIDETREE;
#endif

    /*
     * fill in dialog
     */
    if (!icon_exists)
    {
        pa = app_alloc();
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
        snap_icon(x,y,&pa->a_xspot,&pa->a_yspot, 0, 0);
    }

    id[0] = id[1] = '\0';

    switch(pa->a_type)
    {
    case AT_ISFILE:
    case AT_ISFOLD:
#if CONF_WITH_DESKTOP_SHORTCUTS
        /*
         * for desktop file/folder icons, only label & shape can be changed
         */
        tree[ID_ID].ob_state |= DISABLED;
        tree[ID_DRIVE].ob_state |= DISABLED;
        tree[ID_TRASH].ob_state |= DISABLED;
        tree[ID_PRINT].ob_state |= DISABLED;
        start_fld = ID_LABEL;
#endif
        break;
    case AT_ISDISK:
        id[0] = pa->a_letter;
        FALLTHROUGH;
#if CONF_WITH_PRINTER_ICON
    case AT_ISPRNT:
#endif
    case AT_ISTRSH:
        inf_sset(tree, ID_ID, id);
        if (pa->a_type == AT_ISDISK)
            tree[ID_DRIVE].ob_state |= SELECTED;
#if CONF_WITH_PRINTER_ICON
        else if (pa->a_type == AT_ISPRNT)
            tree[ID_PRINT].ob_state |= SELECTED;
#endif
        else
            tree[ID_TRASH].ob_state |= SELECTED;
#if CONF_WITH_DESKTOP_SHORTCUTS
        /*
         * for desktop disk/trash/printer icons, anything can be changed
         */
        tree[ID_ID].ob_state &= ~DISABLED;
        tree[ID_DRIVE].ob_state &= ~DISABLED;
        tree[ID_TRASH].ob_state &= ~DISABLED;
        tree[ID_PRINT].ob_state &= ~DISABLED;
#endif
    }
    strcpy(curr_label, pa->a_pappl);
    inf_sset(tree, ID_LABEL, pa->a_pappl);

    curr_icon = (pa->a_aicon < 0) ? pa->a_dicon : pa->a_aicon;
    if (curr_icon < 0)
        curr_icon = 0;
    else if (curr_icon > G.g_numiblks-1)
        curr_icon = G.g_numiblks - 1;
    new_icon = curr_icon;

    insert_icon(tree, ID_ICON, curr_icon);

    start_dialog(tree);
    while(1)
    {
        BOOL retry = FALSE;

        exitobj = form_do(tree, start_fld) & 0x7fff;

        switch(exitobj)
        {
        case ID_UP:             /* handle button up */
            new_icon = (curr_icon > 0) ? curr_icon-1 : 0;
            break;
        case ID_DOWN:           /* handle button down */
            new_icon = (curr_icon < G.g_numiblks-1) ? curr_icon+1 : G.g_numiblks-1;
            break;
        case ID_OK:             /* (re)install an icon */
            pa->a_letter = '\0';            /* default is no letter */
            switch(inf_gindex(tree, ID_DRIVE,3))
            {
            case 0:
                pa->a_type = AT_ISDISK;
                inf_sget(tree, ID_ID, id);  /* disks must have a letter */
                pa->a_letter = id[0];
                if ((id[0] == '\0') || (id[0] == ' '))  /* must have something */
                    retry = TRUE;                       /* else force retry    */
                break;
            case 1:
                pa->a_type = AT_ISTRSH;
                break;
#if CONF_WITH_PRINTER_ICON
            case 2:
                pa->a_type = AT_ISPRNT;
                break;
#endif
            }
            inf_sget(tree, ID_LABEL, new_label);
            if (strcmp(curr_label, new_label))      /* if label changed, */
                scan_str(new_label, &pa->a_pappl);  /* update it         */
            if (pa->a_aicon < 0)
                pa->a_dicon = curr_icon;
            else
                pa->a_aicon = curr_icon;
            change = 1;
            break;
        case ID_CNCL:           /* cancel further installs */
            change = -1;
            FALLTHROUGH;
        case ID_SKIP:           /* skip this application */
            if (!icon_exists)       /* we allocated one */
                app_free(pa);       /* so we need to free it */
            break;
        }
        tree[exitobj].ob_state &= ~SELECTED;

        if (retry)
        {
            fun_alert(1, STDRIVID); /* issue error alert */
            draw_dial(tree);        /* redraw original dialog */
            continue;
        }

        if ((exitobj != ID_UP) && (exitobj != ID_DOWN))
            break;

        if (new_icon != curr_icon)
        {
            curr_icon = new_icon;
            insert_icon(tree, ID_ICON, curr_icon);
        }
        draw_fld(tree, ID_IBOX);
    }
    end_dialog(tree);

    return change;
}


#if CONF_WITH_DESKTOP_SHORTCUTS || CONF_WITH_WINDOW_ICONS

#define EXT_LENGTH 3
static const char exec_ext[][EXT_LENGTH+1] = { "TOS", "TTP", "PRG", "APP", "GTP" };

/*
 * test if file is executable, based on extension
 */
BOOL is_executable(const char *filename)
{
    WORD i, n;

    n = strlen(filename);
    if (n < (2 + EXT_LENGTH))       /* must be at least A.XYZ */
        return FALSE;

    filename += n - EXT_LENGTH;     /* point to start of extension */

    for (i = 0; i < ARRAY_SIZE(exec_ext); i++)
        if (strcmp(filename,exec_ext[i]) == 0)
            return TRUE;

    return FALSE;
}
#endif


#if CONF_WITH_WINDOW_ICONS
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

    pa = app_alloc();
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
    char curr_name[LEN_ZFNAME], new_name[LEN_ZFNAME], temp[LEN_ZFNAME];

    /*
     * deselect all objects
     */
    tree = desk_rs_trees[ADINSWIN];
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
    set_tedinfo_name(tree, IW_NAME, curr_name);

    curr_icon = pa ? pa->a_dicon : 0;
    if (curr_icon < 0)
        curr_icon = 0;
    else if (curr_icon > G.g_numiblks-1)
        curr_icon = G.g_numiblks - 1;
    new_icon = curr_icon;

    insert_icon(tree, IW_ICON, curr_icon);

    start_dialog(tree);
    while(1)
    {
        exitobj = form_do(tree, edit_start) & 0x7fff;

        switch(exitobj)
        {
        case IW_UP:             /* handle button up */
            new_icon = (curr_icon > 0) ? curr_icon-1 : 0;
            break;
        case IW_DOWN:           /* handle button down */
            new_icon = (curr_icon < G.g_numiblks-1) ? curr_icon+1 : G.g_numiblks-1;
            break;
        case IW_INST:           /* (re)install an icon */
        case IW_REMV:           /* remove an icon */
            inf_sget(tree, IW_NAME, temp);
            unfmt_str(temp, new_name);
            if (new_name[0] == '\0')    /* treat as skip */
                break;

            type = (tree[IW_FILE].ob_state & SELECTED) ? AT_ISFILE : AT_ISFOLD;
            if (!pf)
                pa = app_afind_by_name(type, AF_ISDESK, "", new_name, &dummy);
            else pa = pf->f_pa;

            if (!pa)                /* can't happen ... */
            {
                KDEBUG(("Null anode ptr for window icon, skipping\n"));
                break;                  /* treat as skip */
            }

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
            FALLTHROUGH;
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
    end_dialog(tree);

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
    if ( (app_afind_by_id(sobj)) )
    {
        for ( ; sobj; sobj = win_isel(G.g_screen, DROOT, sobj))
        {
            pa = app_afind_by_id(sobj);
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
        for (pf = pw->w_pnode.p_flist; pf; pf = pf->f_next)
        {
            if (!fnode_is_selected(pf))
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
        pa = app_afind_by_id(sobj);
        if (!pa)
            continue;
        if (pa->a_flags & AF_ISDESK)
        {
            app_free(pa);
            icons_removed++;
        }
    }

    return icons_removed;
}

#if CONF_WITH_DESKTOP_SHORTCUTS
/*
 * install desktop shortcut
 */
void ins_shortcut(WORD wh, WORD mx, WORD my)
{
    char pathname[MAXPATHLEN], *p, *q;
    WORD sobj, x, y, dummy;
    ANODE *pa, *newpa;
    FNODE *pf;
    WNODE *pw;

    /*
     * put the first icon on the grid where the user wanted
     */
    snap_icon(mx, my, &x, &y, 0, 0);

    /*
     * install each selected icon on desktop
     */
    sobj = 0;
    while ((sobj = win_isel(G.g_screen, G.g_croot, sobj)))
    {
        pa = i_find(wh, sobj, &pf, NULL); /* get ANODE of source */
        if (!pa)
            continue;
        pw = win_find(wh);                  /* get WNODE of source */
        if (!pw)
            continue;
        newpa = app_alloc();                /* ok, so create new ANODE */
        if (!newpa)
            break;

        /*
         * build the full pathname
         */
        p = pw->w_pnode.p_spec;
        q = filename_start(p);
        strlcpy(pathname,p,q-p+1);  /* copy pathname including trailing backslash */
        strcat(pathname,pf->f_name);

        /*
         * set up the new ANODE
         */
        newpa->a_flags = (pa->a_flags & ~AF_WINDOW) | AF_ISDESK;
        if ((pa->a_type == AT_ISFILE) && is_executable(pf->f_name))
            newpa->a_flags |= AF_ISEXEC;
        newpa->a_funkey = 0;
        newpa->a_letter = '\0';
        newpa->a_type = pa->a_type;
        newpa->a_obid = 0;          /* filled in by app_blddesk() */
        scan_str(pf->f_name,&newpa->a_pappl);   /* store name */
        scan_str(pathname,&newpa->a_pdata);     /* store full path */
        newpa->a_pargs = "";
        newpa->a_aicon = pa->a_aicon;
        newpa->a_dicon = pa->a_dicon;
        newpa->a_xspot = x;
        newpa->a_yspot = y;
        ins_posdisk(x, y, &x, &y);  /* update position for next icon */
        /*
         * override the default icon with an installed icon (if it exists)
         */
        pa = app_afind_by_name((pf->f_attr&FA_SUBDIR)?AT_ISFOLD:AT_ISFILE,
                        AF_ISDESK, pw->w_pnode.p_spec, pf->f_name, &dummy);
        if (pa)                     /* paranoia */
        {
            newpa->a_aicon = pa->a_aicon;
            newpa->a_dicon = pa->a_dicon;
        }
    }

    app_blddesk();
}
#endif
