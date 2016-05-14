/*
 * amiga.c - Amiga specific functions
 *
 * Copyright (c) 2013-2016 The EmuTOS development team
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

#if CONF_WITH_AROS
#include "aros.h"
#endif

#ifdef MACHINE_AMIGA

/* DMA register bits */
#define DMAF_SETCLR             0x8000
#define DMAF_COPPER             0x0080
#define DMAF_RASTER             0x0100
#define DMAF_MASTER             0x0200

#define JOY0DAT *(volatile UWORD*)0xdff00a
#define CIAAPRA *(volatile UBYTE*)0xbfe001
#define POTGO *(volatile UWORD*)0xdff034
#define POTGOR *(volatile UWORD*)0xdff016 // = POTINP

/******************************************************************************/
/* Machine detection                                                          */
/******************************************************************************/

int has_gayle;

void amiga_machine_detect(void)
{
#if CONF_WITH_AROS
    aros_machine_detect();
#else
    has_gayle = (mcpu >= 20);
#endif
    KDEBUG(("has_gayle = %d\n", has_gayle));
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

const UBYTE *amiga_screenbase;
UWORD copper_list[6];

void amiga_screen_init(void)
{
    sshiftmod = 0x02; // We emulate the ST-High monochrome video mode

    *(volatile UWORD*)0xdff100 = 0x9204; // Hires, one bit-plane, interlaced
    *(volatile UWORD*)0xdff102 = 0; // Horizontal scroll value 0
    *(volatile UWORD*)0xdff108 = 80; // Modulo = 80 for odd bit-planes
    *(volatile UWORD*)0xdff10a = 80; // Ditto for even bit-planes
    *(volatile UWORD*)0xdff092 = 0x003c; // Set data-fetch start for hires
    *(volatile UWORD*)0xdff094 = 0x00d4; // Set data-fetch stop
    *(volatile UWORD*)0xdff08e = 0x2c81; // Set display window start
    *(volatile UWORD*)0xdff090 = 0xf4c1; // Set display window stop

    // Set up color registers
    *(volatile UWORD*)0xdff180 = 0x0fff; // Background color = white
    *(volatile UWORD*)0xdff182 = 0x0000; // Foreground color = black

    // Set up the Copper list (must be in ST-RAM)
    copper_list[0] = 0x0e0; // BPL1PTH
    copper_list[1] = ((ULONG)amiga_screenbase & 0xffff0000) >> 16;
    copper_list[2] = 0x0e2; // BPL1PTL
    copper_list[3] = ((ULONG)amiga_screenbase & 0x0000ffff);
    copper_list[4] = 0xffff; // End of
    copper_list[5] = 0xfffe; // Copper list

    // Initialize the Copper
    *(UWORD* volatile *)0xdff080 = copper_list; // COP1LCH
    *(volatile UWORD*)0xdff088 = 0; // COPJMP1

    // VBL interrupt
    VEC_LEVEL3 = amiga_vbl;
    *(volatile UWORD*)0xdff09a = 0xc020; // INTENA Set Master and VBL bits

    // Start the DMA
    *(volatile UWORD*)0xdff096 = DMAF_SETCLR | DMAF_COPPER | DMAF_RASTER | DMAF_MASTER;
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
typedef ULONG uaelib_demux_t(ULONG fnum, ...);
static uaelib_demux_t* uaelib_demux = NULL;

#define has_uaelib (uaelib_demux != NULL)

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
    if (!has_uaelib)
        return;

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
