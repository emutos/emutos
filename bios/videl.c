/*
 * videl.c - Falcon VIDEL support
 *
 * Copyright (C) 2013-2024 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "machine.h"
#include "has.h"
#include "screen.h"
#include "videl.h"
#include "biosext.h"
#include "asm.h"
#include "natfeat.h"
#include "tosvars.h"
#include "lineavars.h"
#include "nvram.h"
#include "font.h"
#include "vt52.h"
#include "xbiosbind.h"
#include "vectors.h"

#if CONF_WITH_VIDEL

/*
 * used for byte-juggling between various palette layouts
 */
typedef union {
    ULONG l;
    UBYTE b[4];
} RGB;

static const ULONG videl_dflt_palette[] = {
    FRGB_WHITE, FRGB_RED, FRGB_GREEN, FRGB_YELLOW,
    FRGB_BLUE, FRGB_MAGENTA, FRGB_CYAN, FRGB_LTGRAY,
    FRGB_GRAY, FRGB_LTRED, FRGB_LTGREEN, FRGB_LTYELLOW,
    FRGB_LTBLUE, FRGB_LTMAGENTA, FRGB_LTCYAN, FRGB_BLACK,
    0xffff00ff, 0xeded00ed, 0xdddd00dd, 0xcccc00cc,
    0xbaba00ba, 0xaaaa00aa, 0x99990099, 0x87870087,
    0x77770077, 0x66660066, 0x54540054, 0x44440044,
    0x33330033, 0x21210021, 0x11110011, 0x00000000,
    0xff000000, 0xff000011, 0xff000021, 0xff000033,
    0xff000044, 0xff000054, 0xff000066, 0xff000077,
    0xff000087, 0xff000099, 0xff0000aa, 0xff0000ba,
    0xff0000cc, 0xff0000dd, 0xff0000ed, 0xff0000ff,
    0xed0000ff, 0xdd0000ff, 0xcc0000ff, 0xba0000ff,
    0xaa0000ff, 0x990000ff, 0x870000ff, 0x770000ff,
    0x660000ff, 0x540000ff, 0x440000ff, 0x330000ff,
    0x210000ff, 0x110000ff, 0x000000ff, 0x001100ff,
    0x002100ff, 0x003300ff, 0x004400ff, 0x005400ff,
    0x006600ff, 0x007700ff, 0x008700ff, 0x009900ff,
    0x00aa00ff, 0x00ba00ff, 0x00cc00ff, 0x00dd00ff,
    0x00ed00ff, 0x00ff00ff, 0x00ff00ed, 0x00ff00dd,
    0x00ff00cc, 0x00ff00ba, 0x00ff00aa, 0x00ff0099,
    0x00ff0087, 0x00ff0077, 0x00ff0066, 0x00ff0054,
    0x00ff0044, 0x00ff0033, 0x00ff0021, 0x00ff0011,
    0x00ff0000, 0x11ff0000, 0x21ff0000, 0x33ff0000,
    0x44ff0000, 0x54ff0000, 0x66ff0000, 0x77ff0000,
    0x87ff0000, 0x99ff0000, 0xaaff0000, 0xbaff0000,
    0xccff0000, 0xddff0000, 0xedff0000, 0xffff0000,
    0xffed0000, 0xffdd0000, 0xffcc0000, 0xffba0000,
    0xffaa0000, 0xff990000, 0xff870000, 0xff770000,
    0xff660000, 0xff540000, 0xff440000, 0xff330000,
    0xff210000, 0xff110000, 0xba000000, 0xba000011,
    0xba000021, 0xba000033, 0xba000044, 0xba000054,
    0xba000066, 0xba000077, 0xba000087, 0xba000099,
    0xba0000aa, 0xba0000ba, 0xaa0000ba, 0x990000ba,
    0x870000ba, 0x770000ba, 0x660000ba, 0x540000ba,
    0x440000ba, 0x330000ba, 0x210000ba, 0x110000ba,
    0x000000ba, 0x001100ba, 0x002100ba, 0x003300ba,
    0x004400ba, 0x005400ba, 0x006600ba, 0x007700ba,
    0x008700ba, 0x009900ba, 0x00aa00ba, 0x00ba00ba,
    0x00ba00aa, 0x00ba0099, 0x00ba0087, 0x00ba0077,
    0x00ba0066, 0x00ba0054, 0x00ba0044, 0x00ba0033,
    0x00ba0021, 0x00ba0011, 0x00ba0000, 0x11ba0000,
    0x21ba0000, 0x33ba0000, 0x44ba0000, 0x54ba0000,
    0x66ba0000, 0x77ba0000, 0x87ba0000, 0x99ba0000,
    0xaaba0000, 0xbaba0000, 0xbaaa0000, 0xba990000,
    0xba870000, 0xba770000, 0xba660000, 0xba540000,
    0xba440000, 0xba330000, 0xba210000, 0xba110000,
    0x77000000, 0x77000011, 0x77000021, 0x77000033,
    0x77000044, 0x77000054, 0x77000066, 0x77000077,
    0x66000077, 0x54000077, 0x44000077, 0x33000077,
    0x21000077, 0x11000077, 0x00000077, 0x00110077,
    0x00210077, 0x00330077, 0x00440077, 0x00540077,
    0x00660077, 0x00770077, 0x00770066, 0x00770054,
    0x00770044, 0x00770033, 0x00770021, 0x00770011,
    0x00770000, 0x11770000, 0x21770000, 0x33770000,
    0x44770000, 0x54770000, 0x66770000, 0x77770000,
    0x77660000, 0x77540000, 0x77440000, 0x77330000,
    0x77210000, 0x77110000, 0x44000000, 0x44000011,
    0x44000021, 0x44000033, 0x44000044, 0x33000044,
    0x21000044, 0x11000044, 0x00000044, 0x00110044,
    0x00210044, 0x00330044, 0x00440044, 0x00440033,
    0x00440021, 0x00440011, 0x00440000, 0x11440000,
    0x21440000, 0x33440000, 0x44440000, 0x44330000,
    0x44210000, 0x44110000, FRGB_WHITE, FRGB_BLACK
};

GLOBAL LONG falcon_shadow_count;        /* real Falcon does this, used by vectors.S */
static ULONG falcon_shadow_palette[256];
static UWORD ste_shadow_palette[16];

#define MON_ALL     -1  /* code used in VMODE_ENTRY for match on mode only */

/*
 * tables that cover the same Falcon modes as TOS 4.04
 * note:
 *  . Truecolor modes are not currently supported by the VDI
 */
static const VMODE_ENTRY vga_init_table[] = {
    /*
     * the entries in this table are for VGA/NTSC (i.e. VGA 60Hz) and VGA/PAL
     * (i.e. VGA 50Hz).  in *this* table, each entry applies to four video modes:
     * with & without VIDEL_VERTICAL, with & without VIDEL_PAL.  note that the
     * VIDEL_OVERSCAN bit is ignored when searching for a match in this table.
     *
     * note that modes that include the VIDEL_COMPAT bit may be exact duplicates of
     * others; for example, modes 90 & 98 are the same, as are the 8 modes 91-94,99-9c.
     */
    { 0x0010,  0x00c6, 0x008d, 0x0015, 0x022d, 0x0011, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0011,  0x0017, 0x0012, 0x0001, 0x020a, 0x0009, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0012,  0x00c6, 0x008d, 0x0015, 0x028a, 0x006b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0013,  0x00c6, 0x008d, 0x0015, 0x029a, 0x007b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0014,  0x00c6, 0x008d, 0x0015, 0x02ac, 0x0091, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0018,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0019,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x001a,  0x00c6, 0x008d, 0x0015, 0x02a3, 0x007c, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x001b,  0x00c6, 0x008d, 0x0015, 0x02ab, 0x0084, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x001c,  0x018e, 0x011d, 0x002d, 0x0000, 0x0119, 0x012e, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0090,  0x00c6, 0x008d, 0x0015, 0x022d, 0x0011, 0x0096, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0091,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0092,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0093,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0094,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0098,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0099,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x009a,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x009b,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x009c,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

static const VMODE_ENTRY pal_init_table[] = {
    /*
     * the entries in this table are for RGB/TV with PAL, with or without overscan
     */
    { 0x0020,  0x00fe, 0x00cb, 0x0027, 0x02ae, 0x0042, 0x00d8, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0021,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0022,  0x00fe, 0x00cb, 0x0027, 0x000c, 0x006d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0023,  0x00fe, 0x00cb, 0x0027, 0x001c, 0x007d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0024,  0x00fe, 0x00cb, 0x0027, 0x002e, 0x008f, 0x00d8, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0028,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0029,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x002a,  0x01fe, 0x0199, 0x0050, 0x004d, 0x00fe, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x002b,  0x01fe, 0x0199, 0x0050, 0x005d, 0x010e, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x002c,  0x01fe, 0x0199, 0x0050, 0x0071, 0x0122, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },

    { 0x0060,  0x00fe, 0x00cb, 0x0027, 0x028e, 0x0062, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0061,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0062,  0x00fe, 0x00cb, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0063,  0x00fe, 0x00cb, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0064,  0x00fe, 0x00cb, 0x0027, 0x000e, 0x00af, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0068,  0x01fe, 0x0199, 0x0050, 0x03af, 0x00e0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0069,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x006a,  0x01fe, 0x0199, 0x0050, 0x000d, 0x013e, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x006b,  0x01fe, 0x0199, 0x0050, 0x001d, 0x014e, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x006c,  0x01fe, 0x0199, 0x0050, 0x0031, 0x0162, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },

    { 0x00a1,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00a2,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00a3,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00a4,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00a9,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00aa,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00ab,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00ac,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },

    { 0x00e1,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00e2,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00e3,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00e4,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00e9,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00ea,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00eb,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00ec,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },

    { 0x0120,  0x00fe, 0x00cb, 0x0027, 0x02ae, 0x0042, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0121,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0122,  0x00fe, 0x00cb, 0x0027, 0x000c, 0x006d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0123,  0x00fe, 0x00cb, 0x0027, 0x001c, 0x007d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0124,  0x00fe, 0x00cb, 0x0027, 0x002e, 0x008f, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0128,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0129,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x012a,  0x01fe, 0x0199, 0x0050, 0x004d, 0x00fe, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x012b,  0x01fe, 0x0199, 0x0050, 0x005d, 0x010e, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x012c,  0x01fe, 0x0199, 0x0050, 0x0071, 0x0122, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },

    { 0x0160,  0x00fe, 0x00cb, 0x0027, 0x028e, 0x0062, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0161,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0162,  0x00fe, 0x00cb, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0163,  0x00fe, 0x00cb, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0164,  0x00fe, 0x00cb, 0x0027, 0x000e, 0x00af, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0168,  0x01fe, 0x0199, 0x0050, 0x03af, 0x00e0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0169,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x016a,  0x01fe, 0x0199, 0x0050, 0x000d, 0x013e, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x016b,  0x01fe, 0x0199, 0x0050, 0x001d, 0x014e, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x016c,  0x01fe, 0x0199, 0x0050, 0x0031, 0x0162, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },

    { 0x01a0,  0x00fe, 0x00cb, 0x0027, 0x02ae, 0x0042, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x01a8,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },

    { 0x01e0,  0x00fe, 0x00cb, 0x0027, 0x028e, 0x0062, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x01e8,  0x01fe, 0x0199, 0x0050, 0x03af, 0x00e0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },

    { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

static const VMODE_ENTRY other_init_table[] = {
    /*
     * the entries in this table are for RGB/TV with NTSC, with or without overscan,
     * plus (for convenience) the Atari monochrome monitor
     */
    { 0x0000,  0x00fe, 0x00c9, 0x0027, 0x02ae, 0x0042, 0x00d8, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0001,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0002,  0x00fe, 0x00c9, 0x0027, 0x000c, 0x006d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0003,  0x00fe, 0x00c9, 0x0027, 0x001c, 0x007d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0004,  0x00fe, 0x00c9, 0x0027, 0x002e, 0x008f, 0x00d8, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0008,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0009,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x000a,  0x01ff, 0x0197, 0x0050, 0x004d, 0x00fd, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x000b,  0x01ff, 0x0197, 0x0050, 0x005d, 0x010d, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x000c,  0x01ff, 0x0197, 0x0050, 0x0071, 0x0121, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },

    { 0x0040,  0x00fe, 0x00c9, 0x0027, 0x028e, 0x0062, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0041,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0042,  0x00fe, 0x00c9, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0043,  0x00fe, 0x00c9, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0044,  0x00fe, 0x00c9, 0x0027, 0x000e, 0x00af, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0048,  0x01ff, 0x0197, 0x0050, 0x03b0, 0x00df, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0049,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x004a,  0x01ff, 0x0197, 0x0050, 0x000d, 0x013d, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x004b,  0x01ff, 0x0197, 0x0050, 0x001d, 0x014d, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x004c,  0x01ff, 0x0197, 0x0050, 0x0031, 0x0161, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },

    { 0x0081,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0082,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0083,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0084,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },

    /* the following entry in this table is for the Atari monochrome monitor only */
    { 0x0088,  0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },

    { 0x0089,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x008a,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x008b,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x008c,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },

    { 0x00c1,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00c2,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00c3,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00c4,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00c9,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00ca,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00cb,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00cc,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },

    { 0x0100,  0x00fe, 0x00c9, 0x0027, 0x02ae, 0x0042, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0101,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0102,  0x00fe, 0x00c9, 0x0027, 0x000c, 0x006d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0103,  0x00fe, 0x00c9, 0x0027, 0x001c, 0x007d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0104,  0x00fe, 0x00c9, 0x0027, 0x002e, 0x008f, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0108,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0109,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x010a,  0x01ff, 0x0197, 0x0050, 0x004d, 0x00fd, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x010b,  0x01ff, 0x0197, 0x0050, 0x005d, 0x010d, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x010c,  0x01ff, 0x0197, 0x0050, 0x0071, 0x0121, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },

    { 0x0140,  0x00fe, 0x00c9, 0x0027, 0x028e, 0x0062, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0141,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0142,  0x00fe, 0x00c9, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0143,  0x00fe, 0x00c9, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0144,  0x00fe, 0x00c9, 0x0027, 0x000e, 0x00af, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0148,  0x01ff, 0x0197, 0x0050, 0x03b0, 0x00df, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0149,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x014a,  0x01ff, 0x0197, 0x0050, 0x000d, 0x013d, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x014b,  0x01ff, 0x0197, 0x0050, 0x001d, 0x014d, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x014c,  0x01ff, 0x0197, 0x0050, 0x0031, 0x0161, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },

    { 0x0180,  0x00fe, 0x00c9, 0x0027, 0x02ae, 0x0042, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0188,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },

    { 0x01c0,  0x00fe, 0x00c9, 0x0027, 0x028e, 0x0062, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x01c8,  0x01ff, 0x0197, 0x0050, 0x03b0, 0x00df, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },

    { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/*
 * functions for VIDEL programming
 */

/*
 * get the number of bits-per-pixel from the current hardware settings;
 * this is ultimately used by linea_init() to derive v_planes & V_REZ_HZ
 * when the resolution changes.
 *
 * note that we used to clamp this to 8, but this causes V_REZ_HZ to be
 * wrong in TC modes.
 */
static UWORD get_videl_bpp(void)
{
    UWORD f_shift = *(volatile UWORD *)SPSHIFT;
    UBYTE st_shift = *(volatile UBYTE *)ST_SHIFTER;
    UWORD bits_per_pixel;

    /*
     * to get bpp, we must examine f_shift and st_shift.
     *
     * f_shift is valid if any of bits 10, 8 or 4 is set.
     * Priority in f_shift is: 10 ">" 8 ">" 4, i.e. if SPS_2COLOR
     * is set then SPS_HICOLOR/SPS_256COLOR are don't care ...
     *
     * If all these bits are 0 we get the display depth from
     * st_shift (as for ST and STe)
     */
    if (f_shift & SPS_2COLOR)           /* 1 bitplane */
        bits_per_pixel = 1;
    else if (f_shift & SPS_HICOLOR)     /* 16-bit colour */
        bits_per_pixel = 16;
    else if (f_shift & SPS_256COLOR)    /* 8 bitplanes */
        bits_per_pixel = 8;
    else if (st_shift == ST_LOW)
        bits_per_pixel = 4;
    else if (st_shift == ST_MEDIUM)
        bits_per_pixel = 2;
    else /* if (st_shift == ST_HIGH) */
        bits_per_pixel = 1;

    return bits_per_pixel;
}

static UWORD get_videl_width(void)
{
    return (*(volatile UWORD *)0xffff8210) * 16 / get_videl_bpp();
}

static UWORD get_videl_height(void)
{
    UWORD vdb = *(volatile UWORD *)0xffff82a8;
    UWORD vde = *(volatile UWORD *)0xffff82aa;
    UWORD vmode = *(volatile UWORD *)0xffff82c2;

    /* visible y resolution:
     * Graphics display starts at line VDB and ends at line
     * VDE. If interlace mode off unit of VC-registers is
     * half lines, else lines.
     */
    UWORD yres = vde - vdb;
    if (!(vmode & 0x02))        /* interlace */
        yres >>= 1;
    if (vmode & 0x01)           /* double */
        yres >>= 1;

    return yres;
}


/*
 * lookup videl initialisation data for specified mode (assumed to
 * be valid for current monitor, e.g. via vfixmode())
 *
 * returns NULL if mode is invalid
 */
const VMODE_ENTRY *lookup_videl_mode(WORD mode)
{
    const VMODE_ENTRY *vmode_init_table, *p;

    /*
     * ignore reserved bits (this is what TOS4 does)
     */
    mode &= VIDEL_VALID;

    if (mode&VIDEL_VGA) {
        vmode_init_table = vga_init_table;
        /*
         * ignore OVERSCAN (since VGA is set), plus other
         * bits that don't affect initialisation data
         */
        mode &= ~(VIDEL_OVERSCAN|VIDEL_VERTICAL|VIDEL_PAL);
    } else if (mode&VIDEL_PAL) {
        vmode_init_table = pal_init_table;
    } else {
        vmode_init_table = other_init_table;
    }

    for (p = vmode_init_table; p->vmode >= 0; p++)
        if (p->vmode == mode)
            return p;

    return NULL;
}


/*
 * determine scanline width based on video mode
 */
static WORD determine_width(WORD mode)
{
    WORD linewidth;

    linewidth = (mode&VIDEL_80COL) ? 40 : 20;
    linewidth <<= (mode & VIDEL_BPPMASK);
    /* overscan is ignored if VGA is set */
    if ((mode&(VIDEL_OVERSCAN|VIDEL_VGA)) == VIDEL_OVERSCAN)
        linewidth = linewidth * 12 / 10;    /* multiply by 1.2 */

    return linewidth;
}


/*
 * determine vctl based on video mode and monitor type
 */
static WORD determine_vctl(WORD mode,WORD monitor)
{
    WORD vctl;

    if (mode&VIDEL_VGA) {
        vctl = (mode&VIDEL_80COL) ? 0x08 : 0x04;
        if (mode&VIDEL_VERTICAL)
            vctl |= 0x01;
    } else {
        vctl = (mode&VIDEL_80COL) ? 0x04 : 0x00;
        if (mode&VIDEL_VERTICAL)
            vctl |= 0x02;
    }

    if (!(mode&VIDEL_COMPAT))
        return vctl;

    switch(mode&VIDEL_BPPMASK) {
    case VIDEL_1BPP:
        if (!(mode&VIDEL_VGA) && (monitor == MON_MONO))
            vctl = 0x08;
        break;
    case VIDEL_2BPP:
        vctl = (mode&VIDEL_VGA)? 0x09 : 0x04;
        break;
    case VIDEL_4BPP:
        vctl = (mode&VIDEL_VGA)? 0x05 : 0x00;
        break;
    default:
        vctl = 0x04;
    }

    return vctl;
}


/*
 * determine regc0 based on video mode & monitor type
 */
static WORD determine_regc0(WORD mode,WORD monitor)
{
    /* mono monitors only have 1 setting */
    if (monitor == MON_MONO)
        return 0x0080;

    /* likewise VGA */
    if (mode&VIDEL_VGA)
        return 0x0186;

    /* handle ST-compatible modes */
    if (mode&VIDEL_COMPAT) {
        if ((mode&VIDEL_BPPMASK) != VIDEL_1BPP)     /* not 2-colour */
            return (monitor==MON_TV)?0x0083:0x0081;
    }

    return (monitor==MON_TV)?0x0183:0x0181;
}


/*
 * this routine can set VIDEL to 1,2,4 or 8 bitplanes mode on VGA
 *
 * A note on TOS4 quirks
 * ---------------------
 * For certain videl modes (none of which are used by the desktop), TOS4
 * sets some unusual register values which I consider to be erroneous;
 * therefore this code does not attempt to reproduce them.  For future
 * reference, these are the ones I have discovered:
 * (1) VGA monitor: mode 0x0059 (& equivalent modes 0x0079, 0x0149, 0x169)
 *      The vctl register (0xff82c2) is set to 0 rather than 8, setting
 *      cycles/pixel to 4 instead of 1.  This seems clearly wrong, as
 *      mode 0x0059 on a VGA monitor is logically exactly the same as
 *      mode 0x0019 which does not do this.
 * (2) VGA monitor: modes 0x005n (& many equivalent modes)
 *      The offset register (xff820e) is set to 1/5 of the screen width.
 *      The logically equivalent set of modes (0x001n) do not do this.
 */
static int set_videl_vga(WORD mode)
{
    volatile char *videlregs = (char *)0xffff8200;
#define videlword(n) (*(volatile UWORD *)(videlregs+(n)))
    const VMODE_ENTRY *p;
    WORD linewidth, monitor, vctl;

    monitor = vmontype();

    p = lookup_videl_mode(mode);        /* validate mode */
    if (!p)
        return -1;

    videlregs[0x0a] = (mode&VIDEL_PAL) ? 2 : 0; /* video sync to 50Hz if PAL */

#ifndef MACHINE_FIREBEE
    /* On the Falcon, synchronize Videl register updates like Atari TOS 4 */
    vsync(); /* wait for VBL */
#endif

    videlword(0x82) = p->hht;           /* H hold timer */
    videlword(0x84) = p->hbb;           /* H border begin */
    videlword(0x86) = p->hbe;           /* H border end */
    videlword(0x88) = p->hdb;           /* H display begin */
    videlword(0x8a) = p->hde;           /* H display end */
    videlword(0x8c) = p->hss;           /* H SS */

    videlword(0xa2) = p->vft;           /* V freq timer */
    videlword(0xa4) = p->vbb;           /* V border begin */
    videlword(0xa6) = p->vbe;           /* V border end */
    videlword(0xa8) = p->vdb;           /* V display begin */
    videlword(0xaa) = p->vde;           /* V display end */
    videlword(0xac) = p->vss;           /* V SS */

    videlregs[0x60] = 0x00;             /* clear ST shift for safety */

    videlword(0x0e) = 0;                /* offset */

    linewidth = determine_width(mode);
    vctl = determine_vctl(mode,monitor);

    videlword(0x10) = linewidth;        /* scanline width */
    videlword(0xc2) = vctl;             /* video control */
    videlword(0xc0) = determine_regc0(mode,monitor);
    videlword(0x66) = 0x0000;           /* clear SPSHIFT */

    switch(mode&VIDEL_BPPMASK) {        /* set SPSHIFT / ST shift */
    case VIDEL_1BPP:                    /* 2 colours (mono) */
        if (monitor == MON_MONO) {
            videlregs[0x60] = 0x02;
        } else {
            videlword(0x66) = 0x0400;
#ifndef MACHINE_FIREBEE
            /*
             * When switching to monochrome mode, the Falcon Videl needs these
             * additional steps; otherwise video output can become distorted.
             * Based on: "patch for Videl monochrome bug in Falcons, posted
             * by Thomas Binder" in FreeMiNT.
             *
             * Note that, when running under Hatari, this causes spurious msgs
             * such as:
             *   "WARN : Strange screen size 80x640 -> aspect corrected by 8x1!"
             * each time a Vsetmode() is done for a monochrome mode.
             *
             * To try to avoid these, we check for NatFeats.  Its availability
             * certainly means that we're running under an emulator which
             * shouldn't need workarounds for hardware bugs.  Of course, if
             * Hatari has NatFeats disabled, or if we compile without NatFeats
             * support, we'll still see those messages.
             */
            if (!HAS_NATFEATS) {
                vsync();
                videlword(0x66) = 0;
                vsync();
                videlword(0x66) = 0x0400;
            }
#endif
        }
        break;
    case VIDEL_2BPP:                    /* 4 colours */
        videlregs[0x60] = 0x01;
        videlword(0x10) = linewidth;        /* writing to the ST shifter has    */
        videlword(0xc2) = vctl;             /* just overwritten these registers */
        break;
    case VIDEL_4BPP:                    /* 16 colours */
        /* if not ST-compatible, SPSHIFT was already set correctly above */
        if (mode&VIDEL_COMPAT)
            videlregs[0x60] = 0x00;         /* else set ST shifter */
        break;
    case VIDEL_8BPP:                    /* 256 colours */
        videlword(0x66) = 0x0010;
        break;
    case VIDEL_TRUECOLOR:               /* 65536 colours (Truecolor) */
        videlword(0x66) = 0x0100;
        break;
    }

    return 0;
}

/*
 * the current Falcon video mode; used by vsetmode() & vfixmode()
 */
WORD current_video_mode;

/*
 * Set Falcon video mode - also sets 'sshiftmod' appropriately
 */
WORD vsetmode(WORD mode)
{
    WORD ret;

    if (!has_videl)
        return 0x58;    /* unimplemented xbios call: return function # */

    if (mode == -1)
        return current_video_mode;

    KDEBUG(("vsetmode(0x%04x)\n", mode));

    /*
     * fix up mode according to current monitor type, just like TOS
     */
    mode = vfixmode(mode);

    if (set_videl_vga(mode) < 0)    /* invalid mode */
        return current_video_mode;

    ret = current_video_mode;
    current_video_mode = mode;

    /*
     * set sshiftmod
     *
     * NOTE: ST high can be displayed on both an Atari monochrome monitor
     * (e.g. the SM124/SM125) and a VGA display, so we need to check
     * both the videl & STe-compatible hardware registers.
     */
    if (mode & VIDEL_COMPAT) {
        if (*(volatile UWORD *)SPSHIFT & SPS_2COLOR)
            sshiftmod = ST_HIGH;
        else
            sshiftmod = *(volatile UBYTE *)ST_SHIFTER;
    } else {
        sshiftmod = FALCON_REZ;
    }

    return ret;
}

/*
 * Get Videl monitor type
 */
WORD vmontype(void)
{
    if (!has_videl)
        return 0x59;    /* unimplemented xbios call: return function # */

    return ((*(volatile UBYTE *)0xffff8006) >> 6) & 3;
}

/*
 * Set external video sync mode
 */
WORD vsetsync(WORD external)
{
    UWORD spshift;

    if (!has_videl)
        return 0x5a;    /* unimplemented xbios call: return function # */

    if (external & 0x01)            /* external clock wanted? */
        *(volatile UBYTE *)SYNCMODE |= 0x01;
    else *(volatile UBYTE *)SYNCMODE &= 0xfe;

    spshift = *(volatile UWORD *)SPSHIFT;

    if (external&0x02)              /* external vertical sync wanted? */
        spshift |= 0x0020;
    else spshift &= 0xffdf;

    if (external&0x04)              /* external horizontal sync wanted? */
        spshift |= 0x0040;
    else spshift &= 0xffbf;

    *(volatile UWORD *)SPSHIFT = spshift;

    return 0; /* OK */
}

/*
 * vgetsize - implements the Vgetsize() XBIOS call
 *
 * The described purpose of this call is to return the video ram size
 * required for the specified mode.  Under TOS4, the same results are
 * obtained, no matter what type of monitor is in use at the time of
 * the call.  This is not useful, for the following reasons:
 *  (1) When Vsetscreen() is called to set a new mode, that mode is fixed
 *      up via Vfixmode() which *does* respect the monitor type.  Thus,
 *      calling Vgetsize() before calling Vsetscreen() may give you
 *      invalid information.  Further, if Vsetscreen() is called with NULL
 *      values for logical and physical screen addresses, the actual RAM
 *      allocated may differ from the amount indicated, for the same reason.
 *  (2) The amount calculated can be completely bogus if the passed mode
 *      contains conflicting parameters which cannot all be present in a
 *      valid mode.  This happens, for example, if OVERSCAN and VGA are
 *      both set.
 *
 * Therefore, in this implementation, Vfixmode() is called at the beginning
 * to fix up the mode.  This makes the call useful in all circumstances,
 * with the slight drawback that the values returned may differ from those
 * returned by TOS4 in the same circumstances.
 */
LONG vgetsize(WORD inmode)
{
    const VMODE_ENTRY *p;
    int height;
    WORD mode, vctl;

    if (!has_videl)
        return 0x5b;    /* unimplemented xbios call: return function # */

    mode = inmode & VIDEL_VALID;    /* ignore invalid bits */
    if ((mode&VIDEL_BPPMASK) > VIDEL_TRUECOLOR) {   /* fixup invalid bpp */
        mode &= ~VIDEL_BPPMASK;
        mode |= VIDEL_TRUECOLOR;
    }

    mode = vfixmode(mode);

    p = lookup_videl_mode(mode);
    if (!p) {                       /* invalid mode - "can't happen" */
        KDEBUG(("vgetsize(): entry for mode 0x%04x missing (original=0x%04x)\n",mode,inmode));
        if (mode&VIDEL_COMPAT)      /* try to fix things up */
            return ST_VRAM_SIZE;
        return FALCON_VRAM_SIZE;
    }

    vctl = determine_vctl(mode,vmontype());
    height = p->vde - p->vdb;
    if (!(vctl&0x02))
        height >>= 1;
    if (vctl&0x01)
        height >>= 1;

    return (LONG)determine_width(mode) * 2 * height;
}

/*
 * convert from Falcon palette format to STe palette format
 */
#define falc2ste(a) ((((a)>>1)&0x08)|(((a)>>5)&0x07))
static void convert2ste(UWORD *ste,const ULONG *falcon)
{
    RGB u;  /* staging area for Falcon shadow palette: RG0B */
    int i;

    for (i = 0; i < 16; i++) {
        u.l = *falcon++;
        *ste++ = (falc2ste(u.b[0])<<8) | (falc2ste(u.b[1])<<4) | falc2ste(u.b[3]);
    }
}

/*
 * determine whether to update STe or Falcon h/w palette registers
 * returns TRUE if we need to update the STe h/w palette
 */
static int use_ste_palette(WORD videomode)
{
    if (vmontype() == MON_MONO)                     /* always for ST mono monitor */
        return TRUE;

    if ((videomode&VIDEL_BPPMASK) == VIDEL_2BPP)    /* always for 4-colour modes */
        return TRUE;

    if ((videomode&VIDEL_COMPAT) && ((videomode&VIDEL_BPPMASK) == VIDEL_4BPP))
        return TRUE;                                /* and for ST low */

    return FALSE;
}

/*
 * set palette registers
 *
 * note that the actual update of the hardware registers is done by the
 * VBL interrupt handler, according to the setting of 'colorptr'.  since
 * the address in colorptr must be even, we use bit 0 as a flag.
 *
 * colorptr contents   VBL interrupt handler action
 * -----------------   ----------------------------
 *       0             do nothing
 * address             load STe palette regs from address
 * address | 0x01      load first 16 Falcon palette regs from address
 *       0 | 0x01      load 256 Falcon palette regs from falcon_shadow_palette[]
 */
WORD vsetrgb(WORD index,WORD count,const ULONG *rgb)
{
    ULONG *shadow;
    const ULONG *source;
    RGB u;      /* staging area for software palette: 0RGB */
    WORD limit;

    if (!has_videl)
        return 0x5d;    /* unimplemented xbios call: return function # */

    if ((index < 0) || (count <= 0))
        return -1; /* Generic error */

    limit = (get_videl_bpp()<=4) ? 16 : 256;
    if ((index+count) > limit)
        return -1; /* Generic error */

    /*
     * for ST low or 4-colour modes, we need to convert the RGB value
     * to STe palette register format, and request the VBL interrupt
     * handler to update the STe palette registers rather than the
     * Falcon palette registers
     */
    if (use_ste_palette(vsetmode(-1))) {
        UWORD *ste_shadow = ste_shadow_palette + index;
        source = rgb;
        while(count--) {
            u.l = *source++;
            *ste_shadow++ = (falc2ste(u.b[1])<<8) | (falc2ste(u.b[2])<<4) | falc2ste(u.b[3]);
        }
        colorptr = ste_shadow_palette;
        return 0; /* OK */
    }

    /*
     * update the Falcon shadow palette (that's what we return for VgetRGB())
     */
    shadow = falcon_shadow_palette + index;
    source = rgb;
    while(count--) {
        u.l = *source++;
        u.b[0] = u.b[1];                 /* shift R & G */
        u.b[1] = u.b[2];
        u.b[2] = 0x00;
        *shadow++ = u.l;
    }

    falcon_shadow_count = limit;    /* tell VBL handler how many registers to copy */
    colorptr = (UWORD *)((LONG)falcon_shadow_palette|0x01L);

    return 0; /* OK */
}

/*
 * get palette registers
 */
WORD vgetrgb(WORD index,WORD count,ULONG *rgb)
{
    ULONG *shadow;
    RGB u;      /* staging area for Falcon shadow palette: RG0B */
    WORD limit, mode;
    UWORD value;

    if (!has_videl)
        return 0x5e;    /* unimplemented xbios call: return function # */

    if ((index < 0) || (count <= 0))
        return -1; /* Generic error */

    mode = vsetmode(-1);
    limit = ((mode&VIDEL_BPPMASK)==VIDEL_8BPP) ? 256 : 16;

    if ((index+count) > limit)
        return -1; /* Generic error */

    /*
     * for 4-colour or ST-compatible modes, VgetRGB() returns
     * values derived from the STe palette
     */
    if (use_ste_palette(mode)) {
        UWORD *ste_shadow = ste_shadow_palette + index;
        u.l = 0;
        while(count--) {
            value = *ste_shadow++;
            value = ((value&0x0777)<<1) | ((value&0x0888)>>3);
            u.b[1] = ((value>>8) & 0x0f) * 16;
            u.b[2] = ((value>>4) & 0x0f) * 16;
            u.b[3] = (value & 0x0f) * 16;
            *rgb++ = u.l;
        }
        return 0;   /* OK */
    }

    shadow = falcon_shadow_palette + index;
    while(count--) {
        u.l = *shadow++;
        u.b[2] = u.b[1];        /* shift R & G right*/
        u.b[1] = u.b[0];
        u.b[0] = 0x00;
        *rgb++ = u.l;
    }

    return 0; /* OK */
}

/*
 * Fix Videl mode
 *
 * This converts an (assumed legal) input mode into the
 * corresponding output mode for the current monitor type
 */
WORD vfixmode(WORD mode)
{
WORD monitor, currentmode;

    if (!has_videl)
        return -32;

    monitor = vmontype();
    if (monitor == MON_MONO)
        return FALCON_ST_HIGH;

    currentmode = vsetmode(-1);
    if (currentmode & VIDEL_PAL)    /* set PAL bit per current value */
        mode |= VIDEL_PAL;
    else mode &= ~VIDEL_PAL;

    /* handle VGA monitor */
    if (monitor == MON_VGA) {
        if (!(mode & VIDEL_VGA))            /* if mode doesn't have VGA set, */
            mode ^= (VIDEL_VERTICAL | VIDEL_VGA);   /* set it & flip vertical */
        if (mode & VIDEL_COMPAT) {
            if ((mode&VIDEL_BPPMASK) == VIDEL_1BPP)
                mode &= ~VIDEL_VERTICAL;    /* clear vertical for ST high */
            else mode |= VIDEL_VERTICAL;    /* set it for ST medium, low  */
        }
        return mode;
    }

    /* handle RGB or TV */
    if (mode & VIDEL_VGA)                       /* if mode has VGA set, */
        mode ^= (VIDEL_VERTICAL | VIDEL_VGA);   /* clear it & flip vertical */
    if (mode & VIDEL_COMPAT) {
        if ((mode&VIDEL_BPPMASK) == VIDEL_1BPP)
            mode |= VIDEL_VERTICAL;         /* set vertical for ST high */
        else mode &= ~VIDEL_VERTICAL;       /* clear it for ST medium, low  */
    }

    return mode;
}

WORD videl_check_moderez(WORD moderez)
{
    WORD current_mode, return_mode;

    if (moderez < 0)                /* ignore rez values */
        return 0;

    current_mode = get_videl_mode();
    return_mode = vfixmode(moderez);/* adjust */
    return (return_mode==current_mode)?0:return_mode;
}

void videl_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez)
{
    *planes = get_videl_bpp();
    *hz_rez = get_videl_width();
    *vt_rez = get_videl_height();
}

void videl_setrez(WORD rez, WORD videlmode)
{
    if (rez != FALCON_REZ) {
        switch(rez) {
        case ST_LOW:
            videlmode = FALCON_ST_LOW;
            break;
        case ST_MEDIUM:
            videlmode = FALCON_ST_MEDIUM;
            break;
        case ST_HIGH:
            videlmode = FALCON_ST_HIGH;
            break;
        }
    }

    vsetmode(videlmode);    /* sets 'sshiftmod' */
}

/*
 * Initialise Falcon palette
 */
void initialise_falcon_palette(WORD mode)
{
    volatile UWORD *col_regs = (UWORD *) ST_PALETTE_REGS;
    volatile LONG *fcol_regs = (LONG *) FALCON_PALETTE_REGS;
    int i, limit;

    /* first, set up Falcon shadow palette and real registers */
    for (i = 0; i < 256; i++)
        falcon_shadow_palette[i] = videl_dflt_palette[i];

    switch(mode&VIDEL_BPPMASK) {
    case VIDEL_1BPP:        /* 2-colour mode */
        falcon_shadow_palette[1] = falcon_shadow_palette[15];
        break;
    case VIDEL_2BPP:        /* 4-colour mode */
        falcon_shadow_palette[3] = falcon_shadow_palette[15];
        break;
    }

    /* a 'feature' of the Falcon hardware: if we're in a mode with less
     * than 256 colours, and we attempt to set the Falcon hardware
     * palette registers for colours 16 & above, it will screw up the
     * values in the first 16 hardware palette registers, resulting in
     * a messed-up display ...
     * NOTE: what happens in the Truecolor case is yet to be determined,
     * although it is probably not important since we don't use those
     * registers.
     */
    limit = ((mode&VIDEL_BPPMASK)==VIDEL_8BPP) ? 256 : 16;
    for (i = 0; i < limit; i++)
        fcol_regs[i] = falcon_shadow_palette[i];

    /*
     * if appropriate, set up the STe shadow & real palette registers
     *
     * even though initialise_palette_registers() in screen.c has already
     * set up the STe palette, we update it here for compatibility with
     * Atari TOS.  this results in changes to registers 8-14 inclusive
     */
    if (use_ste_palette(mode)) {
        convert2ste(ste_shadow_palette,falcon_shadow_palette);
        for (i = 0; i < 16; i++)
            col_regs[i] = ste_shadow_palette[i];
    }
}

/*
 * Get videl mode
 * This is the same as vsetmode(-1) except that it returns
 * zero when there is no videl.  Used by app_save().
 */
WORD get_videl_mode(void)
{
    if (has_videl)
        return vsetmode(-1);

    return 0;
}

#endif /* CONF_WITH_VIDEL */
