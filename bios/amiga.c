/*
 * amiga.c - Amiga specific functions
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "kprint.h"
#include "amiga.h"
#include "vectors.h"
#include "tosvars.h"
#include "processor.h"
#include "gemerror.h"
#include "ikbd.h"               /* for call_mousevec() */
#include "screen.h"
#include "videl.h"

#if CONF_WITH_AROS
#include "aros.h"
#endif

#ifdef MACHINE_AMIGA

/* Custom registers */
#define JOY0DAT *(volatile UWORD*)0xdff00a
#define ADKCONR *(volatile UWORD*)0xdff010
#define POTGOR  *(volatile UWORD*)0xdff016 /* = POTINP */
#define DSKBYTR *(volatile UWORD*)0xdff01a
#define INTENAR *(volatile UWORD*)0xdff01c
#define INTREQR *(volatile UWORD*)0xdff01e
#define DSKPTH  *(void* volatile*)0xdff020
#define DSKLEN  *(volatile UWORD*)0xdff024
#define POTGO   *(volatile UWORD*)0xdff034
#define DSKSYNC *(volatile UWORD*)0xdff07e
#define COP1LCH *(UWORD* volatile*)0xdff080
#define COPJMP1 *(volatile UWORD*)0xdff088
#define DIWSTRT *(volatile UWORD*)0xdff08e
#define DIWSTOP *(volatile UWORD*)0xdff090
#define DDFSTRT *(volatile UWORD*)0xdff092
#define DDFSTOP *(volatile UWORD*)0xdff094
#define DMACON  *(volatile UWORD*)0xdff096
#define INTENA  *(volatile UWORD*)0xdff09a
#define INTREQ  *(volatile UWORD*)0xdff09c
#define ADKCON  *(volatile UWORD*)0xdff09e
#define BPLCON0 *(volatile UWORD*)0xdff100
#define BPLCON1 *(volatile UWORD*)0xdff102
#define BPL1MOD *(volatile UWORD*)0xdff108
#define COLOR00 *(volatile UWORD*)0xdff180
#define COLOR01 *(volatile UWORD*)0xdff182

/* CIA A registers */
#define CIAAPRA    *(volatile UBYTE*)0xbfe001
#define CIAAPRB    *(volatile UBYTE*)0xbfe101
#define CIAADDRA   *(volatile UBYTE*)0xbfe201
#define CIAADDRB   *(volatile UBYTE*)0xbfe301
#define CIAATALO   *(volatile UBYTE*)0xbfe401
#define CIAATAHI   *(volatile UBYTE*)0xbfe501
#define CIAATBLO   *(volatile UBYTE*)0xbfe601
#define CIAATBHI   *(volatile UBYTE*)0xbfe701
#define CIAATODLO  *(volatile UBYTE*)0xbfe801
#define CIAATODMID *(volatile UBYTE*)0xbfe901
#define CIAATODHI  *(volatile UBYTE*)0xbfea01
#define CIAASDR    *(volatile UBYTE*)0xbfec01
#define CIAAICR    *(volatile UBYTE*)0xbfed01
#define CIAACRA    *(volatile UBYTE*)0xbfee01
#define CIAACRB    *(volatile UBYTE*)0xbfef01

/* CIA B registers */
#define CIABPRA    *(volatile UBYTE*)0xbfd000
#define CIABPRB    *(volatile UBYTE*)0xbfd100
#define CIABDDRA   *(volatile UBYTE*)0xbfd200
#define CIABDDRB   *(volatile UBYTE*)0xbfd300
#define CIABTALO   *(volatile UBYTE*)0xbfd400
#define CIABTAHI   *(volatile UBYTE*)0xbfd500
#define CIABTBLO   *(volatile UBYTE*)0xbfd600
#define CIABTBHI   *(volatile UBYTE*)0xbfd700
#define CIABTODLO  *(volatile UBYTE*)0xbfd800
#define CIABTODMID *(volatile UBYTE*)0xbfd900
#define CIABTODHI  *(volatile UBYTE*)0xbfda00
#define CIABSDR    *(volatile UBYTE*)0xbfdc00
#define CIABICR    *(volatile UBYTE*)0xbfdd00
#define CIABCRA    *(volatile UBYTE*)0xbfde00
#define CIABCRB    *(volatile UBYTE*)0xbfdf00

/* Gayle registers */
#define GAYLE_ID *(volatile BYTE*)0xde1000
#define INTENA_MIRROR *(volatile UWORD*)0xde109a
#define FAT_GARY_TIMEOUT *(volatile BYTE*)0xde0000

/* Generic Set/Clear bit */
#define SETBITS  (1U << 15)
#define CLRBITS  (0U << 15)

/* INTREQ / INTENA flags */
#define INTEN    (1U << 14)
#define EXTER    (1U << 13)
#define DSKSYN   (1U << 12)
#define RBF      (1U << 11)
#define AUD3     (1U << 10)
#define AUD2     (1U << 9)
#define AUD1     (1U << 8)
#define AUD0     (1U << 7)
#define BLIT     (1U << 6)
#define VERTB    (1U << 5)
#define PORTS    (1U << 3)
#define SOFT     (1U << 2)
#define DSKBLK   (1U << 1)
#define TBE      (1U << 0)

/* DMACON bits */
#define BBUSY    (1U << 14)
#define BZERO    (1U << 13)
#define BLTPRI   (1U << 10)
#define DMAEN    (1U << 9)
#define BPLEN    (1U << 8)
#define COPEN    (1U << 7)
#define BLTEN    (1U << 6)
#define SPREN    (1U << 5)
#define DSKEN    (1U << 4)
#define AUD3EN   (1U << 3)
#define AUD2EN   (1U << 2)
#define AUD1EN   (1U << 1)
#define AUD0EN   (1U << 0)

/******************************************************************************/
/* Machine detection                                                          */
/******************************************************************************/

int has_gayle;

/* Detect A600 / A1200 Gayle chip.
 * Freely inspired from AROS ReadGayle().
 */
static void detect_gayle(void)
{
    UWORD save_intena;
    UBYTE gayle_id;
    int i;

    has_gayle = 0;

    /* Check if 0xde1000 is a mirror of 0xdff000 custom chips */
    save_intena = INTENAR;
    INTENA_MIRROR = ~SETBITS; /* Disable interrupts using mirror */
    if (INTENAR == 0)
    {
        /* Interrupts have been disabled. Maybe mirror of INTENA. */
        INTENA_MIRROR = SETBITS | TBE; /* Enable TBE interrupt */
        if (INTENAR != 0)
        {
            /* Interrupt was enabled. This is an INTENA mirror. */
            /* Restore interrupts */
            INTENA = ~SETBITS;
            INTENA = SETBITS | save_intena;

            /* So this is not a Gayle */
            return;
        }
    }

    /* On A300, we must clear the Fat Gary Timeout register
     * to avoid reading a bogus 0x80 Gayle ID */
    UNUSED(FAT_GARY_TIMEOUT);

    gayle_id = 0;
    GAYLE_ID = 0; /* Reset ID register */
    for (i = 0; i < 8; i++)
    {
        gayle_id <<= 1;
        gayle_id |= (GAYLE_ID & 0x80) ? 1 : 0;
    }

    if (gayle_id == 0 || gayle_id == 0xff)
    {
        /* Bogus ID */
        return;
    }

    /* Gayle ID should be 0xd0 on ECS, or 0xd1 on AGA */
    KDEBUG(("gayle_id = 0x%02x\n", gayle_id));

    has_gayle = 1;
}

void amiga_machine_detect(void)
{
    detect_gayle();
    KDEBUG(("has_gayle = %d\n", has_gayle));

#if CONF_WITH_AROS
    aros_machine_detect();
#endif
}

#if CONF_WITH_ALT_RAM

/******************************************************************************/
/* Alternate RAM                                                              */
/******************************************************************************/

void amiga_add_alt_ram(void)
{
#if CONF_WITH_AROS
    aros_add_alt_ram();
#endif
}

#endif /* CONF_WITH_ALT_RAM */

/******************************************************************************/
/* Screen                                                                     */
/******************************************************************************/

UWORD amiga_screen_width;
UWORD amiga_screen_width_in_bytes;
UWORD amiga_screen_height;
const UBYTE *amiga_screenbase;
UWORD copper_list[8];

ULONG amiga_initial_vram_size(void)
{
    return 640UL * 512 / 8;
}

static void amiga_set_videomode(UWORD width, UWORD height)
{
    UWORD lowres_height = height;
    UWORD bplcon0 = 0x1200; /* 1 bit-plane, COLOR ON */
    UWORD bpl1mod = 0; /* Modulo */
    UWORD ddfstrt = 0x0038; /* Data-fetch start for low resolution */
    UWORD ddfstop = 0x00d0; /* Data-fetch stop for low resolution */
    UWORD hstart = 0x81; /* Display window horizontal start */
    UWORD hstop = 0xc1; /* Display window horizontal stop */
    UWORD vstart; /* Display window vertical start */
    UWORD vstop; /* Display window vertical stop */
    UWORD diwstrt; /* Display window start */
    UWORD diwstop; /* Display window stop */

    amiga_screen_width = width;
    amiga_screen_width_in_bytes = width / 8;
    amiga_screen_height = height;

    if (width >= 640)
    {
        bplcon0 |= 0x8000; /* HIRES */
        ddfstrt = 0x003c;
        ddfstop = 0x00d4;
    }

    if (height >= 400)
    {
        bplcon0 |= 0x0004; /* LACE */
        bpl1mod = amiga_screen_width_in_bytes;
        lowres_height = height / 2;
    }

    vstart = 44 + (256 / 2) - (lowres_height / 2);
    vstop = vstart + lowres_height;
    vstop = (UBYTE)(((WORD)vstop) - 0x100); /* Normalize value */

    diwstrt = (vstart << 8) | hstart;
    diwstop = (vstop << 8) | hstop;

    KINFO(("BPLCON0 = 0x%04x\n", bplcon0));
    KINFO(("BPL1MOD = 0x%04x\n", bpl1mod));
    KINFO(("DDFSTRT = 0x%04x\n", ddfstrt));
    KINFO(("DDFSTOP = 0x%04x\n", ddfstop));
    KINFO(("DIWSTRT = 0x%04x\n", diwstrt));
    KINFO(("DIWSTOP = 0x%04x\n", diwstop));

    BPLCON0 = bplcon0; /* Bit Plane Control */
    BPLCON1 = 0;       /* Horizontal scroll value 0 */
    BPL1MOD = bpl1mod; /* Modulo = line width in interlaced mode */
    DDFSTRT = ddfstrt; /* Data-fetch start */
    DDFSTOP = ddfstop; /* Data-fetch stop */
    DIWSTRT = diwstrt; /* Set display window start */
    DIWSTOP = diwstop; /* Set display window stop */

    /* Set up color registers */
    COLOR00 = 0x0fff; /* Background color = white */
    COLOR01 = 0x0000; /* Foreground color = black */

    if (width == 640 && height == 400)
        sshiftmod = ST_HIGH;
    else
        sshiftmod = FALCON_REZ;
}

WORD amiga_check_moderez(WORD moderez)
{
    WORD current_mode, return_mode;

    if (moderez == 0xff02) /* ST High */
        moderez = VIDEL_COMPAT|VIDEL_1BPP|VIDEL_80COL|VIDEL_VERTICAL;

    if (moderez < 0)                /* ignore other ST video modes */
        return 0;

    current_mode = amiga_vgetmode();
    return_mode = moderez;          /* assume always valid */
    return (return_mode==current_mode)?0:return_mode;
}

void amiga_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez)
{
    *planes = 1;
    *hz_rez = amiga_screen_width;
    *vt_rez = amiga_screen_height;
}

void amiga_screen_init(void)
{
    amiga_set_videomode(640, 400);

    /* The VBL will update the Copper list below with any new value
     * of amiga_screenbase, eventually adjusted for interlace.
     * It is *mandatory* to reset BPL1PTH/BPL1PTL on each VBL.
     * It could have been done manually in the VBL interrupt handler, but in
     * that case the display would be wrong when interrupts are turned off.
     * On the other hand, with a Copper list, the bitplane pointer is always
     * reset correctly, even if the CPU interrupts are turned off.
     * The only remaining issue is that the interlaced fields are not
     * properly switched when interrupts are turned off. It is a minor issue,
     * as this should normally not happen for a long time. And even in that
     * case, displayed texts are still readable, even if a bit distorted.
     * The Copper list waits a few lines at the top to give the VBL interrupt
     * handler enough time to update it.
     */

    /* Set up the Copper list (must be in ST-RAM) */
    copper_list[0] = 0x0a01; /* Wait line 10 to give time to the VBL routine */
    copper_list[1] = 0xff00; /* Vertical wait only */
    copper_list[2] = 0x0e0; /* BPL1PTH */
    copper_list[3] = ((ULONG)amiga_screenbase & 0xffff0000) >> 16;
    copper_list[4] = 0x0e2; /* BPL1PTL */
    copper_list[5] = ((ULONG)amiga_screenbase & 0x0000ffff);
    copper_list[6] = 0xffff; /* End of      */
    copper_list[7] = 0xfffe; /* Copper list */

    /* Initialize the Copper */
    COP1LCH = copper_list;
    COPJMP1 = 0;

    /* Enable VBL interrupt */
    VEC_LEVEL3 = amiga_vbl;
    INTENA = SETBITS | INTEN | VERTB;

    /* Start the DMA, with bit plane and Copper */
    DMACON = SETBITS | COPEN | BPLEN | DMAEN;
}

void amiga_setphys(const UBYTE *addr)
{
    KDEBUG(("amiga_setphys(0x%08lx)\n", (ULONG)addr));
    amiga_screenbase = addr;
}

const UBYTE *amiga_physbase(void)
{
    return amiga_screenbase;
}

WORD amiga_setcolor(WORD colorNum, WORD color)
{
    KDEBUG(("amiga_setcolor(%d, 0x%04x)\n", colorNum, color));

    if (colorNum == 0)
        return 0x777;
    else
        return 0x000;
}

void amiga_setrez(WORD rez, WORD videlmode)
{
    UWORD width, height;

    /* Currently, we only support monochrome video modes */
    if ((videlmode & VIDEL_BPPMASK) != VIDEL_1BPP)
        return;

    width = (videlmode & VIDEL_80COL) ? 640 : 320;

    if (videlmode & VIDEL_VGA)
        height = (videlmode & VIDEL_VERTICAL) ? 240 : 480;
    else if (videlmode & VIDEL_PAL)
        height = (videlmode & VIDEL_VERTICAL) ? 512 : 256;
    else
        height = (videlmode & VIDEL_VERTICAL) ? 400 : 200;

    amiga_set_videomode(width, height);
}

WORD amiga_vgetmode(void)
{
    WORD mode = VIDEL_1BPP;

    if (amiga_screen_width >= 640)
        mode |= VIDEL_80COL;

    if (amiga_screen_height == 240)
        mode |= VIDEL_VGA | VIDEL_VERTICAL;
    else if (amiga_screen_height == 480)
        mode |= VIDEL_VGA;
    else if (amiga_screen_height == 512)
        mode |= VIDEL_PAL | VIDEL_VERTICAL;
    else if (amiga_screen_height == 256)
        mode |= VIDEL_PAL;
    else if (amiga_screen_height == 400)
        mode |= VIDEL_VERTICAL;

    if (amiga_screen_width == 640 && amiga_screen_height == 400)
        mode |= VIDEL_COMPAT;

    return mode;
}

/******************************************************************************/
/* Keyboard                                                                   */
/******************************************************************************/

/* This scancode table will be used by amiga_int_ciaa_serial */
const UBYTE scancode_atari_from_amiga[128] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x29, 0x00, 0x71,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x00, 0x6d, 0x6e, 0x6f,
    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x2b, 0x00, 0x6a, 0x6b, 0x6c,
    0x60, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32,
    0x33, 0x34, 0x35, 0x00, 0x00, 0x67, 0x68, 0x69,
    0x39, 0x0e, 0x0f, 0x72, 0x1c, 0x01, 0x62, 0x00,
    0x00, 0x00, 0x4a, 0x00, 0x48, 0x50, 0x4d, 0x4b,
    0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42,
    0x43, 0x44, 0x63, 0x64, 0x65, 0x66, 0x4e, 0x61,
    0x2a, 0x36, 0x3a, 0x1d, 0x38, 0x38, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/******************************************************************************/
/* Mouse                                                                      */
/******************************************************************************/

static BYTE oldMouseX;
static BYTE oldMouseY;
static BOOL oldButton1;
static BOOL oldButton2;
static BOOL oldMouseSet;

void amiga_mouse_vbl(void)
{
    UWORD data = JOY0DAT;
    BYTE mouseX = (data & 0x00ff);
    BYTE mouseY = (data & 0xff00) >> 8;
    BOOL button1 = (CIAAPRA & 0x40) == 0;
    BOOL button2 = (POTGOR & 0x0400) == 0;

    if (!oldMouseSet)
    {
        POTGO = POTGOR | 0x0500;
    }
    else if (mouseX != oldMouseX
       || mouseY != oldMouseY
       || button1 != oldButton1
       || button2 != oldButton2)
    {
        UBYTE packet[3];
        packet[0] = 0xf8;

        if (button1)
            packet[0] |= 0x02;

        if (button2)
            packet[0] |= 0x01;

        packet[1] = mouseX - oldMouseX;
        packet[2] = mouseY - oldMouseY;

        call_mousevec(packet);
    }

    oldMouseX = mouseX;
    oldMouseY = mouseY;
    oldButton1 = button1;
    oldButton2 = button2;
    oldMouseSet = TRUE;
}

/******************************************************************************/
/* Clock                                                                      */
/******************************************************************************/

/* Date/Time to use when the hardware clock is not set.
 * We use the OS creation date at 00:00:00 */
#define DEFAULT_DATETIME ((ULONG)os_dosdate << 16)

#define BATTCLOCK ((volatile UBYTE*)0x00dc0000)

#define AMIGA_CLOCK_NONE 0
#define AMIGA_CLOCK_MSM6242B 1
#define AMIGA_CLOCK_RF5C01A 2

static int amiga_clock_type;

static UBYTE read_clock_reg(int reg)
{
    return BATTCLOCK[reg * 4 + 3] & 0x0f;
}

static void write_clock_reg(UBYTE reg, UBYTE val)
{
    BATTCLOCK[reg * 4 + 3] = val;
}

static UBYTE read_clock_bcd(int reg)
{
    return read_clock_reg(reg + 1) * 10 + read_clock_reg(reg);
}

void amiga_clock_init(void)
{
    KDEBUG(("d = %d, e = %d, f = %d\n", read_clock_reg(0xd), read_clock_reg(0xe), read_clock_reg(0xf)));

    if (read_clock_reg(0xf) == 4)
    {
        amiga_clock_type = AMIGA_CLOCK_MSM6242B;
    }
    else
    {
        UBYTE before, test, after;

        before = read_clock_reg(0xd);

        /* Write a value */
        test = before | 0x1;
        write_clock_reg(0xd, test);

        /* If the value is still here, there is a register */
        after = read_clock_reg(0xd);
        if (after == test)
            amiga_clock_type = AMIGA_CLOCK_RF5C01A;
        else
            amiga_clock_type = AMIGA_CLOCK_NONE;

        write_clock_reg(0xd, before);
    }

    KDEBUG(("amiga_clock_type = %d\n", amiga_clock_type));
}

static UWORD amiga_dogettime(void)
{
    UWORD seconds = read_clock_bcd(0);
    UWORD minutes = read_clock_bcd(2);
    UWORD hours = read_clock_bcd(4);
    UWORD time;

    KDEBUG(("amiga_dogettime() %02d:%02d:%02d\n", hours, minutes, seconds));

    time = (seconds >> 1)
         | (minutes << 5)
         | (hours << 11);

    return time;
}

static UWORD amiga_dogetdate(void)
{
    UWORD offset = (amiga_clock_type == AMIGA_CLOCK_RF5C01A) ? 1 : 0;
    UWORD days = read_clock_bcd(offset + 6);
    UWORD months = read_clock_bcd(offset + 8);
    UWORD years = read_clock_bcd(offset + 10);
    UWORD date;

    KDEBUG(("amiga_dogetdate() %02d/%02d/%02d\n", years, months, days));

    /* Y2K fix */
    if (years >= 80)
        years += 1900;
    else
        years += 2000;

    date = (days & 0x1F)
        | ((months & 0xF) << 5)
        | ((years - 1980) << 9);

    return date;
}

ULONG amiga_getdt(void)
{
    if (amiga_clock_type == AMIGA_CLOCK_NONE)
        return DEFAULT_DATETIME;

    return (((ULONG) amiga_dogetdate()) << 16) | amiga_dogettime();
}

#if CONF_WITH_UAE

/******************************************************************************/
/* uaelib: special functions provided by the UAE emulator                     */
/******************************************************************************/

/* Location of the UAE Boot ROM, a.k.a. RTAREA */
#define RTAREA_DEFAULT 0x00f00000
#define RTAREA_BACKUP  0x00ef0000

/* uaelib_demux() is the entry point */
#define UAELIB_DEMUX_OFFSET 0xFF60
uaelib_demux_t* uaelib_demux = NULL;

static ULONG uaelib_GetVersion(void);

#define IS_TRAP(x)(((*(ULONG*)(x)) & 0xf000ffff) == 0xa0004e75)

void amiga_uaelib_init(void)
{
    MAYBE_UNUSED(uaelib_GetVersion);

    if (IS_TRAP(RTAREA_DEFAULT + UAELIB_DEMUX_OFFSET))
        uaelib_demux = (uaelib_demux_t*)(RTAREA_DEFAULT + UAELIB_DEMUX_OFFSET);
    else if (IS_TRAP(RTAREA_BACKUP + UAELIB_DEMUX_OFFSET))
        uaelib_demux = (uaelib_demux_t*)(RTAREA_BACKUP + UAELIB_DEMUX_OFFSET);

#ifdef ENABLE_KDEBUG
    if (has_uaelib)
    {
        ULONG version = uaelib_GetVersion();
        KDEBUG(("EmuTOS running on UAE version %d.%d.%d\n",
            (int)((version & 0xff000000) >> 24),
            (int)((version & 0x00ff0000) >> 16),
            (int)(version & 0x0000ffff)));
    }
#endif
}

/* Get UAE version */
static ULONG uaelib_GetVersion(void)
{
    return uaelib_demux(0);
}

/* Write a string prefixed by DBG: to the UAE debug log
 * The final \n will automatically be appended */
static ULONG uaelib_DbgPuts(const char* str)
{
    return uaelib_demux(86, str);
}

/* Exit UAE */
static ULONG uaelib_ExitEmu(void)
{
    return uaelib_demux(13);
}

/******************************************************************************/
/* kprintf()                                                                  */
/******************************************************************************/

#define UAE_MAX_DEBUG_LENGTH 255

static char uae_debug_string[UAE_MAX_DEBUG_LENGTH + 1];

/* The only available output function is uaelib_DbgPuts(),
 * so we have to buffer the string until until \n */
void kprintf_outc_uae(int c)
{
    if (c == '\n')
    {
        /* Output the current string then clear the buffer */
        uaelib_DbgPuts(uae_debug_string);
        uae_debug_string[0] = '\0';
    }
    else
    {
        char* p;

        /* Append the character to the buffer */
        for (p = uae_debug_string; *p; ++p);
        if ((p - uae_debug_string) < UAE_MAX_DEBUG_LENGTH)
        {
            *p++ = (char)c;
            *p = '\0';
        }
    }
}

#endif /* CONF_WITH_UAE */

/******************************************************************************/
/* Shutdown                                                                   */
/******************************************************************************/

void amiga_shutdown(void)
{
#if CONF_WITH_UAE
    if (!has_uaelib)
        return;

    uaelib_ExitEmu();
#endif
}

BOOL amiga_can_shutdown(void)
{
#if CONF_WITH_UAE
    return has_uaelib;
#else
    return FALSE;
#endif
}

/******************************************************************************/
/* Floppy                                                                     */
/******************************************************************************/

BOOL amiga_flop_detect_drive(WORD dev)
{
#if CONF_WITH_AROS
    return aros_flop_detect_drive(dev);
#else
    return FALSE;
#endif
}

WORD amiga_floprw(UBYTE *buf, WORD rw, WORD dev, WORD sect, WORD track, WORD side, WORD count)
{
#if CONF_WITH_AROS
    return aros_floprw(buf, rw, dev, sect, track, side, count);
#else
    return EUNDEV;
#endif
}

#endif /* MACHINE_AMIGA */
