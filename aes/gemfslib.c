/*
 * gemfslib.c - the file selector
 *
 * Copyright 1999, Caldera Thin Clients, Inc.
 *           2002-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "struct.h"
#include "obdefs.h"
#include "tosvars.h"
#include "aesdefs.h"
#include "aesext.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemdos.h"
#include "gemoblib.h"
#include "gemfmlib.h"
#include "gemgsxif.h"
#include "gemgrlib.h"
#include "geminit.h"
#include "gemfslib.h"
#include "gemrslib.h"
#include "optimize.h"
#include "rectfunc.h"
#include "gemerror.h"
#include "gemobed.h"
#include "string.h"
#include "intmath.h"
#include "asm.h"
#include "miscutil.h"

#define NM_NAMES (F9NAME-F1NAME+1)
#define NAME_OFFSET F1NAME
#define NM_DRIVES (FSLSTDRV-FS1STDRV+1)
#define DRIVE_OFFSET FS1STDRV
#define DRIVE_ROWS   9              /* (3 columns of buttons for the standard 26 drives) */

#define LEN_FSNAME (LEN_ZFNAME+1)   /* includes leading flag byte & trailing nul */
#define LEN_FSPATH  (LEN_ZPATH+4)   /* at least 3 bytes longer than max path */
#define LEN_FSWORK  (3*LEN_FSPATH)  /* total workarea length in fs_input() */

static GRECT gl_rfs;

static const char gl_fsobj[4] = {FTITLE, FILEBOX, SCRLBAR, 0x0};

static char *ad_fsnames;    /* holds filenames in currently-displayed directory */
static LONG *g_fslist;      /* offsets of filenames within ad_fsnames */
static LONG nm_files;       /* total number of slots in g_fslist[] */


/*
 *  initialise the file selector
 */
void fs_start(void)
{
#if CONF_WITH_3D_OBJECTS
    OBJECT *obj, *tree = rs_trees[FSELECTR];
    TEDINFO *ted;
    WORD i, row;

    /*
     * adjust the positions and/or dimensions of all the objects within
     * FILEAREA (the IBOX that contains the closer, title, scroll bar,
     * and filenames), plus FILEAREA itself
     *
     * at the same time, we set any 3D object bits required
     */
    tree[FCLSBOX].ob_flags |= FL3DACT;
    tree[FCLSBOX].ob_x += ADJ3DSTD;
    tree[FCLSBOX].ob_y += ADJ3DSTD;

    tree[FTITLE].ob_flags |= FL3DBAK;
    tree[FTITLE].ob_x += 2*ADJ3DSTD-1;
    tree[FTITLE].ob_height += 2*ADJ3DSTD;
    /* use pattern 4, just like TOS4 */
    ted = (TEDINFO *)tree[FTITLE].ob_spec;
    ted->te_color = (ted->te_color & 0xff8f) | (IP_4PATT<<4);

    tree[FILEBOX].ob_y += 3*ADJ3DSTD-1;
    tree[FILEBOX].ob_width += ADJ3DSTD;

    tree[SCRLBAR].ob_x += 2*ADJ3DSTD-1;
    tree[SCRLBAR].ob_y += 3*ADJ3DSTD-1;

    tree[FUPAROW].ob_flags |= FL3DACT;
    tree[FUPAROW].ob_x += ADJ3DSTD;
    tree[FUPAROW].ob_y += ADJ3DSTD;
    tree[FUPAROW].ob_width -= 2*ADJ3DSTD;

    tree[FSVSLID].ob_y += 3*ADJ3DSTD;
    tree[FSVSLID].ob_height -= 6*ADJ3DSTD;

    /* use pattern 4, just like TOS4 */
    tree[FSVSLID].ob_spec = (tree[FSVSLID].ob_spec & 0xffffff8fL) | (IP_4PATT<<4);

    /* we only adjust x/w of FSVELEV here, because y/h are set dynamically */
    tree[FSVELEV].ob_flags |= FL3DACT;
    tree[FSVELEV].ob_x += ADJ3DSTD;
    tree[FSVELEV].ob_width -= 2*ADJ3DSTD;

    tree[FDNAROW].ob_flags |= FL3DACT;
    tree[FDNAROW].ob_x += ADJ3DSTD;
    tree[FDNAROW].ob_y -= ADJ3DSTD;
    tree[FDNAROW].ob_width -= 2*ADJ3DSTD;

    tree[FILEAREA].ob_y -= 6;       /* squeeze things together vertically */
    tree[FILEAREA].ob_width += 2*ADJ3DSTD - 1;
    tree[FILEAREA].ob_height += 3*ADJ3DSTD - 1;

    /*
     * adjust the position of all the drive-letter boxes, plus FSDRIVES
     * (the IBOX that includes them)
     *
     * at the same time we mark them as 3D indicators
     */
    for (i = 0, obj = tree+DRIVE_OFFSET, row = 0; i < NM_DRIVES; i++, obj++, row++)
    {
        if (row >= DRIVE_ROWS)
            row = 0;
        obj->ob_flags |= FL3DIND;
        obj->ob_x += 2*ADJ3DSTD;
        obj->ob_y += 2*ADJ3DSTD + row*(3*ADJ3DSTD-1);
    }

    tree[FSDRIVES].ob_height += 2*ADJ3DSTD + DRIVE_ROWS*(3*ADJ3DSTD-1);
    tree[FSDRIVES].ob_width += 4*ADJ3DSTD;

    /*
     * finally, handle the remaining objects that have ROOT as a parent, plus ROOT
     *
     * FSOK/FSCANCEL must move left to avoid interfering with FSDRIVES in lower resolutions
     */
    tree[FSDIRECT].ob_flags |= FL3DBAK;
    tree[FSSELECT].ob_flags |= FL3DBAK;
    tree[FSDRVTXT].ob_flags |= FL3DBAK;

    tree[FSOK].ob_flags |= FL3DACT;
    tree[FSOK].ob_x = tree[FILEAREA].ob_x;
    tree[FSOK].ob_y += ADJ3DSTD;

    tree[FSCANCEL].ob_flags |= FL3DACT;
    tree[FSCANCEL].ob_x = tree[FILEAREA].ob_x + tree[FILEAREA].ob_width - tree[FSCANCEL].ob_width;
    tree[FSCANCEL].ob_y = tree[FSOK].ob_y;

    tree[ROOT].ob_flags |= FL3DBAK;
    tree[ROOT].ob_height += 2*ADJ3DSTD;

    ob_center(tree, &gl_rfs);
#else
    OBJECT *tree = rs_trees[FSELECTR];
    WORD diff;

    ob_center(tree, &gl_rfs);

    /*
     * for cosmetic reasons, we make the vertical slider width equal to
     * the standard box width in the current resolution.  since the FTITLE
     * object overhangs the vertical slider, we must adjust its width too.
     */
    diff = tree[SCRLBAR].ob_width - gl_wbox;
    tree[FTITLE].ob_width -= diff;
    tree[SCRLBAR].ob_width = gl_wbox;
    tree[FUPAROW].ob_width = gl_wbox;
    tree[FDNAROW].ob_width = gl_wbox;
    tree[FSVSLID].ob_width = gl_wbox;
    tree[FSVELEV].ob_width = gl_wbox;
#endif
}


/*
 *  centre specified G_STRING in the root object
 */
static void centre_title(OBJECT *tree,WORD objnum)
{
    OBJECT *root = tree;
    OBJECT *str = root+objnum;

    str->ob_x = (root->ob_width - strlen((char *)str->ob_spec)*gl_wchar) / 2;
}


/*
 *  Routine to back off the end of a path string, stopping at either
 *  the first path separator or at the colon preceding the drive specifier.
 *  The second argument specifies the end of the string; if NULL, the
 *  end is determined via strlen().  If the scan is stopped by a colon,
 *  the routine inserts a path separator in the string immediately following
 *  the colon.
 *
 *  Returns a pointer to the beginning of the string (if no colon or
 *  backslash found), or to the last path separator.
 */
static char *fs_back(char *pstr, char *pend)
{
    char *p;

    if (!pend)
        pend = pstr + strlen(pstr);

    /* back off to last path separator (or colon) */
    for (p = pend; p != pstr; p--)
    {
        if (*p == PATHSEP)
            break;
        if (*p == DRIVESEP)
        {
            if (p == pstr+1)    /* X: at the start of the string */
            {
                ins_char(++p,0,PATHSEP,LEN_ZPATH-3);
                break;
            }
        }
    }

    return p;
}


/*
 *  Routine to back up a path and return the pointer to the beginning
 *  of the file specification part
 */
static char *fs_pspec(char *pstr, char *pend)
{
    pend = fs_back(pstr, pend);
    if (*pend == PATHSEP)
        pend++;

    return pend;
}


/*
 *  Routine to compare files based on name
 *  Note: folders always sort lowest because the first character is \007
 */
static WORD fs_comp(char *name1, char *name2)
{
    return strcmp(name1, name2);
}


static LONG fs_add(WORD thefile, LONG fs_index)
{
    WORD len;

    g_fslist[thefile] = fs_index;
    ad_fsnames[fs_index++] = (D.g_dta.d_attrib & FA_SUBDIR) ? 0x07 : ' ';
    len = strlencpy(ad_fsnames+fs_index,D.g_dta.d_fname);
    fs_index += len + 1;
    return fs_index;
}


/*
 *  Make a particular path the active path.  This involves
 *  reading its directory, initializing a file list, and filling
 *  out the information in the path node.  Then sort the files.
 *
 *  Returns FALSE iff error occurred
 */
static WORD fs_active(char *ppath, char *pspec, WORD *pcount)
{
    WORD ret;
    LONG thefile, fs_index, temp;
    WORD i, j, gap;
    char *fname, allpath[LEN_ZPATH+1];
    DTA *user_dta;

    set_mouse_to_hourglass();

    thefile = 0L;
    fs_index = 0L;

    strcpy(allpath, ppath);         /* 'allpath' gets all files */
    fname = fs_pspec(allpath,NULL);
    set_all_files(fname);

    user_dta = dos_gdta();          /* remember user's DTA */
    dos_sdta(&D.g_dta);
    ret = dos_sfirst(allpath, FA_SUBDIR);

    /*
     * like Atari TOS, we silently ignore any filenames that we don't
     * have room for.  this should be an extremely rare occurrence.
     */
    while((ret == 0) && (thefile < nm_files))
    {
        /* if it is a real file or directory then save it and set
         * the first byte to tell which
         */
        if (D.g_dta.d_fname[0] != '.')
        {
            if ((D.g_dta.d_attrib & FA_SUBDIR) || (wildcmp(pspec, D.g_dta.d_fname)))
            {
                fs_index = fs_add(thefile, fs_index);
                thefile++;
            }
        }
        ret = dos_snext();
    }

    *pcount = thefile;
    dos_sdta(user_dta);             /* restore user DTA */

    /* sort files using shell sort from page 108 of K&R C Prog. Lang. */
    for (gap = thefile/2; gap > 0; gap /= 2)
    {
        for (i = gap; i < thefile; i++)
        {
            for (j = i-gap; j >= 0; j -= gap)
            {
                if (fs_comp(ad_fsnames+g_fslist[j],ad_fsnames+g_fslist[j+gap]) <= 0)
                    break;
                temp = g_fslist[j];
                g_fslist[j] = g_fslist[j+gap];
                g_fslist[j+gap] = temp;
            }
        }
    }

    set_mouse_to_arrow();

    if ((ret == EFILNF) || (ret == ENMFIL))
        return TRUE;

    if (!IS_BIOS_ERROR(ret))    /* if BDOS error, issue message via form_error(): */
        fm_error(-ret-31);      /* (need to convert to 'MS-DOS error code')       */

    return FALSE;
}


/*
 *  Routine to adjust the scroll counters by one in either
 *  direction, being careful not to overrun or underrun the
 *  tail and head of the list
 */
static WORD fs_1scroll(WORD curr, WORD count, WORD touchob)
{
    WORD newcurr;

    newcurr = (touchob == FUPAROW) ? (curr - 1) : (curr + 1);
    if (newcurr < 0)
        newcurr++;
    if ((count - newcurr) < NM_NAMES)
        newcurr--;
    return (count > NM_NAMES) ? newcurr : curr;
}


/*
 *  Routine to take the filenames that will appear in the window,
 *  based on the current scrolled position, and point at them
 *  with the sub-tree of G_STRINGs that makes up the window box.
 */
static void fs_format(OBJECT *tree, WORD currtop, WORD count)
{
    WORD i, cnt;
    WORD y, h, th;
    char   *p, name[LEN_FSNAME];
    OBJECT *obj, *treeptr = tree;

    /* build in real text strings */
    cnt = min(NM_NAMES, count - currtop);
    for (i = 0, obj = treeptr+NAME_OFFSET; i < NM_NAMES; i++, obj++)
    {
        if (i < cnt)
        {
            p = ad_fsnames + g_fslist[currtop+i];
            fmt_str(p+1, name+1);       /* format file/folder name */
            name[0] = p[0];             /* copy file/folder indicator */
        }
        else
        {
            name[0] = ' ';
            name[1] = '\0';
        }
        inf_sset(treeptr, NAME_OFFSET+i, name);
        obj->ob_type = G_FBOXTEXT;
        obj->ob_state = NORMAL;
    }

    /* size and position the elevator */
    y = 0;
    obj = treeptr + FSVSLID;
    th = h = obj->ob_height;
    if (count > NM_NAMES)
    {
        h = mul_div_round(NM_NAMES, h, count);
        h = max(gl_hbox, h);            /* min size elevator */
        y = mul_div_round(currtop, th-h, count-NM_NAMES);
    }
#if CONF_WITH_3D_OBJECTS
    y += ADJ3DSTD;
    h -= 2 * ADJ3DSTD;      /* we assume that 2*ADJ3DSTD < gl_hbox */
#endif
    obj = treeptr + FSVELEV;
    obj->ob_y = y;
    obj->ob_height = h;
}


/*
 *  Routine to select or deselect a file name in the scrollable list.
 */
static void fs_sel(WORD sel, WORD state)
{
    if (sel)
        ob_change(rs_trees[FSELECTR], F1NAME + sel - 1, state, TRUE);
}


/*
 *  Routine to handle scrolling the directory window a certain number
 *  of file names.
 */
static WORD fs_nscroll(OBJECT *tree, WORD *psel, WORD curr, WORD count,
                       WORD touchob, WORD n)
{
    WORD i, newcurr, diffcurr;
    WORD sy, dy, neg;
    GRECT r[2];

    /* single scroll n times */
    newcurr = curr;
    for (i = 0; i < n; i++)
        newcurr = fs_1scroll(newcurr, count, touchob);

    /* if things changed then redraw */
    diffcurr = newcurr - curr;
    if (diffcurr)
    {
        curr = newcurr;
        fs_sel(*psel, NORMAL);
        *psel = 0;
        fs_format(tree, curr, count);
        gsx_gclip(&r[1]);
        ob_actxywh(tree, F1NAME, r);

        if ((neg = (diffcurr < 0)) != 0)
            diffcurr = -diffcurr;

        if (diffcurr < NM_NAMES)
        {
            sy = r[0].g_y + (r[0].g_h * diffcurr);
            dy = r[0].g_y;

            if (neg)
            {
                dy = sy;
                sy = r[0].g_y;
            }

            bb_screen(r[0].g_x, sy, r[0].g_x, dy, r[0].g_w, r[0].g_h * (NM_NAMES - diffcurr) );
            if (!neg)
                r[0].g_y += r[0].g_h * (NM_NAMES - diffcurr);
        }
        else
            diffcurr = NM_NAMES;

        r[0].g_h *= diffcurr;

        for (i = 0; i < 2; i++)
        {
            gsx_sclip(&r[i]);
            ob_draw(tree, ((i) ? FSVSLID : FILEBOX), MAX_DEPTH);
        }
    }

    return curr;
}


/*
 *  Routine to call when a new directory has been specified.  This
 *  will activate the directory, format it, and display it.
 *
 *  Returns FALSE iff error occurred
 */
static WORD fs_newdir(char *fpath, char *pspec, OBJECT *tree, WORD *pcount)
{
    const char *ptmp;
    OBJECT *obj;
    TEDINFO *tedinfo;

    /* load the filenames matching pspec, sort them, and insert
     * the names in the file selector scroll box
     */
    ob_draw(tree, FSDIRECT, MAX_DEPTH);
    if (!fs_active(fpath, pspec, pcount))
        return FALSE;           /* e.g. path does not exist */

    fs_format(tree, 0, *pcount);

    obj = tree + FTITLE;        /* update FTITLE with ptr to mask */
    tedinfo = (TEDINFO *)obj->ob_spec;
    tedinfo->te_ptext = pspec;

    ptmp = gl_fsobj;            /* redraw file selector objects */
    while(*ptmp)
        ob_draw(tree, *ptmp++, MAX_DEPTH);

    return TRUE;
}


/*
 *  sets wildcard mask from string
 *  if no mask, uses default *.* & adds it to string
 *
 *  note: although the mask ought to be no longer than LEN_ZFNAME bytes,
 *  the user can enter a string of arbitrary length in the file selector
 *  dialog.  we must preserve this for display and comparison purposes
 *  (so path_changed() can detect a change properly).  therefore we allow
 *  up to LEN_ZPATH bytes.
 */
static void set_mask(char *mask, char *path)
{
    char *pend;

    pend = fs_pspec(path, NULL);
    if (!*pend)                 /* if there's no mask, add one */
        set_all_files(pend);
    strlcpy(mask, pend, LEN_ZPATH);
}


/*
 *  Marks object corresponding to specified drive as selected,
 *  and all others as deselected.  Optionally, if the selected
 *  drive has changed, the affected drive buttons are redrawn.
 */
static void select_drive(OBJECT *treeaddr, WORD drive, WORD redraw)
{
    WORD i, olddrive = -1;
    OBJECT *obj, *start = treeaddr+DRIVE_OFFSET;

    if ((drive < 0) || (drive >= NM_DRIVES))    /* invalid, don't change selected */
        return;

    for (i = 0, obj = start; i < NM_DRIVES; i++, obj++)
    {
        if (obj->ob_state & SELECTED)
        {
            obj->ob_state &= ~SELECTED;
            olddrive = i;
        }
    }
    (start+drive)->ob_state |= SELECTED;

    if (redraw && (drive != olddrive))
    {
        if (olddrive >= 0)
            ob_draw(treeaddr,olddrive+DRIVE_OFFSET,MAX_DEPTH);
        ob_draw(treeaddr,drive+DRIVE_OFFSET,MAX_DEPTH);
    }
}


/*
 *  Compares specified path to FSDIRECT TEDINFO text and
 *  returns 1 iff it is different in the first n characters,
 *  where n is the maximum text length from the TEDINFO.
 */
static WORD path_changed(char *path)
{
    OBJECT *obj;
    TEDINFO *ted;

    obj = rs_trees[FSELECTR] + FSDIRECT;
    ted = (TEDINFO *)obj->ob_spec;

    if (strncmp(path,ted->te_ptext,ted->te_txtlen-1))
        return 1;

    return 0;
}


/*
 *  get drive number from specified path (if possible) or default
 */
static WORD get_drive(char *path)
{
    WORD drive;

    drive = extract_drive_number(path);
    if (drive >= 0)
        return drive;

    return dos_gdrv();
}


/*
 *  File Selector input routine that takes control of the mouse
 *  and keyboard, searches and sorts the directory, draws the file
 *  selector, interacts with the user to determine a selection
 *  or change of path, and returns to the application with
 *  the selected path, filename, and exit button.
 */
WORD fs_input(char *pipath, char *pisel, WORD *pbutton, char *pilabel)
{
    BOOL cont, newlist, newsel, newdrive;
    WORD drive, dclkret, error;
    WORD touchob, value, fnum;
    WORD curr, count, sel;
    WORD mx, my;
    OBJECT *tree;
    ULONG bitmask;
    char *ad_fpath, *ad_fname;
    char *pstr;
    GRECT pt;
    char *memblk, *locstr, *locold, *mask;
    char selname[LEN_FSNAME];
    OBJECT *obj;
    TEDINFO *tedinfo;

    curr = 0;
    count = 0;

    /* get out quick if path is nullptr */
    if (pipath == NULL)
        return FALSE;

    /* if path string is empty, set reasonable default */
    if (*pipath == '\0')
    {
        strcpy(pipath,"A:\\*.*");
        *pipath += dos_gdrv();
    }

    /*
     * get memory for the filename array & the array that points to it:
     * we must have enough memory for the first page of directory names.
     *
     * also get memory for some pathname workareas to save stack space
     * (this also happily reduces code size).
     *
     * the order of data within the gotten area is:
     *  filename pointers
     *  filename array
     *  locstr
     *  locold
     *  mask
     */
    memblk = NULL;
    nm_files = (dos_avail_anyram()-LEN_FSWORK) / (LEN_FSNAME+sizeof(char *));
    if (nm_files >= NM_NAMES)
        memblk = dos_alloc_anyram(nm_files*(LEN_FSNAME+sizeof(char *))+LEN_FSWORK);
    if (!memblk)
    {
        fm_show(ALFSMEM, NULL, 1);
        return FALSE;
    }

    g_fslist = (LONG *)memblk;
    ad_fsnames = (char *)(g_fslist+nm_files);
    locstr = ad_fsnames + (nm_files * LEN_FSNAME);
    locold = locstr + LEN_FSPATH;
    mask = locold + LEN_FSPATH;

    strcpy(locstr, pipath);
    strcpy(locold,locstr);

    /* init strings in form */
    tree = rs_trees[FSELECTR];
    obj = tree + FTITLE;
    tedinfo = (TEDINFO *)obj->ob_spec;
    set_mask(mask, locstr);             /* save caller's mask */
    tedinfo->te_ptext = mask;           /*  & point title line at it */

    obj = tree + FSDIRECT;
    tedinfo = (TEDINFO *)obj->ob_spec;
    ad_fpath = tedinfo->te_ptext;
    inf_sset(tree, FSDIRECT, locstr);

    obj = tree + FSSELECT;
    tedinfo = (TEDINFO *)obj->ob_spec;
    ad_fname = tedinfo->te_ptext;
    fmt_str(pisel, selname);            /* selname[] is without dot */
    inf_sset(tree, FSSELECT, selname);

    obj = tree + FSTITLE;
    obj->ob_spec = pilabel ? (LONG)pilabel : (LONG)rs_str(ITEMSLCT);
    centre_title(tree,FSTITLE);

    /* set drive buttons */
    obj = tree + DRIVE_OFFSET;
    for (drive = 0, bitmask = 1; drive < NM_DRIVES; drive++, bitmask <<= 1, obj++)
    {
        if (drvbits & bitmask)
            obj->ob_state &= ~DISABLED;
        else
            obj->ob_state |= DISABLED;
    }
    select_drive(tree,get_drive(locstr),0);

    /* set clip and start form fill-in by drawing the form */
    gsx_sclip(&gl_rfs);
    fm_dial(FMD_START, &gl_rcenter, &gl_rfs);
    ob_draw(tree, ROOT, 2);

    /* init for while loop by forcing initial fs_newdir call */
    sel = 0;
    newsel = newdrive = FALSE;
    cont = newlist = TRUE;
    error = 0;      /* consecutive error count */
    while(cont)
    {
        touchob = (newlist) ? 0x0 : fm_do(tree, FSSELECT);
        gsx_mxmy(&mx, &my);

        if (newlist)
        {
            fs_sel(sel, NORMAL);
            inf_sset(tree, FSDIRECT, locstr);
            pstr = fs_pspec(locstr, NULL);
            strcpy(pstr, mask);
            curr = 0;
            sel = 0;
            newlist = FALSE;
            if (fs_newdir(locstr, mask, tree, &count))  /* ok reading dir */
                error = 0;
            else ++error;
            if (error == 1)     /* only retry once; this avoids continual retries */
            {                   /* due to e.g. missing/unformatted floppy disk    */
                /*
                 * if we have an error & the path was changed, reset it;
                 * otherwise the initial path must have been wrong, so set it
                 * to the root of the current drive.  retry in either case.
                 */
                newlist = TRUE;                     /* make it retry */
                if (strcmp(locstr,locold) != 0)     /* path was changed */
                {                                   /* so try to recover */
                    strcpy(locstr,locold);
                }
                else
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wrestrict"
                    sprintf(locstr,"%c:\\%s",'A'+dos_gdrv(),mask);
#pragma GCC diagnostic pop
                }
                select_drive(tree,get_drive(locstr),TRUE);
            }
            strcpy(locold,locstr);
        }

        value = 0;
        dclkret = ((touchob & 0x8000) != 0);
        switch(touchob &= 0x7fff)
        {
        case FSOK:
            if (path_changed(locstr))   /* just like TOS, if user has edited */
            {                           /*  the mask, 'OK' does not exit     */
                ob_change(tree,FSOK,NORMAL,TRUE);/* (so deselect the button) */
                break;
            }
            FALLTHROUGH;
        case FSCANCEL:
            cont = FALSE;
            break;
        case FUPAROW:
        case FDNAROW:
            value = 1;
            break;
        case FSVSLID:
            ob_actxywh(tree, FSVELEV, &pt);
            /* anemic slidebars
                pt.g_x -= 3;
                pt.g_w += 6;
            */
            if (!inside(mx, my, &pt))
            {
                touchob = (my <= pt.g_y) ? FUPAROW : FDNAROW;
                value = NM_NAMES;
                break;
            }
            FALLTHROUGH;
        case FSVELEV:
            fm_own(TRUE);
            value = gr_slidebox(tree, FSVSLID, FSVELEV, TRUE);
            fm_own(FALSE);
            value = curr - mul_div_round(value, count-NM_NAMES, 1000);
            if (value >= 0)
                touchob = FUPAROW;
            else
            {
                touchob = FDNAROW;
                value = -value;
            }
            break;
        case F1NAME:
        case F2NAME:
        case F3NAME:
        case F4NAME:
        case F5NAME:
        case F6NAME:
        case F7NAME:
        case F8NAME:
        case F9NAME:
            fnum = touchob - F1NAME + 1;
            if (fnum > count)
                break;
            if (sel && (sel != fnum))
                fs_sel(sel, NORMAL);
            if (sel != fnum)
            {
                sel = fnum;
                fs_sel(sel, SELECTED);
            }
            /* get string and see if file or folder */
            inf_sget(tree, touchob, selname);
            if (selname[0] == ' ')          /* a file was selected  */
            {                               /* copy to selection    */
                newsel = TRUE;
                if (dclkret)
                    cont = FALSE;
            }
            else                            /* a folder was selected:  */
            {                               /* insert name before mask */
                pstr = fs_pspec(locstr, NULL);
                unfmt_str(selname+1, pstr);
                pstr += strlen(pstr);
                *pstr++ = PATHSEP;
                strcpy(pstr, mask);
                newlist = TRUE;
            }
            break;
        case FCLSBOX:
            pstr = fs_back(locstr, NULL);
            if (pstr == locstr)             /* we have a locstr like '*.*',  */
                break;                      /*  so do nothing, just like TOS */
            if (*--pstr == DRIVESEP)        /* at root of drive, */
                break;                      /*  so nothing to do */
            pstr = fs_pspec(locstr, pstr);  /* back up past folder */
            strcpy(pstr, mask);
            newlist = TRUE;
            break;
        default:
            drive = touchob - DRIVE_OFFSET;
            if ((drive < 0) || (drive >= NM_DRIVES))/* not for us */
                break;
            if (drive == get_drive(locstr))         /* no change */
                break;
            if (path_changed(locstr))       /* like TOS, if user edited mask, */
                break;                      /*  ignore drive change           */
            obj = tree + touchob;
            if (obj->ob_state & DISABLED)           /* non-existent drive */
                break;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wrestrict"
            sprintf(locstr,"%c:\\%s",'A'+drive,mask);
#pragma GCC diagnostic pop
            newdrive = TRUE;
            break;
        }

        /* exit immediately if cancel */
        if (touchob == FSCANCEL)
            break;

        if (!newlist && !newdrive && path_changed(locstr))  /* path changed manually */
        {
            if (get_drive(ad_fpath) != get_drive(locstr))   /* drive has changed */
                newdrive = TRUE;
            else
                newlist = TRUE;
            strcpy(locstr, ad_fpath);
        }

        if (newdrive)
        {
            select_drive(tree, touchob-DRIVE_OFFSET,1);
            newdrive = FALSE;
            newlist = TRUE;
        }

        if (newlist)
        {
            inf_sset(tree, FSDIRECT, locstr);
            set_mask(mask, locstr);             /* set mask */
            if (!error)                         /* if newlist is NOT due to an error, */
            {
                selname[1] = '\0';              /* clear out the selection            */
                newsel = TRUE;
            }
        }

        if (newsel)
        {
            strcpy(ad_fname, selname + 1);
            ob_draw(tree, FSSELECT, MAX_DEPTH);
            if (!cont)
                ob_change(tree, FSOK, SELECTED, TRUE);
            newsel = FALSE;
        }

        if (value)
            curr = fs_nscroll(tree, &sel, curr, count, touchob, value);
    }

    /* return path and file name to caller */
    strcpy(pipath, locstr);
    unfmt_str(ad_fname, selname);
    strcpy(pisel, selname);

    /* start the redraw */
    fm_dial(FMD_FINISH, &gl_rcenter, &gl_rfs);

    /* return exit button */
    *pbutton = inf_what(tree, FSOK);
    dos_free(memblk);

    return TRUE;
}
