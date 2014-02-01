/*      GEMSHLIB.C      4/18/84 - 09/13/85      Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */
/*      fix sh_envrn                            11/17/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
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

#include "config.h"
#include "portab.h"
#include "compat.h"
#include "asm.h"

#include "obdefs.h"
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

#include "string.h"
#include "kprint.h"             /* for debugging */

#include "gemshlib.h"

#define DBGSHLIB 0

#if DBGSHLIB
#define Dprintf(a) kprintf a
#else
#define Dprintf(a)
#endif

GLOBAL SHELL    sh[NUM_PDS];

static BYTE sh_apdir[LEN_ZPATH];                /* holds directory of   */
                                                /*   applications to be */
                                                /*   run from desktop.  */
                                                /*   GEMDOS resets dir  */
                                                /*   to parent's on ret */
                                                /*   from exec.         */
GLOBAL BYTE     *ad_stail;
GLOBAL LONG     ad_ssave;

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
        memcpy(ptail, (BYTE *)ad_stail, 128);
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



static BYTE *sh_parse(BYTE *psrc, BYTE *pfcb)
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
static void sh_fixtail(WORD iscpm)
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

        memcpy(s_tail+i, (BYTE *)ad_stail, 128 - i);

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
        }
                                                /* copy into true tail  */
        memcpy((BYTE *)ad_stail, s_tail, 128);
}



/*
*       Routine to set the next application to run
*
*               isgem = 0   then run in character mode
*               isgem = 1   them run in graphic mode
*
*               isover = 0  then run above DESKTOP
*               isover = 1  then run over DESKTOP
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
        memcpy((BYTE *)ad_stail, ptail, 128);

        if (isover > 0)
        {
                                                /* stepaside to run     */
          psh = &sh[rlr->p_pid];
          psh->sh_isgem = (isgem != FALSE);
          psh->sh_doexec = doex;
          psh->sh_dodef = FALSE;
          psh->sh_fullstep = isover - 1;
          sh_curdir(sh_apdir);                  /* save apps. current   */
                                                /* directory            */
        }
        else
        {
          sh_fixtail(FALSE);
                                                /* run it above us      */
          if ( sh_find(D.s_cmd) )
          {
            /* Normal Atari-GEM's shel_write does not support running PRGs directly! */
            /*dos_exec(0, D.s_cmd, ad_stail, ad_envrn);*/
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
void sh_get(void *pbuffer, WORD len)
{
        memcpy(pbuffer, (BYTE *)ad_ssave, len);
}


/*
*       Used by the DESKTOP to save away 1024 bytes worth of desktop-
*       context information.
*/
void sh_put(const void *pdata, WORD len)
{
        memcpy((BYTE *)ad_ssave, pdata, len);
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
        cli();
        retake();
        sti();
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
        cli();
        giveerr();
        sti();
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
          tree = ad_stdesk;
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

        Dprintf(("sh_find(): input pspec='%s'\n",pspec));
        pname = sh_name(pspec);                 /* get ptr to name      */

        dos_sdta((LONG)D.g_dta);

        /* (1) search in the application directory */
        if (rlr->p_appdir[0] != '\0')
        {
          strcpy(D.g_dir, rlr->p_appdir);
          strcat(D.g_dir, pname);
          dos_sfirst(D.g_dir, F_RDONLY | F_SYSTEM);
          if (!DOS_ERR)
          {
            strcpy(pspec, D.g_dir);
            Dprintf(("sh_find(1): returning pspec='%s'\n",pspec));
            return 1;
          }
        }

        /* (2) if filename includes path, search that path */
        if (pname != pspec)
        {
          strcpy(D.g_dir, pspec);
          dos_sfirst(D.g_dir, F_RDONLY | F_SYSTEM);
          Dprintf(("sh_find(2): rc=%d, returning pspec='%s'\n",!DOS_ERR,pspec));
          return !DOS_ERR;
        }

        /* (3) search in the current directory */
        sh_curdir(D.g_dir);                 /* get current drive/dir*/
        if (D.g_dir[3] != NULL)             /* if not at root       */
          strcat(D.g_dir, "\\");            /*  add backslash       */
        strcat(D.g_dir, pname);             /* append name          */
        dos_sfirst(D.g_dir, F_RDONLY | F_SYSTEM);
        if (!DOS_ERR)
        {
          Dprintf(("sh_find(3): returning pspec='%s'\n",pspec));
          return 1;
        }

        /* (4) search in the root directory of the current drive */
        D.g_dir[0] = '\\';
        strcpy(D.g_dir+1, pname);
        dos_sfirst(D.g_dir, F_RDONLY | F_SYSTEM);
        if (!DOS_ERR)
        {
          strcpy(pspec, D.g_dir);
          Dprintf(("sh_find(4): returning pspec='%s'\n",pspec));
          return 1;
        }

        /* (5) search in the AES path */
        path = 0;
        while(1)
        {
          path = sh_path(path, D.g_dir, pname);
          if (!path)    /* end of PATH= */
            break;
          dos_sfirst(D.g_dir, F_RDONLY | F_SYSTEM);
          if (!DOS_ERR)
          {
            strcpy(pspec, D.g_dir);
            Dprintf(("sh_find(5): returning pspec='%s'\n",pspec));
            return 1;
          }
        }

        Dprintf(("sh_find(): '%s' not found\n",pspec));
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
          psh->sh_fullstep = 0;
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

typedef void PRG_ENTRY(void); /* Program entry point type */

static void aes_run_rom_program(PRG_ENTRY *entry)
{
        LONG *pd; /* FIXME: Use BDOS PD struct after fixing AES PD conflict */

        /* Create a basepage with the standard Pexec() */
        pd = (LONG *) trap1_pexec(5, NULL, "", NULL);
        pd[2] = (LONG) entry;
        pd[3] = pd[5] = pd[7] = 0;

        /* Run the program with dos_exec() to for AES reentrency issues */
        dos_exec(6, NULL, (const BYTE *)pd, NULL);
}

static void sh_ldapp(void)
{
        WORD    ret, badtry, retry;
        SHELL   *psh;


        psh = &sh[rlr->p_pid];
        strcpy(sh_apdir, D.s_cdir);             /* initialize sh_apdir  */
        badtry = 0;

        /* Set default DESKTOP if there isn't any yet: */
        if(psh->sh_desk[0] == 0)
        {
          strcpy(&psh->sh_desk[0], DEF_DESKTOP);
          strcpy(&psh->sh_cdir[0], &D.s_cdir[0]);
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
                                                /* fix up/parse cmd tail*/
          sh_fixtail(psh->sh_fullstep == 2);
          sh_draw(D.s_cmd, 0, 0);               /* redraw the desktop   */

                                                /* clear his desk field */
          desk_tree[rlr->p_pid] = 0x0L;
                                                /* exec it              */
                                                /* handle bad try msg   */
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

            Dprintf(("sh_ldapp: Starting %s\n", D.s_cmd));
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
              if (psh->sh_fullstep == 0)
              {
                dos_exec(0, D.s_cmd, ad_stail, ad_envrn);   /* Run the APP */

                /* If the user ran an alternative desktop and quitted it,
                   return now to the default desktop: (experimental) */
                if(psh->sh_isdef && psh->sh_dodef)
                {
                  Dprintf(("sh_ldapp: Returning to ROM desktop!\n"));
                  strcpy(&psh->sh_desk[0], DEF_DESKTOP);
                  strcpy(&psh->sh_cdir[0], &D.s_cdir[0]);
                }
              }
              else if (psh->sh_fullstep == 1)
              {
                dos_exec(0, D.s_cmd, ad_stail, ad_envrn);
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
