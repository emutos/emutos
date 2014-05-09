/*      DESKTOP.C       05/04/84 - 09/05/85     Lee Lorenzen            */
/*      for 3.0         3/12/86  - 1/29/87      MDF                     */

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
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include <string.h>

#include "xbiosbind.h"
#include "portab.h"
#include "obdefs.h"
#include "dos.h"
#include "gembind.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskbind.h"

#include "kprint.h"
#include "lineavars.h"
#include "machine.h"
#include "screen.h"
#include "videl.h"
#include "nls.h"
#include "version.h"

#include "optimize.h"
#include "optimopt.h"
#include "rectfunc.h"
#include "gemdos.h"
#include "aesbind.h"
#include "desksupp.h"
#include "deskglob.h"
#include "deskgraf.h"
#include "deskins.h"
#include "deskinf.h"
#include "deskdir.h"
#include "deskrsrc.h"
#include "deskfun.h"
#include "deskpro.h"
#include "deskact.h"
#include "desk1.h"
#include "deskrez.h"
#include "kprint.h"
#include "deskmain.h"

#define abs(x) ( (x) < 0 ? -(x) : (x) )
#define menu_text(tree,inum,ptext) (((OBJECT *)(tree)+(inum))->ob_spec = ptext)

/* BugFix       */
                                        /* keyboard shortcuts & others  */
#define ESCAPE 0x011B                   /* resort directory in window   */
#define ALTA 0x1E00                     /* Configure App                */
#define ALTC 0x2E00                     /* Change resolution            */
#define ALTD 0x2000                     /* Delete                       */
#define ALTI 0x1700                     /* Info/Rename                  */
#define ALTN 0x3100                     /* Sort by Name                 */
#define ALTP 0x1900                     /* Sort by Type                 */
#define ALTS 0x1F00                     /* Show as Text/Icons           */
#define ALTT 0x1400                     /* Sort by Date                 */
#define ALTV 0x2F00                     /* Save Desktop                 */
#define ALTZ 0x2C00                     /* Sort by Size                 */
#define CNTLU 0x1615                    /* To Output                    */
#define CNTLZ 0x2c1a                    /* Execute CLI                  */
#define CNTLQ 0x1011                    /* Exit To Dos                  */
#define ARROW_UP    0x4800
#define ARROW_DOWN  0x5000
#define ARROW_LEFT  0x4b00
#define ARROW_RIGHT 0x4d00


#define BEG_UPDATE 1
#define END_UPDATE 0


/* */
#define SPACE 0x20



GLOBAL BYTE     gl_amstr[4];
GLOBAL BYTE     gl_pmstr[4];

GLOBAL WORD     gl_apid;


/* forward declaration  */
static void    cnx_put(void);


/* BugFix       */
static WORD     ig_close;

#ifndef DESK1
static const BYTE     ILL_ITEM[] = {L1ITEM, L2ITEM, L3ITEM, L4ITEM, L5ITEM, NFOLITEM, CLSWITEM, 0};
#else
static const BYTE     ILL_ITEM[] = {L1ITEM, L2ITEM, L3ITEM, L4ITEM, L5ITEM, 0};
#endif

static const BYTE     ILL_FILE[] = {FORMITEM,IDSKITEM,0};
static const BYTE     ILL_DOCU[] = {FORMITEM,IDSKITEM,IAPPITEM,0};
static const BYTE     ILL_FOLD[] = {FORMITEM,IDSKITEM,IAPPITEM,0};
static const BYTE     ILL_FDSK[] = {DELTITEM,IAPPITEM,0};
static const BYTE     ILL_HDSK[] = {FORMITEM,DELTITEM,IAPPITEM,0};
static const BYTE     ILL_NOSEL[] = {OPENITEM,SHOWITEM,FORMITEM,DELTITEM,
                                IDSKITEM,IAPPITEM,0};
static const BYTE     ILL_YSEL[] = {OPENITEM, IDSKITEM, FORMITEM, SHOWITEM, 0};

#ifdef DESK1
static const BYTE     ILL_TRASH[] = {OPENITEM,FORMITEM,DELTITEM,IDSKITEM,IAPPITEM,0};
static const BYTE     ILL_NOTOP[] = {NFOLITEM,CLOSITEM,CLSWITEM,0};
static const BYTE     ILL_DESKTOP[] = {NFOLITEM,CLOSITEM,CLSWITEM,ICONITEM,
                                NAMEITEM,DATEITEM,SIZEITEM,TYPEITEM,0};
#endif

#if CONF_WITH_EASTER_EGG
/* easter egg */
static const WORD  freq[]=
{
        262, 349, 329, 293, 349, 392, 440, 392, 349, 329, 262, 293,
        349, 262, 262, 293, 330, 349, 465, 440, 392, 349, 698
};

static const WORD  dura[]=
{
        4, 12, 4, 12, 4, 6, 2, 4, 4, 12, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 12, 4, 8, 4
};
#endif


static LONG     ad_ptext;
static LONG     ad_picon;

#ifndef DESK1
GLOBAL GRECT    gl_savewin[NUM_WNODES]; /* preserve window x,y,w,h      */
GLOBAL GRECT    gl_normwin;             /* normal (small) window size   */
static WORD     gl_open1st;             /* index of window to open 1st  */
#endif
static BYTE     gl_defdrv;              /* letter of lowest drive       */

static WORD     can_iapp;               /* TRUE if INSAPP enabled       */
static WORD     can_show;               /* TRUE if SHOWITEM enabled     */
static WORD     can_del;                /* TRUE if DELITEM enabled      */

GLOBAL WORD     gl_whsiztop;            /* wh of window fulled          */
static WORD     gl_idsiztop;            /* id of window fulled          */




/* Function prototypes: */
#ifndef DESK1
static void hot_close(WORD wh);
#endif


static int can_change_resolution;

static void detect_features(void)
{
    can_change_resolution = rez_changeable();
}


#if CONF_DEBUG_DESK_STACK
extern LONG deskstackbottom[];

static void display_free_stack(void)
{
    LONG *p;

    for (p = deskstackbottom; ; p++)
        if (*p != STACK_MARKER)
            break;

    kprintf("Desktop stack has %ld bytes available\n",
            (p-deskstackbottom)*sizeof(LONG));
}
#else
#define display_free_stack()
#endif


#ifndef DESK1
static void fix_wins(void)
{
/* this routine is supposed to keep track of the windows between        */
/* runs of the Desktop. it assumes pws has already been set up;         */
/* gl_savewin is set up by app_start.                                   */
        WSAVE           *pws0, *pws1;

        gl_open1st = 0;                         /* default case         */
        pws0 = &G.g_cnxsave.win_save[0];
        pws1 = &G.g_cnxsave.win_save[1];
                                                /* upper window is full */
        if ( (gl_savewin[0].g_y != gl_savewin[1].g_y) &&
             (gl_savewin[0].g_h != gl_savewin[1].g_h) )
        {
                                        /* which window is upper/lower? */
          if (gl_savewin[0].g_h > gl_savewin[1].g_h)
          {                             /* [0] is (full) upper window   */
            gl_open1st = 1;
            gl_idsiztop = 0;
            pws0->y_save = G.g_yfull;
            pws0->h_save = G.g_hfull;
            pws1->y_save = gl_normwin.g_y + gl_normwin.g_h + (gl_hbox / 2);
            pws1->h_save = gl_normwin.g_h;
          }
          else
          {                             /* [1] is (full) upper window   */
            gl_open1st = 0;
            gl_idsiztop = 1;
            pws1->y_save = G.g_yfull;
            pws1->h_save = G.g_hfull;
            pws0->y_save = gl_normwin.g_y + gl_normwin.g_h + (gl_hbox / 2);
            pws0->h_save = gl_normwin.g_h;
          } /* else */
        } /* if */
                                        /* case 3: lower window is full */
        else if ( (gl_savewin[0].g_y == gl_savewin[1].g_y) &&
                  (gl_savewin[0].g_h != gl_savewin[1].g_h) )
        {
                                        /* which window is upper/lower? */
          if (gl_savewin[0].g_h > gl_savewin[1].g_h)
          {                             /* [0] is (full) lower window   */
            gl_open1st = 1;
            gl_idsiztop = 0;
            pws0->y_save = G.g_yfull;
            pws0->h_save = G.g_hfull;
            pws1->y_save = gl_normwin.g_y;
            pws1->h_save = gl_normwin.g_h;
          }
          else
          {                             /* [1] is (full) lower window   */
            gl_open1st = 0;
            gl_idsiztop = 1;
            pws1->y_save = G.g_yfull;
            pws1->h_save = G.g_hfull;
            pws0->y_save = gl_normwin.g_y;
            pws0->h_save = gl_normwin.g_h;
          } /* else */
        } /* if */
} /* fix_wins */
#endif


/*
*       Turn on the hour glass to signify a wait and turn it off when were
*       done.
*/
static void desk_wait(WORD turnon)
{
        graf_mouse( (turnon) ? HGLASS : ARROW, 0x0L);
}


/*
*       Routine to update all of the desktop windows
*/
static void desk_all(WORD sort)
{
        desk_wait(TRUE);
        if (sort)
          win_srtall();
        win_bdall();
        win_shwall();
        desk_wait(FALSE);
}


/*
*       Given an icon index, go find the ANODE which it represents
*/
ANODE *i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp)
{
        ANODE   *pa;
        BYTE    *pname;
        WNODE   *pw;
        FNODE   *pf;

        pa = (ANODE *) NULL;
        pf = (FNODE *) NULL;

#ifdef DESK1
        if (!wh)        /* On desktop? */
        {
            if (G.g_screen[item].ob_type == G_ICON)
                pname = win_iname(item);
            else
                pname = "";
            pa = app_afind(TRUE, -1, item, pname, pisapp);
        }
        else
        {
#endif
        pw = win_find(wh);
        pf = fpd_ofind(pw->w_path->p_flist, item);
        if (pf)
        {
          pname = &pf->f_name[0];
#ifdef DESK1
          if (pf->f_attr & F_SUBDIR)
          {
                pa = app_afind(FALSE, 1, -1, pname, pisapp);
          }
          else
          {
                pa = app_afind(FALSE, 0, -1, pname, pisapp);
          }
#else
          pa = pf->f_pa;
          if ( (pf->f_attr & F_DESKTOP) ||
               (pf->f_attr & F_SUBDIR) )
            *pisapp = FALSE;
          else
            *pisapp = wildcmp(pa->a_pappl, pname);
#endif
        }
#ifdef DESK1
        }
#endif
        *ppf = pf;
        return (pa);
}


/*
*       Enable/Disable the menu items in dlist
*/
static void men_list(LONG mlist, const BYTE *dlist, WORD enable)
{
        while (*dlist)
          menu_ienable(mlist, *dlist++, enable);
}


/*
*       Based on current selected icons, figure out which
*       menu items should be selected (deselected)
*/
static void men_update(LONG tree)
{
        WORD            item, nsel, *pjunk, isapp;
        const BYTE      *pvalue;
        ANODE           *appl;

        pvalue = 0;
                                                /* enable all items     */
        for (item = OPENITEM; item <= PREFITEM; item++)
          menu_ienable(tree, item, TRUE);

        can_iapp = TRUE;
        can_show = TRUE;
        can_del = TRUE;
                                                /* disable some items   */
        men_list(tree, ILL_ITEM, FALSE);

        nsel = 0;
        for (item = 0; (item = win_isel(G.g_screen, G.g_croot, item)) != 0;
             nsel++)
        {
          appl = i_find(G.g_cwin, item, (FNODE **)(void*)&pjunk, &isapp);
          switch (appl->a_type)
          {
            case AT_ISFILE:
                if ( (isapp) || is_installed(appl) )
                  pvalue = ILL_FILE;
                else
                {
                  pvalue = ILL_DOCU;
                  can_iapp = FALSE;
                }
                break;
            case AT_ISFOLD:
                pvalue = ILL_FOLD;
                can_iapp = FALSE;
                break;
            case AT_ISDISK:
                pvalue = (appl->a_aicon == IG_FLOPPY) ? ILL_FDSK : ILL_HDSK;
                can_iapp = FALSE;
                can_del = FALSE;
                break;
#ifdef DESK1
            case AT_ISTRSH:                    /* Trash */
                pvalue = ILL_TRASH;
                can_del = FALSE;
                break;
#endif
          } /* switch */
          men_list(tree, pvalue, FALSE);       /* disable certain items */
        } /* for */

#ifdef DESK1
        if (win_ontop())
          pvalue = ILL_DESKTOP;
        else
          pvalue = ILL_NOTOP;
        if (pvalue == ILL_DESKTOP)
          men_list(tree, pvalue, TRUE);
        else
          men_list(tree, pvalue, FALSE);
#endif

        if ( nsel != 1 )
        {
          if (nsel)
          {
            pvalue = ILL_YSEL;
            can_show = FALSE;
          }
          else
          {
            pvalue = ILL_NOSEL;
            can_show = FALSE;
            can_del = FALSE;
            can_iapp = FALSE;
          }
          men_list(tree, pvalue, FALSE);
        } /* if */

#if WITH_CLI == 0
        menu_ienable(tree, CLIITEM, FALSE);
#endif

} /* men_update */


static WORD do_deskmenu(WORD item)
{
        WORD            done, touchob;
        LONG            tree;
        OBJECT *obj;

        done = FALSE;
        switch( item )
        {
          case ABOUITEM:
                display_free_stack();
                tree = G.a_trees[ADDINFO];
                                                /* draw the form        */
                show_hide(FMD_START, tree);
                while( !done )
                {
                  touchob = form_do(tree, 0);
                  touchob &= 0x7fff;
                  if ( touchob == DEICON )
                  {
#if CONF_WITH_EASTER_EGG
                    int i;
                    for(i=0; i<23; i++)
                    {
                      sound(TRUE, freq[i], dura[i]);
                      evnt_timer(dura[i]*64, 0);
                    }
#endif
                  }
                  else
                    done = TRUE;
                }
                obj = (OBJECT *)tree + DEOK;
                obj->ob_state = NORMAL;
                show_hide(FMD_FINISH, tree);
                done = FALSE;
                break;
        }
        return(done);
}


static WORD do_filemenu(WORD item)
{
        WORD    done;
        WORD    curr;
        WNODE   *pw;
/*
        WORD    junk, first;
        FNODE   *pf;
        BYTE    *pdst;
*/

        done = FALSE;
#ifdef DESK1
        pw = win_ontop();
#else
        pw = win_find(G.g_cwin);
#endif
        curr = win_isel(G.g_screen, G.g_croot, 0);
        switch( item )
        {
          case OPENITEM:
                if (curr)
                  done = do_open(curr);
                break;
          case SHOWITEM:
                if (curr)
                  do_info(curr);
                break;
#ifdef DESK1
          case NFOLITEM:
                if (pw)
                  fun_mkdir(pw);
                break;
          case CLOSITEM:
                if (pw)
                  fun_close(pw, 0);
                break;
          case CLSWITEM:
                if (pw)
                  fun_close(pw, 1);
                break;
#else
          case CLOSITEM:
          case CLSWITEM:
                hot_close(G.g_cwin);
                break;
#endif
          case DELTITEM:
                if (curr)
                  fun_del(pw);
                break;
          case FORMITEM:
                if (curr)
                  done = do_format(curr);
                break;

          case CLIITEM:                         /* Start EmuCON */
                G.g_tail[0] = G.g_tail[1] = 0;
                strcpy(G.g_cmd, "EMUCON");
                done = pro_run(FALSE, TRUE, -1, -1);
                break;

          case QUITITEM:
                display_free_stack();
                pro_exit(G.g_cmd, G.g_tail);
                done = TRUE;
                break;
        }
        return(done);
} /* do_filemenu */


static WORD do_viewmenu(WORD item)
{
        WORD            newview, newsort;
        LONG            ptext;

        newview = G.g_iview;
        newsort = G.g_isort;
        switch( item )
        {
          case ICONITEM:
#if CONF_WITH_DESK1
                newview = (G.g_iview == V_ICON) ? V_TEXT : V_ICON;
#else
                newview = V_TEXT;
#endif
                break;
          case NAMEITEM:
                newsort = S_NAME;
                break;
          case DATEITEM:
                newsort = S_DATE;
                break;
          case SIZEITEM:
                newsort = S_SIZE;
                break;
          case TYPEITEM:
                newsort = S_TYPE;
                break;
        }
        if ( (newview != G.g_iview) ||
             (newsort != G.g_isort) )
        {
          if (newview != G.g_iview)
          {
            G.g_iview = newview;
            ptext = (newview == V_TEXT) ? ad_picon : ad_ptext;
            menu_text(G.a_trees[ADMENU], ICONITEM, ptext);
          }
          if (newsort != G.g_isort)
          {
            menu_icheck(G.a_trees[ADMENU], G.g_csortitem, FALSE);
            G.g_csortitem = item;
            menu_icheck(G.a_trees[ADMENU], item, TRUE);
          }
          win_view(newview, newsort);
          return(TRUE);                 /* need to rebuild      */
        }
        return( FALSE );
}



static WORD do_optnmenu(WORD item)
{
        ANODE           *pa;
        WORD            done, rebld, curr;
        FNODE           *pf = NULL;
        WORD            isapp = FALSE;
        WORD            newres, newmode;
        BYTE            *pstr;
#ifdef DESK1
        GRECT           rect;
#endif

        pa = 0;
        pstr = 0;

        done = FALSE;
        rebld = FALSE;

        curr = win_isel(G.g_screen, G.g_croot, 0);
        if (curr)
          pa = i_find(G.g_cwin, curr, &pf, &isapp);

        switch( item )
        {
          case IDSKITEM:
                if (pa)
                  rebld = ins_disk(pa);
                if (rebld)
                {
#ifdef DESK1
                  app_blddesk();
                  wind_get(0, WF_WXYWH, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
                  do_wredraw(0, rect.g_x, rect.g_y, rect.g_w, rect.g_h);
#else
                  gl_defdrv = app_blddesk();
                  do_chkall(TRUE);
#endif
                }
                break;
          case IAPPITEM:
                if (pa)
                {
                  if (isapp)
                    pstr = &pf->f_name[0];
                  else if (is_installed(pa))
                    pstr = pa->a_pappl;
                  rebld = ins_app(pstr, pa);
                  rebld = TRUE; /* WORKAROUND for bug that shows up with */
                                /* remove followed by cancel when icon is */
                                /* partially covered by dialog.  Icon not */
                                /* properly redrawn as deselected -- mdf   */
                }
                if (rebld)
                  desk_all(FALSE);
                break;
          case PREFITEM:
                if (inf_pref())
                  desk_all(FALSE);
                break;
          case SAVEITEM:
                desk_wait(TRUE);
                cnx_put();
                app_save(TRUE);
                desk_wait(FALSE);
                break;
          case RESITEM:
                rebld = change_resolution(&newres,&newmode);
                if (rebld == 1)
                {
                  if (FALSE)
                  {
                    /* Dummy case for conditional compilation */
                  }
#if CONF_WITH_VIDEL
                  else if (newres == FALCON_REZ)
                    shel_write(5,newmode,1,NULL,NULL);
#endif
#if CONF_WITH_SHIFTER
                  else shel_write(5,newres+2,0,NULL,NULL);
#endif
                  done = TRUE;
                }
                break;
        }
        return(done);
}



static WORD hndl_button(WORD clicks, WORD mx, WORD my, WORD button, WORD keystate)
{
        WORD            done, junk;
        GRECT           c;
        WORD            wh, dobj, dest_wh;
#ifdef DESK1
        WORD            root;
        WNODE           *wn;
#endif

        done = FALSE;

        wh = wind_find(mx, my);

        if (wh != G.g_cwin)
          desk_clear(G.g_cwin);

/* BUGFIX       */
#ifndef DESK1
        if (wh == 0)                            /* if click outside win's*/
        {
          men_update(G.a_trees[ADMENU]);
          wind_update(BEG_UPDATE);
          while(button & 0x0001)
            graf_mkstate(&junk, &junk, &button, &junk);
          wind_update(END_UPDATE);
          return(done);
        }
#endif
/* */
        desk_verify(wh, FALSE);

        wind_get(wh, WF_WXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h);

        if (clicks == 1)
        {
          act_bsclick(G.g_cwin, G.g_screen, G.g_croot, mx, my,
                      keystate, &c, FALSE);
          graf_mkstate(&junk, &junk, &button, &junk);
          if (button & 0x0001)
          {
            dest_wh = act_bdown(G.g_cwin, G.g_screen, G.g_croot, &mx, &my,
                                keystate, &c, &dobj);
#ifdef DESK1
            if (dest_wh != NIL)
            {
                if (dest_wh == 0)
                {
                  root = 1;
                }
                else
                {
                  wn = win_find(dest_wh);
                  root = wn->w_root;
                }
                desk1_drag(wh, dest_wh, root, dobj, mx, my);
                desk_clear(wh);
            } /* if !NIL */
#else
            if ( (dest_wh != NIL ) && (dest_wh != 0) )
            {
              fun_drag(wh, dest_wh, dobj, mx, my);
              desk_clear(wh);
            } /* if !NIL */
#endif
          } /* if button */
        } /* if clicks */
        else
        {
          act_bsclick(G.g_cwin, G.g_screen, G.g_croot, mx, my, keystate, &c, TRUE);
          done = do_filemenu(OPENITEM);
        } /* else */
        men_update(G.a_trees[ADMENU]);
        return(done);
}



static WORD hndl_menu(WORD title, WORD item)
{
        WORD            done;

        done = FALSE;
        switch( title )
        {
          case DESKMENU:
                done = do_deskmenu(item);
                break;
          case FILEMENU:
                done = do_filemenu(item);
                break;
          case VIEWMENU:
                done = FALSE;
                                                /* for every window     */
                                                /*   go sort again and  */
                                                /*   rebuild views      */
                if (do_viewmenu(item))
                desk_all(TRUE);
                break;
          case OPTNMENU:
                done = do_optnmenu(item);
                break;
        }
        menu_tnormal(G.a_trees[ADMENU], title, TRUE);
        return(done);
}



/* Simulate WM_ARROWED using arrow keys */
static void kbd_arrow(WORD type)
{
    WORD wh;
    WORD dummy;
    WNODE *pw;

    wind_get(0, WF_TOP, &wh, &dummy, &dummy, &dummy);
    if (!wh)
        return;

    pw = win_find(wh);
    if (!pw)
        return;

    desk_clear(wh);
    win_arrow(wh, type);
}



static WORD hndl_kbd(WORD thechar)
{
        WORD            done;

        done = FALSE;

        switch(thechar)
        {
          case ESCAPE:
                do_chkall(TRUE);
                break;
          case ALTA:    /* Options: Install App */
                if (can_iapp)           /* if it's ok to install app    */
                {
                  menu_tnormal(G.a_trees[ADMENU], OPTNMENU, FALSE);
                  done = hndl_menu(OPTNMENU, IAPPITEM);
                }
                break;
          case ALTC:    /* Options: Change resolution  */
                if (!can_change_resolution)
                    break;

                menu_tnormal(G.a_trees[ADMENU], OPTNMENU, FALSE);
                done = hndl_menu(OPTNMENU, RESITEM);
                break;
          case ALTD:    /* File: Delete         */
                if (can_del)            /* if it's ok to delete         */
                {
                  menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
                  done = hndl_menu(FILEMENU, DELTITEM);
                }
                break;
          case ALTI:    /* File: Info/Rename    */
                if (can_show)           /* if it's ok to show           */
                {
                  menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
                  done = hndl_menu(FILEMENU, SHOWITEM);
                }
                break;
          case ALTN:    /* View: Sort by Name   */
                menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
                done = hndl_menu(VIEWMENU, NAMEITEM);
                break;
          case ALTP:    /* View: Sort by Type   */
                menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
                done = hndl_menu(VIEWMENU, TYPEITEM);
                break;
          case ALTS:    /* View: Show as Text/Icons     */
                menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
                done = hndl_menu(VIEWMENU, ICONITEM);
                break;
          case ALTT:    /* View: Sort by Date   */
                menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
                done = hndl_menu(VIEWMENU, DATEITEM);
                break;
          case ALTV:    /* Options: Save Desktop        */
                menu_tnormal(G.a_trees[ADMENU], OPTNMENU, FALSE);
                done = hndl_menu(OPTNMENU, SAVEITEM);
                break;
          case ALTZ:    /* View: Sort by Size   */
                menu_tnormal(G.a_trees[ADMENU], VIEWMENU, FALSE);
                done = hndl_menu(VIEWMENU, SIZEITEM);
                break;
#if WITH_CLI != 0
          case CNTLZ:
                menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
                done = hndl_menu(FILEMENU, CLIITEM);
                break;
#endif
          case CNTLQ:
                menu_tnormal(G.a_trees[ADMENU], FILEMENU, FALSE);
                done = hndl_menu(FILEMENU, QUITITEM);
                break;
          case ARROW_UP:
                kbd_arrow(WA_UPLINE);
                break;
          case ARROW_DOWN:
                kbd_arrow(WA_DNLINE);
                break;
          case ARROW_LEFT:
                kbd_arrow(WA_LFLINE);
                break;
          case ARROW_RIGHT:
                kbd_arrow(WA_RTLINE);
                break;
        } /* switch */
        men_update(G.a_trees[ADMENU]);          /* clean up menu info   */
        return(done);
} /* hndl_kbd */



#ifndef DESK1
static void hot_close(WORD wh)
{
/* "close" the path but don't set a new dir. & don't redraw the window  */
/* until the button is up or there are no more CLOSED messages          */
        WORD            drv, done, at_drvs, cnt;
        WORD            mx, my, button, kstate;
        BYTE            path[LEN_ZPATH+1], name[LEN_ZNODE+1], ext[LEN_ZEXT+1];
        BYTE            *pend, *pname, *ppath;
        WNODE           *pw;

        pw = win_find(wh);
        at_drvs = FALSE;
        done = FALSE;
        cnt = 0;
/*      wind_update(BEG_UPDATE);*/
        do
        {
          cnt++;
          ppath = &pw->w_path->p_spec[0];
          pname = &pw->w_name[0];
          fpd_parse(ppath, &drv, &path[0], &name[0], &ext[0]);
          if (path[0] == NULL)  /* No more path; show disk icons */
          {
            strcpy(pname, ini_str(STDSKDRV));
            ppath[3] = '*';
            ppath[4] = '.';
            ppath[5] = '*';
            ppath[6] = NULL;
            at_drvs = TRUE;
          }
          else
          {                     /* Some path left; scan off last dir.   */
            pend = pname;
            pend += (strlen(pname) - 3);        /* skip trailing '\ '   */
                                                /* Remove last name     */
            while ( (pend != pname) && (*pend != '\\') )
              pend--;
            pend++;                             /* save trailing '\'    */
/*          *pend++ = SPACE;*/
            *pend = NULL;
                                                /* now fix path         */
            pend = ppath;
            pend += (strlen(ppath) - 5);        /* skip trailing '\*.*' */
                                                /* Remove last name     */
            while ( (pend != ppath) && (*pend != '\\') )
              pend--;
            pend++;                             /* save trailing '\'    */
            *pend = NULL;
            strcat(ppath, "*.*");               /* put '*.*' back on    */
          } /* else */
          wind_set(pw->w_id, WF_NAME, pw->w_name, 0, 0);
/*        wind_update(END_UPDATE);*/
          graf_mkstate(&mx, &my, &button, &kstate);
          if (button == 0x0)
            done = TRUE;
          else
          {
            evnt_timer(750, 0);
                                                /* grab the screen      */
/*          wind_update(BEG_UPDATE);*/
            graf_mkstate(&mx, &my, &button, &kstate);
            if (button == 0x0)
              done = TRUE;
          } /* else */
        } while ( !done );
/*      wind_update(END_UPDATE);*/
        if (cnt > 1)
          ig_close = TRUE;
/*      desk_verify(G.g_rmsg[3], FALSE);*/
        fpd_parse(ppath, &drv, &path[0], &name[0], &ext[0]);
        if (at_drvs)
          drv = '@';
        pw->w_cvrow = 0;                        /* reset slider         */
        do_fopen(pw, 0, drv, path, name, ext, FALSE, TRUE);
        cnx_put();
} /* hot_close */
#endif


WORD hndl_msg(void)
{
        WORD            done;
        WNODE           *pw;
        WORD            change, menu;
#ifdef DESK1
        WORD            x,y,w,h;
        WORD            cols, redraw;
#endif

        done = change = menu = FALSE;
        if ( G.g_rmsg[0] == WM_CLOSED && ig_close )
        {
          ig_close = FALSE;
          return(done);
        }
        switch( G.g_rmsg[0] )
        {
          case WM_TOPPED:
          case WM_CLOSED:
          case WM_FULLED:
          case WM_ARROWED:
          case WM_VSLID:
#ifdef DESK1
          case WM_HSLID:
          case WM_SIZED:
          case WM_MOVED:
#endif
                desk_clear(G.g_cwin);
                break;
        }
        switch( G.g_rmsg[0] )
        {
          case MN_SELECTED:
                desk_verify(G.g_wlastsel, FALSE);
                done = hndl_menu(G.g_rmsg[3], G.g_rmsg[4]);
                break;
          case WM_REDRAW:
                menu = TRUE;
                if (G.g_rmsg[3])
                {
                  do_wredraw(G.g_rmsg[3], G.g_rmsg[4], G.g_rmsg[5],
                                        G.g_rmsg[6], G.g_rmsg[7]);
                }
                break;
          case WM_TOPPED:
                wind_set(G.g_rmsg[3], WF_TOP, 0, 0, 0, 0);
#ifdef DESK1
                wind_get(G.g_rmsg[3], WF_WXYWH, &x, &y, &w, &h);
#endif
                pw = win_find(G.g_rmsg[3]);
                if (pw)
                {
                  win_top(pw);
                  desk_verify(pw->w_id, FALSE);
                }
                change = TRUE;
#ifdef DESK1
                G.g_wlastsel = pw->w_id;
#endif
                break;
          case WM_CLOSED:
#ifdef DESK1
                do_filemenu(CLOSITEM);
#else
                hot_close(G.g_rmsg[3]);
#endif
                break;
          case WM_FULLED:
                pw = win_find(G.g_rmsg[3]);
                if (pw)
                {
                  win_top(pw);
                  do_wfull(G.g_rmsg[3]);
                  desk_verify(G.g_rmsg[3], TRUE);
                }
                change = TRUE;
                break;
          case WM_ARROWED:
                win_arrow(G.g_rmsg[3], G.g_rmsg[4]);
                break;
          case WM_VSLID:
                win_slide(G.g_rmsg[3], G.g_rmsg[4]);
                break;
#ifdef DESK1
          case WM_MOVED:
          case WM_SIZED:
                x = G.g_rmsg[4];
                y = G.g_rmsg[5];
                do_xyfix(&x, &y);
                wind_set(G.g_rmsg[3], WF_CXYWH, x, y, G.g_rmsg[6], G.g_rmsg[7]);
                if (G.g_rmsg[0] == WM_SIZED)
                {
                    /*
                     * if our window has shrunk AND we're displaying a
                     * different number of columns, we need to send a
                     * redraw message because the AES won't
                     */
                    redraw = FALSE;
                    wind_get(G.g_rmsg[3], WF_PXYWH, &x, &y, &w, &h);
                    if ((G.g_rmsg[6] <= w) && (G.g_rmsg[7] <= h))
                        redraw = TRUE;
                    pw = win_find(G.g_rmsg[3]); /* get ptr to WNODE */
                    cols = pw->w_pncol;         /* old # cols displayed */
                    desk_verify(G.g_rmsg[3], TRUE);
                    if (redraw && (pw->w_pncol != cols)) {
                        wind_get(G.g_rmsg[3], WF_WXYWH, &x, &y, &w, &h);
                        fun_msg(WM_REDRAW, G.g_rmsg[3], x, y, w, h);
                    }
                }
                else
                {
                        wind_get(G.g_rmsg[3],WF_WXYWH, &x, &y, &w, &h);
                        pw = win_find(G.g_rmsg[3]);
                        r_set((GRECT *)(&G.g_screen[pw->w_root].ob_x), x, y, w, h);
                }
                change = TRUE;
                break;
#endif
        }
        if (change)
          cnx_put();
        G.g_rmsg[0] = 0;
        if (!menu)
          men_update(G.a_trees[ADMENU]);
        return(done);
} /* hndl_msg */


static void cnx_put(void)
{
        WORD            iwin;
        WSAVE           *pws;
        WNODE           *pw;
#ifdef DESK1
        WORD            iwsave;
#endif

        G.g_cnxsave.vitem_save = (G.g_iview == V_ICON) ? 0 : 1;
        G.g_cnxsave.sitem_save = G.g_csortitem - NAMEITEM;
        G.g_cnxsave.ccopy_save = G.g_ccopypref;
        G.g_cnxsave.cdele_save = G.g_cdelepref;
        G.g_cnxsave.cdclk_save = G.g_cdclkpref;
        G.g_cnxsave.covwr_save = G.g_covwrpref;
        G.g_cnxsave.cmclk_save = G.g_cmclkpref;
        G.g_cnxsave.ctmfm_save = G.g_ctimeform;
        G.g_cnxsave.cdtfm_save = G.g_cdateform;

#ifdef DESK1
        for (iwsave=0, iwin = 0; iwin < NUM_WNODES; iwin++)
        {
                pw = win_ith(iwin + 1);
                if (!pw->w_id) continue;

                pws = &G.g_cnxsave.win_save[iwsave++];
                wind_get(pw->w_id, WF_CXYWH, &pws->x_save, &pws->y_save,
                   &pws->w_save, &pws->h_save);
                do_xyfix(&pws->x_save, &pws->y_save);
                pws->vsl_save  = pw->w_cvrow;
                pws->obid_save = pw->w_obid;
                strcpy(pws->pth_save, pw->w_path->p_spec);
        }
        while (iwsave < NUM_WNODES)
        {
                pws = &G.g_cnxsave.win_save[iwsave++];
                pws->obid_save = 0;
                pws->pth_save[0] = 0;
        }
#else
        for (iwin = 0; iwin < NUM_WNODES; iwin++)
        {
          pw = win_find(G.g_wlist[iwin].w_id);
          pws = &G.g_cnxsave.win_save[iwin];
          wind_get(pw->w_id, WF_CXYWH, &pws->x_save, &pws->y_save,
                   &pws->w_save, &pws->h_save);
          do_xyfix(&pws->x_save, &pws->y_save);
          pws->vsl_save = pw->w_cvrow;
          pws->obid_save = 0;
          strcpy(&pws->pth_save[0], pw->w_path->p_spec);
        } /* for */
#endif
} /* cnx_put */


/************************************************************************/
/* c n x _ o p e n                                                      */
/************************************************************************/
#ifndef DESK1
static void cnx_open(WORD idx)
{
        WSAVE           *pws;
        WNODE           *pw;
        WORD            drv;
        BYTE            pname[9];
        BYTE            pext[4];

        pws = &G.g_cnxsave.win_save[idx];
        pw = win_alloc();
        if (pw)
        {
          if (idx == gl_idsiztop)               /* if a window is fulled*/
            gl_whsiztop = pw->w_id;             /*  save the wh         */
          pw->w_cvrow = pws->vsl_save;
          fpd_parse(&pws->pth_save[0], &drv, &G.g_tmppth[0],
                    &pname[0], &pext[0]);
          if (pname[0] == NULL)
            drv = NULL;
          do_xyfix(&pws->x_save, &pws->y_save);
          pro_chdir(drv, &G.g_tmppth[0]);
          if (DOS_ERR)
          {
            drv = '@';
            G.g_tmppth[0] = NULL;
            pname[0] = pext[0] = '*';
            pname[1] = pext[1] = NULL;
          } /* if */
          do_diropen(pw, TRUE, pws->obid_save, drv, &G.g_tmppth[0],
                     &pname[0], &pext[0], (GRECT *)&pws->x_save, TRUE);
        } /* if pw */
} /* cnx_open */
#endif


#ifdef DESK1
static void cnx_get(void)
{
        /* DESKTOP v1.2: This function is a lot more involved */
        /* because CNX_OPEN is no longer a separate function. */
        WORD drv;
        WORD nw;
        WSAVE *pws;
        WNODE *pw;
        BYTE fname[9];
        BYTE fext[4];

        G.g_iview = (G.g_cnxsave.vitem_save == 0) ? V_TEXT : V_ICON;
        do_viewmenu(ICONITEM);
        do_viewmenu(NAMEITEM + G.g_cnxsave.sitem_save);
        G.g_ccopypref = G.g_cnxsave.ccopy_save;
        G.g_cdelepref = G.g_cnxsave.cdele_save;
        G.g_covwrpref = G.g_cnxsave.covwr_save;
        G.g_cdclkpref = G.g_cnxsave.cdclk_save;
        G.g_cmclkpref = G.g_cnxsave.cmclk_save;
        G.g_ctimeform = G.g_cnxsave.ctmfm_save;
        G.g_cdateform = G.g_cnxsave.cdtfm_save;
        G.g_cdclkpref = evnt_dclick(G.g_cdclkpref, TRUE);
        G.g_cmclkpref = menu_click(G.g_cmclkpref, TRUE);

        /* DESKTOP v1.2: Remove 2-window limit; and cnx_open() inlined. */
        for (nw = 0; nw < NUM_WNODES; nw++)
        {
                pws = &G.g_cnxsave.win_save[nw];

                /* Check for valid position */
                if(pws->x_save >= G.g_wdesk)
                {
                  pws->x_save = G.g_wdesk/2;
                }
                if(pws->y_save >= G.g_hdesk)
                {
                  pws->y_save = G.g_hdesk/2;
                }

                /* Check for valid width + height */
                if(pws->w_save <= 0 || pws->w_save > G.g_wdesk)
                {
                  pws->w_save = G.g_wdesk/2;
                }
                if(pws->h_save <= 0 || pws->h_save > G.g_hdesk)
                {
                  pws->h_save = G.g_hdesk/2;
                }

                if (pws->pth_save[0])
                {
                        if ((pw = win_alloc(pws->obid_save)))
                        {
                                pw->w_cvrow = pws->vsl_save;
                                fpd_parse(pws->pth_save, &drv, G.g_tmppth, fname, fext);
                                do_xyfix(&pws->x_save, &pws->y_save);
                                pro_chdir(drv, G.g_tmppth);
                                if (DOS_ERR)
                                {
                                        win_free(pw);
                                        continue;
                                }
                                do_diropen(pw, TRUE, pws->obid_save, drv, G.g_tmppth,
                                           fname, fext, (GRECT *)pws, TRUE);
                        }

                }
        }
        cnx_put();
} /* cnx_get */
#else
static void cnx_get(void)
{
        G.g_iview = (G.g_cnxsave.vitem_save == 0) ? V_TEXT : V_ICON;
        do_viewmenu(ICONITEM);
        do_viewmenu(NAMEITEM + G.g_cnxsave.sitem_save);
        G.g_ccopypref = G.g_cnxsave.ccopy_save;
        G.g_cdelepref = G.g_cnxsave.cdele_save;
        G.g_covwrpref = G.g_cnxsave.covwr_save;
        G.g_cdclkpref = G.g_cnxsave.cdclk_save;
        G.g_cmclkpref = G.g_cnxsave.cmclk_save;
        G.g_ctimeform = G.g_cnxsave.ctmfm_save;
        G.g_cdateform = G.g_cnxsave.cdtfm_save;
        G.g_cdclkpref = evnt_dclick(G.g_cdclkpref, TRUE);
        G.g_cmclkpref = menu_click(G.g_cmclkpref, TRUE);

        cnx_open(gl_open1st);
        cnx_open( abs(1 - gl_open1st) );
        cnx_put();
} /* cnx_get */
#endif



/* Counts the occurrences of c in str */
static int count_chars(char *str, char c)
{
    int count;

    count = 0;
    while(*str) {
        if (*str++ == c)
            count++;
    }

    return count;
}

/*
 * the xlate_ functions below are also used by the GEM rsc in aes/gem_rsc.c
 */

/* Translates the strings in an OBJECT array */
void xlate_obj_array(OBJECT *obj_array, int nobj)
{
    register OBJECT *obj;

    for (obj = obj_array; --nobj >= 0; obj++) {
        switch(obj->ob_type) {
        case G_TEXT:
        case G_BOXTEXT:
        case G_FTEXT:
        case G_FBOXTEXT:
            {
                BYTE **str = & ((TEDINFO *)obj->ob_spec)->te_ptmplt;
                *str = (BYTE *)gettext(*str);
            }
            break;
        case G_STRING:
        case G_BUTTON:
        case G_TITLE:
            obj->ob_spec = (LONG) gettext( (char *) obj->ob_spec);
            break;
        default:
            break;
        }
    }
}

/* Translates and fixes the TEDINFO strings */
void xlate_fix_tedinfo(TEDINFO *tedinfo, int nted)
{
    register int i = 0;
    long len;
    int j;
    char *tedinfptr;

    /* translate strings in TEDINFO */
    for (i = 0; i < nted; i++) {
        TEDINFO *ted = &tedinfo[i];
        ted->te_ptmplt = (BYTE *)gettext(ted->te_ptmplt);
    }

    /* Fix TEDINFO strings: */
    len = 0;
    for (i = 0; i < nted; i++) {
        if (tedinfo[i].te_ptext == 0) {
            /* Count number of '_' in strings
             * ( +2 for @ at the beginning, and \0 at the end )
             */
            len += count_chars((char *) tedinfo[i].te_ptmplt, '_') + 2;
        }
    }
    tedinfptr = (char *) dos_alloc(len);        /* Get memory */
    for (i = 0; i < nted; i++) {
        if (tedinfo[i].te_ptext == 0) {
            tedinfo[i].te_ptext = tedinfptr;
            *tedinfptr++ = '@'; /* First character of uninitialized string */
            len = count_chars((char *) tedinfo[i].te_ptmplt, '_');
            for (j = 0; j < len; j++) {
                *tedinfptr++ = '_';     /* Set other characters to '_' */
            }
            *tedinfptr++ = 0;   /* Final 0 */
        }
    }
}

/* change the sizes of the menus after translation
 * note - the code below is based on the assumption that the width of
 * the system font is eight (documented as such in lineavars.h)
 */
static void adjust_menu(OBJECT *obj_array)
{

#define OBJ(i) (&obj_array[i])

    int i;  /* index in the menu bar */
    int j;  /* index in the array of pull downs */
    int width = (v_hz_rez >> 3); /* screen width in chars */
    int x;
    OBJECT *menu = OBJ(0);
    OBJECT *mbar = OBJ(OBJ(menu->ob_head)->ob_head);
    OBJECT *pulls = OBJ(menu->ob_tail);

    x = 0;
    j = pulls->ob_head;
    for (i = mbar->ob_head; i <= mbar->ob_tail; i++) {
        OBJECT *title = OBJ(i);
        int n = strlen( (char *) title->ob_spec);
        int k, m;
        title->ob_x = x;
        title->ob_width = n;

        m = 0;
        for (k = OBJ(j)->ob_head; k <= OBJ(j)->ob_tail; k++) {
            OBJECT *item = OBJ(k);
            int l = strlen( (char *) item->ob_spec);
            if (m < l)
                m = l;
        }

        OBJ(j)->ob_x = 2+x;

        /* make sure the menu is not too far on the right of the screen */
        if (OBJ(j)->ob_x + m >= width) {
            OBJ(j)->ob_x = width - m;
            m = (m-1) | 0x700;
        }

        for (k = OBJ(j)->ob_head; k <= OBJ(j)->ob_tail; k++) {
            OBJECT *item = OBJ(k);
            item->ob_width = m;
        }
        OBJ(j)->ob_width = m;

        j = OBJ(j)->ob_next;
        x += n;
    }

    mbar->ob_width = x;
#undef OBJ
}

/*
 *      Horizontally centre dialog title: this is done dynamically to
 *      handle translated titles.
 *
 *      If object 1 of a tree is a G_STRING and its y position equals
 *      one character height, we assume it's the title.
 */
void centre_title(LONG tree)
{
        OBJECT          *root, *title;
        WORD            len;

        root = (OBJECT *)tree;
        title = root + 1;

        if ((title->ob_type == G_STRING) && (title->ob_y == gl_hchar)) {
          len = strlen((char *)title->ob_spec) * gl_wchar;
          if (len > root->ob_width)
            len = root->ob_width;
          title->ob_x = (root->ob_width - len) / 2;
        }
}

/*
 * translate and fixup desktop objects
 */
static void desk_xlate_fix(void)
{
    OBJECT *tree = desk_rs_trees[ADDINFO];
    OBJECT *objlabel = &tree[DELABEL];
    OBJECT *objversion = &tree[DEVERSN];
    register int i;

    /* translate strings in objects */
    xlate_obj_array(desk_rs_obj, RS_NOBS);

    /* insert the version number */
    objversion->ob_spec = (LONG) version;

    /* slightly adjust the about box for a CVS build */
    if (version[0] == '(') {
        objlabel->ob_spec = (LONG) "";  /* remove the word "Version" */
        objversion->ob_x -= 10;         /* and move the start of the string */
    }

    /* adjust the size and coordinates of menu items */
    adjust_menu(desk_rs_trees[ADMENU]);

    /* Fix objects coordinates: */
    for(i = 0 ; i < RS_NOBS ; i++) {
        rsrc_obfix((LONG) desk_rs_obj, i);
    }

    /* translate and fix TEDINFO strings */
    xlate_fix_tedinfo(desk_rs_tedinfo, RS_NTED);

#if !CONF_WITH_DESK1
    /* Disable menu entry that toggles icon/text mode */
    menu_ienable((LONG)desk_rs_trees[ADMENU],ICONITEM,FALSE);
#endif
}

/* Fake a rsrc_gaddr for the ROM desktop: */
WORD rsrc_gaddr(WORD rstype, WORD rsid, LONG *paddr)
{
    switch (rstype) {
    case R_TREE:
        *paddr = (LONG) desk_rs_trees[rsid];
        break;
    case R_BITBLK:
        *paddr = (LONG) &desk_rs_bitblk[rsid];
        break;
    case R_STRING:
        *paddr = (LONG) gettext( desk_rs_fstr[rsid] );
        break;
    default:
        kcprintf("FIXME: unsupported (faked) rsrc_gaddr type!\n");
        return FALSE;
    }

    return TRUE;
}



WORD deskmain(void)
{
        WORD            ii, done, flags;
        UWORD           ev_which, mx, my, button, kstate, kret, bret;
#ifndef DESK1
        WSAVE           *pws;
#endif

        /* initialize libraries */
        gl_apid = appl_init();
                                                /* get GEM's gsx handle */
        gl_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
                                                /* open a virtual work- */
                                                /*   station for use by */
                                                /*   the desktop        */
#ifdef NO_ROM
        gsx_vopen();
                                                /* init gsx and related */
                                                /*   device dependent   */
                                                /*   variables          */
        gsx_start();
#endif
        gl_whsiztop = NIL;
        gl_idsiztop = NIL;
                                                /* set clip to working  */
                                                /*   desk and calc full */
        wind_get(0, WF_WXYWH, &G.g_xdesk, &G.g_ydesk, &G.g_wdesk, &G.g_hdesk);
#ifdef DESK1
        wind_calc(1, -1, G.g_xdesk,  G.g_ydesk,  G.g_wdesk,  G.g_hdesk,
                                        &G.g_xfull, &G.g_yfull, &G.g_wfull, &G.g_hfull);
#else
        wind_get(0, WF_WXYWH, &G.g_xfull, &G.g_yfull, &G.g_wfull, &G.g_hfull);
        G.g_xfull += min(16, gl_wbox);
        G.g_yfull += gl_hbox;
        do_xyfix(&G.g_xfull, &G.g_yfull);
        G.g_wfull -= (2 * G.g_xfull);
        G.g_hfull -= (2 * gl_hbox);
                                        /* set up small window size     */
        gl_normwin.g_x = G.g_xfull;
        gl_normwin.g_y = G.g_yfull;
        gl_normwin.g_w = G.g_wfull;
        gl_normwin.g_h = (G.g_hfull - (gl_hbox / 2)) / 2;
#endif
                                                /* initialize mouse     */
        wind_update(BEG_UPDATE);
        desk_wait(TRUE);
        wind_update(END_UPDATE);

                                                /* detect optional features */
        detect_features();
                                                /* initialize resources */
#if 0
        if( !rsrc_load((LONG)"DESKTOP.RSC") )
        {
          form_alert(1,(LONG)"[3][ Can not load | the DESKTOP.RSC ][Cancel]");
          appl_exit();
          return(FALSE);
        }
#else
        desk_rs_init();                         /* copies ROM to RAM */
        desk_xlate_fix();                       /* translates & fixes desktop */
#endif
                                                /* initialize menus and */
                                                /*   dialogs            */
        for(ii = 0; ii < RS_NTREE; ii++) {
          rsrc_gaddr(R_TREE, ii, &G.a_trees[ii]);
          centre_title(G.a_trees[ii]);
        }

        for (ii=0; ii<RS_NBB; ii++)             /* initialize bit images */
        {
          app_tran(ii);
        }

        rsrc_gaddr(R_STRING, STASTEXT, &ad_ptext);
        rsrc_gaddr(R_STRING, STASICON, &ad_picon);

                                                /* These strings are    */
                                                /* used by dr_code.  Can't */
                                                /* get to resource then */
                                                /* because that would   */
                                                /* reenter AES.         */
        strcpy(&gl_amstr[0], ini_str(STAM));
        strcpy(&gl_pmstr[0], ini_str(STPM));
                                                /* initialize icons and */
                                                /*   apps from memory   */
                                                /*   or EMUDESK.INF or  */
                                                /*   builtin defaults   */
        app_start();

                                                /* initialize windows   */
        win_start();
                                                /* initialize folders,  */
                                                /*   paths, and drives  */
        fpd_start();
                                                /* show menu            */
        desk_verify(0, FALSE);                  /* should this be here  */
        wind_update(BEG_UPDATE);
        menu_bar(G.a_trees[ADMENU], TRUE);
        wind_update(END_UPDATE);
                                                /* establish menu items */
        G.g_iview = V_ICON;
        menu_text(G.a_trees[ADMENU], ICONITEM, ad_ptext);

        G.g_csortitem = NAMEITEM;
        menu_icheck(G.a_trees[ADMENU], G.g_csortitem, TRUE);

        menu_ienable(G.a_trees[ADMENU], RESITEM, can_change_resolution);

                                                /* set up initial prefs */
        G.g_ccopypref = TRUE;
#ifdef DESK1
        G.g_cdelepref = TRUE;
#endif
        G.g_ctimeform = TRUE;
        G.g_cdateform = TRUE;
        G.g_cdclkpref = 3;
                                                /* initialize desktop   */
                                                /*   and its objects    */
        gl_defdrv = app_blddesk();

#ifdef DESK1
        do_wredraw(0, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk);

        /* Take over the desktop */
        wind_set(0, WF_NEWDESK, G.g_screen, TRUE, FALSE);
#else
                                                /* fix up subwindows    */
        for (ii = 0; ii < NUM_WNODES; ii++)
        {
          pws = &G.g_cnxsave.win_save[ii];
          rc_copy(&gl_normwin, (GRECT *)&pws->x_save); /* copy in normal size */
          if (pws->pth_save[0])
            do_xyfix(&pws->x_save, &pws->y_save);
        } /* for */
                                /* fix up y loc. of lower window        */
        pws->y_save += pws->h_save + (gl_hbox / 2);

        fix_wins();             /* update with latest window x,y,w,h    */
#endif

                                                /* set up current parms */
        desk_verify(0, FALSE);
                                                /* establish desktop's  */
                                                /*   state from info    */
                                                /*   found in app_start,*/
                                                /*   open windows       */
        wind_update(BEG_UPDATE);
        cnx_get();
        wind_update(END_UPDATE);
        men_update(G.a_trees[ADMENU]);
                                                /* get ready for main   */
                                                /*   loop               */
        flags = MU_BUTTON | MU_MESAG | MU_KEYBD;
        done = FALSE;
#ifndef DESK1
        ig_close = FALSE;
#endif
                                                /* turn mouse on        */
        desk_wait(FALSE);

                                                /* loop handling user   */
                                                /*   input until done   */
        while( !done )
        {
                                                /* block for input      */
          ev_which = evnt_multi(flags, 0x02, 0x01, 0x01,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                G.g_rmsg, 0, 0,
                                &mx, &my, &button, &kstate, &kret, &bret);
                                                /* grab the screen      */
          wind_update(BEG_UPDATE);
                                                /* handle keybd message */
          if (ev_which & MU_KEYBD)
            done = hndl_kbd(kret);

                                                /* handle button down   */
          if (ev_which & MU_BUTTON)
            done = hndl_button(bret, mx, my, button, kstate);

                                                /* handle system message*/
          while (ev_which & MU_MESAG)
          {
            done = hndl_msg();
                                                /* use quick-out to clean */
                                                /* out all messages       */
            if (done)
              break;
            ev_which = evnt_multi(MU_MESAG | MU_TIMER, 0x02, 0x01, 0x01,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                G.g_rmsg, 0, 0,
                                &mx, &my, &button, &kstate, &kret, &bret);
          }

                                                /* free the screen      */
          wind_update(END_UPDATE);
        }
                                                /* save state in memory */
                                                /*   for when we come   */
                                                /*   back to the desktop*/
        cnx_put();
        app_save(FALSE);
                                                /* turn off the menu bar*/
        menu_bar(0x0L, FALSE);
                                                /* close gsx virtual ws */
#ifdef NO_ROM
        gsx_vclose();
#endif
                                                /* exit the gem AES     */
        appl_exit();

        return(TRUE);
} /* main */
