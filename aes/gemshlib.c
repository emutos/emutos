/*      GEMSHLIB.C      4/18/84 - 09/13/85      Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */
/*      fix sh_envrn                            11/17/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2015 The EmuTOS development team
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

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "asm.h"

#include "obdefs.h"
#include "struct.h"
#include "basepage.h"
#include "pd.h"
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

#include "string.h"
#include "kprint.h"             /* for debugging */

#include "gemshlib.h"

#define SIZE_AFILE 2048                 /* size of AES shell buffer: must   */
                                        /*  agree with #define in deskapp.h */

static BYTE shelbuf[SIZE_AFILE];        /* AES shell buffer */

GLOBAL SHELL    sh[NUM_PDS];

static BYTE sh_apdir[LEN_ZPATH];                /* holds directory of   */
                                                /*   applications to be */
                                                /*   run from desktop.  */
                                                /*   GEMDOS resets dir  */
                                                /*   to parent's on ret */
                                                /*   from exec.         */
GLOBAL BYTE     *ad_stail;

GLOBAL LONG     ad_pfile;

GLOBAL WORD     gl_shgem;

/* Resolution settings:
 * changerez: 0=no change, 1=change ST resolution, 2=change Falcon resolution
 * nextrez: Stores the resolution for Setscreen() */
GLOBAL WORD     gl_changerez;
GLOBAL WORD     gl_nextrez;




/* Prototypes: */
extern void deskstart(void) NORETURN;   /* see ../desk/deskstart.S */
#if WITH_CLI != 0
extern void coma_start(void) NORETURN;  /* see cli/cmdasm.S */
#endif

static void sh_toalpha(void);



void sh_read(BYTE *pcmd, BYTE *ptail)
{
        strcpy(pcmd, D.s_cmd);
        memcpy(ptail, ad_stail, CMDTAILSIZE);
}



void sh_curdir(BYTE *ppath)
{
        WORD    drive;
                                                /* remember current     */
                                                /*  directory           */
        drive = dos_gdrv();
        *ppath++ = drive + 'A';
        *ppath++ = ':';
        *ppath++ = '\\';
        dos_gdir( drive+1, ppath );
}



/*
 *      shel_write: multi-purpose function
 *
 *      performs the following functions:
 *      (1) doex = 5
 *          . selects next video resolution
 *              (a) isover = 0
 *                  . indicates ST/TT-style video
 *                      isgem = 2 + value used by Getrez()/Setscreen()
 *              (b) isover = 1
 *                  . indicates Falcon-style video)
 *                      isgem = value used by Setscreen() for video mode
 *
 *      (2) doex = 1
 *          . set the next application to run after the current one terminates
 *              isover      ignored, should currently always be 1
 *              isgem = 0   run in character mode (TOS/TTP)
 *              isgem = 1   run in graphic mode (APP/PRG)
 *              
 *      (3) doex = 0
 *          . set no application to run after the current one terminates
 *              note that this is used by the desktop shutdown code
 */
WORD sh_write(WORD doex, WORD isgem, WORD isover, const BYTE *pcmd, const BYTE *ptail)
{
        SHELL           *psh;

        if (doex == 5)  /* Change resolution */
        {
          gl_changerez = 1 + isover;
          gl_nextrez = isgem;
          D.s_cmd[0] = '\0';
          ad_stail[0] = '\0';
          return TRUE;
        }

        strcpy(D.s_cmd, pcmd);
        memcpy(ad_stail, ptail, CMDTAILSIZE);

        psh = &sh[rlr->p_pid];
        psh->sh_isgem = (isgem != FALSE);
        psh->sh_doexec = doex;
        psh->sh_dodef = FALSE;
        sh_curdir(sh_apdir);    /* save apps. current directory */

        return TRUE;
}


/*
*       Used by the DESKTOP to recall SIZE_AFILE bytes worth of previously
*       'put' desktop-context information.
*/
void sh_get(void *pbuffer, WORD len)
{
        memcpy(pbuffer,shelbuf,len);
}


/*
*       Used by the DESKTOP to save away SIZE_AFILE bytes worth of desktop-
*       context information.
*/
void sh_put(const void *pdata, WORD len)
{
        memcpy(shelbuf,pdata,len);
}


/*
*       Convert the screen to graphics-mode in preparation for the
*       running of a GEM-based graphic application.
*/
void sh_tographic(void)
{
                                                /* retake ints that may */
                                                /*   have been stepped  */
                                                /*   on by char. appl.  */
                                                /*   including err.     */
                                                /*   handler and gem.int*/
        disable_interrupts();
        retake();
        enable_interrupts();
                                                /* convert to graphic   */
        gsx_graphic(TRUE);
                                                /* set initial clip rect*/
        gsx_sclip(&gl_rscreen);
                                                /* allocate screen space*/
        gsx_malloc();
                                                /* start up the mouse   */
        ratinit();
                                                /* put mouse to hourglass*/
        gsx_mfset(ad_hgmice);
}


/*
*       Convert the screen and system back to alpha-mode in preparation for
*       the running of a DOS-based character application.
*/
static void sh_toalpha(void)
{
                                                /* put mouse to arrow   */
        gsx_mfset(ad_armice);
                                                /* give back the error  */
                                                /*   handler since ours */
                                                /*   is graphic         */
        disable_interrupts();
        giveerr();
        enable_interrupts();
                                                /* turn off the mouse   */
        ratexit();
                                                /* return screen space  */
        gsx_mfree();
                                                /* close workstation    */
        gsx_graphic(FALSE);
}




static void sh_draw(const BYTE *lcmd, WORD start, WORD depth)
{
        LONG            tree;

        if (gl_shgem)
        {
          tree = (LONG)rs_trees[DESKTOP];
          gsx_sclip(&gl_rscreen);
          *(LONG *)ad_pfile = (LONG)lcmd;
          ob_draw(tree, start, depth);
        }
}




static void sh_show(const BYTE *lcmd)
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
void sh_envrn(BYTE **ppath, const BYTE *psrch)
{
        BYTE            *lp;
        WORD            len, findend;
        BYTE            last, tmp, loc1[10], loc2[10];


        len = strlencpy(loc2, psrch);
        len--;

        loc1[len] = NULL;

        lp = ad_envrn;
        findend = FALSE;
        tmp = NULL;
        do
        {
          last = tmp;
          tmp = *lp++;
          if ( (findend) &&
               (tmp == NULL) )
          {
            findend = FALSE;
            tmp = (BYTE)0xFF;
          }
          else
          {
            if (((last == NULL) || (last == -1)) && (tmp == loc2[0]))
            {
              memcpy(loc1, lp, len);
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

        *ppath = lp;
}


/*
*       Search first, search next style routine to pick up each path
*       in the PATH= portion of the DOS environment.  It returns the
*       next higher number to look for until there are no more
*       paths to find.
*/

static WORD sh_path(WORD whichone, BYTE *dp, BYTE *pname)
{
        register BYTE   tmp, last;
        BYTE            *lp;
        register WORD   i;

        last = 0;
                                                /* find PATH= in the    */
                                                /*   command tail which */
                                                /*   is a double null-  */
                                                /*   terminated string  */
        sh_envrn(&lp, PATH_ENV);
        if (!lp)
                return(0);

        if (!*lp)                               /* skip nul after PATH= */
                lp++;

                                                /* if found count in to */
                                                /*   appropriate path   */
        i = whichone;
        tmp = ';';
        while (i)
        {
          while (( tmp = *lp) != 0 )
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
        while ( ( tmp = *lp) != 0)
        {
          if ( tmp != ';' )
          {
            *dp++ = tmp;
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
          *dp++ = '\\';
                                                /* append file name     */
        strcpy(dp, pname);
                                                /* make whichone refer  */
                                                /*   to next path       */
        return(whichone+1);
}


/*
*       Routine to verify that a file is present.  Note that this routine
*       tolerates the presence of wildcards in the filespec.
*
*       The directory search order is the same as that in TOS3/TOS4, as
*       deduced from tests on those systems:
*       (1) isolate the filename portion of pspec, and search the
*           application directory; if found, return the fully-qualified
*           name, else continue.
*       (2) if pspec contains a path specification, search for that
*           path/filename; if found, return with pspec unchanged; if not
*           found, return with error.
*       (3) search for pspec in the current directory; if found, return
*           with pspec unchanged, else continue.
*       (4) search for pspec in the root directory of the current drive;
*           if found, return pspec with '\' prefixed, else continue.
*       (5) search for pspec in each path of the AES path string; if found,
*           return the fully-qualified name.
*       (6) if still not found, return with error.
*/

WORD sh_find(BYTE *pspec)
{
        WORD            path;
        BYTE            *pname;

        KDEBUG(("sh_find(): input pspec='%s'\n",pspec));
        pname = sh_name(pspec);                 /* get ptr to name      */

        dos_sdta(&D.g_dta);

        /* (1) search in the application directory */
        if (rlr->p_appdir[0] != '\0')
        {
          strcpy(D.g_work, rlr->p_appdir);
          strcat(D.g_work, pname);
          dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM);
          if (!DOS_ERR)
          {
            strcpy(pspec, D.g_work);
            KDEBUG(("sh_find(1): returning pspec='%s'\n",pspec));
            return 1;
          }
        }

        /* (2) if filename includes path, search that path */
        if (pname != pspec)
        {
          strcpy(D.g_work, pspec);
          dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM);
          KDEBUG(("sh_find(2): rc=%d, returning pspec='%s'\n",!DOS_ERR,pspec));
          return !DOS_ERR;
        }

        /* (3) search in the current directory */
        sh_curdir(D.g_work);                /* get current drive/dir*/
        if (D.g_work[3] != NULL)            /* if not at root       */
          strcat(D.g_work, "\\");           /*  add backslash       */
        strcat(D.g_work, pname);            /* append name          */
        dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM);
        if (!DOS_ERR)
        {
          KDEBUG(("sh_find(3): returning pspec='%s'\n",pspec));
          return 1;
        }

        /* (4) search in the root directory of the current drive */
        D.g_work[0] = '\\';
        strcpy(D.g_work+1, pname);
        dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM);
        if (!DOS_ERR)
        {
          strcpy(pspec, D.g_work);
          KDEBUG(("sh_find(4): returning pspec='%s'\n",pspec));
          return 1;
        }

        /* (5) search in the AES path */
        path = 0;
        while(1)
        {
          path = sh_path(path, D.g_work, pname);
          if (!path)    /* end of PATH= */
            break;
          dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM);
          if (!DOS_ERR)
          {
            strcpy(pspec, D.g_work);
            KDEBUG(("sh_find(5): returning pspec='%s'\n",pspec));
            return 1;
          }
        }

        KDEBUG(("sh_find(): '%s' not found\n",pspec));
        return 0;
}


#if CONF_WITH_PCGEM
/*
*       Read the default application to invoke.
*/

void sh_rdef(BYTE *lpcmd, BYTE *lpdir)
{
        SHELL           *psh;

        psh = &sh[rlr->p_pid];

        strcpy(lpcmd, &psh->sh_desk[0]);
        strcpy(lpdir, &psh->sh_cdir[0]);
}
#endif

/*
*       Write the default application to invoke
*/

void sh_wdef(const BYTE *lpcmd, const BYTE *lpdir)
{
        SHELL           *psh;

        psh = &sh[rlr->p_pid];

        strcpy(&psh->sh_desk[0], lpcmd);
        strcpy(&psh->sh_cdir[0], lpdir);
}



static void sh_chgrf(SHELL *psh)
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
static void sh_chdef(SHELL *psh)
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
          if(psh->sh_cdir[1] == ':')
            dos_sdrv(psh->sh_cdir[0] - 'A');
          dos_chdir(psh->sh_cdir);
          strcpy(&D.s_cmd[0], &psh->sh_desk[0]);
        }
        else
        {
          if(sh_apdir[1] == ':')
            dos_sdrv(sh_apdir[0] - 'A');        /* desktop's def. dir   */
          dos_chdir(sh_apdir);
        }
}

void aes_run_rom_program(PRG_ENTRY *entry)
{
        PD *pd;     /* this is the BDOS PD structure, not the AESPD */

        /* Create a basepage with the standard Pexec() */
        pd = (PD *) trap1_pexec(PE_BASEPAGE, NULL, "", NULL);
        pd->p_tbase = (LONG) entry;

        /* Run the program with dos_exec() to for AES reentrency issues */
        dos_exec(PE_GOTHENFREE, NULL, (const BYTE *)pd, NULL);
}

static void set_default_desktop(SHELL *psh)
{
        strcpy(psh->sh_desk, DEF_DESKTOP);
        strcpy(psh->sh_cdir, D.s_cdir);
}

static void sh_ldapp(void)
{
        WORD    badtry, retry;
        SHELL   *psh;


        psh = &sh[rlr->p_pid];
        strcpy(sh_apdir, D.s_cdir);             /* initialize sh_apdir  */
        badtry = 0;

        /* Set default DESKTOP if there isn't any yet: */
        if (psh->sh_desk[0] == '\0')
        {
          set_default_desktop(psh);
        }

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

          sh_draw(D.s_cmd, 0, 0);               /* redraw the desktop   */

                                                /* clear his desk field */
          desk_tree[rlr->p_pid] = 0x0L;
                                                /* exec it              */
                                                /* handle bad try msg   */
          if (badtry)
          {
            fm_show(badtry, NULLPTR, 1);
            if (badtry == ALNOFIT)
              break;
            badtry = 0;
          }


          do
          {
            retry = FALSE;

            KDEBUG(("sh_ldapp: Starting %s\n",D.s_cmd));
            if(psh->sh_isdef && strcmp(D.s_cmd, DEF_DESKTOP) == 0)
            {
              /* Start the ROM desktop: */
              sh_show(D.s_cmd);
              p_nameit(rlr, sh_name(&D.s_cmd[0]));
              p_setappdir(rlr, D.s_cmd);
              aes_run_rom_program(deskstart);
            }
#if WITH_CLI != 0
            else if(strcmp(D.s_cmd, "EMUCON") == 0)
            {
              /* start the EmuCON shell: */
              aes_run_rom_program(coma_start);
            }
#endif
            else if ( sh_find(D.s_cmd) )
            {
              /* Run a normal application: */
              sh_show(D.s_cmd);
              p_nameit(rlr, sh_name(&D.s_cmd[0]));
              strcpy(rlr->p_appdir,sh_apdir);
              strcat(rlr->p_appdir,"\\");
              dos_exec(PE_LOADGO, D.s_cmd, ad_stail, ad_envrn);   /* Run the APP */

              /* If the user ran an "autorun" application and quitted it,
                 return now to the default desktop: */
              if (psh->sh_isdef && psh->sh_dodef)
              {
                KDEBUG(("sh_ldapp: Returning to ROM desktop!\n"));
                set_default_desktop(psh);
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
              badtry = AL18ERR;
              set_default_desktop(psh);
            }
          } while (retry && !badtry);

          desk_tree[rlr->p_pid] = 0x0L;         /* clear his desk field */

        } while(psh->sh_doexec && !gl_changerez);

}



void sh_main(void)
{
                                                /* do the exec          */
        sh_ldapp();

                                                /* get back to alpha    */
                                                /*   mode if necessary  */
        if (gl_shgem)
          sh_toalpha();
}
