/*
 * vdi_gdp.c - implement GDP functions
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2020 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "intmath.h"
#include "aesext.h"
#include "vdi_defs.h"
#include "lineavars.h"


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
 * the maximum value (in tenths of a degree) that can be looked up
 * in sin_tbl[] below
 */
#define MAX_TABLE_ANGLE 896
#define SINE_TABLE_SIZE ((MAX_TABLE_ANGLE/8)+1)

/*
 * local GDP variables
 *
 * these are used to pass values to clc_arc(), clc_nsteps(), and clc_pts()
 */
static WORD beg_ang, del_ang, end_ang;
static WORD xc, xrad, yc, yrad;


/*
 * Sines of angles 0-90 degrees in 0.8 degree steps, normalized to 0-65536
 *
 * NOTE: because of the construction of this table, there is no entry for
 * exactly 90 degrees (the final entry is for 89.6 degrees).  We take
 * advantage of this to scale to a maximum value which we cannot actually
 * return.  We special-case the maximum value in clc_pts() which is the
 * only place where we use this table.
 */
static const UWORD sin_tbl[SINE_TABLE_SIZE] = {
     0,   915,  1830,  2744,  3658,  4572,  5484,  6395,
  7305,  8214,  9121, 10026, 10929, 11831, 12729, 13626,
 14519, 15410, 16298, 17183, 18064, 18942, 19816, 20686,
 21553, 22415, 23272, 24125, 24974, 25817, 26656, 27489,
 28317, 29140, 29956, 30767, 31572, 32371, 33163, 33949,
 34729, 35501, 36267, 37026, 37777, 38521, 39258, 39986,
 40708, 41421, 42126, 42823, 43511, 44191, 44862, 45525,
 46179, 46824, 47459, 48086, 48703, 49310, 49908, 50496,
 51075, 51643, 52201, 52750, 53287, 53815, 54332, 54838,
 55334, 55819, 56293, 56756, 57208, 57649, 58078, 58497,
 58903, 59299, 59683, 60055, 60415, 60764, 61101, 61426,
 61739, 62040, 62328, 62605, 62870, 63122, 63362, 63589,
 63804, 64007, 64197, 64375, 64540, 64693, 64833, 64960,
 65075, 65177, 65266, 65343, 65407, 65458, 65496, 65522,
 65534
};


/* precalculated sine/cosine values used in gdp_rbox()
 * NOTE: these are scaled to a max value of 32767!
 */
#define Isin225     12539
#define Isin450     23170
#define Isin675     30273
#define Icos225     Isin675
#define Icos450     Isin450
#define Icos675     Isin225


/*
 * Isin - Returns integer sine between 0 and 32767
 *
 * Input is the angle in tenths of a degree, 0 to 900
 */
static UWORD Isin(WORD angle)
{
    UWORD index, remainder, tmpsin;

    index = angle >> 3;
    remainder = angle & 7;
    tmpsin = sin_tbl[index];

    if (remainder != 0)         /* add interpolation. */
        tmpsin += ((sin_tbl[index + 1] - tmpsin) * remainder) >> 3;

    return tmpsin;
}



/*
 * Icos - Return integer cosine between 0 and 32767
 *
 * Input is the angle in tenths of a degree, 0 to 900
 */
static UWORD Icos(UWORD angle)
{
    return Isin(HALFPI-angle);
}



/*
 * clc_pts - calculates and saves an endpoint position (in raster
 *           coordinates), based on the input angle and xc/yc/xrad/yrad
 */
#define X_NEGATIVE 0x02     /* values in 'negative' flag below */
#define Y_NEGATIVE 0x01
static void clc_pts(Point *point, WORD angle)
{
    WORD xdiff, ydiff;
    WORD negative = Y_NEGATIVE;     /* default for first quadrant */

    while (angle >= TWOPI)          /* normalise angle to 0-3599 inclusive */
        angle -= TWOPI;

    if (angle > 3*HALFPI) {         /* fourth quadrant */
        angle = TWOPI - angle;
        negative = 0;
    } else if (angle > PI) {        /* third quadrant */
        angle -= PI;
        negative = X_NEGATIVE;
    } else if (angle > HALFPI) {    /* second quadrant */
        angle = PI - angle;
        negative = X_NEGATIVE|Y_NEGATIVE;
    }

    /* handle the values not handled by the table */
    if (angle > MAX_TABLE_ANGLE)
    {
        xdiff = 0;
        ydiff = yrad;
    }
    else if (angle < HALFPI-MAX_TABLE_ANGLE)
    {
        xdiff = xrad;
        ydiff = 0;
    }
    else
    {
        xdiff = umul_shift(Icos(angle), xrad);
        ydiff = umul_shift(Isin(angle), yrad);
    }

    if (negative & X_NEGATIVE)
        xdiff = -xdiff;

    if (negative & Y_NEGATIVE)
        ydiff = -ydiff;

    point->x = xc + xdiff;
    point->y = yc + ydiff;
}



/*
 * clc_arc - calculates the positions of all the points necessary to draw
 *           a circular/elliptical arc (or a circle/ellipse), and draws it
 */
static void clc_arc(Vwk * vwk, int steps)
{
    WORD i, start, angle;
    Point * point;

    point = (Point *)PTSIN;
    start = beg_ang;
    clc_pts(point++, start);
    for (i = 1; i < steps; i++) {
        angle = mul_div(del_ang, i, steps) + start;
        clc_pts(point, angle);
        if (*(LONG *)point != *(LONG *)(point-1))   /* ignore duplicates */
            point++;
    }
    clc_pts(point++, end_ang);
    steps = point - (Point *)PTSIN; /* number of points, not number of steps */

    /*
     * If pie wedge draw to center and then close
     */
    if ((CONTRL[5] == 3) || (CONTRL[5] == 7)) { /* v_pieslice()/v_ellpie() */
        point->x = xc;
        point->y = yc;
        steps++;
    }

    point = (Point *)PTSIN;
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
    WORD i, xcentre, ycentre, xradius, yradius;
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
    xradius = min(xres>>6,(x2-x1)/2);
    yradius = min(mul_div(xradius,xsize,ysize),(y1-y2)/2);

    /*
     * for each corner we generate 5 points.  the following calculates
     * the unsigned offset of those points from the centre of the
     * 'circle', one quarter of which is drawn at each box corner.
     */
    xoff[0] = 0;
    xoff[1] = mul_div(Icos675, xradius, 32767);
    xoff[2] = mul_div(Icos450, xradius, 32767);
    xoff[3] = mul_div(Icos225, xradius, 32767);
    xoff[4] = xradius;
    yoff[0] = yradius;
    yoff[1] = mul_div(Isin675, yradius, 32767);
    yoff[2] = mul_div(Isin450, yradius, 32767);
    yoff[3] = mul_div(Isin225, yradius, 32767);
    yoff[4] = 0;

    /*
     * now we fill in PTSIN, starting with the UR corner of the box
     *
     * we first calculate the centre of the circle used for the quadrant
     * and then add in the offset (appropriately signed)
     */
    p = PTSIN;

    xcentre = x2 - xradius;
    ycentre = y2 + yradius;
    xp = xoff;
    yp = yoff;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xcentre + *xp++;
        *p++ = ycentre - *yp++;
    }

    /*
     * handle LR corner: note that the offset sequence is reversed
     *
     * xcentre, xp and yp are already set correctly
     */
    ycentre = y1 - yradius;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xcentre + *--xp;
        *p++ = ycentre + *--yp;
    }

    /*
     * handle LL corner
     *
     * ycentre, xp and yp are already set correctly
     */
    xcentre = x1 + xradius;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xcentre - *xp++;
        *p++ = ycentre + *yp++;
    }

    /*
     * handle UL corner: the offset sequence is reversed here too
     *
     * xcentre, xp and yp are already set correctly
     */
    ycentre = y2 + yradius;
    for (i = 0; i < CORNER_POINTS; i++) {
        *p++ = xcentre - *--xp;
        *p++ = ycentre - *--yp;
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
    steps >>= 2;

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
    }

    /*
     * Atari TOS handles negative radii more-or-less the same as
     * positive ones; we explicitly treat them the same.
     */
    if (xrad < 0)
        xrad = -xrad;
    if (yrad < 0)
        yrad = -yrad;

    /*
     * we can quit now if clipping excludes the entire curve
     */
    if (vwk->clip) {
        if (((xc + xrad) < vwk->xmn_clip) || ((xc - xrad) > vwk->xmx_clip) ||
            ((yc + yrad) < vwk->ymn_clip) || ((yc - yrad) > vwk->ymx_clip))
            return;
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
