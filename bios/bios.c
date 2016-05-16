/*
 *  bios.c - C portion of BIOS initialization and front end
 *
 * Copyright (c) 2001 Lineo, Inc.
 * Copyright (c) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  SCC     Steve C. Cavender
 *  KTB     Karl T. Braun (kral)
 *  JSL     Jason S. Loveman
 *  EWF     Eric W. Fleischman
 *  LTG     Louis T. Garavaglia
 *  MAD     Martin Doering
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*#define ENABLE_KDEBUG*/

#include "config.h"
#include "portab.h"
#include "biosext.h"
#include "bios.h"
#include "dos.h"
#include "pd.h"
#include "gemerror.h"
#include "kprint.h"
#include "tosvars.h"
#include "lineavars.h"
#include "vt52.h"
#include "processor.h"
#include "initinfo.h"
#include "machine.h"
#include "cookie.h"
#include "country.h"
#include "nls.h"
#include "biosmem.h"
#include "aespub.h"
#include "ikbd.h"
#include "mouse.h"
#include "midi.h"
#include "mfp.h"
#include "floppy.h"
#include "sound.h"
#include "dmasound.h"
#include "screen.h"
#include "clock.h"
#include "vectors.h"
#include "asm.h"
#include "chardev.h"
#include "blkdev.h"
#include "parport.h"
#include "serport.h"
#include "string.h"
#include "natfeat.h"
#include "delay.h"
#include "biosbind.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif
#ifdef MACHINE_FIREBEE
#include "coldfire.h"
#endif



/*==== Defines ============================================================*/

#define DBGBIOS 0               /* If you want to enable debug wrappers */
#define ENABLE_RESET_RESIDENT 0 /* enable to run "reset-resident" code (see below) */

/*==== External declarations ==============================================*/

#if STONX_NATIVE_PRINT
extern void stonx_kprintf_init(void);
#endif

#if CONF_WITH_CARTRIDGE
extern void run_cartridge_applications(WORD typebit); /* found in startup.S */
#endif

#if WITH_CLI
extern void coma_start(void) NORETURN;  /* found in cli/cmdasm.S */
#endif

#if CONF_WITH_ALT_RAM
extern long xmaddalt(UBYTE *start, long size); /* found in bdos/mem.h */
#endif

/*==== Declarations =======================================================*/

/* Drive specific declarations */
static WORD defdrv;             /* default drive number (0 is a:, 2 is c:) */

/* BYTE env[256];                * environment string, enough bytes??? */
static const BYTE null_env[] = {0, 0};

/* used by kprintf() */
WORD boot_status;               /* see kprint.h for bit flags */
/*==== BOOT ===============================================================*/


/*
 * setup all vectors
 */

static void vecs_init(void)
{
    /* Initialize the exception vectors.
     * By default, any unexpected exception calls dopanic().
     */
    init_exc_vec();
    init_user_vec();

    /* Some user drivers may install interrupt handlers and call the previous
     * ones. For example, ARAnyM's network driver for MiNT (nfeth.xif) and fVDI
     * driver (aranym.sys) install custom handlers on INT3, and call the
     * previous one. This panics with "Exception number 27" if VEC_LEVEL3 is
     * not initialized with a valid default handler.
     */
    VEC_LEVEL1 = just_rte;
    VEC_LEVEL2 = just_rte;
    VEC_LEVEL3 = just_rte;
    VEC_LEVEL4 = just_rte;
    VEC_LEVEL5 = just_rte;
    VEC_LEVEL6 = just_rte;
    VEC_LEVEL7 = just_rte;

#ifdef __mcoldfire__
    /* On ColdFire, when a zero divide exception occurs, the PC value in the
     * exception frame points to the offending instruction, not the next one.
     * If we put a simple rte in the exception handler, this will result in
     * an endless loop.
     * New ColdFire programs are supposed to be clean and avoid zero
     * divides. So we keep the default panic() behaviour in such case. */
#else
    /* Original TOS cowardly ignores integer divide by zero. */
    VEC_DIVNULL = just_rte;
#endif

    /* initialise some vectors we really need */
    VEC_AES = gemtrap;
    VEC_BIOS = biostrap;
    VEC_XBIOS = xbiostrap;
    VEC_LINEA = int_linea;

    /* Emulate some instructions unsupported by the processor. */
#ifdef __mcoldfire__
    /* On ColdFire, all the unsupported assembler instructions
     * will be emulated by a specific emulation layer loaded later. */
#else
    if (longframe) {
        /* On 68010+, "move from sr" called from user mode causes a
         * privilege violation. This instruction must be emulated for
         * compatibility with 68000 processors. */
        VEC_PRIVLGE = int_priv;
    } else {
        /* On 68000, "move from ccr" is unsupported and causes an illegal
         * instruction exception. This instruction must be emulated for
         * compatibility with higher processors. */
        VEC_ILLEGAL = int_illegal;
    }
#endif
}


/*
 * Initialize the BIOS
 */

static void bios_init(void)
{
    KDEBUG(("bios_init()\n"));

    /* initialize Native Features, if available
     * do it as soon as possible so that kprintf can make use of them
     */
#if DETECT_NATIVE_FEATURES
    KDEBUG(("natfeat_init()\n"));
    natfeat_init();
#endif
#if STONX_NATIVE_PRINT
    KDEBUG(("stonx_kprintf_init()\n"));
    stonx_kprintf_init();
#endif
#if CONF_WITH_UAE
    KDEBUG(("amiga_uaelib_init()\n"));
    amiga_uaelib_init();
#endif

    /* Initialize the processor */
    KDEBUG(("processor_init()\n"));
    processor_init();   /* Set CPU type, longframe and FPU type */
    KDEBUG(("vecs_init()\n"));
    vecs_init();        /* setup all exception vectors (above) */
    KDEBUG(("init_delay()\n"));
    init_delay();       /* set 'reasonable' default values for delay */

    /* Detect optional hardware (video, sound, etc.) */
    KDEBUG(("machine_detect()\n"));
    machine_detect();   /* detect hardware */
    KDEBUG(("machine_init()\n"));
    machine_init();     /* initialise machine-specific stuff */

    /* Initialize the screen */
    KDEBUG(("screen_init()\n"));
    screen_init();      /* detect monitor type, ... */

    /* Initialize the BIOS memory management */
    KDEBUG(("bmem_init()\n"));
    bmem_init();        /* this must be done after screen_init() */

    KDEBUG(("cookie_init()\n"));
    cookie_init();      /* sets a cookie jar */
    KDEBUG(("fill_cookie_jar()\n"));
    fill_cookie_jar();  /* detect hardware features and fill the cookie jar */

    /* Set up the BIOS console output */
    KDEBUG(("linea_init()\n"));
    linea_init();       /* initialize screen related line-a variables */
    font_init();        /* initialize font ring (requires cookie_akp) */
    font_set_default(-1);/* set default font */
    vt52_init();        /* initialize the vt52 console */

    /* Now kcprintf() will also send debug info to the screen */
    KDEBUG(("after vt52_init()\n"));

    /* misc. variables */
    dumpflg = -1;
    sysbase = (LONG) os_entry;
    savptr = (LONG) trap_save_area;
    etv_timer = (void(*)(int)) just_rts;
    etv_critic = default_etv_critic;
    etv_term = just_rts;

    /* setup VBL queue */
    nvbls = 8;
    vblqueue = vbl_list;
    {
        int i;
        for(i = 0 ; i < 8 ; i++) {
            vbl_list[i] = 0;
        }
    }

#if CONF_WITH_MFP
    KDEBUG(("mfp_init()\n"));
    mfp_init();
#endif

#if CONF_WITH_TT_MFP
    if (has_tt_mfp)
    {
        KDEBUG(("tt_mfp_init()\n"));
        tt_mfp_init();
    }
#endif

    /* Initialize the system 200 Hz timer */
    KDEBUG(("init_system_timer()\n"));
    init_system_timer();

    /* Initialize the RS-232 port(s) */
    KDEBUG(("chardev_init()\n"));
    chardev_init();     /* Initialize low-memory bios vectors */
    boot_status |= CHARDEV_AVAILABLE;   /* track progress */
    KDEBUG(("init_serport()\n"));
    init_serport();
    boot_status |= RS232_AVAILABLE;     /* track progress */
#if CONF_WITH_SCC
    if (has_scc)
        boot_status |= SCC_AVAILABLE;   /* track progress */
#endif

    /* The sound init must be done before allowing MFC interrupts,
     * because of dosound stuff in the timer C interrupt routine.
     */
#if CONF_WITH_DMASOUND
    KDEBUG(("dmasound_init()\n"));
    dmasound_init();
#endif
    KDEBUG(("snd_init()\n"));
    snd_init();         /* Reset Soundchip, deselect floppies */

    /* Init the two ACIA devices (MIDI and KBD). The three actions below can
     * be done in any order provided they happen before allowing MFP
     * interrupts.
     */
    KDEBUG(("kbd_init()\n"));
    kbd_init();         /* init keyboard, disable mouse and joystick */
    KDEBUG(("midi_init()\n"));
    midi_init();        /* init MIDI acia so that kbd acia irq works */
    KDEBUG(("init_acia_vecs()\n"));
    init_acia_vecs();   /* Init the ACIA interrupt vector and related stuff */
    KDEBUG(("after init_acia_vecs()\n"));
    boot_status |= MIDI_AVAILABLE;  /* track progress */

    /* Now we can enable the interrupts.
     * We need a timer for DMA timeouts in floppy and harddisk initialisation.
     * The VBL processing will be enabled later with the vblsem semaphore.
     */
#if CONF_WITH_ATARI_VIDEO
    /* Keep the HBL disabled */
    set_sr(0x2300);
#else
    set_sr(0x2000);
#endif

    KDEBUG(("calibrate_delay()\n"));
    calibrate_delay();  /* determine values for delay() function */
                        /*  - requires interrupts to be enabled  */
    KDEBUG(("blkdev_init()\n"));
    blkdev_init();      /* floppy and harddisk initialisation */
    KDEBUG(("after blkdev_init()\n"));

    /* initialize BIOS components */

    KDEBUG(("parport_init()\n"));
    parport_init();     /* parallel port */
    //mouse_init();     /* init mouse driver */
    KDEBUG(("clock_init()\n"));
    clock_init();       /* init clock */
    KDEBUG(("after clock_init()\n"));

#if CONF_WITH_NLS
    KDEBUG(("nls_init()\n"));
    nls_init();         /* init native language support */
    nls_set_lang(get_lang_name());
#endif

    /* set start of user interface */
#if WITH_AES
    exec_os = ui_start;
#elif WITH_CLI
    exec_os = coma_start;
#else
    exec_os = NULL;
#endif

    KDEBUG(("osinit()\n"));
    osinit();                   /* initialize BDOS */
    KDEBUG(("after osinit()\n"));
    boot_status |= DOS_AVAILABLE;   /* track progress */

    /* Enable VBL processing */
    vblsem = 1;

#if CONF_WITH_CARTRIDGE
    {
        WORD save_hz = v_hz_rez, save_vt = v_vt_rez, save_pl = v_planes;

        /* Run all boot applications from the application cartridge.
         * Beware: Hatari features a special cartridge which is used
         * for GEMDOS drive emulation. It will hack drvbits and hook Pexec().
         * It will also hack Line A variables to enable extended VDI video modes.
         */
        KDEBUG(("run_cartridge_applications(3)\n"));
        run_cartridge_applications(3); /* Type "Execute prior to bootdisk" */
        KDEBUG(("after run_cartridge_applications()\n"));

        if ((v_hz_rez != save_hz) || (v_vt_rez != save_vt) || (v_planes != save_pl))
        {
            set_rez_hacked();
            font_set_default(-1);   /* set default font */
            vt52_init();            /* initialize the vt52 console */
        }
    }
#endif

#if CONF_WITH_ALT_RAM

#if CONF_WITH_FASTRAM
    /* add TT-RAM that was detected in memory.S */
    if (ramtop != NULL)
    {
        KDEBUG(("xmaddalt()\n"));
        xmaddalt(FASTRAM_START, ramtop - FASTRAM_START);
    }
#endif

#ifdef MACHINE_AMIGA
    KDEBUG(("amiga_add_alt_ram()\n"));
    amiga_add_alt_ram();
#endif

#endif /* CONF_WITH_ALT_RAM */

    KDEBUG(("bios_init() end\n"));
}


static void bootstrap(void)
{
#if DETECT_NATIVE_FEATURES
    /* start the kernel provided by the emulator */
    PD *pd;
    LONG length;
    LONG r;
    char args[128];

    args[0] = '\0';
    nf_getbootstrap_args(args, sizeof(args));

    /* allocate space */
    pd = (PD *) trap1_pexec(PE_BASEPAGE, "mint.prg", args, null_env);

    /* get the TOS executable from the emulator */
    length = nf_bootstrap( (char*)pd->p_lowtpa + sizeof(PD), (long)pd->p_hitpa - pd->p_lowtpa);

    /* free the allocated space if something is wrong */
    if ( length <= 0 )
        goto err;

    /* relocate the loaded executable */
    r = trap1_pexec(PE_RELOCATE, (char*)length, pd, "");
    if ( r != (LONG)pd )
        goto err;

    /* set the boot drive for the new OS to use */
    bootdev = nf_getbootdrive();

    /* execute the relocated process */
    trap1_pexec(PE_GO, "", pd, "");

err:
    trap1(0x49, (long)pd->p_env); /* Mfree() the environment */
    trap1(0x49, (long)pd); /* Mfree() the process area */
#endif
}


#if ENABLE_RESET_RESIDENT
/*
 * run_reset_resident - run "reset-resident" code
 *
 * "Reset-resident" code is code that has been loaded into RAM prior
 * to a warm boot.  It has a special header with a magic number, it
 * is 512 bytes long (aligned on a 512-byte boundary), and it has a
 * specific checksum (calculated on a word basis).
 *
 * Note: this is an undocumented feature of TOS that exists in all
 * versions of Atari TOS.
 */
struct rrcode {
    long magic;
    struct rrcode *pointer;
    char program[502];
    short chksumfix;
};
#define RR_MAGIC    0x12123456L
#define RR_CHKSUM   0x5678

static void run_reset_resident(void)
{
    const struct rrcode *p = (const struct rrcode *)phystop;

    for (--p; p > (struct rrcode *)&etv_timer; p--)
    {
        if (p->magic != RR_MAGIC)
            continue;
        if (p->pointer != p)
            continue;
        if (compute_cksum((const UWORD *)p) != RR_CHKSUM)
            continue;
        regsafe_call(p->program);
    }
}
#endif

/*
 * autoexec - run programs in auto folder
 *
 * Skip this if user holds the Control key down.
 *
 * Note that GEMDOS already created a default basepage so it is safe
 * to use GEMDOS calls here!
 */

static void run_auto_program(const char* filename)
{
    char path[30];

    strcpy(path, "\\AUTO\\");
    strcat(path, filename);

    KDEBUG(("Loading %s ...\n", path));
    trap1_pexec(PE_LOADGO, path, "", null_env);   /* Pexec */
    KDEBUG(("[OK]\n"));
}

static void autoexec(void)
{
    struct {
        BYTE reserved[21];
        BYTE attr;
        WORD time;
        WORD date;
        LONG size;
        BYTE name[14];
    } dta;
    WORD err;

    if (kbshift(-1) & MODE_CTRL)        /* check if Control is held down */
        return;

    bootstrap();                        /* try to boot the new OS kernel directly */

    if( ! blkdev_avail(bootdev) )       /* check, if bootdev available */
        return;

    trap1( 0x1a, &dta);                      /* Setdta */
    err = trap1( 0x4e, "\\AUTO\\*.PRG", 7);  /* Fsfirst */
    while(err == 0) {
#ifdef TARGET_PRG
        if (!strncmp(dta.name, "EMUTOS", 6))
        {
            KDEBUG(("Skipping %s from AUTO folder\n", dta.name));
        }
        else
#endif
        {
            run_auto_program(dta.name);

            /* Setdta. BetaDOS corrupted the AUTO load if the Setdta
             * not repeated here */
            trap1( 0x1a, &dta);
        }

        err = trap1( 0x4f );                 /* Fsnext */
    }
}

#if CONF_WITH_SHUTDOWN

/* Try to shutdown the machine. This may fail. */
static void shutdown(void)
{
#if DETECT_NATIVE_FEATURES
    nf_shutdown();
#endif

#ifdef MACHINE_FIREBEE
    firebee_shutdown();
#elif defined(MACHINE_AMIGA)
    amiga_shutdown();
#endif
}

/* Will shutdown() succeed ? */
BOOL can_shutdown(void)
{
#if DETECT_NATIVE_FEATURES
    if (has_nf_shutdown())
        return TRUE;
#endif

#if defined(MACHINE_FIREBEE) || defined (MACHINE_AMIGA)
    return TRUE;
#else
    return FALSE;
#endif
}

#endif /* CONF_WITH_SHUTDOWN */

/*
 * biosmain - c part of the bios init code
 *
 * Print some status messages
 * exec the user interface (shell or aes)
 */

void biosmain(void)
{
    bios_init();                /* Initialize the BIOS */

    trap1( 0x30 );              /* initial test, if BDOS works: Sversion() */

    if (!HAS_RTC)
        trap1( 0x2b, os_dosdate);  /* set initial date in GEMDOS format: Tsetdate() */

    /* Steem needs this to initialize its GEMDOS hard disk emulation.
     * This may change drvbits. See Steem sources:
     * File steem/code/emulator.cpp, function intercept_bios(). */
    Drvmap();

    /*
     * if it's not the first boot, we use the existing bootdev.
     * this allows a boot device that was selected via the welcome
     * screen to persist across warm boots.
     */
    if (first_boot)
        bootdev = blkdev_avail(DEFAULT_BOOTDEV) ? DEFAULT_BOOTDEV : FLOPPY_BOOTDEV;

#if INITINFO_DURATION > 0
#if ALWAYS_SHOW_INITINFO
    /* No condition */ {
#else
    if (first_boot) {
#endif
        bootdev = initinfo();   /* show initial config information */
    }
#endif

    /* boot eventually from a block device (floppy or harddisk) */
    blkdev_boot();

    defdrv = bootdev;
    trap1( 0x0e , defdrv );    /* Set boot drive: Dsetdrv(defdrv) */

#if ENABLE_RESET_RESIDENT
    run_reset_resident();       /* see comments above */
#endif

#if WITH_CLI
    if (early_cli) {            /* run an early console */
        PD *pd = (PD *) trap1_pexec(PE_BASEPAGE, "", "", null_env);
        pd->p_tbase = (LONG) coma_start;
        pd->p_tlen = pd->p_dlen = pd->p_blen = 0;
        trap1_pexec(PE_GOTHENFREE, "", pd, "");
    }
#endif

    autoexec();                 /* autoexec PRGs from AUTO folder */

/*    env[0]='\0';               - clear environment string */

    /* clear commandline */

    if(cmdload != 0) {
        /* Pexec a program called COMMAND.PRG */
        trap1_pexec(PE_LOADGO, "COMMAND.PRG", "", null_env);
    } else if (exec_os) {
        /* start the default (ROM) shell */
        PD *pd;
        pd = (PD *) trap1_pexec(PE_BASEPAGE, "", "", null_env);
        pd->p_tbase = (LONG) exec_os;
        pd->p_tlen = pd->p_dlen = pd->p_blen = 0;
        trap1_pexec(PE_GO, "", pd, "");
    }

#if CONF_WITH_SHUTDOWN
        /* try to shutdown the machine / close the emulator */
        shutdown();
#endif

    /* hide cursor */
    cprintf("\033f");

    kcprintf(_("System halted!\n"));
    halt();
}



/**
 * bios_0 - (getmpb) Load Memory parameter block
 *
 * Returns values of the initial memory parameter block, which contains the
 * start address and the length of the TPA.
 * Just executed one time, before GEMDOS is loaded.
 *
 * Arguments:
 *   mpb - first memory descriptor, filled from BIOS
 *
 */

#if DBGBIOS
static void bios_0(MPB *mpb)
{
    getmpb(mpb);
}
#endif


/**
 * bios_1 - (bconstat) Status of input device
 *
 * Arguments:
 *   handle - device handle (0:PRT 1:AUX 2:CON)
 *
 *
 * Returns status in D0.L:
 *  -1  device is ready
 *   0  device is not ready
 */

LONG bconstat(WORD handle)        /* GEMBIOS character_input_status */
{
#if BCONMAP_AVAILABLE
    WORD map_index;
#endif

    if (!(boot_status & CHARDEV_AVAILABLE))
        return 0;

#if BCONMAP_AVAILABLE
    map_index = handle - BCONMAP_START_HANDLE;
    if (map_index >= bconmap_root.maptabsize)
        return 0L;
    if (map_index >= 0)
        return protect_v(bconmap_root.maptab[map_index].Bconstat);
#endif

    if ((handle >= 0) && (handle <= 7))
        return protect_v(bconstat_vec[handle]);
    return 0L;
}

#if DBGBIOS
static LONG bios_1(WORD handle)
{
    return bconstat(handle);
}
#endif

/**
 * bconin  - Get character from device
 *
 * Arguments:
 *   handle - device handle (0:PRT 1:AUX 2:CON)
 *
 * This function does not return until a character has been
 * input.  It returns the character value in D0.L, with the
 * high word set to zero.  For CON:, it returns the GSX 2.0
 * compatible scan code in the low byte of the high word, &
 * the ASCII character in the lower byte, or zero in the
 * lower byte if the character is non-ASCII.  For AUX:, it
 * returns the character in the low byte.
 */

LONG bconin(WORD handle)
{
#if BCONMAP_AVAILABLE
    WORD map_index;
#endif

    if (!(boot_status & CHARDEV_AVAILABLE))
        return 0;

#if BCONMAP_AVAILABLE
    map_index = handle - BCONMAP_START_HANDLE;
    if (map_index >= bconmap_root.maptabsize)
        return 0L;
    if (map_index >= 0)
        return protect_v(bconmap_root.maptab[map_index].Bconin);
#endif

    if ((handle >= 0) && (handle <= 7))
        return protect_v(bconin_vec[handle]);
    return 0L;
}

#if DBGBIOS
static LONG bios_2(WORD handle)
{
    return bconin(handle);
}
#endif

/**
 * bconout  - Print character to output device
 */

LONG bconout(WORD handle, WORD what)
{
#if BCONMAP_AVAILABLE
    WORD map_index;
#endif

    if (!(boot_status & CHARDEV_AVAILABLE))
        return 0;

#if BCONMAP_AVAILABLE
    map_index = handle - BCONMAP_START_HANDLE;
    if (map_index >= bconmap_root.maptabsize)
        return 0L;
    if (map_index >= 0)
        return protect_ww((PFLONG)bconmap_root.maptab[map_index].Bconout, handle, what);
#endif

    if ((handle >= 0) && (handle <= 7))
        return protect_ww((PFLONG)bconout_vec[handle], handle, what);
    return 0L;
}

#if DBGBIOS
static LONG bios_3(WORD handle, WORD what)
{
    return bconout(handle, what);
}
#endif

#if CONF_SERIAL_CONSOLE_ANSI
/* Output a string via bconout() */
void bconout_str(WORD handle, const char* str)
{
    while (*str)
        bconout(handle, (UBYTE)*str++);
}
#endif

/**
 * rwabs  - Read or write sectors
 *
 * Returns a 2's complement error number in D0.L.  It
 * is the responsibility of the driver to check for
 * media change before any write to FAT sectors.  If
 * media has changed, no write should take place, just
 * return with error code.
 *
 * r_w   = 0:Read 1:Write
 * *adr  = where to get/put the data
 * numb  = # of sectors to get/put
 * first = 1st sector # to get/put = 1st record # tran
 * drive = drive #: 0 = A:, 1 = B:, etc
 */

LONG lrwabs(WORD r_w, UBYTE *adr, WORD numb, WORD first, WORD drive, LONG lfirst)
{
    return protect_wlwwwl((PFLONG)hdv_rw, r_w, (LONG)adr, numb, first, drive, lfirst);
}

#if DBGBIOS
static LONG bios_4(WORD r_w, UBYTE *adr, WORD numb, WORD first, WORD drive, LONG lfirst)
{
    LONG ret;
    KDEBUG(("BIOS rwabs(rw = %d, addr = 0x%08lx, count = 0x%04x, "
            "sect = 0x%04x, dev = 0x%04x, lsect = 0x%08lx)",
            r_w, adr, numb, first, drive, lfirst));
    ret = lrwabs(r_w, adr, numb, first, drive, lfirst);
    KDEBUG((" = 0x%08lx\n", ret));
    return ret;
}
#endif



/**
 * Setexc - set exception vector
 *
 */

LONG setexc(WORD num, LONG vector)
{
    LONG oldvector;
    LONG *addr = (LONG *) (4L * num);
    oldvector = *addr;

    if(vector != -1) {
        *addr = vector;
    }
    return oldvector;
}

#if DBGBIOS
static LONG bios_5(WORD num, LONG vector)
{
    LONG ret = setexc(num, vector);
    KDEBUG(("Bios 5: Setexc(num = 0x%x, vector = 0x%08lx)\n", num, vector));
    return ret;
}
#endif


/**
 * tickcal - Time between two systemtimer calls
 */

LONG tickcal(void)
{
    return(20L);        /* system timer is 50 Hz so 20 ms is the period */
}

#if DBGBIOS
static LONG bios_6(void)
{
    return tickcal();
}
#endif


/**
 * get_bpb - Get BIOS parameter block
 * Returns a pointer to the BIOS Parameter Block for
 * the specified drive in D0.L.  If necessary, it
 * should read boot header information from the media
 * in the drive to determine BPB values.
 *
 * Arguments:
 *  drive - drive  (0 = A:, 1 = B:, etc)
 */

LONG getbpb(WORD drive)
{
    return protect_w(hdv_bpb, drive);
}

#if DBGBIOS
static LONG bios_7(WORD drive)
{
    return getbpb(drive);
}
#endif


/**
 * bcostat - Read status of output device
 *
 * Returns status in D0.L:
 * -1   device is ready
 * 0    device is not ready
 */

/* handle  = 0:PRT 1:AUX 2:CON 3:MID 4:KEYB */

LONG bcostat(WORD handle)       /* GEMBIOS character_output_status */
{
#if BCONMAP_AVAILABLE
    WORD map_index;
#endif

    if (!(boot_status & CHARDEV_AVAILABLE))
        return 0;

#if BCONMAP_AVAILABLE
    map_index = handle - BCONMAP_START_HANDLE;
    if (map_index >= bconmap_root.maptabsize)
        return 0L;
    if (map_index >= 0)
        return protect_v(bconmap_root.maptab[map_index].Bcostat);
#endif

    if ((handle >= 0) && (handle <= 7))
        return protect_v(bcostat_vec[handle]);
    return 0L;
}

#if DBGBIOS
static LONG bios_8(WORD handle)
{
    return bcostat(handle);
}
#endif



/**
 * bios_9 - (mediach) See if floppy has changed
 *
 * Returns media change status for specified drive in D0.L:
 *   0  Media definitely has not changed
 *   1  Media may have changed
 *   2  Media definitely has changed
 * where "changed" = "removed"
 */

LONG mediach(WORD drv)
{
    return protect_w(hdv_mediach, drv);
}

#if DBGBIOS
static LONG bios_9(WORD drv)
{
    return mediach(drv);
}
#endif

/**
 * bios_a - (drvmap) Read drive bitmap
 *
 * Returns a long containing a bit map of logical drives on the system,
 * with bit 0, the least significant bit, corresponding to drive A.
 * Note that if the BIOS supports logical drives A and B on a single
 * physical drive, it should return both bits set if a floppy drive is
 * present.
 */

LONG drvmap(void)
{
    return blkdev_drvmap();
}

#if DBGBIOS
static LONG bios_a(void)
{
    return drvmap();
}
#endif

/*
 *  bios_b - (kbshift)  Shift Key mode get/set.
 *
 *  two descriptions:
 *      o       If 'mode' is non-negative, sets the keyboard shift bits
 *              accordingly and returns the old shift bits.  If 'mode' is
 *              less than zero, returns the IBM0PC compatible state of the
 *              shift keys on the keyboard, as a bit vector in the low byte
 *              of D0
 *      o       The flag parameter is used to control the operation of
 *              this function.  If flag is not -1, it is copied into the
 *              state variable(s) for the shift, control and alt keys,
 *              and the previous key states are returned in D0.L.  If
 *              flag is -1, then only the inquiry is done.
 */

#if DBGBIOS
static LONG bios_b(WORD flag)
{
    return kbshift(flag);
}
#endif


/*
 * bios_unimpl
 *
 * ASM function _bios_unimpl will call bios_do_unimpl(WORD number);
 * with the function number passed as parameter.
 */

LONG bios_do_unimpl(WORD number)
{
    KINFO(("unimplemented BIOS function 0x%02x\n", number));
    return number;
}

extern LONG bios_unimpl(void);




/**
 * bios_vecs - the table of bios command vectors.
 */

/* PFLONG defined in bios/vectors.h */

#if DBGBIOS
#define VEC(wrapper, direct) (PFLONG) wrapper
#else
#define VEC(wrapper, direct) (PFLONG) direct
#endif

const PFLONG bios_vecs[] = {
    VEC(bios_0, getmpb),
    VEC(bios_1, bconstat),
    VEC(bios_2, bconin),
    VEC(bios_3, bconout),
    VEC(bios_4, lrwabs),
    VEC(bios_5, setexc),
    VEC(bios_6, tickcal),
    VEC(bios_7, getbpb),
    VEC(bios_8, bcostat),
    VEC(bios_9, mediach),
    VEC(bios_a, drvmap),
    VEC(bios_b, kbshift),
};

const UWORD bios_ent = ARRAY_SIZE(bios_vecs);
