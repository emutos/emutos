/*      GEMSHLIB.C      4/18/84 - 09/13/85      Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */ 
/*      fix sh_envrn                            11/17/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "taddr.h"
#include "struct.h"
#include "basepage.h"
#include "dos.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemdosif.h"
#include "gemdos.h"
#include "gemgraf.h"
#include "gemgsxif.h"
#include "gemoblib.h"
#include "gemwmlib.h"
#include "gemfmlib.h"
#include "gempd.h"
#include "gemflag.h"
#include "geminit.h"
#include "gemrslib.h"
#include "gemasm.h"
#include "optimopt.h"
#include "kprint.h"     // just for debugging

#include "string.h"


#if MULTIAPP
EXTERN PD       *desk_ppd[];
EXTERN BYTE     *desk_str[];
EXTERN WORD     gl_dacnt;
EXTERN WORD     gl_mnpds[];
EXTERN PD       *gl_mnppd;
EXTERN LONG     menu_tree[];
EXTERN WORD     gl_pids;                /* bit vector of pids used      */
EXTERN PD       *gl_lastnpd;
EXTERN WORD     gl_numaccs;
EXTERN ACCNODE  gl_caccs[];
EXTERN WORD     proc_msg[8];

GLOBAL WORD     nulp_msg[8];
GLOBAL LONG     gl_efnorm, gl_efsave;
GLOBAL LONG     gl_strtaes;
GLOBAL LONG     gl_endaes;
GLOBAL LONG     gl_enddesktop;
GLOBAL WORD     gl_ldpid = -1;
#endif

GLOBAL SHELL    sh[NUM_PDS];

#if GEMDOS
GLOBAL BYTE     sh_apdir[70];                   /* holds directory of   */
                                                /*   applications to be */
                                                /*   run from desktop.  */
                                                /*   GEMDOS resets dir  */
                                                /*   to parent's on ret */
                                                /*   from exec.         */
#endif
GLOBAL LONG     ad_scmd;
GLOBAL LONG     ad_stail;
#if GEMDOS
#else
GLOBAL LONG     ad_s1fcb;
GLOBAL LONG     ad_s2fcb;
#endif
GLOBAL LONG     ad_ssave;
GLOBAL LONG     ad_dta;
GLOBAL LONG     ad_path;

GLOBAL LONG     ad_pfile;

GLOBAL WORD     gl_shgem;





/* Prototypes: */
WORD sh_find(LONG pspec);
extern void deskstart();        /* see ../desk/deskstart.S */
extern LONG do_pexec(WORD mode, LONG p1, LONG p2, LONG p3);  /* in gemstart.S */



void sh_read(LONG pcmd, LONG ptail)
{
        LBCOPY(pcmd, ad_scmd, 128);
        LBCOPY(ptail, ad_stail, 128);
}



void sh_curdir(LONG ppath)
{
        WORD    drive;
                                                /* remember current     */
                                                /*  directory           */
        drive = dos_gdrv();
        LBSET(ppath++, (drive + 'A') );
        LBSET(ppath++, ':');
        LBSET(ppath++, '\\');
        dos_gdir( drive+1, (BYTE *)ppath );
}



BYTE *sh_parse(BYTE *psrc, BYTE *pfcb)
{
        register BYTE   *ptmp;
        BYTE            *sfcb;
        BYTE            drv;

        sfcb = pfcb;
                                                /* scan off white space */
        while ( (*psrc) &&
                (*psrc == ' ') )
          psrc++;
        if (*psrc == NULL)
          return(psrc);
                                                /* remember the start   */
        ptmp = psrc;
                                                /* look for a colon     */
        while ( (*psrc) &&
                (*psrc != ' ') &&
                (*psrc != ':') )
          psrc++;
                                                /* pick off drive letter*/
        drv = 0;
        if ( *psrc == ':' )
        {
          drv = toupper(*(psrc - 1)) - 'A' + 1;
          psrc++;
        }
        else
          psrc = ptmp;
        *pfcb++ = drv;
        if (*psrc == NULL)
          return(psrc);
                                                /* scan off filename    */
        while ( (*psrc) &&
                (*psrc != ' ') &&
                (*psrc != '*') &&
                (*psrc != '.') &&
                (pfcb <= &sfcb[8]) )
          *pfcb++ = toupper(*psrc++);
                                                /* pad out with blanks  */
        while ( pfcb <= &sfcb[8] )
          *pfcb++ = (*psrc == '*') ? ('?') : (' ');
        if (*psrc == '*')
          psrc++;
                                                /* scan off file ext.   */
        if ( *psrc == '.')
        {
          psrc++;
          while ( (*psrc) &&
                  (*psrc != ' ') &&
                  (*psrc != '*') &&
                  (pfcb <= &sfcb[11]) )
            *pfcb++ = toupper(*psrc++);
        }
        while ( pfcb <= &sfcb[11] )
          *pfcb++ = (*psrc == '*') ? ('?') : (' ');
        if (*psrc == '*')
          psrc++;
                                                /* return pointer to    */
                                                /*   remainder of line  */
        return(psrc);
}



/*
*       Routine to fix up the command tail and parse FCBs for a coming
*       exec.
*/
void sh_fixtail(WORD iscpm)
{
        register WORD   i;
        WORD            len;
        BYTE            *s_tail;
        BYTE            *ptmp;
        BYTE            s_fcbs[32];
                                                /* reuse part of globals*/
        s_tail = &D.g_dir[0];
        i = 0;

        if (iscpm)
        {
          s_tail[i++] = NULL;
          ptmp = &D.s_cmd[0];
          while ( (*ptmp) &&
                  (*ptmp != '.') )
            s_tail[i++] = *ptmp++;
        }

        LBCOPY(ADDR(&s_tail[i]), ad_stail, 128 - i);

        if (iscpm)
        {
                                                /* pick up the length   */
          len = s_tail[i];
                                                /* null over carriage ret*/
          s_tail[i + len + 1] = NULL;
                                                /* copy down space,tail */
          strcpy(&s_tail[i], &s_tail[i+1]);
        }
        else
        {
                                                /* zero the fcbs        */
          memset(s_fcbs, 0, 32);
          memset(&s_fcbs[1], ' ', 11);
          memset(&s_fcbs[17], ' ', 11);

                                      /* parse the fcbs       */
          if ( s_tail[0] )
          {
            s_tail[ 1 + s_tail[0] ] = NULL;
            ptmp = sh_parse(&s_tail[1], &s_fcbs[0]);
            if (*ptmp != NULL)
              sh_parse(ptmp, &s_fcbs[16]);
            s_tail[ 1 + s_tail[0] ] = 0x0d;
          }
#if GEMDOS
#else
                                                /* copy into true fcbs  */
          LBCOPY(ad_s1fcb, ADDR(&s_fcbs[0]), 16); 
          LBCOPY(ad_s2fcb, ADDR(&s_fcbs[16]), 16); 
#endif
        }
                                                /* copy into true tail  */
        LBCOPY(ad_stail, ADDR(s_tail), 128);
}



/*
*       Routine to set the next application to run
*
*               isgem = 0   then run in character mode
*               isgem = 1   them run in graphic mode
*
*               isover = 0  then run above DESKTOP
*               isover = 1  then run over DESKTOP
*               isover = 2  then run over AES and DESKTOP
*/
WORD sh_write(WORD doex, WORD isgem, WORD isover, LONG pcmd, LONG ptail)
{
        SHELL           *psh;

        LBCOPY(ad_scmd, pcmd, 128);
        LBCOPY(ad_stail, ptail, 128);
        if (isover > 0)
        {
                                                /* stepaside to run     */
          psh = &sh[rlr->p_pid];
          psh->sh_isgem = (isgem != FALSE);
          psh->sh_doexec = doex;
          psh->sh_dodef = FALSE;
          psh->sh_fullstep = isover - 1;
#if GEMDOS                                      
          sh_curdir(ADDR(&sh_apdir[0]));        /* save apps. current   */
                                                /* directory            */
#endif
        }
        else
        {
          sh_fixtail(FALSE);
                                                /* run it above us      */
          if ( sh_find(ad_scmd) )
          {
#if GEMDOS
            dos_exec(ad_scmd, ad_envrn, ad_stail);
#else
            dos_exec(ad_scmd, LHIWD(ad_envrn),
                     ad_stail, ad_s1fcb, ad_s2fcb);
#endif
          }
          else
            return(FALSE);
        }
        return(TRUE);                           /* for the future       */
}


/*
*       Used by the DESKTOP to recall 1024 bytes worth of previously
*       'put' desktop-context information.
*/
void sh_get(LONG pbuffer, WORD len)
{
        LBCOPY(pbuffer, ad_ssave, len);
}


/*
*       Used by the DESKTOP to save away 1024 bytes worth of desktop-
*       context information.
*/
void sh_put(LONG pdata, WORD len)
{
        LBCOPY(ad_ssave, pdata, len);
}


/*
*       Convert the screen to graphics-mode in preparation for the 
*       running of a GEM-based graphic application.
*/
void sh_tographic()
{
                                                /* retake ints that may */
                                                /*   have been stepped  */
                                                /*   on by char. appl.  */
                                                /*   including err.     */
                                                /*   handler and gem.int*/
        cli();
        retake();
        sti();
                                                /* convert to graphic   */
        gsx_graphic(TRUE);
                                                /* set initial clip rect*/
        gsx_sclip(&gl_rscreen);
#if SINGLAPP
                                                /* allocate screen space*/
        gsx_malloc();
#endif
                                                /* start up the mouse   */
        ratinit();
                                                /* put mouse to hourglass*/
        gsx_mfset(ad_hgmice);
}


/*
*       Convert the screen and system back to alpha-mode in preparation for
*       the running of a DOS-based character application.
*/
void sh_toalpha()
{
                                                /* put mouse to arrow   */
        gsx_mfset(ad_armice);
                                                /* give back the error  */
                                                /*   handler since ours */
                                                /*   is graphic         */
        cli();
        giveerr();
        sti();
                                                /* turn off the mouse   */
        ratexit();
#if SINGLAPP
                                                /* return screen space  */
        gsx_mfree();
#endif
                                                /* close workstation    */
        gsx_graphic(FALSE);
}




void sh_draw(LONG lcmd, WORD start, WORD depth)
{
        LONG            tree;
        SHELL           *psh;

        psh = &sh[rlr->p_pid];

        if (gl_shgem)
        {
          tree = ad_stdesk;
          gsx_sclip(&gl_rscreen);
          LLSET(ad_pfile, lcmd);
          ob_draw(tree, start, depth);
        }
}




void sh_show(LONG lcmd)
{
        WORD            i;
        
        for(i=1; i<3; i++)
          sh_draw(lcmd, i, 0);
}


/*
*       Routine to take a full path, and scan back from the end to 
*       find the starting byte of the particular filename
*/
BYTE *sh_name(BYTE *ppath)
{
        register BYTE   *pname;

        pname = &ppath[strlen(ppath)];
        while ( (pname >= ppath) &&
                (*pname != '\\') &&
                (*pname != ':') )
          pname--;
        pname++;
        return(pname);
}


/*
*       Search for a particular string in the DOS environment and return
*       a long pointer to the character after the string if it is found. 
*       Otherwise, return a NULLPTR
*/
void sh_envrn(LONG ppath, LONG psrch)
{
        LONG            lp, ad_loc1;
        WORD            len, findend;
        BYTE            last, tmp, loc1[10], loc2[10];


        len = strlencpy(loc2, (char *)psrch);
        len--;

        ad_loc1 = ADDR(loc1);
        loc1[len] = NULL;

        lp = ad_envrn;
        findend = FALSE;
        tmp = NULL;
        do
        {
          last = tmp;
          tmp = LBGET(lp);
          lp++;
          if ( (findend) &&
               (tmp == NULL) )
          {
            findend = FALSE;
            tmp = 0xFF;
          }
          else
          {
            if (((last == NULL) || (last == -1)) && (tmp == loc2[0]))
            {
              LBCOPY(ADDR(&loc1[0]), lp, len);
              if ( strcmp(&loc1[0], &loc2[1])==0 )
              {
                lp += len;
                break;
              }
            }
            else
              findend = TRUE;
          }
          } while( tmp );
          if (!tmp)
                lp = 0x0L;

        LLSET(ppath, lp);
}


/*
*       Search first, search next style routine to pick up each path
*       in the PATH= portion of the DOS environment.  It returns the
*       next higher number to look for until there are no more
*       paths to find.
*/

WORD sh_path(WORD whichone, LONG dp, BYTE *pname)
{
        register BYTE   tmp, last;
        LONG            lp;
        register WORD   i;

        last = 0;
                                                /* find PATH= in the    */
                                                /*   command tail which */
                                                /*   is a double null-  */
                                                /*   terminated string  */
        sh_envrn(ADDR(&lp), ADDR(rs_str(STPATH)));
        if (!lp)
                return(0);

                                                /* if found count in to */
                                                /*   appropriate path   */
        i = whichone;
        tmp = ';';
        while (i)
        {
          while (( tmp = LBGET(lp)) != 0 )
          {
            lp++;
            if (tmp == ';')
              break;
          }
          i--;
        }
        if (!tmp)
          return(0);
                                                /* copy over path       */
        while ( ( tmp = LBGET(lp) ) != 0)
        {
          if ( tmp != ';' )
          {
            LBSET(dp++, tmp);
            last = tmp;
            lp++;
          }
          else
            break;
        }
                                                /* see if extra slash   */
                                                /*   is needed          */
        if ( (last != '\\') &&
             (last != ':') )
          LBSET(dp++, '\\');
                                                /* append file name     */
        strcpy((char *) dp, pname);
                                                /* make whichone refer  */
                                                /*   to next path       */
        return(whichone+1);
}


/*
*       Routine to verify that a file is present.  It first looks in the
*       current directory and then looks down the search path.
*/

WORD sh_find(LONG pspec)
{
        WORD            path;
        BYTE            gotdir, *pname, tmpname[66];

        dos_sdta(ad_dta);

        strcpy((char *) ad_path, (char *) pspec);  /* copy to local buffer */
        pname = sh_name(&D.g_dir[0]);           /* get ptr to name      */
        gotdir = (pname != &D.g_dir[0]);
        if (!gotdir)
        {
          strcpy(tmpname, pname);           /* save name            */
          sh_curdir(ad_path);                   /* get current drive/dir*/
          if (D.g_dir[3] != NULL)               /* if not at root       */
            strcat(&D.g_dir[0], "\\");          /*  add foreslash       */
          strcat(&D.g_dir[0], &tmpname[0]);     /* append name to drive */
        }
                                                /* and directory.       */
        path = 0;
        do
        {

          dos_sfirst(ad_path, F_RDONLY | F_SYSTEM);

          if ( (DOS_AX == E_PATHNOTFND) ||
                ((DOS_ERR) && 
                 ((DOS_AX == E_NOFILES) ||
                  (DOS_AX == E_PATHNOTFND) ||
                  (DOS_AX == E_FILENOTFND))) )
          {
            path = sh_path(path, ad_path, &tmpname[0]);
            DOS_ERR = TRUE;
          }
          else
            path = 0;
        } while ( !gotdir && DOS_ERR && path );

        if (!DOS_ERR)
          strcpy((char *) pspec, (char *) ad_path);

        return(!DOS_ERR);
}




/*
*       Read the default application to invoke.
*/

void sh_rdef(LONG lpcmd, LONG lpdir)
{
        SHELL           *psh;

        psh = &sh[rlr->p_pid];

        strcpy((char *)lpcmd, &psh->sh_desk[0]);
        strcpy((char *)lpdir, &psh->sh_cdir[0]);
}


/*
*       Write the default application to invoke
*/

void sh_wdef(LONG lpcmd, LONG lpdir)
{
        SHELL           *psh;

        psh = &sh[rlr->p_pid];

        strcpy(&psh->sh_desk[0], (char *) lpcmd);
        strcpy(&psh->sh_cdir[0], (char *) lpdir);
}



void sh_chgrf(SHELL *psh)
{
        if ( psh->sh_isgem != gl_shgem )
        {
          gl_shgem = psh->sh_isgem;
          if ( gl_shgem )
            sh_tographic();
          else
            sh_toalpha();
        }
}



/*
*
*/
void sh_chdef(SHELL *psh)
{
                                                /* if we should exec    */
                                                /*   the default command*/
                                                /*   then let it be     */
                                                /*   known that it is   */
                                                /*   a gem appl.        */
        psh->sh_isdef = FALSE;
        if ( psh->sh_dodef )
        {
          psh->sh_isdef = psh->sh_isgem = TRUE;
          psh->sh_fullstep = 0;
          dos_sdrv(psh->sh_cdir[0] - 'A');
          dos_chdir((BYTE *)ADDR(&psh->sh_cdir[0]));
          strcpy(&D.s_cmd[0], &psh->sh_desk[0]);
        }
#if GEMDOS
        else
        {                       
          dos_sdrv(sh_apdir[0] - 'A');          /* desktop's def. dir   */
          dos_chdir(sh_apdir);
        }
#endif
}



#if MULTIAPP
/* 
*       Special case DOS load
*       Used from pr_exec when loading a dos app
*/
void sh_dosexec()
{
        WORD            done;
        PD              *owner;
        WORD            mbuff[8];
        WORD            pid;
        WORD            ret;

        pid = rlr->p_pid;

        pr_exec(pid, ad_scmd, ad_stail, ad_envrn, ad_s1fcb, ad_s2fcb);

        done = FALSE;
        while (!done)
        {
/**/      sh_toalpha();
          
          sh[pid].sh_loadable = TRUE;
          ret = pr_load(pid);

          sh[pid].sh_loadable = FALSE;
          gl_ldpid = 1;/* screen manager is automatically load after dos */
          switch(ret)  /* app by the gdos                                */
          {
            case PR_TERMINATED:
            case PR_FAILURE:
/**/          sh_tographic();

              done = TRUE;
              break;
            case PR_OKGEM:

              owner = desk_ppd[pr_retid()];
              w_newmenu(owner); 
/**/          if (sh[owner->p_pid].sh_isgem)
              { 
                sh_tographic(); 
                w_windfix(owner);
              }                                                         

              if (sh[owner->p_pid].sh_isacc || !sh[owner->p_pid].sh_isgem)
                ap_sendmsg(proc_msg, AC_OPEN, owner, 0, pr_retid(), 0, 0, 0);
          } 
          if (!done)
          {
            mbuff[0] = 0;
            while (mbuff[0] != AC_OPEN)
              ap_rdwr(MU_MESAG, rlr, 16, ADDR(&mbuff[0]) );
          }
        } 
}


void sh_ldapp()  /* for MULTIAPP */
{
        WORD            ret;
        SHELL           *psh;
        PD              *ppd;
        BYTE            name[11];
        WORD            isacc;
        WORD            da_id;
                
        psh = &sh[rlr->p_pid];
#if GEMDOS
        /* initialize sh_apdir */
        strcpy((char *) sh_apdir, (char *) ad_scdir); 
#endif

        strcpy(&psh->sh_desk[0], rs_str(STDESKTP));
        strcpy(&psh->sh_cdir[0], &D.s_cdir[0]);

ldagain:
        sh_chdef(psh);
                                        /* set up so that we will exec  */
                                        /*   the default next time      */
                                        /*   unless the application does*/
                                        /*   a set command      */
        psh->sh_dodef = TRUE;
        isacc = psh->sh_isacc;
        if ( !sh_find(ad_scmd) )
        {
                                        /* error, cant find app or acc  */
          goto ldend;
        }
        if (!isacc)
          sh_show(ad_scmd);
        sh_fixtail(psh->sh_fullstep == 2);
                                        /* take the old guy's windows   */
                                        /*  out of the W_TREE           */
        w_windfix(rlr);
        w_setmen(rlr->p_pid);
                                        /* clear his desk field */
        desk_tree[rlr->p_pid] = 0x0L;
        p_nameit(rlr, sh_name(&D.s_cmd[0]));
        if ( !isacc)
        {
          name[0] = name[1] = ' ';
          movs(8, &rlr->p_name[0], &name[2]);
          name[10] = NULL;
          gl_mnpds[rlr->p_pid] = mn_register(rlr->p_pid, ADDR(&name[0]));
        }
        psh->sh_loadable = TRUE;
                                        /* normal dos exec if gem app   */
                                        /*  or if dos app that is an acc*/
        if (psh->sh_isgem)
        {       
                                        /* make sure channel is clean   */  
          pr_load(rlr->p_pid);
          LLSET(0xefL * 4, LLCS() | LW(&cpmcod));
                                        /* exec app into channel        */
          dos_exec(ad_scmd, LHIWD(ad_envrn), ad_stail, 
                   ad_s1fcb, ad_s2fcb);
        }
        else if (isacc)                 /* dos app that is an acc       */
        {                               /*  this must come back here    */
                                        /*  without any switch key( + ) */
                                        /*  having been depressed.      */
          pr_exec(rlr->p_pid, ad_scmd, ad_stail,
                  ad_envrn, ad_s1fcb, ad_s2fcb);
          pr_load(rlr->p_pid);
        }
        else                            /* normal DOS app               */
          sh_dosexec();

        if (psh->sh_isacc)
        {
          acancel(rlr->p_evbits);       /* cancel his event's           */
          if (rlr->p_evflg)
            apret(rlr->p_evflg);
        }
        psh->sh_loadable = FALSE;
        psh->sh_state = NULL;
        psh->sh_isgem = TRUE;
                                        /* take all the items out of    */
                                        /*  the DESKTOP menue           */
        while ( (da_id = mn_ppdtoid(rlr)) != NIL )
          mn_unregister( da_id );

        gl_mnpds[rlr->p_pid] = 0x0;
                                        /* set name to availnul         */
        p_nameit(rlr, op_gname(STAVAIL) );
        desk_tree[rlr->p_pid] = 0x0L;
                                        /* if the current process has   */
                                        /*  done a sh_write to load a   */
                                        /*  process in place of itself, */
                                        /*  then we must clear out his  */
                                        /*  old windows and load in the */
                                        /*  new app.                    */
        if (psh->sh_doexec)
        {
          oldwfix(rlr, TRUE);
          goto ldagain;
        }

ldend:  
        pr_delete(rlr->p_pid);                  /* close the channel*/
        if(rlr->p_pid != 0x0 )
        {
          w_clswin();                   /* close rlr's windows          */
          ppd = fpdnm(NULLPTR, 0x0);    /* get pd *                     */
          if (isacc)
            gl_lastnpd = NULLPTR;       /* force redraws if acc abort   */
          w_windfix(ppd);               /* make desktop's windows active*/
          w_newmenu(ppd);
        }
}
#endif /* MULTIAPP */


#if SINGLAPP
void sh_ldapp()
{
        WORD    ret, badtry, retry;
        SHELL   *psh;
        LONG    *desk_pd;
        void    *uda_ssp_save;

        psh = &sh[rlr->p_pid];
#if GEMDOS
        strcpy(sh_apdir, (BYTE *)ad_scdir);     /* initialize sh_apdir  */
#endif
        badtry = 0;     

        strcpy(&psh->sh_desk[0], rs_str(STDESKTP));
        strcpy(&psh->sh_cdir[0], &D.s_cdir[0]);
        do
        {
          sh_chdef(psh);
                                                /* set up so that we    */
                                                /*   will exec the      */
                                                /*   default next time  */
                                                /*   unless the         */
                                                /*   application does   */
                                                /*   a set command      */
          psh->sh_dodef = TRUE;
                                                /* init graph/char mode */
          sh_chgrf(psh);
          if (gl_shgem)
          {
            wm_start();
            ratinit();
          }
                                                /* fix up/parse cmd tail*/ 
          sh_fixtail(psh->sh_fullstep == 2);
          sh_draw(ad_scmd, 0, 0);               /* redraw the desktop   */

                                                /* clear his desk field */
          desk_tree[rlr->p_pid] = 0x0L;
                                          /* exec it              */
                                          /* handle bad try msg */
          if (badtry)
          {
            ret = fm_show(badtry, NULLPTR, 1);
            if (badtry == ALNOFIT)
              break;
            badtry = 0;
          }


          do
          {
            retry = FALSE;

            kprintf("starting %s\n",(char *)ad_scmd);
            if(strcmp((char *)ad_scmd,rs_str(STDESKTP))==0)
            { /* Experimental starting of the ROM desktop:  - THH*/
              sh_show(ad_scmd);
              p_nameit(rlr, sh_name(&D.s_cmd[0]));

              desk_pd = (LONG *) do_pexec(5, 0L , (LONG)"", 0L);
              desk_pd[2] = (LONG) deskstart;
              desk_pd[3] = desk_pd[5] = desk_pd[7] = 0;
              dsptch();  /* Updates rlr->p_uda! */
              /* Save old stacks and set up new stacks so that AES' stacks
                 won't be destroyed: */
              uda_ssp_save = rlr->p_uda->u_spsuper;
              rlr->p_uda->u_spsuper = &rlr->p_uda->u_supstk;
              do_pexec(4, 0L, (LONG)desk_pd, 0L);    /* Run the desktop */
              rlr->p_uda->u_spsuper = uda_ssp_save;
            }
            else
            if ( sh_find(ad_scmd) )
            { /* Run a normal application: */
              sh_show(ad_scmd);
              p_nameit(rlr, sh_name(&D.s_cmd[0]));
              if (psh->sh_fullstep == 0)
              {
#if GEMDOS
                dsptch();  /* Updates rlr->p_uda! */
                /* Save old stacks and set up new stacks so that AES' stacks
                   won't be destroyed: */
                uda_ssp_save = rlr->p_uda->u_spsuper;
                rlr->p_uda->u_spsuper = &rlr->p_uda->u_supstk;
                dos_exec(ad_scmd, ad_envrn, ad_stail);    /* Run the APP */
                rlr->p_uda->u_spsuper = uda_ssp_save;
#else
                dos_exec(ad_scmd, LHIWD(ad_envrn), ad_stail, 
                         ad_s1fcb, ad_s2fcb);
#endif
              }
              else if (psh->sh_fullstep == 1)
              {
#if GEMDOS
                dos_exec(ad_scmd, ad_envrn, ad_stail);
#else
                gsx_exec(ad_scmd, LHIWD(ad_envrn), ad_stail, 
                         ad_s1fcb, ad_s2fcb);
#endif
                DOS_ERR = psh->sh_doexec = FALSE;
              }
              if (DOS_ERR)
                badtry = (psh->sh_isdef) ? ALNOFIT : AL08ERR;
/*  02/11/86 LKW begin  */
              if (wind_spb.sy_owner == rlr)     /* if he still owns screen*/
                  unsync(&wind_spb);            /*   then take him off. */
/*  02/11/86 LKW end    */
            }
            else
            {
              if ( (gl_shgem) &&
                   (psh->sh_isdef) )
              {
                ret = fm_show(ALOKDESK, NULLPTR, 1);
                if (ret == 1)
                  retry = TRUE;
                else
                  retry = psh->sh_doexec = FALSE;
              }
              else
                badtry = AL18ERR;
            }
          } while (retry && !badtry);

          desk_tree[rlr->p_pid] = 0x0L;         /* clear his desk field */

        } while(psh->sh_doexec);

}
#endif



void sh_main()
{
#if MULTIAPP
        WORD            ii;

        for (ii = 0; ii < 2; ii++)              /* init pid 0 and 1     */
        {
          sh[ii].sh_isgem = TRUE;
          sh[ii].sh_isacc = FALSE;
          sh[ii].sh_state = 0x0;
          pr_scpid(ii, FALSE);
        }
        sh[1].sh_loadable = TRUE;
        pr_menu(ADDR(desk_str[0]), ADDR(&gl_dacnt));
                                                /* allocate screen space*/
        gsx_malloc();
        ratinit();
                                                /* fix up aes channel   */
        pr_shrink(SCR_MGR, ADDR(&gl_strtaes), ADDR(&gl_endaes), TRUE );
        sh_rdinf();
        sh_ldacc();
        gl_enddesktop = gl_endaes + (SIZE_DESKTOP << 12);
                                                /* create the default   */
                                                /*  channel.            */
        pr_create(rlr->p_pid, gl_endaes, SIZE_DESKTOP, FALSE, TRUE);
#endif
                                                /* do the exec          */
        sh_ldapp();

#if MULTIAPP
                                                /* return screen space  */
        gsx_mfree();
#endif
                                                /* get back to alpha    */
                                                /*   mode if necessary  */
        if (gl_shgem)
          sh_toalpha();
}



#if MULTIAPP
void sh_ldacc()
{
        WORD            id, ii, ret;
        WORD            junk;
        ULONG           caddr, csize, endmem, cssize;

        for(ii = 0; ii < gl_numaccs; ii++)
        {
          prc_create(gl_endaes, 0x00010000L, FALSE, TRUE, &id);
          ret = pr_run(id, TRUE, SH_ACC, ADDR(&gl_caccs[ii].acc_name[0]),
                       ad_stail);
          if (ret)
          {
            pr_info(id, &junk, &junk, &caddr, &csize, &endmem, &cssize);
            gl_endaes = caddr + (csize << 12);  /* + (cssize << 12);*/
          }
          else
            pr_delete(id);
        }
}
#endif


