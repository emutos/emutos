/*
 * machine.c - detection of machine type
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "cookie.h"
#include "machine.h"
#include "processor.h"
#include "biosmem.h"
#include "vectors.h"
#include "nvram.h"
#include "tosvars.h"
#include "country.h"
#include "clock.h"
#include "natfeat.h"
#include "xhdi.h"
#include "string.h"
#include "dmasound.h"
#include "kprint.h"
#include "ide.h"
#include "asm.h"
#include "delay.h"
#include "mfp.h"
#include "scc.h"
#include "coldfire.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

long cookie_vdo;
#if CONF_WITH_FDC
long cookie_fdc;
#endif
long cookie_snd;
long cookie_mch;
#if CONF_WITH_DIP_SWITCHES
long cookie_swi;
#endif
#if CONF_WITH_FRB
static UBYTE *cookie_frb;   /* _initial_ value in the _FRB cookie */
#endif


/*
 * test specific hardware features
 */

#if CONF_WITH_STE_SHIFTER
int has_ste_shifter;
#endif
#if CONF_WITH_TT_SHIFTER
int has_tt_shifter;
#endif
#if CONF_WITH_VIDEL
int has_videl;
#endif

#if CONF_WITH_TT_MFP
int has_tt_mfp;
#endif
#if CONF_WITH_SCC
int has_scc;
#endif

/*
 * Tests video capabilities (STEnhanced Shifter, TT Shifter and VIDEL)
 */
static void detect_video(void)
{
#if CONF_WITH_STE_SHIFTER
    /* test if we have an STe Shifter by testing if register 820d
     * works (put a value, read other reg, read again, and compare)
     */
    volatile BYTE *ste_reg = (BYTE *) 0xffff820d;
    volatile BYTE *other_reg1 = (BYTE *) 0xffff8203;
    volatile WORD *other_reg2 = (WORD *) 0xffff8240;

    has_ste_shifter = 0;
    if (!check_read_byte((long)ste_reg))
        return;
    *ste_reg = 90;
    *other_reg1;        /* force register read (really useful ?) */
    if (*ste_reg == 90)
    {
        *ste_reg = 0;
        *other_reg2;    /* force register read (really useful ?) */
        if (*ste_reg == 0)
            has_ste_shifter = 1;
    }

    KDEBUG(("has_ste_shifter = %d\n", has_ste_shifter));
#endif

#if CONF_WITH_TT_SHIFTER
    /* test if we have a TT Shifter by testing for TT color palette */
    has_tt_shifter = 0;
    if (check_read_byte(TT_PALETTE_REGS))
        has_tt_shifter = 1;

    KDEBUG(("has_tt_shifter = %d\n", has_tt_shifter));
#endif

#if CONF_WITH_VIDEL
    /* test if we have Falcon VIDEL by testing for f030_xreg */
    has_videl = 0;
    if (check_read_byte(FALCON_HHT))
        has_videl = 1;

    KDEBUG(("has_videl = %d\n", has_videl));
#endif
}

/*
 * detect SCC (Falcon and TT) and second MFP (TT only)
 */
static void detect_serial_ports(void)
{
#if CONF_WITH_TT_MFP
    has_tt_mfp = 0;
    if (check_read_byte((LONG)TT_MFP_BASE+1))
        has_tt_mfp = 1;

    KDEBUG(("has_tt_mfp = %d\n", has_tt_mfp));
#endif

#if CONF_WITH_SCC
    has_scc = 0;
    if (check_read_byte(SCC_BASE))
        has_scc = 1;

    KDEBUG(("has_scc = %d\n", has_scc));
#endif
}

#if CONF_WITH_VME

int has_vme;

static void detect_vme(void)
{
    volatile BYTE *vme_mask = (BYTE *) VME_INT_MASK;
    volatile BYTE *sys_mask = (BYTE *) SYS_INT_MASK;

    if (check_read_byte(SCU_GPR1))
    {
        *vme_mask = 0x40;   /* ??? IRQ3 from VMEBUS/soft */
        *sys_mask = 0x14;   /* ??? set VSYNC and HSYNC */
        has_vme = 1;
    } else {
        has_vme = 0;
    }

    KDEBUG(("has_vme = %d\n", has_vme));
}

#endif /* CONF_WITH_VME */

#if CONF_WITH_MONSTER

int has_monster;

static void detect_monster(void)
{
    if (cookie_mch == MCH_ST || cookie_mch == MCH_STE || cookie_mch == MCH_MSTE)
        has_monster = check_read_byte(MONSTER_REG);
    else
        has_monster = FALSE;

    KDEBUG(("has_monster = %d\n", has_monster));
}

#endif /* CONF_WITH_MONSTER */

#if CONF_WITH_BLITTER

/* blitter */

int has_blitter;

static void detect_blitter(void)
{
    has_blitter = 0;

    if (check_read_byte(BLITTER_CONFIG1))
        has_blitter = 1;

    KDEBUG(("has_blitter = %d\n", has_blitter));
}

#endif /* CONF_WITH_BLITTER */


#if CONF_WITH_DIP_SWITCHES

int has_dip_switches;

static void detect_dip_switches(void)
{
    if (IS_ARANYM)
    {
        /* The auto-detection currently crashes ARAnyM-JIT. */
        has_dip_switches = 0;
    }
    else
    {
        has_dip_switches = check_read_byte(DIP_SWITCHES+1);
    }

    KDEBUG(("has_dip_switches = %d\n", has_dip_switches));
}

/* DIP switch usage is as follows (according to the "ATARI FALCON030
 * Service Guide", dated October 1, 1992):
 * bit 7: off => no DMA sound hardware
 * bit 6: off => AJAX FDC chip installed (support for 1.44MB floppy)
 * bit 5: off => quad density floppy
 * other bits are not used (and are set on)
 */

static void setvalue_swi(void)
{
    cookie_swi = (*(volatile UWORD *)DIP_SWITCHES)>>8;
    KDEBUG(("cookie_swi = 0x%08lx\n", cookie_swi));
}

#endif /* CONF_WITH_DIP_SWITCHES */

/* video type */

static void setvalue_vdo(void)
{
    if (HAS_VIDEL)
        cookie_vdo = 0x00030000L;
    else if (HAS_TT_SHIFTER)
        cookie_vdo = 0x00020000L;
    else if (HAS_STE_SHIFTER)
        cookie_vdo = 0x00010000L;
    else
        cookie_vdo = 0x00000000L;

    KDEBUG(("cookie_vdo = 0x%08lx\n", cookie_vdo));
}

/* machine type */
static void setvalue_mch(void)
{
#if CONF_ATARI_HARDWARE
    if (IS_ARANYM)
        cookie_mch = MCH_ARANYM;
    else if (HAS_VIDEL)
        cookie_mch = MCH_FALCON;
    else if (HAS_TT_SHIFTER)
        cookie_mch = MCH_TT;
    else if (HAS_STE_SHIFTER)
    {
        if (HAS_VME)
            cookie_mch = MCH_MSTE;
        else
            cookie_mch = MCH_STE;
    }
    else
        cookie_mch = MCH_ST;
#else
    cookie_mch = MCH_NOHARD;
#endif /* CONF_ATARI_HARDWARE */

    KDEBUG(("cookie_mch = 0x%08lx\n", cookie_mch));
}

/* SND */

static void setvalue_snd(void)
{
    cookie_snd = 0;

#if CONF_WITH_YM2149
    cookie_snd |= SND_PSG;
#endif

    if (HAS_DMASOUND)
    {
        cookie_snd |= SND_8BIT;
    }

    if (HAS_FALCON_DMASOUND)
    {
        cookie_snd |= SND_16BIT | SND_MATRIX;
    }

#if CONF_WITH_DIP_SWITCHES
    if (has_dip_switches)
    {
        /*
         * if DIP sw 8 is on (i.e. bit 7 is off), then we turn off the
         * indicator for 8-bit DMA stereo in the _SND cookie, just like
         * TOS3/TOS4 do.
         */
        if (!(cookie_swi & 0x80))
        {
            cookie_snd &= ~SND_8BIT;
        }
    }
#endif

    KDEBUG(("cookie_snd = 0x%08lx\n", cookie_snd));
}

#if CONF_WITH_FRB

/* FRB */

static void setvalue_frb(void)
{
    BOOL need_frb = FALSE; /* Required only if the system has Alt RAM */

#if CONF_WITH_FASTRAM
    /* Standard Atari TT-RAM may be present */
    need_frb = (ramtop != NULL);
#endif

#if CONF_WITH_MONSTER
    need_frb |= has_monster;
#endif

    if (need_frb)
    {
        cookie_frb = balloc(64 * 1024UL);
    }

    KDEBUG(("cookie_frb = 0x%08lx\n", (ULONG)cookie_frb));
}

#endif /* CONF_WITH_FRB */

#if CONF_WITH_FDC

/* FDC */

static void setvalue_fdc(void)
{
#if CONF_WITH_DIP_SWITCHES
    if (has_dip_switches && !(cookie_swi & 0x40))
    {
    /* switch *off* means AJAX controller is installed */
        cookie_fdc = FDC_1ATC;
    } else
#endif
    {
        cookie_fdc = FDC_0ATC;
    }

    KDEBUG(("cookie_fdc = 0x%08lx\n", cookie_fdc));
}

#endif /* CONF_WITH_FDC */

#if CONF_WITH_ARANYM
static const char *aranym_name = "ARAnyM";
int is_aranym;

static void aranym_machine_detect(void)
{
    char buffer[80];
    long bufsize;

    bufsize = nfGetFullName(buffer, sizeof(buffer)-1);
    is_aranym = bufsize > 0 && !strncasecmp(buffer, aranym_name, strlen(aranym_name));

    KDEBUG(("is_aranym = %d\n", is_aranym));
}
#endif

void machine_detect(void)
{
#if CONF_WITH_ARANYM
    aranym_machine_detect();
#endif
#ifdef MACHINE_AMIGA
    amiga_machine_detect();
#endif
    detect_video();
    detect_serial_ports();
#if CONF_WITH_VME
    detect_vme();
#endif
#if CONF_WITH_MEGARTC
    detect_megartc();
    KDEBUG(("has_megartc = %d\n", has_megartc));
#endif /* CONF_WITH_MEGARTC */
#if CONF_WITH_ICDRTC
    detect_icdrtc();
    KDEBUG(("has_icdrtc = %d\n", has_icdrtc));
#endif /* CONF_WITH_ICDRTC */
#if CONF_WITH_NVRAM
    detect_nvram();
#endif
#if CONF_WITH_DMASOUND
    detect_dmasound();
#endif
#if CONF_WITH_DIP_SWITCHES
    detect_dip_switches();
#endif
#if CONF_WITH_BLITTER
    detect_blitter();
#endif
#if CONF_WITH_IDE
    detect_ide();
#endif
#if CONF_WITH_MONSTER
    detect_monster();
    if (has_monster)
    {
        detect_monster_rtc();
        KDEBUG(("has_monster_rtc = %d\n", has_monster_rtc));
    }
#endif
}

/*
 * perform machine-specific initialisation
 */
void machine_init(void)
{
#if CONF_WITH_VIDEL
volatile BYTE *fbcr = (BYTE *)FALCON_BUS_CTL;
/* the Falcon Bus Control Register uses the following bits:
 *   0x40 : type of start (0=cold, 1=warm)
 *   0x20 : STe Bus emulation (0=on, 1=off)
 *   0x08 : blitter control (0=on, 1=off)
 *   0x04 : blitter speed (0=8MHz, 1=16MHz)
 *   0x01 : cpu speed (0=8MHz, 1=16MHz)
 * source: Hatari source code
 */
    if (has_videl)      /* i.e. it's a Falcon */
        *fbcr |= 0x29;  /* set STe Bus emulation off, blitter off, 16MHz CPU */
#endif

#if !CONF_WITH_RESET
/*
 * we must disable interrupts here, because the reset instruction hasn't
 * been run during startup
 */
 #if CONF_WITH_MFP
    {
        MFP *mfp = MFP_BASE;  /* set base address of MFP */

        mfp->iera = 0x00;     /* disable MFP interrupts */
        mfp->ierb = 0x00;
    }
 #endif

 #if CONF_WITH_TT_MFP
    if (has_tt_mfp)
    {
        MFP *mfp = TT_MFP_BASE; /* set base address of TT MFP */

        mfp->iera = 0x00;       /* disable MFP interrupts */
        mfp->ierb = 0x00;
    }
 #endif

 #if CONF_WITH_SCC
    if (has_scc)
    {
        SCC *scc = (SCC *)SCC_BASE;
        ULONG loops = loopcount_1_msec / 1000;  /* 1 usec = 8 cycles of SCC PCLK */

        scc->portA.ctl = 0x09;  /* issue hardware reset */
        delay_loop(loops);
        scc->portA.ctl = 0xC0;
        delay_loop(loops);
    }
 #endif
#endif /* CONF_WITH_RESET */
}

void fill_cookie_jar(void)
{
#ifdef __mcoldfire__
    cookie_add(COOKIE_COLDFIRE, 0);
    setvalue_mcf();
    cookie_add(COOKIE_MCF, (long)&cookie_mcf);
#else
    /* this is detected by detect_cpu(), called from processor_init() */
    cookie_add(COOKIE_CPU, mcpu);
#endif

    /* _VDO
     * This cookie represents the revision of the video shifter present.
     * Currently valid values are:
     * 0x00000000  ST
     * 0x00010000  STe
     * 0x00020000  TT030
     * 0x00030000  Falcon030
     */
    setvalue_vdo();
    cookie_add(COOKIE_VDO, cookie_vdo);

#ifndef __mcoldfire__
  /* this is detected by detect_fpu(), called from processor_init() */
    cookie_add(COOKIE_FPU, fputype);
#endif

    /* _MCH */
    setvalue_mch();
    cookie_add(COOKIE_MCH, cookie_mch);

#if CONF_WITH_DIP_SWITCHES
    /* _SWI  On machines that contain internal configuration dip switches,
     * this value specifies their positions as a bitmap. Dip switches are
     * generally used to indicate the presence of additional hardware which
     * will be represented by other cookies.
     */
    if (has_dip_switches)
    {
        setvalue_swi();
        cookie_add(COOKIE_SWI, cookie_swi);
    }
#endif

    /* _SND
     * This cookie contains a bitmap of sound features available to the
     * system as follows:
     * 0x01 GI Sound Chip (PSG)
     * 0x02 1 Stereo 8-bit Playback
     * 0x04 DMA Record (w/XBIOS)
     * 0x08 16-bit CODEC
     * 0x10 DSP
     */
    setvalue_snd();
    cookie_add(COOKIE_SND, cookie_snd);

#if CONF_WITH_FRB
    /* _FRB  This cookie is present when alternative RAM is present. It
     * points to a 64k buffer that may be used by DMA device drivers to
     * transfer memory between alternative RAM and ST RAM for DMA operations.
     */
    setvalue_frb();
    if (cookie_frb)
    {
        cookie_add(COOKIE_FRB, (long)cookie_frb);
    }
#endif /* CONF_WITH_FRB */

    /* _FLK  The presence of this cookie indicates that file and record
     * locking extensions to GEMDOS exist. The value field is a version
     * number currently undefined.
     */

    /* _AKP  This cookie indicates the presence of an Advanced Keyboard
     * Processor. The high word of this cookie is currently reserved.
     * The low word indicates the language currently used by TOS for
     * keyboard interpretation and alerts.
     */
    detect_akp();
    KDEBUG(("cookie_akp = 0x%08lx\n", cookie_akp));
    cookie_add(COOKIE_AKP, cookie_akp);

    /* _IDT  This cookie defines the currently configured date and time
     * format.  Bits #0-7 contain the ASCII code of the date separator.
     * Bits #8-11 contain a value indicating the date display format as
     * follows:
     *   0 MM-DD-YY
     *   1 DD-MM-YY
     *   2 YY-MM-DD
     *   3 YY-DD-MM
     * Bits #12-15 contain a value indicating the time format as follows:
     *   0 12 hour
     *   1 24 hour
     * Note: The value of this cookie does not affect any of the internal
     * time functions. It is intended for informational use by applications
     * and may also used by the desktop for its date & time displays.
     */
    detect_idt();
    KDEBUG(("cookie_idt = 0x%08lx\n", cookie_idt));
    cookie_add(COOKIE_IDT, cookie_idt);

#if CONF_WITH_FDC
    /* Floppy Drive Controller
     * Most significant byte means:
     * 0 - DD (Normal floppy interface)
     * 1 - HD (1.44 MB with 3.5")
     * 2 - ED (2.88 MB with 3.5")
     * the 3 other bytes are the Controller ID:
     * 0 - No information available
     * 'ATC' - Fully compatible interface built in a way that
     * behaves like part of the system.
     */
    setvalue_fdc();
    cookie_add(COOKIE_FDC, cookie_fdc);
#endif

#if DETECT_NATIVE_FEATURES
    if (has_natfeats())
    {
        cookie_add(COOKIE_NATFEAT, (long)&natfeat_cookie);
    }
#endif

#if CONF_WITH_XHDI
    create_XHDI_cookie();
#endif

#if !CONF_WITH_MFP
    /* Set the _5MS cookie with the address of the 200 Hz system timer
     * interrupt vector so FreeMiNT can hook it. */
    cookie_add(COOKIE__5MS, (long)&vector_5ms);
#endif
}

static const char * guess_machine_name(void)
{
#if CONF_WITH_ARANYM
    if (is_aranym)
        return aranym_name;
#endif

    switch(cookie_mch) {
    case MCH_ST:
        if (HAS_MEGARTC)
            return "Atari Mega ST";
        else
            return "Atari ST";
    case MCH_STE:
        return "Atari STe";
    case MCH_MSTE:
        return "Atari Mega STe";
    case MCH_TT:
        return "Atari TT";
    case MCH_FALCON:
        return "Atari Falcon";
    default:
        return "unknown";
    }
}

const char * machine_name(void)
{
    MAYBE_UNUSED(guess_machine_name);
#ifdef MACHINE_FIREBEE
    return "FireBee";
#elif defined(MACHINE_AMIGA)
    return "Amiga";
#elif defined(MACHINE_M548X)
    return m548x_machine_name();
#else
    return guess_machine_name();
#endif
}
