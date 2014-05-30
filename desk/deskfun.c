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
            win_sinfo(pwin);
            wind_set(pwin->w_id, WF_INFO, pwin->w_info, 0, 0);
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
              ptmp = add_fname(path, unew_name);
              dos_mkdir(path);
              if (DOS_ERR)
              {
                if (strlen(path) >= LEN_ZPATH-3)
                  fun_alert(1,STDEEPPA,NULLPTR);
                else
                  cont = fun_alert(2,STFOEXIS,NULLPTR) - 1;
                restore_path(ptmp); /* restore original path */
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



WORD fun_op(WORD op, PNODE *pspath, BYTE *pdest)
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
                 &fcnt, &dcnt, &size);
                                                /* do the operation     */
          if ( fcnt || dcnt )
          {
            dir_op(op, &pspath->p_spec[0], pspath->p_flist, pdest,
                   &fcnt, &dcnt, &size);
            return(TRUE);
          } /* if */
        } /* if */
        graf_mouse(ARROW, 0x0L);
        return(FALSE);
} /* fun_op */

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

        ret = fun_op(OP_COPY, psw->w_path, destpath);

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

        ret = fun_op(OP_DELETE, pdw->w_path, NULL);
        if (ret)
/*        fun_rebld(pdw);*/
          do_chkall(TRUE);
}
