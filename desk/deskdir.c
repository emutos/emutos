/*      DESKDIR.C       09/03/84 - 06/05/85     Lee Lorenzen            */
/*                      4/7/86   - 8/27/86      MDF                     */
/*      merge source    5/19/97  - 5/28/87      mdf                     */
/*      for 3.0         11/13/87                mdf                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
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

#include "config.h"
#include "portab.h"
#include "string.h"
#include "obdefs.h"
#include "dos.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "deskbind.h"

#include "optimize.h"
#include "gemdos.h"
#include "aesbind.h"
#include "deskglob.h"
#include "deskgraf.h"
#include "deskgsx.h"
#include "desksupp.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskmain.h"
#include "deskdir.h"


#define MAX_TWIDTH 45                           /* used in blank_it()   */

#define MAX_CLUS_SIZE   (32*1024L)  /* maximum cluster size */

#define ALLFILES    (F_SUBDIR|F_SYSTEM|F_HIDDEN)

static UBYTE    *copybuf;   /* for copy operations */
static LONG     copylen;    /* size of above buffer */

static WORD     ml_havebox;
static BYTE     ml_fsrc[LEN_ZFNAME], ml_fdst[LEN_ZFNAME], ml_fstr[LEN_ZFNAME], ml_ftmp[LEN_ZFNAME];


/*
*       Routine to DRAW a DIALog box centered on the screen
*/
static void draw_dial(LONG tree)
{
        WORD            xd, yd, wd, hd;
        OBJECT          *obtree = (OBJECT *)tree;

        form_center(tree, &xd, &yd, &wd, &hd);
        objc_draw(obtree, ROOT, MAX_DEPTH, xd, yd, wd, hd);
} /* draw_dial */


void show_hide(WORD fmd, LONG tree)
{
        WORD            xd, yd, wd, hd;
        OBJECT          *obtree = (OBJECT *)tree;

        form_center(tree, &xd, &yd, &wd, &hd);
        form_dial(fmd, 0, 0, 0, 0, xd, yd, wd, hd);
        if (fmd == FMD_START)
          objc_draw(obtree, ROOT, MAX_DEPTH, xd, yd, wd, hd);
}


static void do_namecon(void)
{
/* BugFix       */
        graf_mouse(ARROW, 0x0L);
        if (ml_havebox)
          draw_dial(G.a_trees[ADCPALER]);
        else
        {
          show_hide(FMD_START, G.a_trees[ADCPALER]);
          ml_havebox = TRUE;
        }
        form_do(G.a_trees[ADCPALER], 0);
        draw_dial(G.a_trees[ADCPYDEL]);
        graf_mouse(HGLASS, NULL);
} /* do_namecon */


/*
*       Draw a single field of a dialog box
*/
void draw_fld(LONG tree, WORD obj)
{
        GRECT           t;
        OBJECT *obtree = (OBJECT *)tree;
        OBJECT *objptr = obtree + obj;

        memcpy(&t,&objptr->ob_x,sizeof(GRECT));
        objc_offset(obtree, obj, &t.g_x, &t.g_y);
        objc_draw(obtree, obj, MAX_DEPTH, t.g_x, t.g_y, t.g_w, t.g_h);
} /* draw_fld */


/*
 *      Scans the specified path string & returns pointer to last
 *      path separator (i.e. backslash).
 *      If a separator isn't found, returns pointer to end of string.
 */
BYTE *last_separator(BYTE *path)
{
        BYTE *last = NULL;

        for ( ; *path; path++)
          if (*path == '\\')
            last = path;

        return last ? last : path;
}


/*
*       Add a new directory name to the end of an existing path.  This
*       includes appending a \*.*.
*/
void add_path(BYTE *path, BYTE *new_name)
{
        while (*path != '*')
          path++;
        strcpy(path, new_name);
        strcat(path, "\\*.*");
} /* add_path */


/*
*       Remove the last directory in the path and replace it with
*       *.*.
*/
static void sub_path(BYTE *path)
{
                                                /* scan to last slash   */
        path = last_separator(path);
                                                /* now skip to previous */
                                                /*   directroy in path  */
        path--;
        while (*path != '\\')
          path--;
                                                /* append a *.*         */
        strcpy(path, "\\*.*");
} /* sub_path */


/*
 *      Add a file name to the end of an existing path, assuming that
 *      the existing path ends in "*...".
 *      Returns pointer to the start of the added name within the path.
 */
BYTE *add_fname(BYTE *path, BYTE *new_name)
{
        while (*path != '*')
          path++;

        return strcpy(path, new_name);
} /* add_fname */



/*
 *      Restores "*.*" to the position in a path that was
 *      overwritten by add_fname() above
 */
void restore_path(BYTE *target)
{
        strcpy(target,"*.*");
}



/*
*       Check if path is associated with an open window
*
*       If so, returns pointer to first matching window;
*       otherwise returns NULL.
*/
WNODE *fold_wind(BYTE *path)
{
        WORD            i;
        WNODE           *pwin;

        for (i = 0, pwin = G.g_wlist; i < NUM_WNODES; i++, pwin++)
        {
          if (pwin->w_id)
            if (strcmp(pwin->w_path->p_spec, path) == 0)
              return pwin;
        }
        return NULL;
}





/*
*       Routine to check that the name we will be adding is like the
*       last folder name in the path.
*/
static void like_parent(BYTE *path, BYTE *new_name)
{
        BYTE            *pstart, *lastfold, *lastslsh;
                                                /* remember start of path*/
        pstart = path;
                                                /* scan to lastslsh     */
        lastslsh = path = last_separator(path);
                                                /* back up to next to   */
                                                /*   last slash if it   */
                                                /*   exists             */
        path--;
        while ( (*path != '\\') &&
                (path > pstart) )
          path--;
                                                /* remember start of    */
                                                /*   last folder name   */
        if (*path == '\\')
          lastfold = path + 1;
        else
          lastfold = 0;

        if (lastfold)
        {
          *lastslsh = NULL;
          if( strcmp(lastfold, new_name)==0 )
            return;
          *lastslsh = '\\';
        }
        add_fname(pstart, new_name);
} /* like_parent */


/*
*       See if these two paths represent the same folder.  The first
*       path ends in \*.*, the second path ends with just the folder.
*/
static WORD same_fold(BYTE *psrc, BYTE *pdst)
{
        WORD            ret;
        BYTE            *lastslsh;
                                                /* scan to lastslsh     */
        lastslsh = last_separator(psrc);
                                                /* null it              */
        *lastslsh = NULL;
                                                /* see if they match    */
        ret = !strcmp(psrc, pdst);
                                                /* restore it           */
        *lastslsh = '\\';
                                                /* return if same       */
        return( ret );
}



/*
*       Remove the file name from the end of a path and append on
*       an \*.*
*/
void del_fname(BYTE *pstr)
{
        strcpy(last_separator(pstr), "\\*.*");
} /* del_fname */


/*
*       Parse to find the filename part of a path and return a copy of it
*       in a form ready to be placed in a dialog box.
*/
static void get_fname(BYTE *pstr, BYTE *newstr)
{
        strcpy(&ml_ftmp[0], last_separator(pstr)+1);
        fmt_str(&ml_ftmp[0], newstr);
} /* get_fname */


WORD d_errmsg(void)
{
        if (DOS_ERR)
        {
          form_error(DOS_AX);
          return(FALSE);
        }
        return(TRUE);
}


/*
*       Directory routine to DO File DELeting.
*/
static WORD d_dofdel(BYTE *ppath)
{
        dos_delete(ppath);
        return( d_errmsg() );
} /* d_dofdel */


/*
 *      Determines output filename as required by d_dofcopy()
 *      Returns:
 *          1   filename is OK
 *          0   error, stop copying
 *          -1  error, but allow more copying
 */
WORD output_fname(BYTE *psrc_file, BYTE *pdst_file)
{
    WORD fh, ob, samefile;
    LONG tree;

    while(1)
    {
        fh = dos_open(pdst_file, 0);
        if (DOS_ERR)
        {
            if (DOS_AX == E_FILENOTFND)
                break;
            return d_errmsg();
        }
        dos_close(fh);

        /*
         * file exists: this is OK as long as
         *     a) user doesn't want to be notified about overwrites
         *     b) i/p and o/p filenames are different (prevent overwriting ourselves)
         */
        samefile = !strcmp(psrc_file, pdst_file);
        if (!G.g_covwrpref && !samefile)
            break;

        /*
         * need to talk to user: get i/p & o/p filenames and prefill dialog
         */
        get_fname(psrc_file, ml_fsrc);  /* get input filename */
        if (samefile)                   /* don't prefill o/p if same file */
            ml_fdst[0] = '\0';
        else
            get_fname(pdst_file, ml_fdst);
        inf_sset(G.a_trees[ADCPALER], CACURRNA, ml_fsrc);
        inf_sset(G.a_trees[ADCPALER], CACOPYNA, ml_fdst);

        /*
         * display dialog & get input
         */
        do_namecon();

        tree = G.a_trees[ADCPALER];
        ob = inf_gindex(G.a_trees[ADCPALER], CAOK, 3) + CAOK;
        ((OBJECT *)tree+ob)->ob_state = NORMAL;
        if (ob == CASTOP)
            return 0;
        else if (ob == CACNCL)
            return -1;

        /*
         * user says ok, so decode filename & try again
         */
        inf_sget(G.a_trees[ADCPALER], CACOPYNA, ml_fdst);
        unfmt_str(ml_fdst, ml_fstr);
        if (ml_fstr[0] != '\0')
        {
            del_fname(pdst_file);
            add_fname(pdst_file, ml_fstr);
        }
    }

    return 1;
}


/*
 *      Directory routine to DO File COPYing
 *      Returns FALSE iff failure
 */
static WORD d_dofcopy(BYTE *psrc_file, BYTE *pdst_file, WORD time, WORD date, WORD attr)
{
    WORD srcfh, dstfh, rc;
    LONG readlen, writelen;

    srcfh = dos_open(psrc_file, 0);
    if (DOS_ERR)
        return d_errmsg();

    rc = output_fname(psrc_file, pdst_file);
    if (rc <= 0)        /* not allowed to copy file */
    {
        dos_close(srcfh);
        if (rc == 0)    /* unexpected error opening dest, or user said stop */
            return FALSE;
        return TRUE;    /* user just said cancel, so allow continuation */
    }

    /*
     * we have the (possibly-modified) filename in pdst_file
     */
    dstfh = dos_create(pdst_file, attr);
    if (DOS_ERR)
        return d_errmsg();

    /*
     * perform copy
     */
    while(1)
    {
        readlen = dos_read(srcfh, copylen, copybuf);
        if (DOS_ERR)
            break;
        if (readlen == 0)   /* end of file */
            break;
        writelen = dos_write(dstfh, readlen, copybuf);
        if (DOS_ERR)
            break;
        if (writelen != readlen)    /* disk full? */
        {
            graf_mouse(ARROW, NULL);
            fun_alert(1, STDISKFU, NULL);
            graf_mouse(HGLASS, NULL);
            rc = -1;        /* indicate disk full error */
            break;
        }
    }

    if (DOS_ERR)
        rc = d_errmsg();    /* report read or write error */

    if (rc > 0)
    {
        dos_setdt(dstfh, time, date);
        rc = d_errmsg();
    }

    dos_close(srcfh);       /* close files */
    dos_close(dstfh);

    if (rc < 0)             /* disk full? */
    {
        dos_delete(pdst_file);
        rc = FALSE;
    }

    return rc;
}


/*
*       Directory routine to DO an operation on an entire sub-directory.
*/
WORD d_doop(WORD level, WORD op, BYTE *psrc_path, BYTE *pdst_path,
            LONG tree, WORD *pfcnt, WORD *pdcnt)
{
    BYTE    *ptmp, *ptmpdst;
    DTA     *dta = &G.g_dtastk[level];
    WORD    more;

    dos_sdta(dta);

    for (dos_sfirst(psrc_path, ALLFILES); ; dos_snext())
    {
        more = TRUE;
        /*
         * handle end of folder
         */
        if (DOS_ERR && ((DOS_AX == E_NOFILES) || (DOS_AX == E_FILENOTFND)))
        {
            switch(op)
            {
            case OP_COUNT:
                G.g_ndirs++;
                break;
            case OP_DELETE:
                if (fold_wind(psrc_path))
                {
                    DOS_ERR = TRUE;       /* trying to delete  */
                    DOS_AX = E_NODELDIR;  /*  active directory */
                }
                else
                {
                    ptmp = last_separator(psrc_path);
                    *ptmp = NULL;
                    dos_rmdir(psrc_path);
                    strcpy(ptmp, "\\*.*");
                }
                more = d_errmsg();
                break;
            default:
                break;
            }
            if (tree)
            {
                inf_numset(tree, CDFOLDS, --*pdcnt);
                draw_fld(tree, CDFOLDS);
            }
            return more;
        }

        /*
         * return if real error
         */
        if (DOS_ERR)
            return d_errmsg();

        /*
         * handle folder
         */
        if (dta->d_attrib & F_SUBDIR)
        {
            if ((dta->d_fname[0] != '.') && (level < (MAX_LEVEL-1)))
            {
                add_path(psrc_path, dta->d_fname);
                if (op == OP_COPY)
                {
                    add_fname(pdst_path, dta->d_fname);
                    dos_mkdir(pdst_path);
                    if (DOS_ERR && (DOS_AX != E_NOACCESS))
                        more = d_errmsg();
                    strcat(pdst_path, "\\*.*");
                }
                if (more)
                {
                    more = d_doop(level+1,op,psrc_path,pdst_path,tree,pfcnt,pdcnt);
                    dos_sdta(dta);      /* must restore DTA address! */
                }
                sub_path(psrc_path);    /* restore the old paths */
                if (op == OP_COPY)
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
            ptmpdst = add_fname(pdst_path, dta->d_fname);
            more = d_dofcopy(psrc_path, pdst_path, dta->d_time,
                            dta->d_date, dta->d_attrib);
            restore_path(ptmpdst);  /* restore original dest path */
            break;
        }
        if (op != OP_COUNT)
            restore_path(ptmp);     /* restore original source path */
        if (tree)
        {
            inf_numset(tree, CDFILES, --*pfcnt);
            draw_fld(tree, CDFILES);
        }
        if (!more)
            break;
    }

    return more;
}


/*
*       return pointer to next folder in path.
*       start at the current position of the ptr.
*       assume path will eventually end with \*.*
*/
static BYTE *ret_path(BYTE *pcurr)
{
        REG BYTE        *path;
                                        /* find next level              */
        while( (*pcurr) &&
               (*pcurr != '\\') )
          pcurr++;
        pcurr++;
                                        /* get to current position      */
        path = pcurr;
                                        /* find end of curr level       */
        while( (*path) &&
               (*path != '\\') )
          path++;

        *path = NULL;
        return(pcurr);
} /* ret_path */


/*
*       Check to see if source is a parent of the destination.
*       Return TRUE if all ok else FALSE.
*       Must assume that src and dst paths both end with "\*.*".
*/
WORD par_chk(BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path)
{
        REG BYTE        *tsrc, *tdst;
        WORD    same;
        REG FNODE       *pf;
        BYTE            srcpth[MAXPATHLEN];
        BYTE            dstpth[MAXPATHLEN];

        if (psrc_path[0] != pdst_path[0])               /* check drives */
          return(TRUE);

        tsrc = srcpth;
        tdst = dstpth;
        same = TRUE;
        do
        {
                                                        /* new copies   */
          strcpy(srcpth, psrc_path);
          strcpy(dstpth, pdst_path);
                                                        /* get next paths*/
          tsrc = ret_path(tsrc);
          tdst = ret_path(tdst);
          if ( strcmp(tsrc, "*.*") )
          {
            if ( strcmp(tdst, "*.*") )
              same = strcmp(tdst, tsrc);
            else
              same = FALSE;
          }
          else
          {
                                                /* check to same level  */
            if ( !strcmp(tdst, "*.*") )
              same = FALSE;
            else
            {
                                                /* walk file list       */
              for(pf=pflist; pf; pf=pf->f_next)
              {
                                                /* exit if same subdir  */
                if ( (pf->f_obid != NIL) &&
                     (G.g_screen[pf->f_obid].ob_state & SELECTED) &&
                     (pf->f_attr & F_SUBDIR) &&
                     (!strcmp(&pf->f_name[0], tdst)) )
                {
                                                /* INVALID      */
                  fun_alert(1, STBADCOP, NULLPTR);
                  return(FALSE);
                }
              }
              same = FALSE;                     /* ALL OK               */
            }
          }
        } while(same);
        return(TRUE);
} /* par_chk */


/*
*       DIRectory routine that does an OPeration on all the selected files and
*       folders in the source path.  The selected files and folders are
*       marked in the source file list.
*/
WORD dir_op(WORD op, BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path,
            WORD *pfcnt, WORD *pdcnt, LONG *psize,
            WORD dulx, WORD duly, WORD from_disk, WORD src_ob)
{
        LONG            tree;
        FNODE           *pf;
        WORD            ret, more, confirm;
        BYTE            *ptmpsrc, *ptmpdst;
        LONG            lavail;
        BYTE            srcpth[MAXPATHLEN];
        BYTE            dstpth[MAXPATHLEN];
        OBJECT          *obj;

/* BugFix       */
        graf_mouse(HGLASS, 0x0L);

        if (op == OP_COUNT)
            tree = NULL;
        else
        {
            tree = G.a_trees[ADCPYDEL];
            obj = (OBJECT *)tree + CDTITLE;
        }

        ml_havebox = FALSE;
        confirm = 0;
        switch(op)
        {
          case OP_COUNT:
                G.g_nfiles = 0x0L;
                G.g_ndirs = 0x0L;
                G.g_size = 0x0L;
                break;
          case OP_DELETE:
                confirm = G.g_cdelepref;
                obj->ob_spec = (LONG) ini_str(STDELETE);
                break;
          case OP_COPY:
                lavail = dos_avail() - 0x400;   /* allow safety margin */
                if (lavail < 0L)
                    return FALSE;       /* TODO: alert for insufficient memory */
                /*
                 * for efficiency, the copy length should be a multiple of
                 * cluster size.  it's a lot of work to figure out the actual
                 * cluster sizes for the source and destination, but in most
                 * cases, available memory will be >=32K, the maximum possible
                 * cluster size.  in this case, we set 'copylen' to the largest
                 * multiple that fits in available memory.  if we have less
                 * than 32K available, we just set it as large as possible.
                 */
                if (lavail >= MAX_CLUS_SIZE)
                    copylen = lavail & ~(MAX_CLUS_SIZE-1);
                else copylen = lavail;
                copybuf = (UBYTE *)dos_alloc(copylen);

                confirm = G.g_ccopypref;
                obj->ob_spec = (LONG) ini_str(STCOPY);
                break;
        } /* switch */

        ret = TRUE;

        if (tree)
        {
          centre_title(tree);
          inf_numset(tree, CDFILES, *pfcnt);
          inf_numset(tree, CDFOLDS, *pdcnt);
          ml_havebox = TRUE;
          show_hide(FMD_START, tree);
          if (confirm)
          {
            graf_mouse(ARROW, NULL);
            form_do(tree, 0);
            graf_mouse(HGLASS, NULL);
            ret = inf_what(tree, CDOK, CDCNCL);
          }
        }

        more = ret;
        for (pf = pflist; pf && more; pf = pf->f_next)
        {
          if ( (pf->f_obid != NIL) &&
               (G.g_screen[pf->f_obid].ob_state & SELECTED))
          {
            strcpy(srcpth, psrc_path);
            if (op == OP_COPY)
            {
              strcpy(dstpth, pdst_path);
            } /* if OP_COPY */
            if (pf->f_attr & F_SUBDIR)
            {
              add_path(srcpth, &pf->f_name[0]);
              if (op == OP_COPY)
              {
                like_parent(dstpth, &pf->f_name[0]);
                dos_mkdir(dstpth);
                while (DOS_ERR && more)
                {
                                                /* see if dest folder   */
                                                /*   already exists     */
                  if (DOS_AX == E_NOACCESS)
                  {
                    if ( same_fold(srcpth, dstpth) )
                    {
                                                /* get the folder name  */
                                                /*   from the pathnames */
                      fmt_str(&pf->f_name[0], &ml_fsrc[0]);
                      ml_fdst[0] = NULL;
                                                /* put in folder name   */
                                                /*   in dialog          */
                      inf_sset(G.a_trees[ADCPALER], 2, &ml_fsrc[0]);
                      inf_sset(G.a_trees[ADCPALER], 3, &ml_fdst[0]);
                                                /* show dialog          */
                      do_namecon();
                                                /* if okay then make    */
                                                /*   dir or try again   */
                                                /*   until we succeed or*/
                                                /*   cancel is hit      */
                      more = inf_what(G.a_trees[ADCPALER],
                                        CAOK, CACNCL);

                      if (more)
                      {
                        inf_sget(G.a_trees[ADCPALER], 3, &ml_fdst[0]);
                        unfmt_str(&ml_fdst[0], &ml_fstr[0]);
                        del_fname(dstpth);
                        if (ml_fstr[0] != NULL)
                        {
                          add_fname(dstpth, &ml_fstr[0]);
                          dos_mkdir(dstpth);
                        } /* if */
                        else
                          more = FALSE;
                      } /* if more */
                    } /* if */
                    else
                      DOS_ERR = FALSE;
                  } /* if NOACCESS */
                  else
                    more = FALSE;
                } /* while */
                strcat(dstpth, "\\*.*");
              } /* if */
              if (more)
              {
                more = d_doop(0, op, srcpth, dstpth, tree, pfcnt, pdcnt);
              }
            } /* if SUBDIR */
            else
            {
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
                        ptmpdst = add_fname(dstpth, pf->f_name);
                        more = d_dofcopy(srcpth, dstpth, pf->f_time,
                                         pf->f_date, pf->f_attr);
                        restore_path(ptmpdst);  /* restore original dest path */
                        break;
              }
              if (op != OP_COUNT)
                restore_path(ptmpsrc);  /* restore original source path */
              if (tree)
              {
                *pfcnt -= 1;
                inf_numset(tree, CDFILES, *pfcnt);
                draw_fld(tree, CDFILES);
              } /* if tree */
            } /* else */
          } /* if */
        } /* for */

        switch(op)
        {
          case OP_COUNT:
                *pfcnt = G.g_nfiles;
                *pdcnt = G.g_ndirs;
                *psize = G.g_size;
                break;
          case OP_DELETE:
                break;
          case OP_COPY:
                dos_free((LONG)copybuf);
                break;
        } /* switch */
        if (ml_havebox)
          show_hide(FMD_FINISH, G.a_trees[ADCPALER]);
        graf_mouse(ARROW, 0x0L);
        return(TRUE);
} /* dir_op */
