/*
 * screen.c - low-level screen routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  THO   Thomas Huth
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "machine.h"
#include "screen.h"
#include "asm.h"

/* private prototypes */

static void setphys(LONG addr);

/* determine monitor type, ... */

/* Define pallette */

static WORD dflt_palette[] = {
    RGB_WHITE, RGB_RED, RGB_GREEN, RGB_YELLOW,
    RGB_BLUE, RGB_MAGENTA, RGB_CYAN, RGB_LTGRAY,
    RGB_GRAY, RGB_LTRED, RGB_LTGREEN, RGB_LTYELLOW,
    RGB_LTBLUE, RGB_LTMAGENTA, RGB_LTCYAN, RGB_BLACK
};



/*
 * In the original TOS there used to be an early screen init, 
 * before memory configuration. This is not used here, and all is
 * done at the same time from C.
 */

void screen_init(void)
{
    volatile BYTE *rez_reg = (BYTE *) 0xffff8260;
    volatile WORD *col_regs = (WORD *) 0xffff8240;
    volatile struct {
        BYTE gpip;
    } *mfp = (void *) 0xfffffa01;
    WORD rez;
    WORD i;

    *(BYTE *) 0xffff820a = 2;   /* sync-mode to 50 hz pal, internal sync */

    for (i = 0; i < 16; i++) {
        col_regs[i] = dflt_palette[i];
    }

    rez = *rez_reg;

    rez &= 3;
    if (rez == 3) {
        rez = 2;
    }
    if ((mfp->gpip & 0x80) != 0) {
        /* color monitor */
        if (rez == 2)
            rez = 0;
    } else {
        if (rez < 2)
            rez = 2;
    }

    *rez_reg = rez;
    sshiftmod = rez;

    if (rez == 1) {
        col_regs[3] = col_regs[15];
    }

    v_bas_ad = (BYTE *) (phystop - 0x8000L);
    setphys((LONG) v_bas_ad);
}

/* misc routines */

static void setphys(LONG addr)
{
    *(UBYTE *) 0xffff8201 = ((ULONG) addr) >> 16;
    *(UBYTE *) 0xffff8203 = ((ULONG) addr) >> 8;
    if (has_ste_shifter) {
        *(UBYTE *) 0xffff820d = ((ULONG) addr);
    }
}

/* xbios routines */

LONG physbase(void)
{
    LONG addr;

    addr = *(UBYTE *) 0xffff8201;
    addr <<= 8;
    addr += *(UBYTE *) 0xffff8203;
    addr <<= 8;
    if (has_ste_shifter) {
        addr += *(UBYTE *) 0xffff820D;
    }

    return (addr);
}

LONG logbase(void)
{
    return ((ULONG) v_bas_ad);
}

WORD getrez(void)
{
    return (*(UBYTE *) 0xffff8260);
}


void setscreen(LONG logLoc, LONG physLoc, WORD rez)
{
    if (logLoc >= 0) {
        v_bas_ad = (char *) logLoc;
    }
    if (physLoc >= 0) {
        screenpt = (char *) physLoc;
        /* will be set up at next VBL */
    }
    if (rez >= 0) {
        /* rez ignored for now */
    }
}

void setpalette(LONG palettePtr)
{
    /* next VBL will do this */
    colorptr = (WORD *) palettePtr;
}

WORD setcolor(WORD colorNum, WORD color)
{
    WORD rez = getrez();
    WORD max;
    WORD mask;
    WORD *palette = (WORD *) 0xffff8240;
    switch (rez) {
    case 0:
        max = 15;
        break;
    case 1:
        max = 3;
        break;
    case 2:
        max = 1;
        break;
    default:
        max = 0;
    }
    if (has_ste_shifter) {
        mask = 0xfff;
    } else {
        mask = 0x777;
    }
    if (colorNum >= 0 && colorNum <= max) {
        if (color == -1) {
            return palette[colorNum] & mask;
        } else {
            palette[colorNum] = color;
            return color & mask;
        }
    } else {
        return 0;
    }
}


void vsync(void)
{
    WORD old_sr;
    LONG a;
    volatile LONG frclock;

    old_sr = set_sr(0x2300);    /* allow VBL interrupt */
    a = frclock;
#if 0	/* disabled by joy - it was neverending loop */
    while (frclock == a)        /* TODO, we could use stop2300() later */
        ;
#endif
    set_sr(old_sr);
}
