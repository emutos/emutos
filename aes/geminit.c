/*      GEMINIT.C       4/23/84 - 08/14/85      Lee Lorenzen            */
/*      GEMCLI.C        1/28/84 - 08/14/85      Lee Jay Lorenzen        */
/*      GEM 2.0         10/31/85                Lowell Webster          */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */
/*      fix command tail handling               10/19/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
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
#include "obdefs.h"
#include "struct.h"
#include "basepage.h"
#include "gemlib.h"
#include "crysbind.h"
#include "gem_rsc.h"
#include "dos.h"
#include "xbiosbind.h"
#include "../bios/screen.h"
#include "../bios/videl.h"
#include "biosbind.h"

#include "gemgsxif.h"
#include "gemdosif.h"
#include "gemctrl.h"
#include "gemshlib.h"
#include "gempd.h"
#include "gemdisp.h"
#include "gemrslib.h"
#include "gemobed.h"
#include "gemdos.h"
#include "gemgraf.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemfslib.h"
#include "gemoblib.h"
#include "gemsclib.h"
#include "gemfmlib.h"
#include "gemasm.h"
#include "gemaplib.h"
#include "gemsuper.h"
#include "geminput.h"
#include "gemmnlib.h"
#include "geminit.h"
#include "optimize.h"
#include "optimopt.h"
#include "aespub.h"

#include "string.h"
#include "biosdefs.h"
#include "kprint.h"

extern LONG size_theglo(void); /* called only from gemstart.S */
extern LONG init_p0_stkptr(void); /* called only from gemstart.S */
extern void run_accs_and_desktop(void); /* called only from gemstart.S */
extern void gem_main(void); /* called only from gemstart.S */

#define ROPEN 0

#define INF_SIZE   300                  /* size of buffer used by sh_rdinf() */
                                        /*  for start of EMUDESK.INF file    */
#define ENV_SIZE   200                  /* size of initial desktop environment */


static BYTE     infbuf[INF_SIZE+1];     /* used to read part of EMUDESK.INF */
static BYTE     envbuf[ENV_SIZE];       /* for initial desktop environment */
static BYTE     acc_name[NUM_ACCS][LEN_ZFNAME]; /* used by count_accs()/ldaccs() */

/* Some global variables: */

GLOBAL const GEM_MUPB ui_mupb =
{
    GEM_MUPB_MAGIC, /* Magic value identifying this structure */
    _endgembss,     /* End of GEM BSS */
    ui_start        /* AES entry point */
};

GLOBAL WORD     totpds;
GLOBAL WORD     num_accs;

GLOBAL MFORM    *ad_armice;
GLOBAL MFORM    *ad_hgmice;
GLOBAL BYTE     *ad_envrn;              /* initialized in GEMSTART      */

GLOBAL MFORM    gl_mouse;
GLOBAL BYTE     gl_logdrv;

GLOBAL AESPD    *rlr, *drl, *nrl;
GLOBAL EVB      *eul, *dlr, *zlr;

GLOBAL LONG     elinkoff;

GLOBAL BYTE     indisp;

GLOBAL WORD     fpt, fph, fpcnt;                /* forkq tail, head,    */
                                                /*   count              */
GLOBAL SPB      wind_spb;
GLOBAL WORD     curpid;

GLOBAL THEGLO   D;

/* Prototypes: */
extern void accdesk_start(void) NORETURN;   /* see gemstart.S */

/*
 *  Convert a single hex ASCII digit to the corresponding decimal number
 *
 *  Note that this could be static, since it is used only within this
 *  module.  However, making it so will consume more code space in
 *  current versions of GCC, even with -Os optimisation
 */
UBYTE hex_dig(BYTE achar)
{
    if ((achar >= '0') && (achar <= '9'))
        return (achar - '0');

    achar = toupper(achar);
    if ((achar >= 'A') && (achar <= 'F'))
        return (achar - 'A' + 10);

    return 0;
}


/*
 *  Starting at the specified position within a string, skip over any
 *  leading spaces.  If the next non-space byte is '\r', stop scanning,
 *  set the scanned value to zero, and return a pointer to the '\r'.
 *
 *  Otherwise, convert the next two characters (assumed to be hex digits)
 *  into a value N.  If N is 0xff, set the scanned value to -1; otherwise
 *  set the scanned value to N.  In either case, return a pointer to the
 *  byte immediately following the two hex characters.
 */
BYTE *scan_2(BYTE *pcurr, WORD *pwd)
{
    WORD temp = 0;

    while(*pcurr == ' ')
        pcurr++;

    if (*pcurr != '\r')
    {
        temp = hex_dig(*pcurr++) << 4;
        temp |= hex_dig(*pcurr++);
        if (temp == 0x00ff)
            temp = -1;
        *pwd = temp;
    }

    return pcurr;
}


/*
 *  return size of global area
 *  called from gemstart.S
 */
LONG size_theglo(void)
{
    return sizeof(THEGLO);
}


/*
 *  called from startup code to initialise the process 0 supervisor stack ptr:
 *      1. determines the end of the supervisor stack
 *      2. initialises the supervisor stack pointer in the UDA
 *      3. returns the offset from the start of THEGLO to the end of the stack
 */
LONG init_p0_stkptr(void)
{
    UDA *u = &D.g_int[0].a_uda;

    u->u_spsuper = &u->u_supstk + 1;

    return (char *)u->u_spsuper - (char *)u;
}


static void ev_init(EVB evblist[], WORD cnt)
{
    WORD    i;

    for (i = 0; i < cnt; i++)
    {
        evblist[i].e_nextp = eul;
        eul = &evblist[i];
    }
}


/*
 *  Create a local process for the routine and start him executing.
 *  Also do all the initialization that is required.
 *      TODO - get rid of this.
 */
static AESPD *iprocess(BYTE *pname, PFVOID routine)
{
    ULONG ldaddr;

    KDEBUG(("iprocess(\"%s\")\n", (const char*)pname));

    /* figure out load addr */

    ldaddr = (ULONG) routine;

    /* create process to execute it */
    return pstart(routine, pname, ldaddr);
}


/*
 *  Start up the file selector by initializing the fs_tree
 */
static void fs_start(void)
{
    OBJECT *tree;

    tree = rs_trees[FSELECTR];
    ad_fstree = (LONG)tree;
    ob_center((LONG)tree, &gl_rfs);
}


/*
 *  Routine to load program file pointed at by pfilespec, then create a
 *  new process context for it.  This is used to load a desk accessory.
 */
static void sndcli(BYTE *pfilespec)
{
    WORD    handle;
    WORD    err_ret;
    LONG    ldaddr, ret;

    KDEBUG(("sndcli(\"%s\")\n", (const char*)pfilespec));

    strcpy(D.s_cmd, pfilespec);

    ret = dos_open(D.s_cmd, ROPEN);
    if (ret >= 0L)
    {
        handle = (WORD)ret;
        err_ret = pgmld(handle, D.s_cmd, (LONG **)&ldaddr);
        dos_close(handle);
        /* create process to execute it */
        if (err_ret != -1)
            pstart(gotopgm, pfilespec, ldaddr);
    }
}


/*
 *  Count up to a maximum of NUM_ACCS desk accessories, saving
 *  their names in acc_name[].
 */
static WORD count_accs(void)
{
    WORD i, rc;

    /* if Control is held down, skip loading of accessories */
    if ((Kbshift(-1) & MODE_CTRL))
        return 0;

    strcpy(D.g_work,"*.ACC");
    dos_sdta(&D.g_dta);

    for (i = 0; i < NUM_ACCS; i++)
    {
        rc = (i==0) ? dos_sfirst(D.g_work,F_RDONLY) : dos_snext();
        if (rc < 0)
            break;
        strlcpy(acc_name[i],D.g_dta.d_fname,LEN_ZFNAME);
    }

    return i;
}


/*
 *  Load in the desk accessories specified by acc_name[]
 */
static void load_accs(WORD n)
{
    WORD i;

    for (i = 0; i < n; i++)
        sndcli(acc_name[i]);
}


static void sh_addpath(void)
{
    char    *lp, *np, *new_envr;
    const char *pp;
    WORD    oelen, oplen, nplen, pplen, fstlen;
    BYTE    tmp;
    char    tmpstr[MAX_LEN];

    lp = ad_envrn;

    /* get to end of environment string (ends with 2 nulls) */
    while (*lp || *(lp+1))
        lp++;
    lp++;               /* past 2nd null */

    /* old environment length*/
    oelen = (lp - ad_envrn) + 2;

    /* PATH= length & new path length */
    pp = PATH_ENV;
    strcpy(tmpstr, DEF_PATH);
    np = tmpstr;

    pplen = strlen(pp);
    nplen = strlen(np);

    if (oelen+nplen+pplen+1 > ENV_SIZE)
    {
        KDEBUG(("sh_addpath(): cannot add path, environment buffer too small\n"));
        return;
    }

    /* fix up drive letters */
    lp = np;
    while ((tmp = *lp) != 0)
    {
        if (tmp == ':')
            *(lp-1) = gl_logdrv;
        lp++;
    }

    /* alloc new environ    */
    new_envr = envbuf;

    /* get ptr to initial PATH= */
    sh_envrn(&lp, pp);

    if (lp)
    {
        /* first part length    */
        oplen = strlen(lp);             /* length of actual path */

        fstlen = lp - ad_envrn + oplen; /* len thru end of path */
        memcpy(new_envr,ad_envrn,fstlen);
    }
    else
    {
        oplen = 0;
        strcpy(new_envr,pp);
        fstlen = pplen + 1;
    }

    if (oplen)
    {
        *(new_envr+fstlen) = ';';       /* to splice in new path */
        fstlen += 1;
    }

    memcpy(new_envr+fstlen,np,nplen);   /* splice on more path */

    /* copy rest of environ */
    if (lp)
        memcpy(new_envr+fstlen+nplen,lp+oplen,oelen-fstlen);

    ad_envrn = new_envr;                /* remember new environ.*/
}


void sh_deskf(WORD obj, LONG plong)
{
    OBJECT *tree;

    tree = rs_trees[DESKTOP];
    *(LONG *)plong = tree[obj].ob_spec;
}


static void sh_init(void)
{
    SHELL   *psh;

    psh = sh;

    sh_deskf(2, (LONG)&ad_pfile);

    /* add in internal search paths with right drive letter */
    sh_addpath();

    /* set defaults */
    psh->sh_doexec = psh->sh_dodef = gl_shgem = psh->sh_isgem = TRUE;
}


/*
 *  Routine to read in the start of the emudesk.inf file,
 *  expected to contain the #E and #Z lines.
 */
static void sh_rdinf(void)
{
    WORD    fh;
    LONG    size, ret;
    char    *pfile;
    char    tmpstr[MAX_LEN];

    infbuf[0] = 0;

    strcpy(tmpstr, INF_FILE_NAME);
    pfile = tmpstr;
    *pfile += dos_gdrv();                   /* set the drive        */

    ret = dos_open(pfile, ROPEN);
    if (ret < 0L)
        return;
    fh = (WORD)ret;

    /* NOTA BENE: all required info MUST be within INF_SIZE
     * bytes from the beginning of the file
     */
    size = dos_read(fh, INF_SIZE, infbuf);
    dos_close(fh);
    if (size < 0L)      /* if read error, force empty buffer */
        size = 0L;
    infbuf[size] = 0;
}


/*
 *  Part 1 of early emudesk.inf processing
 *
 *  This has one function: determine if we need to change resolutions
 *  (from #E).  If so, we set gl_changerez and gl_nextrez appropriately.
 */
static void process_inf1(void)
{
#if CONF_WITH_ATARI_VIDEO
    WORD    env1, env2;
    WORD    mode;
#endif
    char    *pcurr;

    gl_changerez = 0;           /* assume no change */

    for (pcurr = infbuf; *pcurr; )
    {
        if ( *pcurr++ != '#' )
            continue;
        if (*pcurr++ == 'E')            /* #E 3A 11 FF 02               */
        {                               /* desktop environment          */
            pcurr += 6;                 /* skip over non-video preferences */
            if (*pcurr == '\r')         /* no video info saved */
                break;

#if CONF_WITH_ATARI_VIDEO
            pcurr = scan_2(pcurr, &env1);
            pcurr = scan_2(pcurr, &env2);
            mode = (env1 << 8) | (env2 & 0x00ff);
            mode = check_moderez(mode);
            if (mode == 0)              /* no change required */
                break;
            if (mode > 0)               /* need to set Falcon mode */
            {
                gl_changerez = 2;
                gl_nextrez = mode;
            }
            else                        /* set ST/TT rez */
            {
                gl_changerez = 1;
                gl_nextrez = (mode & 0x00ff) + 2;
            }
#endif /* CONF_WITH_ATARI_VIDEO */
        }
    }
}


/*
 *  Part 2 of early emudesk.inf processing
 *
 *  This has two functions:
 *      1. Determine the auto-run program to be started (from #Z).
 *      2. Set the double-click speed (from #E).  This is done here
 *         in case we have an auto-run program.
 */
static void process_inf2(void)
{
    WORD    env;
    char    *pcurr;
    BYTE    tmp;

    pcurr = infbuf;
    while (*pcurr)
    {
        if ( *pcurr++ != '#' )
            continue;
        tmp = *pcurr;
        if (tmp == 'E')             /* #E 3A 11                     */
        {                           /* desktop environment          */
            pcurr += 2;
            scan_2(pcurr, &env);
            ev_dclick(env & 0x07, TRUE);
        }
        else if (tmp == 'Z')        /* something like "#Z 01 C:\THING.APP@" */
        {
            BYTE *tmpptr1, *tmpptr2;
            pcurr += 5;
            tmpptr1 = pcurr;
            while (*pcurr && (*pcurr != '@'))
                ++pcurr;
            *pcurr = 0;
            tmpptr2 = sh_name(tmpptr1);
            *(tmpptr2-1) = 0;
            KDEBUG(("Found #Z entry in EMUDESK.INF: path=%s, prg=%s\n",tmpptr1,tmpptr2));
            sh_wdef(tmpptr2, tmpptr1);
            ++pcurr;
        }
    }
}


/*
 *  Give everyone a chance to run, at least once
 */
void all_run(void)
{
    WORD  i;

    /* let all the acc's run*/
    for (i = 0; i < num_accs; i++)
    {
        dsptch();
    }
    /* then get in the wait line */
    wm_update(TRUE);
    wm_update(FALSE);
}


/*
 *  This function is called from accdesk_start (in gemstart.S) which
 *  is itself called from gem_main() below.
 *
 *  It runs under a separate process which terminates on shutdown or
 *  resolution change (see accdesk_start).  This ensures that all
 *  memory allocated to or by desk accessories is automatically freed
 *  on resolution change.
 */
void run_accs_and_desktop(void)
{
    WORD i;

    /* load gem resource and fix it up before we go */
    gem_rsc_init();

    /* get mice forms */
    ad_armice = (MFORM *)rs_bitblk[MICE00].bi_pdata;
    ad_hgmice = (MFORM *)rs_bitblk[MICE02].bi_pdata;

    /* init button stuff */
    gl_btrue = 0x0;
    gl_bdesired = 0x0;
    gl_bdely = 0x0;
    gl_bclick = 0x0;

    gl_logdrv = dos_gdrv() + 'A';   /* boot directory       */
    gsx_init();                     /* do gsx open work station */

    load_accs(num_accs);            /* load up to 'num_accs' desk accessories */

    /* fix up icons */
    for (i = 0; i < 3; i++) {
        memcpy(&bi, &rs_bitblk[NOTEBB+i], sizeof(BITBLK));
        gsx_trans(bi.bi_pdata, bi.bi_wb, bi.bi_pdata, bi.bi_wb, bi.bi_hl);
    }

    /* take the critical err handler int. */
    disable_interrupts();
    takeerr();
    enable_interrupts();

    sh_tographic();                 /* go into graphic mode */

    /* take the tick interrupt */
    disable_interrupts();
    gl_ticktime = gsx_tick(tikaddr, &tiksav);
    enable_interrupts();

    /* set initial click rate: must do this after setting gl_ticktime */
    ev_dclick(3, TRUE);

    /* fix up the GEM rsc file now that we have an open WS */
    gem_rsc_fixit();

    wm_start();                     /* initialise window vars */
    fs_start();                     /* startup gem libs */
    sh_curdir(D.s_cdir);            /* remember current desktop directory */
    process_inf2();                 /* process emudesk.inf part 2 */

    dsptch();                       /* off we go !!! */
    all_run();                      /* let them run  */

    sh_init();                      /* init for shell loop */
    sh_main();                      /* main shell loop */

    /* give back the tick   */
    disable_interrupts();
    gl_ticktime = gsx_tick(tiksav, &tiksav);
    enable_interrupts();

    /* close workstation    */
    gsx_wsclose();
}

void gem_main(void)
{
    WORD    i;

    sh_rdinf();                 /* get start of emudesk.inf */
    if (!gl_changerez)          /* can't be here because of rez change,       */
        process_inf1();         /*  so see if .inf says we need to change rez */

    if (gl_changerez) {
        switch(gl_changerez) {
#if CONF_WITH_ATARI_VIDEO
        case 1:                     /* ST(e) or TT display */
            Setscreen(-1L,-1L,gl_nextrez-2,0);
            initialise_palette_registers(gl_nextrez-2,0);
            break;
#endif
#if CONF_WITH_VIDEL
        case 2:                     /* Falcon display */
            Setscreen(-1L, -1L, FALCON_REZ, gl_nextrez);
            initialise_palette_registers(FALCON_REZ,gl_nextrez);
            break;
#endif
        }
        gsx_wsclear();              /* avoid artefacts that may show briefly */
        /*
         * resolution change always resets the default drive to the
         * boot device.  TOS3 issues a Dsetdrv() when this happens,
         * which Hatari's GEMDOS drive emulation uses to keep track
         * of the current drive.  we do the same.
         */
        dos_sdrv(bootdev);
    }

    ml_ocnt = 0;

    gl_changerez = FALSE;

    mn_init();                      /* initialise variables for menu_register() */

    num_accs = count_accs();        /* puts ACC names in acc_name[] */

    D.g_acc = NULL;
    if (num_accs)
        D.g_acc = dos_alloc(num_accs*sizeof(AESPROCESS));
    if (D.g_acc)
        memset(D.g_acc,0x00,num_accs*sizeof(AESPROCESS));
    else num_accs = 0;

    totpds = num_accs + 2;

    disable_interrupts();
    set_aestrap();                  /* set trap#2 -> aestrap */

    /* init event recorder  */
    gl_recd = FALSE;
    gl_rlen = 0;
    gl_rbuf = NULL;

    elinkoff = (BYTE *) &(D.g_int[0].a_evb[0].e_link) - (BYTE *) &(D.g_int[0].a_evb[0]);

    /* link up all the evb's to the event unused list */
    eul = NULL;
    for (i = 0; i < 2; i++)
        ev_init(D.g_int[i].a_evb,EVBS_PER_PD);
    for (i = 0; i < num_accs; i++)
        ev_init(D.g_acc[i].a_evb,EVBS_PER_PD);

    /* initialize sync blocks */
    wind_spb.sy_tas = 0;
    wind_spb.sy_owner = NULL;
    wind_spb.sy_wait = 0;

    /*
     * init processes - TODO: should go in gempd or gemdisp.
     */

    /* initialize list and unused lists   */
    nrl = drl = NULL;
    dlr = zlr = NULL;
    fph = fpt = fpcnt = 0;

    /* init initial process */
    for(i=totpds-1; i>=0; i--)
    {
        rlr = pd_index(i);
        if (i < 2)
        {
            rlr->p_uda = &D.g_int[i].a_uda;
            rlr->p_cda = &D.g_int[i].a_cda;
        }
        else
        {
            rlr->p_uda = &D.g_acc[i-2].a_uda;
            rlr->p_cda = &D.g_acc[i-2].a_cda;
        }
        rlr->p_qaddr = rlr->p_queue;
        rlr->p_qindex = 0;
        memset(rlr->p_name, ' ', AP_NAMELEN);
        rlr->p_appdir[0] = '\0'; /* by default, no application directory */
        /* if not rlr then initialize his stack pointer */
        if (i != 0)
            rlr->p_uda->u_spsuper = &rlr->p_uda->u_supstk;
        rlr->p_pid = i;
        rlr->p_stat = 0;
    }
    curpid = 0;
    rlr->p_pid = curpid++;
    rlr->p_link = NULL;

    /* end of process init */

    /* restart the tick     */
    enable_interrupts();

    /*
     * screen manager process init. this process starts out owning the mouse
     * and the keyboard. it has a pid == 1
     */
    gl_mowner = ctl_pd = iprocess("SCRENMGR", ctlmgr);

    /*
     * run the accessories and the desktop until termination
     * (for shutdown or resolution change)
     */
    aes_run_rom_program(accdesk_start);

    /* restore previous trap#2 address */
    disable_interrupts();
    unset_aestrap();
    enable_interrupts();

    if (D.g_acc)
        dos_free((LONG)D.g_acc);
}
