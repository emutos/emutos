/*      DESKFUN.C       08/30/84 - 05/30/85             Lee Lorenzen    */
/*                      10/2/86  - 01/16/87             MDF             */
/*      merge source    5/27/87  - 5/28/87              mdf             */
/*      for 2.3         6/11/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2001 John Elliott
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
#include "deskins.h"
#include "biosdefs.h"

#include "string.h"
#include "gemerror.h"
#include "kprint.h"



/*
 *  Issue an alert
 */
WORD fun_alert(WORD defbut, WORD stnum)
{
    rsrc_gaddr_rom(R_STRING, stnum, (void **)&G.a_alert);
    return form_alert(defbut, G.a_alert);
}


/*
 *  Issue an alert after merging in an optional character variable
 */
WORD fun_alert_merge(WORD defbut, WORD stnum, BYTE merge)
{
    rsrc_gaddr_rom(R_STRING, stnum, (void **)&G.a_alert);
    sprintf(G.g_1text, G.a_alert, merge);

    return form_alert(defbut, G.g_1text);
}


/*
 *  Issue an alert after merging in a long variable
 */
WORD fun_alert_long(WORD defbut, WORD stnum, LONG merge)
{
    rsrc_gaddr_rom(R_STRING, stnum, (void **)&G.a_alert);
    sprintf(G.g_1text, G.a_alert, merge);

    return form_alert(defbut, G.g_1text);
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
 *  Rebuild any windows with matching path
 */
void fun_rebld(BYTE *ptst)
{
    GRECT gr;
    WNODE *pwin;

    graf_mouse(HGLASS, NULL);

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

    tree = G.a_trees[ADMKDBOX];
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
            fun_rebld(pw_node->w_path->p_spec);
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

static void w_setpath(WNODE *pw, BYTE *pathname)
{
    WORD icx, icy;
    GRECT rc;

    wind_get_grect(pw->w_id,WF_WXYWH, &rc);

    icx = rc.g_x + (rc.g_w / 2) - (G.g_wicon / 2);
    icy = rc.g_y + (rc.g_h / 2) - (G.g_hicon / 2);
    graf_shrinkbox(icx, icy, G.g_wicon, G.g_hicon,
                    rc.g_x, rc.g_y, rc.g_w, rc.g_h);

    do_fopen(pw, 0, pathname, TRUE);
}


static void fun_full_close(WNODE *pw)
{
    WORD icx, icy;
    GRECT rc;

    wind_get_grect(pw->w_id,WF_WXYWH, &rc);
    wind_close(pw->w_id);

    /*
     * only do the shrinkbox effect if there is a matching icon
     */
    if (pw->w_obid > 0)
    {
        icx = G.g_screen[pw->w_obid].ob_x;
        icy = G.g_screen[pw->w_obid].ob_y;
        graf_shrinkbox(icx, icy, G.g_wicon, G.g_hicon,
                        rc.g_x, rc.g_y, rc.g_w, rc.g_h);
    }

    pn_close(pw->w_path);
    win_free(pw);
}


/*
 * full or partial close of desktop window
 */
void fun_close(WNODE *pw, WORD closetype)
{
    BYTE pathname[MAXPATHLEN];
    BYTE *fname;

    if (!pw->w_path)
    {
        KDEBUG(("Invalid WNODE passed to fun_close()\n"));
        return;
    }

    graf_mouse(HGLASS, NULL);

    /*
     * handle CLOSE_FOLDER and CLOSE_TO_ROOT
     *
     * if already in the root, change CLOSE_FOLDER to CLOSE_WINDOW
     * (but don't change CLOSE_TO_ROOT!)
     */
    if (closetype != CLOSE_WINDOW)
    {
        strcpy(pathname,pw->w_path->p_spec);
        fname = filename_start(pathname);
        if (closetype == CLOSE_TO_ROOT)
            strcpy(pathname+3,fname);
        else if (pathname+3 == fname)
            closetype = CLOSE_WINDOW;
        else    /* we need to go up one level */
            remove_one_level(pathname);
    }

    if (closetype == CLOSE_WINDOW)
        fun_full_close(pw);
    else
        w_setpath(pw,pathname);

    graf_mouse(ARROW, NULL);
}



/*
 *  Routine to call when several icons have been dragged from a
 *  window to another window (it might be the same window) and
 *  dropped on a particular icon or open space.
 *
 *  Note that, for DESK1, this is NEVER called if either the source
 *  or destination is the desktop.  Therefore 'datype' can ONLY be
 *  AT_ISFILE or AT_ISFOLD.
 */
static void fun_win2win(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD keystate)
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
            fun_rebld(psw->w_path->p_spec);
        fun_rebld(pdw->w_path->p_spec);
        /*
         * if we copied into a folder, we must redraw any windows with
         * a matching path
         */
        if (datype == AT_ISFOLD)
            fun_rebld(destpath);
    }
}


static WORD fun_file2desk(PNODE *pn_src, WORD icontype_src, ANODE *an_dest, WORD dobj, WORD keystate)
{
    ICONBLK *dicon;
    BYTE pathname[10];      /* must be long enough for X:\\*.* */
    WORD operation, ret;

    pathname[1] = ':';      /* set up everything except drive letter */
    strcpy(pathname+2, "\\*.*");

    operation = -1;
    if (an_dest)
    {
        switch(an_dest->a_type)
        {
        case AT_ISDISK:
            dicon = (ICONBLK *)G.g_screen[dobj].ob_spec;
            pathname[0] = LOBYTE(dicon->ib_char);
            operation = (keystate&MODE_CTRL) ? OP_MOVE : OP_COPY;
            break;
        case AT_ISTRSH:
            if (icontype_src >= 0)      /* source is desktop icon */
                if (wants_to_delete_files() == FALSE)
                    return FALSE;       /* i.e. remove icons or cancel */
            pathname[0] = pn_src->p_spec[0];
            operation = OP_DELETE;
            break;
        }
    }

    if (operation >= 0)
        ret = fun_op(operation, icontype_src, pn_src, pathname);
    else ret = FALSE;

    /*
     * if operation succeeded, rebuild any corresponding open windows
     */
    if (ret)
        fun_rebld(pathname);

    return ret;
}


static WORD fun_file2win(PNODE *pn_src, BYTE  *spec, ANODE *an_dest, FNODE *fn_dest)
{
    BYTE *p;
    BYTE pathname[MAXPATHLEN];

    strcpy(pathname, spec);

    p = filename_start(pathname);

    if (an_dest && an_dest->a_type == AT_ISFOLD)
    {
        strcpy(p, fn_dest->f_name);
        strcat(p, "\\*.*");
    }
    else
    {
        strcpy(p, "*.*");
    }

    return fun_op(OP_COPY, -1, pn_src, pathname);
}


static void fun_win2desk(WORD wh, WORD obj, WORD keystate)
{
    WNODE *wn_src;
    ANODE *an_dest;

    an_dest = app_afind_by_id(obj);
    if (!an_dest)   /* "can't happen" */
        return;

    wn_src = win_find(wh);
    if (!wn_src)
        return;

    if (fun_file2desk(wn_src->w_path, -1, an_dest, obj, keystate))
        fun_rebld(wn_src->w_path->p_spec);
}


static WORD fun_file2any(WORD sobj, WNODE *wn_dest, ANODE *an_dest, FNODE *fn_dest,
                  WORD dobj, WORD keystate)
{
    WORD icontype, okay = 0;
    FNODE *bp8;
    ICONBLK * ib_src;
    PNODE *pn_src;
    ANODE *an_src;
    BYTE path[MAXPATHLEN];

    an_src = i_find(0, sobj, NULL, NULL);

#if CONF_WITH_DESKTOP_SHORTCUTS
    if ((an_src->a_type == AT_ISFILE) || (an_src->a_type == AT_ISFOLD))
    {
        strcpy(path, an_src->a_pdata);
    }
    else
#endif
    {
        ib_src = (ICONBLK *)G.g_screen[sobj].ob_spec;
        build_root_path(path, ib_src->ib_char);
        strcat(path,"*.*");
    }

    pn_src = pn_open(path, F_SUBDIR);

    if (pn_src)
    {
        okay = pn_active(pn_src);

        if (pn_src->p_flist)
        {
            for (bp8 = pn_src->p_flist; bp8; bp8 = bp8->f_next)
                bp8->f_obid = 0;
            /*
             * if we do not set the root's SELECTED attribute, dir_op()
             * (which is called by fun_file2win() & fun_file2desk())
             * will not process any of these files ...
             */
            G.g_screen->ob_state = SELECTED;
            if (wn_dest)    /* we are dragging a desktop icon to a window */
            {
                okay = fun_file2win(pn_src, wn_dest->w_path->p_spec, an_dest, fn_dest);
            }
            else    /* we are dragging a desktop item to another desktop item */
            {
                icontype = an_src ? an_src->a_type : -1;
                okay = fun_file2desk(pn_src, icontype, an_dest, dobj, keystate);
            }
            G.g_screen->ob_state = 0;
        }
        pn_close(pn_src);
        desk_clear(0);
    }

    return okay;
}


static void fun_desk2win(WORD wh, WORD dobj, WORD keystate)
{
    WNODE *wn_dest;
    FNODE *fn_dest;
    WORD sobj, copied;
    ANODE *an_src, *an_dest;

    wn_dest = win_find(wh);
    if (!wn_dest)
        return;

    an_dest = i_find(wh, dobj, &fn_dest, NULL);
    sobj = 0;
    while ((sobj = win_isel(G.g_screen, DROOT, sobj)))
    {
        an_src = i_find(0, sobj, NULL, NULL);
        if (an_src && (an_src->a_type == AT_ISTRSH))
        {
            fun_alert(1, STNODRA2);
            continue;
        }
        copied = fun_file2any(sobj, wn_dest, an_dest, fn_dest, dobj, keystate);
        if (copied)
            fun_rebld(wn_dest->w_path->p_spec);
    }
}


static void fun_desk2desk(WORD dobj, WORD keystate)
{
    WORD sobj;
    ANODE *source;
    ANODE *target;

    target = app_afind_by_id(dobj);
    if (!target)    /* "can't happen" */
        return;

    sobj  = 0;
    while ((sobj = win_isel(G.g_screen, DROOT, sobj)))
    {
        source = i_find(0, sobj, NULL, NULL);
        if (!source || (source == target))
            continue;
        if (source->a_type == AT_ISTRSH)
        {
            fun_alert(1, STNOSTAK);
            continue;
        }
        fun_file2any(sobj, NULL, target, NULL, dobj, keystate);
    }
}


void fun_drag(WORD wh, WORD dest_wh, WORD sobj, WORD dobj, WORD mx, WORD my, WORD keystate)
{
    if (wh)
    {
        if (dest_wh)    /* dragging from window to window, */
        {               /* e.g. copy/move files/folders    */
            fun_win2win(wh, dest_wh, dobj, keystate);
        }
        else            /* dragging from window to desktop */
        {
            if (sobj == dobj)   /* dropping onto desktop surface */
            {
#if CONF_WITH_DESKTOP_SHORTCUTS
                ins_shortcut(wh, mx, my);
#else
                fun_alert(1, STNODRA1);
#endif
            }
            else                /* dropping onto desktop icon */
                fun_win2desk(wh, dobj, keystate);
        }
    }
    else    /* Dragging something from desk */
    {
        if (dest_wh)    /* dragging from desktop to window,  */
        {               /* e.g. copying a disk into a folder */
            fun_desk2win(dest_wh, dobj, keystate);
        }
        else            /* dragging from desktop to desktop,   */
        {               /* e.g. copying a disk to another disk */
            if (sobj != dobj)
                fun_desk2desk(dobj, keystate);
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   e n d   o f   D E S K 1   r o u t i n e s                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


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
            fun_rebld(pw->w_path->p_spec);
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
