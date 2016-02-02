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
#include "vdi_defs.h"
#include "string.h"
#include "machine.h"
#include "xbiosbind.h"
#include "vdi_col.h"
#include "lineavars.h"

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


/* Create a VDI color value from ST color */
static int st2vdi(int col)
{
    return (col & 0x7) * 1000 / 7;
}


#if CONF_WITH_STE_SHIFTER
/* Create an STe color value from VDI color */
static int vdi2ste(int col)
{
    col = col * 3 / 200;
    col = ((col & 1) << 3) | (col >> 1);  // Shift lowest bit to top

    return col;
}


/* Create a VDI color value from STe color */
static int ste2vdi(int col)
{
    col = ((col & 0x7) << 1) | ((col >> 3) & 0x1);
    col = col * 200 / 3;

    return col;
}
#endif


#if CONF_WITH_TT_SHIFTER
/* Create a TT color value from VDI color */
static int vdi2tt(int col)
{
    return col * 3 / 200;
}


/* Create a VDI color value from TT color */
static int tt2vdi(int col)
{
    return (col & 0x0f) * 200 / 3;
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


/* Set an entry in the hardware color palette */
static void set_color(int colnum, int r, int g, int b)
{
    colnum = MAP_COL[colnum];   /* get hardware register */

#if CONF_WITH_VIDEL
    if (has_videl)
    {
        LONG rgb;

        rgb = (vdi2videl(r) << 16) | (vdi2videl(g) << 8) | vdi2videl(b);
        VsetRGB(colnum,1,(LONG)&rgb);
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
    {
        r = vdi2tt(r);
        g = vdi2tt(g);
        b = vdi2tt(b);        
        EsetColor(colnum, (r << 8) | (g << 4) | b);
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter)
    {
        r = vdi2ste(r);
        g = vdi2ste(g);
        b = vdi2ste(b);
        Setcolor(colnum, (r << 8) | (g << 4) | b);
    }
    else
#endif
    {
        r = vdi2st(r);
        g = vdi2st(g);
        b = vdi2st(b);
        Setcolor(colnum, (r << 8) | (g << 4) | b);
    }
}


/*
 * _vs_color - set color index table
 */
void _vs_color(Vwk *vwk)
{
    int colnum, i;

    colnum = INTIN[0];

    /* Check for valid color index */
    if (colnum < 0 || colnum >= DEV_TAB[13])
    {
        /* It was out of range */
        return;
    }

    /* Check if color values are in range and copy them to REQ_COL array */
    for (i = 1; i <= 3; i++)
    {
        if (INTIN[i] > 1000)
            INTIN[i] = 1000;
        else if (INTIN[i] < 0)
            INTIN[i] = 0;

        if (colnum < 16)
            REQ_COL[colnum][i-1] = INTIN[i];
#if EXTENDED_PALETTE
        else
            req_col2[colnum-16][i-1] = INTIN[i];
#endif
    }

    set_color(colnum, INTIN[1], INTIN[2], INTIN[3]);
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
            set_color(i, REQ_COL[i][0], REQ_COL[i][1], REQ_COL[i][2]);
#if EXTENDED_PALETTE
        else
            set_color(i, req_col2[i-16][0], req_col2[i-16][1], req_col2[i-16][2]);
#endif
    }
}


/*
 * _vq_color - query color index table
 */
void _vq_color(Vwk *vwk)
{
    int colnum, c;

    colnum = INTIN[0];

    INTOUT[0] = INTOUT[1] = INTOUT[2] = INTOUT[3] = 0;  // Default values
    CONTRL[4] = 4;

    /* Check for valid color index */
    if (colnum < 0 || colnum >= DEV_TAB[13])
    {
        /* It was out of range */
        INTOUT[0] = -1;
        return;
    }

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
    colnum = MAP_COL[colnum];   /* get hardware register */

#if CONF_WITH_VIDEL
    if (has_videl)
    {
    LONG rgb;

        VgetRGB(colnum,1,(LONG)&rgb);
        INTOUT[1] = videl2vdi(rgb >> 16);
        INTOUT[2] = videl2vdi(rgb >> 8);
        INTOUT[3] = videl2vdi(rgb);
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
    {
        c = EsetColor(colnum, -1);
        INTOUT[1] = tt2vdi(c >> 8);
        INTOUT[2] = tt2vdi(c >> 4);
        INTOUT[3] = tt2vdi(c);
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter)
    {
        c = Setcolor(colnum, -1);
        INTOUT[1] = ste2vdi(c >> 8);
        INTOUT[2] = ste2vdi(c >> 4);
        INTOUT[3] = ste2vdi(c);
    }
    else
#endif
    /* ST shifter */
    {
        c = Setcolor(colnum, -1);
        INTOUT[1] = st2vdi(c >> 8);
        INTOUT[2] = st2vdi(c >> 4);
        INTOUT[3] = st2vdi(c);
    }
}
