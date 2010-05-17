/*
 * vdi_col.c - VDI color palette functions and tables.
 *
 * Copyright 2005, 2007 by The EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "portab.h"
#include "vdi_defs.h"
#include "string.h"
#include "machine.h"
#include "xbiosbind.h"
#include "vdi_col.h"
#include "lineavars.h"


/* Some color mapping tables */
WORD MAP_COL[MAX_COLOR];

WORD MAP_COL_4BITS[] =
    { 0, 15, 1, 2, 4, 6, 3, 5, 7, 8, 9, 10, 12, 14, 11, 13 };
WORD MAP_COL_2BITS[] =
    { 0, 3, 1, 2 };

WORD REV_MAP_COL[MAX_COLOR];

WORD REV_MAP_COL_4BITS[] =
    { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };
WORD REV_MAP_COL_2BITS[] =
    { 0, 2, 3, 1 };


/* req_col2 contains the VDI color palette entries 16 - 255 for vq_color().
 * To stay compatible with the line-a variables, only entries > 16 are
 * stored in this array, the first 16 entries are stored in REQ_COL */
WORD req_col2[240][3];


/* Initial color palette */
static const WORD initial_palette[16][3] =
{
    { 1000, 1000, 1000 },
    { 0, 0, 0 },
    { 1000, 0, 0 },
    { 0, 1000, 0 },
    { 0, 0, 1000 },
    { 0, 1000, 1000 },
    { 1000, 1000, 0 },
    { 1000, 0, 1000 },
    { 733, 733, 733 },
    { 533, 533, 533 },
    { 667, 0, 0 },
    { 0, 667, 0 },
    { 0, 0, 667 },
    { 0, 667, 667 },
    { 667, 667, 0 },
    { 667, 0, 667 }
};


/* Create a ST/STE color value from VDI color */
static int vdi2ste(int col)
{
    col = col * 3 / 200;
    col = ((col & 1) << 3) | (col >> 1);  // Shift lowest bit to top

    return col;
}


/* Set an entry in the hardware color palette */
static void set_color(int colnum, int r, int g, int b)
{
    if (has_videl)
    {
        /* TODO: not implemented */
    }
    else if (has_tt_shifter)
    {
        /* TODO: not implemented */
    }
    else
    {
        /* ST and STE shifter: */
        colnum = MAP_COL[colnum];
        r = vdi2ste(r);
        g = vdi2ste(g);
        b = vdi2ste(b);
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
        else
            req_col2[colnum-16][i-1] = INTIN[i];
    }

    set_color(colnum, INTIN[1], INTIN[2], INTIN[3]);
}


/* Set the default palette etc. */
void init_colors(void)
{
    int i;

    memcpy(REQ_COL, initial_palette, sizeof(initial_palette));

    if (v_planes == 4)
    {
        memcpy(MAP_COL, MAP_COL_4BITS, sizeof(MAP_COL_4BITS));
        memcpy(REV_MAP_COL, REV_MAP_COL_4BITS, sizeof(REV_MAP_COL_4BITS));
    }
    else if (v_planes == 2)
    {
        memcpy(MAP_COL, MAP_COL_2BITS, sizeof(MAP_COL_2BITS));
        memcpy(REV_MAP_COL, REV_MAP_COL_2BITS, sizeof(REV_MAP_COL_2BITS));
    }
    else
    {
        MAP_COL[1] = 1;  // monochrome mapping
    }

    for (i = 0; i < DEV_TAB[13]; i++)
    {
        set_color(i, initial_palette[i][0], initial_palette[i][1],
                     initial_palette[i][2]);
    }

    /* TODO: Also initialize the colors 16 - 255 ? */
}


/* Create a VDI color value from STE color */
static int ste2vdi(int col)
{
    col = ((col & 0x7) << 1) | ((col >> 3) & 0x1);
    col = col * 200 / 3;
    return col;
}


/* Create a VDI color value from ST color */
static int st2vdi(int col)
{
    return (col & 0x7) * 1000 / 7;
}


/*
 * _vq_color - query color index table
 */
void _vq_color(Vwk *vwk)
{
    int colnum, c;

    colnum = INTIN[0];

    INTOUT[1] = INTOUT[2] = INTOUT[3] = 0;      // Default values

    /* Check for valid color index */
    if (colnum < 0 || colnum >= DEV_TAB[13])
    {
        /* It was out of range */
        INTOUT[0] = -1;
        return;
    }

    if (INTIN[1] == 0)
    {
        if (colnum < 16)
        {
            INTOUT[1] = REQ_COL[colnum][0];
            INTOUT[2] = REQ_COL[colnum][1];
            INTOUT[3] = REQ_COL[colnum][2];
        }
        else
        {
            INTOUT[1] = req_col2[colnum-16][0];
            INTOUT[2] = req_col2[colnum-16][1];
            INTOUT[3] = req_col2[colnum-16][2];
        }
    }
    else if (has_videl)
    {
        /* TODO: not implemented */
    }
    else if (has_tt_shifter)
    {
        /* TODO: not implemented */
    }
    else if (has_ste_shifter)
    {
        colnum = MAP_COL[colnum];
        c = Setcolor(colnum, -1);
        INTOUT[1] = ste2vdi(c >> 8);
        INTOUT[2] = ste2vdi(c >> 4);
        INTOUT[3] = ste2vdi(c);
    }
    else  /* ST shifter */
    {
        colnum = MAP_COL[colnum];
        c = Setcolor(colnum, -1);
        INTOUT[1] = st2vdi(c >> 8);
        INTOUT[2] = st2vdi(c >> 4);
        INTOUT[3] = st2vdi(c);
    }

    INTOUT[0] = 0;
}

