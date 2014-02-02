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
#include "compat.h"
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


#define S_FILL_STYLE            23              /* used in blank_it()   */
#define S_FILL_INDEX            24
#define S_FILL_COLOR            25
#define SET_WRITING_MODE        32
#define abs(x) ( (x) < 0 ? -(x) : (x) )
#define MAX_TWIDTH 45                           /* used in blank_it()   */

#define MAX_CLUS_SIZE   (32*1024L)  /* maximum cluster size */

static UBYTE    *copybuf;   /* for copy operations */
static LONG     copylen;    /* size of above buffer */

static BYTE     ml_files[4], ml_dirs[4];
static WORD     ml_dlpr, ml_havebox;
static BYTE     ml_fsrc[LEN_ZFNAME], ml_fdst[LEN_ZFNAME], ml_fstr[LEN_ZFNAME], ml_ftmp[LEN_ZFNAME];




/************************************************************************/
/* b l a n k _ i t                                                      */
/************************************************************************/
static void blank_it(WORD obid)
{
/* blit white over just-deleted icon                                    */
        WORD            blt_x, blt_y, blt_w, blt_h, pxy[4];
        GRECT           clipr;
        ICONBLK         *piblk;
        FDB             dst;

        graf_mouse(M_OFF, 0x0L);
        wind_get(G.g_wlastsel, WF_WXYWH, &clipr.g_x, &clipr.g_y,
                 &clipr.g_w, &clipr.g_h);
        gsx_sclip(&clipr);
        objc_offset(G.g_screen, obid, &blt_x, &blt_y);
        if (G.g_iview == V_ICON)
        {
          piblk = get_spec(G.g_screen, obid);
          blt_x += piblk->ib_xtext;
          blt_y += piblk->ib_yicon;
          blt_w = piblk->ib_wtext;
          blt_h = piblk->ib_hicon + piblk->ib_htext;
        } /* if V_ICON */
        else                                    /* view is V_TEXT       */
        {
          blt_w = gl_wchar * MAX_TWIDTH;
          blt_h = gl_hchar + 1;
        } /* else */
        gsx_1code(SET_WRITING_MODE, MD_REPLACE);
        gsx_1code(S_FILL_STYLE, FIS_SOLID);
        gsx_1code(S_FILL_INDEX, IP_SOLID);
        gsx_1code(S_FILL_COLOR, WHITE);
        gsx_fix(&dst, 0x0L, 0, 0);
        pxy[0] = blt_x;
        pxy[1] = blt_y;
        pxy[2] = blt_x + blt_w - 1;
        pxy[3] = blt_y + blt_h - 1;
        vr_recfl( &pxy[0], &dst );
        gsx_1code(S_FILL_COLOR, BLACK);
        gsx_1code(SET_WRITING_MODE, MD_XOR);
        gsx_attr(FALSE, MD_XOR, BLACK);
        graf_mouse(M_ON, 0x0L);
} /* blank_it */


/************************************************************************/
/* m o v e _ i c o n                                                    */
/************************************************************************/
static void move_icon(WORD obj, WORD dulx, WORD duly)
{
/* animate an icon moving from its place on the desktop to dulx,duly    */
        WORD            sulx, suly, w, h;

        objc_offset(G.g_screen, obj, &sulx, &suly);
        if (G.g_iview == V_ICON)
        {
          w = G.g_wicon;
          h = G.g_hicon;
        } /* if V_ICON */
        else                                    /* view must be V_TEXT  */
        {
          w = gl_wchar * MAX_TWIDTH;
          h = gl_hchar;
        } /* else */
        graf_mbox(w, h, sulx, suly, dulx, duly);
} /* move_icon */




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
        if (ml_dlpr)
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


BYTE *scan_slsh(BYTE *path)
{
                                                /* scan to first '*'    */
        while (*path != '*')
          path++;
                                                /* back up to last slash*/
        while (*path != '\\')
          path--;
        return(path);
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
        path = scan_slsh(path);
                                                /* now skip to previous */
                                                /*   directroy in path  */
        path--;
        while (*path != '\\')
          path--;
                                                /* append a *.*         */
        strcpy(path, "\\*.*");
} /* sub_path */


/*
*       Add a file name to the end of an existing path.
*/
void add_fname(BYTE *path, BYTE *new_name)
{
        while (*path != '*')
          path++;

        strcpy(path, new_name);
} /* add_fname */



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
        lastslsh = path = scan_slsh(path);
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
        lastslsh = scan_slsh(psrc);
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
        while (*pstr)
          pstr++;
        while (*pstr != '\\')
          pstr--;
        strcpy(pstr, "\\*.*");
} /* sub_path */


/*
*       Parse to find the filename part of a path and return a copy of it
*       in a form ready to be placed in a dialog box.
*/
static void get_fname(BYTE *pstr, BYTE *newstr)
{
        while (*pstr)
          pstr++;
        while(*pstr != '\\')
          pstr--;
        pstr++;
        strcpy(&ml_ftmp[0], pstr);
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
*       Directory routine to DO File COPYing.
*/
static WORD d_dofcopy(BYTE *psrc_file, BYTE *pdst_file, WORD time, WORD date, WORD attr)
{
        LONG            tree;
        WORD            srcfh, dstfh;
        LONG            amntrd, amntwr;
        WORD            copy, cont, more, samedir, ob;

        copy = TRUE;
                                                /* open the source file */
        srcfh = dos_open(psrc_file, 0);
        more = d_errmsg();
        if (!more)
          return(more);
                                                /* open the dest file   */
        cont = TRUE;
        while (cont)
        {
          copy = FALSE;
          more = TRUE;
          dstfh = dos_open(pdst_file, 0);
                                                /* handle dos error     */
          if (DOS_ERR)
          {
            if (DOS_AX == E_FILENOTFND)
              copy = TRUE;
            else
              more = d_errmsg();
            cont = FALSE;
          }
          else
          {
                                                /* dest file already    */
                                                /*   exists             */
            dos_close(dstfh);
                                                /* get the filenames    */
                                                /*   from the pathnames */
            get_fname(psrc_file, &ml_fsrc[0]);
                                                /* if same dir, then    */
                                                /*   don't prefill the  */
                                                /*   new name string    */
            samedir = !strcmp(psrc_file, pdst_file);
            if (samedir)
              ml_fdst[0] = NULL;
            else
              get_fname(pdst_file, &ml_fdst[0]);
                                                /* put in filenames     */
                                                /*   in dialog          */
            inf_sset(G.a_trees[ADCPALER], 2, &ml_fsrc[0]);
            inf_sset(G.a_trees[ADCPALER], 3, &ml_fdst[0]);
                                                /* show dialog          */
            if ((G.g_covwrpref) || (samedir))
            {
              do_namecon();
                                                /* if okay then if its  */
                                                /*   the same name then */
                                                /*   overwrite else get */
                                                /*   new name and go    */
                                                /*   around to check it */


              tree = G.a_trees[ADCPALER];
              ob = inf_gindex(G.a_trees[ADCPALER], CAOK, 3) + CAOK;
              ((OBJECT *)tree+ob)->ob_state = NORMAL;
              if (ob == CASTOP)
                copy = more = FALSE;
              else if (ob == CACNCL)
                copy = FALSE;
              else
                copy = TRUE;
            }
            else
              copy = TRUE;
/* */
            if (copy)
            {
              cont = FALSE;
              inf_sget(G.a_trees[ADCPALER], 3, &ml_fdst[0]);
              unfmt_str(&ml_fdst[0], &ml_fstr[0]);
              if ( ml_fstr[0] == NULL )
              {
                copy = FALSE;
                dos_close(srcfh);
              }
              else
              {
                del_fname(pdst_file);
                add_fname(pdst_file, &ml_fstr[0]);
              }
            }
            else
            {
              dos_close(srcfh);
              cont = copy = FALSE;
            }
          } /* else */
        } /* while cont */

        if ( copy && more )
          dstfh = dos_create(pdst_file, attr);

        amntrd = copy;
        while( amntrd && more )
        {
          more = d_errmsg();
          if (more)
          {
            amntrd = dos_read(srcfh, copylen, copybuf);
            more = d_errmsg();
            if (more)
            {
              if (amntrd)
              {
                amntwr = dos_write(dstfh, amntrd, copybuf);
                more = d_errmsg();
                if (more)
                {
                  if (amntrd != amntwr)
                  {
                                                /* disk full            */
                    graf_mouse(ARROW, 0x0L);
                    fun_alert(1, STDISKFU, NULLPTR);
                    graf_mouse(HGLASS, NULL);
                    more = FALSE;
                    dos_close(srcfh);
                    dos_close(dstfh);
                    dos_delete(pdst_file);
                  } /* if */
                } /* if more */
              } /* if amntrd */
            } /* if more */
          } /* if more */
        } /* while */
        if (copy && more)
        {
          dos_setdt(dstfh, time, date);
          more = d_errmsg();
          dos_close(srcfh);
          dos_close(dstfh);
        }
/*      graf_mouse(ARROW, 0x0L);*/
        return(more);
} /* d_dofcopy */


/*
*       Directory routine to DO an operation on an entire sub-directory.
*/
WORD d_doop(WORD op, LONG tree, WORD obj, BYTE *psrc_path, BYTE *pdst_path,
            WORD *pfcnt, WORD *pdcnt, WORD flag)
{
        BYTE            *ptmp;
        WORD            cont, skip, more, level;
                                                /* start recursion at   */
                                                /*   level 0            */
        level = 0;
                                                /* set up initial DTA   */
        dos_sdta(&G.g_dtastk[level]);
        dos_sfirst(psrc_path, 0x16);

        cont = more = TRUE;
        while (cont && more)
        {
          skip = FALSE;
          if (DOS_ERR)
          {
                                                /* no more files error  */
            if ( (DOS_AX == E_NOFILES) || (DOS_AX == E_FILENOTFND) )
            {
              switch(op)
              {
                case OP_COUNT:
                        G.g_ndirs++;
                        break;
                case OP_DELETE:
                        if (fold_wind(psrc_path))
                        {
                          DOS_ERR = TRUE;       /* trying to delete     */
                          DOS_AX = 16;          /* active directory     */
                        }
                        else
                        {
                          ptmp = psrc_path;
                          while(*ptmp != '*')
                            ptmp++;
                          ptmp--;
                          *ptmp = NULL;
                          dos_rmdir(psrc_path);
                        }
                        more = d_errmsg();
                        strcat(psrc_path, "\\*.*");
                        break;
                case OP_COPY:
                        break;
              }
              if (tree)
              {
                *pdcnt -= 1;
                sprintf(&ml_dirs[0], "%d", *pdcnt);
                inf_sset(tree, CDFOLDS, &ml_dirs[0]);
                draw_fld(tree, CDFOLDS);
              }
              skip = TRUE;
              level--;
              if (level < 0)
                cont = FALSE;
              else
              {
                sub_path(psrc_path);
                if (op == OP_COPY)
                  sub_path(pdst_path);
                dos_sdta(&G.g_dtastk[level]);
              }
            } /* if no more files */
            else
              more = d_errmsg();
          }
          if ( !skip && more )
          {
            if ( G.g_dtastk[level].d_attr & F_SUBDIR )
            {                                   /* step down 1 level    */
              if ( (G.g_dtastk[level].d_name[0] != '.') &&
                   (level < (MAX_LEVEL-1)) )
              {
                                                /* change path name     */
                add_path(psrc_path, &G.g_dtastk[level].d_name[0]);
                if (op == OP_COPY)
                {
                  add_fname(pdst_path, &G.g_dtastk[level].d_name[0]);
                  dos_mkdir(pdst_path);
                  if ( (DOS_ERR) && (DOS_AX != E_NOACCESS) )
                    more = d_errmsg();
                  strcat(pdst_path, "\\*.*");
                }
                level++;
                dos_sdta(&G.g_dtastk[level]);
                if (more)
                  dos_sfirst(psrc_path, 0x16);
              } /* if not a dir */
            } /* if */
            else
            {
              if (op)
                add_fname(psrc_path, &G.g_dtastk[level].d_name[0]);
              switch(op)
              {
                case OP_COUNT:
                        G.g_nfiles++;
                        G.g_size += G.g_dtastk[level].d_size;
                        break;
                case OP_DELETE:
                        more = d_dofdel(psrc_path);
                        break;
                case OP_COPY:
                        add_fname(pdst_path, &G.g_dtastk[level].d_name[0]);
                        more = d_dofcopy(psrc_path, pdst_path,
                                G.g_dtastk[level].d_time,
                                G.g_dtastk[level].d_date,
                                G.g_dtastk[level].d_attr);
                        del_fname(pdst_path);
                        break;
              }
              if (op)
                del_fname(psrc_path);
              if (tree)
              {
                *pfcnt -= 1;
                sprintf(&ml_files[0], "%d", *pfcnt);
                inf_sset(tree, CDFILES, &ml_files[0]);
                draw_fld(tree, CDFILES);
              }
            }
          }
          if (cont)
            dos_snext();
        }
        if (op == OP_DELETE && !flag)
          blank_it(obj);
        return(more);
} /* d_doop */


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
        BYTE            srcpth[LEN_ZPATH+LEN_ZFNAME+1];
        BYTE            dstpth[LEN_ZPATH+LEN_ZFNAME+1];

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
#ifndef DESK1
                     !(pf->f_attr & F_FAKE) &&
#endif
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
        WORD            ret, more, ob;
        BYTE            *pglsrc, *pgldst;
        LONG            lavail;
        BYTE            srcpth[LEN_ZPATH+LEN_ZFNAME+1];
        BYTE            dstpth[LEN_ZPATH+LEN_ZFNAME+1];
        OBJECT          *obj;

/* BugFix       */
        graf_mouse(HGLASS, 0x0L);
        pglsrc = srcpth;
        pgldst = dstpth;
        tree = 0x0L;
        ml_havebox = FALSE;
        switch(op)
        {
          case OP_COUNT:
                G.g_nfiles = 0x0L;
                G.g_ndirs = 0x0L;
                G.g_size = 0x0L;
                break;
          case OP_DELETE:
                ml_dlpr = G.g_cdelepref;
                if (ml_dlpr)
                {
                  tree = G.a_trees[ADCPYDEL];
                  obj = (OBJECT *)tree + CDTITLE;
                  obj->ob_spec = (LONG) ini_str(STDELETE);
                }
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

                ml_dlpr = G.g_ccopypref;
                if (ml_dlpr)
                {
                  tree = G.a_trees[ADCPYDEL];
                  obj = (OBJECT *)tree + CDTITLE;
                  obj->ob_spec = (LONG) ini_str(STCOPY);
                }
                break;
        } /* switch */

        if (tree)
        {
          centre_title(tree);
          sprintf(&ml_files[0], "%d", *pfcnt);
          inf_sset(tree, CDFILES, &ml_files[0]);
          sprintf(&ml_dirs[0], "%d", *pdcnt);
          inf_sset(tree, CDFOLDS, &ml_dirs[0]);
          ml_havebox = TRUE;
          show_hide(FMD_START, tree);
          graf_mouse(ARROW, 0x0L);
          form_do(tree, 0);
          graf_mouse(HGLASS, 0x0L);
          ret = inf_what(tree, CDOK, CDCNCL);
        }
        else
          ret = TRUE;

        more = ret;
        for (pf = pflist; pf && more; pf = pf->f_next)
        {
          if ( (pf->f_obid != NIL) &&
#ifndef DESK1
               !(pf->f_attr & F_FAKE) &&
#endif
               (G.g_screen[pf->f_obid].ob_state & SELECTED))
          {
            strcpy(pglsrc, psrc_path);
            if (op == OP_COPY)
            {
              strcpy(pgldst, pdst_path);
              if (!ml_dlpr)             /* show the moving icon!        */
              {
                if (from_disk)
                  ob = src_ob;
                else
                  ob = pf->f_obid;
                move_icon(ob, dulx, duly);
              } /* if */
            } /* if OP_COPY */
            if (pf->f_attr & F_SUBDIR)
            {
              add_path(pglsrc, &pf->f_name[0]);
              if (op == OP_COPY)
              {
                like_parent(pgldst, &pf->f_name[0]);
                dos_mkdir(pgldst);
                while (DOS_ERR && more)
                {
                                                /* see if dest folder   */
                                                /*   already exists     */
                  if (DOS_AX == E_NOACCESS)
                  {
                    if ( same_fold(pglsrc, pgldst) )
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
                        del_fname(pgldst);
                        if (ml_fstr[0] != NULL)
                        {
                          add_fname(pgldst, &ml_fstr[0]);
                          dos_mkdir(pgldst);
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
                strcat(pgldst, "\\*.*");
              } /* if */
              if (more)
                more = d_doop(op, tree, pf->f_obid, pglsrc, pgldst,
                              pfcnt, pdcnt, ml_dlpr);
            } /* if SUBDIR */
            else
            {
              if (op)
                add_fname(pglsrc, &pf->f_name[0]);
              switch(op)
              {
                    case OP_COUNT:
                        G.g_nfiles++;
                        G.g_size += pf->f_size;
                        break;
                    case OP_DELETE:
                        more = d_dofdel(pglsrc);
                        if (!ml_dlpr)
                          blank_it(pf->f_obid);
                        break;
                    case OP_COPY:
                        add_fname(pgldst, &pf->f_name[0]);
                        more = d_dofcopy(pglsrc, pgldst, pf->f_time,
                                         pf->f_date, pf->f_attr);
                        del_fname(pgldst);
                        break;
              }
              if (op)
                del_fname(psrc_path);
              if (tree)
              {
                *pfcnt -= 1;
                sprintf(&ml_files[0], "%d", *pfcnt);
                inf_sset(tree, CDFILES, &ml_files[0]);
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
