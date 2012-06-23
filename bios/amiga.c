/*
 * amiga.c - Amiga specific functions
 *
 * Copyright (c) 2012 EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "amiga.h"
#include "vectors.h"
#include "tosvars.h"

#ifdef MACHINE_AMIGA

/*
 * DMA register bits
 */
#define DMAF_SETCLR             0x8000
#define DMAF_AUD0               0x0001
#define DMAF_AUD1               0x0002
#define DMAF_AUD2               0x0004
#define DMAF_AUD3               0x0008
#define DMAF_DISK               0x0010
#define DMAF_SPRITE             0x0020
#define DMAF_BLITTER            0x0040
#define DMAF_COPPER             0x0080
#define DMAF_RASTER             0x0100
#define DMAF_MASTER             0x0200
#define DMAF_BLITHOG            0x0400
#define DMAF_BLTNZERO           0x2000
#define DMAF_BLTDONE            0x4000
#define DMAF_ALL                0x01ff

#define JOY0DAT *(volatile UWORD*)0xdff00a
#define CIAAPRA *(volatile UBYTE*)0xbfe001
#define POTGO *(volatile UWORD*)0xdff034
#define POTGOR *(volatile UWORD*)0xdff016 // = POTINP

#if CONF_WITH_ALT_RAM
extern long xmaddalt(long start, long size); /* found in bdos/mem.h */

void amiga_add_alt_ram(void)
{
    /* Slow RAM */
    /* FIXME: Detect the size */
    //xmaddalt(0x00c00000, 512*1024L);

    /* Fast RAM */
    /* FIXME: Maybe enable Zorro 2, and detect the size */
    //xmaddalt(0x00200000, 512*1024L);
}

#endif /* CONF_WITH_ALT_RAM */

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
    copper_list[1] = (((ULONG)v_bas_ad) & 0xffff0000) >> 16;
    copper_list[2] = 0x0e2; // BPL1PTL
    copper_list[3] = (((ULONG)v_bas_ad) & 0x0000ffff);
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
        BYTE packet[3];
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

#endif /* MACHINE_AMIGA */
