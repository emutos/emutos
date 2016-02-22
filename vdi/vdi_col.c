/*
 * vdi_col.c - VDI color palette functions and tables.
 *
 * Copyright (c) 2005-2013 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "intmath.h"
#include "vdi_defs.h"
#include "string.h"
#include "machine.h"
#include "xbiosbind.h"
#include "vdi_col.h"
#include "lineavars.h"
#include "screen.h"

#define EXTENDED_PALETTE (CONF_WITH_VIDEL || CONF_WITH_TT_SHIFTER)

#if EXTENDED_PALETTE
#define MAXCOLOURS  256
#else
#define MAXCOLOURS  16
#endif

/* Some color mapping tables */
WORD MAP_COL[MAXCOLOURS];       /* maps vdi pen -> hardware register */
WORD REV_MAP_COL[MAXCOLOURS];   /* maps hardware register -> vdi pen */

static const WORD MAP_COL_ROM[] =
    { 0, 15, 1, 2, 4, 6, 3, 5, 7, 8, 9, 10, 12, 14, 11, 13 };

/*
 * the following factors are used in adjust_mono_values()
 */
#define STE_MONO_FUDGE_FACTOR   0x43
#define ST_MONO_FUDGE_FACTOR    0x8e

#if EXTENDED_PALETTE
/* req_col2 contains the VDI color palette entries 16 - 255 for vq_color().
 * To stay compatible with the line-a variables, only entries > 16 are
 * stored in this array, the first 16 entries are stored in REQ_COL */
static WORD req_col2[240][3];
#endif

/* Initial color palettes */
static const WORD st_palette[16][3] =
{
    { 1000, 1000, 1000 },
    { 0, 0, 0 },
    { 1000, 0, 0 },
    { 0, 1000, 0 },
    { 0, 0, 1000 },
    { 0, 1000, 1000 },
    { 1000, 1000, 0 },
    { 1000, 0, 1000 },
    { 714, 714, 714 },
    { 428, 428, 428 },
    { 1000, 428, 428 },
    { 428, 1000, 428 },
    { 428, 428, 1000 },
    { 428, 1000, 1000 },
    { 1000, 1000, 428 },
    { 1000, 428, 1000 }
};
#if CONF_WITH_TT_SHIFTER
static const WORD tt_palette1[16][3] =
{
    { 1000, 1000, 1000 }, { 0, 0, 0 }, { 1000, 0, 0 }, { 0, 1000, 0 },
    { 0, 0, 1000 }, { 0, 1000, 1000 }, { 1000, 1000, 0 }, { 1000, 0, 1000 },
    { 667, 667, 667 }, { 400, 400, 400 }, { 1000, 600, 600 }, { 600, 1000, 600 },
    { 600, 600, 1000 }, { 600, 1000, 1000 }, { 1000, 1000, 600 }, { 1000, 600, 1000 }
};
static const WORD tt_palette2[240][3] =
{
    { 1000, 1000, 1000 }, { 933, 933, 933 }, { 867, 867, 867 }, { 800, 800, 800 },
    { 733, 733, 733 }, { 667, 667, 667 }, { 600, 600, 600 }, { 533, 533, 533 },
    { 467, 467, 467 }, { 400, 400, 400 }, { 333, 333, 333 }, { 267, 267, 267 },
    { 200, 200, 200 }, { 133, 133, 133 }, { 67, 67, 67 }, { 0, 0, 0 },
    { 1000, 0, 0 }, { 1000, 0, 67 }, { 1000, 0, 133 }, { 1000, 0, 200 },
    { 1000, 0, 267 }, { 1000, 0, 333 }, { 1000, 0, 400 }, { 1000, 0, 467 },
    { 1000, 0, 533 }, { 1000, 0, 600 }, { 1000, 0, 667 }, { 1000, 0, 733 },
    { 1000, 0, 800 }, { 1000, 0, 867 }, { 1000, 0, 933 }, { 1000, 0, 1000 },
    { 933, 0, 1000 }, { 867, 0, 1000 }, { 800, 0, 1000 }, { 733, 0, 1000 },
    { 667, 0, 1000 }, { 600, 0, 1000 }, { 533, 0, 1000 }, { 467, 0, 1000 },
    { 400, 0, 1000 }, { 333, 0, 1000 }, { 267, 0, 1000 }, { 200, 0, 1000 },
    { 133, 0, 1000 }, { 67, 0, 1000 }, { 0, 0, 1000 }, { 0, 67, 1000 },
    { 0, 133, 1000 }, { 0, 200, 1000 }, { 0, 267, 1000 }, { 0, 333, 1000 },
    { 0, 400, 1000 }, { 0, 467, 1000 }, { 0, 533, 1000 }, { 0, 600, 1000 },
    { 0, 667, 1000 }, { 0, 733, 1000 }, { 0, 800, 1000 }, { 0, 867, 1000 },
    { 0, 933, 1000 }, { 0, 1000, 1000 }, { 0, 1000, 933 }, { 0, 1000, 867 },
    { 0, 1000, 800 }, { 0, 1000, 733 }, { 0, 1000, 667 }, { 0, 1000, 600 },
    { 0, 1000, 533 }, { 0, 1000, 467 }, { 0, 1000, 400 }, { 0, 1000, 333 },
    { 0, 1000, 267 }, { 0, 1000, 200 }, { 0, 1000, 133 }, { 0, 1000, 67 },
    { 0, 1000, 0 }, { 67, 1000, 0 }, { 133, 1000, 0 }, { 200, 1000, 0 },
    { 267, 1000, 0 }, { 333, 1000, 0 }, { 400, 1000, 0 }, { 467, 1000, 0 },
    { 533, 1000, 0 }, { 600, 1000, 0 }, { 667, 1000, 0 }, { 733, 1000, 0 },
    { 800, 1000, 0 }, { 867, 1000, 0 }, { 933, 1000, 0 }, { 1000, 1000, 0 },
    { 1000, 933, 0 }, { 1000, 867, 0 }, { 1000, 800, 0 }, { 1000, 733, 0 },
    { 1000, 667, 0 }, { 1000, 600, 0 }, { 1000, 533, 0 }, { 1000, 467, 0 },
    { 1000, 400, 0 }, { 1000, 333, 0 }, { 1000, 267, 0 }, { 1000, 200, 0 },
    { 1000, 133, 0 }, { 1000, 67, 0 }, { 733, 0, 0 }, { 733, 0, 67 },
    { 733, 0, 133 }, { 733, 0, 200 }, { 733, 0, 267 }, { 733, 0, 333 },
    { 733, 0, 400 }, { 733, 0, 467 }, { 733, 0, 533 }, { 733, 0, 600 },
    { 733, 0, 667 }, { 733, 0, 733 }, { 667, 0, 733 }, { 600, 0, 733 },
    { 533, 0, 733 }, { 467, 0, 733 }, { 400, 0, 733 }, { 333, 0, 733 },
    { 267, 0, 733 }, { 200, 0, 733 }, { 133, 0, 733 }, { 67, 0, 733 },
    { 0, 0, 733 }, { 0, 67, 733 }, { 0, 133, 733 }, { 0, 200, 733 },
    { 0, 267, 733 }, { 0, 333, 733 }, { 0, 400, 733 }, { 0, 467, 733 },
    { 0, 533, 733 }, { 0, 600, 733 }, { 0, 667, 733 }, { 0, 733, 733 },
    { 0, 733, 667 }, { 0, 733, 600 }, { 0, 733, 533 }, { 0, 733, 467 },
    { 0, 733, 400 }, { 0, 733, 333 }, { 0, 733, 267 }, { 0, 733, 200 },
    { 0, 733, 133 }, { 0, 733, 67 }, { 0, 733, 0 }, { 67, 733, 0 },
    { 133, 733, 0 }, { 200, 733, 0 }, { 267, 733, 0 }, { 333, 733, 0 },
    { 400, 733, 0 }, { 467, 733, 0 }, { 533, 733, 0 }, { 600, 733, 0 },
    { 667, 733, 0 }, { 733, 733, 0 }, { 733, 667, 0 }, { 733, 600, 0 },
    { 733, 533, 0 }, { 733, 467, 0 }, { 733, 400, 0 }, { 733, 333, 0 },
    { 733, 267, 0 }, { 733, 200, 0 }, { 733, 133, 0 }, { 733, 67, 0 },
    { 467, 0, 0 }, { 467, 0, 67 }, { 467, 0, 133 }, { 467, 0, 200 },
    { 467, 0, 267 }, { 467, 0, 333 }, { 467, 0, 400 }, { 467, 0, 467 },
    { 400, 0, 467 }, { 333, 0, 467 }, { 267, 0, 467 }, { 200, 0, 467 },
    { 133, 0, 467 }, { 67, 0, 467 }, { 0, 0, 467 }, { 0, 67, 467 },
    { 0, 133, 467 }, { 0, 200, 467 }, { 0, 267, 467 }, { 0, 333, 467 },
    { 0, 400, 467 }, { 0, 467, 467 }, { 0, 467, 400 }, { 0, 467, 333 },
    { 0, 467, 267 }, { 0, 467, 200 }, { 0, 467, 133 }, { 0, 467, 67 },
    { 0, 467, 0 }, { 67, 467, 0 }, { 133, 467, 0 }, { 200, 467, 0 },
    { 267, 467, 0 }, { 333, 467, 0 }, { 400, 467, 0 }, { 467, 467, 0 },
    { 467, 400, 0 }, { 467, 333, 0 }, { 467, 267, 0 }, { 467, 200, 0 },
    { 467, 133, 0 }, { 467, 67, 0 }, { 267, 0, 0 }, { 267, 0, 67 },
    { 267, 0, 133 }, { 267, 0, 200 }, { 267, 0, 267 }, { 200, 0, 267 },
    { 133, 0, 267 }, { 67, 0, 267 }, { 0, 0, 267 }, { 0, 67, 267 },
    { 0, 133, 267 }, { 0, 200, 267 }, { 0, 267, 267 }, { 0, 267, 200 },
    { 0, 267, 133 }, { 0, 267, 67 }, { 0, 267, 0 }, { 67, 267, 0 },
    { 133, 267, 0 }, { 200, 267, 0 }, { 267, 267, 0 }, { 267, 200, 0 },
    { 267, 133, 0 }, { 267, 67, 0 }, { 1000, 1000, 1000 }, { 0, 0, 0 }
};
#endif
#if CONF_WITH_VIDEL
static const WORD videl_palette1[16][3] =
{
    { 1000, 1000, 1000 }, { 0, 0, 0 }, { 1000, 0, 0 }, { 0, 1000, 0 },
    { 0, 0, 1000 }, { 0, 1000, 1000 }, { 1000, 1000, 0 }, { 1000, 0, 1000 },
    { 733, 733, 733 }, { 533, 533, 533 }, { 667, 0, 0 }, { 0, 667, 0 },
    { 0, 0, 667 }, { 0, 667, 667 }, { 667, 667, 0 }, { 667, 0, 667 }
};
static const WORD videl_palette2[240][3] =
{
    { 1000, 1000, 1000 }, { 933, 933, 933 }, { 867, 867, 867 }, { 800, 800, 800 },
    { 733, 733, 733 }, { 667, 667, 667 }, { 600, 600, 600 }, { 533, 533, 533 },
    { 467, 467, 467 }, { 400, 400, 400 }, { 333, 333, 333 }, { 267, 267, 267 },
    { 200, 200, 200 }, { 133, 133, 133 }, { 67, 67, 67 }, { 0, 0, 0 },
    { 1000, 0, 0 }, { 1000, 0, 67 }, { 1000, 0, 133 }, { 1000, 0, 200 },
    { 1000, 0, 267 }, { 1000, 0, 333 }, { 1000, 0, 400 }, { 1000, 0, 467 },
    { 1000, 0, 533 }, { 1000, 0, 600 }, { 1000, 0, 667 }, { 1000, 0, 733 },
    { 1000, 0, 800 }, { 1000, 0, 867 }, { 1000, 0, 933 }, { 1000, 0, 1000 },
    { 933, 0, 1000 }, { 867, 0, 1000 }, { 800, 0, 1000 }, { 733, 0, 1000 },
    { 667, 0, 1000 }, { 600, 0, 1000 }, { 533, 0, 1000 }, { 467, 0, 1000 },
    { 400, 0, 1000 }, { 333, 0, 1000 }, { 267, 0, 1000 }, { 200, 0, 1000 },
    { 133, 0, 1000 }, { 67, 0, 1000 }, { 0, 0, 1000 }, { 0, 67, 1000 },
    { 0, 133, 1000 }, { 0, 200, 1000 }, { 0, 267, 1000 }, { 0, 333, 1000 },
    { 0, 400, 1000 }, { 0, 467, 1000 }, { 0, 533, 1000 }, { 0, 600, 1000 },
    { 0, 667, 1000 }, { 0, 733, 1000 }, { 0, 800, 1000 }, { 0, 867, 1000 },
    { 0, 933, 1000 }, { 0, 1000, 1000 }, { 0, 1000, 933 }, { 0, 1000, 867 },
    { 0, 1000, 800 }, { 0, 1000, 733 }, { 0, 1000, 667 }, { 0, 1000, 600 },
    { 0, 1000, 533 }, { 0, 1000, 467 }, { 0, 1000, 400 }, { 0, 1000, 333 },
    { 0, 1000, 267 }, { 0, 1000, 200 }, { 0, 1000, 133 }, { 0, 1000, 67 },
    { 0, 1000, 0 }, { 67, 1000, 0 }, { 133, 1000, 0 }, { 200, 1000, 0 },
    { 267, 1000, 0 }, { 333, 1000, 0 }, { 400, 1000, 0 }, { 467, 1000, 0 },
    { 533, 1000, 0 }, { 600, 1000, 0 }, { 667, 1000, 0 }, { 733, 1000, 0 },
    { 800, 1000, 0 }, { 867, 1000, 0 }, { 933, 1000, 0 }, { 1000, 1000, 0 },
    { 1000, 933, 0 }, { 1000, 867, 0 }, { 1000, 800, 0 }, { 1000, 733, 0 },
    { 1000, 667, 0 }, { 1000, 600, 0 }, { 1000, 533, 0 }, { 1000, 467, 0 },
    { 1000, 400, 0 }, { 1000, 333, 0 }, { 1000, 267, 0 }, { 1000, 200, 0 },
    { 1000, 133, 0 }, { 1000, 67, 0 }, { 733, 0, 0 }, { 733, 0, 67 },
    { 733, 0, 133 }, { 733, 0, 200 }, { 733, 0, 267 }, { 733, 0, 333 },
    { 733, 0, 400 }, { 733, 0, 467 }, { 733, 0, 533 }, { 733, 0, 600 },
    { 733, 0, 667 }, { 733, 0, 733 }, { 667, 0, 733 }, { 600, 0, 733 },
    { 533, 0, 733 }, { 467, 0, 733 }, { 400, 0, 733 }, { 333, 0, 733 },
    { 267, 0, 733 }, { 200, 0, 733 }, { 133, 0, 733 }, { 67, 0, 733 },
    { 0, 0, 733 }, { 0, 67, 733 }, { 0, 133, 733 }, { 0, 200, 733 },
    { 0, 267, 733 }, { 0, 333, 733 }, { 0, 400, 733 }, { 0, 467, 733 },
    { 0, 533, 733 }, { 0, 600, 733 }, { 0, 667, 733 }, { 0, 733, 733 },
    { 0, 733, 667 }, { 0, 733, 600 }, { 0, 733, 533 }, { 0, 733, 467 },
    { 0, 733, 400 }, { 0, 733, 333 }, { 0, 733, 267 }, { 0, 733, 200 },
    { 0, 733, 133 }, { 0, 733, 67 }, { 0, 733, 0 }, { 67, 733, 0 },
    { 133, 733, 0 }, { 200, 733, 0 }, { 267, 733, 0 }, { 333, 733, 0 },
    { 400, 733, 0 }, { 467, 733, 0 }, { 533, 733, 0 }, { 600, 733, 0 },
    { 667, 733, 0 }, { 733, 733, 0 }, { 733, 667, 0 }, { 733, 600, 0 },
    { 733, 533, 0 }, { 733, 467, 0 }, { 733, 400, 0 }, { 733, 333, 0 },
    { 733, 267, 0 }, { 733, 200, 0 }, { 733, 133, 0 }, { 733, 67, 0 },
    { 467, 0, 0 }, { 467, 0, 67 }, { 467, 0, 133 }, { 467, 0, 200 },
    { 467, 0, 267 }, { 467, 0, 333 }, { 467, 0, 400 }, { 467, 0, 467 },
    { 400, 0, 467 }, { 333, 0, 467 }, { 267, 0, 467 }, { 200, 0, 467 },
    { 133, 0, 467 }, { 67, 0, 467 }, { 0, 0, 467 }, { 0, 67, 467 },
    { 0, 133, 467 }, { 0, 200, 467 }, { 0, 267, 467 }, { 0, 333, 467 },
    { 0, 400, 467 }, { 0, 467, 467 }, { 0, 467, 400 }, { 0, 467, 333 },
    { 0, 467, 267 }, { 0, 467, 200 }, { 0, 467, 133 }, { 0, 467, 67 },
    { 0, 467, 0 }, { 67, 467, 0 }, { 133, 467, 0 }, { 200, 467, 0 },
    { 267, 467, 0 }, { 333, 467, 0 }, { 400, 467, 0 }, { 467, 467, 0 },
    { 467, 400, 0 }, { 467, 333, 0 }, { 467, 267, 0 }, { 467, 200, 0 },
    { 467, 133, 0 }, { 467, 67, 0 }, { 267, 0, 0 }, { 267, 0, 67 },
    { 267, 0, 133 }, { 267, 0, 200 }, { 267, 0, 267 }, { 200, 0, 267 },
    { 133, 0, 267 }, { 67, 0, 267 }, { 0, 0, 267 }, { 0, 67, 267 },
    { 0, 133, 267 }, { 0, 200, 267 }, { 0, 267, 267 }, { 0, 267, 200 },
    { 0, 267, 133 }, { 0, 267, 67 }, { 0, 267, 0 }, { 67, 267, 0 },
    { 133, 267, 0 }, { 200, 267, 0 }, { 267, 267, 0 }, { 267, 200, 0 },
    { 267, 133, 0 }, { 267, 67, 0 }, { 1000, 1000, 1000 }, { 0, 0, 0 }
};
#endif


/* Create an ST color value from VDI color */
static int vdi2st(int col)
{
    return col * 7 / 1000;
}


/* Create a VDI color value from ST color
 *
 * we use a lookup table for speed and space savings
 */
#define st2vdi(col) st2vdi_lookup_table[(col)&0x07]
/*
 * this table implements the following calculation, as used by ST TOS:
 *      VDI value = (ST palette hardware value * 1000) / 7
 */
static const WORD st2vdi_lookup_table[8] =
    { 0, 142, 285, 428, 571, 714, 857, 1000 };


#if CONF_WITH_STE_SHIFTER
/* Create an STe color value from VDI color */
static int vdi2ste(int col)
{
    col = col * 3 / 200;
    col = ((col & 1) << 3) | (col >> 1);  // Shift lowest bit to top

    return col;
}


/* Create a VDI color value from STe color
 *
 * we use a lookup table for speed and space savings
 */
#define ste2vdi(col) ste2vdi_lookup_table[(col)&0x0f]
/*
 * this table implements the following calculation, as used by STe TOS:
 *      VDI value = (STe palette hardware value * 1000 + 7) / 15
 *
 * the sequence of numbers in the table reflects the arrangement of bits
 * in the STe palette hardware: 0 3 2 1
 */
static const WORD ste2vdi_lookup_table[16] =
    { 0, 133, 267, 400, 533, 667, 800, 933,     /* values 0,2 ... 14 */
      67, 200, 333, 467, 600, 733, 867, 1000 }; /* values 1,3 ... 15 */
#endif


#if CONF_WITH_TT_SHIFTER
/* Create a TT color value from VDI color */
static int vdi2tt(int col)
{
    return (col * 15 + 500) / 1000;
}


/* Create a VDI color value from TT color
 *
 * we use a lookup table for speed & space savings
 */
#define tt2vdi(col) tt2vdi_lookup_table[(col)&0x0f]
/*
 * this table implements the following calculation, as used by TT TOS:
 *      VDI value = (TT palette hardware value * 1000 + 7) / 15
 */
static const WORD tt2vdi_lookup_table[16] =
    { 0, 67, 133, 200, 267, 333, 400, 467,
      533, 600, 667, 733, 800, 867, 933, 1000 };


/* Return adjusted VDI color number for TT systems
 *
 * This ensures that we access the right save area
 * (REQ_COL or req_col2) if bank switching is in effect
 */
static WORD adjust_tt_colnum(WORD colnum)
{
    UWORD tt_shifter, rez, bank;

    tt_shifter = EgetShift();
    rez = (tt_shifter>>8) & 0x07;
    switch(rez) {
    case ST_LOW:
    case ST_MEDIUM:
    case TT_MEDIUM:
        bank = tt_shifter & 0x000f;
        colnum += bank * 16;
    }

    return colnum;
}


/* Set an entry in the TT hardware palette
 *
 * TT video hardware has several obscure features which complicate
 * the VDI handler; we try to be TOS3-compatible
 *
 * Input is VDI-style: colnum is VDI pen#, rgb[] entries are 0-1000
 */
static void set_tt_color(WORD colnum, WORD *rgb)
{
    WORD r, g, b;
    WORD hwreg, hwvalue;
    UWORD tt_shifter, rez, bank, mask;

    /*
     * first we determine which h/w palette register to update
     */
    hwreg = MAP_COL[colnum];    /* default, can be modified below */

    tt_shifter = EgetShift();
    rez = (tt_shifter>>8) & 0x07;
    bank = tt_shifter & 0x000f;
    mask = DEV_TAB[13] - 1;

    switch(rez) {
    case ST_LOW:
    case ST_MEDIUM:
    case TT_MEDIUM:
        hwreg &= mask;      /* mask out unwanted bits */
        hwreg += bank * 16; /* allow for bank number */
        break;
    case ST_HIGH:   /* also known as duochrome on a TT */
        /*
         * set register 254 or 255 depending on the VDI pen#
         * and the invert bit in palette register 0
         */
        hwvalue = EsetColor(0,-1);
        if (hwvalue & TT_DUOCHROME_INVERT)
            hwreg = 255 - colnum;
        else hwreg = 254 + colnum;
        break;
    case TT_HIGH:
        return;
    }

    /*
     * then we determine what value to put in it
     */
    r = rgb[0];     /* VDI values */
    g = rgb[1];
    b = rgb[2];

    if (tt_shifter & TT_HYPER_MONO)
    {
        /* we do what TOS3 does: first, derive a weighted value 0-1000
         * based on input RGB values; then, scale it to a value 0-255
         * (which the h/w applies to all 3 guns)
         */
        hwvalue = mul_div(30,r,100) + mul_div(59,g,100) + mul_div(11,b,100);
        hwvalue = mul_div(255,hwvalue,1000);
    }
    else
    {
        hwvalue = (vdi2tt(r) << 8) | (vdi2tt(g) << 4) | vdi2tt(b);
    }
    EsetColor(hwreg, hwvalue);
}


/*
 * Return VDI values for hyper mono mode
 *
 * In hyper mono mode, the original VDI values are not even approximately
 * preserved in the hardware palette registers.  So we do what TOS3 does:
 * we fake the return values based on the previously saved values.
 */
static void get_tt_hyper_mono(WORD colnum,WORD *retval)
{
    WORD *save, i, vditemp;

    save = (colnum < 16) ? REQ_COL[colnum] : req_col2[colnum-16];

    for (i = 0; i < 3; i++, save++, retval++)
    {
        /* first clamp the raw values */
        if (*save > 1000)
            vditemp = 1000;
        else if (*save < 0)
            vditemp = 0;
        else vditemp = *save;

        /*
         * convert to h/w value then back to the equivalent VDI value,
         * just as if we had actually written them in a normal mode
         */
        *retval = tt2vdi(vdi2tt(vditemp));
    }
}


/* Query an entry in the TT hardware palette
 *
 * TT video hardware has several obscure features which complicate
 * the VDI handler; we try to be TOS3-compatible
 *
 * Input colnum is VDI pen#, returned values are nominally 0-1000
 */
static void query_tt_color(WORD colnum,WORD *retval)
{
    WORD hwreg, hwvalue;
    UWORD tt_shifter, rez, bank, mask;

    /*
     * first we determine which h/w palette register to read
     */
    hwreg = MAP_COL[colnum];    /* default, can be modified below */

    tt_shifter = EgetShift();
    rez = (tt_shifter>>8) & 0x07;
    bank = tt_shifter & 0x000f;
    mask = DEV_TAB[13] - 1;

    switch(rez) {
    case ST_LOW:
    case ST_MEDIUM:
    case TT_MEDIUM:
        hwreg &= mask;      /* mask out unwanted bits */
        hwreg += bank * 16; /* allow for bank number */
        break;
    case ST_HIGH:   /* also known as duochrome on a TT */
        /*
         * set register 254 or 255 depending on the VDI pen#
         * and the invert bit in palette register 0
         */
        hwvalue = EsetColor(0,-1);
        if (hwvalue & TT_DUOCHROME_INVERT)
            hwreg = 255 - colnum;
        else hwreg = 254 + colnum;
        break;
    case TT_HIGH:
        retval[0] = retval[1] = retval[2] = colnum ? 0 : 1000;
        return;
    }

    if (tt_shifter & TT_HYPER_MONO)
    {
        get_tt_hyper_mono(colnum,retval);
    }
    else
    {
        hwvalue = EsetColor(hwreg,-1);
        retval[0] = tt2vdi(hwvalue >> 8);
        retval[1] = tt2vdi(hwvalue >> 4);
        retval[2] = tt2vdi(hwvalue);
    }
}
#endif


#if CONF_WITH_VIDEL
/* Create videl colour value from VDI colour */
static LONG vdi2videl(WORD col)
{
    return (LONG)col * 51 / 200;            /* scale 1000 -> 255 */
}


/* Create VDI colour value from videl colour */
static WORD videl2vdi(LONG col)
{
    return (WORD)((col & 0xff) * 200 / 51); /* scale 255 -> 1000 */
}
#endif


/*
 * Monochrome screens get special handling because they don't use the
 * regular palette setup; instead, bit 0 of h/w palette register 0
 * controls whether the screen background is white (bit0=0) or black
 * (bit0=1).
 *
 * Also, from a VDI standpoint, you would expect that setting pen 0 to
 * (1000,1000,1000) would get white and setting pen 0 to (0,0,0) would
 * get black; but what should happen with intermediate values?
 *
 * Atari TOS handles this as follows:
 * 1. each RGB value less than a fudge factor F is converted to 0 and
 *    each value greater than or equal to 1000-F is converted to 1000
 * 2. if the sum of the values is neither 0 nor 3000, nothing is done
 * 3. if asked to change pen 1, it sets pen 0 to white, irrespective of
 *    RGB values
 * 4. if changing pen 0, it respects the RGB values
 *
 * We do the same ...
 */
static WORD adjust_mono_values(WORD colnum,WORD *rgb)
{
    WORD i, sum, fudge = ST_MONO_FUDGE_FACTOR;

#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter)
        fudge = STE_MONO_FUDGE_FACTOR;
#endif

    for (i = 0, sum = 0; i < 3; i++)
    {
        if (rgb[i] < fudge)
            rgb[i] = 0;
        else if (rgb[i] >= (1000-fudge))
            rgb[i] = 1000;
        sum += rgb[i];
    }

    if ((sum > 0) && (sum < 3000))
        return -1;      /* 'do nothing' indicator */

    if (colnum == 1)
    {
        colnum = 0;     /* set pen 0 to white */
        rgb[0] = rgb[1] = rgb[2] = 1000;
    }

    return colnum;
}


/* Set an entry in the hardware color palette
 *
 * Input is VDI-style: colnum is VDI pen#, rgb[] entries are 0-1000
 */
static void set_color(WORD colnum, WORD *rgb)
{
    WORD r, g, b;
    WORD hwreg = MAP_COL[colnum];   /* get hardware register */

    r = rgb[0];
    g = rgb[1];
    b = rgb[2];

#if CONF_WITH_VIDEL
    if (has_videl)
    {
        LONG videlrgb;

        videlrgb = (vdi2videl(r) << 16) | (vdi2videl(g) << 8) | vdi2videl(b);
        VsetRGB(hwreg,1,(LONG)&videlrgb);
        return;
    }
#endif

#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
    {
        set_tt_color(colnum,rgb);
        return;
    }
#endif

    if (v_planes == 1)  /* special handling for monochrome screens */
    {
        hwreg = adjust_mono_values(hwreg,rgb);  /* may update rgb[] */
        if (hwreg < 0)                          /* 'do nothing' */
            return;
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
    }

#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter)
    {
        r = vdi2ste(r);
        g = vdi2ste(g);
        b = vdi2ste(b);
    }
    else
#endif
    {
        r = vdi2st(r);
        g = vdi2st(g);
        b = vdi2st(b);
    }

    Setcolor(hwreg, (r << 8) | (g << 4) | b);
}


/*
 * _vs_color - set color index table
 */
void _vs_color(Vwk *vwk)
{
    WORD colnum, i;
    WORD *intin, rgb[3], *rgbptr;

    colnum = INTIN[0];

    /* Check for valid color index */
    if (colnum < 0 || colnum >= DEV_TAB[13])
    {
        /* It was out of range */
        return;
    }

#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        colnum = adjust_tt_colnum(colnum);  /* handles palette bank issues */
#endif

    /*
     * Copy raw values to the "requested colour" arrays, then clamp
     * them to 0-1000 before calling set_color()
     */
    for (i = 0, intin = INTIN+1, rgbptr = rgb; i < 3; i++, intin++, rgbptr++)
    {
        if (colnum < 16)
            REQ_COL[colnum][i] = *intin;
#if EXTENDED_PALETTE
        else
            req_col2[colnum-16][i] = *intin;
#endif
        if (*intin > 1000)
            *rgbptr = 1000;
        else if (*intin < 0)
            *rgbptr = 0;
        else *rgbptr = *intin;
    }

    colnum = INTIN[0];      /* may have been munged on TT system, see above */
    set_color(colnum, rgb);
}


/* Set the default palette etc. */
void init_colors(void)
{
    int i;

    /* set up palette */
    memcpy(REQ_COL, st_palette, sizeof(st_palette));    /* use ST as default */

#if CONF_WITH_VIDEL
    if (has_videl)
    {
        memcpy(REQ_COL, videl_palette1, sizeof(videl_palette1));
        memcpy(req_col2, videl_palette2, sizeof(videl_palette2));
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
    {
        memcpy(REQ_COL, tt_palette1, sizeof(tt_palette1));
        memcpy(req_col2, tt_palette2, sizeof(tt_palette2));
    }
    else
#endif
    {
        /* Nothing */
    }

    /* set up vdi pen -> hardware colour register mapping */
    memcpy(MAP_COL, MAP_COL_ROM, sizeof(MAP_COL_ROM));
    MAP_COL[1] = DEV_TAB[13] - 1;   /* pen 1 varies according to # colours available */

#if EXTENDED_PALETTE
    for (i = 16; i < MAXCOLOURS-1; i++)
        MAP_COL[i] = i;
    MAP_COL[i] = 15;
#endif

    /* set up reverse mapping (hardware colour register -> vdi pen) */
    for (i = 0; i < DEV_TAB[13]; i++)
        REV_MAP_COL[MAP_COL[i]] = i;

    /* now initialise the hardware */
    for (i = 0; i < DEV_TAB[13]; i++)
    {
        if (i < 16)
            set_color(i, REQ_COL[i]);
#if EXTENDED_PALETTE
        else
            set_color(i, req_col2[i-16]);
#endif
    }

#ifdef HATARI_DUOCHROME_WORKAROUND
    /*
     * The following is a workaround for a bug in Hatari v1.9 and earlier.
     * It is disabled by default, and should be removed once the Hatari
     * bug is fixed.
     *
     * In Duochrome mode (ST high on a TT), Hatari uses palette register
     * 0 as the background colour and register 255 as the foreground
     * colour; this is not how real hardware behaves.  However, there
     * was a compensating bug in EmuTOS that set register 0 to white and
     * register 255 to black, so everything displayed OK.
     *
     * On real hardware, register 0 contains the inversion bit (bit 1),
     * and the foreground/background colours are in registers 254/255.
     * For both TOS3 and EmuTOS, the initial value for VDI pens 0/254/255
     * are white/white/black for all resolutions, which causes hardware
     * registers 0/254/255 to be set to 0x0fff/0x0fff/0x000.  Without any
     * compensation, this would cause problems when switching to duochrome
     * mode: since the inversion bit in register 0 is set, the display
     * would show as white on black.
     *
     * Since it's desirable for other reasons to leave register 0 as
     * white, TOS3 (and EmuTOS) compensate as follows: if the inversion
     * bit is set, registers 254/255 are set to 0x0000/0x0fff.  This
     * produces the correct black on white display on real hardware, but
     * a display of white on white under Hatari (both EmuTOS and TOS3).
     *
     * The following workaround preserves the inversion bit, but sets a
     * value of "almost black" in register 0.  The output will be visible
     * on both Hatari and real TT hardware, but Hatari will display black
     * on white, while real hardware displays white on black.  Another
     * consequence of this workaround is that a vq_color() for the actual
     * value of pen 0 will return an unexpected result.
     */
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter && (sshiftmod == ST_HIGH))
        EsetColor(0,0x0002);
#endif
#endif
}


/*
 * _vq_color - query color index table
 */
void _vq_color(Vwk *vwk)
{
    WORD colnum, c, hwreg;

    colnum = INTIN[0];

    INTOUT[1] = INTOUT[2] = INTOUT[3] = 0;  /* Default values */
    CONTRL[4] = 4;

    /* Check for valid color index */
    if (colnum < 0 || colnum >= DEV_TAB[13])
    {
        /* It was out of range */
        INTOUT[0] = -1;
        return;
    }
    INTOUT[0] = colnum;

#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        colnum = adjust_tt_colnum(colnum);  /* handles palette bank issues */
#endif

    if (INTIN[1] == 0)  /* return last-requested value */
    {
        if (colnum < 16)
        {
            INTOUT[1] = REQ_COL[colnum][0];
            INTOUT[2] = REQ_COL[colnum][1];
            INTOUT[3] = REQ_COL[colnum][2];
        }
#if EXTENDED_PALETTE
        else
        {
            INTOUT[1] = req_col2[colnum-16][0];
            INTOUT[2] = req_col2[colnum-16][1];
            INTOUT[3] = req_col2[colnum-16][2];
        }
#endif
        return;
    }

    /*
     * return actual current value
     */
    colnum = INTIN[0];          /* may have been munged on TT system, see above */
    hwreg = MAP_COL[colnum];    /* get hardware register */

#if CONF_WITH_VIDEL
    if (has_videl)
    {
    LONG rgb;

        VgetRGB(hwreg,1,(LONG)&rgb);
        INTOUT[1] = videl2vdi(rgb >> 16);
        INTOUT[2] = videl2vdi(rgb >> 8);
        INTOUT[3] = videl2vdi(rgb);
        return;
    }
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
    {
        query_tt_color(colnum,&INTOUT[1]);
        return;
    }
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter)
    {
        c = Setcolor(hwreg, -1);
        INTOUT[1] = ste2vdi(c >> 8);
        INTOUT[2] = ste2vdi(c >> 4);
        INTOUT[3] = ste2vdi(c);
        return;
    }
#endif
    /* ST shifter */
    c = Setcolor(hwreg, -1);
    INTOUT[1] = st2vdi(c >> 8);
    INTOUT[2] = st2vdi(c >> 4);
    INTOUT[3] = st2vdi(c);
}
