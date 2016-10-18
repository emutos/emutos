/*      DESKFUN.C       08/30/84 - 05/30/85             Lee Lorenzen    */
/*                      10/2/86  - 01/16/87             MDF             */
/*      merge source    5/27/87  - 5/28/87              mdf             */
/*      for 2.3         6/11/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
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
#include "obdefs.h"
#include "dos.h"
#include "gemdos.h"
#include "optimize.h"

#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "deskbind.h"

#include "aesbind.h"
#include "deskmain.h"
#include "deskglob.h"
#include "desksupp.h"
#include "deskdir.h"
#include "deskfun.h"
#include "biosdefs.h"

#include "string.h"
#include "gemerror.h"
#include "kprint.h"



/*
 *  Issue an alert
 */
WORD fun_alert(WORD defbut, WORD stnum)
{
    rsrc_gaddr(R_STRING, stnum, (void **)&G.a_alert);
    return form_alert(defbut, G.a_alert);
}


/*
 *  Issue an alert after merging in an optional character variable
 */
WORD fun_alert_merge(WORD defbut, WORD stnum, BYTE merge)
{
    rsrc_gaddr(R_STRING, stnum, (void **)&G.a_alert);
    strcpy(G.g_2text, G.a_alert);
    sprintf(G.g_1text, G.g_2text, merge);
    G.a_alert = G.g_1text;

    return form_alert(defbut, G.a_alert);
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
 *  Rebuild window path and pflist
 */
void fun_rebld(WNODE *pwin)
{
    GRECT gr;
    BYTE *ptst;

    graf_mouse(HGLASS, NULL);

    /* set up path to check against all windows*/
    ptst = pwin->w_path->p_spec;

    /* check all wnodes     */
    for (pwin = G.g_wfirst; pwin; pwin = pwin->w_next)
    {
        /* if opened and same path then rebuild */
        if ( (pwin->w_id) && (strcmp(pwin->w_path->p_spec, ptst)==0) )
        {
            pn_active(pwin->w_path);
            desk_verify(pwin->w_id, TRUE);
            win_sinfo(pwin);
            wind_set(pwin->w_id, WF_INFO, pwin->w_info, 0, 0);
            wind_get_grect(pwin->w_id, WF_WXYWH, &gr);
            fun_msg(WM_REDRAW, pwin->w_id, gr.g_x, gr.g_y, gr.g_w, gr.g_h);
        } /* if */
    } /* for */

    graf_mouse(ARROW, NULL);
} /* fun_rebld */


/*
 *  Routine that creates a new directory in the specified window/path
 */
WORD fun_mkdir(WNODE *pw_node)
{
    PNODE *pp_node;
    OBJECT *tree;
    WORD  i, len, err;
    BYTE  fnew_name[LEN_ZFNAME], unew_name[LEN_ZFNAME], *ptmp;
    BYTE  path[MAXPATHLEN];

    tree = (OBJECT *)G.a_trees[ADMKDBOX];
    pp_node = pw_node->w_path;
    ptmp = path;
    strcpy(ptmp, pp_node->p_spec);

    i = 0;
    while (*ptmp++)
    {
        if (*ptmp == '\\')
            i++;
    }

    if (i > MAX_LEVEL)
    {
        fun_alert(1, STFO8DEE);
        return FALSE;
    }

    while(1)
    {
        fnew_name[0] = '\0';
        inf_sset(tree, MKNAME, fnew_name);
        show_hide(FMD_START, tree);
        form_do(tree, 0);
        if (inf_what(tree, MKOK, MKCNCL) == 0)
            break;

        inf_sget(tree, MKNAME, fnew_name);
        unfmt_str(fnew_name, unew_name);

        if (unew_name[0] == '\0')
            break;

        ptmp = add_fname(path, unew_name);
        err = dos_mkdir(path);
        if (err == 0)       /* mkdir succeeded */
        {
            fun_rebld(pw_node);
            break;
        }

        /*
         * if we're getting a BIOS (rather than GEMDOS) error, the
         * critical error handler has already issued a message, so
         * just quit
         */
        if (IS_BIOS_ERROR(err))
            break;

        len = strlen(path); /* before we restore old path */
        restore_path(ptmp); /* restore original path */
        if (len >= LEN_ZPATH-3)
        {
            fun_alert(1,STDEEPPA);
            break;
        }

        /*
         * mkdir failed with a recoverable error:
         * prompt for Cancel or Retry
         */
        if (fun_alert(2,STFOFAIL) == 1)     /* Cancel */
            break;
    }

    show_hide(FMD_FINISH, tree);
    return TRUE;
}


/*
 *  return pointer to next folder in path.
 *  start at the current position of the ptr.
 *  assume path will eventually end with \*.*
 */
static BYTE *ret_path(BYTE *pcurr)
{
    BYTE *path;

    /* find next level */
    while( (*pcurr) && (*pcurr != '\\') )
        pcurr++;
    pcurr++;

    /* get to current position */
    path = pcurr;

    /* find end of curr level */
    while( (*path) && (*path != '\\') )
        path++;

    *path = '\0';
    return(pcurr);
} /* ret_path */


/*
 *  Check to see if source is a parent of the destination.
 *  If it is, issue alert & return TRUE; otherwise just return FALSE.
 *  Must assume that src and dst paths both end with "\*.*".
 */
static WORD source_is_parent(BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path)
{
    BYTE *tsrc, *tdst;
    WORD same;
    FNODE *pf;
    BYTE srcpth[MAXPATHLEN];
    BYTE dstpth[MAXPATHLEN];

    if (psrc_path[0] != pdst_path[0])   /* check drives */
        return FALSE;

    tsrc = srcpth;
    tdst = dstpth;
    same = TRUE;
    do
    {
        /* new copies */
        strcpy(srcpth, psrc_path);
        strcpy(dstpth, pdst_path);

        /* get next paths */
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
            /* check to same level */
            if ( !strcmp(tdst, "*.*") )
                same = FALSE;
            else
            {
                /* walk file list */
                for (pf = pflist; pf; pf = pf->f_next)
                {
                    /* exit if same subdir  */
                    if ( (pf->f_obid != NIL) &&
                        (G.g_screen[pf->f_obid].ob_state & SELECTED) &&
                        (pf->f_attr & F_SUBDIR) &&
                        (!strcmp(pf->f_name, tdst)) )
                    {
                        /* INVALID      */
                        fun_alert(1, STBADCOP);
                        return TRUE;
                    }
                }
                same = FALSE;   /* ALL OK */
            }
        }
    } while(same);

    return FALSE;
}


/*
 *  Perform the operation 'op' on all the files & folders in the
 *  path associated with 'pspath'.  'op' can be OP_DELETE, OP_COPY,
 *  OP_MOVE.
 */
WORD fun_op(WORD op, WORD icontype, PNODE *pspath, BYTE *pdest)
{
    DIRCOUNT count;

    switch(op)
    {
    case OP_COPY:
    case OP_MOVE:
        if (source_is_parent(pspath->p_spec, pspath->p_flist, pdest))
            return FALSE;
        /* drop thru */
    case OP_DELETE:
        dir_op(OP_COUNT, icontype, pspath, pdest, &count);  /* get count of source files */
        if ((count.files+count.dirs) == 0)
            break;
        dir_op(op, icontype, pspath, pdest, &count);        /* do the operation     */
        return TRUE;
    }

    return FALSE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   D E S K 1   r o u t i n e s                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 *  Routine to call when several icons have been dragged from a
 *  window to another window (it might be the same window) and
 *  dropped on a particular icon or open space.
 *
 *  Note that, for DESK1, this is NEVER called if either the source
 *  or destination is the desktop.  Therefore 'datype' can ONLY be
 *  AT_ISFILE or AT_ISFOLD.
 */
void fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly, WORD keystate)
{
    WORD  ret, datype, op;
    WNODE *psw, *pdw;
    ANODE *pda;
    FNODE *pdf;
    BYTE  destpath[MAXPATHLEN];

    op = (keystate&MODE_CTRL) ? OP_MOVE : OP_COPY;
    psw = win_find(src_wh);
    if (!psw)
        return;
    pdw = win_find(dst_wh);
    if (!pdw)
        return;

    pda = i_find(dst_wh, dst_ob, &pdf, NULL);
    datype = (pda) ? pda->a_type : AT_ISFILE;

    /* set up default destination path name */
    strcpy(destpath, pdw->w_path->p_spec);

    /* if destination is folder, insert folder name in path */
    if (datype == AT_ISFOLD)
        add_path(destpath, pdf->f_name);

    ret = fun_op(op, -1, psw->w_path, destpath);

    if (ret)
    {
        if (src_wh != dst_wh)
            desk_clear(src_wh);
        if (op == OP_MOVE)
            fun_rebld(psw);
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
 * Function called to redisplay appropriate windows when disk is deleted
 */
static void do_refresh_drive(WORD drive)
{
    WNODE *pw;

    for (pw = G.g_wfirst; pw; pw = pw->w_next)
    {
        if (pw->w_id)
            if (pw->w_path->p_spec[0] == drive)
                do_refresh(pw);
    }
}

/*
 * Function called to delete the contents of a disk
 */
static WORD delete_disk(ANODE *pa)
{
    PNODE *pn;
    FNODE *fn;
    BYTE path[10];
    WORD ret = 0;

    build_root_path(path, pa->a_letter);
    strcat(path,"*.*");
    pn = pn_open(path, F_SUBDIR);
    if (pn == NULL)
        return 0;

    graf_mouse(HGLASS, NULL);
    pn_active(pn);
    if (pn->p_flist)
    {
        /*
         * point all the FNODEs to the root, then set the root's
         * SELECTED attribute; this is a cheap way of making dir_op()
         * (called by fun_op()) think all the files are selected
         */
        for (fn = pn->p_flist; fn; fn = fn->f_next)
            fn->f_obid = 0;
        G.g_screen->ob_state = SELECTED;
        ret = fun_op(OP_DELETE, pa->a_type, pn, NULL);
        G.g_screen->ob_state = 0;   /* reset for safety */
    }
    pn_close(pn);
    graf_mouse(ARROW, NULL);

    return ret;
}

/*
 *  This routine is called when the 'Delete' menu item is selected
 */
void fun_del(WORD sobj)
{
    ANODE *pa;
    WNODE *pw;
    WORD disk_found = 0;

    /*
     * if the item selected is on the desktop, there may be other desktop
     * items that have been selected; make sure we process all of them
     */
    if ( (pa = i_find(0, sobj, NULL, NULL)) )
    {
        if (wants_to_delete_files() == FALSE)   /* i.e. remove icons or cancel */
            return;
        for ( ; sobj; sobj = win_isel(G.g_screen, DROOT, sobj))
        {
            pa = i_find(0,sobj,NULL,NULL);
            if (!pa)
                continue;
            if (pa->a_type == AT_ISDISK)
            {
                disk_found++;
                if (delete_disk(pa))
                    do_refresh_drive(pa->a_letter);
            }
        }
        if (disk_found)
        {
            desk_clear(0);
            return;
        }
    }

    /*
     * otherwise, process path associated with selected window icon, if any
     */
    pw = win_find(G.g_cwin);

    if (pw)
    {
        if (fun_op(OP_DELETE, -1, pw->w_path, NULL))
            fun_rebld(pw);
    }
}

/*
 * prompt for delete files or remove icons
 *
 * if user selects Delete, returns TRUE
 * if user selects Remove, sends a message to remove icons & returns FALSE
 * else returns FALSE
 */
BOOL wants_to_delete_files(void)
{
    WORD ret;

    ret = fun_alert(1,STRMVDEL);

    if (ret == 2)       /* Delete */
        return TRUE;

    if (ret == 1)       /* Remove */
        fun_msg(MN_SELECTED,OPTNMENU,RICNITEM,0,0,0);

    return FALSE;       /* Remove or Cancel */
}
