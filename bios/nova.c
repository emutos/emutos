/*
 * nova.c - Nova graphic card routines
 *
 * Copyright (C) 2018 The EmuTOS development team
 *
 * Authors:
 * Christian Zietz
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */
/* #define DEBUG_REGISTER_WRITES */

#include "config.h"
#include "portab.h"
#include "kprint.h"
#include "asm.h"
#include "delay.h"
#include "vectors.h"
#include "tosvars.h"
#include "lineavars.h"
#include "machine.h"
#include "nova.h"

#if CONF_WITH_NOVA

/* Base addresses of Nova registers and video memory.
   Autodetected by detect_nova().
*/
static volatile UBYTE* novaregbase;
static volatile UBYTE* novamembase;
int has_nova;

/* Macros for VGA register access */
#define VGAREG(x) (*(novaregbase+(x)))

#define CRTC_I  0x3D4   /* CRT Controller index and data ports */
#define CRTC_D  0x3D5
#define GDC_SEG 0x3CD   /* GDC segement select, index and data ports */
#define GDC_I   0x3CE
#define GDC_D   0x3CF
#define TS_I    0x3C4   /* Timing Sequencer index and data ports */
#define TS_D    0x3C5
#define VIDSUB  0x3C3   /* Video Subsystem register */
#define MISC_W  0x3C2   /* Misc Output Write Register */
#define DAC_PEL 0x3C6   /* RAMDAC pixel mask */
#define DAC_IW  0x3C8   /* RAMDAC write index */
#define DAC_D   0x3C9   /* RAMDAC palette data */
#define ATC_IW  0x3C0   /* Attribute controller: index and data write */
#define IS1_RC  0x3DA   /* Input Status Register 1: color emulation */

static ULONG delay20us;
#define SHORT_DELAY delay_loop(delay20us)
#define LONG_DELAY  delay_loop(loopcount_1_msec)


/* Functions for easier access of indexed registers */
static inline UBYTE get_idxreg(UWORD port, UBYTE reg) __attribute__((always_inline));
static inline void set_idxreg(UWORD port, UBYTE reg, UBYTE value) __attribute__((always_inline));
static void set_multiple_idxreg(UWORD port, UBYTE startreg, UBYTE cnt, const UBYTE *values);
static void set_multiple_atcreg(UBYTE startreg, UBYTE cnt, const UBYTE *values);

/* Register data for ET4000 init */
/* Timing Sequencer registers 1...4 */
static const UBYTE et4000_TS_1_4[] = {0x01,0x01,0x00,0x06};
/* Timing Sequencer registers 6...8 */
static const UBYTE et4000_TS_6_8[] = {0x00,0xF4,0x03};
/* Misc Output Write Register */
static const UBYTE et4000_MISC_W = 0x63; /* sync polarity: H-, V+ */
/* CRT Controller: registers 0 - 0x18 */
static const UBYTE et4000_CRTC_0_0x18[] = {0x5F,0x4F,0x50,0x82,
    0x54,0x80,0xBF,0x1F,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
    0x00,0x9C,0x0E,0x8F,0x28,0x00,0x96,0xB9,0xC3,0xFF};
/* CRT Controller: registers 0x33 - 0x35 */
static const UBYTE et4000_CRTC_0x33_0x35[] = {0x00,0x00,0x00};
/* Attribute Controller: registers 0 - 0x16 */
static const UBYTE et4000_ATC_0_0x16[] = {0x00,0x01,0x02,0x03,
     0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
     0x0F,0x01,0xFF,0x01,0x10,0x00,0x05,0x00};
static const UBYTE et4000_GDC_0_8[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x0F,0xFF};
static const UBYTE et4000_palette[] = {0xFF,0xFF,0xFF, 0,0,0, 0,0,0};

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
#ifdef DEBUG_REGISTER_WRITES
    UBYTE readback = VGAREG(port+1);
    if (readback != value)
        KDEBUG(("error %03x(%02x): %02x!=%02x\n", port, reg, readback, value));
#endif
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
    volatile UBYTE dummy;
    dummy = VGAREG(IS1_RC); /* set ATC_IW to index */

    for (; cnt>0; cnt--, startreg++, values++)
    {
        VGAREG(ATC_IW) = startreg;
        VGAREG(ATC_IW) = *values; /* yes, they really go to the same port */
    }
    VGAREG(ATC_IW) = 0x20; /* enable screen output */

    UNUSED(dummy);
}

/* Check for presence of a VGA card using the ATC palette registers */
static int check_for_vga(void)
{
    volatile UBYTE dummy;
    dummy = VGAREG(IS1_RC); /* set ATC_IW to index */

    SHORT_DELAY;
    VGAREG(ATC_IW) = 0x0A;
    SHORT_DELAY;
    VGAREG(ATC_IW) = 0x55;
    SHORT_DELAY;

    /* Reading the index/write register will return the register index */
    if (VGAREG(ATC_IW) == 0x0A)
    {
        VGAREG(ATC_IW) = 0x20; /* enable screen output */
        return 1;
    }
    else
    {
        return 0;
    }

    UNUSED(dummy);
}

/* Detect Nova addresses */
void detect_nova(void)
{
    has_nova = 0;

    if (IS_BUS32 && HAS_VME && check_read_byte(0xFE900000L+VIDSUB))
    {
        /* Mach32(?) in Atari TT */
        novaregbase = (UBYTE *)0xFE900000L;
        novamembase = (UBYTE *)0xFE800000L;
        has_nova = 1;
    }
    else if (HAS_VME && check_read_byte(0x00DC0000L+VIDSUB))
    {
        /* Nova in Atari MegaSTE */
        novaregbase = (UBYTE *)0x00DC0000L;
        novamembase = (UBYTE *)0x00C00000L;
        has_nova = 1;
    }
    else if (((long)phystop < 0x00C00000L) && check_read_byte(0x00CC0000L+VIDSUB))
    {
        /* Nova in Atari MegaST: be sure via phystop that it's not RAM we read */
        novaregbase = (UBYTE *)0x00CC0000L;
        novamembase = (UBYTE *)0x00C00000L;
        has_nova = 1;
    }
    else if (IS_BUS32 && HAS_VME && check_read_byte(0xFEDC0000L+VIDSUB))
    {
        /* ET4000 in Atari TT */
        novaregbase = (UBYTE *)0xFEDC0000L;
        novamembase = (UBYTE *)0xFEC00000L;
        has_nova = 1;
    }

    if (has_nova)
    {
        KDEBUG(("Nova detected at IO=%p / mem=%p\n", novaregbase, novamembase));
    }
}

/* Unlocks access to the extended registers of ET4000 */
static void unlock_et4000(void)
{
    VGAREG(0x3BF) = 0x03;
    SHORT_DELAY;
    VGAREG(0x3D8) = 0xA0;
}

/* Initializes card and memory access */
static void init_et4000_memory(void)
{
    KDEBUG(("init_et4000_memory()\n"));
    set_idxreg(CRTC_I, 0x34, 0x00); /* 6845 compatibility reg */
    set_idxreg(CRTC_I, 0x31, 0x00); /* W32: clock select */

    set_idxreg(TS_I, 0x02, 0x0F);   /* Write enable to all planes */
    set_idxreg(TS_I, 0x04, 0x0C);   /* Chain 4 mode */
    set_idxreg(GDC_I, 0x01, 0x00);  /* Disable set/reset mode */
    set_idxreg(GDC_I, 0x03, 0x00);  /* Function select = MOVE */
    set_idxreg(GDC_I, 0x06, 0x05);  /* Memory map: graphics mode enable */
    set_idxreg(GDC_I, 0x08, 0xFF);  /* Bit mask: pass all CPU bits */
    VGAREG(GDC_SEG) = 0x00;        /* Select segment 0, because of linear mode(?) */
    set_idxreg(CRTC_I, 0x32, 0x28); /* RAS/CAS timing */
    set_idxreg(CRTC_I, 0x36, 0xF3); /* Video System Conf. 1: linear memory access, 16 bit access */
    set_idxreg(CRTC_I, 0x37, 0x0F); /* Video System Conf. 2: DRAM, 32 bit wide */
}

/*
   Switches HiColor RAMDAC to palette mode.
   Note that we don't have to test whether this actually is a HiColor DAC at all.
   With a non-HiColor DAC this only overwrites the pixel mask register that will be reset later, anyway.
*/
static void ramdac_hicolor_off(void)
{
    volatile UBYTE dummy;

    /* Switch DAC to command mode, if supported */
    dummy = VGAREG(DAC_IW);
    SHORT_DELAY;
    dummy = VGAREG(DAC_PEL);
    SHORT_DELAY;
    dummy = VGAREG(DAC_PEL);
    SHORT_DELAY;
    dummy = VGAREG(DAC_PEL);
    SHORT_DELAY;
    dummy = VGAREG(DAC_PEL);
    SHORT_DELAY;

    VGAREG(DAC_PEL) = 0x00;  /* HiColor off */
    dummy = VGAREG(DAC_IW);  /* Back to normal mode */

    UNUSED(dummy);
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
    VGAREG(DAC_IW) = 255; /* color 0 */
    for (k=0; k<3; k++)
    {
        VGAREG(DAC_D) = *palette++;
        SHORT_DELAY;
    }
}


/* Loads predefined values to all relevant VGA registers */
static void init_et4000_resolution(void)
{
    UBYTE temp;

    KDEBUG(("init_et4000_resolution()\n"));
    ramdac_hicolor_off();

    /* Load registers */

    /* Reset Timing Sequencer */
    set_idxreg(TS_I, 0x00, 0x01);
    LONG_DELAY;
    set_idxreg(TS_I, 0x00, 0x03);

    unlock_et4000(); /* TODO: really required again? */

    set_multiple_idxreg(TS_I, 1, sizeof(et4000_TS_1_4), et4000_TS_1_4);
    set_multiple_idxreg(TS_I, 6, sizeof(et4000_TS_6_8), et4000_TS_6_8);

    VGAREG(MISC_W) = et4000_MISC_W;

    set_idxreg(CRTC_I, 0x11, 0); /* enable write to CRTC */
    set_multiple_idxreg(CRTC_I, 0, sizeof(et4000_CRTC_0_0x18), et4000_CRTC_0_0x18);
    set_multiple_idxreg(CRTC_I, 0x33, sizeof(et4000_CRTC_0x33_0x35), et4000_CRTC_0x33_0x35);
    temp = get_idxreg(CRTC_I, 0x11);
    set_idxreg(CRTC_I, 0x11, temp | 0x80); /* disable write to CRTC */

    set_multiple_atcreg(0, sizeof(et4000_ATC_0_0x16), et4000_ATC_0_0x16);
    set_multiple_idxreg(GDC_I, 0, sizeof(et4000_GDC_0_8), et4000_GDC_0_8);

    set_idxreg(CRTC_I, 0x36, 0x53);
    set_idxreg(TS_I, 1, et4000_TS_1_4[0] | 0x20); /* screen off */
    set_palette_entries(et4000_palette);
    set_idxreg(TS_I, 1, et4000_TS_1_4[0]); /* screen on */
}

/* Certain ET4000 graphic cards require a different clock divider.
   There is no direct way of finding out the clock the ET4000 runs on.
   Instead, this function counts the number of VBLs for half a second.
*/
#define VBL_TIMEOUT 100 /* 0.5 second */
#define VBL_LIMIT   30  /* nominally: 70 Hz, i.e. 35 VBLs in 0.5s */
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
static int test_video_memory(void)
{
    /* Note that novamembase is declared volatile,
       so the compiler won't optimize this out. */
    *novamembase = 0x00;
    if (*novamembase != 0x00)
        return 0;

    *novamembase = 0x55;
    if (*novamembase != 0x55)
        return 0;

    return 1;
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
    V_REZ_VT = 400;
    /* Horizontal resolution */
    V_REZ_HZ = 640;
}

/* Initialize Nova card */
int init_nova(void)
{
    delay20us = loopcount_1_msec / 50;

    /* Fail if detect_nova() hasn't found card */
    if (!has_nova)
        return 0;

    /* Enables VGA mode and selects MCLK 1 */
    VGAREG(VIDSUB) = 0x01;
    VGAREG(MISC_W) = 0xE3;
    
    /* Sanity check that no other VME or Megabus HW has been detected. 
     * Note that we can do this only after enabling VGA in the lines above. 
     */
    if (!check_for_vga()) {
        KDEBUG(("No Nova or no VGA card found\n"));
        return 0;
    }

    unlock_et4000();
    init_et4000_memory();
    init_et4000_resolution();
    count_vbls();

    if (!test_video_memory()) {
        KDEBUG(("Nova memory inaccessible\n"));
        /* TODO: Try alternative address, like driver does */
        return 0;
    }

    init_system_vars();

    return 1;
}

#endif /* CONF_WITH_NOVA */
