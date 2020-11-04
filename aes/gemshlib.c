/*      GEMSHLIB.C      4/18/84 - 09/13/85      Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */
/*      fix sh_envrn                            11/17/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
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

#include "emutos.h"
#include "asm.h"

#include "obdefs.h"
#include "struct.h"
#include "aesvars.h"
#include "bdosbind.h"
#include "xbiosbind.h"
#include "gemerror.h"
#include "aesdefs.h"
#include "aesext.h"
#include "gemlib.h"
#include "gem_rsc.h"
#include "biosext.h"
#include "optimize.h"

#include "gemdosif.h"
#include "gemdos.h"
#include "gemgsxif.h"
#include "gemoblib.h"
#include "gemwmlib.h"
#include "gemfmlib.h"
#include "gempd.h"
#include "gemflag.h"
#include "geminit.h"
#include "gemaplib.h"
#include "gemmnlib.h"

#include "string.h"

#include "gemshlib.h"
#include "../desk/deskstub.h"

#if WITH_CLI
#include "../cli/clistub.h"
#endif

/*
 * clear screen value for ob_spec:
 * white border, white text, hollow pattern, white fill
 */
#define CLEAR_SCREEN    ((WHITE<<12) | (WHITE<< 8) | (IP_HOLLOW<<4) | WHITE)


/*
 * values used in sh_nextapp below
 */
#define NORMAL_APP  0
#define CONSOLE_APP 1
#define AUTORUN_APP 2
#define DESKTOP_APP 3

typedef struct
{
    WORD sh_doexec;             /* for values, see aesdefs.h */
    WORD sh_nextapp;            /* type of application to be run next (see above) */
    BOOL sh_isgem;              /* TRUE if the application to be run is a GEM */
                                /*  application; FALSE if character-mode      */
    char sh_desk[LEN_ZFNAME];   /* the name of the default startup app */
    char sh_cdir[LEN_ZPATH];    /* the current directory for the default startup app */
} SHELL;

static SHELL sh;

static char sh_apdir[LEN_ZPATH];        /* saves initial value of current directory */
                                        /* for applications run from the desktop.   */
GLOBAL char *ad_stail;

static BOOL gl_shgem;                   /* TRUE iff currently in graphics mode */

/*
 *  Resolution settings:
 *      gl_changerez: 0=no change, 1=change ST resolution, 2=change Falcon resolution
 *      gl_nextrez: stores the resolution for Setscreen()
 */
GLOBAL WORD gl_changerez;
GLOBAL WORD gl_nextrez;


void sh_read(char *pcmd, char *ptail)
{
    strcpy(pcmd, D.s_cmd);
    memcpy(ptail, ad_stail, CMDTAILSIZE);
}


static void sh_curdrvdir(char *ppath)
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
 *      (2) doex = 4
 *          . system shutdown management
 *              isgem       ignored
 *              isover = 0  abort shutdown (not possible, ignored)
 *              isover = 1  partial shutdown (currently the same as full)
 *              isover = 2  full shutdown
 *
 *      (3) doex = 1
 *          . set the next application to run after the current one terminates
 *              isover      ignored, should currently always be 1
 *              isgem = 0   run in character mode (TOS/TTP)
 *              isgem = 1   run in graphic mode (APP/PRG)
 *
 *      (4) doex = 0
 *          . run no application after the current one terminates, i.e.
 *            just exit to desktop
 */
WORD sh_write(WORD doex, WORD isgem, WORD isover, const char *pcmd, const char *ptail)
{
    SHELL *psh = &sh;

    switch(doex) {
    case SHW_NOEXEC:    /* exit to desktop */
        strcpy(D.s_cmd, DEF_DESKTOP);
        psh->sh_doexec = doex;
        psh->sh_nextapp = DESKTOP_APP;
        psh->sh_isgem = TRUE;
        break;
    case SHW_EXEC:      /* run another program */
        strcpy(D.s_cmd, pcmd);
        memcpy(ad_stail, ptail, CMDTAILSIZE);
        psh->sh_doexec = doex;
        psh->sh_nextapp = (strcmp(pcmd, DEF_CONSOLE) == 0) ? CONSOLE_APP : NORMAL_APP;
        psh->sh_isgem = (isgem != FALSE);
        if (psh->sh_nextapp == NORMAL_APP)
            sh_curdrvdir(sh_apdir);     /* save app's current directory */
        break;
#if CONF_WITH_SHUTDOWN
    case SHW_SHUTDOWN:  /* shutdown system */
        if (can_shutdown())
        {
            psh->sh_doexec = doex;
            psh->sh_nextapp = NORMAL_APP;   /* irrelevant, I think */
            psh->sh_isgem = FALSE;
        }
        break;
#endif
    case SHW_RESCHNG:   /* change resolution */
        gl_changerez = 1 + isover;
        gl_nextrez = isgem;
        D.s_cmd[0] = '\0';
        ad_stail[0] = '\0';
        break;
    }

    return TRUE;
}


/*
 *  Used by the DESKTOP to recall up to SIZE_SHELBUF bytes worth of previously
 *  'put' desktop-context information.
 */
void sh_get(void *pbuffer, WORD len)
{
    memcpy(pbuffer, D.g_shelbuf, len);
}


/*
 *  Used by the DESKTOP to save up to SIZE_SHELBUF bytes worth of desktop-
 *  context information.
 */
void sh_put(const void *pdata, WORD len)
{
    memcpy(D.g_shelbuf, pdata, len);
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
static void sh_draw(const char *lcmd, BOOL clear)
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
        ted->te_ptext = (char *)lcmd;       /* text string displayed in menu bar */
        ob_draw(tree, ROOT, MAX_DEPTH);
    }
}


static void sh_show(const char *lcmd)
{
    if (!gl_shgem)
        return;

    sh_draw(lcmd, FALSE);
}


/*
 *  Return a pointer to the start of the filename in a path
 *  (assumed to be the last component of the path)
 */
char *sh_name(char *ppath)
{
    char *pname = ppath;

    /*
     * note: filename_start() assumes that there is a filename separator
     * within the path, so we handle a path like X:AAAAAAAA.BBB before
     * calling the general function
     */
    if (ppath[0] && (ppath[1] == ':'))
        pname += 2;

    return filename_start(pname);
}


/*
 *  Search for a particular string in the DOS environment and return a
 *  value in the pointer pointed to by the first argument.  If the string
 *  is found, the value is a pointer to the first character after the
 *  string; otherwise it is a NULL pointer.
 */
void sh_envrn(char **ppath, const char *psrch)
{
    char *p;
    WORD len;

    len = strlen(psrch);
    *ppath = NULL;

    /*
     * scan environment string until double nul
     */
    for (p = ad_envrn; *p; )
    {
        if (strncmp(p, psrch, len) == 0)
        {
            *ppath = p + len;
            break;
        }
        while(*p++) /* skip to end of current env variable */
            ;
    }
}


/*
 *  Search next style routine to pick up each path in the PATH= portion
 *  of the DOS environment.  It returns a pointer to the start of the
 *  following path until there are no more paths to find.
 */
static char *sh_path(char *src, char *dest, char *pname)
{
    char last = 0;
    char *p;

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
static WORD findfile(char *pspec)
{
    char *path;
    char *pname;

    KDEBUG(("sh_find(): input pspec='%s'\n",pspec));
    pname = sh_name(pspec);                 /* get ptr to name      */

    dos_sdta(&D.g_dta);

    /* (1) search in the application directory */
    if (rlr->p_appdir[0] != '\0')
    {
        strcpy(D.g_work, rlr->p_appdir);
        strcat(D.g_work, pname);
        if (dos_sfirst(D.g_work, FA_RO | FA_HIDDEN | FA_SYSTEM) == 0)   /* found */
        {
            strcpy(pspec, D.g_work);
            KDEBUG(("sh_find(1): returning pspec='%s'\n",pspec));
            return 1;
        }
    }

    /* (2) search in the current directory */
    strcpy(D.g_work, pspec);
    if (dos_sfirst(D.g_work, FA_RO | FA_HIDDEN | FA_SYSTEM) == 0)   /* found */
    {
        KDEBUG(("sh_find(2): returning pspec='%s'\n",pspec));
        return 1;
    }

    /* (3) search in the root directory of the current drive */
    D.g_work[0] = '\\';
    strcpy(D.g_work+1, pname);
    if (dos_sfirst(D.g_work, FA_RO | FA_HIDDEN | FA_SYSTEM) == 0)   /* found */
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
        if (dos_sfirst(D.g_work, FA_RO | FA_HIDDEN | FA_SYSTEM) == 0)   /* found */
        {
            strcpy(pspec, D.g_work);
            KDEBUG(("sh_find(4): returning pspec='%s'\n",pspec));
            return 1;
        }
    }

    KDEBUG(("sh_find(): '%s' not found\n",pspec));
    return 0;
}

WORD sh_find(char *pspec)
{
    DTA *save_dta;
    WORD ret;

    save_dta = dos_gdta();      /* save, findfile() modifies it */
    ret = findfile(pspec);      /* do the actual shel_find() */
    dos_sdta(save_dta);         /* restore */

    return ret;
}


#if CONF_WITH_PCGEM
/*
 *  Read the default application to invoke.
 */
void sh_rdef(char *lpcmd, char *lpdir)
{
    SHELL *psh = &sh;

    strcpy(lpcmd, psh->sh_desk);
    strcpy(lpdir, psh->sh_cdir);
}
#endif


/*
 *  Write the default application to invoke
 */
void sh_wdef(const char *lpcmd, const char *lpdir)
{
    SHELL *psh = &sh;

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


static void sh_chdef(SHELL *psh)
{
    int n;

    switch(psh->sh_nextapp) {
    case NORMAL_APP:
        if (sh_apdir[1] == ':')     /* set default drive (if specified) */
            dos_sdrv(sh_apdir[0] - 'A');
        dos_chdir(sh_apdir);        /* and default directory */
        break;
    case CONSOLE_APP:
        break;
    case AUTORUN_APP:
    case DESKTOP_APP:
        if (psh->sh_cdir[1] == ':')
            dos_sdrv(psh->sh_cdir[0] - 'A');
        dos_chdir(psh->sh_cdir);

        /*
         * if not the default desktop, build a fully-qualified name
         */
        n = 0;
        if (psh->sh_nextapp == AUTORUN_APP)
        {
            strcpy(D.s_cmd, psh->sh_cdir);
            n = strlen(D.s_cmd);
            if (n)
                D.s_cmd[n++] = '\\';
        }
        strcpy(D.s_cmd+n, psh->sh_desk);
        break;
    }
}


LONG aes_run_rom_program(PRG_ENTRY *entry)
{
    PD *pd;     /* this is the BDOS PD structure, not the AESPD */

    /* Create a basepage with the standard Pexec() */
    pd = (PD *) Pexec(PE_BASEPAGEFLAGS, (char*)PF_STANDARD, "", NULL);
    pd->p_tbase = (UBYTE *) entry;

    /* Run the program with dos_exec() for AES reentrancy issues */
    return dos_exec(PE_GOTHENFREE, NULL, (const char *)pd, NULL);
}


static void set_default_desktop(SHELL *psh)
{
    psh->sh_nextapp = DESKTOP_APP;
    psh->sh_isgem = TRUE;
    strcpy(psh->sh_desk, DEF_DESKTOP);
    strcpy(psh->sh_cdir, D.s_cdir);
}


static WORD sh_ldapp(SHELL *psh)
{
    char *fname = sh_name(D.s_cmd);     /* filename portion of program */
    LONG ret;

    KDEBUG(("sh_ldapp: Starting %s, sh_nextapp=%d, sh_isgem=%d\n",
            D.s_cmd,psh->sh_nextapp,psh->sh_isgem));

    if (psh->sh_nextapp == DESKTOP_APP)
    {
        /* Start the ROM desktop: */
        sh_show("");        /* like TOS, we don't display a name */
        p_nameit(rlr, fname);
        p_setappdir(rlr, D.s_cmd);
        if (aes_run_rom_program(deskstart))
        {
            KDEBUG(("sh_ldapp(): ROM desktop terminated abnormally\n"));
            wm_new();           /* run wind_new() to clean up */
        }
        return 0;
    }

#if WITH_CLI
    if (psh->sh_nextapp == CONSOLE_APP)
    {
        /* start the EmuCON shell: */
        aes_run_rom_program(coma_start);
        psh->sh_nextapp = DESKTOP_APP;
        psh->sh_isgem = TRUE;
        return 0;
    }
#endif

    /*
     * we are now going to run a normal application, possibly via autorun.
     * if it's being run via autorun, we should *not* invoke sh_find().
     * otherwise we do, and only attempt to run the application if it's found.
     */
    ret = (psh->sh_nextapp == AUTORUN_APP) ? 1 : sh_find(D.s_cmd);
    if (ret)
    {
        /* Run a normal or autorun application: */
        sh_show(fname);
        p_nameit(rlr, fname);
        p_setappdir(rlr, D.s_cmd);
        rlr->p_flags = 0;

        /* by default, run the desktop after a normal application */
        if (psh->sh_nextapp == NORMAL_APP)
        {
            psh->sh_nextapp = DESKTOP_APP;
            psh->sh_isgem = TRUE;
        }

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
        if (psh->sh_nextapp == AUTORUN_APP)
        {
            KDEBUG(("sh_ldapp: autorun program returning to ROM desktop\n"));
            set_default_desktop(psh);
        }

        if (wind_spb.sy_owner == rlr)   /* if he still owns screen*/
            unsync(&wind_spb);          /*   then take him off. */

        /*
         * in case the user crashed or left something screwed up,
         * we call wind_new() to do a general cleanup - but only
         * if we were running in graphics mode
         */
        if (gl_shgem)
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


void sh_main(BOOL isauto, BOOL isgem)
{
    WORD rc = 0;
    SHELL *psh = &sh;

    psh->sh_doexec = SHW_EXEC;
    psh->sh_nextapp = isauto ? AUTORUN_APP : DESKTOP_APP;
    psh->sh_isgem = isgem;              /* may be character mode if autorun */
    strcpy(sh_apdir, D.s_cdir);         /* initialize sh_apdir  */
    gl_shgem = TRUE;

    /* Set default DESKTOP if no autorun app */
    if (psh->sh_nextapp == DESKTOP_APP)
        set_default_desktop(psh);

    /*
     * Loop until a resolution change or a shutdown
     */
    do
    {
        sh_chdef(psh);
        sh_chgrf(psh);                  /* set alpha/graphics mode */

        if (gl_shgem)
        {
            wm_init();                  /* re-init windows, without resetting colours */
            ratinit();
            sh_draw(D.s_cmd, TRUE);     /* clear the screen */
        }

        if (rc)                         /* display alert for most recent error */
            fm_show(rc, NULL, 1);

        rc = sh_ldapp(psh);             /* run the desktop/console/app */

    } while((psh->sh_doexec != SHW_SHUTDOWN) && !gl_changerez);

    if (gl_shgem)                       /* if graphics mode,     */
        sh_toalpha();                   /*  switch back to alpha */
}
