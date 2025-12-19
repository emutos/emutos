/*
 * nova.c - Nova/Vofa/Wittich adapter graphics card routines
 *
 * Copyright (C) 2018-2025 The EmuTOS development team
 *
 * Authors:
 * Christian Zietz
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "asm.h"
#include "delay.h"
#include "vectors.h"
#include "tosvars.h"
#include "lineavars.h"
#include "machine.h"
#include "has.h"
#include "biosext.h"
#include "nova.h"

#if CONF_WITH_NOVA

/* Base addresses of Nova registers and video memory.
   Autodetected by detect_nova().
*/
static volatile UBYTE* novaregbase;
static volatile UBYTE* novamembase;
int has_nova;

/* Nova and Volksfarben HW differ in the way that accesses to IO ports
 * are handled: On Nova, read accesses to odd IO addresses will only
 * work when the card is switched to 16 bit IO mode. On the Volksfarben,
 * these accesses will not work when the card is switched to 16 bit.
 */
static BOOL use_16bit_io;

/* CrazyDots has a special clock generator register. */
static BOOL is_crazydots;
#define CRAZY_DOTS_CLK_SEL 0x000

/* Macros for VGA register access */
#define VGAREG(x)   (*(novaregbase+(x)))
/* Note that for 16 bit access high and low word are swapped by Nova HW.
   Writing to and reading from VGAREG_W needs to take this into account. */
#define VGAREG_W(x) (*(volatile UWORD*)(novaregbase+(x)))

#define CRTC_I  0x3D4   /* CRT Controller index and data ports */
#define CRTC_D  0x3D5
#define GDC_SEG 0x3CD   /* GDC segment select, index and data ports */
#define GDC_I   0x3CE
#define GDC_D   0x3CF
#define TS_I    0x3C4   /* Timing Sequencer index and data ports */
#define TS_D    0x3C5
#define VIDSUB  0x3C3   /* Video Subsystem register */
#define MISC_W  0x3C2   /* Misc Output Write Register */
#define DAC_PEL 0x3C6   /* RAMDAC pixel mask */
#define DAC_IR  0x3C7   /* RAMDAC read index */
#define DAC_IW  0x3C8   /* RAMDAC write index */
#define DAC_D   0x3C9   /* RAMDAC palette data */
#define ATC_IW  0x3C0   /* Attribute controller: index and data write */
#define IS1_RC  0x3DA   /* Input Status Register 1: color emulation */

/* ATI Mach32 only */
#define SETUP_CONTROL   0x0102
#define ATI_I           0x01CE
#define ATI_DAC_R2      0x02EA
#define ATI_DAC_R3      0x02EB
#define ATI_DAC_R0      0x02EC
#define ATI_DAC_R1      0x02ED
#define CONFIG_STATUS_1 0x12EE
#define MISC_OPTIONS    0x36EE
#define SUBSYS_CNTL     0x42E8
#define ADVFUNC_CNTL    0x4AE8
#define CLOCK_SEL       0x4AEE
#define MEM_BNDRY       0x42EE
#define ROM_PAGE_SEL    0x46E8
#define ROM_ADDR_1      0x52EE
#define SCRATCH_PAD_1   0x56EE
#define MEM_CFG         0x5EEE
#define HORZ_OVERSCAN   0x62EE
#define MAX_WAITSTATES  0x6AEE
#define EXT_GE_CONFIG   0x7AEE
#define MISC_CNTL       0x7EEE
#define R_MISC_CNTL     0x92EE

/* ATI Mach64 only */
static UBYTE m64_dac_type = 0;
#define M64_DAC_ATI68860    5
#define M64_DAC_CH8398      7

#define M64_BUS_CNTL        0x4EEC
#define M64_CONFIG_CHIP_ID  0x6EEC
#define M64_SCRATCH_REG_1   0x46EC
#define M64_CRTC_INT_CNTL   0x1AEC
#define M64_CRTC_GEN_CNTL   0x1EEC
#define M64_GEN_TEST_CNTL   0x66EC
#define M64_MEM_CNTL        0x52EC
#define M64_CONFIG_CNTL     0x6AEC
#define M64_DAC_CNTL        0x62EC
#define M64_CLOCK_CNTL      0x4AEC

#define M64_GX_CHIP_ID      0xD7

typedef enum {
    MACH_NOT_DETECTED = 0,
    MACH_32,
    MACH_64
} MACH_TYPE;

#define IS_MACH (mach_type != MACH_NOT_DETECTED)
#define IS_MACH32 (mach_type == MACH_32)
#define IS_MACH64 (mach_type == MACH_64)

static ULONG delay20us;
#define SHORT_DELAY delay_loop(delay20us)
#define LONG_DELAY  delay_loop(loopcount_1_msec)


/* Functions for easier access of indexed registers */
static inline UBYTE get_idxreg(UWORD port, UBYTE reg) __attribute__((always_inline));
static inline void set_idxreg(UWORD port, UBYTE reg, UBYTE value) __attribute__((always_inline));
static void set_multiple_idxreg(UWORD port, UBYTE startreg, UBYTE cnt, const UBYTE *values);
static void set_multiple_atcreg(UBYTE startreg, UBYTE cnt, const UBYTE *values);

/* Register data for Mach32/64/ET4000 init */
/* Timing Sequencer registers 1...4 */
static const UBYTE vga_TS_1_4[] = {0x01,0x01,0x00,0x06};
/* Timing Sequencer registers 6...8 */
static const UBYTE et4000_TS_6_8[] = {0x00,0xF4,0x03};
/* Misc Output Write Register */
static const UBYTE vga_MISC_W = 0xE3; /* sync polarity: H-, V- */
/* CRT Controller: registers 0 - 0x18 */
static const UBYTE vga_CRTC_0_0x18[] = {0x5F,0x4F,0x50,0x82,
    0x54,0x80,0x0B,0x3E,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
    0x00,0xEA,0x0C,0xDF,0x28,0x00,0xE7,0x04,0xC3,0xFF};
/* CRT Controller: registers 0x33 - 0x35 */
static const UBYTE et4000_CRTC_0x33_0x35[] = {0x00,0x00,0x00};
/* Attribute Controller: registers 0 - 0x16 */
static const UBYTE vga_ATC_0_0x16[] = {0x00,0x01,0x02,0x03,
     0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
     0x0F,0x01,0xFF,0x01,0x10,0x00,0x05,0x00};
static const UBYTE vga_GDC_0_8[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x0F,0xFF};
static const UBYTE vga_palette[] = {0xFF,0xFF,0xFF, 0,0,0, 0,0,0};

/* Read an indexed VGA register */
static inline UBYTE get_idxreg(UWORD port, UBYTE reg)
{
    VGAREG(port) = reg;
    SHORT_DELAY;
    return VGAREG(port+1);
}

/* Write an indexed VGA register */
static inline void set_idxreg(UWORD port, UBYTE reg, UBYTE value)
{
    VGAREG(port) = reg;
    SHORT_DELAY;
    VGAREG(port+1) = value;
}

/* Write multiple indexed VGA register */
static void set_multiple_idxreg(UWORD port, UBYTE startreg, UBYTE cnt, const UBYTE *values)
{
    for (; cnt>0; cnt--, startreg++, values++)
    {
        set_idxreg(port, startreg, *values);
    }
}

/* Write multiple ATC registers.
   The Attribute Controller uses a different indexing scheme than other VGA ports. */
static void set_multiple_atcreg(UBYTE startreg, UBYTE cnt, const UBYTE *values)
{
    FORCE_READ(VGAREG(IS1_RC)); /* set ATC_IW to index */

    for (; cnt>0; cnt--, startreg++, values++)
    {
        VGAREG(ATC_IW) = startreg;
        VGAREG(ATC_IW) = *values; /* yes, they really go to the same port */
    }
    VGAREG(ATC_IW) = 0x20; /* enable screen output */
}

/* Check for presence of a VGA card using the ATC palette registers. */
static BOOL check_for_vga(void)
{
    FORCE_READ(VGAREG(IS1_RC)); /* set ATC_IW to index */

    SHORT_DELAY;
    VGAREG(ATC_IW) = 0x0A;
    SHORT_DELAY;
    VGAREG(ATC_IW) = 0x55;
    SHORT_DELAY;

    /* Reading the index/write register will return the register index */
    if (VGAREG(ATC_IW) == 0x0A)
    {
        VGAREG(ATC_IW) = 0x20; /* enable screen output */
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/* Detect Nova/Volksfarben/CrazyDots addresses */
void detect_nova(void)
{
    has_nova = 0;
    use_16bit_io = TRUE; /* Default for everything but Volksfarben/ST */
    is_crazydots = FALSE;

    if (IS_BUS32 && HAS_VME && check_read_byte(0xFE900000UL+VIDSUB))
    {
        /* Nova/Mach in Atari TT */
        novaregbase = (UBYTE *)0xFE900000UL;
        novamembase = (UBYTE *)0xFE800000UL;
        has_nova = 1;
    }
    else if (HAS_VME && check_read_byte(0xFEDC0000UL+VIDSUB))
    {
        /*
         * Nova/ET4000 in Atari MegaSTe or TT:
         * In the MegaSTE the top 8 address bits are simply ignored.
         */
        novaregbase = (UBYTE *)0xFEDC0000UL;
        novamembase = (UBYTE *)0xFEC00000UL;
        has_nova = 1;
    }
    else if (HAS_VME && check_read_byte(0xFEBF0000UL+VIDSUB))
    {
        /* CrazyDots in Atari MegaSTe or TT */
        novaregbase = (UBYTE *)0xFEBF0000UL;
        novamembase = (UBYTE *)0xFEC00000UL;
        has_nova = 1;
        is_crazydots = TRUE;
    }
    else if (((ULONG)phystop < 0x00C00000UL) && check_read_byte(0x00D00000UL+VIDSUB) &&
             check_read_byte(0x00C00000UL) && check_read_byte(0x00C80000UL))
    {
        /*
         * Volksfarben 4000 in ST:
         * - be sure via phystop that it's not RAM we read
         * - check 0xC00000 and 0xC80000 to exclude MiSTer's Viking emulation
         */
        novaregbase = (UBYTE *)0x00D00000UL;
        novamembase = (UBYTE *)0x00C00000UL;
        has_nova = 1;
        use_16bit_io = FALSE;
    }

    if (has_nova)
    {
        KDEBUG(("VGA card detected at IO=%p / mem=%p\n", novaregbase, novamembase));
    }
}

/* Detect ATI Mach32 and Mach64 */
static MACH_TYPE detect_mach(void)
{
    MACH_TYPE result = MACH_NOT_DETECTED;

    /*
     * For Mach64 check chip ID.
     * Only the Mach64 GX is known to exist on ISA cards.
     */
    if (VGAREG(M64_CONFIG_CHIP_ID) == M64_GX_CHIP_ID)
    {
        /* Try to read and write Mach64-specific scratch register. */
        VGAREG(M64_SCRATCH_REG_1) = 0x55U;
        if (VGAREG(M64_SCRATCH_REG_1) == 0x55U)
        {
            result = MACH_64;
        }
    }
    /*
     * For Mach32 not every revision has a chip ID register.
     * Instead, check ATI magic number in the BIOS ROM on the card.
     * After power-up, only even bytes are accessible.
     */
    else if (*(novamembase + 0xC0032UL) == '6' && *(novamembase + 0xC0034UL) == '2')
    {
        /* Try to read and write Mach32-specific scratch register. */
        VGAREG(SCRATCH_PAD_1) = 0x55U;
        if (VGAREG(SCRATCH_PAD_1) == 0x55U)
        {
            result = MACH_32;
        }
    }

    if (result) {
        KDEBUG(("detect_mach() detected an ATI %s\n", (result==MACH_32)?"Mach32":"Mach64"));
    } else {
        KDEBUG(("detect_mach() did not detect an ATI Mach32/64.\n"));
    }
    return result;
}

/* Basic Mach32 initialisation before VGA mode can be enabled. */
static void init_mach32(void)
{

    KDEBUG(("init_mach32()\n"));

    /* General card configuration */
    VGAREG(MISC_OPTIONS) = 0xA5;    /* Enable 16 bit IO */
    VGAREG(MISC_OPTIONS+1) = 0x90;
    VGAREG(MAX_WAITSTATES) = 0x0E;  /* Memory config */
    VGAREG(MEM_BNDRY) = 0x00;
    VGAREG_W(MEM_CFG) = 0x0202;
    VGAREG_W(ROM_ADDR_1) = 0x0040;
    /* VGAREG_W(SCRATCH_PAD_1) = 0x0000; */
    VGAREG(ADVFUNC_CNTL) = 0x03;    /* Go to 8514 mode */
    VGAREG_W(SUBSYS_CNTL) = 0x90;   /* 8514 reset */
    VGAREG_W(SUBSYS_CNTL) = 0x50;

    /* Configure DAC */
    VGAREG_W(EXT_GE_CONFIG) = 0x1A20; /* Select DAC registers 8-11. */
    VGAREG(ATI_DAC_R1) = 0x00;      /* Input clock select */
    VGAREG(ATI_DAC_R2) = 0x30;      /* Output clock select */
    VGAREG(ATI_DAC_R3) = 0x2D;      /* Mux control */
    VGAREG(MISC_CNTL+1) = (VGAREG(R_MISC_CNTL+1) & 0xF0) | 0x0C;
    VGAREG_W(EXT_GE_CONFIG) = 0x1A40; /* DAC 8 bit mode. */
    VGAREG_W(HORZ_OVERSCAN) = 0;
    /* VGAREG(ATI_DAC_R2) = 0xFF; */   /* DAC mask register. Set later. */

    VGAREG_W(CLOCK_SEL) = 0x5002;   /* Back to ATI mode and clock select */

    /* Enable VGA mode */
    VGAREG(ROM_PAGE_SEL) = 0x10;    /* VGA setup mode */
    VGAREG(SETUP_CONTROL) = 0x01;   /* Enable card */
    VGAREG(ROM_PAGE_SEL) = 0x08;    /* Enable VGA */
}

/* Set address lines RS2 and RS3 of DAC on Mach64 */
static void mach_dac_setrs2rs3(UBYTE rs2rs3)
{
    set_idxreg(ATI_I, 0xA0, ((rs2rs3&3)<<5) | 0x8);
}

/*
   Control word to program MCLK3 to 40 MHz:
   N = 0+257, PostDiv = 2, RefDiv = 46, RefFreq = 14.318 MHz.
   PPDDE7654321043210WS
   11100000000001001100 = 0xE004C
*/
#define PROG_MCLK3_40000_KHZ 0xE004Cul
/*
   Control word to program VCLK4 to 50.35 MHz,
   twice the VGA pixel clock:
   N = 67+257, PostDiv = 2, RefDiv = 46, RefFreq = 14.318 MHz.
   PPDDE7654321043210WS
   11100010000110010000 = 0xE2190
*/
#define PROG_VCLK4_50350_KHZ 0xE2190ul

#define FS2(x) ((x)<<2)
#define FS3(x) ((x)<<3)
#define STROBE (1<<6)

/* Program the ATI-18818 / ICS2595 clock generator found in some Mach64. */
static void prog_ics2595(ULONG prog)
{
    /* start of programming sequence */
    VGAREG(M64_CLOCK_CNTL) = FS2(0) | FS3(0) | STROBE;
    SHORT_DELAY;
    VGAREG(M64_CLOCK_CNTL) = FS2(1) | FS3(0) | STROBE;
    SHORT_DELAY;
    /* shift out programming bits */
    while (prog)
    {
        VGAREG(M64_CLOCK_CNTL) = FS2(prog&1) | FS3(0) | STROBE;
        SHORT_DELAY;
        VGAREG(M64_CLOCK_CNTL) = FS2(prog&1) | FS3(1) | STROBE;
        SHORT_DELAY;
        prog >>= 1;
    }
}

/* Basic Mach64 initialisation before VGA mode can be enabled. */
static void init_mach64(void)
{

    KDEBUG(("init_mach64()\n"));

    /* General card configuration */
    VGAREG(M64_BUS_CNTL) = 0xF1;
    VGAREG(M64_BUS_CNTL+1) = 0x20;          /* Enable 16 bit IO */
    VGAREG_W(M64_BUS_CNTL+2) = 0x0e89;
    VGAREG_W(M64_CRTC_INT_CNTL) = 0x0000;
    VGAREG_W(M64_CRTC_GEN_CNTL) = 0x0002;   /* 8 bpp */
    VGAREG(M64_CRTC_GEN_CNTL+3) = 0x00;     /* reset CRTC */
    VGAREG_W(M64_GEN_TEST_CNTL) = 0x0000;
    VGAREG_W(M64_GEN_TEST_CNTL+2) = 0x0000;
    VGAREG_W(M64_MEM_CNTL) = 0xf101;        /* at least 1 MB of memory */
    VGAREG_W(M64_CONFIG_CNTL) = 0x1100;     /* linear aperture enabled */
    VGAREG_W(M64_DAC_CNTL) = 0x0020;
    m64_dac_type = VGAREG(M64_DAC_CNTL+2) & 0x7; /* RAM DAC type */

    KDEBUG(("Detected DAC type %d\n", m64_dac_type));

    /* Switch to accelerator mode to have better access to clock gen and DAC */
    VGAREG(M64_CRTC_GEN_CNTL+3) = 0x03;

    /* ATI68860 RAM DAC is used with ICS-style clock generator */
    if (m64_dac_type == M64_DAC_ATI68860)
    {
        /* Program clock generator with pixel and memory clock */
        prog_ics2595(PROG_VCLK4_50350_KHZ);
        prog_ics2595(PROG_MCLK3_40000_KHZ);
    }

    /* Enable VGA mode */
    VGAREG(M64_CRTC_GEN_CNTL+3) = 0x02;     /* switch to VGA mode */
    VGAREG(ROM_PAGE_SEL) = 0x10;    /* VGA setup mode */
    VGAREG(SETUP_CONTROL) = 0x01;   /* Enable card */
    VGAREG(ROM_PAGE_SEL) = 0x08;    /* Enable VGA */
}

/* Unlocks access to the extended registers of ET4000 */
static void unlock_et4000(void)
{
    VGAREG(0x3BF) = 0x03;
    SHORT_DELAY;
    VGAREG(0x3D8) = 0xA0;
}

/* Initializes card and memory access */
static void init_et4000(void)
{
    KDEBUG(("init_et4000()\n"));
    set_idxreg(CRTC_I, 0x34, 0x00); /* 6845 compatibility reg */
    set_idxreg(CRTC_I, 0x31, 0x00); /* W32: clock select */

    set_idxreg(TS_I, 0x02, 0x0F);   /* Write enable to all planes */
    set_idxreg(TS_I, 0x04, 0x0C);   /* Chain 4 mode */
    set_idxreg(GDC_I, 0x01, 0x00);  /* Disable set/reset mode */
    set_idxreg(GDC_I, 0x03, 0x00);  /* Function select = MOVE */
    set_idxreg(GDC_I, 0x06, 0x05);  /* Memory map: graphics mode enable */
    set_idxreg(GDC_I, 0x08, 0xFF);  /* Bit mask: pass all CPU bits */
    VGAREG(GDC_SEG) = 0x00;         /* Select segment 0, because of linear mode(?) */
    set_idxreg(CRTC_I, 0x32, 0x28); /* RAS/CAS timing */
    if (use_16bit_io) {
        set_idxreg(CRTC_I, 0x36, 0xF3); /* Video System Conf. 1: linear memory access, 16 bit IO access */
    } else {
        set_idxreg(CRTC_I, 0x36, 0x73); /* Video System Conf. 1: linear memory access, 8 bit IO access */
    }
    set_idxreg(CRTC_I, 0x37, 0x0F); /* Video System Conf. 2: DRAM, 32 bit wide */
}

/*
   Switches HiColor RAMDAC to palette mode.
   Note that we don't have to test whether this actually is a HiColor DAC at all.
   With a non-HiColor DAC this only overwrites the pixel mask register that will be reset later, anyway.
*/
static void ramdac_hicolor_off(void)
{
    /* Switch DAC to command mode, if supported */
    FORCE_READ(VGAREG(DAC_IW));
    SHORT_DELAY;
    FORCE_READ(VGAREG(DAC_PEL));
    SHORT_DELAY;
    FORCE_READ(VGAREG(DAC_PEL));
    SHORT_DELAY;
    FORCE_READ(VGAREG(DAC_PEL));
    SHORT_DELAY;
    FORCE_READ(VGAREG(DAC_PEL));
    SHORT_DELAY;

    VGAREG(DAC_PEL) = 0x00;  /* HiColor off */
    FORCE_READ(VGAREG(DAC_IW)); /* Back to normal mode */
}

/* Configure the ATI-68860 RAM DAC found on some Mach64 */
static void ramdac_68860_config(void)
{
    /* Configure DAC */
    mach_dac_setrs2rs3(2);      /* select DAC registers 8 - 11 */
    VGAREG(DAC_PEL) = 0x1D;     /* always set to 0x1D */
    VGAREG(DAC_IR) = 0x80;      /* Graphic Mode register: VGA mode (?) */
    VGAREG(DAC_IW) = 0x02;      /* black level(?), always set to 2 */
    mach_dac_setrs2rs3(0);
}

/* Unlock or lock the clock selection of the CH8398 RAM DAC.
   Note that RS2 must be set to high before calling this function. */
static void ramdac_ch8398_unlock(BOOL unlock)
{
    /* Magic access sequence for Clock Select Register */
    FORCE_READ(VGAREG(DAC_PEL));
    FORCE_READ(VGAREG(DAC_IW));
    FORCE_READ(VGAREG(DAC_IW));
    FORCE_READ(VGAREG(DAC_IW));
    FORCE_READ(VGAREG(DAC_IW));
    /* reset or set the PLL frequency hold bit */
    VGAREG(DAC_IW) = unlock?0x80:0x00;
}

/* Values from CH8398 data sheet: M = 3, N = 27, K = 1 = 50.11 MHz */
#define PROG_PLL4_50MHZ ((3u<<8)|27u|(1u<<14))

/* Configure the CH8398 RAM DAC found on some Mach64 */
static void ramdac_ch8398_config(void)
{
    mach_dac_setrs2rs3(1);    /* RS2=1, select DAC registers 4 - 7 */
    /* unlock clock settings */
    ramdac_ch8398_unlock(TRUE);

    /* Access control register and set it to 8bpp mode */
    VGAREG(DAC_PEL) = 0x04;

    /* Configure the clock entry 4, which is used by this driver, for 50 MHz. */
    VGAREG(DAC_IW) = 4;                /* program clock entry 4 */
    VGAREG(DAC_D) = (UBYTE)PROG_PLL4_50MHZ;
    VGAREG(DAC_D) = (UBYTE)(PROG_PLL4_50MHZ >> 8);

    mach_dac_setrs2rs3(0);
}

/* Loads the Mach specific indexed registers */
static void set_mach_idxreg(void)
{
    static const UBYTE initvalues[] = {
    /* reg, val */
      0x86, 0x7A,
      0xA3, 0x00,
      0xAD, 0x00, /* does not exist on Mach64, but writes are ignored */
      0xAE, 0x00, /* does not exist on Mach64, but writes are ignored */
      0xB0, 0x08,
      0xB1, 0x00,
      0xB2, 0x00,
      0xB3, 0x00,
      0xB4, 0x00,
      0xB5, 0x00,
      0xB6, 0x01,
      0xBD, 0x04,
      0xBF, 0x01,
      0x00 };

    const UBYTE* p;

    for (p = initvalues; *p != 0; p+=2) {
        set_idxreg(ATI_I, *p, *(p+1));
    }

    set_idxreg(TS_I, 0x00, 0x01);   /* Reset Timing Sequencer */
    set_idxreg(ATI_I, 0xB9, 0x42);  /* Configure clock generator */
    set_idxreg(ATI_I, 0xB8, 0x40);
    set_idxreg(ATI_I, 0xBE, 0x00);
    VGAREG(MISC_W) = vga_MISC_W;    /* Needed again here. */
    set_idxreg(TS_I, 0x00, 0x03);
}


/* Loads the palette entries for colors 0 = white, 1 = black and 255 = overscan, also black */
static void set_palette_entries(const UBYTE* palette)
{
    int k;

    VGAREG(DAC_PEL) = 0xFF; /* DAC pixel mask */

    /* Load colors 0 and 1 */
    VGAREG(DAC_IW) = 0; /* color 0 */
    for (k=0; k<6; k++)
    {
        VGAREG(DAC_D) = *palette++;
        SHORT_DELAY;
    }

    /* Load color 255 */
    VGAREG(DAC_IW) = 255; /* color 255 */
    for (k=0; k<3; k++)
    {
        VGAREG(DAC_D) = *palette++;
        SHORT_DELAY;
    }
}


/* Loads predefined values to all relevant VGA registers */
static void init_nova_resolution(MACH_TYPE mach_type)
{
    UBYTE temp;

    KDEBUG(("init_nova_resolution()\n"));
    if (!IS_MACH) {
        ramdac_hicolor_off();
    }

    /* Load registers */
    if (IS_MACH) {
        /* Mach32/64 indexed registers at 0x1CE. */
        set_idxreg(GDC_I, 0x50, 0xCE);
        set_idxreg(GDC_I, 0x51, 0x81);
    }

    /* Configure Mach64 DACs */
    if (m64_dac_type == M64_DAC_CH8398) {
        ramdac_ch8398_config();
    } else if (m64_dac_type == M64_DAC_ATI68860) {
        ramdac_68860_config();
    }

    /* Reset Timing Sequencer */
    set_idxreg(TS_I, 0x00, 0x01);
    LONG_DELAY;
    set_idxreg(TS_I, 0x00, 0x03);

    if (!IS_MACH) {
        unlock_et4000(); /* required again after TS reset */
    }

    set_multiple_idxreg(TS_I, 1, sizeof(vga_TS_1_4), vga_TS_1_4);
    if (!IS_MACH) {
        set_multiple_idxreg(TS_I, 6, sizeof(et4000_TS_6_8), et4000_TS_6_8);
    }

    VGAREG(MISC_W) = vga_MISC_W;

    set_idxreg(CRTC_I, 0x11, 0); /* enable write to CRTC */
    set_multiple_idxreg(CRTC_I, 0, sizeof(vga_CRTC_0_0x18), vga_CRTC_0_0x18);
    if (!IS_MACH) {
        set_multiple_idxreg(CRTC_I, 0x33, sizeof(et4000_CRTC_0x33_0x35), et4000_CRTC_0x33_0x35);
    }
    temp = get_idxreg(CRTC_I, 0x11);
    set_idxreg(CRTC_I, 0x11, temp | 0x80); /* disable write to CRTC */

    if (IS_MACH) {
        /* Do not write ATC registers 0x15 and 0x16 on Mach32/64 */
        set_multiple_atcreg(0, sizeof(vga_ATC_0_0x16) - 2, vga_ATC_0_0x16);
    } else {   /* ET4000 */
        set_multiple_atcreg(0, sizeof(vga_ATC_0_0x16), vga_ATC_0_0x16);
    }
    set_multiple_idxreg(GDC_I, 0, sizeof(vga_GDC_0_8), vga_GDC_0_8);

    if (IS_MACH) {
        set_mach_idxreg();
    } else {   /* ET4000 */
        set_idxreg(CRTC_I, 0x36, use_16bit_io? 0xD3:0x53);
    }
    set_idxreg(TS_I, 1, vga_TS_1_4[0] | 0x20); /* screen off */
    set_palette_entries(vga_palette);
    set_idxreg(TS_I, 1, vga_TS_1_4[0]); /* screen on */

    /*
     * after setting our mode / clock config lock the DAC,
     * because Nova VDI expects to find it locked.
     */
    if (m64_dac_type == M64_DAC_CH8398) {
        mach_dac_setrs2rs3(1);  /* RS2 = 1 */
        ramdac_ch8398_unlock(FALSE);
        mach_dac_setrs2rs3(0);
    }
}

/* Certain ET4000 graphic cards require a different clock divider.
   There is no direct way of finding out the clock the ET4000 runs on.
   Instead, this function counts the number of VBLs for 250 ms.
*/
#define VBL_TIMEOUT 50  /* 0.25 seconds */
#define VBL_LIMIT   12  /* nominally: 60 Hz, i.e. 15 VBLs in 0.25s */
static void count_vbls(void)
{
    LONG end = hz_200 + VBL_TIMEOUT;
    int vbl_count = 0;

    KDEBUG(("count_vbls(): "));
    while (hz_200 < end)
    {
         /* wait for Vertical Retrace bit to go low */
        while (VGAREG(IS1_RC) & 0x08)
        {
            /* time out in case bit is stuck */
            if (hz_200 >= end)
                break;
        }

         /* wait for Vertical Retrace bit to go high */
        while (!(VGAREG(IS1_RC) & 0x08))
        {
            /* time out in case bit is stuck */
            if (hz_200 >= end)
                break;
        }

        vbl_count++;
    }
    KDEBUG(("%d VBLs/second\n", (200/VBL_TIMEOUT)*vbl_count));

    if (vbl_count < VBL_LIMIT)
    {
        /* Switch off MCLK/2 divider */
        set_idxreg(TS_I, 0x07, 0xA4);
    }
}

/* Test that video memory is accessible */
static BOOL test_video_memory(void)
{
    /* Note that novamembase is declared volatile,
       so the compiler won't optimize this out. */
    *novamembase = 0x00;
    if (*novamembase != 0x00)
        return FALSE;

    *novamembase = 0x55;
    if (*novamembase != 0x55)
        return FALSE;

    return TRUE;
}

/* Sets system variables so that EmuTOS will use the graphics card */
static void init_system_vars(void)
{
    KDEBUG(("init_system_vars()\n"));

    /* Screen address */
    v_bas_ad = (UBYTE *)novamembase;
    /* Fake 640x400x2 video mode (ST high) */
    sshiftmod = 2;

    /* Line A vars */
    /* Number of bitplanes */
    v_planes = 1;
    /* Bytes per scan-line */
    BYTES_LIN = v_lin_wr = 80;
    /* Vertical resolution */
    V_REZ_VT = 480;
    /* Horizontal resolution */
    V_REZ_HZ = 640;
}

/* Initialize Nova card */
BOOL init_nova(void)
{
    MACH_TYPE mach_type;

    delay20us = loopcount_1_msec / 50;

    /* Fail if detect_nova() hasn't found card */
    if (!has_nova)
        return FALSE;

    if (!is_crazydots) {
        /* Detect ATI Mach (as opposed to ET4000). */
        mach_type = detect_mach();
    } else {
        /* Crazydots always has a ET4000 */
        mach_type = MACH_NOT_DETECTED;
    }

    if (IS_MACH32) {
        novamembase += 0x0A0000UL;
        init_mach32();
    } else if (IS_MACH64) {
        novamembase += 0x0A0000UL;
        init_mach64();
    }

    /* Enable VGA mode */
    VGAREG(VIDSUB) = 0x01;
    VGAREG(MISC_W) = vga_MISC_W; /* Select color mode & MCLK1 */

    /* Sanity check that no other VME or Megabus HW has been detected.
     * Note that we can do this only after enabling VGA in the code above.
     */
    if (!check_for_vga()) {
        KDEBUG(("No Nova or no VGA card found\n"));
        return FALSE;
    }

    if (!IS_MACH) {   /* ET4000 */
        if (is_crazydots) {
            /* Program clock generator to 25.175 MHz pixel clock */
            VGAREG(CRAZY_DOTS_CLK_SEL) = 0x4;
        }
        unlock_et4000();
        init_et4000();
    }

    init_nova_resolution(mach_type);

    if (!IS_MACH) {   /* ET4000 */
        count_vbls();
    }

    if (!test_video_memory()) {
        KDEBUG(("Nova memory inaccessible\n"));
        /* TODO: Try alternative address, like driver does */
        return FALSE;
    }

    init_system_vars();

    return TRUE;
}

UBYTE* get_novamembase(void)
{
    return (UBYTE *)novamembase;
}

#endif /* CONF_WITH_NOVA */
