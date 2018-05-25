/*      DESKDIR.C       09/03/84 - 06/05/85     Lee Lorenzen            */
/*                      4/7/86   - 8/27/86      MDF                     */
/*      merge source    5/19/97  - 5/28/87      mdf                     */
/*      for 3.0         11/13/87                mdf                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2017 The EmuTOS development team
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
#include "dos.h"
#include "gemdos.h"
#include "optimize.h"

#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "aesbind.h"
#include "desksupp.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskmain.h"
#include "deskinf.h"
#include "deskdir.h"
#include "deskins.h"
#include "gemerror.h"
#include "kprint.h"


#define MAX_CLUS_SIZE   (32*1024L)  /* maximum cluster size */

#define ALLFILES    (F_SUBDIR|F_SYSTEM|F_HIDDEN)

#define OP_RENAME   777     /* used internally by dir_op(): must not be the same as any other OP_XXX ! */

static UBYTE    *copybuf;   /* for copy operations */
static LONG     copylen;    /* size of above buffer */

static WORD     ml_havebox;
static WORD     deleted_folders;
/*
 * check for UNDO key pressed: if so, ask user if she wants to abort and,
 * if so, return TRUE.  otherwise return FALSE.
 */
static WORD user_abort(void)
{
    LONG rawin;
    WORD rc = 0;

    if (dos_conis() == -1)          /* character waiting */
    {
        rawin = dos_rawcin() & 0x00ff00ffL;
        if (rawin == 0x00610000L)   /* the Atari UNDO key */
            rc = fun_alert(1, STABORT);
    }

    return (rc==1) ? 1 : 0;
}


/*
 *  Routine to DRAW a DIALog box centered on the screen
 */
void draw_dial(OBJECT *tree)
{
    WORD xd, yd, wd, hd;

    form_center(tree, &xd, &yd, &wd, &hd);
    objc_draw(tree, ROOT, MAX_DEPTH, xd, yd, wd, hd);
}


/*
 * display copy alert dialog & wait for selection, then (re)display copying dialog
 *
 * returns selected object number
 */
static WORD do_namecon(void)
{
    OBJECT *tree = G.a_trees[ADCPALER];
    WORD ob;

    graf_mouse(ARROW, NULL);
    if (ml_havebox)
        draw_dial(tree);
    else
    {
        start_dialog(tree);
        ml_havebox = TRUE;
    }
    form_do(tree, 0);
    draw_dial(G.a_trees[ADCPYDEL]);
    graf_mouse(HGLASS, NULL);

    ob = inf_gindex(tree, CAOK, 3) + CAOK;
    (tree+ob)->ob_state = NORMAL;

    return ob;
}


/*
 *  Draw a single field of a dialog box
 */
void draw_fld(OBJECT *tree, WORD obj)
{
    GRECT t;
    OBJECT *objptr = tree + obj;

    memcpy(&t,&objptr->ob_x,sizeof(GRECT));
    objc_offset(tree, obj, &t.g_x, &t.g_y);
    objc_draw(tree, obj, MAX_DEPTH, t.g_x, t.g_y, t.g_w, t.g_h);
}


/*
 *  Add a new directory name to the end of an existing path.  This
 *  includes appending a \*.*.
 */
void add_path(BYTE *path, BYTE *new_name)
{
    path = filename_start(path);
    strcpy(path, new_name);
    strcat(path, "\\*.*");
}


/*
 *  Remove the last directory in the path and replace it with *.*
 */
static void sub_path(BYTE *path)
{
    /* scan to last segment in path */
    path = filename_start(path);

    /* now back up to previous directory in path */
    path -= 2;
    while (*path != '\\')
        path--;

    strcpy(path, "\\*.*");
}


/*
 *  Add a file name to the end of an existing path, assuming that
 *  the existing path ends in "*...".
 *
 *  Returns pointer to the start of the added name within the path.
 */
BYTE *add_fname(BYTE *path, BYTE *new_name)
{
    path = filename_start(path);

    return strcpy(path, new_name);
}


/*
 *  Restores "*.*" to the position in a path that was
 *  overwritten by add_fname() above
 */
void restore_path(BYTE *target)
{
    strcpy(target,"*.*");
}


/*
 *  test if specified file/folder exists
 */
static WORD item_exists(BYTE *path, BOOL is_folder)
{
    BYTE *p;
    DTA *dta;
    WORD rc;

    dta = dos_gdta();
    dos_sdta(&G.g_wdta);
    if (is_folder)
    {
        p = path + strlen(path);    /* point to end of path */
        strcpy(p, "\\*.*");
    }
    rc = dos_sfirst(path, ALLFILES);/* check if item exists */
    if (is_folder)
        *p = '\0';
    dos_sdta(dta);

    return !rc;
}


/*
 *  Remove the file name from the end of a path and append an *.*
 */
void del_fname(BYTE *pstr)
{
    strcpy(filename_start(pstr), "*.*");
}


/*
 *  Parse to find the filename part of a path and return a copy of it
 *  in a form ready to be placed in a dialog box.
 *
 *  input:  pstr, the full pathname
 *  output: newstr, the formatted filename
 */
static void get_fname(BYTE *pstr, BYTE *newstr)
{
    BYTE ml_ftmp[LEN_ZFNAME];

    strcpy(ml_ftmp, filename_start(pstr));
    fmt_str(ml_ftmp, newstr);
}


/*
 *  if err is not negative, return TRUE; otherwise:
 *      (1) if it's a BDOS error, issue a message via form_error()
 *      (2) return FALSE
 */
WORD d_errmsg(WORD err)
{
    if (err >= 0)
        return TRUE;

    if (!IS_BIOS_ERROR(err))
        form_error(-err-31);    /* convert to 'MS-DOS error code' */

    return FALSE;
}


static WORD invalid_copy_msg(void)
{
    fun_alert(1, STINVCPY);
    return FALSE;
}


/*
 *  Directory routine to DO File DELeting
 */
static WORD d_dofdel(BYTE *ppath)
{
    WORD ret;

    ret = dos_delete(ppath);

    return d_errmsg(ret);
}


/*
 *  Determines output filename as required by d_dofcopy()
 *
 *  Returns:
 *      1   filename is OK
 *      0   error, stop copying
 *      -1  error, but allow more copying
 */
static WORD output_fname(BYTE *psrc_file, BYTE *pdst_file)
{
    WORD fh, ob = 0, samefile;
    LONG ret;
    OBJECT *tree = G.a_trees[ADCPALER];
    BYTE ml_fsrc[LEN_ZFNAME], ml_fdst[LEN_ZFNAME], ml_fstr[LEN_ZFNAME];
    BYTE old_dst[LEN_ZFNAME];

    while(1)
    {
        /*
         * set flag if user is trying to overwrite a file with itself
         */
        samefile = !strcmp(psrc_file, pdst_file);

        /*
         * if the files are different:
         *  . if the user said OK in response to the do_namecon() dialog
         *    in a previous iteration, OR
         *  . if the user doesn't want to be warned about overwrites, OR
         *  . if the output file doesn't exist,
         *      exit this loop, the copy/move can proceed
         */
        if (!samefile)
        {
            if (ob == CAOK)
                break;
            if (!G.g_covwrpref)
                break;
            ret = dos_open(pdst_file, 0);
            if (ret < 0L)
            {
                if (ret == EFILNF)
                    break;
                return d_errmsg((WORD)ret);
            }
            fh = (WORD)ret;
            dos_close(fh);
        }

        /*
         * either the files are the same, or the output file exists and
         * the user wants to be notified about overwrites, so we need
         * to tell the user: get i/p & o/p filenames and prefill dialog
         */
        get_fname(psrc_file, ml_fsrc);  /* get input filename */
        get_fname(pdst_file, ml_fdst);  /* get output filename */
        inf_sset(tree, CACURRNA, ml_fsrc);
        inf_sset(tree, CACOPYNA, ml_fdst);

        /*
         * display dialog & get input
         */
        strcpy(old_dst,ml_fdst);        /* remember old destination */
        ob = do_namecon();
        if (ob == CASTOP)
            return 0;
        else if (ob == CASKIP)
            return -1;

        /*
         * user says ok, so update destination filename
         */
        inf_sget(tree, CACOPYNA, ml_fdst);
        unfmt_str(ml_fdst, ml_fstr);
        if (ml_fstr[0] != '\0')
        {
            del_fname(pdst_file);
            add_fname(pdst_file, ml_fstr);
        }

        /*
         * if destination has changed, pretend there was no OK, so
         * the next iteration of this loop can check everything again
         */
        if (strcmp(old_dst,ml_fdst))
        {
            ob = 0;
            continue;
        }

        /*
         * the user has said OK, and the destination is unchanged:
         * if the user was trying to overwrite a file with itself,
         * treat it as a skip
         */
        if (samefile)
            return -1;
    }

    return 1;
}


/*
 *  Directory routine to DO File COPYing
 *
 *  Returns:
 *      1/TRUE  ok
 *      0/FALSE if error opening destination, or user said stop,
 *              or error during copy (including disk full)
 *      -1      user skipped this copy
 */
static WORD d_dofcopy(BYTE *psrc_file, BYTE *pdst_file, WORD time, WORD date, WORD attr)
{
    BOOL diskfull = FALSE;
    WORD srcfh, dstfh, rc;
    LONG readlen, writelen, error;

    error = dos_open(psrc_file, 0);
    if (error < 0L)
        return d_errmsg((WORD)error);
    srcfh = (WORD)error;

    rc = output_fname(psrc_file, pdst_file);
    if (rc <= 0)        /* not allowed to copy file */
    {
        dos_close(srcfh);
        if (rc == 0)    /* unexpected error opening dest, or user said stop */
            return FALSE;
        return -1;      /* user said skip, notify caller */
    }

    /*
     * we have the (possibly-modified) filename in pdst_file
     */
    error = dos_create(pdst_file, attr);
    if (error < 0L)
        return invalid_copy_msg();
    dstfh = (WORD)error;

    /*
     * perform copy
     */
    rc = TRUE;
    while(1)
    {
        error = readlen = dos_read(srcfh, copylen, copybuf);
        if (error <= 0L)    /* error or end of file */
            break;

        error = writelen = dos_write(dstfh, readlen, copybuf);
        if (error < 0L)
            break;

        if (writelen != readlen)
        {
            diskfull = TRUE;
            break;
        }
    }

    if (diskfull)
    {
        fun_alert_merge(1, STDISKFU, pdst_file[0]);
        rc = FALSE;
    }
    else if (error < 0L)
    {
        if (IS_BIOS_ERROR(error))
            invalid_copy_msg();
        else d_errmsg((WORD)error);
        rc = FALSE;
    }
    else
    {
        rc = d_errmsg(dos_setdt(dstfh, time, date));
    }

    dos_close(srcfh);       /* close files */
    dos_close(dstfh);

    if (diskfull)
        dos_delete(pdst_file);

    return rc;
}


/*
 *  Routine to update any windows that were displaying
 *  a subfolder of a folder that has been deleted/moved.
 *  Such windows are updated to display the root directory
 *  of the drive concerned.
 */
static void update_modified_windows(BYTE *path,WORD length)
{
    WNODE *pwin;

    for (pwin = G.g_wfirst; pwin; pwin = pwin->w_next)
    {
       if (pwin->w_id)
            if (strncmp(pwin->w_pnode.p_spec,path,length) == 0)
                fun_close(pwin,CLOSE_TO_ROOT);
    }
}


/*
 *  Directory routine to DO an operation on an entire sub-directory
 */
WORD d_doop(WORD level, WORD op, BYTE *psrc_path, BYTE *pdst_path, OBJECT *tree, DIRCOUNT *count)
{
    BYTE *ptmp, *ptmpdst;
    DTA  *dta = &G.g_dtastk[level];
    WORD more, ret;

    if (level == 0)
        deleted_folders = 0L;

    dos_sdta(dta);

    for (ret = dos_sfirst(psrc_path, ALLFILES); ; ret = dos_snext())
    {
        more = TRUE;
        /*
         * handle end of folder
         */
        if ((ret < 0) && ((ret == ENMFIL) || (ret == EFILNF)))
        {
            switch(op)
            {
            case OP_COUNT:
                G.g_ndirs++;
                break;
            case OP_DELETE:
            case OP_MOVE:
                ptmp = filename_start(psrc_path) - 1;
                *ptmp = '\0';
                ret = dos_rmdir(psrc_path);
                strcpy(ptmp, "\\*.*");
                more = d_errmsg(ret);
                if (more)
                    deleted_folders++;
                /*
                 * if we're finishing up, and we deleted one or more folders,
                 * update any window that was displaying the contents of
                 * that folder or a subfolder of it
                 */
                if ((level == 0) && deleted_folders)
                    update_modified_windows(psrc_path,ptmp-psrc_path+1);
                break;
            default:
                break;
            }
            if (tree)
            {
                inf_numset(tree, CDFOLDS, --(count->dirs));
                draw_fld(tree, CDFOLDS);
            }
            return more;
        }

        /*
         * return if real error
         */
        if (ret < 0)
            return d_errmsg(ret);

        if (op != OP_COUNT)
            if (user_abort())
            {
                more = FALSE;
                break;
            }

        /*
         * handle folder
         */
        if (dta->d_attrib & F_SUBDIR)
        {
            if ((dta->d_fname[0] != '.') && (level < (MAX_LEVEL-1)))
            {
                add_path(psrc_path, dta->d_fname);
                if ((op == OP_COPY) || (op == OP_MOVE))
                {
                    add_fname(pdst_path, dta->d_fname);
                    ret = dos_mkdir(pdst_path);
                    if (ret < 0)
                    {
                        if (ret != EACCDN)
                            more = d_errmsg(ret);
                        else if (!item_exists(pdst_path, TRUE))
                            more = invalid_copy_msg();
                    }
                    strcat(pdst_path, "\\*.*");
                }
                if (more)
                {
                    more = d_doop(level+1,op,psrc_path,pdst_path,tree,count);
                    dos_sdta(dta);      /* must restore DTA address! */
                }
                sub_path(psrc_path);    /* restore the old paths */
                if ((op == OP_COPY) || (op == OP_MOVE))
                    sub_path(pdst_path);
            }
            if (!more)
                break;
            continue;
        }

        /*
         * handle file
         */
        if (op != OP_COUNT)
            ptmp = add_fname(psrc_path, dta->d_fname);
        switch(op)
        {
        case OP_COUNT:
            G.g_nfiles++;
            G.g_size += dta->d_length;
            break;
        case OP_DELETE:
            more = d_dofdel(psrc_path);
            break;
        case OP_COPY:
        case OP_MOVE:
            ptmpdst = add_fname(pdst_path, dta->d_fname);
            more = d_dofcopy(psrc_path, pdst_path, dta->d_time,
                            dta->d_date, dta->d_attrib);
            restore_path(ptmpdst);  /* restore original dest path */
            /* if moving, delete original only if copy was ok */
            if ((op == OP_MOVE) && (more > 0))
                more = d_dofdel(psrc_path);
            break;
        }
        if (op != OP_COUNT)
            restore_path(ptmp);     /* restore original source path */
        if (tree)
        {
            inf_numset(tree, CDFILES, --(count->files));
            draw_fld(tree, CDFILES);
        }
        if (!more)
            break;
    }

    return more;
}


/*
 *      Prompt for new name (updates dstpth with new name)
 *
 *      Returns:
 *          2   OK, and name has changed
 *          1   OK, and name hasn't changed
 *          0   STOP
 *          -1  SKIP
 */
static WORD get_new_name(BYTE *dstpth)
{
    BYTE ml_fsrc[LEN_ZFNAME], ml_fdst[LEN_ZFNAME], str[LEN_ZFNAME];
    OBJECT *tree = G.a_trees[ADCPALER];
    WORD ob;

    get_fname(dstpth, ml_fsrc);         /* extract current folder name */
    strcpy(ml_fdst,ml_fsrc);            /* pre-fill new folder name */
    inf_sset(tree, CACURRNA, ml_fsrc);  /* and put both in dialog */
    inf_sset(tree, CACOPYNA, ml_fdst);

    ob = do_namecon();                  /* show dialog */
    if (ob == CASTOP)                   /* "Stop" button */
        return 0;
    if (ob == CASKIP)                   /* "Skip" button */
        return -1;

    inf_sget(tree, CACOPYNA, ml_fdst);
    unfmt_str(ml_fdst, str);            /* get new dest folder in str */
    del_fname(dstpth);                  /* & update destination path  */
    add_fname(dstpth, str);             /* (it may not have changed)  */

    return strcmp(ml_fdst,ml_fsrc) ? 2 : 1;
}


/*
 *      Determines output path as required by dir_op()
 *
 *  Returns:
 *      1   path is OK (folder has been created if necessary)
 *      0   error, stop copying
 *     -1   error, but allow more copying
 */
static WORD output_path(WORD op,BYTE *srcpth, BYTE *dstpth)
{
    WORD ret;

    while(1)
    {
        /*
         * for move via rename, the destination folder must not exist
         */
        if (op == OP_RENAME)
        {
            if (!item_exists(dstpth, TRUE))
                break;
        }
        else
        {
            ret = dos_mkdir(dstpth);
            if (ret == 0)           /* ok, we created the new folder */
                break;
            if (ret != EACCDN)      /* some strange problem */
                return d_errmsg(ret);

            /*
             * we cannot create the folder: either it already exists
             * or there is insufficient space (e.g. in root dir)
             */
            if (!item_exists(dstpth, TRUE))
                return invalid_copy_msg();
        }

        /*
         * the destination folder exists: we try to get a new one
         */
        ret = get_new_name(dstpth);
        if (ret <= 0)
            return ret;

        /*
         * if it's not move via rename, and the destination path has not
         * changed, we're done (we'll copy/move into the existing path);
         * otherwise we loop to check the updated destination path
         */
        if ((op != OP_RENAME) && (ret == 1))
            break;
    }

    strcat(dstpth, "\\*.*");        /* complete path */

    return 1;
}


/*
 *      Routine to do file rename for dir_op()
 */
static WORD d_dofileren(BYTE *oldname, BYTE *newname, BOOL is_folder)
{
    WORD ret;

    while(1)
    {
        ret = dos_rename(oldname,newname);
        if (ret == 0)               /* rename ok */
            return TRUE;

        if (ret != EACCDN)          /* some strange problem */
            return d_errmsg(ret);

        /*
         * we cannot rename the file/folder: either it already exists
         * or there is insufficient space (e.g. in root dir)
         */
        if (!item_exists(newname,is_folder))
            return invalid_copy_msg();

        /*
         * we cannot rename because the file/folder exists, so
         * prompt for new name
         */
        if (get_new_name(newname) <= 0)
            break;
    }

    return FALSE;
}


/*
 *      Routine to do folder rename for dir_op()
 */
static WORD d_dofoldren(BYTE *oldname, BYTE *newname)
{
    BYTE *p;

    p = filename_start(oldname) - 1;    /* remove trailing wildcards */
    *p = '\0';
    p = filename_start(newname) - 1;
    *p = '\0';

    return d_dofileren(oldname,newname,TRUE);
}


/*
 *  DIRectory routine that does an OPeration on all the selected files and
 *  folders in the source path.  The selected files and folders are
 *  marked in the source file list.
 */
WORD dir_op(WORD op, WORD icontype, PNODE *pspath, BYTE *pdst_path, DIRCOUNT *count)
{
    OBJECT *tree, *obj;
    FNODE *pf;
    WORD more, confirm;
    BYTE *ptmpsrc, *ptmpdst, *psrc_path = pspath->p_spec;
    LONG lavail;
    BYTE srcpth[MAXPATHLEN], dstpth[MAXPATHLEN];

    graf_mouse(HGLASS, NULL);

    ml_havebox = FALSE;
    confirm = 0;

    if ((op == OP_MOVE) && (*psrc_path == *pdst_path))
    {
        KDEBUG(("dir_op(): converting move %s->%s to rename\n",psrc_path,pdst_path));
        op = OP_RENAME;
    }

    tree = NULL;
    if (op != OP_COUNT)
    {
        tree = G.a_trees[ADCPYDEL];
        obj = tree + CDTITLE;
    }

    switch(op)
    {
    case OP_COUNT:
        G.g_nfiles = 0L;
        G.g_ndirs = 0L;
        G.g_size = 0L;
        break;
    case OP_DELETE:
        confirm = G.g_cdelepref;
        obj->ob_spec = (LONG) ini_str(STDELETE);
        break;
    case OP_COPY:
    case OP_MOVE:
        lavail = dos_avail_stram() - 0x400; /* allow safety margin */
        if (lavail < 0L)
        {
            form_error(E_NOMEMORY);     /* let user know */
            graf_mouse(ARROW, NULL);
            return FALSE;
        }
        /*
         * for efficiency, the copy length should be a multiple of
         * cluster size.  it's a lot of work to figure out the actual
         * cluster sizes for the source and destination, but in most
         * cases, available memory will be >=32K, the maximum possible
         * cluster size.  in this case, we set 'copylen' to the largest
         * multiple that fits in available memory.  if we have less
         * than 32K available, we just set it as large as possible.
         * we always allocate in ST RAM to avoid extra copying via
         * the FRB for ACSI & floppy I/O.
         */
        if (lavail >= MAX_CLUS_SIZE)
            copylen = lavail & ~(MAX_CLUS_SIZE-1);
        else copylen = lavail;
#if CONF_PREFER_STRAM_DISK_BUFFERS
        copybuf = dos_alloc_stram(copylen);
#else
        copybuf = dos_alloc_anyram(copylen);
#endif
        /* drop thru */
    case OP_RENAME:
        confirm = G.g_ccopypref;
        obj->ob_spec = (LONG) ini_str(STCOPY);
        if (op != OP_COPY)      /* i.e. OP_MOVE or OP_RENAME */
        {
            confirm |= G.g_cdelepref;
            obj->ob_spec = (LONG) ini_str(STMOVE);
        }
        break;
    }

    more = TRUE;

    if (tree)
    {
        centre_title(tree);
        inf_numset(tree, CDFILES, count->files);
        inf_numset(tree, CDFOLDS, count->dirs);
        start_dialog(tree);
        ml_havebox = TRUE;
        if (confirm)
        {
            graf_mouse(ARROW, NULL);
            form_do(tree, 0);
            graf_mouse(HGLASS, NULL);
            more = inf_what(tree, CDOK, CDCNCL);
        }
    }

    /*
     * if user says OK, but we're deleting a whole disk, we always
     * get an additional special prompt
     */
    if (more && (op == OP_DELETE) && (icontype == AT_ISDISK))
    {
        graf_mouse(ARROW, NULL);
        more = (fun_alert_merge(2, STDELDIS, psrc_path[0]) == 1) ? TRUE: FALSE;
        graf_mouse(HGLASS, NULL);
    }

    for (pf = pspath->p_flist; pf && more; pf = pf->f_next)
    {
        if (!fnode_is_selected(pf))
            continue;
        if (op != OP_COUNT)
            if (user_abort())
                break;

        strcpy(srcpth, psrc_path);
        if ((op == OP_COPY) || (op == OP_MOVE) || (op == OP_RENAME))
            strcpy(dstpth, pdst_path);

        /*
         * handle folder
         */
        if (pf->f_attr & F_SUBDIR)
        {
            add_path(srcpth, pf->f_name);
            if ((op == OP_COPY) || (op == OP_MOVE) || (op == OP_RENAME))
            {
                add_fname(dstpth, pf->f_name);
                more = output_path(op,srcpth,dstpth);
            }

            if (more > 0)   /* no conflict, or user said OK */
            {
                if (((op == OP_COPY) || (op == OP_MOVE))
                 && (strcmp(srcpth,dstpth) == 0))
                    ;       /* do nothing for copy/move to self */
                else
                    more = (op==OP_RENAME) ? d_dofoldren(srcpth,dstpth) :
                            d_doop(0, op, srcpth, dstpth, tree, count);
            }
            continue;
        }

        /*
         * handle file
         */
        if (op != OP_COUNT)
            ptmpsrc = add_fname(srcpth, pf->f_name);
        switch(op)
        {
        case OP_COUNT:
            G.g_nfiles++;
            G.g_size += pf->f_size;
            break;
        case OP_DELETE:
            more = d_dofdel(srcpth);
            break;
        case OP_COPY:
        case OP_MOVE:
        case OP_RENAME:
            ptmpdst = add_fname(dstpth, pf->f_name);
            more = (op==OP_RENAME) ? d_dofileren(srcpth,dstpth,FALSE) :
                    d_dofcopy(srcpth, dstpth, pf->f_time, pf->f_date, pf->f_attr);
            restore_path(ptmpdst);  /* restore original dest path */
            /* if moving, delete original only if copy was ok */
            if ((op == OP_MOVE) && (more > 0))
                more = d_dofdel(srcpth);
            break;
        }
        if (op != OP_COUNT)
            restore_path(ptmpsrc);  /* restore original source path */

        if (tree)
        {
            count->files -= 1;
            inf_numset(tree, CDFILES, count->files);
            draw_fld(tree, CDFILES);
        }
    }

    switch(op)
    {
    case OP_COUNT:
        count->files = G.g_nfiles;
        count->dirs = G.g_ndirs;
        count->size = G.g_size;
        break;
    case OP_DELETE:
    case OP_RENAME:
        break;
    case OP_COPY:
    case OP_MOVE:
        dos_free(copybuf);
        break;
    }

    if (tree)
        end_dialog(tree);
    graf_mouse(ARROW, NULL);

    return TRUE;
}
