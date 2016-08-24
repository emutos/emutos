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
#include "kprint.h"


#if HAVE_APPL_IBLKS
static ICONBLK  gl_aib;
static ICONBLK  gl_dib;
#endif

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
        fun_alert(1, STAPGONE, NULL);
        return -1;
    }

    pa->a_flags = AF_ISCRYS | AF_ISDESK;
    pa->a_rsvd = 0;
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


#if HAVE_APPL_IBLKS
static void insa_icon(LONG tree, WORD obj, WORD nicon, ICONBLK *pic, BYTE *ptext)
{
    OBJECT *objptr = (OBJECT *)tree + obj;

    memcpy(pic, &G.g_iblist[nicon], sizeof(ICONBLK));
    pic->ib_ptext = ptext;
    objptr->ob_type = G_ICON;
    objptr->ob_spec = (LONG)pic;
}
#endif


static void insa_elev(LONG tree, WORD nicon, WORD numics)
{
    WORD y, h, th;
    const char *lp;
    OBJECT *obj;

    y = 0;
    obj = (OBJECT *)tree + APFSVSLI;
    th = h = obj->ob_height;
    if (numics > 1)
    {
        h = mul_div(1, h, numics);
        h = max((gl_hbox/2)+2, h);          /* min size elevator    */
        y = mul_div(nicon, th-h, numics-1);
    }

    obj = (OBJECT *)tree + APFSVELE;
    obj->ob_y = y;
    obj->ob_height = h;

#if HAVE_APPL_IBLKS
    strcpy(&G.g_1text[0], ini_str(STAPPL));
    insa_icon(tree, APF1NAME, IA_GENERIC+nicon, &gl_aib, &G.g_1text[0]);

    strcpy(&G.g_2text[0], ini_str(STDOCU));
    insa_icon(tree, APF2NAME, ID_GENERIC+nicon, &gl_dib, &G.g_2text[0]);

    lp = icon_rs_fstr[nicon];
#else
    lp = ini_str(STNOTAVL);
#endif

    inf_sset(tree, APFTITLE, (BYTE *)lp );
}


static WORD insa_dial(LONG tree, WORD nicon, WORD numics)
{
    WORD firstslot, i;
    WORD touchob, oicon, value;
    WORD mx, my, kret, bret, cont;
    BYTE   *pstr, doctype[4];
    GRECT  pt;
    OBJECT *obj;

    /* draw the form */
    show_hide(FMD_START, tree);

    /* init for while loop by forcing initial fs_newdir call */
    cont = TRUE;
    while(cont)
    {
        firstslot = 6;
        for (i = 0; i < firstslot; i++)
        {
            pstr = &doctype[0];
            inf_sget(tree, APDFTYPE+i, pstr);
            if (*pstr == '\0')
                firstslot = i;
        }
        touchob = form_do(tree, APDFTYPE+firstslot);
        graf_mkstate(&mx, &my, &kret, &bret);

        value = 0;
        touchob &= 0x7fff;
        switch(touchob)
        {
        case APINST:
        case APREMV:
        case APCNCL:
            cont = FALSE;
            break;
        case APFUPARO:
            value = -1;
            break;
        case APFDNARO:
            value = 1;
            break;
        case APFSVSLI:
/* BugFix       */
            ob_actxywh(tree, APFSVELE, &pt);
            pt.g_x -= 3;
            pt.g_w += 6;
            if (inside(mx, my, &pt))
                goto dofelev;
            value = (my <= pt.g_y) ? -1 : 1;
            break;
        case APFSVELE:
dofelev:    wind_update(3);
            ob_relxywh(tree, APFSVSLI, &pt);
            pt.g_x += 3;
            pt.g_w -= 6;
            obj = (OBJECT *)tree + APFSVSLI;
            obj->ob_x = pt.g_x;
            obj->ob_width = pt.g_w;
            value = graf_slidebox(tree, APFSVSLI, APFSVELE, TRUE);
            pt.g_x -= 3;
            pt.g_w += 6;
            obj->ob_x = pt.g_x;
            obj->ob_width = pt.g_w;
            wind_update(2);
            value = mul_div(value, numics-1, 1000) - nicon;
            break;
        }
        if (value)
        {
            oicon = nicon;
            nicon += value;
            if (nicon < 0)
                nicon = 0;
            if (nicon >= numics)
                nicon = numics - 1;
            if (oicon != nicon)
            {
                insa_elev(tree, nicon, numics);
                draw_fld(tree, APFTITLE);
                draw_fld(tree, APFSVSLI);
                draw_fld(tree, APFILEBO);
            }
        }
    }

    /* undraw the form */
    show_hide(FMD_FINISH, tree);
    return nicon;
}


static void insa_gtypes(LONG tree, BYTE *ptypes)
{
    WORD i, j;
    BYTE *pstr, doctype[4];

    j = 0;
    *ptypes = '\0';
    for (i = 0; i < 8; i++)
    {
        pstr = &doctype[0];
        inf_sget(tree, APDFTYPE+i, pstr);
        if (*pstr)
        {
            if (j != 0)
                ptypes[j++] = ',';
            strcpy(&ptypes[j], "*.*");
            strcpy(&ptypes[j+2], pstr);
            j += 2 + strlen(pstr);
        }
    }
}


static void insa_stypes(LONG tree, BYTE *pdata)
{
    WORD i;
    BYTE *pstr, doctype[4];

    for (i = 0; i < 8; i++)
    {
        pdata = scasb(pdata, '.');
        if (*pdata == '.')
            pdata++;
        pstr = &doctype[0];
        while ((*pdata) && (*pdata != ','))
            *pstr++ = *pdata++;
        *pstr = '\0';
        inf_sset(tree, APDFTYPE+i, &doctype[0]);
    }
}



/************************************************************************/
/* i n s _ a p p                                                        */
/************************************************************************/
WORD ins_app(BYTE *pfname, ANODE *pa)
{
    OBJECT *tree;
    ANODE *newpa;
    BYTE pname[12];
    BYTE ntypes[6*8];
    WORD oicon, nicon;
    WORD oflag, nflag;
    WORD change, field;
    WORD uninstalled, h;

    tree = (OBJECT *)G.a_trees[ADINSAPP];

    h = tree[APSCRLBA].ob_height;
    tree[APFUPARO].ob_height = gl_hbox + 2;
    tree[APFSVSLI].ob_y = gl_hbox + 2;
    tree[APFSVSLI].ob_height = h - (2 * (gl_hbox + 2));
    tree[APFDNARO].ob_y = h - (gl_hbox + 2);
    tree[APFDNARO].ob_height = gl_hbox + 2;

    uninstalled = !is_installed(pa);
    tree[APREMV].ob_state = uninstalled ? DISABLED : NORMAL;

    /* stuff in appl name */
    fmt_str(pfname, &pname[0]);
    inf_sset((LONG)tree, APNAME, &pname[0]);

    /* stuff in docu types  */
    insa_stypes((LONG)tree, pa->a_pdata);
    oflag = pa->a_flags;
    if (pa->a_flags & AF_ISCRYS)
    {
        field = APGEM;
    }
    else
        field = (pa->a_flags & AF_ISPARM) ? APPARMS : APDOS;
    tree[field].ob_state = SELECTED;

    if (pa->a_aicon == IA_GENERIC_ALT)
        oicon = 0;
    else oicon = pa->a_aicon - IA_GENERIC;

    insa_elev((LONG)tree, oicon, gl_numics);
    nicon = insa_dial((LONG)tree, oicon, gl_numics);
    change = FALSE;

    /* set type flags */
    nflag = 0;
    field = inf_gindex((LONG)tree, APGEM, 3);
    if (field == 0)
        nflag = AF_ISCRYS;
    if (field == 2)
        nflag = AF_ISPARM;
    tree[APGEM+field].ob_state = NORMAL;

    /* get button selection */
    field = inf_gindex((LONG)tree, APINST, 3);
    tree[APINST+field].ob_state = NORMAL;

    if (field == 0)
    {
        /* install the appl. if it's uninstalled or has new types */
        insa_gtypes((LONG)tree, &ntypes[0]);
        if (uninstalled || (strcmp(&ntypes[0], pa->a_pdata)))
        {
            newpa = (uninstalled) ? app_alloc(TRUE) : pa;

            if (newpa)
            {
                if ( (uninstalled) ||
                    (strcmp(&ntypes[0], pa->a_pdata)) )
                {
                    change = TRUE;
                    scan_str(&ntypes[0], &newpa->a_pdata);
                }

                if (newpa != pa)
                {
                    uninstalled = change = TRUE;
                    scan_str(pfname, &newpa->a_pappl);
                    newpa->a_flags = nflag;
                    newpa->a_type = AT_ISFILE;
                    newpa->a_obid = NIL;
                    newpa->a_letter = '\0';
                    newpa->a_xspot = 0x0;
                    newpa->a_yspot = 0x0;
                }
                pa = newpa;
            }
            else
                fun_alert(1, STAPGONE, NULL);
        }

        /* see if icon changed or flags changed */
        if (uninstalled || (oicon != nicon) || (oflag != nflag))
        {
            change = TRUE;
#if HAVE_APPL_IBLKS
            pa->a_aicon = nicon + IA_GENERIC;
            pa->a_dicon = nicon + ID_GENERIC;
#else
            pa->a_aicon = IA_GENERIC_ALT;
            pa->a_dicon = ID_GENERIC_ALT;
#endif
            pa->a_flags = nflag;
        }
    }
    else if (field == 1)
    {
        /* remove installed app */
        if (!uninstalled)
        {
            app_free(pa);
            change = TRUE;
        }
    }

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
