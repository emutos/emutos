/*      GEMINIT.C       4/23/84 - 08/14/85      Lee Lorenzen            */
/*      GEMCLI.C        1/28/84 - 08/14/85      Lee Jay Lorenzen        */
/*      GEM 2.0         10/31/85                Lowell Webster          */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */
/*      fix command tail handling               10/19/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2022 The EmuTOS development team
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
#include "obdefs.h"
#include "struct.h"
#include "gemlib.h"
#include "gem_rsc.h"
#include "mforms.h"
#include "xbiosbind.h"
#include "has.h"
#include "../bdos/bdosstub.h"
#include "biosext.h"
#include "miscutil.h"

#include "aescfg.h"
#include "gemgsxif.h"
#include "gemdosif.h"
#include "gemctrl.h"
#include "gemshlib.h"
#include "gempd.h"
#include "gemrslib.h"
#include "gemdos.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemfslib.h"
#include "gemsclib.h"
#include "gemfmlib.h"
#include "gemasm.h"
#include "gemaplib.h"
#include "geminput.h"
#include "gemmnext.h"
#include "gemmnlib.h"
#include "gemoblib.h"
#include "geminit.h"
#include "optimize.h"
#include "aesdefs.h"
#include "aesext.h"
#include "aesstub.h"

#include "string.h"
#include "tosvars.h"
#include "shellutl.h"

/* Prototypes: */
void accdesk_start(void) NORETURN;  /* called only from gemstart.S */
LONG size_theglo(void);         /* called only from gemstart.S */
LONG init_p0_stkptr(void);      /* called only from gemstart.S */
void run_accs_and_desktop(void);/* called only from gemstart.S */
void gem_main(void);            /* called only from gemstart.S */

#define ROPEN 0

#define NUM_MOUSE_CURSORS   8

#if CONF_WITH_LOADABLE_CURSORS
#define CURSOR_RSC_SIZE     1024
#define CURSOR_WIDTH        2           /* in BITBLK */
#define CURSOR_HEIGHT       37
#endif

/*
 * for compatibility purposes, the following string is written to the
 * start of the shell buffer during AES initialisation.  its purpose is
 * to prevent crashes in certain control-panel-like desk accessories
 * (including CTRL.ACC & EMULATOR.ACC) that were released before XCONTROL.
 * these accessories use the area at the start of the shell buffer to save
 * configuration data, and expect that the data returned by shel_get()
 * will *always* contain a string starting with #a.
 *
 * in this section of the shell buffer, the #a line is used to store
 * serial port settings.  for complete compatibility, this string could
 * also contain #b, #c, and #d lines, although no accessories are known
 * to require them.  the #b line is for printer settings, the #c line is
 * for palette/mouse/keyboard settings, and the #d line is reserved.
 *
 * the maximum allowed length of this string is 128 bytes (excluding the
 * terminating NUL byte).
 */
#define CP_SHELL_INIT   "#a000000\r\n"  /* initialisation string */

#define WAIT_TIMEOUT 500                /* see wait_for_accs() */

typedef struct {                     /* used by count_accs()/ldaccs() */
    LONG addr;                          /* DA load address */
    char name[LEN_ZFNAME];              /* DA file name */
} ACC;

static ACC      acc[NUM_ACCS];
static char    *accpath;                /* used to read the ACCPATH env var, empty string if var is not present */

/* Some global variables: */

GLOBAL WORD     totpds;
GLOBAL WORD     num_accs;

GLOBAL char     *ad_envrn;              /* initialized in GEMSTART      */

GLOBAL MFORM    *mouse_cursor[NUM_MOUSE_CURSORS];

GLOBAL MFORM    gl_mouse;
#if CONF_WITH_GRAF_MOUSE_EXTENSION
GLOBAL MFORM    gl_prevmouse;           /* previous AES  mouse form */
#endif

GLOBAL AESPD    *rlr, *drl, *nrl;
GLOBAL EVB      *eul, *dlr, *zlr;

GLOBAL UBYTE    indisp;

GLOBAL WORD     fpt, fph, fpcnt;                /* forkq tail, head,    */
                                                /*   count              */
GLOBAL SPB      wind_spb;
GLOBAL WORD     curpid;

GLOBAL THEGLO   D;


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


/*
 *  return pointer to default mouse form
 */
MFORM *default_mform(void)
{
    return mouse_cursor[ARROW];
}


/*
 *  set mouse pointer to arrow shape
 */
void set_mouse_to_arrow(void)
{
    gsx_mfset(mouse_cursor[ARROW]);
}


/*
 *  set mouse pointer to hourglass shape
 */
void set_mouse_to_hourglass(void)
{
    gsx_mfset(mouse_cursor[HOURGLASS]);
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
static AESPD *iprocess(char *pname, PFVOID routine)
{
    ULONG ldaddr;

    KDEBUG(("iprocess(\"%s\")\n", (const char*)pname));

    /* figure out load addr */

    ldaddr = (ULONG) routine;

    /* create process to execute it */
    return pstart(routine, pname, ldaddr);
}


/*
 *  Routine to load program file pointed at by acc->name, then create a
 *  new process context for it.  The load address is stored in acc->addr.
 *
 *  This is used to load a desk accessory.
 */
static void load_one_acc(ACC *acc)
{
    WORD    handle;
    WORD    err_ret;
    LONG    ret;

    KDEBUG(("load_one_acc(\"%s\")\n", (const char *)acc->name));

    acc->addr = -1L;
    sprintf(D.s_cmd, "%s\\%s", accpath , acc->name);

    ret = dos_open(D.s_cmd, ROPEN);
    if (ret >= 0L)
    {
        handle = (WORD)ret;
        err_ret = pgmld(handle, D.s_cmd, (LONG **)&acc->addr);
        dos_close(handle);
        /* create process to execute it */
        if (err_ret != -1)
            pstart(gotopgm, acc->name, acc->addr);
    }
}


/*
 *  Count up to a maximum of NUM_ACCS desk accessories, saving
 *  their names in acc[].name
 */
static WORD count_accs(void)
{
    const char accpathvar[] = "ACCPATH=";
    WORD i, rc;

    /* if the user decided to skip loading accessories, then do so */
    if (bootflags & BOOTFLAG_SKIP_AUTO_ACC)
        return 0;

    shellutl_getenv(ad_envrn, accpathvar, &accpath);
    if (accpath == NULL)
	accpath = "";
    sprintf(D.g_work,"%s\\*.ACC", accpath);

    dos_sdta(&D.g_dta);

    for (i = 0; i < NUM_ACCS; i++)
    {
        rc = (i==0) ? dos_sfirst(D.g_work,FA_RO) : dos_snext();
        if (rc < 0)
            break;
        strlcpy(acc[i].name,D.g_dta.d_fname,LEN_ZFNAME);
    }

    return i;
}


/*
 *  Free memory occupied by the desk accessories specified by acc[]
 *
 *  Note that this is NOT required for correct functioning of EmuTOS,
 *  since when the 'run_accs_and_desktop' process terminates, all
 *  allocated memory is freed.  However, some DAs (I'm looking at you,
 *  Chameleon) require explicit freeing of their memory to trigger
 *  proper cleanup.
 */
static void free_accs(WORD n)
{
    WORD i;

    for (i = 0; i < n; i++)
        if (acc[i].addr >= 0L)
            dos_free((void *)acc[i].addr);
}


/*
 *  Load in the desk accessories specified by acc[]
 */
static void load_accs(WORD n)
{
    WORD i;

    for (i = 0; i < n; i++)
        load_one_acc(&acc[i]);
}

/*
 *  Part 1 of early emudesk.inf processing
 *
 *  The main function is to determine (from #E) if we need to change
 *  resolution.  If so, we set gl_changerez and gl_nextrez appropriately.
 */
static void process_inf_res_change(void)
{
    if (aes_configuration.flags & AES_CFG_PROVIDES_RESOLUTION)
    {
        WORD mode = check_moderez(aes_configuration.videomode);

        if (mode == 0)              /* no change required */
            return;
        else if (mode > 0)          /* need to set Falcon mode */
        {
            gl_changerez = TO_FALCON_RES;
            gl_nextrez = mode;
        }
        else                        /* set ST/TT rez */
        {
            gl_changerez = TO_ST_RES;
            gl_nextrez = (mode & 0x00ff) + 2;
        }
    }
}

/*
 *  Part 2 of early emudesk.inf processing
 *
 *  The main function is to determine the auto-run program to be
 *  started, from the #Z line.  We also set the double-click speed
 *  and the blitter/cache status (if applicable), from the #E line.
 *
 *  Returns:
 *      TRUE if initial program is a GEM program (normal)
 *      FALSE if initial program is character-mode (only if an autorun
 *      entry exists, and it is for a character-mode program).
 */
static BOOL process_inf2(BOOL *isauto)
{
    *isauto = FALSE;                /* assume no autorun program */

    if (aes_configuration.flags & AES_CFG_PROVIDES_DCLICK_SPEED)
        ev_dclick(aes_configuration.double_click_rate, TRUE);

#if MPS_BLITTER_ALWAYS_ON
    Blitmode(1);
#else
  #if CONF_WITH_BLITTER
    if (aes_configuration.flags & AES_CFG_PROVIDES_BLITTER && has_blitter)
        Blitmode(aes_configuration.blitter_on);
  #endif
#endif // MPS_BLITTER_ALWAYS_ON

#if CONF_WITH_CACHE_CONTROL
    if (aes_configuration.flags & AES_CFG_PROVIDES_CPUCACHE)
        set_cache(aes_configuration.cpu_cache_on);
#endif

    *isauto = !(bootflags & BOOTFLAG_SKIP_AUTO_ACC) && (aes_configuration.flags & AES_CFG_PROVIDES_AUTOSTART);   
    if (isauto)
    {
        char path[MAXPATHLEN];
        char *filename;

        strcpy(path, aes_configuration.autostart); /* autostart is actually the absolute file name (including path) */
        filename = sh_name(path);
        *(filename-1) = 0;
        KDEBUG(("Found #Z entry in EMUDESK.INF: path=%s, prg=%s\n", path, filename));

        /* Run autorun program */
        sh_wdef(filename, path);

        return aes_configuration.autostart_is_gem;
    }
    return TRUE;
}


#if CONF_WITH_LOADABLE_CURSORS
/*
 *  Copy mouse cursors from buffered RSC file
 */
static WORD load_mouse_cursors(char *buf)
{
    RSHDR *hdr = (RSHDR *)buf;
    BITBLK *bb;
    WORD i;

    /*
     * perform a little validation
     */
    if (hdr->rsh_nbb != NUM_MOUSE_CURSORS)
    {
        KDEBUG(("Wrong number of mouse cursors (%d)\n",hdr->rsh_nbb));
        return -1;
    }
    if (hdr->rsh_rssize > CURSOR_RSC_SIZE)
    {
        KDEBUG(("Mouse cursor file is too big (%d bytes)\n",hdr->rsh_rssize));
        return -1;
    }
    for (i = 0, bb = (BITBLK *)(buf+hdr->rsh_bitblk); i < NUM_MOUSE_CURSORS; i++, bb++)
    {
        if ((bb->bi_wb != CURSOR_WIDTH) || (bb->bi_hl != CURSOR_HEIGHT))
        {
            KDEBUG(("Invalid mouse cursor dimensions (%dx%d)\n",bb->bi_wb,bb->bi_hl));
            return -1;
        }
    }

    /*
     * load the pointers
     */
    for (i = 0, bb = (BITBLK *)(buf+hdr->rsh_bitblk); i < NUM_MOUSE_CURSORS; i++, bb++)
        mouse_cursor[i] = (MFORM *)(buf+(LONG)bb->bi_pdata);

    return 0;
}
#endif


/*
 *  Set up RAM array of pointers to mouse cursors
 */
static void setup_mouse_cursors(void)
{
    WORD i;
#if CONF_WITH_LOADABLE_CURSORS
    LONG rc;
    char *buf;
#endif

    for (i = 0; i < NUM_MOUSE_CURSORS; i++)
        mouse_cursor[i] = (MFORM *)mform_rs_data[i];

#if CONF_WITH_GRAF_MOUSE_EXTENSION
    /* init mouse form so that first gsx_mfset() will populate gl_prevmouse */
    gl_mouse = *(mouse_cursor[HOURGLASS]);
#endif

#if CONF_WITH_LOADABLE_CURSORS
    /* Do not load user cursors if Control was held on startup */
    if (bootflags & BOOTFLAG_SKIP_AUTO_ACC)
        return;

    /*
     * update pointers to point to user-supplied versions, if available
     */
    buf = dos_alloc_anyram(CURSOR_RSC_SIZE);
    if (!buf)
        return;

    rc = sh_readfile(CURSOR_RSC_NAME, CURSOR_RSC_SIZE, buf);
    if (rc >= 0)
        rc = load_mouse_cursors(buf);

    if (rc < 0)     /* load failed */
        dos_free(buf);
#endif
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
    wm_update(BEG_UPDATE);
    wm_update(END_UPDATE);
}


/*
 * this waits until all the desk accessories have a bit set in the AES
 * p_flags field matching the bitmask argument.
 *
 * it is currently used with the following bitmask values:
 *  AP_MESAG
 *    . This bit indicates that the accessory has issued at least one
 *      wait for a message (either via evnt_mesag() or evnt_multi()).
 *      The accessory is not necessarily currently waiting, but there
 *      should always be a wait for a message, since accessories have
 *      to handle AC_OPEN/AC_CLOSE messages.
 *    . It's assumed that the accessory will have completed initialisation
 *      before issuing the wait, but if it hasn't this code will not
 *      make things worse.
 *    . This instance is called after the accessories have been loaded,
 *      before starting the desktop (or an autorun program, if present).
 *      This will cause any memory allocated during initialisation (by
 *      e.g. v_opnvwk()) to be owned by the AES, and thus the memory
 *      will not be freed by termination of the desktop/autorun program.
 *  AP_ACCLOSE
 *    . This bit indicates that the accessory has received an AC_CLOSE
 *      (either via evnt_mesag() or evnt_multi()).
 *    . This instance is called in appl_exit() after sending the AC_CLOSE
 *      messages to all DAs, to block until the messages have actually
 *      been received by the DAs.  This ensures that the DAs have a chance
 *      to close properly, before the application exits, freeing its memory
 *      which includes any memory allocated by the DAs during the life of
 *      the application.  This provides the same function that is described
 *      in the TT TOS Release Notes.
 */
void wait_for_accs(WORD bitmask)
{
    AESPD *pd;
    WORD n, pid;

    /*
     * as a precaution against infinite loops, we set a count timeout.
     * in limited testing, the maximum number of loops actually used
     * was less than 100.
     */
    for (n = 0; n < WAIT_TIMEOUT; n++)
    {
        /* let everybody run */
        all_run();

        /*
         * check the AESPDs for all the DAs (pid 0 & 1 are system)
         */
        for (pid = 2; ; pid++)
        {
            pd = fpdnm(NULL, pid);
            if (!pd)                    /* no more DAs: they must */
                return;                 /*  all have the bit set  */

            if (!(pd->p_flags&bitmask)) /* bit not set, so we  */
                break;                  /* must go round again */
        }
    }
    KDEBUG(("wait_for_accs(): %8.8s took too long\n",pd->p_name));
}


#if CONF_WITH_BACKGROUNDS
/*
 *  Set AES's root DESKTOP object's background pattern/colour
 */
void set_aes_background(UBYTE patcol)
{
    OBJECT *tree = rs_trees[DESKTOP];

    tree[ROOT].ob_spec &= 0xffffff00L;
    tree[ROOT].ob_spec |= patcol;
}
#endif


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
    BOOL isgem, isauto;
    BITBLK bi;
    OBJECT *tree;
    void *dummy;

    /* load gem resource and fix it up before we go */
    gem_rsc_init();
    setup_mouse_cursors();

    /* init button stuff */
    gl_btrue = 0x0;
    gl_bdesired = 0x0;
    gl_bdely = 0x0;
    gl_bclick = 0x0;

    strcpy(D.g_scrap, SCRAP_DIR_NAME);
    D.g_scrap[0] = dos_gdrv() + 'A';/* set up scrap dir path */

    gsx_init();                     /* do gsx open work station */

#if CONF_WITH_MENU_EXTENSION
    mnext_init();                   /* initialise menu library extension variables */
#endif

#if CONF_WITH_3D_OBJECTS
    init_3d();                      /* initialise 3D-related variables */
#endif

    sh_put(CP_SHELL_INIT,sizeof(CP_SHELL_INIT)-1);  /* see description at top */

    load_accs(num_accs);            /* load up to 'num_accs' desk accessories */

    /* fix up icons */
    for (i = 0; i < 3; i++) {
        bi = rs_bitblk[NOTEBB+i];
        gsx_trans(bi.bi_pdata, bi.bi_wb, bi.bi_hl);
    }

    /* take the critical err handler int. */
    disable_interrupts();
    takeerr();
    enable_interrupts();

    sh_tographic();                 /* go into graphic mode */

    /* take the tick interrupt */
    disable_interrupts();
    gl_ticktime = gsx_tick(&tikcod, &tiksav);
    enable_interrupts();

    /* set initial click rate: must do this after setting gl_ticktime */
    ev_dclick(3, TRUE);

    /* fix up the GEM rsc file now that we have an open WS */
    gem_rsc_fixit();

    /*
     * set height of root DESKTOP object to screen height
     */
    tree = rs_trees[DESKTOP];
    tree[ROOT].ob_height = gl_rscreen.g_h;

#if CONF_WITH_BACKGROUNDS
    /*
    *  If CONF_WITH_BACKGOUNDS is specified, we also get the desktop background
    *  colours (from #Q) & save them for use when initialising the desktop.
    *  This needs to happens before the desktop is loaded.
    */
    if (aes_configuration.flags & AES_CFG_PROVIDES_BACKGROUND)
    {
        WORD n = (gl_nplanes > 2) ? 2 : gl_nplanes-1;
        set_aes_background(aes_configuration.desktop_background[n] & 0xff);
    }
#endif

    wm_start();                     /* initialise window vars */
    fs_start();                     /* startup gem libs */
    build_root_path(D.s_cdir, 'A'+dos_gdrv());  /* root of current drive */
    isgem = process_inf2(&isauto);  /* process emudesk.inf part 2 */

    dsptch();                       /* off we go !!! */
    wait_for_accs(AP_MESAG);        /* wait until DAs have initialised */
    sh_main(isauto, isgem);         /* main shell loop */

    free_accs(num_accs);            /* free DA memory */

    /* give back the tick   */
    disable_interrupts();
    gl_ticktime = gsx_tick(tiksav, &dummy);
    enable_interrupts();

    /* close workstation    */
    gsx_wsclose();
}

#if CONF_WITH_ATARI_VIDEO || defined(MACHINE_AMIGA)
/*
 * change resolution & reinitialise the palette registers
 *
 * note: passing a screen address of 0L to Setscreen() means reallocate
 * screen memory.  on non-Falcon systems (where the size of screen memory
 * is fixed), this is effectively the same as passing an address of -1L,
 * and our implementation of Setscreen() handles this without problems.
 *
 * however, if someone has hooked Setscreen() and does not understand
 * this, passing a screen address of 0L will set the screen address to
 * NULL, and the system will crash.  this happens, for example, when
 * running NVDI 4.11 on a Mega ST.
 *
 * therefore, for safety, we only use 0L on explicitly Falcon resolutions.
 */
static void new_resolution(WORD rez, WORD videlmode)
{
    LONG addr = (rez==FALCON_REZ) ? 0L : -1L;

    Setscreen(addr, addr, rez, videlmode);      /* change resolution */
    initialise_palette_registers(rez, videlmode);
}
#endif

/*
 * Returns true if resolution was changed indeed.
 */
static BOOL handle_resolution_change(void)
{
    BOOL res_was_changed;

    /* If we're not processing a user-initiated resolution change,
     * find out the resolution to set from the config file. */
    if (gl_changerez == NO_RES_CHANGE)
        process_inf_res_change();
    if ((res_was_changed = (gl_changerez != NO_RES_CHANGE))) {
        switch(gl_changerez) {
#if CONF_WITH_ATARI_VIDEO
        case TO_ST_RES:         /* ST(e) or TT display */
            new_resolution(gl_nextrez-2, 0);
            break;
#endif
#if CONF_WITH_VIDEL || defined(MACHINE_AMIGA)
        case TO_FALCON_RES:          /* Falcon display */
            new_resolution(FALCON_REZ, gl_nextrez);            
            break;
#endif
        default:
            res_was_changed = FALSE;
            break;
        }
    }
    
    gl_changerez = NO_RES_CHANGE;
    return res_was_changed;
}

void gem_main(void)
{
    WORD    i;

    /*
     * turn off the text cursor now to prevent an irritating blinking
     * cursor on a blank screen during resolution change.  this is most
     * noticeable on a floppy-only system with no diskettes loaded.
     */
    dos_conws("\033f\033E");    /* cursor off, clear screen */

    /* Read config */
    aescfg_read();

    /* Handle any resolution change */
    if (handle_resolution_change()) {
        gsx_wsclear();  /* avoid artifacts that may show briefly */

        /*
         * resolution change always resets the default drive to the
         * boot device.  TOS3 issues a Dsetdrv() when this happens,
         * which Hatari's GEMDOS drive emulation uses to keep track
         * of the current drive.  we do the same.
         */
        dos_sdrv(bootdev);
    }

    /* initialise AES libraries */
    fm_init();
    mn_init();

    num_accs = count_accs();        /* puts ACC names in acc[].name */

    D.g_acc = NULL;
    if (num_accs)
        D.g_acc = dos_alloc_anyram(num_accs*sizeof(AESPROCESS));
    if (D.g_acc)
        bzero(D.g_acc,num_accs*sizeof(AESPROCESS));
    else num_accs = 0;

    totpds = num_accs + 2;

    disable_interrupts();
    set_aestrap();                  /* set trap#2 -> aestrap */

    /* init event recorder  */
    gl_recd = FALSE;
    gl_rlen = 0;
    gl_rbuf = NULL;

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
        rlr->p_msg.action = -1;
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
        dos_free(D.g_acc);
}
