/*
 *  bios.c - C portion of BIOS initialization and front end
 *
 * Copyright (C) 2001 Lineo, Inc.
 * Copyright (C) 2001-2023 The EmuTOS development team
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

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "biosext.h"
#include "bios.h"
#include "../bdos/bdosstub.h"
#include "../vdi/vdistub.h"
#include "bdosbind.h"
#include "gemerror.h"
#include "lineavars.h"
#include "vt52.h"
#include "processor.h"
#include "machine.h"
#include "has.h"
#include "cookie.h"
#include "country.h"
#include "nls.h"
#include "biosmem.h"
#include "ikbd.h"
#include "mouse.h"
#include "midi.h"
#include "mfp.h"
#include "floppy.h"
#include "sound.h"
#include "dmasound.h"
#include "dsp.h"
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
#include "memory.h"
#include "nova.h"
#include "tosvars.h"
#include "version.h"
#include "amiga.h"
#include "lisa.h"
#include "coldfire.h"

/*==== Defines ============================================================*/

#define DBGBIOS 0               /* If you want to enable debug wrappers */

/*==== External declarations ==============================================*/

#if STONX_NATIVE_PRINT
void stonx_kprintf_init(void);      /* defined in kprintasm.S */
#endif

#if CONF_WITH_CARTRIDGE
void run_cartridge_applications(WORD typebit);  /* defined in startup.S */
#endif

#if CONF_WITH_68040_PMMU
long setup_68040_pmmu(void);        /* defined in 68040_pmmu.S */
#endif

extern UBYTE osxhbootdelay;         /* defined in OSXH header in startup.S */

/*==== Declarations =======================================================*/

/* used by kprintf() */
WORD boot_status;               /* see kprint.h for bit flags */

static void display_startup_msg(void);

/* Non-Atari hardware vectors */
#if !CONF_WITH_MFP
void (*vector_5ms)(void);       /* 200 Hz system timer */
#endif

/*==== BOOT ===============================================================*/


/*
 * setup all vectors
 */

static void vecs_init(void)
{
#if !CONF_ATARI_HARDWARE
    /* On Atari hardware, the first 2 longs of the address space are physically
     * routed to the start of the ROM (instead of the start of the ST-RAM).
     * On other machines, there is actual RAM at that place.
     * To mimic Atari hardware behaviour, we copy the start of our OS there.
     * This may improve compatibility with some Atari software,
     * which may peek the OS version or reset address from there.
     * As C forbids from dereferencing the NULL pointer, we use assembly. */
    __asm__ volatile
    (
        "move.l  %0,a0\n\t"         /* a0 = ROM */
        "suba.l  a1,a1\n\t"         /* a1 = 0 */
        "move.l  (a0)+,(a1)+\n\t"   /* Reset: Initial SSP */
        "move.l  (a0),(a1)"         /* Reset: Initial PC */
    : /* outputs */
    : "g"(&os_header) /* inputs */
    : "a0", "a1", "cc" AND_MEMORY
    );
#endif

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
    VEC_GEM = vditrap;
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
#if CONF_WITH_ADVANCED_CPU
    /* On the 68060, instructions that were implemented in earlier
     * processors but not in the 68060 cause this trap to be taken,
     * for the purposes of emulation. The only instruction currently
     * emulated is movep; fortunately this is both the simplest and
     * commonest.
     */
    VEC_UNIMPINT = int_unimpint;
#endif
}

extern PFVOID vbl_list[8]; /* Default array for vblqueue */

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
    KDEBUG(("amiga_uae_init()\n"));
    amiga_uae_init();
#endif

    /* Initialize the processor */
    KDEBUG(("processor_init()\n"));
    processor_init();   /* Set CPU type, longframe and FPU type */

#if CONF_WITH_ADVANCED_CPU
    is_bus32 = (UBYTE)detect_32bit_address_bus();
#endif
    KDEBUG(("Address Bus width is %d-bit\n", IS_BUS32 ? 32 : 24));

    KDEBUG(("vecs_init()\n"));
    vecs_init();        /* setup all exception vectors (above) */
    KDEBUG(("init_delay()\n"));
    init_delay();       /* set 'reasonable' default values for delay */

    /* Detect optional hardware (video, sound, etc.) */
    KDEBUG(("machine_detect()\n"));
    machine_detect();   /* detect hardware */
    KDEBUG(("machine_init()\n"));
    machine_init();     /* initialise machine-specific stuff */

    /* Initialize the BIOS memory management */
    KDEBUG(("bmem_init()\n"));
    bmem_init();

#if defined(MACHINE_AMIGA)
    /* Detect and initialize Zorro II/III expansion boards.
     * This must be done after is_bus32 and bmem_init().
     * Alt-RAM found on those boards will be added later in altram_init().
     */
    amiga_autoconfig();
#endif

#if CONF_WITH_68040_PMMU
    /*
     * Initialize the 68040 MMU if required
     * Must be done after TT-RAM memory detection (which takes place
     * in machine_detect() above) and memory management initialization.
     */
    if ((mcpu == 40) && mmu_is_emulated())
    {
        if (setup_68040_pmmu() != 0)
            panic("setup_68040_pmmu() failed\n");
    }
#endif /* CONF_WITH_68040_PMMU */

    KDEBUG(("cookie_init()\n"));
    cookie_init();      /* sets a cookie jar */
    KDEBUG(("fill_cookie_jar()\n"));
    fill_cookie_jar();  /* detect hardware features and fill the cookie jar */
    KDEBUG(("font_init()\n"));
    font_init();        /* initialize font ring (requires cookie_akp) */

#if !MPS_STF && !MPS_STE
  #if CONF_WITH_BLITTER
    /*
     * If a PAK 68/3 is installed, the blitter cannot access the PAK ROMs.
     * So we must mark the blitter as not installed (this is what the
     * patched TOS3.06 ROM on the PAK does).  Since we can't detect a
     * PAK 68/3 directly, we look for a 68030 on an ST(e)/Mega ST(e).
     */
    if ((mcpu == 30)
     && ((cookie_mch == MCH_ST) || (cookie_mch == MCH_STE) || (cookie_mch == MCH_MSTE)))
        has_blitter = 0;
  #endif
#endif

    /*
     * Initialize MFP and SCC: among other things this ensures that the
     * respective interrupts are disabled.
     */

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

#if CONF_WITH_SCC
    if (has_scc)
    {
        KDEBUG(("scc_init()\n"));
        scc_init();
    }
#endif

    /*
     * Initialize the screen mode
     * Must be done before calling linea_init().
     */
    KDEBUG(("screen_init_mode()\n"));
    screen_init_mode(); /* detect monitor type, ... */

    /* Set up the BIOS console output */
    KDEBUG(("linea_init()\n"));
    linea_init();       /* initialize screen related line-a variables */

    /*
     * Initialize the screen address
     * Must be done after calling linea_init().
     */
    KDEBUG(("screen_init_address()\n"));
    screen_init_address();

    KDEBUG(("vt52_init()\n"));
    vt52_init();        /* initialize the vt52 console */

    /* Now kcprintf() will also send debug info to the screen */
    KDEBUG(("after vt52_init()\n"));

    /* now we have output, let the user know we're alive */
    display_startup_msg();

#if DETECT_NATIVE_FEATURES
    /*
     * Tell ARAnyM where the LineA variables are
     */
    nf_setlinea();
#endif

    /* misc. variables */
    dumpflg = -1;
    sysbase = &os_header;
    savptr = (LONG) trap_save_area;
    etv_timer = (ETV_TIMER_T) just_rts;
    etv_critic = default_etv_critic;
    etv_term = just_rts;
    swv_vec = just_rts;

    /* setup default VBL queue with vbl_list[] */
    nvbls = ARRAY_SIZE(vbl_list);
    vblqueue = vbl_list;
    {
        int i;
        for(i = 0 ; i < nvbls ; i++) {
            vbl_list[i] = NULL;
        }
    }

    /*
     * Initialize the system 200 Hz timer (timer C on Atari hardware).
     * Note that init_system_timer() no longer enables the 50 Hz processing
     * that is called from the system timer interrupt.  This allows us to
     * enable interrupts earlier (so that the interrupt-driven serial port
     * routines work), even though we haven't yet initialised the sound &
     * keyboard repeat stuff.
     */
    KDEBUG(("init_system_timer()\n"));
    init_system_timer();

    /*
     * Now we can enable interrupts.  Although VBL & timer interrupts will
     * occur:
     *  . VBL processing will not be enabled until later, when the vblsem
     *    semaphore is set
     *  . 50 Hz processing (sound playback & keyboard repeat) will not be
     *    enabled until later, when timer_c_sieve is set
     */
#if CONF_WITH_ATARI_VIDEO
    /* Keep the HBL disabled */
    set_sr(0x2300);
#else
    set_sr(0x2000);
#endif

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

    /*
     * Initialise sound processing
     */
#if CONF_WITH_DMASOUND
    KDEBUG(("dmasound_init()\n"));
    dmasound_init();
#endif
    KDEBUG(("snd_init()\n"));
    snd_init();         /* Reset Soundchip, deselect floppies */

    if (FIRST_BOOT)
        coldbootsound();
    else
        warmbootsound();

    /*
     * Initialise the two ACIA devices (MIDI and KBD), then initialise
     * the associated IORECs & vectors
     */
    KDEBUG(("kbd_init()\n"));
    kbd_init();         /* init keyboard, disable mouse and joystick */
    KDEBUG(("midi_init()\n"));
    midi_init();        /* init MIDI acia so that kbd acia irq works */
    KDEBUG(("init_acia_vecs()\n"));
    init_acia_vecs();   /* Init the ACIA interrupt vector and related stuff */
    KDEBUG(("after init_acia_vecs()\n"));
    boot_status |= MIDI_AVAILABLE;  /* track progress */

    /* Enable 50 Hz processing */
    timer_c_sieve = 0x1111;

    KDEBUG(("calibrate_delay()\n"));
    calibrate_delay();  /* determine values for delay() function */
                        /*  - requires interrupts to be enabled  */

    /* Initialize the DSP.  Since we currently use the system timer
     * in dsp_execboot(), which is called from dsp_init(), the latter
     * must be called *after* system timer interrupts are enabled.
     */
#if CONF_WITH_DSP
    KDEBUG(("dsp_init()\n"));
    dsp_init();
#endif

#if CONF_WITH_MEMORY_TEST
    /*
     * we only do this on cold boot
     */
    if (FIRST_BOOT)
    {
        BOOL ok;
        cprintf("\n%s:\n",_("Memory test"));
        ok = memory_test();         /* simple memory test, like Atari TOS */
        cprintf("%s %s\n",_("Memory test"),ok?_("complete"):_("aborted"));
    }
#endif

    /* User configurable boot delay to allow harddisks etc. to get ready */
    if (FIRST_BOOT && osxhbootdelay)
    {
        long end = hz_200 + (long)osxhbootdelay * CLOCKS_PER_SEC;
        while (hz_200 < end)
        {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
            stop_until_interrupt();
#endif
        }
    }

    KDEBUG(("blkdev_init()\n"));
    blkdev_init();      /* floppy and harddisk initialisation */
    KDEBUG(("after blkdev_init()\n"));

    /* initialize BIOS components */

    KDEBUG(("parport_init()\n"));
    parport_init();     /* parallel port */
    KDEBUG(("clock_init()\n"));
    clock_init();       /* init clock */
    KDEBUG(("after clock_init()\n"));

#if CONF_WITH_NOVA
    /* Detect and initialize a Nova card, skip if Ctrl is pressed */
    if (HAS_NOVA && !(kbshift(-1) & MODE_CTRL)) {
        KDEBUG(("init_nova()\n"));
        if (init_nova()) {
            set_rez_hacked();   /* also reinitializes the vt52 console */
        }
    }
#endif

#if CONF_WITH_NLS
    KDEBUG(("nls_init()\n"));
    nls_init();         /* init native language support */
    nls_set_lang(get_lang_name());
#endif

#if CONF_WITH_ALT_RAM
    /* Detect Alt-RAM */
    KDEBUG(("altram_init()\n"));
    altram_init();
#endif
    boot_status |= DOS_AVAILABLE;   /* track progress */

    /* Enable VBL processing */
    swv_vec = os_header.reseth; /* reset system on monitor change & jump to _main */
    vblsem = 1;

#if CONF_WITH_CARTRIDGE
    {
        WORD save_hz = V_REZ_HZ, save_vt = V_REZ_VT, save_pl = v_planes;

        /* Run all boot applications from the application cartridge.
         * Beware: Hatari features a special cartridge which is used
         * for GEMDOS drive emulation. It will hack drvbits and hook Pexec().
         * It will also hack Line A variables to enable extended VDI video modes.
         */
        KDEBUG(("run_cartridge_applications(3)\n"));
        run_cartridge_applications(3); /* Type "Execute prior to bootdisk" */
        KDEBUG(("after run_cartridge_applications()\n"));

        if ((V_REZ_HZ != save_hz) || (V_REZ_VT != save_vt) || (v_planes != save_pl))
        {
            set_rez_hacked();   /* also reinitializes the vt52 console */
        }
    }
#endif

    KDEBUG(("bios_init() end\n"));
}


#if DETECT_NATIVE_FEATURES

void natfeat_bootstrap(const char *env)
{
    /* start the kernel provided by the emulator */
    PD *pd;
    LONG length;
    LONG r;
    char args[128];

    args[0] = '\0';
    nf_getbootstrap_args(args, sizeof(args));

    /* allocate space */
    pd = (PD *) Pexec(PE_BASEPAGEFLAGS, (char*)PF_STANDARD, args, env);

    /* get the TOS executable from the emulator */
    length = nf_bootstrap(pd->p_lowtpa + sizeof(PD), pd->p_hitpa - pd->p_lowtpa);

    /* free the allocated space if something is wrong */
    if (length <= 0)
        goto err;

    /* relocate the loaded executable */
    r = Pexec(PE_RELOCATE, (char *)length, (char *)pd, env);
    if (r != (LONG)pd)
        goto err;

    /* set the boot drive for the new OS to use */
    bootdev = nf_getbootdrive();

    /* execute the relocated process */
    Pexec(PE_GO, "", (char *)pd, env);

err:
    Mfree(pd->p_env); /* Mfree() the environment */
    Mfree(pd); /* Mfree() the process area */
}

#endif /* DETECT_NATIVE_FEATURES */

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
#elif defined(MACHINE_LISA)
    lisa_shutdown();
#endif
}

/* Will shutdown() succeed ? */
BOOL can_shutdown(void)
{
#if DETECT_NATIVE_FEATURES
    if (has_nf_shutdown())
        return TRUE;
#endif

#ifdef MACHINE_FIREBEE
    return TRUE;
#elif defined(MACHINE_AMIGA)
    return amiga_can_shutdown();
#elif defined(MACHINE_LISA)
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
 * exec the user interface (shell or AES)
 */

void biosmain(void)
{
    bios_init();                /* Initialize the BIOS */

    /* Set start of user interface.
     * No need to check if os_header.os_magic->gm_magic == GEM_MUPB_MAGIC,
     * as this is always true. */
    exec_os = os_header.os_magic->gm_init;

    bdos_bootstrap();

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
    KDEBUG(("BIOS rwabs(rw = %d, addr = %p, count = 0x%04x, "
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
    KDEBUG(("Bios 5: Setexc(num = 0x%x, vector = %p)\n", num, (void*)vector));
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


/**
 * bios_vecs - the table of bios command vectors.
 */

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

#if CONF_WITH_EXTENDED_MOUSE

/* Does this pointer belong to our TEXT segment? */
BOOL is_text_pointer(const void *p)
{
    UBYTE *pb = (UBYTE *)p;
    return (pb >= (UBYTE *)&os_header) && (pb < _etext);
}

#endif /* CONF_WITH_EXTENDED_MOUSE */

static void display_startup_msg(void)
{
    cprintf("EmuTOS Version %s\r\n", version);
}
