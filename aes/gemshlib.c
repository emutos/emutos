/*      GEMSHLIB.C      4/18/84 - 09/13/85      Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */
/*      fix sh_envrn                            11/17/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2017 The EmuTOS development team
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
#include "gemerror.h"
#include "aespub.h"
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
#include "gemaplib.h"
#include "gemmnlib.h"
#include "gemasm.h"
#include "optimopt.h"

#include "string.h"
#include "kprint.h"             /* for debugging */

#include "gemshlib.h"

/*
 * clear screen value for ob_spec:
 * white border, white text, hollow pattern, white fill
 */
#define CLEAR_SCREEN    ((WHITE<<12) | (WHITE<< 8) | (IP_HOLLOW<<4) | WHITE)

#define SIZE_AFILE 2048                 /* size of AES shell buffer: must   */
                                        /*  agree with #define in deskapp.h */

static BYTE shelbuf[SIZE_AFILE];        /* AES shell buffer */

GLOBAL SHELL sh[NUM_PDS];

static BYTE sh_apdir[LEN_ZPATH];        /* holds directory of applications to be */
                                        /* run from desktop.  GEMDOS resets dir  */
                                        /* to parent's on return from exec.      */
GLOBAL BYTE *ad_stail;

GLOBAL WORD gl_shgem;

/*
 *  Resolution settings:
 *      gl_changerez: 0=no change, 1=change ST resolution, 2=change Falcon resolution
 *      gl_nextrez: stores the resolution for Setscreen()
 */
GLOBAL WORD gl_changerez;
GLOBAL WORD gl_nextrez;

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
    WORD drive;

    /* remember current directory */
    drive = dos_gdrv();
    *ppath++ = drive + 'A';
    *ppath++ = ':';
    *ppath = '\0';
    dos_gdir(drive+1, ppath);
    if (*ppath == '\0')
    {
        *ppath++ = '\\';
        *ppath = '\0';
    }
}


/*
 *  shel_write: multi-purpose function
 *
 *  performs the following functions:
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
    SHELL *psh;

    if (doex == 5)      /* Change resolution */
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
    sh_curdir(sh_apdir);    /* save app's current directory */

    return TRUE;
}


/*
 *  Used by the DESKTOP to recall SIZE_AFILE bytes worth of previously
 *  'put' desktop-context information.
 */
void sh_get(void *pbuffer, WORD len)
{
    memcpy(pbuffer,shelbuf,len);
}


/*
 *  Used by the DESKTOP to save away SIZE_AFILE bytes worth of desktop-
 *  context information.
 */
void sh_put(const void *pdata, WORD len)
{
    memcpy(shelbuf,pdata,len);
}


/*
 *  Convert the screen to graphics-mode in preparation for the
 *  running of a GEM-based graphic application.
 */
void sh_tographic(void)
{
    /*
     * retake vectors that may have been stepped on by the character
     * application, including the critical error handler and the
     * AES trap
     */
    disable_interrupts();
    retake();
    enable_interrupts();

    gsx_graphic(TRUE);      /* convert to graphic */
    gsx_sclip(&gl_rscreen); /* set initial clip rectangle */
    gsx_malloc();           /* allocate screen space */
    ratinit();              /* start up the mouse */
    set_mouse_to_hourglass();/* put mouse to hourglass */
}


/*
 *  Convert the screen and system back to alpha-mode in preparation for
 *  the running of a DOS-based character application.
 */
static void sh_toalpha(void)
{
    set_mouse_to_arrow();   /* put mouse to arrow */

    /*
     * give back the error handler since ours is graphic
     */
    disable_interrupts();
    giveerr();
    enable_interrupts();

    ratexit();              /* turn off the mouse */
    gsx_mfree();            /* return screen space */
    gsx_graphic(FALSE);     /* close workstation */
}


/*
 * draw (part of) the desktop tree
 *
 * if 'clear' is TRUE, update the ROOT object to clear the screen,
 * and just draw the ROOT; otherwise, draw the entire tree.
 */
static void sh_draw(const BYTE *lcmd, BOOL clear)
{
    OBJECT *tree;

    tree = rs_trees[DESKTOP];
    gsx_sclip(&gl_rscreen);

    if (clear)
    {
        LONG specsave = tree[ROOT].ob_spec;
        tree[ROOT].ob_spec = CLEAR_SCREEN;  /* white desktop screen */
        ob_draw(tree, ROOT, 0);
        tree[ROOT].ob_spec = specsave;
    }
    else
    {
        TEDINFO *ted = (TEDINFO *)tree[DTNAME].ob_spec;
        ted->te_ptext = (BYTE *)lcmd;       /* text string displayed in menu bar */
        ob_draw(tree, ROOT, MAX_DEPTH);
    }
}


static void sh_show(const BYTE *lcmd)
{
    if (!gl_shgem)
        return;

    sh_draw(lcmd, FALSE);
}


/*
 *  Routine to take a full path, and scan back from the end to
 *  find the starting byte of the particular filename
 */
BYTE *sh_name(BYTE *ppath)
{
    BYTE *pname;

    pname = &ppath[strlen(ppath)];
    while((pname >= ppath) && (*pname != '\\') && (*pname != ':'))
        pname--;
    pname++;

    return pname;
}


/*
 *  Search for a particular string in the DOS environment and return
 *  a long pointer to the character after the string if it is found.
 *  Otherwise, return a NULL pointer
 */
void sh_envrn(BYTE **ppath, const BYTE *psrch)
{
    BYTE *lp;
    WORD len, findend;
    BYTE last, tmp, loc1[10], loc2[10];


    len = strlencpy(loc2, psrch);
    len--;

    loc1[len] = '\0';

    lp = ad_envrn;
    findend = FALSE;
    tmp = '\0';
    do
    {
        last = tmp;
        tmp = *lp++;
        if (findend && (tmp == '\0'))
        {
            findend = FALSE;
            tmp = (BYTE)0xFF;
        }
        else
        {
            if (((last == '\0') || (last == -1)) && (tmp == loc2[0]))
            {
                memcpy(loc1, lp, len);
                if (strcmp(&loc1[0], &loc2[1]) == 0)
                {
                    lp += len;
                    break;
                }
            }
            else
                findend = TRUE;
        }
    } while(tmp);

    if (!tmp)
        lp = 0x0L;

    *ppath = lp;
}


/*
 *  Search next style routine to pick up each path in the PATH= portion
 *  of the DOS environment.  It returns a pointer to the start of the
 *  following path until there are no more paths to find.
 */
static BYTE *sh_path(BYTE *src, BYTE *dest, BYTE *pname)
{
    BYTE last = 0;
    BYTE *p;

    if (!src)           /* precautionary */
        return NULL;

    /* check for end of PATH= env var */
    if (!*src)
        return NULL;

    /* copy over path */
    for (p = src; *p; )
    {
        if ((*p == ';') || (*p == ','))
            break;
        last = *p;
        *dest++ = *p++;
    }

    /* see if extra slash is needed */
    if ((last != '\\') && (last != ':'))
        *dest++ = '\\';

    /* append file name */
    strcpy(dest, pname);

    /* point past terminating separator or nul */
    return p + 1;
}


/*
 *  Routine to verify that a file is present.  Note that this routine
 *  tolerates the presence of wildcards in the filespec.
 *
 *  The directory search order is the same as that in TOS2/TOS3/TOS4, as
 *  deduced from tests on those systems:
 *      (1) isolate the filename portion of pspec, and search for it in the
 *          application directory; if found, return the fully-qualified
 *          name, else continue.
 *      (2) search for pspec in the current directory; if found, return
 *          with pspec unchanged, else continue.
 *      (3) search for pspec in the root directory of the current drive;
 *          if found, return pspec with '\' prefixed, else continue.
 *      (4) search for pspec in each path of the AES path string; if found,
 *          return the fully-qualified name.
 *      (5) if still not found, return with error.
 */
WORD sh_find(BYTE *pspec)
{
    BYTE *path;
    BYTE *pname;

    KDEBUG(("sh_find(): input pspec='%s'\n",pspec));
    pname = sh_name(pspec);                 /* get ptr to name      */

    dos_sdta(&D.g_dta);

    /* (1) search in the application directory */
    if (rlr->p_appdir[0] != '\0')
    {
        strcpy(D.g_work, rlr->p_appdir);
        strcat(D.g_work, pname);
        if (dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM) == 0) /* found */
        {
            strcpy(pspec, D.g_work);
            KDEBUG(("sh_find(1): returning pspec='%s'\n",pspec));
            return 1;
        }
    }

    /* (2) search in the current directory */
    strcpy(D.g_work, pspec);
    if (dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM) == 0) /* found */
    {
        KDEBUG(("sh_find(2): returning pspec='%s'\n",pspec));
        return 1;
    }

    /* (3) search in the root directory of the current drive */
    D.g_work[0] = '\\';
    strcpy(D.g_work+1, pname);
    if (dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM) == 0) /* found */
    {
        strcpy(pspec, D.g_work);
        KDEBUG(("sh_find(3): returning pspec='%s'\n",pspec));
        return 1;
    }

    /* (4) search in the AES path */
    sh_envrn(&path, PATH_ENV);      /* find PATH= in the command tail */
    if (!path)
    {
        KDEBUG(("sh_find(): no AES path, '%s' not found\n",pspec));
        return 0;
    }
    if (!*path)                     /* skip nul after PATH= */
        path++;

    while(1)
    {
        path = sh_path(path, D.g_work, pname);
        if (!path)                  /* end of PATH= */
            break;
        if (dos_sfirst(D.g_work, F_RDONLY | F_SYSTEM) == 0) /* found */
        {
            strcpy(pspec, D.g_work);
            KDEBUG(("sh_find(4): returning pspec='%s'\n",pspec));
            return 1;
        }
    }

    KDEBUG(("sh_find(): '%s' not found\n",pspec));
    return 0;
}


#if CONF_WITH_PCGEM
/*
 *  Read the default application to invoke.
 */
void sh_rdef(BYTE *lpcmd, BYTE *lpdir)
{
    SHELL *psh;

    psh = &sh[rlr->p_pid];

    strcpy(lpcmd, psh->sh_desk);
    strcpy(lpdir, psh->sh_cdir);
}
#endif


/*
 *  Write the default application to invoke
 */
void sh_wdef(const BYTE *lpcmd, const BYTE *lpdir)
{
    SHELL *psh;

    psh = &sh[rlr->p_pid];

    strcpy(psh->sh_desk, lpcmd);
    strcpy(psh->sh_cdir, lpdir);
}


static void sh_chgrf(SHELL *psh)
{
    if (psh->sh_isgem != gl_shgem)
    {
        gl_shgem = psh->sh_isgem;
        if (gl_shgem)
            sh_tographic();
        else
            sh_toalpha();
    }
}


static void sh_chdef(SHELL *psh,BOOL isgem)
{
    int n;

    psh->sh_isdef = FALSE;
    if (psh->sh_dodef)
    {
        psh->sh_isdef = TRUE;
        psh->sh_isgem = isgem;  /* FALSE iff a character-mode autorun program */
        if (psh->sh_cdir[1] == ':')
            dos_sdrv(psh->sh_cdir[0] - 'A');
        dos_chdir(psh->sh_cdir);

        /*
         * if not the default desktop, build a fully-qualified name
         */
        n = 0;
        if (strcmp(psh->sh_desk, DEF_DESKTOP) != 0)
        {
            strcpy(D.s_cmd, psh->sh_cdir);
            n = strlen(D.s_cmd);
            if (n)
                D.s_cmd[n++] = '\\';
        }
        strcpy(D.s_cmd+n, psh->sh_desk);
    }
    else
    {
        if (sh_apdir[1] == ':')
            dos_sdrv(sh_apdir[0] - 'A');    /* desktop's def. dir   */
        dos_chdir(sh_apdir);
    }
}


LONG aes_run_rom_program(PRG_ENTRY *entry)
{
    PD *pd;     /* this is the BDOS PD structure, not the AESPD */

    /* Create a basepage with the standard Pexec() */
    pd = (PD *) trap1_pexec(PE_BASEPAGEFLAGS, (char*)PF_STANDARD, "", NULL);
    pd->p_tbase = (BYTE *) entry;

    /* Run the program with dos_exec() for AES reentrancy issues */
    return dos_exec(PE_GOTHENFREE, NULL, (const BYTE *)pd, NULL);
}


static void set_default_desktop(SHELL *psh)
{
    psh->sh_isgem = TRUE;
    strcpy(psh->sh_desk, DEF_DESKTOP);
    strcpy(psh->sh_cdir, D.s_cdir);
}


static WORD sh_ldapp(SHELL *psh)
{
    LONG ret;

    KDEBUG(("sh_ldapp: Starting %s, sh_isgem=%d\n",D.s_cmd,psh->sh_isgem));
    if (psh->sh_isdef && strcmp(D.s_cmd, DEF_DESKTOP) == 0)
    {
        /* Start the ROM desktop: */
        sh_show("");        /* like TOS, we don't display a name */
        p_nameit(rlr, sh_name(D.s_cmd));
        p_setappdir(rlr, D.s_cmd);
        if (aes_run_rom_program(deskstart))
        {
            KDEBUG(("sh_ldapp(): ROM desktop terminated abnormally\n"));
            wm_new();           /* run wind_new() to clean up */
        }
        return 0;
    }

#if WITH_CLI != 0
    if (strcmp(D.s_cmd, "EMUCON") == 0)
    {
        /* start the EmuCON shell: */
        aes_run_rom_program(coma_start);
        return 0;
    }
#endif

    /*
     * we are now going to run a normal application, possibly via
     * autorun.  if it's being run via autorun, sh_isdef will be TRUE,
     * and we should *not* invoke sh_find().  otherwise we do, and
     * only attempt to run the application if it's found.
     */
    ret = (psh->sh_isdef) ? 1 : sh_find(D.s_cmd);
    if (ret)
    {
        /* Run a normal application: */
        sh_show(D.s_cmd);
        p_nameit(rlr, sh_name(D.s_cmd));
        p_setappdir(rlr, D.s_cmd);
        rlr->p_flags = 0;

        ret = dos_exec(PE_LOADGO, D.s_cmd, ad_stail, ad_envrn); /* Run the APP */

        /* if the user did an appl_init() without an appl_exit(),
         * do the important parts for him
         */
        if (rlr->p_flags&AP_OPEN)
        {
            KDEBUG(("sh_ldapp: appl_init() without appl_exit()\n"));
            mn_clsda();
            if (rlr->p_qindex)
                ap_rdwr(MU_MESAG, rlr, rlr->p_qindex, (WORD *)D.g_valstr);
            rlr->p_flags &= ~AP_OPEN;
        }

        /* If the user ran an "autorun" application and quitted it,
         * return now to the default desktop
         */
        if (psh->sh_isdef && psh->sh_dodef)
        {
            KDEBUG(("sh_ldapp: Returning to ROM desktop!\n"));
            set_default_desktop(psh);
        }

        if (wind_spb.sy_owner == rlr)   /* if he still owns screen*/
            unsync(&wind_spb);          /*   then take him off. */

        /*
         * in case the user crashed or left something screwed up,
         * we call wind_new() to do a general cleanup - but only
         * if we were running in graphics mode
         */
        if (psh->sh_isgem)
            wm_new();

        KDEBUG(("sh_ldapp: %s exited with rc=%ld\n",D.s_cmd,ret));
        switch(ret)
        {
        case EFILNF:
        case EPTHNF:
            ret = AL18ERR;      /* "This application cannot find ..." */
            break;
        default:
            if (ret < 0L)
                ret = AL08ERR;  /* "There is not enough memory ..." */
            else
                ret = 0L;
        }
        return ret;
    }

    set_default_desktop(psh);   /* ensure something valid will run */
    return AL18ERR;
}


void sh_main(BOOL isgem)
{
    WORD rc = 0;
    SHELL *psh;

    psh = &sh[rlr->p_pid];
    strcpy(sh_apdir, D.s_cdir);         /* initialize sh_apdir  */

    /* Set default DESKTOP if there isn't any yet */
    if (psh->sh_desk[0] == '\0')
        set_default_desktop(psh);

    /*
     * Loop until a resolution change or a shutdown
     */
    do
    {
        sh_chdef(psh,isgem);
        /*
         * set up to run the default app, i.e. the desktop, immediately
         * after this one, and make sure it's started in graphics mode
         */
        psh->sh_dodef = TRUE;
        isgem = TRUE;
        sh_chgrf(psh);                  /* set alpha/graphics mode */

        if (gl_shgem)
        {
            wm_start();
            ratinit();
            sh_draw(D.s_cmd, TRUE);     /* clear the screen */
        }

        if (rc)                         /* display alert for most recent error */
            fm_show(rc, NULL, 1);

        rc = sh_ldapp(psh);             /* run the desktop/console/app */

    } while(psh->sh_doexec && !gl_changerez);

    if (gl_shgem)                       /* if graphics mode,     */
        sh_toalpha();                   /*  switch back to alpha */
}
