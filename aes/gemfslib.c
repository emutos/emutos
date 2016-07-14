/*
 * gemfslib.c - the file selector
 *
 * Copyright 1999, Caldera Thin Clients, Inc.
 *           2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "config.h"
#include "portab.h"
#include "struct.h"
#include "obdefs.h"
#include "dos.h"
#include "../bios/tosvars.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemdos.h"
#include "gemoblib.h"
#include "gemgraf.h"
#include "gemfmlib.h"
#include "gemgsxif.h"
#include "gemgrlib.h"
#include "geminit.h"
#include "gemsuper.h"
#include "gemshlib.h"
#include "gemfslib.h"
#include "gemrslib.h"
#include "gsx2.h"
#include "optimize.h"
#include "optimopt.h"
#include "rectfunc.h"

#include "string.h"
#include "intmath.h"

#define NM_NAMES (F9NAME-F1NAME+1)
#define NAME_OFFSET F1NAME
#define NM_DRIVES (FSLSTDRV-FS1STDRV+1)
#define DRIVE_OFFSET FS1STDRV

#define LEN_FSNAME (LEN_ZFNAME+1)   /* includes leading flag byte & trailing nul */

                            /* max number of files/directory that we can handle */
#define MAX_NM_FILES 1600L          /*  ... if we have enough memory */
#define MIN_NM_FILES 100L           /*  ... if memory is tight */


GLOBAL LONG ad_fstree;
GLOBAL GRECT gl_rfs;

static const BYTE gl_fsobj[4] = {FTITLE, FILEBOX, SCRLBAR, 0x0};

static BYTE *ad_fsnames;    /* holds filenames in currently-displayed directory */
static LONG *g_fslist;      /* offsets of filenames within ad_fsnames */
static LONG nm_files;       /* total number of slots in g_fslist[] */


/*
 *  centre specified G_STRING in the root object
 */
static void centre_title(LONG tree,WORD objnum)
{
    OBJECT *root = (OBJECT *)tree;
    OBJECT *str = root+objnum;

    str->ob_x = (root->ob_width - strlen((BYTE *)str->ob_spec)*gl_wchar) / 2;
}


/*
 *  Routine to back off the end of a path string, stopping at either
 *  the first backslash or at the colon preceding the drive specifier.
 *  The second argument specifies the end of the string; if NULL, the
 *  end is determined via strlen().  If the scan is stopped by a colon,
 *  the routine inserts a backslash in the string immediately following
 *  the colon.
 *
 *  Returns a pointer to the beginning of the string (if no colon or
 *  backslash found), or to the last backslash.
 */
static BYTE *fs_back(BYTE *pstr, BYTE *pend)
{
    BYTE *p;

    if (!pend)
        pend = pstr + strlen(pstr);

    /* back off to last backslash (or colon) */
    for (p = pend; p != pstr; p--)
    {
        if (*p == '\\')
            break;
        if (*p == ':')
        {
            if (p == pstr+1)    /* X: at the start of the string */
            {
                ins_char(++p,0,'\\',LEN_ZPATH-3);
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
static BYTE *fs_pspec(BYTE *pstr, BYTE *pend)
{
    pend = fs_back(pstr, pend);
    if (*pend == '\\')
        pend++;

    return pend;
}


/*
 *  Routine to compare files based on name
 *  Note: folders always sort lowest because the first character is \007
 */
static WORD fs_comp(BYTE *name1, BYTE *name2)
{
    return strcmp(name1, name2);
}


static LONG fs_add(WORD thefile, LONG fs_index)
{
    WORD len;

    g_fslist[thefile] = fs_index;
    ad_fsnames[fs_index++] = (D.g_dta.d_attrib & F_SUBDIR) ? 0x07 : ' ';
    len = strlencpy(ad_fsnames+fs_index,D.g_dta.d_fname);
    fs_index += len + 1;
    return fs_index;
}


/*
 *  Make a particular path the active path.  This involves
 *  reading its directory, initializing a file list, and filling
 *  out the information in the path node.  Then sort the files.
 */
static WORD fs_active(BYTE *ppath, BYTE *pspec, WORD *pcount)
{
    WORD ret;
    LONG thefile, fs_index, temp;
    WORD i, j, gap;
    BYTE *fname, allpath[LEN_ZPATH+1];
    DTA *user_dta;

    gsx_mfset(ad_hgmice);

    thefile = 0L;
    fs_index = 0L;

    strcpy(allpath, ppath);         /* 'allpath' gets all files */
    fname = fs_pspec(allpath,NULL);
    strcpy(fname,"*.*");

    user_dta = dos_gdta();          /* remember user's DTA */
    dos_sdta(&D.g_dta);
    ret = dos_sfirst(allpath, F_SUBDIR);
    while (ret == 0)
    {
        /* if it is a real file or directory then save it and set
         * the first byte to tell which
         */
        if (D.g_dta.d_fname[0] != '.')
        {
            if ((D.g_dta.d_attrib & F_SUBDIR) || (wildcmp(pspec, D.g_dta.d_fname)))
            {
                fs_index = fs_add(thefile, fs_index);
                thefile++;
            }
        }
        ret = dos_snext();

        if (thefile >= nm_files)    /* too many files */
        {
            sound(TRUE, 660, 4);
            break;
        }
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

    gsx_mfset(ad_armice);

    return TRUE;
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
static void fs_format(LONG tree, WORD currtop, WORD count)
{
    WORD i, cnt;
    WORD y, h, th;
    BYTE   *p, name[LEN_FSNAME];
    OBJECT *obj, *treeptr = (OBJECT *)tree;

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
        inf_sset(tree, NAME_OFFSET+i, name);
        obj->ob_type = G_FBOXTEXT;
        obj->ob_state = NORMAL;
    }

    /* size and position the elevator */
    y = 0;
    obj = treeptr + FSVSLID;
    th = h = obj->ob_height;
    if (count > NM_NAMES)
    {
        h = mul_div(NM_NAMES, h, count);
        h = max(gl_hbox/2, h);          /* min size elevator */
        y = mul_div(currtop, th-h, count-NM_NAMES);
    }
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
        ob_change(ad_fstree, F1NAME + sel - 1, state, TRUE);
}


/*
 *  Routine to handle scrolling the directory window a certain number
 *  of file names.
 */
static WORD fs_nscroll(LONG tree, WORD *psel, WORD curr, WORD count,
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

            bb_screen(S_ONLY, r[0].g_x, sy, r[0].g_x, dy, r[0].g_w,
                                r[0].g_h * (NM_NAMES - diffcurr) );
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
 *  will activate the directory, format it, and display ir[0].
 */
static WORD fs_newdir(BYTE *fpath, BYTE *pspec, LONG tree, WORD *pcount)
{
    const BYTE *ptmp;
    OBJECT *obj;
    TEDINFO *tedinfo;

    /* load the filenames matching pspec, sort them, and insert
     * the names in the file selector scroll box
     */
    ob_draw(tree, FSDIRECT, MAX_DEPTH);
    fs_active(fpath, pspec, pcount);
    fs_format(tree, 0, *pcount);

    obj = ((OBJECT *)tree) + FTITLE;    /* update FTITLE with ptr to mask */
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
static void set_mask(BYTE *mask,BYTE *path)
{
    BYTE *pend;

    pend = fs_pspec(path, NULL);
    if (!*pend)                 /* if there's no mask, add one */
        strcpy(pend, "*.*");
    strlcpy(mask, pend, LEN_ZPATH);
}


/*
 *  Marks object corresponding to specified drive as selected,
 *  and all others as deselected.  Optionally, if the selected
 *  drive has changed, the affected drive buttons are redrawn.
 */
static void select_drive(LONG treeaddr, WORD drive, WORD redraw)
{
    WORD i, olddrive = -1;
    OBJECT *obj, *start = (OBJECT *)treeaddr+DRIVE_OFFSET;

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

    obj = ((OBJECT *)ad_fstree) + FSDIRECT;
    ted = (TEDINFO *)obj->ob_spec;

    if (strncmp(path,(BYTE *)ted->te_ptext,ted->te_txtlen-1))
        return 1;

    return 0;
}


/*
 *  get drive number from specified path (if possible) or default
 */
static WORD get_drive(char *path)
{
    char c;

    if (path[1] == ':')     /* drive letter is present */
    {
        c = toupper(path[0]);
        if ((c >= 'A') && (c <= 'Z'))
            return c - 'A';
    }

    return dos_gdrv();
}


/*
 *  File Selector input routine that takes control of the mouse
 *  and keyboard, searchs and sort the directory, draws the file
 *  selector, interacts with the user to determine a selection
 *  or change of path, and returns to the application with
 *  the selected path, filename, and exit button.
 */
WORD fs_input(BYTE *pipath, BYTE *pisel, WORD *pbutton, BYTE *pilabel)
{
    WORD touchob, value, fnum;
    WORD curr, count, sel;
    WORD mx, my;
    LONG tree;
    ULONG bitmask;
    BYTE *ad_fpath, *ad_fname, *ad_ftitle;
    WORD drive;
    WORD dclkret, cont, newlist, newsel, newdrive;
    BYTE *pstr;
    GRECT pt;
    BYTE locstr[LEN_ZPATH+1], mask[LEN_ZPATH+1], selname[LEN_FSNAME];
    OBJECT *obj;
    TEDINFO *tedinfo;

    curr = 0;
    count = 0;

    /* get out quick if path is nullptr or if pts to null. */
    if (pipath == NULL)
        return FALSE;

    /* if path string is empty, set reasonable default */
    if (*pipath == '\0')
    {
        strcpy(pipath,"A:\\*.*");
        *pipath += dos_gdrv();
    }

    /* get memory for the filename buffer
     *  & for the array that points to it
     */
    for (nm_files = MAX_NM_FILES; nm_files >= MIN_NM_FILES; nm_files /= 2)
    {
        ad_fsnames = dos_alloc(nm_files*(LEN_FSNAME+sizeof(BYTE *)));
        if (ad_fsnames)
            break;
    }
    if (!ad_fsnames)
        return FALSE;

    g_fslist = (LONG *)(ad_fsnames+nm_files*LEN_FSNAME);

    strcpy(locstr, pipath);

    /* init strings in form */
    tree = ad_fstree;
    obj = ((OBJECT *)tree) + FTITLE;
    tedinfo = (TEDINFO *)obj->ob_spec;
    ad_ftitle = (BYTE *)tedinfo->te_ptext;
    set_mask(mask, locstr);             /* save caller's mask */
    strcpy(ad_ftitle, mask);            /*  & copy to title line */

    obj = ((OBJECT *)tree) + FSDIRECT;
    tedinfo = (TEDINFO *)obj->ob_spec;
    ad_fpath = (BYTE *)tedinfo->te_ptext;
    inf_sset(tree, FSDIRECT, locstr);

    obj = ((OBJECT *)tree) + FSSELECT;
    tedinfo = (TEDINFO *)obj->ob_spec;
    ad_fname = (BYTE *)tedinfo->te_ptext;
    fmt_str(pisel, selname);            /* selname[] is without dot */
    inf_sset(tree, FSSELECT, selname);

    obj = ((OBJECT *)tree) + FSTITLE;
    obj->ob_spec = pilabel ? (LONG)pilabel : (LONG)rs_str(ITEMSLCT);
    centre_title(tree,FSTITLE);

    /* set drive buttons */
    obj = ((OBJECT *)tree) + DRIVE_OFFSET;
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
    fm_dial(FMD_START, &gl_rfs);
    ob_draw(tree, ROOT, 2);

    /* init for while loop by forcing initial fs_newdir call */
    sel = 0;
    newsel = newdrive = FALSE;
    cont = newlist = TRUE;
    while(cont)
    {
        touchob = (newlist) ? 0x0 : fm_do(tree, FSSELECT);
        gsx_mxmy(&mx, &my);

        if (newlist)
        {
            fs_sel(sel, NORMAL);
            if ((touchob == FSOK) || (touchob == FSCANCEL))
                ob_change(tree, touchob, NORMAL, TRUE);
            inf_sset(tree, FSDIRECT, locstr);
            pstr = fs_pspec(locstr, NULL);
            strcpy(pstr, mask);
            fs_newdir(locstr, mask, tree, &count);
            curr = 0;
            sel = touchob = 0;
            newlist = FALSE;
        }

        value = 0;
        dclkret = ((touchob & 0x8000) != 0);
        switch(touchob &= 0x7fff)
        {
        case FSOK:
            if (path_changed(locstr))   /* just like TOS, if user has edited */
            {                           /*  the mask, 'OK' does not exit     */
                ob_change(tree,FSOK,NORMAL,TRUE);  /* (so deselect the button) */
                break;
            }
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
            /* drop through */
        case FSVELEV:
            fm_own(TRUE);
            value = gr_slidebox(tree, FSVSLID, FSVELEV, TRUE);
            fm_own(FALSE);
            value = curr - mul_div(value, count-NM_NAMES, 1000);
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
                *pstr++ = '\\';
                strcpy(pstr, mask);
                newlist = TRUE;
            }
            break;
        case FCLSBOX:
            pstr = fs_back(locstr, NULL);
            if (pstr == locstr)             /* we have a locstr like '*.*',  */
                break;                      /*  so do nothing, just like TOS */
            if (*--pstr == ':')             /* at root of drive, */
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
            obj = ((OBJECT *)tree) + touchob;
            if (obj->ob_state & DISABLED)           /* non-existent drive */
                break;
            sprintf(locstr,"%c:\\%s",'A'+drive,mask);
            newdrive = TRUE;
            break;
        }

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
            selname[1] = '\0';                  /* selected is empty */
            newsel = TRUE;
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
    fm_dial(FMD_FINISH, &gl_rfs);

    /* return exit button */
    *pbutton = inf_what(tree, FSOK, FSCANCEL);
    dos_free((LONG)ad_fsnames);

    return TRUE;
}
