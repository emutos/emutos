/*      DESKFUN.C       08/30/84 - 05/30/85             Lee Lorenzen    */
/*                      10/2/86  - 01/16/87             MDF             */
/*      merge source    5/27/87  - 5/28/87              mdf             */
/*      for 2.3         6/11/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
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

#include "config.h"
#include "portab.h"
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
#include "deskmain.h"
#include "deskglob.h"
#include "desksupp.h"
#include "deskdir.h"
#include "deskfun.h"

#include "string.h"




/*
 *  Routine to transfer a string that requires an integrated variable
 *  that is merged in.  The resultant alert is then displayed;
 */
WORD fun_alert(WORD defbut, WORD stnum, WORD *pwtemp)
{
        rsrc_gaddr(R_STRING, stnum, &G.a_alert);
        if (pwtemp != (WORD *)0)
        {
          strcpy(&G.g_2text[0], (char *)G.a_alert);
          sprintf(&G.g_1text[0], &G.g_2text[0], *pwtemp);
          G.a_alert = (LONG)&G.g_1text[0];
        }
        return( form_alert(defbut, G.a_alert) );
}


void fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7)
{
        /* keep DESKTOP messages internal to DESKTOP -- no AES call     */
        G.g_rmsg[0] = type;
        G.g_rmsg[1] = gl_apid;
        G.g_rmsg[2] = 0;
        G.g_rmsg[3] = w3;
        G.g_rmsg[4] = w4;
        G.g_rmsg[5] = w5;
        G.g_rmsg[6] = w6;
        G.g_rmsg[7] = w7;
        hndl_msg();
}


/*
*       Rebuild window path and pflist
*/
void fun_rebld(WNODE *pwin)
{
        WORD            i, x, y, w, h;
        BYTE            *ptst;

        graf_mouse(HGLASS, 0x0L);
                                                /* set up path to check */
                                                /*   against all windows*/
        ptst = &pwin->w_path->p_spec[0];
                                                /* check all wnodes     */
        for(i = NUM_WNODES; i; i--)
        {
          pwin = win_ith(i);
                                                /* if opened and same   */
                                                /*   path then rebuild  */
          if ( (pwin->w_id) && (strcmp(&pwin->w_path->p_spec[0], ptst)==0) )
          {
            pn_active(pwin->w_path);
            desk_verify(pwin->w_id, TRUE);
#ifdef DESK1
            win_sinfo(pwin);
            wind_set(pwin->w_id, WF_INFO, pwin->w_info, 0, 0);
#endif
            wind_get(pwin->w_id, WF_WXYWH, &x, &y, &w, &h);
            fun_msg(WM_REDRAW, pwin->w_id, x, y, w, h);
          } /* if */
        } /* for */
        graf_mouse(ARROW, 0x0L);
} /* fun_rebld */


/*
*       Routine that creates a new directory in the specified window/path
*/
WORD fun_mkdir(WNODE *pw_node)
{
        PNODE           *pp_node;
        LONG            tree;
        WORD            more, cont, i;
        BYTE            fnew_name[LEN_ZFNAME], unew_name[LEN_ZFNAME], *ptmp;
        BYTE            path[MAXPATHLEN];

        tree = G.a_trees[ADMKDBOX];
        pp_node = pw_node->w_path;
        ptmp = path;
        strcpy(ptmp, &pp_node->p_spec[0]);
        i = 0;
        while (*ptmp++)
        {
          if (*ptmp == '\\')
            i++;
        }
        if (i > MAX_LEVEL)
        {
          fun_alert(1, STFO8DEE, NULLPTR);
          return(FALSE);
        }
        cont = TRUE;
        while (cont)
        {
          fnew_name[0] = NULL;
          inf_sset(tree, MKNAME, &fnew_name[0]);
          show_hide(FMD_START, tree);
          form_do(tree, 0);
          cont = FALSE;
          if (inf_what(tree, MKOK, MKCNCL))
          {
            inf_sget(tree, MKNAME, &fnew_name[0]);
            unfmt_str(&fnew_name[0], &unew_name[0]);
            if ( unew_name[0] != NULL )
            {
              add_fname(path, unew_name);
              dos_mkdir(path);
              if (DOS_ERR)
              {
                if (strlen(path) >= LEN_ZPATH-3)
                  fun_alert(1,STDEEPPA,NULLPTR);
                else
                  cont = fun_alert(2,STFOEXIS,NULLPTR) - 1;
                del_fname(path);
              }
              else
              {
                more = d_errmsg();
                if (more)
                  fun_rebld(pw_node);
              } /* else */
            } /* if */
          } /* if OK */
        } /* while */
        show_hide(FMD_FINISH, tree);
        return(TRUE);
} /* fun_mkdir */



WORD fun_op(WORD op, PNODE *pspath, BYTE *pdest, WORD dulx, WORD duly,
            WORD from_disk, WORD src_ob)
{
        WORD            fcnt, dcnt;
        LONG            size;
                                                /* do the operation     */
        if (op != -1)
        {
          if (op == OP_COPY)
          {
            if ( !par_chk(&pspath->p_spec[0], pspath->p_flist,
                                &G.g_tmppth[0]) )
              return(FALSE);
          }
                                                /* get count of source  */
                                                /*   files              */
          dir_op(OP_COUNT, &pspath->p_spec[0], pspath->p_flist, pdest,
                 &fcnt, &dcnt, &size, dulx, duly, from_disk, src_ob);
                                                /* do the operation     */
          if ( fcnt || dcnt )
          {
            dir_op(op, &pspath->p_spec[0], pspath->p_flist, pdest,
                   &fcnt, &dcnt, &size, dulx, duly, from_disk, src_ob);
            return(TRUE);
          } /* if */
        } /* if */
        graf_mouse(ARROW, 0x0L);
        return(FALSE);
} /* fun_op */

#ifdef DESK1
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   D E S K 1   r o u t i n e s                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 *      Routine to call when several icons have been dragged from a
 *      window to another window (it might be the same window) and
 *      dropped on a particular icon or open space.
 *
 *      Note that, for DESK1, this is NEVER called if either the source
 *      or destination is the desktop.  Therefore 'datype' can ONLY be
 *      AT_ISFILE or AT_ISFOLD.
 */
void fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly)
{
        WORD            ret, junk, datype;
        WNODE           *psw, *pdw;
        ANODE           *pda;
        FNODE           *pdf;
        BYTE            destpath[MAXPATHLEN];

        psw = win_find(src_wh);
        pdw = win_find(dst_wh);

        pda = i_find(dst_wh, dst_ob, &pdf, &junk);
        datype = (pda) ? pda->a_type : AT_ISFILE;

        /* set up default destination path name */
        strcpy(destpath, pdw->w_path->p_spec);

        /* if destination is folder, insert folder name in path */
        if (datype == AT_ISFOLD)
          add_path(destpath, pdf->f_name);

        ret = fun_op(OP_COPY, psw->w_path, destpath, dulx, duly, FALSE, 0);

        if (ret)
        {
          if (src_wh != dst_wh)
            desk_clear(src_wh);
          fun_rebld(pdw);
          /*
           * if we copied into a folder, we must check to see if it's
           * open in a window and, if so, redraw it too
           */
          if (datype == AT_ISFOLD) {
            pdw = fold_wind(destpath);
            if (pdw)
              fun_rebld(pdw);
          }
        }
}

/*
 *      This routine is called when the 'Delete' menu item is selected
 */
void fun_del(WNODE *pdw)
{
        WORD        ret;

        ret = fun_op(OP_DELETE, pdw->w_path, NULL, 0, 0, 0, 0);
        if (ret)
/*        fun_rebld(pdw);*/
          do_chkall(TRUE);
}

#else
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   R o u t i n e s   f o r   t h e   c r i p p l e d   d e s k t o p   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static WORD cmp_names(BYTE *psrc, BYTE *pdst)
{
        WORD            ret;
        BYTE            *lastslsh, *ptmp;
                                                /* scan to lastslsh     */
        lastslsh = scan_slsh(psrc);
                                                /* null it              */
        *lastslsh = NULL;
                                                /* find trailing slash  */
        ptmp = pdst;
        while (*ptmp)
          ptmp++;
        ptmp--;
        *ptmp = NULL;                           /* null it              */
                                                /* see if they match    */
        ret = !strcmp(psrc, pdst);
                                                /* restore it           */
        *lastslsh = '\\';
        *ptmp = '\\';
                                                /* return if same       */
        return( ret );
} /* cmp_names */

/*
*       Routine to call when a list of files has been dragged on
*       top of a particular destination inside of a window.
*         pspath       source path
*         pdspec       destination file spec
*         datype       destination icon
*         pdf          destination file/fold
*         dulx, duly   destination x, y
*         from_disk    TRUE if src is disk
*         src_ob       source object
*         pdo_both     see AT_ISFOLD below
*/
static WORD
fun_wdst(PNODE *pspath, BYTE *pdspec, WORD datype, FNODE *pdf,
         WORD dulx, WORD duly, WORD from_disk, WORD src_ob, WORD *pdo_both)
{
        WORD            op;
        BYTE            *pdname, drv_ltr, *ptmp;
        ICONBLK         *spib;
        BYTE            *pwname1, *pwname2;
                                                /* default case: don't  */
        *pdo_both = FALSE;                      /* redraw both windows  */
                                                /* set up destination   */
                                                /*   path name          */
        drv_ltr = 'A';
/* BugFix       */
        if (G.g_iview == V_TEXT)
        {
          ptmp = &G.g_tmppth[0];                /* set in dr_fnode()    */
          while (*ptmp < 'A' - 1)               /* find drive letter    */
            ptmp++;
          drv_ltr = *ptmp;
        } /* if V_TEXT */
/* */
        strcpy(&G.g_tmppth[0], &pdspec[0]);
        pdname = &G.g_tmppth[0];
        while (*pdname != '*')
          pdname++;
        *pdname = NULL;
                                                /* check destination    */
        op = -1;
        switch( datype )
        {
          case AT_ISFOLD:
                                                /* if destination is    */
                                                /*   folder then append */
                                                /*   folder to window   */
                                                /*   path, if it is a   */
                                                /*   fake then treat it */
                                                /*   like open space    */
                if (pdf->f_attr & F_FAKE)
                  strcat(pdname, "*.*");
                else
                {
                  strcpy(pdname, &pdf->f_name[0]);
                  strcat(pdname, "\\*.*");
                }
                op = OP_COPY;
/* BugFix       */
                                /* check to see if this dest. folder    */
                                /*   is also an open window             */
                pwname1 = &G.g_wlist[0].w_name[0];
                pwname2 = &G.g_wlist[1].w_name[0];
                if ( cmp_names(&G.g_tmppth[0], pwname1) ||
                     cmp_names(&G.g_tmppth[0], pwname2) )
                  *pdo_both = TRUE;
/* */
                break;
          case AT_ISDISK:
                                                /* if destination is    */
                                                /*   disk then use      */
                                                /*   icon char. to build*/
                                                /*   dest path          */
                if (G.g_iview == V_TEXT)
                  G.g_tmppth[0] = drv_ltr;
                else
                {
                  spib = get_spec(G.g_screen, pdf->f_obid);
                  G.g_tmppth[0] = (0x00FF & spib->ib_char);
                } /* else */
                strcpy(&G.g_tmppth[1], ":\\*.*");
                op = OP_COPY;
                break;
          case AT_ISFILE:
                                                /* if destination is    */
                                                /*   window or a file   */
                                                /*   icon then use      */
                                                /*   window path        */
                strcat(pdname, "*.*");
                op = OP_COPY;
                break;
          case AT_ISTRSH:
                op = OP_DELETE;
                break;
        } /* switch */
        return( fun_op(op, pspath, &G.g_tmppth[0], dulx, duly,
                from_disk, src_ob) );
} /* fun_wdst */


/*
*       Routine to call when the source of a drag is a disk
*       and the destination is either a window or another
*       disk.
*/
static WORD fun_disk(WORD src_ob, WNODE *pdw, WORD datype, FNODE *pdf, WORD dulx, WORD duly)
{
        WORD            ret, do_both;
        FNODE           *pf;
        ICONBLK         *spib;
        PNODE           *pspath;
        BYTE            chr;
        union {
          LONG parm;
          BYTE *ptr;
        } u;

        ret = FALSE;                            /* just initialize */

        if (G.g_iview == V_TEXT) {
          u.parm = G.g_udefs[src_ob].ub_parm;   /* get ptr to user obj */
          chr = *u.ptr;                         /* get drive from user obj */
        }
        else
        {
          spib = get_spec(G.g_screen, src_ob);
          chr = spib->ib_char;
        } /* else */
                                                /* build a source path  */
        pspath = pn_open(chr, "", "*", "*", F_SUBDIR);
                                                /* if one available     */
        if (pspath)
        {
                                                /* read the directory   */
          ret = pn_active(pspath);
                                                /* if files to copy     */
          ret = FALSE;
          if (pspath->p_flist)
          {
                                                /* select all files     */
            for(pf = pspath->p_flist; pf; pf = pf->f_next)
              pf->f_obid = 0;
            G.g_screen[0].ob_state = SELECTED;
                                                /* do the copy or delete*/
            if (pdw != (WNODE *) 0)
              ret = fun_wdst(pspath, &pdw->w_path->p_spec[0], datype,
                             pdf, dulx, duly, TRUE, src_ob, &do_both);
            else
              ret = fun_op(OP_DELETE, pspath, &G.g_tmppth[0],
                           dulx, duly, TRUE, src_ob);
                                                /* return to normalcy   */
            G.g_screen[0].ob_state = NORMAL;
          } /* if */
          pn_close(pspath);
                /* rebuild any windows with dpib.ib_char in title       */
          desk_clear(0);
        } /* if pspath */
        return(ret);
} /* fun_disk */



/*
*       Routine to call when several icons have been dragged from a
*       window to another window (it might be the same window) and
*       dropped on a particular icon or open space.
*/
void fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly)
{
        WORD            ret, junk, datype, src_ob, do_both;
        WNODE           *psw, *pdw;
        ANODE           *pda;
        FNODE           *pdf;

        psw = win_find(src_wh);
        pdw = win_find(dst_wh);
        if (pdw->w_path->p_spec[0] == '@' && dst_ob <= 3)
          return;

        pda = i_find(dst_wh, dst_ob, &pdf, &junk);
        datype = (pda) ? pda->a_type : AT_ISFILE;

        if (psw->w_path->p_spec[0] != '@')
        {
          ret = fun_wdst(psw->w_path, &pdw->w_path->p_spec[0],
                         datype, pdf, dulx, duly, FALSE, 0, &do_both);
          if (ret)
          {
            if (src_wh != dst_wh)
              desk_clear(src_wh);
/* BugFix       */
            if (do_both)                /* force redraw of both windows */
            {
              fun_rebld(&G.g_wlist[0]);
              fun_rebld(&G.g_wlist[1]);
            }
/* */
            else
              fun_rebld(pdw);
          } /* if ret */
        } /* if */
        else
        {
          src_ob = 0;
          while ( (src_ob = win_isel(G.g_screen, G.g_croot, src_ob)) != 0 )
          {
            ret = fun_disk(src_ob, pdw, datype, pdf, dulx, duly);
            if (ret)
              fun_rebld(pdw);
          } /* while */
        } /* else */
} /* fun_drag */

/*
*       Routine to call when several icons have been dragged from a
*       desktop to the desktop and dropped on a particular icon.
*/
void fun_del(WNODE *pdw)
{
        WORD        src_ob, ret;
        ICONBLK     *spib;
        BYTE        *ptmp;
        WORD        drive_letter;

        if (pdw->w_path->p_spec[0] != '@')
        {
          ret = fun_op(OP_DELETE, pdw->w_path, &G.g_tmppth[0], 0, 0, 0, 0);
          if (ret)
/*          fun_rebld(pdw);*/
            do_chkall(TRUE);
        }
        else
        {
          src_ob = 0;
          while ( (src_ob = win_isel(G.g_screen, G.g_croot, src_ob)) != 0 )
          {
            ret = 1;
/* BugFix       */
            if (G.g_iview == V_TEXT)
            {
              ptmp = &G.g_tmppth[0];
              while (*ptmp < 0x40)
                ptmp++;
              drive_letter = (UWORD)*ptmp;
            } /* if V_TEXT */
/* */
            else
            {
              spib = get_spec(G.g_screen, src_ob);
              drive_letter = (0x00FF & spib->ib_char);
            } /* else */

            graf_mouse(ARROW, 0x0L);
            ret = fun_alert(2, STDELDIS, &drive_letter);
            graf_mouse(HGLASS, 0x0L);
            if (ret == 1)
            {
              fun_disk(src_ob, NULLPTR, AT_ISTRSH, NULLPTR, 0, 0);
              do_chkall(TRUE);
            }
          } /* while */
          graf_mouse(ARROW, 0x0L);
        } /* else */
} /* fun_del */

#endif
