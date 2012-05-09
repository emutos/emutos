/*
 * screen.c - low-level screen routines
 *
 * Copyright (c) 2001-2011 The EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  THH   Thomas Huth
 *  LVL   Laurent Vogel
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"  
#include "machine.h"
#include "screen.h"
#include "asm.h"
#include "tosvars.h"
#include "lineavars.h"
#include "nvram.h"
#include "kprint.h"
#include "font.h"
#include "vt52.h"

#define DBG_SCREEN 0

#define ST_VRAM_SIZE        32000UL
#define TT_VRAM_SIZE        153600UL
#define FALCON_VRAM_SIZE    307200UL

static unsigned long initial_vram_size();
static unsigned long vram_size();
static void setphys(LONG addr,int checkaddr);

#if CONF_WITH_SHIFTER

#define VIDEOBASE_ADDR_HI   0xffff8201L
#define VIDEOBASE_ADDR_MID  0xffff8203L
#define VIDEOBASE_ADDR_LOW  0xffff820dL

#define SYNCMODE            0xffff820aL

#define ST_SHIFTER          0xffff8260L
#define TT_SHIFTER          0xffff8262L
#define SPSHIFT             0xffff8266L

#define TT_SHIFTER_BITMASK  0x970f      /* valid bits in TT_SHIFTER */

#define ST_PALETTE_REGS     0xffff8240L
#define TT_PALETTE_REGS     0xffff8400L
#define FALCON_PALETTE_REGS 0xffff9800L

#define TT_PALETTE_BITMASK  0x0fff      /* valid bits in TT_PALETTE_REGS */

/* determine monitor type, ... */

/* Define palette */

static const WORD dflt_palette[] = {
    RGB_WHITE, RGB_RED, RGB_GREEN, RGB_YELLOW,
    RGB_BLUE, RGB_MAGENTA, RGB_CYAN, RGB_LTGRAY,
    RGB_GRAY, RGB_LTRED, RGB_LTGREEN, RGB_LTYELLOW,
    RGB_LTBLUE, RGB_LTMAGENTA, RGB_LTCYAN, RGB_BLACK
};

#if CONF_WITH_VIDEL
static const LONG videl_dflt_palette[] = {
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

GLOBAL LONG falcon_shadow_palette[256];   /* real Falcon does this */
static WORD ste_shadow_palette[16];
#endif

#if CONF_WITH_TT_SHIFTER
static const WORD tt_dflt_palette[] = {
 TTRGB_WHITE, TTRGB_RED, TTRGB_GREEN, TTRGB_YELLOW,
 TTRGB_BLUE, TTRGB_MAGENTA, TTRGB_CYAN, TTRGB_LTGRAY,
 TTRGB_GRAY, TTRGB_LTRED, TTRGB_LTGREEN, TTRGB_LTYELLOW,
 TTRGB_LTBLUE, TTRGB_LTMAGENTA, TTRGB_LTCYAN, TTRGB_BLACK,
 0x0fff, 0x0eee, 0x0ddd, 0x0ccc, 0x0bbb, 0x0aaa, 0x0999, 0x0888,
 0x0777, 0x0666, 0x0555, 0x0444, 0x0333, 0x0222, 0x0111, 0x0000,
 0x0f00, 0x0f01, 0x0f02, 0x0f03, 0x0f04, 0x0f05, 0x0f06, 0x0f07,
 0x0f08, 0x0f09, 0x0f0a, 0x0f0b, 0x0f0c, 0x0f0d, 0x0f0e, 0x0f0f,
 0x0e0f, 0x0d0f, 0x0c0f, 0x0b0f, 0x0a0f, 0x090f, 0x080f, 0x070f,
 0x060f, 0x050f, 0x040f, 0x030f, 0x020f, 0x010f, 0x000f, 0x001f,
 0x002f, 0x003f, 0x004f, 0x005f, 0x006f, 0x007f, 0x008f, 0x009f,
 0x00af, 0x00bf, 0x00cf, 0x00df, 0x00ef, 0x00ff, 0x00fe, 0x00fd,
 0x00fc, 0x00fb, 0x00fa, 0x00f9, 0x00f8, 0x00f7, 0x00f6, 0x00f5,
 0x00f4, 0x00f3, 0x00f2, 0x00f1, 0x00f0, 0x01f0, 0x02f0, 0x03f0,
 0x04f0, 0x05f0, 0x06f0, 0x07f0, 0x08f0, 0x09f0, 0x0af0, 0x0bf0,
 0x0cf0, 0x0df0, 0x0ef0, 0x0ff0, 0x0fe0, 0x0fd0, 0x0fc0, 0x0fb0,
 0x0fa0, 0x0f90, 0x0f80, 0x0f70, 0x0f60, 0x0f50, 0x0f40, 0x0f30,
 0x0f20, 0x0f10, 0x0b00, 0x0b01, 0x0b02, 0x0b03, 0x0b04, 0x0b05,
 0x0b06, 0x0b07, 0x0b08, 0x0b09, 0x0b0a, 0x0b0b, 0x0a0b, 0x090b,
 0x080b, 0x070b, 0x060b, 0x050b, 0x040b, 0x030b, 0x020b, 0x010b,
 0x000b, 0x001b, 0x002b, 0x003b, 0x004b, 0x005b, 0x006b, 0x007b,
 0x008b, 0x009b, 0x00ab, 0x00bb, 0x00ba, 0x00b9, 0x00b8, 0x00b7,
 0x00b6, 0x00b5, 0x00b4, 0x00b3, 0x00b2, 0x00b1, 0x00b0, 0x01b0,
 0x02b0, 0x03b0, 0x04b0, 0x05b0, 0x06b0, 0x07b0, 0x08b0, 0x09b0,
 0x0ab0, 0x0bb0, 0x0ba0, 0x0b90, 0x0b80, 0x0b70, 0x0b60, 0x0b50,
 0x0b40, 0x0b30, 0x0b20, 0x0b10, 0x0700, 0x0701, 0x0702, 0x0703,
 0x0704, 0x0705, 0x0706, 0x0707, 0x0607, 0x0507, 0x0407, 0x0307,
 0x0207, 0x0107, 0x0007, 0x0017, 0x0027, 0x0037, 0x0047, 0x0057,
 0x0067, 0x0077, 0x0076, 0x0075, 0x0074, 0x0073, 0x0072, 0x0071,
 0x0070, 0x0170, 0x0270, 0x0370, 0x0470, 0x0570, 0x0670, 0x0770,
 0x0760, 0x0750, 0x0740, 0x0730, 0x0720, 0x0710, 0x0400, 0x0401,
 0x0402, 0x0403, 0x0404, 0x0304, 0x0204, 0x0104, 0x0004, 0x0014,
 0x0024, 0x0034, 0x0044, 0x0043, 0x0042, 0x0041, 0x0040, 0x0140,
 0x0240, 0x0340, 0x0440, 0x0430, 0x0420, 0x0410, TTRGB_WHITE, TTRGB_BLACK
};
#endif

#if CONF_WITH_VIDEL
typedef struct {
    WORD vmode;         /* video mode (-1 => end marker) */
    WORD monitor;       /* applicable monitors */
    UWORD hht;          /* H hold timer */
    UWORD hbb;          /* H border begin */
    UWORD hbe;          /* H border end */
    UWORD hdb;          /* H display begin */
    UWORD hde;          /* H display end */
    UWORD hss;          /* H SS */
    UWORD vft;          /* V freq timer */
    UWORD vbb;          /* V border begin */
    UWORD vbe;          /* V border end */
    UWORD vdb;          /* V display begin */
    UWORD vde;          /* V display end */
    UWORD vss;          /* V SS */
} VMODE_ENTRY;

#define MON_ALL     -1  /* code used in VMODE_ENTRY for match on mode only */

/*
 * a list of all(?) valid Falcon modes
 * note:
 *  . 256-colour and Truecolor modes are not currently supported by the VDI
 *  . we could save space by replacing each of the sets of h and v registers
 *    within the table by indices into separate tables of h & v register sets.
 *    there are about 40 different sets of h registers and 12 different sets
 *    of v registers, so if we used one byte (as an index) for each within
 *    this table, we could save about 22 * 170 = 3740 bytes in this table at
 *    a cost of about 52 * 12 = 624 bytes for the additional tables plus the
 *    bytes required for the code to do the additional table lookup.
 */
static const VMODE_ENTRY vmode_init_table[] = {
    /* the first entries are for VGA, since we expect to access them most frequently */
    /* there are entries for VGA/NTSC (i.e. VGA 60Hz) and VGA/PAL (i.e. VGA 50Hz)    */
    { 0x0011, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020a, 0x0009, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0012, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x028a, 0x006b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0013, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x029a, 0x007b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0014, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ac, 0x0091, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0018, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0019, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x001a, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02a3, 0x007c, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x001b, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ab, 0x0084, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0031, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020a, 0x0009, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0032, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x028a, 0x006b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0033, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x029a, 0x007b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0034, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ac, 0x0091, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0038, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0039, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x003a, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02a3, 0x007c, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x003b, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ab, 0x0084, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0092, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0098, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0099, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x00b2, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x00b8, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x00b9, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0111, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020a, 0x0009, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0112, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x028a, 0x006b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0113, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x029a, 0x007b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0114, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ac, 0x0091, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0118, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0119, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x011a, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02a3, 0x007c, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x011b, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ab, 0x0084, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0131, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020a, 0x0009, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0132, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x028a, 0x006b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0133, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x029a, 0x007b, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0134, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ac, 0x0091, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0138, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0139, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x013a, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02a3, 0x007c, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x013b, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x02ab, 0x0084, 0x0096, 0x0419, 0x03ff, 0x003f, 0x003f, 0x03ff, 0x0415 },
    { 0x0192, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0198, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x0199, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x01b2, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x01b8, MON_ALL,  0x00c6, 0x008d, 0x0015, 0x0273, 0x0050, 0x0096, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },
    { 0x01b9, MON_ALL,  0x0017, 0x0012, 0x0001, 0x020e, 0x000d, 0x0011, 0x0419, 0x03af, 0x008f, 0x008f, 0x03af, 0x0415 },

    /* the remaining entries are for TV+NTSC, TV+PAL, TV+NTSC+overscan, TV+PAL+overscan */
    { 0x0001, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0002, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x000c, 0x006d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0003, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x001c, 0x007d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0004, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x002e, 0x008f, 0x00d8, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0008, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0009, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x000a, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x004d, 0x00fd, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x000b, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x005d, 0x010d, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0021, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0022, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x000c, 0x006d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0023, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x001c, 0x007d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0024, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x002e, 0x008f, 0x00d8, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0028, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0029, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x002a, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x004d, 0x00fe, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x002b, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x005d, 0x010e, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x0041, MON_VGA,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0041, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0042, MON_VGA,  0x00fe, 0x00c9, 0x0027, 0x000c, 0x006d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0042, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0043, MON_VGA,  0x00fe, 0x00c9, 0x0027, 0x001c, 0x007d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0043, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0044, MON_VGA,  0x00fe, 0x00c9, 0x0027, 0x002e, 0x008f, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0044, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x000e, 0x00af, 0x00d8, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0048, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0048, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03b0, 0x00df, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0049, MON_VGA,  0x003e, 0x0030, 0x0008, 0x023b, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0049, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x004a, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x004d, 0x00fd, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x004a, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x000d, 0x013d, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x004b, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x005d, 0x010d, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x004b, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x001d, 0x014d, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x0061, MON_VGA,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0061, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0062, MON_VGA,  0x00fe, 0x00cb, 0x0027, 0x000c, 0x006d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0062, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0063, MON_VGA,  0x00fe, 0x00cb, 0x0027, 0x001c, 0x007d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0063, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0064, MON_VGA,  0x00fe, 0x00cb, 0x0027, 0x002e, 0x008f, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0064, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x000e, 0x00af, 0x00d8, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0068, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0068, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03af, 0x00e0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0069, MON_VGA,  0x003e, 0x0030, 0x0008, 0x023b, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0069, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x006a, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x004d, 0x00fe, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x006a, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x000d, 0x013e, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x006b, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x005d, 0x010e, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x006b, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x001d, 0x014e, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x0082, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0088, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x0088, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0089, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00a2, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00a8, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x00a8, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x007f, 0x020f, 0x026b },
    { 0x00a9, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00c2, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00c8, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x00c8, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x00c8, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03b0, 0x00df, 0x01b4, 0x020d, 0x0201, 0x0016, 0x0025, 0x0205, 0x0207 },
    { 0x00c9, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x00e2, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x00e8, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x00e8, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x00e8, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03af, 0x00e0, 0x01b2, 0x0271, 0x0265, 0x002f, 0x0057, 0x0237, 0x026b },
    { 0x00e9, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x0101, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0102, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x000c, 0x006d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0103, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x001c, 0x007d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0104, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x002e, 0x008f, 0x00d8, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0108, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0109, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x010a, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x004d, 0x00fd, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x010b, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x005d, 0x010d, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0121, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0122, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x000c, 0x006d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0123, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x001c, 0x007d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0124, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x002e, 0x008f, 0x00d8, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0128, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0129, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0002, 0x0020, 0x0034, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x012a, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x004d, 0x00fe, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x012b, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x005d, 0x010e, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x0141, MON_VGA,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0141, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0142, MON_VGA,  0x00fe, 0x00c9, 0x0027, 0x000c, 0x006d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0142, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0143, MON_VGA,  0x00fe, 0x00c9, 0x0027, 0x001c, 0x007d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0143, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0144, MON_VGA,  0x00fe, 0x00c9, 0x0027, 0x002e, 0x008f, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0144, MON_ALL,  0x00fe, 0x00c9, 0x0027, 0x000e, 0x00af, 0x00d8, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0148, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0148, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03b0, 0x00df, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0149, MON_VGA,  0x003e, 0x0030, 0x0008, 0x023b, 0x001c, 0x0034, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0149, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x014a, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x004d, 0x00fd, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x014a, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x000d, 0x013d, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x014b, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x005d, 0x010d, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x014b, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x001d, 0x014d, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x0161, MON_VGA,  0x003e, 0x0030, 0x0008, 0x0239, 0x0012, 0x0034, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0161, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0232, 0x001b, 0x0034, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0162, MON_VGA,  0x00fe, 0x00cb, 0x0027, 0x000c, 0x006d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0162, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x02ec, 0x008d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0163, MON_VGA,  0x00fe, 0x00cb, 0x0027, 0x001c, 0x007d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0163, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x02fc, 0x009d, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0164, MON_VGA,  0x00fe, 0x00cb, 0x0027, 0x002e, 0x008f, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0164, MON_ALL,  0x00fe, 0x00cb, 0x0027, 0x000e, 0x00af, 0x00d8, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0168, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0168, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03af, 0x00e0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0169, MON_VGA,  0x003e, 0x0030, 0x0008, 0x023b, 0x001c, 0x0034, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0169, MON_ALL,  0x003e, 0x0030, 0x0008, 0x0237, 0x0020, 0x0034, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x016a, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x004d, 0x00fe, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x016a, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x000d, 0x013e, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x016b, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x005d, 0x010e, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x016b, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x001d, 0x014e, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x0182, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x0188, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x0188, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020c, 0x0201, 0x0016, 0x004c, 0x01dc, 0x0207 },
    { 0x0189, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x01a2, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x01a8, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x01a8, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x007e, 0x020e, 0x026b },
    { 0x01a9, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x01c2, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x01c8, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x01c8, MON_VGA,  0x01ff, 0x0197, 0x0050, 0x03f0, 0x009f, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x01c8, MON_ALL,  0x01ff, 0x0197, 0x0050, 0x03b0, 0x00df, 0x01b4, 0x020c, 0x0201, 0x0016, 0x0024, 0x0204, 0x0207 },
    { 0x01c9, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x020d, 0x0201, 0x0016, 0x004d, 0x01dd, 0x0207 },
    { 0x01e2, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { 0x01e8, MON_MONO, 0x001a, 0x0000, 0x0000, 0x020f, 0x000c, 0x0014, 0x03e9, 0x0000, 0x0000, 0x0043, 0x0363, 0x03e7 },
    { 0x01e8, MON_VGA,  0x01fe, 0x0199, 0x0050, 0x03ef, 0x00a0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x01e8, MON_ALL,  0x01fe, 0x0199, 0x0050, 0x03af, 0x00e0, 0x01b2, 0x0270, 0x0265, 0x002f, 0x0056, 0x0236, 0x026b },
    { 0x01e9, MON_ALL,  0x003e, 0x0032, 0x0009, 0x023f, 0x001c, 0x0034, 0x0271, 0x0265, 0x002f, 0x006f, 0x01ff, 0x026b },
    { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

static const VMODE_ENTRY *lookup_videl_mode(WORD mode,WORD monitor);
#endif


/* get monitor type (same encoding as VgetMonitor()) */
static WORD get_monitor_type()
{
#if CONF_WITH_VIDEL
    if (has_videl) {
        return vmontype();
    }
    else
#endif
    {
        volatile UBYTE *gpip = (UBYTE *)0xfffffa01;

        if (*gpip & 0x80)       /* colour monitor */
            return 1;

        return 0;               /* monochrome monitor */
    }
}


/* xbios routines */

#if CONF_WITH_TT_SHIFTER

/*
 * TT shifter functions
 */

/*
 * Set TT shifter mode
 */
WORD esetshift(WORD mode)
{
    volatile WORD *resreg = (WORD *)TT_SHIFTER;
    WORD oldmode;

    if (!has_tt_shifter)
        return -32;

    oldmode = *resreg & TT_SHIFTER_BITMASK;
    *resreg = mode & TT_SHIFTER_BITMASK;

    return oldmode;
}


/*
 * Get TT shifter mode
 */
WORD egetshift(void)
{
    if (!has_tt_shifter)
        return -32;

    return *(volatile WORD *)TT_SHIFTER & TT_SHIFTER_BITMASK;
}


/*
 * Read/modify TT shifter colour bank number
 */
WORD esetbank(WORD bank)
{
    volatile UBYTE *shiftreg = (UBYTE *)(TT_SHIFTER+1);
    UBYTE old;

    if (!has_tt_shifter)
        return -32;

    old = *shiftreg & 0x0f;
    if (bank >= 0)
        *shiftreg = bank & 0x0f;

    return old;
}


/*
 * Read/modify TT palette colour entry
 */
WORD esetcolor(WORD index,WORD color)
{
    volatile WORD *ttcol_regs = (WORD *) TT_PALETTE_REGS;
    WORD oldcolor;

    if (!has_tt_shifter)
        return -32;

    index &= 0xff;                  /* force valid index number */
    oldcolor = ttcol_regs[index] & TT_PALETTE_BITMASK;
    if (color >= 0)
        ttcol_regs[index] = color & TT_PALETTE_BITMASK;

    return oldcolor;
}


/*
 * Set multiple TT palette colour registers
 */
void esetpalette(WORD index,WORD count,WORD *rgb)
{
    volatile WORD *ttcolour;

    if (!has_tt_shifter)
        return;

    index &= 0xff;              /* force valid index number */

    if ((index+count) > 256)
        count = 256 - index;    /* force valid count */

    ttcolour = (WORD *)TT_PALETTE_REGS + index;
    while(count--)
        *ttcolour++ = *rgb++ & TT_PALETTE_BITMASK;
}


/*
 * Get multiple TT palette colour registers
 */
void egetpalette(WORD index,WORD count,WORD *rgb)
{
    volatile WORD *ttcolour;

    if (!has_tt_shifter)
        return;

    index &= 0xff;              /* force valid index number */

    if ((index+count) > 256)
        count = 256 - index;    /* force valid count */

    ttcolour = (WORD *)TT_PALETTE_REGS + index;
    while(count--)
        *rgb++ = *ttcolour++ & TT_PALETTE_BITMASK;
}


/*
 * Read/modify TT shifter grey mode bit
 */
WORD esetgray(WORD mode)
{
    volatile UBYTE *shiftreg = (UBYTE *)TT_SHIFTER;
    UBYTE old;

    if (!has_tt_shifter)
        return -32;

    old = *shiftreg;
    if (mode > 0)
        *shiftreg = old | 0x10;
    else if (mode == 0)
        *shiftreg = old & 0xef;

    return (old&0x10)?1:0;
}


/*
 * Read/modify TT shifter smear mode bit
 */
WORD esetsmear(WORD mode)
{
    volatile UBYTE *shiftreg = (UBYTE *)TT_SHIFTER;
    UBYTE old;

    if (!has_tt_shifter)
        return -32;

    old = *shiftreg;
    if (mode > 0)
        *shiftreg = old | 0x80;
    else if (mode == 0)
        *shiftreg = old & 0x7f;

    return (old&0x80)?1:0;
}

#endif /* CONF_WITH_TT_SHIFTER */


#if CONF_WITH_VIDEL

/*
 * functions for VIDEL programming
 */

UWORD get_videl_bpp(void)
{
    UWORD f_shift = *(volatile UWORD *)SPSHIFT;
    UBYTE st_shift = *(volatile UBYTE *)ST_SHIFTER;
    /* to get bpp, we must examine f_shift and st_shift.
     * f_shift is valid if any of bits no. 10, 8 or 4
     * is set. Priority in f_shift is: 10 ">" 8 ">" 4, i.e.
     * if bit 10 set then bit 8 and bit 4 don't care...
     * If all these bits are 0 get display depth from st_shift
     * (as for ST and STE)
     */
    int bits_per_pixel = 1;
    if (f_shift & 0x400)         /* 2 colors */
        bits_per_pixel = 1;
    else if (f_shift & 0x100)    /* hicolor */
        bits_per_pixel = 16;
    else if (f_shift & 0x010)    /* 8 bitplanes */
        bits_per_pixel = 8;
    else if (st_shift == 0)
        bits_per_pixel = 4;
    else if (st_shift == 0x1)
        bits_per_pixel = 2;
    else /* if (st_shift == 0x2) */
        bits_per_pixel = 1;

    return bits_per_pixel;
}

UWORD get_videl_width(void)
{
    return (*(volatile UWORD *)0xffff8210) * 16 / get_videl_bpp();
}

UWORD get_videl_height(void)
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
 * lookup videl initialisation data for specified mode/monitor
 * returns NULL if mode/monitor combination is invalid
 */
static const VMODE_ENTRY *lookup_videl_mode(WORD mode,WORD monitor)
{
    const VMODE_ENTRY *p;

    for (p = vmode_init_table; p->vmode >= 0; p++)
        if (p->vmode == mode)
            if ((p->monitor == MON_ALL) || (p->monitor == monitor))
                return p;

    return NULL;
}


/*
 * determine scanline width based on video mode
 */
WORD determine_width(WORD mode)
{
    WORD linewidth;

    linewidth = (mode&VIDEL_80COL) ? 40 : 20;
    linewidth <<= (mode & VIDEL_BPPMASK);
    if (mode&VIDEL_OVERSCAN)
        linewidth = linewidth * 12 / 10;    /* multiply by 1.2 */

    return linewidth;
}


/*
 * determine vctl based on video mode and monitor type
 */
WORD determine_vctl(WORD mode,WORD monitor)
{
    WORD bppcode, vctl;

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

    bppcode = mode & VIDEL_BPPMASK;

    if (mode&VIDEL_VGA) {
        switch(bppcode) {
        case VIDEL_1BPP:
            break;
        case VIDEL_4BPP:
            vctl = 0x04;
            break;
        default:
            vctl = 0x08;
        }
    } else {
        switch(bppcode) {
        case VIDEL_1BPP:
            if (monitor == MON_MONO)
                vctl = 0x08;
            break;
        case VIDEL_4BPP:
            vctl = 0x00;
            break;
        default:
            vctl = 0x04;
        }
    }

    return vctl;
}


/*
 * determine regc0 based on video mode & monitor type
 */
WORD determine_regc0(WORD mode,WORD monitor)
{
    if (mode&VIDEL_VGA)
        return 0x0186;

    if (!(mode&VIDEL_COMPAT))
        return (monitor==MON_TV)?0x0183:0x0181;

    /* handle ST-compatible modes */
    if ((mode&(VIDEL_80COL|VIDEL_BPPMASK)) == (VIDEL_80COL|VIDEL_1BPP)) {  /* 80-column, 2-colour */
        switch(monitor) {
        case MON_MONO:
            return 0x0080;
        case MON_TV:
            return 0x0183;
        default:
            return 0x0181;
        }
    }

    return (monitor==MON_TV)?0x0083:0x0081;
}


/*
 * this routine can set VIDEL to 1,2,4 or 8 bitplanes mode on VGA
 */
static int set_videl_vga(WORD mode)
{
    volatile char *videlregs = (char *)0xffff8200;
#define videlword(n) (*(volatile UWORD *)(videlregs+(n)))
    const VMODE_ENTRY *p;
    WORD linewidth, monitor, vctl;

    monitor = vmontype();

    p = lookup_videl_mode(mode,monitor);/* validate mode */
    if (!p)
        return -1;

    videlregs[0x0a] = (mode&VIDEL_PAL) ? 2 : 0; /* video sync to 50Hz if PAL */

    if (frclock)        /* if we're not being called during initialisation, */
        vsync();        /* wait for vbl so we're not interrupted :-) */

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

    videlregs[0x60] = 0x00;				/* clear ST shift for safety */

    videlword(0x0e) = 0;                /* offset */

    linewidth = determine_width(mode);
    vctl = determine_vctl(mode,monitor);

    videlword(0x10) = linewidth;        /* scanline width */
    videlword(0xc2) = vctl;             /* video control */
    videlword(0xc0) = determine_regc0(mode,monitor);
    videlword(0x66) = 0x0000;           /* clear SPSHIFT */

    switch(mode&VIDEL_BPPMASK) {        /* set SPSHIFT / ST shift */
    case VIDEL_1BPP:                    /* 2 colours (mono) */
        if (monitor == MON_MONO)
            videlregs[0x60] = 0x02;
        else videlword(0x66) = 0x0400;
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
static WORD current_video_mode;

/*
 * Set Falcon video mode
 */
WORD vsetmode(WORD mode)
{
    WORD ret;

    if (!has_videl)
        return -32;

    if (mode == -1)
        return current_video_mode;

#if DBG_SCREEN
    kprintf("vsetmode(0x%04x)\n", mode);
#endif

    if (set_videl_vga(mode) < 0)    /* invalid mode */
        return current_video_mode;

    ret = current_video_mode;
    current_video_mode = mode;

    return ret;
}

/*
 * Get Videl monitor type
 */
WORD vmontype(void)
{
    if (!has_videl)
        return -32;

    return ((*(volatile UBYTE *)0xffff8006) >> 6) & 3;
}

/*
 * Set external video sync mode
 */
void vsetsync(WORD external)
{
    UWORD spshift;

    if (!has_videl)
        return;

    if (external & 0x01)            /* external clock wanted? */
        *(volatile BYTE *)SYNCMODE |= 0x01;
    else *(volatile BYTE *)SYNCMODE &= 0xfe;

    spshift = *(volatile UWORD *)SPSHIFT;

    if (external&0x02)              /* external vertical sync wanted? */
        spshift |= 0x0020;
    else spshift &= 0xffdf;

    if (external&0x04)              /* external horizontal sync wanted? */
        spshift |= 0x0040;
    else spshift &= 0xffbf;

    *(volatile UWORD *)SPSHIFT = spshift;
}

/*
 * get video ram size according to mode
 */
LONG vgetsize(WORD mode)
{
    const VMODE_ENTRY *p;
    int height;
    WORD vctl, monitor;

    if (!has_videl)
        return -32;

    monitor = vmontype();

    mode &= VIDEL_VALID;        /* ignore invalid bits */
    if ((mode&VIDEL_BPPMASK) > VIDEL_TRUECOLOR) {   /* fixup invalid bpp */
        mode &= ~VIDEL_BPPMASK;
        mode |= VIDEL_TRUECOLOR;
    }

    p = lookup_videl_mode(mode,monitor);
    if (!p) {                   /* invalid mode */
        if (mode&VIDEL_COMPAT)
            return ST_VRAM_SIZE;
        mode &= ~(VIDEL_OVERSCAN|VIDEL_PAL);/* ignore less-important bits */
        p = lookup_videl_mode(mode,monitor);/* & try again */
        if (!p)                             /* "can't happen" */
            return FALCON_VRAM_SIZE;
    }

    vctl = determine_vctl(mode,monitor);
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
static void convert2ste(WORD *ste,LONG *falcon)
{
    union {
        LONG l;
        UBYTE b[4];
    } u;
    int i;

    for (i = 0; i < 16; i++) {
        u.l = *falcon++;
        *ste++ = (falc2ste(u.b[0])<<8) | (falc2ste(u.b[1])<<4) | falc2ste(u.b[3]);
    }
}

/*
 * determine whether to update STe or Falcon h/w palette registers
 * returns TRUE if we need to update the STE h/w palette
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
void vsetrgb(WORD index,WORD count,LONG *rgb)
{
    LONG *shadow, *source;
    union {
        LONG l;
        UBYTE b[4];
    } u;
    WORD limit;

    if (!has_videl)
        return;

    if ((index < 0) || (count <= 0))
        return;

    limit = (get_videl_bpp()<=4) ? 16 : 256;
    if ((index+count) > limit)
        return;

    /*
     * we always update the Falcon shadow palette, since that's
     * what we'll return for VgetRGB()
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

    /*
     * for ST low or 4-colour modes, we need to convert the
     * Falcon shadow registers to STe palette register format, and
     * request the VBL interrupt handler to update the STe palette
     * registers rather than the Falcon registers
     */
    if (use_ste_palette(vsetmode(-1))) {
        convert2ste(ste_shadow_palette,falcon_shadow_palette);
        colorptr = ste_shadow_palette;
        return;
    }

    colorptr = (limit==256) ? (WORD *)0x01L : (WORD *)((LONG)falcon_shadow_palette|0x01L);
}

/*
 * get palette registers
 */
void vgetrgb(WORD index,WORD count,LONG *rgb)
{
    LONG *shadow;
    union {
        LONG l;
        UBYTE b[4];
    } u;
    WORD limit;

    if (!has_videl)
        return;

    if ((index < 0) || (count <= 0))
        return;

    limit = (get_videl_bpp()<=4) ? 16 : 256;
    if ((index+count) > limit)
        return;

    shadow = falcon_shadow_palette + index;
    while(count--) {
        u.l = *shadow++;
        u.b[2] = u.b[1];        /* shift R & G right*/
        u.b[1] = u.b[0];
        u.b[0] = 0x00;
        *rgb++ = u.l;
    }
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
        mode &= ~VIDEL_OVERSCAN;    /* turn off overscan (not used with VGA) */
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

#endif /* CONF_WITH_VIDEL */

#endif /* CONF_WITH_SHIFTER */

#if CONF_WITH_SHIFTER
/*
 * Initialise ST(e) palette registers
 */
static void initialise_ste_palette(WORD mask)
{
    volatile WORD *col_regs = (WORD *) ST_PALETTE_REGS;
    int i;

    for (i = 0; i < 16; i++)
        col_regs[i] = dflt_palette[i] & mask;
}

/*
 * Fixup ST(e) palette registers
 */
static void fixup_ste_palette(WORD rez)
{
    volatile WORD *col_regs = (WORD *) ST_PALETTE_REGS;

    if (rez == ST_MEDIUM)
        col_regs[3] = col_regs[15];
    else if (rez == ST_HIGH)
        col_regs[1] = col_regs[15];
}

#if CONF_WITH_TT_SHIFTER
/*
 * Initialise TT palette
 */
static void initialise_tt_palette(WORD rez)
{
    volatile WORD *col_regs = (WORD *) ST_PALETTE_REGS;
    volatile WORD *ttcol_regs = (WORD *) TT_PALETTE_REGS;
    int i;

    for (i = 0; i < 256; i++)
        ttcol_regs[i] = tt_dflt_palette[i];

    if (rez == TT_HIGH) {
        col_regs[1] = col_regs[15];
        ttcol_regs[1] = ttcol_regs[15];
    }
}
#endif

#if CONF_WITH_VIDEL
/*
 * Initialise Falcon palette
 */
static void initialise_falcon_palette(WORD mode)
{
    volatile WORD *col_regs = (WORD *) ST_PALETTE_REGS;
    volatile LONG *fcol_regs = (LONG *) FALCON_PALETTE_REGS;
    int i;

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

    for (i = 0; i < 256; i++)
        fcol_regs[i] = falcon_shadow_palette[i];

    /*
     * if appropriate, set up the STe shadow & real palette registers
     */
    if (use_ste_palette(mode)) {
        convert2ste(ste_shadow_palette,falcon_shadow_palette);
        for (i = 0; i < 16; i++)
            col_regs[i] = ste_shadow_palette[i];
    }
}
#endif
#endif

/*
 * Get videl mode
 * This is the same as vsetmode(-1) except that it returns
 * zero when there is no videl.  Used by app_save().
 */
WORD get_videl_mode(void)
{
#if CONF_WITH_VIDEL
    if (has_videl)
        return vsetmode(-1);
#endif
    return 0;
}

/*
 * Check specified mode/rez to see if we should change; used in early
 * emudesk.inf processing.  We only indicate a change if all of the
 * following are true:
 *  . the current monitor is not mono
 *  . the specified & current values are the same type (both modes
 *    or both rezs)
 *  . the specified value differs from the current value
 * If these are all true, we return the new mode/rez; otherwise we
 * return zero.
 *
 * Mode/rez values are encoded as follows:
 *      0xFFnn: ST/TT resolution nn
 *      otherwise, Falcon mode value
 */
WORD check_moderez(WORD moderez)
{
WORD current_mode, return_mode, return_rez;
int tt;

    if (get_monitor_type() == MON_MONO)
        return 0;

    current_mode = get_videl_mode();
    if (current_mode) {
        if (moderez < 0)                    /* ignore rez values */
            return 0;
        return_mode = vfixmode(moderez);    /* adjust */
        return (return_mode==current_mode)?0:return_mode;
    }

    /* handle old-fashioned rez :-) */
    if (moderez > 0)                        /* ignore mode values */
        return 0;

    tt = 0;
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        tt = 1;
#endif

    return_rez = moderez & 0x00ff;
    if (tt) {
        if (return_rez == TT_HIGH)
            return_rez = TT_MEDIUM;
    } else {
        if (return_rez > ST_MEDIUM)
            return_rez = ST_MEDIUM;
    }

    return (return_rez==getrez())?0:(0xff00|return_rez);
}

/*
 * Initialise palette registers
 * This routine is also used by resolution change
 */
void initialise_palette_registers(WORD rez,WORD mode)
{
#if CONF_WITH_SHIFTER
WORD mask;

#if CONF_WITH_VIDEL
    if (has_videl)
        mask = 0x0fff;
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        mask = 0x0fff;
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter)
        mask = 0x0fff;
    else
#endif
    {
        mask = 0x0777;
	}
    initialise_ste_palette(mask);

#if CONF_WITH_VIDEL
    if (has_videl)
        initialise_falcon_palette(mode);
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        initialise_tt_palette(rez);
    else
#endif
    {
    }

    fixup_ste_palette(rez);
#endif
}

/*
 * In the original TOS there used to be an early screen init, 
 * before memory configuration. This is not used here, and all is
 * done at the same time from C.
 */

void screen_init(void)
{
    ULONG screen_start;
#if CONF_WITH_SHIFTER
    volatile BYTE *rez_reg = (BYTE *) ST_SHIFTER;
    UWORD boot_resolution = FALCON_DEFAULT_BOOT;
#if CONF_WITH_TT_SHIFTER
    volatile BYTE *ttrez_reg = (BYTE *) TT_SHIFTER;
#endif
    WORD monitor_type, sync_mode;
    WORD rez = 0;   /* avoid 'may be uninitialized' warning */

/*
 * first, see what we're connected to, and set the
 * resolution / video mode appropriately
 */
    monitor_type = get_monitor_type();
#if DBG_SCREEN
    kprintf("monitor_type = %d\n", monitor_type);
#endif

#if CONF_WITH_VIDEL
    if (has_videl) {
        WORD ret;

        UNUSED(ret);

        /* reset VIDEL on boot-up */
        /* first set the physbase to a safe memory */
#if CONF_VRAM_ADDRESS
        setphys(CONF_VRAM_ADDRESS,0);
#else
        setphys(0x10000L,0);
#endif

#if CONF_WITH_NVRAM && !defined(MACHINE_FIREBEE)
        /* get boot resolution from NVRAM */
        ret = nvmaccess(0, 14, 2, (PTR)&boot_resolution);
        if (ret != 0) {
#if DBG_SCREEN
            kprintf("Invalid NVRAM, defaulting to boot video mode 0x%04x\n", boot_resolution);
#endif
        }
        else {
#if DBG_SCREEN
            kprintf("NVRAM boot video mode is 0x%04x\n", boot_resolution);
#endif
        }
#endif // CONF_WITH_NVRAM

        if (!lookup_videl_mode(boot_resolution,monitor_type)) { /* mode isn't in table */
#if DBG_SCREEN
            kprintf("Invalid video mode 0x%04x changed to 0x%04x\n",
                    boot_resolution,FALCON_DEFAULT_BOOT);
#endif
            boot_resolution = FALCON_DEFAULT_BOOT;  /* so pick one that is */
        }

        /* initialise the current video mode, for vfixmode()/vsetmode() */
        current_video_mode = boot_resolution;

        /* fix the video mode according to the actual monitor */
        boot_resolution = vfixmode(boot_resolution);
#if DBG_SCREEN
        kprintf("Fixed boot video mode is 0x%04x\n", boot_resolution);
#endif
        vsetmode(boot_resolution);
        rez = FALCON_REZ;   /* fake value indicates Falcon/Videl */
    }
    else
#endif // CONF_WITH_VIDEL
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        rez = monitor_type?TT_MEDIUM:TT_HIGH;
        *ttrez_reg = rez;
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        rez = monitor_type?ST_LOW:ST_HIGH;
        *rez_reg = rez;
    }
    else
#endif
    {
        rez = monitor_type?ST_LOW:ST_HIGH;
        *rez_reg = rez;
    }

#if CONF_WITH_VIDEL
    if (rez == FALCON_REZ) {    /* detected a Falcon */
        sync_mode = (boot_resolution&VIDEL_PAL)?0x02:0x00;
    }
    else
#endif
    {
        sync_mode = (os_pal&0x01)?0x02:0x00;
    }
    *(volatile BYTE *) SYNCMODE = sync_mode;

/*
 * next, set up the palette(s)
 */
    initialise_palette_registers(rez,boot_resolution);
    sshiftmod = rez;

#endif /* CONF_WITH_SHIFTER */

#if CONF_VRAM_ADDRESS
    screen_start = CONF_VRAM_ADDRESS;
#else
    /* videoram is placed just below the phystop */
    screen_start = (ULONG)phystop - initial_vram_size();
    /* round down to 256 byte boundary */
    screen_start &= 0x00ffff00;
    /* Original TOS leaves a gap of 768 bytes between screen ram and phys_top...
     * ... we normally don't need that, but some old software relies on that fact,
     * so we use this gap, too. */
#if CONF_WITH_VIDEL
    if (has_videl)
        ;
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        ;
    else
#endif
    {
        screen_start -= 0x300;
    }
#endif /* CONF_VRAM_ADDRESS */
    /* set new v_bas_ad */
    v_bas_ad = (UBYTE *)screen_start;
    /* correct physical address */
    setphys(screen_start,1);
}

/* calculate initial VRAM size based on video hardware */
static unsigned long initial_vram_size()
{
#if CONF_WITH_VIDEL
    if (has_videl) {
        return FALCON_VRAM_SIZE;
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        return TT_VRAM_SIZE;
    }
    else
#endif
    return ST_VRAM_SIZE;
}

/* calculate VRAM size for current video mode */
static unsigned long vram_size()
{
#if CONF_WITH_VIDEL
    if (has_videl) {
        return get_videl_width() / 8L * get_videl_height() * get_videl_bpp();
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        WORD rez = getrez();
        if (rez >= 4)
            return TT_VRAM_SIZE;
        return ST_VRAM_SIZE;
    }
    else
#endif
    return ST_VRAM_SIZE;
}

/* hardware independant xbios routines */

LONG physbase(void)
{
    LONG addr;

#if CONF_WITH_SHIFTER
    addr = *(volatile UBYTE *) VIDEOBASE_ADDR_HI;
    addr <<= 8;
    addr += *(volatile UBYTE *) VIDEOBASE_ADDR_MID;
    addr <<= 8;
#if CONF_WITH_VIDEL
    if (has_videl) {
        addr += *(volatile UBYTE *) VIDEOBASE_ADDR_LOW;
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        addr += *(volatile UBYTE *) VIDEOBASE_ADDR_LOW;
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        addr += *(volatile UBYTE *) VIDEOBASE_ADDR_LOW;
    }
#endif
#else /* CONF_WITH_SHIFTER */
    /* No real physical screen, fall back to Logbase() */
    addr = logbase();
#endif /* #if CONF_WITH_SHIFTER */

    return (addr);
}

/* Set physical screen address */

static void setphys(LONG addr,int checkaddr)
{
    if (checkaddr) {
        if (addr > ((ULONG)phystop - vram_size())) {
            panic("VideoRAM covers ROM area!!\n");
        }
    }

#if CONF_WITH_SHIFTER
    *(volatile UBYTE *) VIDEOBASE_ADDR_HI = ((ULONG) addr) >> 16;
    *(volatile UBYTE *) VIDEOBASE_ADDR_MID = ((ULONG) addr) >> 8;
#if CONF_WITH_VIDEL
    if (has_videl) {
        *(volatile UBYTE *) VIDEOBASE_ADDR_LOW = ((ULONG) addr);
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        *(volatile UBYTE *) VIDEOBASE_ADDR_LOW = ((ULONG) addr);
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        *(volatile UBYTE *) VIDEOBASE_ADDR_LOW = ((ULONG) addr);
    }
#endif
#endif /* CONF_WITH_SHIFTER */
}

LONG logbase(void)
{
    return ((ULONG) v_bas_ad);
}

WORD getrez(void)
{
    UBYTE rez;

#if CONF_WITH_SHIFTER
#if CONF_WITH_VIDEL
    if (has_videl) {
        /* Get the video mode for Falcon-hardware */
        WORD vmode = vsetmode(-1);
        if (vmode & VIDEL_COMPAT) {
            switch(vmode&VIDEL_BPPMASK) {
            case VIDEL_1BPP:
                rez = 2;
                break;
            case VIDEL_2BPP:
                rez = 1;
                break;
            case VIDEL_4BPP:
                rez = 0;
                break;
            default:
                kprintf("Problem - unsupported video mode\n");
                rez = 0;
            }
        } else
            rez = 2;
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        /* Get the video mode for TT-hardware */
        rez = *(volatile UBYTE *)TT_SHIFTER & 0x07;
    }
    else
#endif
    {
        rez = *(volatile UBYTE *)ST_SHIFTER & 0x03;
    }
#else /* CONF_WITH_SHIFTER */
    /* No video hardware, default to some standard video mode */
    rez = ST_LOW;
#endif /* CONF_WITH_SHIFTER */

    return rez;
}


void setscreen(LONG logLoc, LONG physLoc, WORD rez, WORD videlmode)
{
    if (logLoc >= 0) {
        v_bas_ad = (UBYTE *) logLoc;
    }
    if (physLoc >= 0) {
        setphys(physLoc,1);
    }
    if (rez >= 0 && rez < 8) {
        if (FALSE) {
            /* Dummy case for conditional compilation */
        }
#if CONF_WITH_VIDEL
        else if (has_videl) {
            if (rez == FALCON_REZ) {
                vsetmode(videlmode);
                sshiftmod = rez;
            } else if (rez < 3) {   /* ST compatible resolution */
                *(volatile UWORD *)SPSHIFT = 0;
                *(volatile UBYTE *)ST_SHIFTER = sshiftmod = rez;
            }
        }
#endif
#if CONF_WITH_TT_SHIFTER
        else if (has_tt_shifter) {
            if ((rez != 3) && (rez != 5))
                *(volatile UBYTE *)TT_SHIFTER = sshiftmod = rez;
        }
#endif
#if CONF_WITH_SHIFTER
        else if (rez < 3) {      /* ST resolution */
            *(volatile UBYTE *)ST_SHIFTER = sshiftmod = rez;
        }
#endif

        /* Re-initialize line-a, VT52 etc: */
        linea_init();
        font_set_default();
        vt52_init();
    }
}

void setpalette(LONG palettePtr)
{
#if DBG_SCREEN
    int i, max;
    WORD *p = (WORD *)palettePtr;
    max = getrez() == 0 ? 15 : getrez() == 1 ? 3 : 1;
    kprintf("Setpalette(");
    for(i = 0 ; i <= max ; i++) {
        kprintf("%03x", p[i]);
        if(i < 15) kprintf(" ");
    }
    kprintf(")\n");
#endif
    /* next VBL will do this */
    colorptr = (WORD *) palettePtr;
}

WORD setcolor(WORD colorNum, WORD color)
{
    WORD oldcolor;
#if CONF_WITH_SHIFTER
    WORD mask;
    volatile WORD *palette = (WORD *) ST_PALETTE_REGS;
#if CONF_WITH_TT_SHIFTER
    volatile WORD *ttpalette = (WORD *) TT_PALETTE_REGS;
#endif

#if DBG_SCREEN
    kprintf("Setcolor(0x%04x, 0x%04x)\n", colorNum, color);
#endif

    colorNum &= 0x000f;         /* just like real TOS */

#if CONF_WITH_VIDEL
    if (has_videl) {
        mask = 0xfff;
    } else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        mask = 0xfff;
    } else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        mask = 0xfff;
    } else
#endif
    {
        mask = 0x777;
    }

    oldcolor = palette[colorNum] & mask;
    if (color == -1)
        return oldcolor;

#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
		WORD msb, lsb;
        msb = (color << 1) & 0x0eee;    /* move most significant bits to left */
        lsb = (color >> 3) & 0x0111;    /* move least significant bit to right */
        ttpalette[colorNum] = msb | lsb;/* update TT-compatible palette */
    }
#endif

    palette[colorNum] = color;          /* update ST(e)-compatible palette */
#else /* CONF_WITH_SHIFTER */
    /* No hardware, fake value */
    oldcolor = 0;
#endif /* CONF_WITH_SHIFTER */
    return oldcolor;
}


void vsync(void)
{
    LONG a;
#if CONF_WITH_SHIFTER
    WORD old_sr = set_sr(0x2300);       /* allow VBL interrupt */
#endif

    a = frclock;
    while (frclock == a) {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        stop_until_interrupt();
#endif
        /* Wait */
    }

#if CONF_WITH_SHIFTER
    set_sr(old_sr);
#endif /* CONF_WITH_SHIFTER */
}
