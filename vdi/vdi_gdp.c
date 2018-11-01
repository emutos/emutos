/*
 * vdi_gdp.c - implement GDP functions
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2017 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "intmath.h"
#include "vdi_defs.h"


/*
 * the number of points used to describe the corners of a rounded
 * rectangle.  this should be 5 for TOS visual compatibility.
 */
#define CORNER_POINTS   5
#define RBOX_POINTS     (4*CORNER_POINTS+1)

/* Definitions for sine and cosine */
#define    HALFPI    900
#define    PI        1800
#define    TWOPI     3600

/*
 * local GDP variables
 *
 * these are used to pass values to clc_arc(), clc_nsteps(), and clc_pts()
 */
static WORD beg_ang, del_ang, end_ang;
static WORD xc, xrad, yc, yrad;


/* Sines of angles 0 - 90 degrees normalized between 0-32767. */
static const WORD sin_tbl[92] = {
        0,   572,  1144,  1715,  2286,  2856,  3425,  3993,
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


/* precalculated sine/cosine values used in gdp_rbox() */
#define Isin225     12539
#define Isin450     23170
#define Isin675     30273
#define Icos225     Isin675
#define Icos450     Isin450
#define Icos675     Isin225


/*
 * Isin - Returns integer sine between 0 and 32767
 *
 * Uses integer lookup table sin_tbl[]. Expects angle in tenths of
 * degree 0 - 32767; angles >3599 will be normalised to 0-3599.
 * Assumes positive angles only.
 */
static WORD Isin(WORD angle)
{
    WORD index, remainder, tmpsin;      /* holder for sin. */
    WORD quadrant;              /* 0-3 = 1st, 2nd, 3rd, 4th.        */

    while (angle >= TWOPI)      /* normalise angle to 0-3599 inclusive */
        angle -= TWOPI;
    quadrant = angle / HALFPI;
    switch(quadrant) {          /* quadrant MUST be 0-3 */
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
    }
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
 * Icos - Return integer cosine between 0 and 32767
 *
 * Assumes positive angles only.
 */
static WORD Icos(WORD angle)
{
    if (angle >= TWOPI)         /* partial normalisation to prevent */
        angle -= TWOPI;         /*  possible arithmetic overflow    */
    angle = angle + HALFPI;

    return Isin(angle);         /* Isin() will fully normalise */
}



/*
 * clc_pts - calculates a segment endpoint position, based on the
 *           point number and xc/yc/xrad/yrad
 */
static void clc_pts(WORD j, WORD angle)
{
    WORD k;
    WORD *pointer;

    pointer = PTSIN + j;
    k = mul_div(Icos(angle), xrad, 32767) + xc;
    *pointer++ = k;
    k = yc - mul_div(Isin(angle), yrad, 32767);        /* FOR RASTER CORDS. */
    *pointer = k;
}



/*
 * clc_arc - calculates the positions of all the points necessary to draw
 *           a circular/elliptical arc (or a circle/ellipse), and draws it
 */
static void clc_arc(Vwk * vwk, int steps)
{
    WORD i, j, start, angle;
    Point * point;

    if (vwk->clip) {
        if (((xc + xrad) < vwk->xmn_clip) || ((xc - xrad) > vwk->xmx_clip) ||
            ((yc + yrad) < vwk->ymn_clip) || ((yc - yrad) > vwk->ymx_clip))
            return;
    }
    start = beg_ang;
    j = 0;
    clc_pts(j, beg_ang);
    for (i = 1; i < steps; i++) {
        j += 2;
        angle = mul_div(del_ang, i, steps) + start;
        clc_pts(j, angle);
    }
    j += 2;
    clc_pts(j, end_ang);

    point = (Point*)PTSIN;

    /*
     * If pie wedge draw to center and then close
     */
    if ((CONTRL[5] == 3) || (CONTRL[5] == 7)) { /* v_pieslice()/v_ellpie() */
        Point * endpoint;

        steps++;
        endpoint = point + steps;
        endpoint->x = xc;
        endpoint->y = yc;
    }
    steps++;                 /* since loop in Clc_arc starts at 0 */

    /*
     * If arc or circle, do nothing because loop should close circle
     */
    if ((CONTRL[5] == 2) || (CONTRL[5] == 6)) { /* v_arc() or v_ellarc() */
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
 * gdp_rbox - implements v_rbox(), v_rfbox()
 */
static void gdp_rbox(Vwk *vwk)
{
    WORD i;
    WORD x1, y1, x2, y2;
    WORD xoff[CORNER_POINTS], yoff[CORNER_POINTS];
    WORD *p, *xp, *yp;
    Line * line = (Line*)PTSIN;

    /*
     * set (x1,y1) to LL corner of box, (x2,y2) to UR corner of box
     */
    arb_line(line);
    x1 = line->x1;
    y1 = line->y1;
    x2 = line->x2;
    y2 = line->y2;

    /*
     * calculate x & y radii:
     * . the x radius is nominally 1/64th of the screen width
     * . because the corners are nominally quadrants of a circle, we
     *   scale the y radius according to pixel dimensions
     * . we clamp both radii to a maximum of half the length of the
     *   corresponding box side
     */
    xrad = min(xres>>6,(x2-x1)/2);
    yrad = min(mul_div(xrad,xsize,ysize),(y1-y2)/2);

    /*
     * for each corner we generate 5 points.  the following calculates
     * the unsigned offset of those points from the centre of the
     * 'circle', one quarter of which is drawn at each box corner.
     */
    xoff[0] = 0;
    xoff[1] = mul_div(Icos675, xrad, 32767);
    xoff[2] = mul_div(Icos450, xrad, 32767);
    xoff[3] = mul_div(Icos225, xrad, 32767);
    xoff[4] = xrad;
    yoff[0] = yrad;
    yoff[1] = mul_div(Isin675, yrad, 32767);
    yoff[2] = mul_div(Isin450, yrad, 32767);
    yoff[3] = mul_div(Isin225, yrad, 32767);
    yoff[4] = 0;

    /*
     * now we fill in PTSIN, starting with the UR corner of the box
     *
     * we first calculate the centre of the circle used for the quadrant
     * and then add in the offset (appropriately signed)
     */
    p = PTSIN;

    xc = x2 - xrad;
    yc = y2 + yrad;
    xp = xoff;
    yp = yoff;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xc + *xp++;
        *p++ = yc - *yp++;
    }

    /*
     * handle LR corner: note that the offset sequence is reversed
     *
     * xc, xp and yp are already set correctly
     */
    yc = y1 - yrad;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xc + *--xp;
        *p++ = yc + *--yp;
    }

    /*
     * handle LL corner
     *
     * yc, xp and yp are already set correctly
     */
    xc = x1 + xrad;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xc - *xp++;
        *p++ = yc + *yp++;
    }

    /*
     * handle UL corner: the offset sequence is reversed here too
     *
     * xc, xp and yp are already set correctly
     */
    yc = y2 + yrad;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xc - *--xp;
        *p++ = yc - *--yp;
    }

    /*
     * join up the box
     */
    *p++ = PTSIN[0];
    *p = PTSIN[1];

    if (CONTRL[5] == 8) {       /* v_rbox() */
        set_LN_MASK(vwk);

        if (vwk->line_width == 1) {
            polyline(vwk, (Point*)PTSIN, RBOX_POINTS, vwk->line_color);
        } else
            wideline(vwk, (Point*)PTSIN, RBOX_POINTS);
    } else {                    /* v_rfbox() */
        polygon(vwk, (Point*)PTSIN, RBOX_POINTS);
    }
}



/*
 * clc_nsteps - calculates the number of line segments ('steps') to draw
 *              for a circle/ellipse, based on the larger of xrad/yrad,
 *              and clamped to a range of MIN_ARC_CT -> MAX_ARC_CT
 */
static int clc_nsteps(void)
{
    int steps;

    if (xrad > yrad)
        steps = xrad;
    else
        steps = yrad;
    steps = steps >> 2;

    if (steps < MIN_ARC_CT)
        steps = MIN_ARC_CT;
    else if (steps > MAX_ARC_CT)
        steps = MAX_ARC_CT;

    return steps;
}



/*
 * gdp_curve: handles all circle/ellipse GDP functions:
 *  v_arc(), v_pieslice(), v_circle(), v_ellipse(), v_ellarc(), v_ellpie()
 */
static void gdp_curve(Vwk *vwk)
{
    WORD steps;

    xc = PTSIN[0];
    yc = PTSIN[1];

    if (CONTRL[5] <= 4) {   /* v_arc(), v_pieslice(), v_circle() */
        xrad = (CONTRL[5] == 4) ? PTSIN[4] : PTSIN[6];
        yrad = mul_div(xrad, xsize, ysize);
    } else {                /* v_ellipse(), v_ellarc(), v_ellpie() */
        xrad = PTSIN[2];
        yrad = PTSIN[3];
        if (vwk->xfm_mode < 2)  /* NDC coordinates ... not tested AFAIK */
            yrad = yres - yrad;
    }

    if ((CONTRL[5] == 4) || (CONTRL[5] == 5)) { /* v_circle(), v_ellipse() */
        beg_ang = 0;
        end_ang = TWOPI;
    } else {
        beg_ang = INTIN[0];
        end_ang = INTIN[1];
    }

    del_ang = end_ang - beg_ang;
    if (del_ang < 0)
        del_ang += TWOPI;

    steps = clc_nsteps();

    clc_arc(vwk, steps);    
}



/*
 * vdi_v_gdp - Major opcode for graphics device primitives
 */
void vdi_v_gdp(Vwk * vwk)
{
    WORD save_beg, save_end;
    WORD *xy;

    xy = PTSIN;

    switch(CONTRL[5]) {
    case 1:         /* GDP BAR - converted to alpha 2 RJG 12-1-84 */
        vdi_vr_recfl(vwk);
        if (vwk->fill_per == TRUE) {
            LN_MASK = 0xffff;

            xy[5] = xy[7] = xy[3];
            xy[3] = xy[9] = xy[1];
            xy[4] = xy[2];
            xy[6] = xy[8] = xy[0];

            polyline(vwk, (Point*)PTSIN, 5, vwk->fill_color);
        }
        break;

    case 2:         /* GDP Arc */
    case 3:         /* GDP Pieslice */
    case 4:         /* GDP Circle */
    case 5:         /* GDP Ellipse */
    case 6:         /* GDP Elliptical Arc */
    case 7:         /* GDP Elliptical Pieslice */
        gdp_curve(vwk);
        break;

    case 8:         /* GDP Rounded Box */
        save_beg = vwk->line_beg;
        save_end = vwk->line_end;
        vwk->line_beg = SQUARED;
        vwk->line_end = SQUARED;
        gdp_rbox(vwk);
        vwk->line_beg = save_beg;
        vwk->line_end = save_end;
        break;

    case 9:         /* GDP Rounded Filled Box */
        gdp_rbox(vwk);
        break;

    case 10:         /* GDP Justified Text */
        gdp_justified(vwk);
        break;
#if HAVE_BEZIER
    case 13:         /* GDP Bezier */
        v_bez_control(vwk);     /* check, if we can do bezier curves */
        break;
#endif
    }
}
