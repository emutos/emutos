/*      GEMMNLIB.C      04/26/84 - 08/14/86     Lowell Webster          */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */ 
/*      fix mn_bar -- bar too wide              11/19/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright                                 
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include "portab.h"
#include "compat.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "taddr.h"
#include "gemlib.h"

#include "gemgsxif.h"
#include "gemevlib.h"
#include "gemoblib.h"
#include "gemobed.h"
#include "gemwmlib.h"
#include "gemgraf.h"
#include "geminput.h"
#include "gemsuper.h"
#include "gemctrl.h"
#include "gempd.h"
#include "rectfunc.h"

#include "string.h"


GLOBAL LONG     gl_mntree;
GLOBAL PD       *gl_mnppd;

GLOBAL LONG     desk_acc[NUM_DESKACC];
GLOBAL PD       *desk_ppd[NUM_DESKACC];
GLOBAL LONG     menu_tree[NUM_PDS];

static BYTE     desk_str_space[NUM_DESKACC][22];
GLOBAL BYTE     * const desk_str[NUM_DESKACC] = 
{
    desk_str_space[0],
    desk_str_space[1],
    desk_str_space[2],
    desk_str_space[3],
    desk_str_space[4],
    desk_str_space[5],
    desk_str_space[6],
    desk_str_space[7],
#if MULTIAPP
    desk_str_space[8],
    desk_str_space[9],
    desk_str_space[10],
    desk_str_space[11],
    desk_str_space[12],
    desk_str_space[13],
    desk_str_space[14],
    desk_str_space[15],
#endif
};

GLOBAL WORD     gl_dacnt;
GLOBAL WORD     gl_dabox;
GLOBAL LONG     gl_datree;

GLOBAL OBJECT   M_DESK[3+NUM_DESKACC];

#if MULTIAPP

OBJECT  gl_sysmenu[10] = {
-1,  1,  4,  G_IBOX, NONE, NORMAL,       0x0L, 0, 0, 80, 25, /* SCREEN   */
 4,  2,  2,   G_BOX, NONE, NORMAL,    0x1100L, 0, 0, 80,513, /* BAR      */
 1,  3,  3,  G_IBOX, NONE, NORMAL,       0x0L, 2, 0, 20,769, /* ACTIVE   */
 2, -1, -1, G_TITLE, NONE, NORMAL,       0x0L, 0, 0,  8,769, /* TITLE #1 */ 
 0, -1, -1,  G_IBOX, NONE, NORMAL,       0x0L, 0,769,80, 19  /* MENUS    */
};

GLOBAL LONG     ad_sysmenu; 
GLOBAL WORD     gl_mninit = FALSE;

mn_init() /* initialize default menu */
{
        WORD    i;

        if (gl_mninit)
          return;
        ad_sysmenu = ADDR(gl_sysmenu);
        for (i=0; i<=4; i++)
          rs_obfix(ad_sysmenu, i);
        gl_sysmenu[3].ob_spec = ADDR(" SWITCH ");
        menu_tree[1] = ad_sysmenu;
}
#endif



static WORD menu_sub(LONG *ptree, WORD ititle)
{
        LONG            tree;
        WORD            imenu;
        WORD            i;

        tree = *ptree;
                                                /* correlate title #    */
                                                /*   to menu subtree #  */
        imenu = LWGET(OB_HEAD(THEMENUS));
        for (i=ititle-THEACTIVE; i>1; i--)
          imenu = LWGET(OB_NEXT(imenu));
                                                /* special case desk acc*/
        if (imenu == gl_dabox)  
        {
          *ptree = gl_datree;
          imenu = 0;
        }

        return(imenu);
}



/*
*       Routine to find a desk accessory id number given a process descriptor.
*/
#if 0  /* unused */
WORD mn_getda(PD *ppd)
{
        register WORD   i;

        for (i=0; i<NUM_DESKACC; i++)
        {
          if (ppd == desk_ppd[i])
            return(i);
        }
        return(0);
}
#endif


static void menu_fixup(BYTE *pname)
{
        register OBJECT *pob;
        GRECT           t;
        WORD            i, cnt, st;
        LONG            tree;

        if ( (tree = gl_mntree) == 0x0L )
          return;

        w_nilit(3 + NUM_DESKACC, &M_DESK[0]);
                                                
        gl_dabox = LWGET( OB_HEAD(THEMENUS) );
        pob = &M_DESK[ROOT];
        gl_datree = ADDR( pob );
                                                /* fix up desk root     */
        pob->ob_type = G_BOX;
        pob->ob_state = pob->ob_flags = 0x0;
        pob->ob_spec = 0x00FF1100L;
        ob_actxywh(tree, gl_dabox, (GRECT *)&pob->ob_x);

        cnt = (gl_dacnt) ? (2 + gl_dacnt) : 1;
                                                /* fix up links         */
#if MULTIAPP
        pob->ob_head = 3;
#endif
#if SINGLAPP
        pob->ob_head = 1;
#endif
        pob->ob_tail = cnt;
                                                /* build up desk items  */
        ob_relxywh(tree, gl_dabox + 1, &t);
#if MULTIAPP
        for(i=3; i<=cnt; i++)
#endif
#if SINGLAPP
        for(i=1; i<=cnt; i++)
#endif
        {
          pob = &M_DESK[i];
          pob->ob_next = i+1;
          /* Special support for custom accessory separator line
           * customized using a USERDEF (ex: CF-Lib, QED).
           * We must keep the original ob_type (usually G_STRING). */
          pob->ob_type = LWGET(OB_TYPE(gl_dabox+i));
          pob->ob_state = pob->ob_flags = 0x0;
          if (i > 2)
          {
            st = i-3;
            while( !desk_acc[st] )
              st++;
            pob->ob_spec = desk_acc[st];
          }
          else
            pob->ob_spec = LLGET(OB_SPEC(gl_dabox+i));
          rc_copy(&t, (GRECT *)&pob->ob_x);
          t.g_y += gl_hchar;
        }
                                                /* link back to root    */
        pob->ob_next = 0;
                                                /* fix up size          */
        M_DESK[ROOT].ob_height = t.g_y;
                                                /* fix up disabled line */
        M_DESK[2].ob_state = DISABLED;
}



/*
*       Change a mouse-wait rectangle based on an object's size.
*/
static void rect_change(LONG tree, MOBLK *prmob, WORD iob, WORD x)
{
        ob_actxywh(tree, iob, (GRECT *)&prmob->m_x);
        prmob->m_out = x;
}



/*
*       Routine to change the state of a particular object.  The
*       change in state will not occur if the object is disabled
*       and the chkdisabled parameter is set.  The object will
*       be drawn with its new state only if the dodraw parameter
*       is set.
*/

UWORD do_chg(LONG tree, WORD iitem, UWORD chgvalue,
             WORD dochg, WORD dodraw, WORD chkdisabled)
/* tree:         tree that holds item */
/* iitem:        item to affect       */
/* chgvalue:     bit value to change  */
/* dochg:        set or reset value   */
/* dodraw:       draw resulting change*/
/* chkdisabled:  only if item enabled */
{
        register UWORD  curr_state;

        curr_state = LWGET(OB_STATE(iitem));
        if ( (chkdisabled) &&
             (curr_state & DISABLED) )
          return(FALSE);
        
        if ( dochg )
          curr_state |= chgvalue;
        else
          curr_state &= ~chgvalue;

        if (dodraw)
          gsx_sclip(&gl_rzero);
        
        ob_change(tree, iitem, curr_state, dodraw);
        return(TRUE);
}


/*
*       Routine to set and reset values of certain items if they
*       are not the current item
*/
WORD menu_set(LONG tree, WORD last_item, WORD cur_item, WORD setit)
{
        if ( (last_item != NIL) &&
             (last_item != cur_item) )
        {
          return( do_chg( tree, last_item, SELECTED, setit, TRUE, TRUE) );
        }
        return(FALSE);
}


/*
*       Routine to save or restore the portion of the screen underneath
*       a menu tree.  This involves BLTing out and back
*       the data that was underneath the menu before it was pulled
*       down.
*/
void menu_sr(WORD saveit, LONG tree, WORD imenu)
{
        GRECT           t;
                                                /* do the blit to save  */
                                                /*   or restore         */
        gsx_sclip(&gl_rzero);   
        ob_actxywh(tree, imenu, &t);
        t.g_x -= MTH;
        t.g_w += 2*MTH;
        t.g_h += 2*MTH;
        if (saveit)
          bb_save(&t);
        else
          bb_restore(&t);
}



/*
*       Routine to pull a menu down.  This involves saving the data
*       underneath the menu and drawing in the proper menu sub-tree.
*/
WORD menu_down(WORD ititle)
{
        LONG            tree;
        register WORD   imenu;

        tree = gl_mntree;
        imenu = menu_sub(&tree, ititle);
                                                /* draw title selected  */
        if ( do_chg(gl_mntree, ititle, SELECTED, TRUE, TRUE, TRUE) )
        {
                                                /* save area underneath */
                                                /*   the menu           */
          menu_sr(TRUE, tree, imenu);
                                                /* draw all items in menu */
          ob_draw(tree, imenu, MAX_DEPTH);
        }
        return(imenu);
}



WORD mn_do(WORD *ptitle, WORD *pitem)
{
        register LONG   tree;
        LONG            buparm, cur_tree, last_tree;
        WORD            mnu_flags, done;
        register WORD   cur_menu, cur_item, last_item;
        WORD            cur_title, last_title;
        UWORD           ev_which;
        MOBLK           p1mor, p2mor;
        WORD            menu_state, theval;
        WORD            rets[6];

                                                /* initially wait to    */
                                                /*   go into the active */
                                                /*   part of the bar    */
                                                /*   or the button state*/
                                                /*   to change          */
                                                /*   or out of the bar  */
                                                /*   when nothing is    */
                                                /*   down               */
        menu_state = INBAR;

        done = FALSE;
        buparm = 0x00010101L;
        cur_title = cur_menu = cur_item = NIL;
        cur_tree = tree = gl_mntree;
        
        while (!done)
        {
                                                /* assume menustate is  */
                                                /*   the OUTTITLE case  */
          mnu_flags = MU_KEYBD | MU_BUTTON | MU_M1;
          last_tree = tree;
          last_item = cur_title;
          theval = TRUE;
                                                /* switch on menu state */
          switch (menu_state)
          {
            case INBAR:
                mnu_flags |= MU_M2;
                last_item = THEBAR;
                break;
            case INBARECT:
                mnu_flags |= MU_M2;
                last_item = cur_menu;
                theval = FALSE;
                if (last_item == 0)
                  last_tree = gl_datree;
                break;
            case OUTITEM:
                last_tree = cur_tree;
                last_item = cur_item;
                buparm = (button & 0x0001) ? 0x00010100L : 0x00010101L;
                break;
          }
                                                /* set up rects. to     */
                                                /*   wait for           */
          if (last_item == NIL)
            last_item = THEBAR;
          if (mnu_flags & MU_M2)
          {
            rect_change(last_tree, &p2mor, last_item, theval);
            last_tree = tree;
            last_item = THEACTIVE;
            theval = FALSE;
          }
          rect_change(last_tree, &p1mor, last_item, theval);
                                                /* wait for something   */
          rets[5] = 0;
          ev_which = ev_multi(mnu_flags, &p1mor, &p2mor, 0x0L,
                                buparm, 0x0L, &rets[0]);
                                                /* if its a button and  */
                                                /*   not in a title then*/
                                                /*   done else flip state*/
          if ( ev_which & MU_BUTTON )
          {
            if ( (menu_state != OUTTITLE) &&
                 ((buparm & 0x00000001) || (gl_mnclick != 0)) )
              done = TRUE;
            else
              buparm ^= 0x00000001;
          }
                                                /* if not done then do  */
                                                /*   menus              */
          if ( !done )
          {
                                                /* save old values      */
            last_title = cur_title;
            last_item = cur_item;
                                                /* see if over the bar  */

            cur_title = ob_find( tree, THEACTIVE, 1, rets[0], rets[1] );
            if ( (cur_title != NIL) && (cur_title != THEACTIVE) )
            {
              menu_state = OUTTITLE;
              if ((gl_mnclick == 0) || (rets[5] == 1))
                cur_item = NIL;
              else
                cur_title = last_title;
            }
            else
            {
              cur_title = last_title;
                                                /* if menu never shown  */
                                                /*  nothing selected.   */
              if (cur_menu == NIL)
                cur_title = NIL;
                                                /* if nothing selected  */
                                                /*  get out.            */
              if (cur_title == NIL)
              {
                menu_state = INBAR;
                done = TRUE;
              }
              else
              {
                cur_item = ob_find(cur_tree, cur_menu, 1, rets[0], rets[1]);
                if (cur_item != NIL)
                  menu_state = OUTITEM;
                else
                {
                  if ( (LWGET(OB_STATE(cur_title))) & DISABLED )
                  {
                    menu_state = INBAR;
                    cur_title = NIL;
                    done = TRUE;
                  }
                  else
                    menu_state = INBARECT;
                }
              }
            }
                                                /* unhilite old item    */
            menu_set(cur_tree, last_item, cur_item, FALSE);
                                                /* unhilite old title & */
                                                /*   pull up old menu   */
            if ( menu_set(tree, last_title, cur_title, FALSE) )
              menu_sr(FALSE, cur_tree, cur_menu);
                                                /* hilite new title &   */
                                                /*   pull down new menu */
            if ( menu_set(tree, cur_title, last_title, TRUE) )
            {
              cur_menu = menu_down(cur_title);
                                                /* special case desk acc*/
              cur_tree = (cur_menu == 0) ? gl_datree : gl_mntree;
            }
                                                /* hilite new item      */
            menu_set(cur_tree, cur_item, last_item, TRUE);
          }
        }
                                                /* decide what should   */
                                                /*   be cleaned up and  */
                                                /*   returned           */
        done = FALSE;
        if ( cur_title != NIL )
        {
          menu_sr(FALSE, cur_tree, cur_menu);
          if ( (cur_item != NIL) &&
               ( do_chg( cur_tree, cur_item, SELECTED, FALSE, FALSE, TRUE) ) )
          {
                                                /* only return TRUE when*/
                                                /*   item is enabled and*/
                                                /*   is not NIL         */
             *ptitle = cur_title;
             *pitem = cur_item;
             done = TRUE;
          }
          else
            do_chg( tree, cur_title, SELECTED, FALSE, TRUE, TRUE);
        }
        return(done);
}


/*
*       Routine to display the menu bar.  Clipping is turned completely
*       off so that this operation will be as fast as possible.  The
*       global variable gl_mntree is also set or reset.
*/
void mn_bar(LONG tree, WORD showit, WORD pid)
{
        PD              *p;

        p = fpdnm(NULLPTR, pid);
        
        if ( showit )
        {
          gl_mnppd = p;
          menu_tree[pid] = gl_mntree = tree;
          menu_fixup(&p->p_name[0]);
          LWSET(OB_WIDTH(1), gl_width - LWGET(OB_X(1)));
          ob_actxywh(gl_mntree, THEACTIVE, (GRECT *)&gl_ctwait.m_x);
          gsx_sclip(&gl_rzero);
          ob_draw(gl_mntree, THEBAR, MAX_DEPTH);
          gsx_cline(0, gl_hbox - 1, gl_width - 1, gl_hbox - 1);
        }
        else
        {
          menu_tree[pid] = gl_mntree = 0x0L;
          rc_copy(&gl_rmenu, (GRECT *)&gl_ctwait.m_x);
        }
                                                /* make ctlmgr fix up   */
                                                /*   the size of rect   */
                                                /*   its waiting for    */
                                                /*   by sending fake key*/
        post_keybd(ctl_pd->p_cda, 0x0000);
}

#if SINGLAPP
/*
*       Routine to tell all desk accessories that the currently running
*       application is about to terminate.
*/
void mn_clsda()
{
        register WORD   i;

        for (i=0; i<NUM_DESKACC; i++)
        {
          if (desk_ppd[i])
            ap_sendmsg(appl_msg, AC_CLOSE, desk_ppd[i], i, 0, 0, 0, 0);
        }
}
#endif


/*
*       Routine to register a desk accessory item on the menu bar.
*       The return value is the object index of the menu item that
*       was added.
*/
WORD mn_register(WORD pid, LONG pstr)
{
        WORD            openda;
        WORD            len;
        BYTE            *ptmp;
#if MULTIAPP
        LONG            pdest;
#endif
                                                /* add desk acc. if room*/
        if ( (pid >= 0) &&
             (gl_dacnt < NUM_DESKACC) )
        {
          gl_dacnt++;
          openda = 0;
          while( desk_acc[openda] )
            openda++;
          desk_ppd[openda] = rlr;
          desk_acc[openda] = ADDR(desk_str[openda]);

#if MULTIAPP
          pdest = desk_acc[openda];
          if (sh[pid].sh_isacc)
          {
            LBSET(pdest, '*');
            pdest += 1;
          }
          len = strlencpy((char *) pdest, (char *) pstr);
#endif
#if SINGLAPP
          len = strlencpy((char *) desk_acc[openda], (char *) pstr);
#endif
          ptmp = desk_str[openda] + len + 1;
          while( *ptmp == ' ' )
          {
            *ptmp = NULL;
            ptmp--;
          } 
          strcat(desk_str[openda], " ");

          menu_fixup(&rlr->p_name[0]);
          return(openda);
        }
        else
          return(-1);
}       


/*
*       Routine to unregister a desk accessory item on the menu bar.
*/
void mn_unregister(WORD da_id)
{
        WORD i, j;

        for (i=da_id; i<gl_dacnt-1; i++)
        { 
          for (j=0; j<22; j++)
            *(desk_str[i]+j) = *(desk_str[i+1]+j);
          desk_ppd[i] = desk_ppd[i+1];
        }
        gl_dacnt--;
        desk_ppd[gl_dacnt] = (PD *)0x0;
        desk_acc[gl_dacnt] = 0x0L;
#if SINGLAPP
        menu_fixup(&rlr->p_name[0]);
#endif
}       

#if MULTIAPP
/*
*       Given a PD * , find its desk accessory id
*/
        WORD
mn_ppdtoid(p)
        PD      *p;
{
        WORD i;

        for (i=0; i<NUM_DESKACC; i++)
          if (desk_ppd[i] == p) 
            return(i);
        return(NIL);                            /* should never get here*/
}
#endif

