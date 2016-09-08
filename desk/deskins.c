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

#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "../aes/gemoblib.h"
#include "deskbind.h"

#include "../aes/optimize.h"
#include "../aes/optimopt.h"
#include "../aes/rectfunc.h"
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
    {
        fun_alert(1, STAPGONE);
        return -1;
    }

    pa->a_flags = AF_ISCRYS | AF_ISDESK;
    pa->a_funkey = 0;
    pa->a_letter = 'A' + drive;
    pa->a_type = AT_ISDISK;
    pa->a_obid = 0;     /* fixed up when deskmain() calls app_blddesk() */
    sprintf(G.g_1text,"%s %c",ini_str(STDISK),pa->a_letter);
    scan_str(G.g_1text, &pa->a_pappl);  /* set up disk name */
    scan_str("", &pa->a_pdata);         /* points to empty string */
    pa->a_aicon = (drive > 1) ? IG_HARD : IG_FLOPPY;
    pa->a_dicon = NIL;
    snap_disk(x,y,&pa->a_xspot,&pa->a_yspot);

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
            drivebits &= ~(1<<(pa->a_letter-'A'));

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

    inf_sget((LONG)tree,APFUNKEY,fkey);
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
    OBJECT *tree, *obj;
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
    obj = tree = (OBJECT *)G.a_trees[ADINSAPP];
    do {
        obj->ob_state &= ~SELECTED;
    } while(!(obj++->ob_flags&LASTOB));

    /*
     * fill in dialog
     */
    fmt_str(pfname, name);
    inf_sset((LONG)tree, APNAME, name);
    inf_sset((LONG)tree, APARGS, installed ? pa->a_pargs : "");
    inf_sset((LONG)tree, APDOCTYP, installed ? pa->a_pdata+2 : "");
    if (pa->a_funkey)
        sprintf(name, "%02d", pa->a_funkey);
    else name[0] = '\0';
    inf_sset((LONG)tree, APFUNKEY, installed ? name : "");

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

    show_hide(FMD_START, (LONG)tree);
    do
    {
        exitobj = form_do((LONG)tree, APARGS);

        switch(exitobj&0x7fff)
        {
        case APINSTAL:      /* (re)install an application */
            if (!installed)
                pa = app_alloc(TRUE);
            if (!pa)
            {
                fun_alert(1, STAPGONE);
                change = -1;        /* don't try any more */
                break;
            }

            funkey = get_funkey(tree,pa,installed);
            if (funkey < 0)
            {
                inf_sset((LONG)tree, APFUNKEY, "");
                tree[APINSTAL].ob_state &= ~SELECTED;
                if (!installed)
                    app_free(pa);
                draw_dial((LONG)tree);
                exitobj = -1;       /* request retry */
                break;
            }
            pa->a_funkey = funkey;  /* now we can store it */

            pa->a_flags = 0;
            field = inf_gindex((LONG)tree, APTOS, APGTP-APTOS+1);
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
            inf_sget((LONG)tree,APDOCTYP,name+2);
            if (!installed || strcmp(name,pa->a_pdata)) /* doc type has changed */
                scan_str(name,&pa->a_pdata);

            inf_sget((LONG)tree,APARGS,name);
            scan_str(name,&pa->a_pargs);

#if HAVE_APPL_IBLKS
            pa->a_aicon = IA_GENERIC;
            pa->a_dicon = ID_GENERIC;
#else
            pa->a_aicon = IA_GENERIC_ALT;
            pa->a_dicon = ID_GENERIC_ALT;
#endif
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
    show_hide(FMD_FINISH, (LONG)tree);

    return change;
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
        if (pa->a_type == AT_ISDISK)
        {
            app_free(pa);
            icons_removed++;
        }
    }

    return icons_removed;
}
