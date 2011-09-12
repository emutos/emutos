/*
 *  bios.c - C portion of BIOS initialization and front end
 *
 * Copyright (c) 2001 Lineo, Inc. and
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


#include "config.h"
#include "portab.h"
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
#include "string.h"
#include "natfeat.h"



/*==== Defines ============================================================*/

#define DBGBIOS 0               /* If you want debugging output */
#define DBGAUTOBOOT 1           /* If you want to see AUTO folder loading */

/*==== Forward prototypes =================================================*/

void biosmain(void);
LONG bios_do_unimpl(WORD number);

/*==== External declarations ==============================================*/

extern LONG osinit(void);       /* found in bdosmain.c */
extern void linea_init(void);   /* found in lineainit.c */
extern void cartscan(WORD);     /* found in startup.S */

extern void ui_start(void);   /* found in cli/coma.S or aes/gemstart.S */
                              /* it is the start addr. of the user interface */
#if WITH_CLI
extern void coma_start(void);
#endif

extern long xmaddalt(long start, long size); /* found in bdos/mem.h */

/*==== Declarations =======================================================*/

/* Drive specific declarations */
static WORD defdrv;             /* default drive number (0 is a:, 2 is c:) */

/* BYTE env[256];                * environment string, enough bytes??? */
static const BYTE null_env[] = {0, 0};

/*==== BOOT ===============================================================*/


/*
 * setup all vectors
 */
 
static void vecs_init(void)
{
    /* setup default exception vectors */
    init_exc_vec();
    init_user_vec();

    /* initialise some vectors we really need */
    VEC_HBL = int_hbl;
    VEC_VBL = int_vbl;
    VEC_AES = gemtrap;
    VEC_BIOS = biostrap;
    VEC_XBIOS = xbiostrap;
    VEC_LINEA = int_linea;
    VEC_DIVNULL = just_rte;     /* just return for this */
    VEC_LEVEL1 = just_rte;     /* just return for this */
    VEC_LEVEL3 = just_rte;     /* just return for this */
    VEC_LEVEL5 = just_rte;     /* just return for this */
    VEC_LEVEL6 = just_rte;     /* just return for this */
    VEC_NMI = just_rte;     /* just return for this */

    /* Emulate some instructions unsupported by the processor. */
#ifdef __mcoldfire__
    /* On ColdFire, all the unsupported assembler instructions
     * will be emulated by a specific emulation layer loaded later. */
#else
    if (longframe) {
        /* On 68010+, "move from sr" called from the user mode cause a
         * privilege violation. This instruction must be emulated for
         * compatibility with the 68000 processors. */
        VEC_PRIVLGE = int_priv;
    } else {
        /* On 68000, "move from ccr" is unsupported and cause an illegal
         * exception. This instruction must be emulated for compatibility
         * with higher processors. */
        VEC_ILLEGAL = int_illegal;
    }
#endif
}


/*
 * Initialize the BIOS
 */

static void bios_init(void)
{
    /* initialize Native Features, if available 
     * do it as soon as possible so that kprintf can make use of them
     */
#if DETECT_NATIVE_FEATURES
    natfeat_init();
#endif

#if DBGBIOS
    kprintf("beginning of BIOS init\n");
#endif

    /* first detect available hardware (video, sound etc.) */
    machine_detect();

    /* First cut memory for screen, rest goes in memory descriptor */
    screen_init();      /* detect monitor type, ... */

    bmem_init();        /* initialize BIOS memory management */
    processor_init();   /* Set CPU type, VEC_ILLEGAL, longframe and FPU type */
    cookie_init();      /* sets a cookie jar */
    machine_init();     /* detect hardware features and fill the cookie jar */
    vecs_init();        /* setup all exception vectors (above) */

    /* misc. variables */
    dumpflg = -1;
    sysbase = (LONG) os_entry;

    savptr = (LONG) trap_save_area;

    /* some more variables */
    etv_timer = (void(*)(int)) just_rts;
    etv_critic = criter1;
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


    mfp_init();         /* init MFP, timers, USART */
    
    /* The sound init must be done before allowing MFC interrupts,
     * because of dosound stuff in the timer C interrupt routine.
     */
#if CONF_WITH_DMASOUND
    dmasound_init();
#endif
    snd_init();         /* Reset Soundchip, deselect floppies */

    /* Init the two ACIA devices (MIDI and KBD). The three actions below can 
     * be done in any order provided they happen before allowing MFP 
     * interrupts.
     */
    kbd_init();         /* init keyboard, disable mouse and joystick */
    midi_init();        /* init MIDI acia so that kbd acia irq works */
    init_acia_vecs();   /* Init the ACIA interrupt vector and related stuff */

    /* Now that the MFP is configured, allow MFP interrupts (we need a
     * Timer C for DMA timeouts in floppy and harddisk initialisation)
     */
    set_sr(0x2500);
  
    blkdev_init();      /* floppy and harddisk initialisation */

    /* these routines must be called in the given order */

    linea_init();       /* initialize screen related line-a variables */
    font_init();        /* initialize font ring */
    font_set_default(); /* set default font */
    vt52_init();        /* initialize the vt52 console */

    vblsem = 1;

    /* initialize BIOS components */

    chardev_init();     /* Initialize character devices */
    parport_init();     /* parallel port */
    //mouse_init();     /* init mouse driver */
    clock_init();       /* init clock */
    nls_init();         /* init native language support */
    nls_set_lang(get_lang_name());
    

    exec_os = ui_start;         /* set start of user interface */

    osinit();                   /* initialize BDOS */
  
    set_sr(0x2300);
  
#if DBGBIOS
    kprintf("BIOS: Last test point reached ...\n");
#endif

    cartscan(3);

    /* add TT-RAM that was detected in memory.S */
    if (ramtop > 0x1000000)
        xmaddalt( 0x1000000, ramtop - 0x1000000);
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
    pd = (PD *) trap1_pexec(5, "mint.prg", args, null_env);

    /* get the TOS executable from the emulator */
    length = nf_bootstrap( (char*)pd->p_lowtpa + sizeof(PD), (long)pd->p_hitpa - pd->p_lowtpa);

    /* free the allocated space if something is wrong */
    if ( length <= 0 )
        goto err;

    /* relocate the loaded executable */
    r = trap1_pexec(50, (char*)length, pd, "");
    if ( r != (LONG)pd )
        goto err;

    /* set the boot drive for the new OS to use */
    bootdev = nf_getbootdrive();

    /* execute the relocated process */
    trap1_pexec(4, "", pd, "");

err:
    trap1(0x49, (long)pd->p_env); /* Mfree() the environment */
    trap1(0x49, (long)pd); /* Mfree() the process area */
#endif
}


/*
 * autoexec - run programs in auto folder
 *
 * Skip this if user holds the Control key down.
 *
 * Note that the GEMDOS already created a default basepage so it is save
 * to use GEMDOS calls here!
 */

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
    BYTE path[30];
    WORD err;

    if (kbshift(-1) & 0x04)             /* check if Control is held down */
        return;

    bootstrap();                        /* try to boot the new OS kernel directly */

    if( ! blkdev_avail(bootdev) )       /* check, if bootdev available */
        return;

    trap1( 0x1a, &dta);                      /* Setdta */
    err = trap1( 0x4e, "\\AUTO\\*.PRG", 7);  /* Fsfirst */
    while(err == 0) {
        strcpy(path, "\\AUTO\\");
        dta.name[12] = 0;
        strcat(path, dta.name);

#if DBGAUTOBOOT
        kprintf("Loading %s ...\n", path);
#endif
        trap1_pexec(0, path, "", null_env);   /* Pexec */
#if DBGAUTOBOOT
        kprintf("[OK]\n");
#endif

        /* Setdta. BetaDOS corrupted the AUTO load if the Setdta
         * not here again */
        trap1( 0x1a, &dta);
        err = trap1( 0x4f );                 /* Fsnext */
    }
}



/*
 * biosmain - c part of the bios init code
 *
 * Print some status messages
 * exec the user interface (shell or aes)
 */

void biosmain(void)
{
    int coldboot; /* unfortunately register d2 gets overwritten by bios_init */
    BOOL rtc_present = FALSE; /* some hardware keeps the time when power is off */

    bios_init();                /* Initialize the BIOS */ 

    /* cold or warm boot? */
    coldboot = (memvalid!=0x752019f3 || memval2!=0x237698aa || memval3!=0x5555aaaa);
    if (coldboot) {
        /* make memory config valid */
        memvalid = 0x752019f3;
        memval2  = 0x237698aa;
        memval3  = 0x5555aaaa;
    }

    trap1( 0x30 );              /* initial test, if BDOS works: Sversion() */

#if CONF_WITH_NVRAM
    if (has_nvram)
        rtc_present = TRUE;
#endif

    if (has_megartc)
        rtc_present = TRUE;

    if (!rtc_present)
        trap1( 0x2b, os_dosdate);  /* set initial date in GEMDOS format: Tsetdate() */

#ifdef EMUTOS_RAM
    /* if TOS in RAM booted from an autoboot floppy, ask to remove the
     * floppy before going on.
     */
    if(os_magic == OS_MAGIC_EJECT) {
        cprintf(_("Please eject the floppy and hit RETURN"));
        bconin2();
    }
#endif

    initscreen();               /* clear the screen, etc. */
#if INITINFO_DURATION > 0
#if ALWAYS_SHOW_INITINFO
    /* No condition */ {
#else
    if (coldboot) {
#endif
        initinfo();             /* show initial config information */
    }
#endif
    
    /* boot eventually from a block device (floppy or harddisk) */
    blkdev_hdv_boot();

    defdrv = bootdev;
    trap1( 0x0e , defdrv );    /* Set boot drive: Dsetdrv(defdrv) */

    /* TODO: execute Reset-resistent PRGs ? */

#if WITH_CLI
    if (early_cli) {            /* run an early console */
        PD *pd = (PD *) trap1_pexec(5, "", "", null_env);
        pd->p_tbase = (LONG) coma_start;
        pd->p_tlen = pd->p_dlen = pd->p_blen = 0;
        trap1_pexec(4, "", pd, "");
    }
#endif
    
    autoexec();                 /* autoexec Prgs from AUTO folder */

/*    env[0]='\0';               - clear environment string */

    /* clear commandline */
    
    if(cmdload != 0) {
        /* Pexec a program called COMMAND.PRG */
        trap1_pexec(0, "COMMAND.PRG", "", null_env); 
    } else {
        /* start the default (ROM) shell */
        PD *pd;
        pd = (PD *) trap1_pexec(5, "", "", null_env);
        pd->p_tbase = (LONG) exec_os;
        pd->p_tlen = pd->p_dlen = pd->p_blen = 0;
        trap1_pexec(4, "", pd, "");
    }

    /* try to shutdown the machine if available */
#if DETECT_NATIVE_FEATURES
    nf_shutdown();
#endif

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
void bios_0(MPB *mpb)
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

static LONG bconstat(WORD handle)        /* GEMBIOS character_input_status */
{
    return protect_v(bconstat_vec[handle & 7]);
}

#if DBGBIOS
LONG bios_1(WORD handle)
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

static LONG bconin(WORD handle)
{
    return protect_v(bconin_vec[handle & 7]);
}

#if DBGBIOS
LONG bios_2(WORD handle)
{
    return bconin(handle);
}
#endif

/**
 * bconout  - Print character to output device
 */

static void bconout(WORD handle, WORD what)
{
    protect_ww((PFLONG)(bconout_vec[handle & 7]), handle, what);
}

#if DBGBIOS
void bios_3(WORD handle, WORD what)
{
    bconout(handle, what);
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

static LONG lrwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD drive, LONG lfirst)
{
    return protect_wlwwwl((PFLONG)hdv_rw, r_w, adr, numb, first, drive, lfirst);
}

#if DBGBIOS
LONG bios_4(WORD r_w, LONG adr, WORD numb, WORD first, WORD drive, LONG lfirst)
{
    LONG ret;
    kprintf("BIOS rwabs(rw = %d, addr = 0x%08lx, count = 0x%04x, "
            "sect = 0x%04x, dev = 0x%04x, lsect = 0x%08lx)",
            r_w, adr, numb, first, drive, lfirst);
    ret = lrwabs(r_w, adr, numb, first, drive, lfirst);
    kprintf(" = 0x%08lx\n", ret);
    return ret;
}
#endif



/**
 * Setexec - set exception vector
 *
 */

static LONG setexec(WORD num, LONG vector)
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
LONG bios_5(WORD num, LONG vector)
{
    LONG ret = setexec(num, vector);
    kprintf("Bios 5: Setexec(num = 0x%x, vector = 0x%08lx)\n", num, vector);
    return ret;
}
#endif


/**
 * tickcal - Time between two systemtimer calls
 */

static LONG tickcal(void)
{
    return(20L);        /* system timer is 50 Hz so 20 ms is the period */
}

#if DBGBIOS
LONG bios_6(void)
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

static LONG getbpb(WORD drive)
{
    return protect_w(hdv_bpb, drive);
}

#if DBGBIOS
LONG bios_7(WORD drive)
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

static LONG bcostat(WORD handle)        /* GEMBIOS character_output_status */
{
    if(handle>7)
        return 0;               /* Illegal handle */

    /* compensate for a known BIOS bug: MIDI and IKBD are switched */
    /*    if(handle==3)  handle=4; else if (handle==4)  handle=3;  */
    /* LVL: now done directly in the table */

    return protect_v(bcostat_vec[handle]);
}

#if DBGBIOS
LONG bios_8(WORD handle)
{
    return bcostat(handle);
}
#endif



/**
 * bios_9 - (mediach) See, if floppy has changed
 *
 * Returns media change status for specified drive in D0.L:
 *   0  Media definitely has not changed
 *   1  Media may have changed
 *   2  Media definitely has changed
 * where "changed" = "removed"
 */

static LONG mediach(WORD drv)
{
    return protect_w(hdv_mediach, drv);
}

#if DBGBIOS
LONG bios_9(WORD drv)
{
    return mediach(drv);
}
#endif

/**
 * bios_a - (drvmap) Read drive bitmap
 *
 * Returns a long containing a bit map of logical drives on  the system,
 * with bit 0, the least significant bit, corresponding to drive A.
 * Note that if the BIOS supports logical drives A and B on a single
 * physical drive, it should return both bits set if a floppy is present.
 */

static LONG drvmap(void)
{
    return blkdev_drvmap();
}

#if DBGBIOS
LONG bios_a(void)
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
LONG bios_b(WORD flag)
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
#if DBGBIOS
    kprintf("unimplemented BIOS function 0x%02x\n", number);
#endif
    return 0;
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
    VEC(bios_5, setexec),
    VEC(bios_6, tickcal),
    VEC(bios_7, getbpb),
    VEC(bios_8, bcostat),
    VEC(bios_9, mediach),
    VEC(bios_a, drvmap),
    VEC(bios_b, kbshift),
};

const short bios_ent = sizeof(bios_vecs) / sizeof(PFLONG);


