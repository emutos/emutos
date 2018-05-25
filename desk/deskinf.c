/*      DESKINF.C       09/03/84 - 05/29/85     Gregg Morris            */
/*      for 3.0 & 2.1   5/5/86                  MDF                     */
/*      merge source    5/27/87  - 5/28/87      mdf                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*       Copyright (C) 2002-2017 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985 - 1987               Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "string.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "dos.h"
#include "gemdos.h"
#include "optimopt.h"
#include "optimize.h"

#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "aesbind.h"
#include "deskmain.h"
#include "deskdir.h"
#include "deskrsrc.h"
#include "desksupp.h"
#include "deskinf.h"
#include "deskins.h"
#include "../bios/country.h"
#include "kprint.h"


/*
 * Border flags for 'BGSAMPLE' object in 'Set background' dialog:
 * outside border, thickness=1, border colour black, text black.
 */
#define SAMPLE_BORDER_FLAGS 0x00ff1100L

/*
 * Routine to format DOS style time
 *
 *  15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
 *  <     hh     > <    mm    > <   xx  >
 *  hh = binary 0-23
 *  mm = binary 0-59
 *  xx = binary seconds / 2
 *
 * Outputs 6 bytes, left aligned, filled with spaces:
 *  1245pm (12-hour clock)
 *  0045   (24-hour clock)
 * formatted as per sprintf string 'fmt_string'
 *
 * Returns pointer to end of formatted string
 */
static BYTE *fmt_time(UWORD time, BYTE *fmt_string, BYTE *ptime)
{
    WORD hh, mm;
    BYTE *suffix = "  ";

    hh = (time >> 11) & 0x001f;
    mm = (time >> 5) & 0x003f;

    switch(G.g_ctimeform)
    {
    case TIMEFORM_IDT:
        if ((cookie_idt&IDT_TMASK) == IDT_24H)
            break;
        /* else 12 hour clock, so drop thru */
    case TIMEFORM_12H:
        if (hh >= 12)
        {
            hh -= 12;
            suffix = gl_pmstr;  /* "pm" */
        }
        else
        {
            suffix = gl_amstr;  /* "am" */
        }
        if (hh == 0)
            hh = 12;
        break;
    }
    sprintf(ptime,fmt_string,hh,mm,suffix);

    return ptime+strlen(ptime);
}


/*
 * Routine to format DOS style date
 *
 *  15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
 *  <     yy          > < mm  > <  dd   >
 *  yy = 0 - 119 (1980 - 2099)
 *  mm = 1 - 12
 *  dd = 1 - 31
 *
 * Outputs 6 bytes:
 *  ddmmyy      (English style)
 *  mmddyy      (US style)
 * where yy is a 2- or 4-digit year, depending on the value of 'fourdigit'
 *
 * Returns pointer to end of formatted string
 */
static BYTE *fmt_date(UWORD date, BOOL fourdigit, BYTE *pdate)
{
    WORD dd, mm, yy;
    WORD var1, var2, var3;
    UBYTE tmp;
    char separator = DATEFORM_SEP;
    WORD format;

    yy = 1980 + ((date >> 9) & 0x007f);
    if (!fourdigit)
        yy %= 100;
    mm = (date >> 5) & 0x000f;
    dd = date & 0x001f;

    switch(G.g_cdateform)
    {
    case DATEFORM_IDT:
        tmp = cookie_idt & IDT_SMASK;   /* separator */
        if (tmp)
            separator = tmp;
        format = cookie_idt&IDT_DMASK;
        break;
    case DATEFORM_DMY:
        format = IDT_DDMMYY;
        break;
    default:
        format = IDT_MMDDYY;
        break;
    }

    switch(format)
    {
    case IDT_MMDDYY:
        var1 = mm;
        var2 = dd;
        var3 = yy;
        break;
    case IDT_YYMMDD:
        var1 = yy;
        var2 = mm;
        var3 = dd;
        break;
    case IDT_YYDDMM:    /* does anyone ever use this? */
        var1 = yy;
        var2 = dd;
        var3 = mm;
        break;
    default:            /* default to DDMMYY */
        var1 = dd;
        var2 = mm;
        var3 = yy;
        break;
    }
    sprintf(pdate,"%02d%c%02d%c%02d",var1,separator,var2,separator,var3);

    return pdate+strlen(pdate);
}


/*
 * Routine to format f_size into an 8- or 11-byte field,
 * depending on the current screen width
 *
 * Note: files larger than 9999999 bytes will be displayed
 * in kbytes on narrow screens
 */
static BYTE *fmt_size(LONG size, BOOL wide, BYTE *psize)
{
    BYTE *fmt_string;

    /*
     * if the screen is wide enough, it's simple
     */
    if (wide)
    {
        sprintf(psize, " %10lu", size);
        return psize+11;
    }

    /*
     * ST low or similar: we may have to scrunch things
     * for big files
     */
    if (size <= 9999999L)       /* small files are ok */
        fmt_string = " %7lu";
    else                        /* big files are a bit ugly */
    {
        size /= 1024;
        fmt_string = "%7luK";
    }
    sprintf(psize, fmt_string, size);

    return psize+8;
}


static WORD format_fnode(LONG pfnode, BYTE *pfmt)
{
    FNODE *pf;
    BYTE *pdst, *psrc;
    WORD i;
    BOOL wide;
    BYTE indicator;

    /*
     * determine if we should use the wide format
     */
    wide = USE_WIDE_FORMAT();

    pf = (FNODE *)pfnode;
    pdst = pfmt;

    /*
     * folder or read-only indicator
     */
    if (pf->f_attr & F_SUBDIR)
        indicator = 0x07;
    else if (pf->f_attr & F_RDONLY)
        indicator = 0x7f;
    else indicator = ' ';
    *pdst++ = indicator;
    if (wide)
        *pdst++ = ' ';

    /*
     * name and extension
     */
    for (i = 0, psrc = pf->f_name; *psrc; i++)
    {
        if (*psrc == '.')
        {
            psrc++;
            break;
        }
        *pdst++ = *psrc++;
    }
    for ( ; i < 9; i++)
        *pdst++ = ' ';
    for (i = 0; *psrc; i++)
        *pdst++ = *psrc++;
    for ( ; i < 3; i++)
        *pdst++ = ' ';

    /*
     * size
     */
    if (pf->f_attr & F_SUBDIR)
    {
        WORD n = wide ? 11 : 8;
        while(n--)
            *pdst++ = ' ';
    }
    else
    {
        pdst = fmt_size(pf->f_size, wide, pdst);
    }

    /*
     * date and time
     *
     * note: we display 4 digits for year on wide screens, or if
     * we're using a 24-hour clock on any screen (no need for
     * am/pm space)
     */
    *pdst++ = ' ';
    if (wide)
        *pdst++ = ' ';
    pdst = fmt_date(pf->f_date, wide, pdst);
    *pdst++ = ' ';
    if (wide)
        *pdst++ = ' ';
    pdst = fmt_time(pf->f_time, wide?"%02d:%02d %s":"%02d:%02d%s", pdst);

    return (pdst-pfmt);
}


static WORD dr_fnode(UWORD last_state, UWORD curr_state, WORD x, WORD y,
            WORD w, WORD h, LONG fnode)
{
    WORD len;
    BYTE temp[LEN_FNODE];

    if ((last_state ^ curr_state) & SELECTED)
        bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, x, y, w, h);
    else
    {
        len = format_fnode(fnode, temp);    /* convert to text */
        gsx_attr(TRUE, MD_TRANS, BLACK);
        expand_string(intin, temp);
        gsx_tblt(IBM, x, y, len);
        gsx_attr(FALSE, MD_XOR, BLACK);
    }

    return curr_state;
}


WORD dr_code(PARMBLK *pparms)
{
    GRECT oc;
    WORD  state;

    gsx_gclip(&oc);
    gsx_sclip((GRECT *)&pparms->pb_xc);
    state = dr_fnode(pparms->pb_prevstate, pparms->pb_currstate,
                    pparms->pb_x, pparms->pb_y, pparms->pb_w, pparms->pb_h, pparms->pb_parm);
    gsx_sclip(&oc);

    return state;
}


/*
 * Start a dialog
 */
void start_dialog(OBJECT *tree)
{
    WORD xd, yd, wd, hd;

    form_center(tree, &xd, &yd, &wd, &hd);
    form_dial(FMD_START, 0, 0, 0, 0, xd, yd, wd, hd);
    objc_draw(tree, ROOT, MAX_DEPTH, xd, yd, wd, hd);
}


/*
 * End a dialog
 */
void end_dialog(OBJECT *tree)
{
    WORD xd, yd, wd, hd, event;
    UWORD junk;

    form_center(tree, &xd, &yd, &wd, &hd);
    form_dial(FMD_FINISH, 0, 0, 0, 0, xd, yd, wd, hd);

    /*
     * now handle any messages (expected to be redraws) triggered by
     * the form_dial(FMD_FINISH) above.  because the desktop handles
     * internally-generated messages directly (rather than calling
     * the AES), AES-generated messages will stay pending until the
     * next evnt_multi() call.  if we do not call evnt_multi() here,
     * the redraws will be out of sequence, causing irritating
     * partial-refresh effects in windows.
     */
    while(1)
    {
        event = evnt_multi(MU_TIMER|MU_MESAG, 0x02, 0x01, 0x01,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                G.g_rmsg, 0, 0,
                                &junk, &junk, &junk, &junk, &junk, &junk);
        if (!(event & MU_MESAG))    /* no (more) messages */
            break;
        hndl_msg();
    }
}


/*
 * Put up dialog box & call form_do
 */
WORD inf_show(OBJECT *tree, WORD start)
{
    start_dialog(tree);
    form_do(tree, start);
    end_dialog(tree);

    return TRUE;
}


/*
 *  Insert an unsigned long value into the te_ptext field of the TEDINFO
 *  structure for the specified object, truncating if necessary
 */
void inf_numset(OBJECT *tree, WORD obj, ULONG value)
{
    WORD len;
    TEDINFO *ted;
    OBJECT  *objptr = tree + obj;

    ted = (TEDINFO *)objptr->ob_spec;
    len = ted->te_txtlen - 1;

    sprintf(ted->te_ptext,"%*.*lu",len,len,value);
}


/*
 * Routine to put number of files, folders and size into
 * a dialog box
 */
static void inf_fifosz(OBJECT *tree, WORD dl_fi, WORD dl_fo, WORD dl_sz)
{
    OBJECT *obj;

    G.g_ndirs--;    /* reproduce TOS's folder count in this situation */

    obj = tree + dl_fi;
    obj->ob_state &= ~DISABLED;
    inf_numset(tree, dl_fi, G.g_nfiles);

    obj = tree + dl_fo;
    obj->ob_state &= ~DISABLED;
    inf_numset(tree, dl_fo, G.g_ndirs);

    obj = tree + dl_sz;
    obj->ob_state &= ~DISABLED;
    inf_numset(tree, dl_sz, G.g_size);
}


static void inf_dttm(OBJECT *tree, FNODE *pf, WORD dl_dt, WORD dl_tm)
{
    BYTE str[11];

    fmt_date(pf->f_date, 1, str);   /* 4-digit year always */
    inf_sset(tree, dl_dt, str);

    fmt_time(pf->f_time, "%02d%02d%s", str);
    inf_sset(tree, dl_tm, str);
}


/*
 * Count files, folders, and total filesize in a given path
 * Values are stored in G.g_nfiles & friends
 */
static WORD count_ffs(BYTE *path)
{
    G.g_nfiles = G.g_ndirs = G.g_size = 0L;

    return d_doop(0, OP_COUNT, path, path, 0L, NULL);
}


/************************************************************************/
/* i n f _ f i l e _ f o l d e r                                        */
/************************************************************************/
/*
 * returns:
 *      0   cancel
 *      1   continue
 *      -1  continue, and mark window for redraw
 */
WORD inf_file_folder(BYTE *ppath, FNODE *pf)
{
    OBJECT *tree;
    WORD nmidx, title, ret;
    BOOL more, changed;
    BYTE attr;
    BYTE srcpth[MAXPATHLEN];
    BYTE dstpth[MAXPATHLEN];
    BYTE poname[LEN_ZFNAME], pnname[LEN_ZFNAME];
    OBJECT *obj;

    tree = G.a_trees[ADFFINFO];
    deselect_all(tree);

    title = (pf->f_attr & F_SUBDIR) ? STFOINFO : STFIINFO;
    obj = tree + FFTITLE;
    obj->ob_spec = (LONG) ini_str(title);
    centre_title(tree);

    strcpy(srcpth, ppath);
    strcpy(dstpth, ppath);
    nmidx = filename_start(srcpth) - srcpth;

    /*
     * for folders, count the contents & insert the values in the
     * dialog; for files, blank out the corresponding dialog fields
     */
    if (pf->f_attr & F_SUBDIR)
    {
        graf_mouse(HGLASS, NULL);
        strcpy(srcpth+nmidx, pf->f_name);
        strcat(srcpth, "\\*.*");
        more = count_ffs(srcpth);
        graf_mouse(ARROW, NULL);

        if (!more)
            return 1;

        inf_fifosz(tree, FFNUMFIL, FFNUMFOL, FFSIZE);
    }
    else
    {
        obj = tree + FFNUMFIL;
        obj->ob_state |= DISABLED;
        inf_sset(tree,FFNUMFIL,"");
        obj = tree + FFNUMFOL;
        obj->ob_state |= DISABLED;
        inf_sset(tree,FFNUMFOL,"");
        inf_numset(tree,FFSIZE,pf->f_size);
    }

    fmt_str(pf->f_name, poname);
    inf_sset(tree, FFNAME, poname);
    inf_dttm(tree, pf, FFDATE, FFTIME);

    /*
     * initialise the attributes display
     */
    obj = tree + FFRONLY;
    if (pf->f_attr & F_SUBDIR)
        obj->ob_state = DISABLED;
    else if (pf->f_attr & F_RDONLY)
        obj->ob_state = SELECTED;
    else
        obj->ob_state = NORMAL;

    obj = tree + FFRWRITE;
    if (pf->f_attr & F_SUBDIR)
        obj->ob_state = DISABLED;
    else if (!(pf->f_attr & F_RDONLY))
        obj->ob_state = SELECTED;
    else
        obj->ob_state = NORMAL;

    inf_show(tree, ROOT);
    ret = inf_what(tree, FFSKIP, FFCNCL);
    if (ret >= 0)       /* skip or cancel */
        return ret;

    /*
     * user selected OK - we rename and/or change attributes
     */
    graf_mouse(HGLASS, NULL);

    inf_sget(tree, FFNAME, pnname);

    /* unformat the strings */
    unfmt_str(poname, srcpth+nmidx);
    unfmt_str(pnname, dstpth+nmidx);

    /*
     * if user has changed the name, do the DOS rename
     */
    more = TRUE;
    changed = FALSE;
    if (strcmp(srcpth+nmidx, dstpth+nmidx))
    {
        ret = dos_rename(srcpth, dstpth);
        if ((more=d_errmsg(ret)) != 0)
        {
            strcpy(pf->f_name, dstpth+nmidx);
            changed = TRUE;
        }
    }

    /*
     * if user has changed the attributes, and we haven't had
     * an error from the rename, tell DOS to change them
     */
    if (!(pf->f_attr & F_SUBDIR) && more)
    {
        attr = pf->f_attr;
        obj = tree + FFRONLY;
        if (obj->ob_state & SELECTED)
            attr |= F_RDONLY;
        else
            attr &= ~F_RDONLY;
        if (attr != pf->f_attr)
        {
            ret = dos_chmod(dstpth, F_SETMOD, attr);
            if (d_errmsg(ret) != 0)
            {
                pf->f_attr = attr;
                changed = TRUE;
            }
        }
    }

    graf_mouse(ARROW, NULL);

    return changed ? -1 : 1;
}


/************************************************************************/
/* i n f _ d i s k                                                      */
/************************************************************************/
/*
 * returns:
 *      0   cancel
 *      1   continue
 */
WORD inf_disk(BYTE dr_id)
{
    OBJECT *tree;
    LONG total, avail;
    WORD more;
    BYTE srcpth[MAXPATHLEN];
    BYTE label[LEN_ZFNAME];
    BYTE drive[2];

    graf_mouse(HGLASS, NULL);
    tree = G.a_trees[ADDISKIN];

    drive[0] = dr_id;
    drive[1] = '\0';
    srcpth[0] = dr_id;
    srcpth[1] = ':';
    strcpy(srcpth+2, "\\*.*");
    more = count_ffs(srcpth);

    if (!more)
    {
        graf_mouse(ARROW, NULL);
        return 1;
    }

    dos_space(dr_id - 'A' + 1, &total, &avail);
    graf_mouse(ARROW, NULL);

    if (!dos_label(dr_id - 'A' + 1, label))
        label[0] = '\0';

    inf_sset(tree, DIDRIVE, drive);
    fmt_str(label, srcpth);
    inf_sset(tree, DIVOLUME, srcpth);

    inf_fifosz(tree, DINFILES, DINFOLDS, DIUSED);
    inf_numset(tree, DIAVAIL, avail);

    inf_show(tree, ROOT);
    return inf_what(tree, DIOK, DICNCL);
}


/*
 *  Examines 'numobj' objects in 'tree', starting at 'baseobj', looking
 *  for a SELECTED object.  Returns the number of the first such object,
 *  relative to the end.  For example, if there are four objects, the
 *  value returned is 3, 2, 1, or 0 for the first, second, third, or
 *  fourth object respectively.  If none of the objects is selected,
 *  -1 is returned.
 *  This is more-or-less the reverse of inf_gindex()
 */
static WORD inf_which(OBJECT *tree, WORD baseobj, WORD numobj)
{
    WORD   i;
    OBJECT *obj;

    for (i = numobj-1, obj = tree+baseobj; i >= 0; i--, obj++)
        if (obj->ob_state & SELECTED)
            break;

    return i;
}


/*
 * Handle preferences dialogs
 */
WORD inf_pref(void)
{
    OBJECT *tree1 = G.a_trees[ADSETPRE];
    OBJECT *tree2 = G.a_trees[ADSETPR2];
    WORD oldtime, olddate;
    WORD sndefpref;
    WORD button;

    /*
     * handle dialog 1
     */

    /* first, deselect all objects */
    deselect_all(tree1);
    deselect_all(tree2);

    /* select buttons corresponding to current state */
    if (G.g_cdelepref)
        tree1[SPCDYES].ob_state |= SELECTED;
    else
        tree1[SPCDNO].ob_state |= SELECTED;

    if (G.g_ccopypref)
        tree1[SPCCYES].ob_state |= SELECTED;
    else
        tree1[SPCCNO].ob_state |= SELECTED;

    if (G.g_covwrpref)
        tree1[SPCOWYES].ob_state |= SELECTED;
    else
        tree1[SPCOWNO].ob_state |= SELECTED;

    sndefpref = !disable_sound(0xFFFF);
    if (sndefpref)
        tree1[SPSEYES].ob_state |= SELECTED;
    else
        tree1[SPSENO].ob_state |= SELECTED;

    /* select buttons corresponding to current state of more preferences */
    G.g_cdclkpref = evnt_dclick(0, FALSE);
    tree2[SPDC1+G.g_cdclkpref].ob_state |= SELECTED;

    if (G.g_cmclkpref)
        tree2[SPMNCLKY].ob_state |= SELECTED;
    else
        tree2[SPMNCLKN].ob_state |= SELECTED;

    switch(G.g_ctimeform)
    {
    case TIMEFORM_12H:
        tree2[SPTF12HR].ob_state |= SELECTED;
        break;
    case TIMEFORM_24H:
        tree2[SPTF24HR].ob_state |= SELECTED;
        break;
    default:
        tree2[SPTF_DEF].ob_state |= SELECTED;
        break;
    }

    switch(G.g_cdateform)
    {
    case DATEFORM_MDY:
        tree2[SPDFMMDD].ob_state |= SELECTED;
        break;
    case DATEFORM_DMY:
        tree2[SPDFDDMM].ob_state |= SELECTED;
        break;
    default:
        tree2[SPDF_DEF].ob_state |= SELECTED;
        break;
    }

    /* allow user to select preferences */
    inf_show(tree1, ROOT);
    button = inf_what(tree1,SPOK,SPCNCL);

    /*
     * handle dialog 2 if necessary
     */
    if (button < 0)         /* user selected More */
    {
        /* allow user to select preferences */
        inf_show(tree2, ROOT);
        button = inf_what(tree2, SPOK2, SPCNCL2);
    }

    if (button)
    {
        G.g_cdelepref = inf_which(tree1, SPCDYES, 2);
        G.g_ccopypref = inf_which(tree1, SPCCYES, 2);
        G.g_covwrpref = inf_which(tree1, SPCOWYES, 2);
        sndefpref = inf_which(tree1, SPSEYES, 2);
        disable_sound(!sndefpref);

        G.g_cdclkpref = inf_gindex(tree2, SPDC1, 5);
        G.g_cdclkpref = evnt_dclick(G.g_cdclkpref, TRUE);
        G.g_cmclkpref = inf_which(tree2, SPMNCLKY, 2);
        G.g_cmclkpref = menu_click(G.g_cmclkpref, TRUE);

        /*
         * the following assumes that the buttons are in the
         * left-to-right order: default, 12hour, 24hour
         */
        oldtime = G.g_ctimeform;
        G.g_ctimeform = inf_which(tree2, SPTF_DEF, 3);

        /*
         * the following assumes that the buttons are in the
         * left-to-right order: default, mmddyy, ddmmyy
         */
        olddate = G.g_cdateform;
        G.g_cdateform = inf_which(tree2, SPDF_DEF, 3);

        /*
         * if the current view is as text, and the date or time
         * format has changed, we need to tell the caller so he
         * can rebuild the windows if necessary
         */
        if (G.g_iview == V_TEXT)
            if ((G.g_ctimeform != oldtime) || (G.g_cdateform != olddate))
                return TRUE;
    }

    return FALSE;
}


#if CONF_WITH_BACKGROUNDS
/*
 *      Handle background pattern/colour configuration dialog
 */
BOOL inf_backgrounds(void)
{
    OBJECT *tree, *obj;
    WORD ret;
    LONG curdesk, curwin;
    WORD i, index, unused;

    /* set pattern/colour index and first unused colour */
    switch(gl_nplanes)
    {
    case 1:
        index = 0;
        unused = 2;
        break;
    case 2:
        index = 1;
        unused = 4;
        break;
    default:
        index = 2;
        unused = 16;
        break;
    }

    tree = G.a_trees[ADBKGND];

    /* hide colours that are not available in the current resolution */
    for (i = unused, obj = tree+BGCOL0+unused; i < 16; i++, obj++)
        obj->ob_flags |= HIDETREE;

    /* set the initially-displayed background pattern */
    curdesk = G.g_screen[DROOT].ob_spec;
    curwin = G.g_screen[DROOT+1].ob_spec;
    tree[BGSAMPLE].ob_spec = ((tree[BGDESK].ob_state & SELECTED) ? curdesk : curwin)
                            | SAMPLE_BORDER_FLAGS;

    /* handle the dialog */
    start_dialog(tree);

    while(1)
    {
        draw_fld(tree, BGSAMPLE);
        ret = form_do(tree, ROOT);

        if ((ret == BGOK) || (ret == BGCANCEL))
        {
            tree[ret].ob_state &= ~SELECTED;
            break;
        }

        if (ret == BGDESK)
            tree[BGSAMPLE].ob_spec = curdesk;
        else if (ret == BGWIN)
            tree[BGSAMPLE].ob_spec = curwin;
        else if ((ret >= BGPAT0) && (ret <= BGPAT7))
        {
            tree[BGSAMPLE].ob_spec &= ~FILLPAT_MASK;
            tree[BGSAMPLE].ob_spec |= tree[ret].ob_spec & FILLPAT_MASK;
        }
        else if ((ret >= BGCOL0) && (ret <= BGCOL15))
        {
            tree[BGSAMPLE].ob_spec &= ~FILLCOL_MASK;
            tree[BGSAMPLE].ob_spec |= tree[ret].ob_spec & FILLCOL_MASK;
        }

        if (tree[BGDESK].ob_state & SELECTED)
            curdesk = tree[BGSAMPLE].ob_spec;
        else
            curwin = tree[BGSAMPLE].ob_spec;
    }

    end_dialog(tree);

    /* handle dialog exit */
    if (ret == BGOK)
    {
        /* check for desktop background change */
        if (G.g_screen[DROOT].ob_spec != curdesk)
        {
            G.g_patcol[index].desktop = curdesk & 0xff;
            G.g_screen[DROOT].ob_spec = curdesk;
            do_wredraw(0, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk);
        }

        /* check for window background change */
        if (G.g_screen[DROOT+1].ob_spec != curwin)
        {
            G.g_patcol[index].window = curwin & 0xff;
            for (i = DROOT+1, tree = G.g_screen+i; i < WOBS_START; i++, tree++)
                tree->ob_spec = curwin;
            return TRUE;
        }
    }

    return FALSE;
}
#endif


#if CONF_WITH_DESKTOP_CONFIG
/*
 *      Handle desktop configuration dialog
 */
void inf_conf(void)
{
    OBJECT *tree = G.a_trees[ADDESKCF];
    WORD button;

    /* first, deselect all objects */
    deselect_all(tree);

    /* select buttons corresponding to current state */
    if (G.g_appdir)
        tree[DCDEFAPP].ob_state |= SELECTED;
    else
        tree[DCDEFWIN].ob_state |= SELECTED;

    if (G.g_fullpath)
        tree[DCPMFULL].ob_state |= SELECTED;
    else
        tree[DCPMFILE].ob_state |= SELECTED;

    /* allow user to select preferences */
    inf_show(tree, ROOT);
    button = inf_what(tree, DCOK, DC_CNCL);

    if (button)
    {
        G.g_appdir = inf_which(tree, DCDEFAPP, 2);
        G.g_fullpath = inf_which(tree, DCPMFULL, 2);
    }
}
#endif


/*
 *       Open application icon
 */
WORD opn_appl(BYTE *papname, BYTE *ptail)
{
    OBJECT *tree;
    BYTE poname[LEN_ZFNAME];

    tree = G.a_trees[ADOPENAP];

    fmt_str(papname, poname);
    inf_sset(tree, APPLNAME, poname);
    inf_sset(tree, APPLPARM, "");
    inf_show(tree, APPLPARM);

    /* now find out what happened */
    if ( inf_what(tree, APPLOK, APPLCNCL) )
    {
        inf_sget(tree, APPLPARM, ptail);
        return TRUE;
    }

    return FALSE;
}
