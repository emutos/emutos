/*      DESKSUPP.C      05/04/84 - 06/20/85     Lee Lorenzen            */
/*      for 3.0 (xm)    3/12/86  - 01/17/87     MDF                     */
/*      for 3.0                 11/13/87                mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
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

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "taddr.h"
#include "dos.h"
#include "desk_rsc.h"
#include "infodef.h"
#include "gembind.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskbind.h"

#include "optimize.h"
#include "rectfunc.h"
#include "gemdos.h"
#include "aesbind.h"
#include "deskact.h"
#include "deskpro.h"
#include "deskinf.h"
#include "deskfun.h"
#include "deskrsrc.h"
#include "deskmain.h"
#include "deskglob.h"
#include "deskgraf.h"
#include "desk1.h"


#if MULTIAPP
EXTERN WORD     pr_kbytes;
EXTERN LONG     pr_ssize;
EXTERN LONG     pr_topdsk;
EXTERN LONG     pr_topmem;
EXTERN WORD     gl_keepac;
#endif



/*
*       Clear out the selections for this particular window
*/
void desk_clear(WORD wh)
{
        WNODE   *pw;
        GRECT   c;

                                                /* get current size     */
        wind_get(wh, WF_WXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h);
                                                /* find its tree of     */
                                                /*   items              */
        pw = win_find(wh);
        if (pw)
        {
                                                /* clear all selections */
          act_allchg(wh, G.a_screen, pw->w_root, 0, &gl_rfull, &c,
                 SELECTED, FALSE, TRUE);
        }
}


/*
*       Verify window display by building a new view.
*/
void desk_verify(WORD wh, WORD changed)
{
        WNODE   *pw;
        WORD    xc, yc, wc, hc;

#ifdef DESK1
        if (wh)
        { 
#endif
                                                /* get current size     */
        pw = win_find(wh);
        if (pw)
        {
          if (changed)
          {
            wind_get(wh, WF_WXYWH, &xc, &yc, &wc, &hc);
            win_bldview(pw, xc, yc, wc, hc);
          }
          G.g_croot = pw->w_root;
        }
#ifdef DESK1
        }
        else G.g_croot = 1;                     /* DESK v1.2: The Desktop */
#endif

        G.g_cwin = wh;
        G.g_wlastsel = wh;
}



void do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc)
{
        GRECT   clip_r, t;
        WNODE   *pw;
        LONG    tree;
        WORD    root;

        tree = G.a_screen;

        clip_r.g_x = xc;
        clip_r.g_y = yc;
        clip_r.g_w = wc;
        clip_r.g_h = hc;
        if (w_handle != 0)
        {
          pw = win_find(w_handle);
          root = pw->w_root;
        }
        else
#ifdef DESK1
          root = 1;
#else
          return;
#endif

        graf_mouse(M_OFF, 0x0L);

        wind_get(w_handle, WF_FIRSTXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
        while ( t.g_w && t.g_h )
        {
          if ( rc_intersect(&clip_r, &t) )
            objc_draw(tree, root, MAX_DEPTH, t.g_x, t.g_y, t.g_w, t.g_h);
          wind_get(w_handle, WF_NEXTXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
        }
        graf_mouse(M_ON, 0x0L);
}


/*
*       Picks ob_x, ob_y, ob_width, ob_height fields out of object list.
*/
void get_xywh(OBJECT olist[], WORD obj, WORD *px, WORD *py, WORD *pw, WORD *ph)
{
        *px = olist[obj].ob_x;
        *py = olist[obj].ob_y;
        *pw = olist[obj].ob_width;
        *ph = olist[obj].ob_height;
}


/*
*       Picks ob_spec field out of object list.
*/
ICONBLK  *get_spec(OBJECT olist[], WORD obj)
{
        return( (ICONBLK *)LPOINTER(olist[obj].ob_spec) );
}


void do_xyfix(WORD *px, WORD *py)
{
        WORD    tx, ty, tw, th;

        wind_get(0, WF_WXYWH, &tx, &ty, &tw, &th);
        tx = *px;
        *px = (tx & 0x000f);
        if ( *px < 8 )
          *px = tx & 0xfff0;
        else
          *px = (tx & 0xfff0) + 16;
        *py = max(*py, ty);
}


void do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h)
{
        GRECT   c;
#ifdef DESK1
        GRECT   d;
#endif

        do_xyfix(&x, &y);
        get_xywh(G.g_screen, G.g_croot, &c.g_x, &c.g_y, &c.g_w, &c.g_h);

#ifdef DESK1
        /* Zooming box effect */
        get_xywh(G.g_screen, curr, &d.g_x, &d.g_y, &d.g_w, &d.g_h);
        d.g_x += c.g_x;
        d.g_y += c.g_y;
        graf_growbox(d.g_x, d.g_y, d.g_w, d.g_h, x, y, w, h);
#endif

        act_chg(G.g_cwin, G.a_screen, G.g_croot, curr, &c, SELECTED, 
                FALSE, TRUE, TRUE);
        if (new_win)
          wind_open(wh, x, y, w, h);

        G.g_wlastsel = wh;
}


void do_wfull(WORD wh)
{
#ifndef DESK1
        WORD     tmp_wh, y;
        GRECT    temp;
#endif
        GRECT    curr, prev, full;

        gl_whsiztop = NIL;
        wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
        wind_get(wh, WF_PXYWH, &prev.g_x, &prev.g_y, &prev.g_w, &prev.g_h);
        wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);

#ifdef DESK1
        if (rc_equal(&curr, &full))
        {
                wind_set(wh, WF_CXYWH, prev.g_x, prev.g_y, prev.g_w, prev.g_h);
                graf_shrinkbox(prev.g_x, prev.g_y, prev.g_w, prev.g_h,
                               full.g_x, full.g_y, full.g_w, full.g_h);
        }       
        else
        {
                graf_growbox(curr.g_x, curr.g_y, curr.g_w, curr.g_h,
                             full.g_x, full.g_y, full.g_w, full.g_h);
                wind_set(wh, WF_CXYWH, full.g_x, full.g_y, full.g_w, full.g_h);
        }
#else /* DESK1 */
                        /* have to check for shrinking a window that    */
                        /* was full when Desktop was first started.     */
        if ( (rc_equal(&curr, &prev)) && (curr.g_h > gl_normwin.g_h) )
        {                               /* shrink full window           */
                /* find the other window (assuming only 2 windows)      */
          if ( G.g_wlist[0].w_id == wh)
            tmp_wh = G.g_wlist[1].w_id;
          else
            tmp_wh = G.g_wlist[0].w_id;
                                /* decide which window we're shrinking  */
          wind_get(tmp_wh, WF_CXYWH, &temp.g_x, &temp.g_y,
                   &temp.g_w, &temp.g_h);
          if (temp.g_y > gl_normwin.g_y)
            y = gl_normwin.g_y;         /* shrinking upper window       */
          else                          /* shrinking lower window       */
            y = gl_normwin.g_y + gl_normwin.g_h + (gl_hbox / 2);
          wind_set(wh, WF_CXYWH, gl_normwin.g_x, y,
                   gl_normwin.g_w, gl_normwin.g_h);
        } /* if */
                                        /* already full, so change      */
                                        /* back to previous             */
        else if ( rc_equal(&curr, &full) )
          wind_set(wh, WF_CXYWH, prev.g_x, prev.g_y, prev.g_w, prev.g_h);
                                        /* make it full                 */
        else
        {
          gl_whsiztop = wh;
          wind_set(wh, WF_SIZTOP, full.g_x, full.g_y, full.g_w, full.g_h);
        }
#endif /* DESK1 */
} /* do_wfull */


/*
*       Open a directory, it may be the root or a subdirectory.
*/
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv,
                BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt, WORD redraw)
{
        WORD    ret;
        PNODE   *tmp;
                                                /* convert to hourglass */
        graf_mouse(HGLASS, 0x0L);
                                                /* open a path node     */
        tmp = pn_open(drv, ppath, pname, pext, F_SUBDIR);
        if ( tmp == NULLPTR)
        {
          graf_mouse(ARROW, 0x0L);
          return(FALSE);
        }
        else
          pw->w_path = tmp;


                                                /* activate path by     */
                                                /*   search and sort    */
                                                /*   of directory       */
        ret = pn_active(pw->w_path);
        if ( ret != E_NOFILES )
        {
                                                /* some error condition */
        }
                                                /* set new name and info*/
                                                /*   lines for window   */
        win_sname(pw);
#ifdef DESK1
        win_sinfo(pw);
#endif
        wind_set(pw->w_id, WF_NAME, ADDR(&pw->w_name[0]), 0, 0);
#ifdef DESK1
        wind_set(pw->w_id, WF_INFO, ADDR(&pw->w_info[0]), 0, 0);
#endif

                                                /* do actual wind_open  */
#ifdef DESK1
        if (curr_icon)
        {
                do_wopen(new_win, pw->w_id, curr_icon, 
                                pt->g_x, pt->g_y, pt->g_w, pt->g_h);
                if (new_win)
                        win_top(pw);
        }
#else
        do_wopen(new_win, pw->w_id, curr_icon, 
                                pt->g_x, pt->g_y, pt->g_w, pt->g_h);
        if (new_win)
            win_top(pw);
#endif
                                                /* verify contents of   */
                                                /*   windows object list*/
                                                /*   by building view   */
                                                /*   and make it curr.  */
        desk_verify(pw->w_id, TRUE);
                                                /* make it redraw       */
        if (redraw && ( !new_win ))
          fun_msg(WM_REDRAW, pw->w_id, pt->g_x, pt->g_y, pt->g_w, pt->g_h);

        graf_mouse(ARROW, 0x0L);
        return(TRUE);
} /* do_diropen */


/*
*       Open an application
*/
static WORD do_aopen(ANODE *pa, WORD isapp, WORD curr, WORD drv,
                     BYTE *ppath, BYTE *pname)
{
        WORD    ret, done;
        WORD    isgraf, isover, isparm, uninstalled;
        BYTE    *ptmp, *pcmd, *ptail;
        BYTE    name[13];

        done = FALSE;
                                                /* set flags            */
        isgraf = pa->a_flags & AF_ISGRAF;
#if MULTIAPP
        pr_kbytes = pa->a_memreq;               /* K bytes needed for app */
        isover = (pa->a_flags & AF_ISFMEM) ? 2 : -1;
        if ((isover == 2) && gl_keepac)         /* full-step ok?        */
        {
          rsrc_gaddr(R_STRING, STNOFSTP, &G.a_alert);
          form_alert(1, G.a_alert);
          return(FALSE);
        }
#else
        isover = (pa->a_flags & AF_ISFMEM) ? 2 : 1;
#endif
        isparm = pa->a_flags & AF_ISPARM;
        uninstalled = ( (*pa->a_pappl == '*') || 
                        (*pa->a_pappl == '?') ||
                        (*pa->a_pappl == NULL) );
                                                /* change current dir.  */
                                                /*   to selected icon's */
        pro_chdir(drv, ppath);
                                                /* see if application   */
                                                /*   was selected       */
                                                /*   directly or a      */
                                                /*   data file with an  */
                                                /*   associated primary */
                                                /*   application        */
        pcmd = ptail = NULLPTR;
        G.g_cmd[0] = G.g_tail[1] = NULL;
        ret = TRUE;

        if ( (!uninstalled) && (!isapp) )
        {
                                                /* an installed docum.  */
          pcmd = pa->a_pappl;
          ptail = pname;
        }
        else
        {
          if ( isapp )
          {
                                                /* DOS-based app. has   */
                                                /*   been selected      */
            if (isparm)
            {
              pcmd = &G.g_cmd[0];
              ptail = &G.g_tail[1];
              ret = opn_appl(pname, "\0", pcmd, ptail);
            }
            else
              pcmd = pname;
          } /* if isapp */
          else
          {
                                                /* DOS-based document   */
                                                /*   has been selected  */
            fun_alert(1, STNOAPPL, NULLPTR);
            ret = FALSE;
          } /* else */
        } /* else */
                                                /* see if it is a       */
                                                /*   batch file         */
        if ( wildcmp( ini_str(STGEMBAT), pcmd) )
        {
                                                /* if is app. then copy */
                                                /*   typed in parameter */
                                                /*   tail, else it was  */
                                                /*   a document installed*/
                                                /*   to run a batch file*/
          strcpy(&G.g_1text[0], (isapp) ? &G.g_tail[1] : ptail);
          ptmp = &name[0];
          pname = pcmd;
          while ( *pname != '.' )
            *ptmp++ = *pname++;
          *ptmp = NULL;
          ret = pro_cmd(&name[0], &G.g_1text[0], TRUE);
          pcmd = &G.g_cmd[0];
          ptail = &G.g_tail[1];
        } /* if */
                                                /* user wants to run    */
                                                /*   another application*/
        if (ret)
        {
          if ( (pcmd != &G.g_cmd[0]) &&
               (pcmd != NULLPTR) )
            strcpy(&G.g_cmd[0], pcmd);
          if ( (ptail != &G.g_tail[1])  &&
               (ptail != NULLPTR) )
            strcpy(&G.g_tail[1], ptail);
          done = pro_run(isgraf, isover, G.g_cwin, curr);
        } /* if ret */
#if MULTIAPP
        return(FALSE);                          /* don't want to exit   */
#else
        return(done);
#endif
} /* do_aopen */



/*
*       Open a disk
*/
#ifdef DESK1
WORD do_dopen(WORD curr)
{
        WORD    drv;
        WNODE   *pw;
        ICONBLK *pib;

        pib = (ICONBLK *) get_spec(G.g_screen, curr);
        pw = win_alloc(curr);
        if (pw)
        {
          drv = (0x00ff & pib->ib_char);
          pro_chdir(drv, "");
          if (!DOS_ERR)
            do_diropen(pw, TRUE, curr, drv, "", "*", "*", 
                        (GRECT *)&G.g_screen[pw->w_root].ob_x, TRUE);
          else
            win_free(pw);
        }
        else
        {
          rsrc_gaddr(R_STRING, STNOWIND, &G.a_alert);
          form_alert(1, G.a_alert);
        }
        return( FALSE );
}
#endif


/*
*       Open a folder
*/
/* chkall=TRUE if coming from do_chkall */
void do_fopen(WNODE *pw, WORD curr, WORD drv, BYTE *ppath, BYTE *pname,
              BYTE *pext, WORD chkall, WORD redraw)
{
        GRECT   t;
        WORD    ok;
        BYTE    *pnew;
#ifndef DESK1
        BYTE    *pp;
#endif
        
        ok = TRUE;
        pnew = ppath;
        wind_get(pw->w_id, WF_WXYWH, &t.g_x, &t.g_y, &t.g_w, &t.g_h);
        pro_chdir(drv, "");
        if (DOS_ERR)
        {
#ifdef DESK1
          true_closewnd(pw);
          return;
#else /*DESK1*/
          if ( DOS_AX == E_PATHNOTFND )
          {
            if (!chkall)
              fun_alert(1, STDEEPPA, NULLPTR);
            else
            {
              pro_chdir(drv, "");
              pnew = "";
            }
          } /* if */
          else
            return;                     /* error opening disk drive     */
#endif  /*DESK1 */
        } /* if DOS_ERR */
        else
        {
          pro_chdir(drv, ppath);
#ifndef DESK1
          if (DOS_ERR)
          {
            if ( DOS_AX == E_PATHNOTFND )
            {                           /* DOS path is too long?        */
              if (chkall)
              {
                pro_chdir(drv, "");
                pnew = "";
              }
              else
              {
                fun_alert(1, STDEEPPA, NULLPTR);
                                                /* back up one level    */
                pp = ppath;
                while (*pp)
                  pp++;
                while(*pp != '\\')
                  pp--;
                *pp = NULL;
                pname = "*";
                pext  = "*";
                return;
              } /* else */
            } /* if DOS_AX */
            else
              return;                   /* error opening disk drive     */
          } /* if DOS_ERR */
#endif
        } /* else */
        pn_close(pw->w_path);
#ifdef DESK1
        if (!DOS_ERR)
        {
                ppath = "";
                pname = "*";
                pext  = "*";
        }
        ok = do_diropen(pw, FALSE, curr, drv, pnew, pname, pext, &t, redraw);
#else /* DESK1 */
        if (ok)
        {
          ok = do_diropen(pw, FALSE, curr, drv, pnew, pname, pext, &t, redraw);
          if ( !ok )
          {
            fun_alert(1, STDEEPPA, NULLPTR);
                                                /* back up one level    */
            pp = ppath;
            while (*pp)
              pp++;
            while(*pp != '\\')
              pp--;
            *pp = NULL;
            do_diropen(pw, FALSE, curr, drv, pnew, pname, pext, &t, redraw);
          }
        }
#endif /* DESK1 */
} /* do_fopen */


/*
*       Open an icon
*/
WORD do_open(WORD curr)
{
        WORD    done;
        ANODE   *pa;
        WNODE   *pw;
        FNODE   *pf;
        WORD    drv, isapp;
        BYTE    path[66], name[9], ext[4];

        done = FALSE;

        pa = i_find(G.g_cwin, curr, &pf, &isapp);
        pw = win_find(G.g_cwin);
        if ( pf )
          fpd_parse(&pw->w_path->p_spec[0],&drv,&path[0],&name[0],&ext[0]);

        if ( pa )
        {       
          switch( pa->a_type )
          {
            case AT_ISFILE:
#if MULTIAPP
                if (strcmp("DESKTOP.APP", &pf->f_name[0])==0)
                  break;
#endif
                done = do_aopen(pa,isapp,curr,drv,&path[0],&pf->f_name[0]);
                break;
            case AT_ISFOLD:
#ifndef DESK1
                if ( (pf->f_attr & F_FAKE) && pw )
                  fun_mkdir(pw);
                else
#endif
                {
                  if (path[0] != NULL)
                    strcat(&path[0], "\\");
                  if ( (strlen(&path[0]) + LEN_ZFNAME) >= (LEN_ZPATH-3) )
                    fun_alert(1, STDEEPPA, NULLPTR);
                  else
                  {
                    strcat(&path[0], &pf->f_name[0]);
                    pw->w_cvrow = 0;            /* reset slider         */
                    do_fopen(pw, curr, drv, &path[0], &name[0],
                             &ext[0], FALSE, TRUE);
                  }
                }
                break;
            case AT_ISDISK:
#ifdef DESK1
                do_dopen(curr);
#else
                drv = (0x00ff & pa->a_letter);
                path[0] = NULL;
                name[0] = ext[0] = '*';
                name[1] = ext[1] = NULL;
                do_fopen(pw, curr, drv, &path[0], &name[0], &ext[0],
                                         FALSE, TRUE);
#endif
                break;
#ifdef DESK1
            case AT_ISTRSH:
                form_alert(1, (LONG)ini_str(STNOOPEN));
                break;
#endif
          }
        }

        return(done);
}



/*
*       Get information on an icon.
*/
WORD do_info(WORD curr)
{
        WORD    ret, junk;
        ANODE   *pa;
        WNODE   *pw;
        FNODE   *pf;
#ifndef DESK1
        LONG    tree;
#endif

        pa = i_find(G.g_cwin, curr, &pf, &junk);
        pw = win_find(G.g_cwin);

        if ( pa )
        {       
          switch( pa->a_type )
          {
            case AT_ISFILE:
                ret = inf_file(&pw->w_path->p_spec[0], pf);
                if (ret)
                  fun_rebld(pw);
                break;
            case AT_ISFOLD:
#ifndef DESK1
                if (pf->f_attr & F_FAKE)
                {
                  tree = G.a_trees[ADNFINFO];
                  inf_show(tree, 0);
                  LWSET(OB_STATE(NFINOK), NORMAL);
                }
                else
#endif
                  inf_folder(&pw->w_path->p_spec[0], pf);
                break;
            case AT_ISDISK:
#ifdef DESK1
                junk = (get_spec(G.g_screen, curr)->ib_char) & 0xFF;
                inf_disk(junk);
#else
                inf_disk( pf->f_junk );
#endif
                break;
#ifdef DESK1
            case AT_ISTRSH:
                fun_alert(1, STTRINFO, NULLPTR);
                break;
#endif
          }
        }
        return( FALSE );
}


/*
*       Format the currently selected disk.
*/
void do_format(WORD curr)
{
        WORD    junk, ret;
#if I8086
        WORD    foundit
#endif
        BYTE    msg[6];
        ANODE   *pa;
        FNODE   *pf;
        WORD    drive_letter;

        pa = i_find(G.g_cwin, curr, &pf, &junk);

        if ( (pa) && (pa->a_type == AT_ISDISK) )
        {
          drive_letter = pf->f_junk;

          ret = fun_alert(2, STFORMAT, &drive_letter);

          msg[0] = drive_letter;
          msg[1] = ':';
          msg[2] = 0;

          if (ret == 1)
          {
#if MC68K
            ret = pro_cmd( ini_str(STDKFRM1), &msg[0], TRUE);
            if (ret)
              /*done =*/ pro_run(FALSE, FALSE, G.g_cwin, curr);
#else
            strcpy(&G.g_cmd[0],ini_str(STDKFRM1));
            foundit = shel_find(G.a_cmd);
            if (!foundit)
            {
              strcpy(&G.g_cmd[0], ini_str(STDKFRM2));
              foundit = shel_find(G.a_cmd);
            }
            if (foundit)
            {
              strcpy(&G.g_tail[1], &msg[0]);

              takedos();
              takekey();
              takevid();

              romerr(curr);
              givevid();
              givekey();
              givedos();
            } /* if */
            else
              fun_alert(1, STNOFRMT, NULLPTR);
#endif
            graf_mouse(ARROW, 0x0L);    
          } /* if ret */
        } /* if */
} /* do_format */


/*
*       Routine to check the all windows directory by doing a change
*       disk/directory to it and redrawing the window;
*/
void do_chkall(WORD redraw)
{
        WORD    ii;
        WORD    drv;
        BYTE    path[66], name[9], ext[4];
        WNODE   *pw;

        for(ii = 0; ii < NUM_WNODES; ii++)
        {
          pw = &G.g_wlist[ii];
          if (pw->w_id)
          {
            fpd_parse(&pw->w_path->p_spec[0], &drv, &path[0],
                      &name[0], &ext[0]);
            do_fopen(pw, 0, drv, &path[0], &name[0], &ext[0], TRUE, redraw);
          }
          else
          {
            desk_verify(0, TRUE);
          }
        }
} /* do_chkall */

