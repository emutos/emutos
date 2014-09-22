/*
 *
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2014 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "intmath.h"
#include "vdi_defs.h"



/* Definitions for sine and cosinus */
#define    HALFPI    900
#define    PI        1800
#define    TWOPI     3600

/* local GDP variables */
static WORD angle, beg_ang, del_ang, end_ang;
static WORD xc, xrad, yc, yrad;

/* Prototypes local to this module */
static void gdp_rbox(Vwk * vwk);
static void gdp_arc(Vwk * vwk);
static int  clc_nsteps(void);
static void gdp_ell(Vwk * vwk);
static void clc_arc(Vwk * vwk, int steps);



/* Sines of angles 1 - 90 degrees normalized between 0-32767. */
static const WORD sin_tbl[92] = {
        0,   572, 1144,   1716,  2286,  2856,  3425,  3993,
     4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
     9032,  9580, 10126, 10668, 11207, 11743, 12275, 12803,
    13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621,
    21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841,
    27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982,
    31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722,
    32747, 32762, 32767, 32767
};



/*
 * ISin - Returns integer sin between -32767 - 32767.
 *
 * Uses integer lookup table sintable^[]. Expects angle in tenths of
 * degree 0 - 3600. Assumes positive angles only.
 */
static WORD Isin(WORD angle)
{
    WORD index, remainder, tmpsin;      /* holder for sin. */
    WORD quadrant;              /* 0-3 = 1st, 2nd, 3rd, 4th.        */

    while (angle > 3600)
        angle -= 3600;
    quadrant = angle / HALFPI;
    switch (quadrant) {
    case 0:
        break;

    case 1:
        angle = PI - angle;
        break;

    case 2:
        angle -= PI;
        break;

    case 3:
        angle = TWOPI - angle;
        break;

    case 4:
        angle -= TWOPI;
        break;
    };
    index = angle / 10;
    remainder = angle % 10;
    tmpsin = sin_tbl[index];
    if (remainder != 0)         /* add interpolation. */
        tmpsin += ((sin_tbl[index + 1] - tmpsin) * remainder) / 10;
    if (quadrant > 1)
        tmpsin = -tmpsin;
    return (tmpsin);
}



/*
 * Icos - Return integer cos between -32767 and 32767.
 */
static WORD Icos(WORD angle)
{
    angle = angle + HALFPI;
    if (angle > TWOPI)
        angle -= TWOPI;
    return (Isin(angle));
}



/*
 * clc_pts - calculates
 */

static void clc_pts(WORD j)
{
    WORD k;
    WORD *pointer;

    pointer = PTSIN;
    k = mul_div(Icos(angle), xrad, 32767) + xc;
    *(pointer + j) = k;
    k = yc - mul_div(Isin(angle), yrad, 32767);        /* FOR RASTER CORDS. */
    *(pointer + j + 1) = k;
}



/*
 * clc_arc - calculates
 */

static void clc_arc(Vwk * vwk, int steps)
{
    WORD i, j, start;
    Point * point;

    if (vwk->clip) {
        if (((xc + xrad) < vwk->xmn_clip) || ((xc - xrad) > vwk->xmx_clip) ||
            ((yc + yrad) < vwk->ymn_clip) || ((yc - yrad) > vwk->ymx_clip))
            return;
    }
    start = angle = beg_ang;
    j = 0;
    clc_pts(j);
    for (i = 1; i < steps; i++) {
        j += 2;
        angle = mul_div(del_ang, i, steps) + start;
        clc_pts(j);
    }
    j += 2;
    angle = end_ang;
    clc_pts(j);

    point = (Point*)PTSIN;

    /* If pie wedge draw to center and then close. */
    if ((CONTRL[5] == 3) || (CONTRL[5] == 7)) {
        /* pie wedge */
        Point * endpoint;

        steps++;
        endpoint = point + steps;
        endpoint->x = xc;
        endpoint->y = yc;
    }
    steps++;                 /* since loop in Clc_arc starts at 0 */

    /* If arc or circle, do nothing because loop should close circle. */
    if ((CONTRL[5] == 2) || (CONTRL[5] == 6)) { /* v_arc() or v_ellarc() */
        /* open arc */
        if (vwk->line_width == 1) {
            set_LN_MASK(vwk);
            polyline(vwk, point, steps, vwk->line_color);
            /* If the ends are arrowed, output them. */
            if ((vwk->line_beg | vwk->line_end) & ARROWED)
                arrow(vwk, point, steps);
        } else
            wideline(vwk, point, steps);
    }
    else
        polygon(vwk, point, steps);
}



/*
 * v_gdp - Major opcode for graphics device primitives
 */

void v_gdp(Vwk * vwk)
{
    WORD i, ltmp_end, rtmp_end;
    WORD *xy_pointer;

    i = CONTRL[5];
    xy_pointer = PTSIN;

    switch (i) {
    case 1:         /* GDP BAR - converted to alpha 2 RJG 12-1-84 */
        dr_recfl(vwk);
        if (vwk->fill_per == TRUE) {
            LN_MASK = 0xffff;

            xy_pointer = PTSIN;
            *(xy_pointer + 5) = *(xy_pointer + 7) = *(xy_pointer + 3);
            *(xy_pointer + 3) = *(xy_pointer + 9) = *(xy_pointer + 1);
            *(xy_pointer + 4) = *(xy_pointer + 2);
            *(xy_pointer + 6) = *(xy_pointer + 8) = *(xy_pointer);

            polyline(vwk, (Point*)PTSIN, 5, vwk->fill_color);
        }
        break;

    case 2:         /* GDP ARC */
    case 3:         /* GDP PIE */
        gdp_arc(vwk);
        break;

    case 4:         /* GDP CIRCLE */
        xc = *xy_pointer;
        yc = *(xy_pointer + 1);
        xrad = *(xy_pointer + 4);
        yrad = mul_div(xrad, xsize, ysize);
        del_ang = 3600;
        beg_ang = 0;
        end_ang = 3600;
        clc_arc(vwk, clc_nsteps(/*vwk*/));
        break;

    case 5:         /* GDP ELLIPSE */
        xc = *xy_pointer;
        yc = *(xy_pointer + 1);
        xrad = *(xy_pointer + 2);
        yrad = *(xy_pointer + 3);
        if (vwk->xfm_mode < 2)
            yrad = yres - yrad;
        del_ang = 3600;
        beg_ang = 0;
        end_ang = 0;
        clc_arc(vwk, clc_nsteps(/*vwk*/));
        break;

    case 6:         /* GDP ELLIPTICAL ARC */
    case 7:         /* GDP ELLIPTICAL PIE */
        gdp_ell(vwk);
        break;

    case 8:         /* GDP Rounded Box */
        ltmp_end = vwk->line_beg;
        vwk->line_beg = SQUARED;
        rtmp_end = vwk->line_end;
        vwk->line_end = SQUARED;
        gdp_rbox(vwk);
        vwk->line_beg = ltmp_end;
        vwk->line_end = rtmp_end;
        break;

    case 9:         /* GDP Rounded Filled Box */
        gdp_rbox(vwk);
        break;

    case 10:         /* GDP Justified Text */
        d_justified(vwk);
        break;
#if HAVE_BEZIER
    case 13:         /* GDP Bezier */
        v_bez_control(vwk);     /* check, if we can do bezier curves */
        break;
#endif
    }
}



/*
 * gdp_rbox - draws an rbox
 */

static void gdp_rbox(Vwk * vwk)
{
    WORD i, j;
    WORD x1,y1,x2,y2;
    WORD rdeltax, rdeltay;
    WORD *pointer;
    Line * line = (Line*)PTSIN;

    arb_line(line);
    x1 = line->x1;
    y1 = line->y1;
    x2 = line->x2;
    y2 = line->y2;

    rdeltax = (x2 - x1) / 2;
    rdeltay = (y1 - y2) / 2;

    xrad = xres >> 6;
    if (xrad > rdeltax)
        xrad = rdeltax;

    yrad = mul_div(xrad, xsize, ysize);
    if (yrad > rdeltay)
        yrad = rdeltay;

    pointer = PTSIN;
    *pointer++ = 0;
    *pointer++ = yrad;
    *pointer++ = mul_div(Icos(675), xrad, 32767);
    *pointer++ = mul_div(Isin(675), yrad, 32767);
    *pointer++ = mul_div(Icos(450), xrad, 32767);
    *pointer++ = mul_div(Isin(450), yrad, 32767);
    *pointer++ = mul_div(Icos(225), xrad, 32767);
    *pointer++ = mul_div(Isin(225), yrad, 32767);
    *pointer++ = xrad;
    *pointer = 0;

    pointer = PTSIN;
    xc = x2 - xrad;
    yc = y1 - yrad;
    j = 10;
    for (i = 9; i >= 0; i--) {
        *(pointer + j + 1) = yc + *(pointer + i--);
        *(pointer + j) = xc + *(pointer + i);
        j += 2;
    }
    xc = x1 + xrad;
    j = 20;
    for (i = 0; i < 10; i++) {
        *(pointer + j++) = xc - *(pointer + i++);
        *(pointer + j++) = yc + *(pointer + i);
    }
    yc = y2 + yrad;
    j = 30;
    for (i = 9; i >= 0; i--) {
        *(pointer + j + 1) = yc - *(pointer + i--);
        *(pointer + j) = xc - *(pointer + i);
        j += 2;
    }
    xc = x2 - xrad;
    j = 0;
    for (i = 0; i < 10; i++) {
        *(pointer + j++) = xc + *(pointer + i++);
        *(pointer + j++) = yc - *(pointer + i);
    }
    *(pointer + 40) = *pointer;
    *(pointer + 41) = *(pointer + 1);

    if (CONTRL[5] == 8) {       /* v_rbox() */
        set_LN_MASK(vwk);

        if (vwk->line_width == 1) {
            polyline(vwk, (Point*)PTSIN, 21, vwk->line_color);
        } else
            wideline(vwk, (Point*)PTSIN, 21);
    } else {
        polygon(vwk, (Point*)PTSIN, 21);
    }

    return;
}



/*
 * gdp_arc - draw an arc
 */

static void gdp_arc(Vwk * vwk)
{
    WORD *pointer;
    int steps;

    pointer = INTIN;

    beg_ang = *pointer++;
    end_ang = *pointer;
    del_ang = end_ang - beg_ang;
    if (del_ang < 0)
        del_ang += 3600;

    pointer = PTSIN;
    xrad = *(pointer + 6);
    yrad = mul_div(xrad, xsize, ysize);
    steps = clc_nsteps(/*vwk*/);
    steps = mul_div(del_ang, steps, 3600);
    if (steps == 0)
        steps = 1;      /* always draw something! */
    xc = *pointer++;
    yc = *pointer;
    clc_arc(vwk, steps);
    return;
}



/*
 * clc_nsteps - calculates
 */

static int clc_nsteps(void)
{
    int steps;

    if (xrad > yrad)
        steps = xrad;
    else
        steps = yrad;
    steps = steps >> 2;
    if (steps < 16)
        steps = 16;
    else {
        if (steps > MAX_ARC_CT)
            steps = MAX_ARC_CT;
    }
    return steps;
}



/*
 * gdp_ell - draws an ell
 */

static void gdp_ell(Vwk * vwk)
{
    WORD *pointer;
    int steps;

    pointer = INTIN;
    beg_ang = *pointer++;
    end_ang = *pointer;
    del_ang = end_ang - beg_ang;
    if (del_ang < 0)
        del_ang += 3600;

    pointer = PTSIN;
    xc = *pointer++;
    yc = *pointer++;
    xrad = *pointer++;
    yrad = *pointer;
    if (vwk->xfm_mode < 2)
        yrad = yres - yrad;
    steps = clc_nsteps(/*vwk*/);
    steps = mul_div(del_ang, steps, 3600);
    if (steps == 0)
        steps = 1;      /* always draw something! */
    clc_arc(vwk, steps);
    return;
}
