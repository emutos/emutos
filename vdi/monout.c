/*
 * monout.c - Graphical higher level output functions in C
 *
 * Copyright (c) 1999 Caldera, Inc. and Authors:
 *               1999 Johan Klockars
 *               2002 The EmuTOS development team
 *                    SCC
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "vdidef.h"
#include "gsxextrn.h"
#include "lineavars.h"
#include "tosvars.h"
#include "kprint.h"



/* Definitions for sine and cosinus */
#define    HALFPI    900
#define    PI        1800
#define    TWOPI     3600



extern void dr_recfl();



/* Prototypes local to this module */
void do_arrow();
void wline();
void plygn();
void pline();
void gdp_rbox();
void gdp_arc();
void clc_nsteps();
void gdp_ell();
void clc_arc();
void quad_xform(WORD quad, WORD x, WORD y, WORD *tx, WORD *ty);
void do_circ(WORD cx, WORD cy);
void perp_off(WORD * px, WORD * py);

//static void drawpixel(UWORD x, UWORD y, int linebit);
void abline();

/* Marker definitions */

static WORD m_dot[] = { 1, 2, 0, 0, 0, 0 };
static WORD m_plus[] = { 2, 2, 0, -3, 0, 3, 2, -4, 0, 4, 0 };
static WORD m_star[] = { 3, 2, 0, -3, 0, 3, 2, 3, 2, -3, -2, 2, 3, -2, -3, 2};
static WORD m_square[] = { 1, 5, -4, -3, 4, -3, 4, 3, -4, 3, -4, -3 };
static WORD m_cross[] = { 2, 2, -4, -3, 4, 3, 2, -4, 3, 4, -3 };
static WORD m_dmnd[] = { 1, 5, -4, 0, 0, -3, 4, 0, 0, 3, -4, 0 };



/*
 * smul_div - signed integer multiply and divide
 *
 * smul_div (m1,m2,d1)
 * 
 * ( ( m1 * m2 ) / d1 ) + 1/2
 *
 * m1 = signed 16 bit integer
 * m2 = unsigned 15 bit integer
 * d1 = signed 16 bit integer
 */

int smul_div(WORD m1, WORD m2, WORD d1)
{
    return (short)((m1 * (long)m2) / d1);
}


/*
 * v_clrwk - clear screen
 *
 * Screen is cleared between v_bas_ad and phystop.
 */

void v_clrwk()
{
    UBYTE * addr;               /* pointer to screen longword */

    /* clear the screen */
    for (addr = v_bas_ad; addr < (UBYTE *)phystop; addr++) {
        *addr = 0;             /* clear the long word */
    }
}



/*
 * v_pline -
 */

void v_pline()
{
    REG WORD l;
    REG struct attribute *work_ptr;

    work_ptr = cur_work;
    l = work_ptr->line_index;
    LN_MASK = (l < 6) ? LINE_STYLE[l] : work_ptr->ud_ls;

    l = work_ptr->line_color;
    FG_BP_1 = (l & 1);
    FG_BP_2 = (l & 2);
    FG_BP_3 = (l & 4);
    FG_BP_4 = (l & 8);

    if (work_ptr->line_width == 1) {
        pline();
        work_ptr = cur_work;
        if ((work_ptr->line_beg | work_ptr->line_end) & ARROWED)
            do_arrow();
    } else
        wline();
}



/*
 * v_pmarker - Polymarker
 */

void v_pmarker()
{

/* If this constant goes greater than 5, you must increase size of sav_points */

#define MARKSEGMAX 5

    extern WORD m_dot[], m_plus[], m_star[], m_square[], m_cross[],
        m_dmnd[];

    static WORD *markhead[] = {
        m_dot, m_plus, m_star, m_square, m_cross, m_dmnd
    };

    WORD i, j, num_lines, num_vert, x_center, y_center, sav_points[10];
    WORD sav_index, sav_color, sav_width, sav_beg, sav_end;
    WORD *mrk_ptr, *old_ptsin, scale, num_points, *src_ptr;
    REG WORD h, *pts_in, *m_ptr;
    REG struct attribute *work_ptr;

    /* Save the current polyline attributes which will be used. */

    work_ptr = cur_work;
    sav_index = work_ptr->line_index;
    sav_color = work_ptr->line_color;
    sav_width = work_ptr->line_width;
    sav_beg = work_ptr->line_beg;
    sav_end = work_ptr->line_end;

    /* Set the appropriate polyline attributes. */

    work_ptr->line_index = 0;
    work_ptr->line_color = work_ptr->mark_color;
    work_ptr->line_width = 1;
    work_ptr->line_beg = 0;
    work_ptr->line_end = 0;
    CLIP = 1;

    scale = work_ptr->mark_scale;

    /* Copy the PTSIN pointer since we will be doing polylines */

    num_vert = CONTRL[1];
    src_ptr = old_ptsin = PTSIN;
    PTSIN = sav_points;

    /* Loop over the number of points. */

    for (i = 0; i < num_vert; i++) {

        pts_in = src_ptr;
        x_center = *pts_in++;
        y_center = *pts_in++;
        src_ptr = pts_in;

        /* Get the pointer to the appropriate marker type definition. */

        m_ptr = markhead[cur_work->mark_index];
        num_lines = *m_ptr++;

        /* Loop over the number of polylines which define the marker. */

        for (j = 0; j < num_lines; j++) {

            num_points = CONTRL[1] = *m_ptr++;  /* How many points?  Get
                                                   them.  */

            pts_in = sav_points;
            for (h = 0; h < num_points; h++) {
                *pts_in++ = x_center + scale * (*m_ptr++);
                *pts_in++ = y_center + scale * (*m_ptr++);
            }                   /* End for:  extract points. */

            /* Output the polyline. */

            mrk_ptr = m_ptr;    /* Save for next pass */
            v_pline();
            m_ptr = mrk_ptr;
        }                       /* End for:  over the number of polylines
                                   defining the marker. */

    }                           /* End for:  over marker points. */

    /* Restore the PTSIN pointer */

    PTSIN = old_ptsin;

    /* Restore the current polyline attributes. */

    work_ptr = cur_work;
    work_ptr->line_index = sav_index;
    work_ptr->line_color = sav_color;
    work_ptr->line_width = sav_width;
    work_ptr->line_beg = sav_beg;
    work_ptr->line_end = sav_end;
}

/*
 * v_fillarea . Fill an area
 */

void v_fillarea()
{
    plygn();
}

/*
 * v_gdp - Major opcode for graphics device primitives
 */

void v_gdp()
{
    WORD i, ltmp_end, rtmp_end;
    REG WORD *xy_pointer;
    REG struct attribute *work_ptr;

    i = *(CONTRL + 5);
    xy_pointer = PTSIN;
    work_ptr = cur_work;

    if ((i > 0) && (i < 11)) {
        i--;
        switch (i) {
        case 0:         /* GDP BAR - converted to alpha 2 RJG 12-1-84 
                                 */
            dr_recfl();
            if (cur_work->fill_per == TRUE) {
                LN_MASK = 0xffff;

                xy_pointer = PTSIN;
                *(xy_pointer + 5) = *(xy_pointer + 7) = *(xy_pointer + 3);
                *(xy_pointer + 3) = *(xy_pointer + 9) = *(xy_pointer + 1);
                *(xy_pointer + 4) = *(xy_pointer + 2);
                *(xy_pointer + 6) = *(xy_pointer + 8) = *(xy_pointer);

                *(CONTRL + 1) = 5;

                pline();
            }
            break;

        case 1:         /* GDP ARC */
        case 2:         /* GDP PIE */
            gdp_arc();
            break;

        case 3:         /* GDP CIRCLE */
            xc = *xy_pointer;
            yc = *(xy_pointer + 1);
            xrad = *(xy_pointer + 4);
            yrad = smul_div(xrad, xsize, ysize);
            del_ang = 3600;
            beg_ang = 0;
            end_ang = 3600;
            clc_nsteps();
            clc_arc();
            break;

        case 4:         /* GDP ELLIPSE */
            xc = *xy_pointer;
            yc = *(xy_pointer + 1);
            xrad = *(xy_pointer + 2);
            yrad = *(xy_pointer + 3);
            if (work_ptr->xfm_mode < 2)
                yrad = yres - yrad;
            del_ang = 3600;
            beg_ang = 0;
            end_ang = 0;
            clc_nsteps();
            clc_arc();
            break;

        case 5:         /* GDP ELLIPTICAL ARC */
        case 6:         /* GDP ELLIPTICAL PIE */
            gdp_ell();
            break;

        case 7:         /* GDP Rounded Box */
            ltmp_end = work_ptr->line_beg;
            work_ptr->line_beg = SQUARED;
            rtmp_end = work_ptr->line_end;
            work_ptr->line_end = SQUARED;
            gdp_rbox();
            work_ptr = cur_work;
            work_ptr->line_beg = ltmp_end;
            work_ptr->line_end = rtmp_end;
            break;

        case 8:         /* GDP Rounded Filled Box */
            gdp_rbox();
            break;

        case 9:         /* GDP Justified Text */
            d_justified();
            break;
        }
    }
}



/*
 * vql_attr - Inquire current polyline attributes
 */

void vql_attr()
{
    REG WORD *pointer;
    REG struct attribute *work_ptr;

    pointer = INTOUT;
    work_ptr = cur_work;
    *pointer++ = work_ptr->line_index + 1;
    *pointer++ = REV_MAP_COL[work_ptr->line_color];
    *pointer = WRT_MODE + 1;

    pointer = PTSOUT;
    *pointer++ = work_ptr->line_width;
    *pointer = 0;

    pointer = CONTRL;
    *(pointer + 2) = 1;
    *(pointer + 4) = 3;
}



/*
 * vql_attr - Inquire current polymarker attributes
 */

void vqm_attr()
{
    REG WORD *pointer;
    REG struct attribute *work_ptr;

    pointer = INTOUT;
    work_ptr = cur_work;
    *pointer++ = work_ptr->mark_index;
    *pointer++ = REV_MAP_COL[work_ptr->mark_color];
    *pointer = WRT_MODE + 1;

    pointer = PTSOUT;
    *pointer++ = 0;
    *pointer = work_ptr->mark_height;

    pointer = CONTRL;
    *(pointer + 4) = 3;
    *(pointer + 2) = 1;
    flip_y = 1;
}



/*
 * vql_attr - Inquire current fill area attributes
 */

void vqf_attr()
{
    REG WORD *pointer;
    REG struct attribute *work_ptr;

    pointer = INTOUT;
    work_ptr = cur_work;
    *pointer++ = work_ptr->fill_style;
    *pointer++ = REV_MAP_COL[work_ptr->fill_color];
    *pointer++ = work_ptr->fill_index + 1;
    *pointer++ = WRT_MODE + 1;
    *pointer = work_ptr->fill_per;

    *(CONTRL + 4) = 5;
}



/*
 * code - helper function
 */

WORD code(WORD x, WORD y)
{
    WORD clip_flag;

    clip_flag = 0;
    if (x < XMN_CLIP)
        clip_flag = 1;
    else if (x > XMX_CLIP)
        clip_flag = 2;
    if (y < YMN_CLIP)
        clip_flag += 4;
    else if (y > YMX_CLIP)
        clip_flag += 8;
    return (clip_flag);
}



/*
 * clip_line - helper function
 */

WORD clip_line()
{
    WORD deltax, deltay, x1y1_clip_flag, x2y2_clip_flag, line_clip_flag;
    REG WORD *x, *y;

    while ((x1y1_clip_flag = code(X1, Y1)) |
           (x2y2_clip_flag = code(X2, Y2))) {
        if ((x1y1_clip_flag & x2y2_clip_flag))
            return (FALSE);
        if (x1y1_clip_flag) {
            line_clip_flag = x1y1_clip_flag;
            x = &X1;
            y = &Y1;
        } else {
            line_clip_flag = x2y2_clip_flag;
            x = &X2;
            y = &Y2;
        }
        deltax = X2 - X1;
        deltay = Y2 - Y1;
        if (line_clip_flag & 1) {       /* left ? */
            *y = Y1 + smul_div(deltay, (XMN_CLIP - X1), deltax);
            *x = XMN_CLIP;
        } else if (line_clip_flag & 2) {        /* right ? */
            *y = Y1 + smul_div(deltay, (XMX_CLIP - X1), deltax);
            *x = XMX_CLIP;
        } else if (line_clip_flag & 4) {        /* top ? */
            *x = X1 + smul_div(deltax, (YMN_CLIP - Y1), deltay);
            *y = YMN_CLIP;
        } else if (line_clip_flag & 8) {        /* bottom ? */
            *x = X1 + smul_div(deltax, (YMX_CLIP - Y1), deltay);
            *y = YMX_CLIP;
        }
    }
    return (TRUE);              /* segment now cliped  */
}


/*
 * pline - draw a poly-line
 */

void pline(void)
{
    short i, j;

    j = 0;
    LSTLIN = FALSE;
    for(i = CONTRL[1] - 1; i > 0; i--) {
        if (i == 1)
            LSTLIN = TRUE;
        X1 = PTSIN[j++];
        Y1 = PTSIN[j++];
        X2 = PTSIN[j];
        Y2 = PTSIN[j+1];
        if (!CLIP || clip_line())
            abline();
    }
}


/*
 * plygn - draw a filled polygone
 */

void plygn()
{
    REG WORD *pointer, i, k;

    i = cur_work->fill_color;
    FG_BP_1 = (i & 1);
    FG_BP_2 = (i & 2);
    FG_BP_3 = (i & 4);
    FG_BP_4 = (i & 8);
    LSTLIN = FALSE;

    pointer = PTSIN;
    pointer++;

    fill_maxy = fill_miny = *pointer++;
    pointer++;

    for (i = (*(CONTRL + 1) - 1); i > 0; i--) {
        k = *pointer++;
        pointer++;
        if (k < fill_miny)
            fill_miny = k;
        else
            if (k > fill_maxy)
                fill_maxy = k;
    }
    if (CLIP) {
        if (fill_miny < YMN_CLIP) {
            if (fill_maxy >= YMN_CLIP) {        /* plygon starts before clip */
                fill_miny = YMN_CLIP - 1;       /* plygon partial overlap */
                if (fill_miny < 1)
                    fill_miny = 1;
            } else
                return;         /* plygon entirely before clip */
        }
        if (fill_maxy > YMX_CLIP) {
            if (fill_miny <= YMX_CLIP)  /* plygon ends after clip */
                fill_maxy = YMX_CLIP;   /* plygon partial overlap */
            else
                return;         /* plygon entirely after clip */
        }
    }
    k = *(CONTRL + 1) * 2;
    pointer = PTSIN;
    *(pointer + k) = *pointer;
    *(pointer + k + 1) = *(pointer + 1);
    for (Y1 = fill_maxy; Y1 > fill_miny; Y1--) {
        fil_intersect = 0;
        CLC_FLIT();
    }
    if (cur_work->fill_per == TRUE) {
        LN_MASK = 0xffff;
        (*(CONTRL + 1))++;
        pline();
    }
}



/* Sines of angles 1 - 90 degrees normalized between 0-32767. */
static WORD sin_tbl[92] = {
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
    WORD direction;             /* when checking quadrants, must keep   */
    /* track of direction in array.     */
    WORD index, remainder, tmpsin;      /* holder for sin. */
    WORD quadrant;              /* 0-3 = 1st, 2nd, 3rd, 4th.        */

    while (angle > 3600)
        angle -= 3600;
    quadrant = angle / HALFPI;
    direction = 1;
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



short Isqrt(unsigned long x)
{
    unsigned long s1, s2;

    if (x < 2)
        return x;

    s1 = x;
    s2 = 2;
    do {
        s1 /= 2;
        s2 *= 2;
    } while (s1 > s2);

    s2 = (s1 + (s2 / 2)) / 2;

    do {
        s1 = s2;
        s2 = (x / s1 + s1) / 2;
    } while (s1 > s2);

    return (short)s1;
}



/*
 * _vec_len
 *
 * This routine computes the length of a vector using the formula:
 *
 * sqrt(dx*dx + dy*dy)
 */

WORD vec_len(WORD dx, WORD dy)
{
    return (Isqrt(dx*dx + dy*dy));
}

/*
 * gdp_rbox - draws an rbox
 */

void gdp_rbox()
{
    REG WORD i, j;
    WORD rdeltax, rdeltay;
    REG struct attribute *work_ptr;
    REG WORD *pointer = NULL;

    arb_corner(pointer, LLUR);

    pointer = PTSIN;
    X1 = *pointer++;
    Y1 = *pointer++;
    X2 = *pointer++;
    Y2 = *pointer;

    rdeltax = (X2 - X1) / 2;
    rdeltay = (Y1 - Y2) / 2;

    xrad = xres >> 6;
    if (xrad > rdeltax)
        xrad = rdeltax;

    yrad = smul_div(xrad, xsize, ysize);
    if (yrad > rdeltay)
        yrad = rdeltay;

    pointer = PTSIN;
    *pointer++ = 0;
    *pointer++ = yrad;
    *pointer++ = smul_div(Icos(675), xrad, 32767);
    *pointer++ = smul_div(Isin(675), yrad, 32767);
    *pointer++ = smul_div(Icos(450), xrad, 32767);
    *pointer++ = smul_div(Isin(450), yrad, 32767);
    *pointer++ = smul_div(Icos(225), xrad, 32767);
    *pointer++ = smul_div(Isin(225), yrad, 32767);
    *pointer++ = xrad;
    *pointer = 0;

    pointer = PTSIN;
    xc = X2 - xrad;
    yc = Y1 - yrad;
    j = 10;
    for (i = 9; i >= 0; i--) {
        *(pointer + j + 1) = yc + *(pointer + i--);
        *(pointer + j) = xc + *(pointer + i);
        j += 2;
    }
    xc = X1 + xrad;
    j = 20;
    for (i = 0; i < 10; i++) {
        *(pointer + j++) = xc - *(pointer + i++);
        *(pointer + j++) = yc + *(pointer + i);
    }
    yc = Y2 + yrad;
    j = 30;
    for (i = 9; i >= 0; i--) {
        *(pointer + j + 1) = yc - *(pointer + i--);
        *(pointer + j) = xc - *(pointer + i);
        j += 2;
    }
    xc = X2 - xrad;
    j = 0;
    for (i = 0; i < 10; i++) {
        *(pointer + j++) = xc + *(pointer + i++);
        *(pointer + j++) = yc - *(pointer + i);
    }
    *(pointer + 40) = *pointer;
    *(pointer + 41) = *(pointer + 1);

    pointer = CONTRL;
    *(pointer + 1) = 21;
    if (*(pointer + 5) == 8) {
        work_ptr = cur_work;
        i = work_ptr->line_index;
        LN_MASK = (i < 6) ? LINE_STYLE[i] : work_ptr->ud_ls;
        i = work_ptr->line_color;
        FG_BP_1 = (i & 1);
        FG_BP_2 = (i & 2);
        FG_BP_3 = (i & 4);
        FG_BP_4 = (i & 8);

        if (work_ptr->line_width == 1) {
            pline();
        } else
            wline();
    } else
        plygn();

    return;
}



/*
 * gdp_arc - draw an arc
 */

void gdp_arc()
{
    REG WORD *pointer;

    pointer = INTIN;

    beg_ang = *pointer++;
    end_ang = *pointer;
    del_ang = end_ang - beg_ang;
    if (del_ang < 0)
        del_ang += 3600;

    pointer = PTSIN;
    xrad = *(pointer + 6);
    yrad = smul_div(xrad, xsize, ysize);
    clc_nsteps();
    n_steps = smul_div(del_ang, n_steps, 3600);
    if (n_steps == 0)
        return;
    xc = *pointer++;
    yc = *pointer;
    clc_arc();
    return;
}



/*
 * clc_nsteps - calculates
 */

void clc_nsteps()
{
    if (xrad > yrad)
        n_steps = xrad;
    else
        n_steps = yrad;
    n_steps = n_steps >> 2;
    if (n_steps < 16)
        n_steps = 16;
    else {
        if (n_steps > MAX_ARC_CT)
            n_steps = MAX_ARC_CT;
    }
    return;
}



/*
 * gdp_ell - draws an ell
 */

void gdp_ell()
{
    REG WORD *pointer;

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
    if (cur_work->xfm_mode < 2)
        yrad = yres - yrad;
    clc_nsteps();
    n_steps = smul_div(del_ang, n_steps, 3600);
    if (n_steps == 0)
        return;
    clc_arc();
    return;
}



/*
 * clc_pts - calculates
 */

void clc_pts(WORD j)
{
    WORD k;
    REG WORD *pointer;

    pointer = PTSIN;
    k = smul_div(Icos(angle), xrad, 32767) + xc;
    *(pointer + j) = k;
    k = yc - smul_div(Isin(angle), yrad, 32767);        /* FOR RASTER CORDS. */
    *(pointer + j + 1) = k;
}



/*
 * clc_arc - calculates
 */

void clc_arc()
{
    WORD i, j;
    REG WORD *cntl_ptr, *xy_ptr;

    if (CLIP) {
        if (((xc + xrad) < XMN_CLIP) || ((xc - xrad) > XMX_CLIP) ||
            ((yc + yrad) < YMN_CLIP) || ((yc - yrad) > YMX_CLIP))
            return;
    }
    start = angle = beg_ang;
    i = j = 0;
    clc_pts(j);
    for (i = 1; i < n_steps; i++) {
        j += 2;
        angle = smul_div(del_ang, i, n_steps) + start;
        clc_pts(j);
    }
    j += 2;
    i = n_steps;
    angle = end_ang;
    clc_pts(j);

    /*
     * If pie wedge draw to center and then close. If arc or circle, do
     * nothing because loop should close circle.
     */

    cntl_ptr = CONTRL;
    xy_ptr = PTSIN;

    *(cntl_ptr + 1) = n_steps + 1;      /* since loop in Clc_arc starts at 0 */
    if ((*(cntl_ptr + 5) == 3) || (*(cntl_ptr + 5) == 7)) {     /* pie wedge */
        n_steps++;
        j += 2;
        *(xy_ptr + j) = xc;
        *(xy_ptr + j + 1) = yc;
        *(cntl_ptr + 1) = n_steps + 1;
    }
    if ((*(cntl_ptr + 5) == 2) || (*(cntl_ptr + 5) == 6))       /* open arc */
        v_pline();

    else
        plygn();
}



/*
 * cir_dda - Used in wline()
 */

void cir_dda()
{
    WORD i, j;
    REG WORD *xptr, *yptr, x, y, d;

    /* Calculate the number of vertical pixels required. */
    d = cur_work->line_width;
    num_qc_lines = (d * xsize / ysize) / 2 + 1;

    /* Initialize the circle DDA.  "y" is set to the radius. */
    line_cw = d;
    y = (d + 1) / 2;
    x = 0;
    d = 3 - 2 * y;

    xptr = &q_circle[x];
    yptr = &q_circle[y];

    /* Do an octant, starting at north.  The values for the next octant */
    /* (clockwise) will be filled by transposing x and y.               */
    while (x < y) {
        *yptr = x;
        *xptr = y;

        if (d < 0)
            d = d + 4 * x + 6;
        else {
            d = d + 4 * (x - y) + 10;
            yptr--;
            y--;
        }
        xptr++;
        x++;
    }

    if (x == y)
        q_circle[x] = x;

    /* Fake a pixel averaging when converting to non-1:1 aspect ratio. */
    x = 0;

    yptr = q_circle;
    for (i = 0; i < num_qc_lines; i++) {
        y = ((2 * i + 1) * ysize / xsize) / 2;
        d = 0;

        xptr = &q_circle[x];
        for (j = x; j <= y; j++)
            d += *xptr++;

        *yptr++ = d / (y - x + 1);
        x = y + 1;
    }                           /* End for loop. */
}



/*
 * wline - draw a line with width >1
 */

#define ABS(x) ((x) >= 0 ? (x) : -(x))

void wline()
{
    WORD i, k, box[10];         /* box two high to close polygon */

    WORD numpts, wx1, wy1, wx2, wy2, vx, vy;

    WORD *old_ptsin, *src_ptr;

    REG WORD *pointer, x, y, d, d2;
    REG struct attribute *work_ptr;

    /* Don't attempt wide lining on a degenerate polyline */

    if ((numpts = *(CONTRL + 1)) < 2)
        return;

    work_ptr = cur_work;

    if (work_ptr->line_width != line_cw) {
        cir_dda();
    }

    /* If the ends are arrowed, output them. */
    if ((work_ptr->line_beg | work_ptr->line_end) & ARROWED)
        do_arrow();
    s_fa_attr();

    /* Initialize the starting point for the loop. */

    old_ptsin = pointer = PTSIN;
    wx1 = *pointer++;
    wy1 = *pointer++;
    src_ptr = pointer;

    /* If the end style for the first point is not squared, output a circle. */
    if (s_begsty != SQUARED) {
        do_circ(wx1, wy1);
    }
    /* Loop over the number of points passed in. */

    for (i = 1; i < numpts; i++) {
        /* Get the ending point for the line segment and the vector from the */
        /* start to the end of the segment.                                  */

        pointer = src_ptr;
        wx2 = *pointer++;
        wy2 = *pointer++;
        src_ptr = pointer;

        vx = wx2 - wx1;
        vy = wy2 - wy1;

        /* Ignore lines of zero length. */

        if ((vx == 0) && (vy == 0))
            continue;

        /* Calculate offsets to fatten the line.  If the line segment is */
        /* horizontal or vertical, do it the simple way.                 */

        if (vx == 0) {
            vx = q_circle[0];
            vy = 0;
        }
        /* End if:  vertical. */
        else if (vy == 0) {
            vx = 0;
            vy = num_qc_lines - 1;
        }
        /* End else if:  horizontal. */
        else {
            /* Find the offsets in x and y for a point perpendicular */
            /* to the line segment at the appropriate distance. */
            k = smul_div(-vy, ysize, xsize);
            vy = smul_div(vx, xsize, ysize);
            vx = k;
            perp_off(&vx, &vy);
        }                       /* End else:  neither horizontal nor
                                   vertical. */

        /* Prepare the control and points parameters for the polygon call. */

        *(CONTRL + 1) = 4;

        PTSIN = pointer = box;

        x = wx1;
        y = wy1;
        d = vx;
        d2 = vy;

        *pointer++ = x + d;
        *pointer++ = y + d2;
        *pointer++ = x - d;
        *pointer++ = y - d2;

        x = wx2;
        y = wy2;

        *pointer++ = x - d;
        *pointer++ = y - d2;
        *pointer++ = x + d;
        *pointer = y + d2;

        plygn();

        /* restore the PTSIN pointer */
        PTSIN = old_ptsin;

        /* If the terminal point of the line segment is an internal joint, */
        /* output a filled circle.                                         */
        if ((i < numpts - 1) || (s_endsty != SQUARED))
            do_circ(wx2, wy2);
        /* end point becomes the starting point for the next line segment. */
        wx1 = wx2;
        wy1 = wy2;
    }                           /* End for:  over number of points. */

    /* Restore the attribute environment. */
    r_fa_attr();
}                               /* End "wline". */



/*
 * perp_off -
 */

void perp_off(WORD * px, WORD * py)
{
    REG WORD *vx, *vy, *pcircle, u, v;
    WORD x, y, quad, magnitude, min_val;
    WORD x_val = 0;
    WORD y_val = 0;

    vx = px;
    vy = py;

    pcircle = q_circle;

    /* Mirror transform the vector so that it is in the first quadrant. */
    if (*vx >= 0)
        quad = (*vy >= 0) ? 1 : 4;
    else
        quad = (*vy >= 0) ? 2 : 3;

    quad_xform(quad, *vx, *vy, &x, &y);

    /* Traverse the circle in a dda-like manner and find the coordinate pair */
    /* (u, v) such that the magnitude of (u*y - v*x) is minimized.  In case
       of */
    /* a tie, choose the value which causes (u - v) to be minimized.  If not */
    /* possible, do something. */

    min_val = 32767;
    u = *pcircle;
    v = 0;
    while (TRUE) {
        /* Check for new minimum, same minimum, or finished. */
        if (((magnitude = ABS(u * y - v * x)) < min_val) ||
            ((magnitude == min_val) && (ABS(x_val - y_val) > ABS(u - v)))) {
            min_val = magnitude;
            x_val = u;
            y_val = v;
        }
        /* End if:  new minimum. */
        else
            break;

        /* Step to the next pixel. */
        if (v == num_qc_lines - 1) {
            if (u == 1)
                break;
            else
                u--;
        }
        /* End if:  doing top row. */
        else {
            if (pcircle[v + 1] >= u - 1) {
                v++;
                u = pcircle[v];
            } /* End if:  do next row up. */
            else {
                u--;
            }                   /* End else:  continue on row. */
        }                       /* End else:  other than top row. */
    }                           /* End FOREVER loop. */

    /* Transform the solution according to the quadrant. */

    quad_xform(quad, x_val, y_val, vx, vy);
}                               /* End "perp_off". */



/*
 * quad_xform - Transform according to the quadrant.
 */

void quad_xform(WORD quad, WORD x, WORD y, WORD *tx, WORD *ty)
{
    switch (quad) {
    case 1:
    case 4:
        *tx = x;
        break;

    case 2:
    case 3:
        *tx = -x;
        break;
    }                           /* End switch. */

    switch (quad) {
    case 1:
    case 2:
        *ty = y;
        break;

    case 3:
    case 4:
        *ty = -y;
        break;
    }                           /* End switch. */
}                               /* End "quad_xform". */



/*
 * do_circ - draw a circle
 */

void do_circ(WORD cx, WORD cy)
{
    WORD k;
    REG WORD *pointer;

    /* Only perform the act if the circle has radius. */
    if (num_qc_lines > 0) {
        /* Do the horizontal line through the center of the circle. */

        pointer = q_circle;
        X1 = cx - *pointer;
        X2 = cx + *pointer;
        Y1 = Y2 = cy;
        if (clip_line())
            abline();

        /* Do the upper and lower semi-circles. */

        for (k = 1; k < num_qc_lines; k++) {
            /* Upper semi-circle. */

            pointer = &q_circle[k];
            X1 = cx - *pointer;
            X2 = cx + *pointer;
            Y1 = Y2 = cy - k;
            if (clip_line()) {
                abline();
                pointer = &q_circle[k];
            }

            /* Lower semi-circle. */

            X1 = cx - *pointer;
            X2 = cx + *pointer;
            Y1 = Y2 = cy + k;
            if (clip_line())
                abline();
        }                       /* End for. */
    }                           /* End if:  circle has positive radius. */
}                               /* End "do_circ". */



/*
 * s_fa_attr - Save the fill area attribute
 */

void s_fa_attr()
{
    REG struct attribute *work_ptr;

    /* Set up the fill area attribute environment. */
    work_ptr = cur_work;

    LN_MASK = LINE_STYLE[0];
    s_fil_col = work_ptr->fill_color;
    work_ptr->fill_color = work_ptr->line_color;
    s_fill_per = work_ptr->fill_per;
    work_ptr->fill_per = TRUE;
    patptr = &SOLID;
    patmsk = 0;
    s_begsty = work_ptr->line_beg;
    s_endsty = work_ptr->line_end;
    work_ptr->line_beg = SQUARED;
    work_ptr->line_end = SQUARED;
}                               /* End "s_fa_attr". */



/*
 * r_fa_attr - Restore the fill area attribute
 */

void r_fa_attr()
{
    REG struct attribute *work_ptr;

    /* Restore the fill area attribute environment. */
    work_ptr = cur_work;

    work_ptr->fill_color = s_fil_col;
    work_ptr->fill_per = s_fill_per;
    work_ptr->line_beg = s_begsty;
    work_ptr->line_end = s_endsty;
}                               /* End "r_fa_attr". */



/*
 * do_arrow - Draw an arrow
 */

void do_arrow()
{
    WORD x_start, y_start;
    WORD new_x_start, new_y_start;
    REG WORD *pts_in;

    /* Set up the attribute environment. */
    s_fa_attr();

    /* Function "arrow" will alter the end of the line segment.  Save the */
    /* starting point of the polyline in case two calls to "arrow" are    */
    /* necessary.                                                         */
    pts_in = PTSIN;
    new_x_start = x_start = *pts_in;
    new_y_start = y_start = *(pts_in + 1);

    if (s_begsty & ARROWED) {
        arrow(pts_in, 2);
        pts_in = PTSIN;         /* arrow calls plygn which trashes regs */
        new_x_start = *pts_in;
        new_y_start = *(pts_in + 1);
    }
    /* End if:  beginning point is arrowed. */
    if (s_endsty & ARROWED) {
        *pts_in = x_start;
        *(pts_in + 1) = y_start;
        arrow((pts_in + 2 ** (CONTRL + 1) - 2), -2);
        pts_in = PTSIN;         /* arrow calls plygn which trashes regs */
        *pts_in = new_x_start;
        *(pts_in + 1) = new_y_start;
    }

    /* End if:  ending point is arrowed. */
    /* Restore the attribute environment. */
    r_fa_attr();
}                               /* End "do_arrow". */



/*
 * arrow - Draw an arrow
 */

void arrow(WORD * xy, WORD inc)
{
    WORD arrow_len, arrow_wid, line_len;
    WORD *xybeg, sav_contrl, triangle[8];       /* triangle 2 high to close
                                                   polygon */
    WORD dx, dy;
    WORD base_x, base_y, ht_x, ht_y;
    WORD *old_ptsin;
    REG WORD *ptr1, *ptr2, temp, i;

    line_len = dx = dy = 0;

    /* Set up the arrow-head length and width as a function of line width. */

    temp = cur_work->line_width;
    arrow_wid = (arrow_len = (temp == 1) ? 8 : 3 * temp - 1) / 2;

    /* Initialize the beginning pointer. */

    xybeg = ptr1 = ptr2 = xy;

    /* Find the first point which is not so close to the end point that it */
    /* will be obscured by the arrowhead.                                  */

    temp = *(CONTRL + 1);
    for (i = 1; i < temp; i++) {
        /* Find the deltas between the next point and the end point.
           Transform */
        /* to a space such that the aspect ratio is uniform and the x axis */
        /* distance is preserved. */

        ptr1 += inc;
        dx = *ptr2 - *ptr1;
        dy = smul_div(*(ptr2 + 1) - *(ptr1 + 1), ysize, xsize);

        /* Get the length of the vector connecting the point with the end
           point. */
        /* If the vector is of sufficient length, the search is over. */

        if ((line_len = vec_len(ABS(dx), ABS(dy))) >= arrow_len)
            break;
    }                           /* End for:  over i. */

    /* Set xybeg to the point we found */

    xybeg = ptr1;

    /* If the longest vector is insufficiently long, don't draw an arrow. */

    if (line_len < arrow_len)
        return;

    /* Rotate the arrow-head height and base vectors.  Perform calculations */
    /* in 1000x space.                                                      */

    ht_x = smul_div(arrow_len, smul_div(dx, 1000, line_len), 1000);
    ht_y = smul_div(arrow_len, smul_div(dy, 1000, line_len), 1000);
    base_x = smul_div(arrow_wid, smul_div(dy, -1000, line_len), 1000);
    base_y = smul_div(arrow_wid, smul_div(dx, 1000, line_len), 1000);

    /* Transform the y offsets back to the correct aspect ratio space. */

    ht_y = smul_div(ht_y, xsize, ysize);
    base_y = smul_div(base_y, xsize, ysize);

    /* Save the vertice count */

    ptr1 = CONTRL;
    sav_contrl = *(ptr1 + 1);

    /* Build a polygon to send to plygn.  Build into a local array first */
    /* since xy will probably be pointing to the PTSIN array. */

    *(ptr1 + 1) = 3;
    ptr1 = triangle;
    ptr2 = xy;
    *ptr1 = *ptr2 + base_x - ht_x;
    *(ptr1 + 1) = *(ptr2 + 1) + base_y - ht_y;
    *(ptr1 + 2) = *ptr2 - base_x - ht_x;
    *(ptr1 + 3) = *(ptr2 + 1) - base_y - ht_y;
    *(ptr1 + 4) = *ptr2;
    *(ptr1 + 5) = *(ptr2 + 1);

    old_ptsin = PTSIN;
    PTSIN = ptr1;
    plygn();
    PTSIN = old_ptsin;

    /* Restore the vertex count. */

    *(CONTRL + 1) = sav_contrl;

    /* Adjust the end point and all points skipped. */

    ptr1 = xy;
    ptr2 = xybeg;
    *ptr1 -= ht_x;
    *(ptr1 + 1) -= ht_y;

    temp = inc;
    while ((ptr2 -= temp) != ptr1) {
        *ptr2 = *ptr1;
        *(ptr2 + 1) = *(ptr1 + 1);
    }                           /* End while. */
}                               /* End "arrow". */



/*
 * dsf_udpat - Update pattern
 */

void dsf_udpat()
{
    REG WORD *sp, *dp, i, count;
    REG struct attribute *work_ptr;

    work_ptr = cur_work;
    count = CONTRL[3];

    if (count == 16)
        work_ptr->multifill = 0;        /* Single Plane Pattern */
    else if (count == (INQ_TAB[4] * 16))
        work_ptr->multifill = 1;        /* Valid Multi-plane pattern */
    else
        return;             /* Invalid pattern, return */

    sp = INTIN;
    dp = &work_ptr->ud_patrn[0];
    for (i = 0; i < count; i++)
        *dp++ = *sp++;
}


/*
 * abline - draw a line (general purpose)
 *
 * This routine draws a line between (_X1,_Y1) and (_X2,_Y2).
 * The line is modified by the LN_MASK and WRT_MODE variables.
 * This routine handles all 3 interleaved bitplanes video resolutions.
 *
 * Note that for line-drawing the background color is always 0 (i.e., there
 * is no user-settable background color).  This fact allows coding short-cuts
 * in the implementation of "replace" and "not" modes, resulting in faster
 * execution of their inner loops.
 *
 * This routines is more or less the one from the original VDI asm part.
 * I could not take bresenham, because pixels were set improperly in
 * use with the polygone filling part, did look ugly.  (MAD)
 *
 * input:
 *     X1, Y1, X2, Y2 = coordinates.
 *     num_planes     = number of video planes. (resolution)
 *     LN_MASK        = line mask. (for dashed/dotted lines)
 *     WRT_MODE       = writing mode:
 *                          0 => replace mode.
 *                          1 => or mode.
 *                          2 => xor mode.
 *                          3 => not mode.
 *
 * output:
 *     LN_MASK rotated to proper alignment with (X2,Y2).
 */

void
abline ()
{
    void *adr;                  /* using void pointer is much faster */
    UWORD x1,y1,x2,y2;          /* the coordinates */
    WORD dx;                    /* width of rectangle around line */
    WORD dy;                    /* height of rectangle around line */
    WORD xinc;                  /* positive increase for each x step */
    WORD yinc;                  /* in/decrease for each y step */
    UWORD msk;
    int plane;
    UWORD linemask;             /* linestyle bits */
    WORD *color;

    /* Make x axis always goind up */
    if (X2 < X1) {
        /* if delta x < 0 then draw from point 2 to 1 */
        x1 = X2;
        y1 = Y2;
        x2 = X1;
        y2 = Y1;
    } else {
        /* positive, start with first point */
        x1 = X1;
        y1 = Y1;
        x2 = X2;
        y2 = Y2;
    }

    dx = x2 - x1;
    dy = y2 - y1;

    /* calculate increase values for x and y to add to actual address */
    if (dy < 0) {
        dy = -dy;                       /* make dy absolute */
        yinc = (LONG) -1 * v_lin_wr;    /* sub one line of bytes */
    } else {
        yinc = (LONG) v_lin_wr;         /* add one line of bytes */
    }
    xinc = v_planes<<1;                 /* add v_planes WORDS */

    adr = v_bas_ad;                     /* start of screen */
    adr += (LONG)y1 * v_lin_wr;         /* add y coordinate part of addr */
    adr += (x1&0xfff0)>>shft_off;       /* add x coordinate part of addr */
    msk = 0x8000 >> (x1&0xf);           /* initial bit position in WORD */
    linemask = LN_MASK;                 /* to avoid compiler warning */
    color = &FG_BP_1;

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        void *addr;
        WORD  eps;              /* epsilon */
        WORD  e1;               /* epsilon 1 */
        WORD  e2;               /* epsilon 2 */
        WORD  loopcnt;
        UWORD bit;

        /* load values fresh for this bitplane */
        addr = adr;             /* initial start address for changes */
        bit = msk;              /* initial bit position in WORD */
        linemask = LN_MASK;

        if (dx >= dy) {
            e1 = 2*dy;
            eps = -dx;
            e2 = 2*dx;

            switch (WRT_MODE) {
            case 3:              /* reverse transparent  */
                if (*color) {
                    for (loopcnt=dx;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr &= ~bit;
                        bit = bit >> 1| bit << 15;
                        if (bit&0x8000)
                            addr += xinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            addr += yinc;       /* increment y */
                        }
                    }
                } else {
                    for (loopcnt=dx;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr |= bit;
                        bit = bit >> 1| bit << 15;
                        if (bit&0x8000)
                            addr += xinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            addr += yinc;       /* increment y */
                        }
                    }
                }
                break;
            case 2:              /* xor */
                for (loopcnt=dx;loopcnt >= 0;loopcnt--) {
                    linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                    if (linemask&0x0001)
                        *(WORD*)addr ^= bit;
                    bit = bit >> 1| bit << 15;
                    if (bit&0x8000)
                        addr += xinc;
                    eps += e1;
                    if (eps >= 0 ) {
                        eps -= e2;
                        addr += yinc;       /* increment y */
                    }
                }
                break;
            case 1:              /* or */
                if (*color) {
                    for (loopcnt=dx;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr |= bit;
                        bit = bit >> 1| bit << 15;
                        if (bit&0x8000)
                            addr += xinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            addr += yinc;       /* increment y */
                        }
                    }
                } else {
                    for (loopcnt=dx;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr &= ~bit;
                        bit = bit >> 1| bit << 15;
                        if (bit&0x8000)
                            addr += xinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            addr += yinc;       /* increment y */
                        }
                    }
                }
                break;
            case 0:              /* not */
                if (*color) {
                    for (loopcnt=dx;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr |= bit;
                        else
                            *(WORD*)addr &= ~bit;
                        bit = bit >> 1| bit << 15;
                        if (bit&0x8000)
                            addr += xinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            addr += yinc;       /* increment y */
                        }
                    }
                }
            }
        } else {
            e1 = 2*dx;
            eps = - dy;
            e2 = 2*dy;

            switch (WRT_MODE) {
            case 3:              /* reverse transparent */
                if (*color) {
                    for (loopcnt=dy;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr &= ~bit;
                        addr += yinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            bit = bit >> 1| bit << 15;
                            if (bit&0x8000)
                                addr += xinc;
                        }
                    }
                } else {
                    for (loopcnt=dy;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr |= bit;
                        addr += yinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            bit = bit >> 1| bit << 15;
                            if (bit&0x8000)
                                addr += xinc;
                        }
                    }
                }
                break;
            case 2:              /* xor */
                for (loopcnt=dy;loopcnt >= 0;loopcnt--) {
                    linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                    if (linemask&0x0001)
                        *(WORD*)addr ^= bit;
                    addr += yinc;
                    eps += e1;
                    if (eps >= 0 ) {
                        eps -= e2;
                        bit = bit >> 1| bit << 15;
                        if (bit&0x8000)
                            addr += xinc;
                    }
                }
                break;
            case 1:              /* or */
                if (*color) {
                    for (loopcnt=dy;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr |= bit;
                        addr += yinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            bit = bit >> 1| bit << 15;
                            if (bit&0x8000)
                                addr += xinc;
                        }
                    }
                } else {
                    for (loopcnt=dy;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                        if (linemask&0x0001)
                            *(WORD*)addr &= ~bit;
                        addr += yinc;
                        eps += e1;
                        if (eps >= 0 ) {
                            eps -= e2;
                            bit = bit >> 1| bit << 15;
                            if (bit&0x8000)
                                addr += xinc;
                        }
                    }
                }
                break;
            case 0:              /* rep */
                if (!*color)
                    linemask = ~linemask;
                for (loopcnt=dy;loopcnt >= 0;loopcnt--) {
                    linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
                    if (linemask&0x0001)
                        *(WORD*)addr |= bit;
                    else
                        *(WORD*)addr &= ~bit;
                    addr += yinc;
                    eps += e1;
                    if (eps >= 0 ) {
                        eps -= e2;
                        bit = bit >> 1| bit << 15;
                        if (bit&0x8000)
                            addr += xinc;
                    }
                }
                if (!*color)
                    linemask = ~linemask;
            }
        }
        adr+=2;
        color++;
    }
    LN_MASK = linemask;
}






/*
 * st_fl_ptr -
 */

void st_fl_ptr()
{
    REG WORD fi, pm;
    REG WORD *pp = NULL;
    REG struct attribute *work_ptr;

    work_ptr = cur_work;
    fi = work_ptr->fill_index;
    pm = 0;
    switch (work_ptr->fill_style) {
    case 0:
        pp = &HOLLOW;
        break;

    case 1:
        pp = &SOLID;
        break;

    case 2:
        if (fi < 8) {
            pm = DITHRMSK;
            pp = &DITHER[fi * (pm + 1)];
        } else {
            pm = OEMMSKPAT;
            pp = &OEMPAT[(fi - 8) * (pm + 1)];
        }
        break;
    case 3:
        if (fi < 6) {
            pm = HAT_0_MSK;
            pp = &HATCH0[fi * (pm + 1)];
        } else {
            pm = HAT_1_MSK;
            pp = &HATCH1[(fi - 6) * (pm + 1)];
        }
        break;
    case 4:
        pm = 0x000f;
        pp = &work_ptr->ud_patrn[0];
        break;
    }
    work_ptr->patptr = pp;
    work_ptr->patmsk = pm;
}



/*
 * hzline_rep - draw a horizontal line in replace mode
 *
 * This routine is used by habline() and rectfill()
 */

inline
void hzline_rep(UWORD *addr, int dx, int leftpart, UWORD rightmask, UWORD leftmask, int patind)
{
    UWORD *adr;
    UWORD pattern;
    int pixels;                   /* counting down the rest of dx */
    int bw;
    int planes;
    int plane;
    WORD *color;
    int patadd;                         /* advance for multiplane patterns */

    /* precalculate, what to draw */
    patadd = multifill ? 16 : 0;        /* multi plane pattern offset */
    color = &FG_BP_1;
    planes = v_planes;

    for (plane = planes-1; plane >= 0; plane-- ) {

        adr = addr;
        pixels = dx-16;

        /* load values fresh for this bitplane */
        if (*color++)
            pattern = patptr[patind];
        else
            pattern = 0;

        /* check, if the line is completely contained within one WORD */
        if (pixels+leftpart < 0) {
            UWORD bits;

            /* Isolate the necessary pixels */
            bits = *adr;        /* get data from screen address */
            bits ^= pattern;    /* xor the pattern with the source */
            bits &= leftmask|rightmask; /* isolate the bits outside the fringe */
            bits ^= pattern;    /* restore the bits outside the fringe */
            *adr = bits;        /* write back the result */
        } else {
            UWORD bits;
            /* Draw the left fringe */
            if (leftmask) {
                bits = *adr;    /* get data from screen address */
                bits ^= pattern;  /* xor the pattern with the source */
                bits &= leftmask; /* isolate the bits outside the fringe */
                bits ^= pattern;  /* restore the bits outside the fringe */
                *adr = bits;      /* write back the result */

                adr += planes;;
                pixels -= 16;
                pixels += leftpart;
            }
            /* Full bytes */
            for (bw = pixels >> 4;bw>=0;bw--) {
                *adr = pattern;
                adr += planes;
            }
            /* Draw the right fringe */
            if (~rightmask) {
                bits = *adr;            /* get data from screen address */
                bits ^= pattern;  /* xor the pattern with the source */
                bits &= rightmask;/* isolate the bits outside the fringe */
                bits ^= pattern;  /* restore the bits outside the fringe */
                *adr = bits;      /* write back the result */
            }
        }
        addr++; /* advance one WORD to next plane */
        patind += patadd;
    }
}



/*
 * hzline_or - draw a horizontal line in transparent mode
 *
 * If the foreground color requires that bits in the current plane be set
 * then 'or' the mask with the source.  Otherwise, the foreground color
 * requires that bits in the current plane be cleared so 'and' the complement
 * of the mask with the source.  Bits that would be drawn with the background
 * color (black) are left unchanged.
 *
 * This routine is used by habline() and rectfill()
 */

static
void hzline_or(UWORD *addr, int dx, int leftpart, UWORD rightmask, UWORD leftmask, int patind)
{
    UWORD *adr;
    UWORD pattern;
    WORD *color;
    int pixels;                   /* counting down the rest of dx */
    int bw;
    int planes;
    int plane;
    int patadd;               /* advance for multiplane patterns */

    /* init start values */
    planes = v_planes;
    patadd = multifill ? 16 : 0;     /* multi plane pattern offset */
    color = &FG_BP_1;

    for (plane = v_planes-1; plane >= 0; plane-- ) {

        /* load values fresh for this bitplane */
        adr = addr;
        pixels = dx-16;
        pattern = patptr[patind];

        if (*color++) {
            /* check, if the line is completely contained within one WORD */
            if (pixels+leftpart < 0) {
                UWORD help,bits;

                /* Isolate the necessary pixels */
                bits = *adr;            /* get data from screen address */
                help = bits;
                bits |= pattern;        /* and complement of mask with source */
                help ^= bits;           /* isolate changed bits */
                help &= ~(leftmask | rightmask);        /* isolate bits */
                bits ^= help;           /* restore them to original states */
                *adr = bits;            /* write back the result */
            } else {
                UWORD help,bits;

                /* Draw the left fringe */
                if (leftmask) {
                    bits = *adr;                /* get data from screen address */
                    help = bits;
                    bits |= pattern;    /* and complement of mask with source */
                    help ^= bits;               /* isolate changed bits */
                    help &= ~leftmask;    /* isolate changed bits outside of fringe */
                    bits ^= help;               /* restore them to original states */
                    *adr = bits;        /* write back the result */

                    adr += planes;;
                    pixels -= 16;
                    pixels += leftpart;
                }
                /* Full bytes */
                for (bw = pixels >> 4;bw>=0;bw--) {
                    *adr |= pattern;
                    adr += planes;
                }
                /* Draw the right fringe */
                if (~rightmask) {
                    bits = *adr;                /* get data from screen address */
                    help = bits;
                    bits |= pattern;    /* and complement of mask with source */
                    help ^= bits;               /* isolate changed bits */
                    help &= ~rightmask;   /* isolate changed bits outside of fringe */
                    bits ^= help;               /* restore them to original states */
                    *adr = bits;        /* write back the result */
                }
            }
        } else {
            pattern = ~pattern;
            /* check, if the line is completely contained within one WORD */
            if (pixels+leftpart < 0) {
                UWORD help,bits;

                /* Isolate the necessary pixels */
                bits = *adr;            /* get data from screen address */
                help = bits;
                bits &= pattern;        /* and complement of mask with source */
                help ^= bits;           /* isolate changed bits */
                help &= leftmask | rightmask;   /* isolate bits */
                bits ^= help;           /* restore them to original states */
                *adr = bits;            /* write back the result */
            } else {
                UWORD help,bits;
                /* Draw the left fringe */
                if (leftmask) {
                    bits = *adr;        /* get data from screen address */
                    help = bits;
                    bits &= pattern;  /* and complement of mask with source */
                    help ^= bits;       /* isolate changed bits */
                    help &= leftmask; /* isolate changed bits outside of fringe */
                    bits ^= help;       /* restore them to original states */
                    *adr = bits;      /* write back the result */

                    adr += planes;;
                    pixels -= 16;
                    pixels += leftpart;
                }
                /* Full bytes */
                for (bw = pixels >> 4;bw>=0;bw--) {
                    *adr &= pattern;
                    adr += planes;
                }
                /* Draw the right fringe */
                if (~rightmask) {
                    bits = *adr;        /* get data from screen address */
                    help = bits;
                    bits &= pattern;  /* and complement of mask with source */
                    help ^= bits;       /* isolate changed bits */
                    help &= rightmask;/* isolate changed bits outside of fringe */
                    bits ^= help;       /* restore them to original states */
                    *adr = bits;      /* write back the result */
                }
            }
            pattern = ~pattern;
        }
        addr++; /* advance one WORD to next plane */
        patind += patadd;
    }
}



/*
 * hzline_rep - draw a horizontal line in xor mode
 *
 * This routine is used by habline() and rectfill()
 */

static
void hzline_xor(UWORD *addr, int dx, int leftpart, UWORD rightmask, UWORD leftmask, int patind)
{
    UWORD *adr;
    UWORD pattern;
    int pixels;                   /* counting down the rest of dx */
    int bw;
    int planes;
    int plane;
    int patadd;               /* advance for multiplane patterns */

    /* init adress counter */
    planes = v_planes;
    patadd = multifill ? 16 : 0;     /* multi plane pattern offset */

    for (plane = v_planes-1; plane >= 0; plane-- ) {

        /* load values fresh for this bitplane */
        pattern = patptr[patind];
        adr = addr;
        pixels = dx-16;

        /* check, if the line is completely contained within one WORD */
        if (pixels+leftpart < 0) {
            UWORD help,bits;

            /* Isolate the necessary pixels */
            bits = *adr;        /* get data from screen address */
            help = bits;
            bits ^= pattern;  /* xor the pattern with the source */
            help ^= bits;       /* xor result with source - now have pattern */
            help &= leftmask | rightmask;       /* isolate bits */
            bits ^= help;       /* restore states of bits outside of fringe */
            *adr = bits;      /* write back the result */
        } else {
            UWORD help,bits;
            /* Draw the left fringe */
            if (leftmask) {
                bits = *adr;    /* get data from screen address */
                help = bits;
                bits ^= pattern;        /* xor the pattern with the source */
                help ^= bits;           /* xor result with source - now have pattern */
                help &= leftmask;     /* isolate changed bits outside of fringe */
                bits ^= help;           /* restore states of bits outside of fringe */
                *adr = bits;            /* write back the result */

                adr += planes;;
                pixels -= 16;
                pixels += leftpart;
            }
            /* Full bytes */
            for (bw = pixels >> 4;bw>=0;bw--) {
                *adr ^= pattern;         /* write back the result */
                adr += planes;
            }
            /* Draw the right fringe */
            if (~rightmask) {
                    bits = *adr;        /* get data from screen address */
                    help = bits;
                    bits ^= pattern;    /* xor the pattern with the source */
                    help ^= bits;               /* xor result with source - now have pattern */
                    help &= rightmask;    /* isolate changed bits outside of fringe */
                    bits ^= help;               /* restore states of bits outside of fringe */
                    *adr = bits;                /* write back the result */
            }
        }
        addr++; /* advance one WORD to next plane */
        patind += patadd;
    }
}



/*
 * hzline_or - draw a horizontal line in transparent mode
 *
 * If the foreground color requires that bits in the current plane be set
 * then 'or' the mask with the source.  Otherwise, the foreground color
 * requires that bits in the current plane be cleared so 'and' the complement
 * of the mask with the source.  Bits that would be drawn with the background
 * color (black) are left unchanged.
 *
 * This routine is used by habline() and rectfill()
 */

static
void hzline_nor(UWORD *addr, int dx, int leftpart, UWORD rightmask, UWORD leftmask, int patind)
{
    UWORD *adr;
    UWORD pattern;
    WORD *color;
    int pixels;                   /* counting down the rest of dx */
    int bw;
    int planes;
    int plane;
    int patadd;               /* advance for multiplane patterns */

    /* init adress counter */
    planes = v_planes;
    patadd = multifill ? 16 : 0;     /* multi plane pattern offset */
    color = &FG_BP_1;

    for (plane = v_planes-1; plane >= 0; plane-- ) {

        /* load values fresh for this bitplane */

        adr = addr;
        pixels = dx-16;
        pattern = patptr[patind];

        if (*color++) {
            pattern = ~pattern;
            /* check, if the line is completely contained within one WORD */
            if (pixels+leftpart < 0) {
                UWORD mask;
                UWORD help,bits;

                /* Isolate the necessary pixels */
                mask  = leftmask | rightmask;
                bits = *adr;            /* get data from screen address */
                help = bits;
                bits |= pattern;        /* and complement of mask with source */
                help ^= bits;           /* isolate changed bits */
                help &= ~mask;          /* isolate changed bits outside of fringe */
                bits ^= help;           /* restore them to original states */
                *adr = bits;            /* write back the result */
            } else {
                UWORD help,bits;

                /* Draw the left fringe */
                if (leftmask) {
                    bits = *adr;                /* get data from screen address */
                    help = bits;
                    bits |= pattern;    /* and complement of mask with source */
                    help ^= bits;               /* isolate changed bits */
                    help &= ~leftmask;    /* isolate changed bits outside of fringe */
                    bits ^= help;               /* restore them to original states */
                    *adr = bits;        /* write back the result */

                    adr += planes;;
                    pixels -= 16;
                    pixels += leftpart;
                }
                /* Full bytes */
                for (bw = pixels >> 4;bw>=0;bw--) {
                    *adr |= pattern;
                    adr += planes;
                }
                /* Draw the right fringe */
                if (~rightmask) {
                    bits = *adr;                /* get data from screen address */
                    help = bits;
                    bits |= pattern;    /* and complement of mask with source */
                    help ^= bits;               /* isolate changed bits */
                    help &= ~rightmask;   /* isolate changed bits outside of fringe */
                    bits ^= help;               /* restore them to original states */
                    *adr = bits;        /* write back the result */
                }
            }
            pattern = ~pattern;
        } else {
            /* check, if the line is completely contained within one WORD */
            if ((leftpart+pixels) < 16 ) {
                UWORD mask;
                UWORD help,bits;
                /* Isolate the necessary pixels */
                mask  = leftmask | rightmask;

                bits = *adr;            /* get data from screen address */
                help = bits;
                bits &= pattern;        /* and complement of mask with source */
                help ^= bits;           /* isolate changed bits */
                help &= mask;           /* isolate changed bits outside of fringe */
                bits ^= help;           /* restore them to original states */
                *adr = bits;            /* write back the result */
            } else {
                UWORD help,bits;
                /* Draw the left fringe */
                if (leftmask) {
                    bits = *adr;        /* get data from screen address */
                    help = bits;
                    bits &= pattern;  /* and complement of mask with source */
                    help ^= bits;       /* isolate changed bits */
                    help &= leftmask; /* isolate changed bits outside of fringe */
                    bits ^= help;       /* restore them to original states */
                    *adr = bits;      /* write back the result */

                    adr += planes;;
                    pixels -= 16-leftpart;
                }
                /* Full bytes */
                for (bw = pixels >> 4;bw>0;bw--) {
                    *adr &= pattern;
                    adr += planes;
                }
                /* Draw the right fringe */
                if (~rightmask) {
                    bits = *adr;        /* get data from screen address */
                help = bits;
                bits &= pattern;  /* and complement of mask with source */
                help ^= bits;       /* isolate changed bits */
                help &= rightmask;/* isolate changed bits outside of fringe */
                bits ^= help;       /* restore them to original states */
                *adr = bits;      /* write back the result */
                }
            }
        }
        addr++; /* advance one WORD to next plane */
        patind += patadd;
    }
}



/*
 * habline - draw a horizontal line
 *
 * This routine is just a wrapper for horzline.
 */

void habline() {
    horzline(X1, X2, Y1);
}



/*
 * horzline - draw a horizontal line
 *
 * This routine draws a line between (x1,y) and (x2,y) using a left fringe,
 * inner loop, right fringe bitblt algorithm.  The line is modified by the
 * pattern and WRT_MODE variables.
 * This routine handles all 3 interleaved bitplanes video resolutions.
 *
 * input:
 *     x1, x2, y = coordinates.
 *     v_planes  = number of video planes. (resolution)
 *     patmsk    = index into pattern.
 *     patptr    = ptr to pattern.
 *     WRT_MODE  = writing mode:
 *                     0 => replace mode.
 *                     1 => or mode.
 *                     2 => xor mode.
 *                     3 => not mode.
 */

void horzline(WORD x1, WORD x2, WORD y) {
    WORD x;
    UWORD leftmask;
    UWORD rightmask;
    void *addr;
    int dx;
    int patind;               /* index into pattern table */
    int patadd;               /* advance for multiplane patterns */
    int leftpart;
    int rightpart;

    if (x2 > x1) {
        dx = x2 - x1;             /* width of line */
        x = x1;
    } else {
        dx = x1 - x2;            /* width of line */
        x = x2;
    }

    /* Get the pattern with which the line is to be drawn. */
    patind = y&patmsk;               /* which pattern to start with */
    patadd = multifill ? 16 : 0;     /* multi plane pattern offset */

    /* init adress counter */
    addr = v_bas_ad;                    /* start of screen */
    addr += (x1&0xfff0)>>shft_off;      /* add x coordinate part of addr */
    addr += (LONG)y * v_lin_wr;         /* add y coordinate part of addr */

    /* precalculate, what to draw */
    leftpart = x&0xf;
    rightpart = (x+dx)&0xf;
    leftmask = ~(0xffff>>leftpart);     /* origin for not left fringe lookup */
    rightmask = 0x7fff>>rightpart;      /* origin for right fringe lookup */

    switch (WRT_MODE) {
    case 3:  /* nor */
        hzline_nor(addr, dx, leftpart, rightmask, leftmask, patind);
        break;
    case 2:  /* xor */
        hzline_xor(addr, dx, leftpart, rightpart, leftmask, patind);
        break;
    case 1:  /* or */
        hzline_or(addr, dx, leftpart, rightpart, leftmask, patind);
        break;
    default: /* rep */
        hzline_rep(addr, dx, leftpart, rightmask, leftmask, patind);
    }
}



/*
 * put_pix - plot a pixel, just for linea
 *
 * input:
 *     INTIN(0) = pixel value.
 *     PTSIN(0) = x coordinate.
 *     PTSIN(1) = y coordinate.
 */

void put_pix()
{
    UWORD *addr;
    WORD x,y;
    UWORD color;
    UWORD bit;
    int plane;

    x = PTSIN[0];       /* fetch x coord. */
    y = PTSIN[1];       /* fetch y coord. */
    color = INTIN[0];   /* device dependent encoded color bits */

    /* set adress and bit in the WORD for drawing */
    addr = (UWORD*)(v_bas_ad + (LONG)y * v_lin_wr + ((x&0xfff0)>>shft_off));
    bit = 0x8000 >> (x&0xf);            /* initial bit position in WORD */

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        color = color >> 1| color << 15;        /* rotate color bits */
        if (color&0x8000)
            *addr++ |= bit;
        else
            *addr++ &= ~bit;
    }
}



/*
 * get_pix - gets a pixel, just for linea!
 *
 * input:
 *     PTSIN(0) = x coordinate.
 *     PTSIN(1) = y coordinate.
 * output:
 *     pixel value
 */

WORD get_pix()
{
    UWORD *addr;
    WORD x,y;
    UWORD color;
    UWORD bit;
    int plane;

    x = PTSIN[0];       /* fetch x coord. */
    y = PTSIN[1];       /* fetch y coord. */
    color = 0;          /* start with empty color value */

    /* set adress and bit in the WORD for drawing */
    addr = (UWORD*)(v_bas_ad + (LONG)y * v_lin_wr + ((x&0xfff0)>>shft_off));
    bit = 0x8000 >> (x&0xf);            /* initial bit position in WORD */

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        if (*addr++ & bit)              /* mask out the questioned bit */
            color |= 1;                 /* the bit was set in this plane */
        color = color << 1;             /* next color bit */
    }
    return color;                       /* return the composed color value */
}


/*
 * clipbox - Just clips and copies the inputs for use by "rectfill"
 *
 * input:
 *     X1       = x coord of upper left corner.
 *     Y1       = y coord of upper left corner.
 *     X2       = x coord of lower right corner.
 *     Y2       = y coord of lower right corner.
 *     CLIP     = clipping flag. (0 => no clipping.
 *     XMN_CLIP = x clipping minimum.
 *     XMX_CLIP = x clipping maximum.
 *     YMN_CLIP = y clipping minimum.
 *     YMX_CLIP = y clipping maximum.
 *
 * output:
 *     X1 = x coord of upper left corner.
 *     Y1 = y coord of upper left corner.
 *     X2 = x coord of lower right corner.
 *     Y2 = y coord of lower right corner.
 */

static
WORD clipbox()
{
    WORD x1, y1, x2, y2;

    x1 = X1;
    y1 = Y1;
    x2 = X2;
    y2 = Y2;

    /* clip x coordinates */
    if ( x1 < XMN_CLIP) {
        if (x2 < XMX_CLIP) {
            return(FALSE);             /* clipped box is null */
        }
        X1 = XMN_CLIP;
    }
    if ( x2 > XMX_CLIP) {
        if (x1 > XMN_CLIP) {
            return(FALSE);             /* clipped box is null */
        }
        X2 = XMX_CLIP;
    }
    /* clip y coordinates */
    if ( y1 < YMN_CLIP) {
        if (y2 < YMX_CLIP) {
            return(FALSE);             /* clipped box is null */
        }
        Y1 = YMN_CLIP;
    }
    if ( y2 > YMX_CLIP) {
        if (y1 > YMN_CLIP) {
            return(FALSE);             /* clipped box is null */
        }
        Y2 = YMX_CLIP;
    }
    return (TRUE);
}



/*
 * rectfill - fills a rectangular area of the screen with a pattern
 *            using a "bitblt" algorithm similar to "_HABLINE"'s.
 *
 * input:
 *     X1       = x coord of upper left corner.
 *     Y1       = y coord of upper left corner.
 *     X2       = x coord of lower right corner.
 *     Y2       = y coord of lower right corner.
 *     CLIP     = clipping flag. (0 => no clipping.
 *     XMN_CLIP = x clipping minimum.
 *     XMX_CLIP = x clipping maximum.
 *     YMN_CLIP = y clipping minimum.
 *     YMX_CLIP = y clipping maximum.
 *
 * output:
 *     X1 = x coord of upper left corner.
 *     Y1 = y coord of upper left corner.
 *     X2 = x coord of lower right corner.
 *     Y2 = y coord of lower right corner.
 */

void rectfill ()
{
    WORD x1, y1, x2, y2;
    UWORD leftmask;
    UWORD rightmask;
    void *addr;
    int dx, dy;
    int yinc;
    int leftpart;
    int rightpart;

    if (CLIP)
        if (!clipbox())
            return;

    x1 = X1;
    y1 = Y1;
    x2 = X2;
    y2 = Y2;

    /* sort x coordinates */
    if (x2 < x1) {
       x1 = X2;
       x2 = X1;
    }
    dx = x2 - x1;             /* width of line */

    /* sort y coordinates */
    if (y2 < y1) {
        y1 = Y2;
        y2 = Y1;
    }
    dy = y2 - y1;             /* width of line */

    /* precalculate masks for left and right fringe */
    leftpart = x1&0xf;
    rightpart = (x1+dx)&0xf;
    leftmask = ~(0xffff>>leftpart);     /* origin for not left fringe lookup */
    rightmask = 0x7fff>>rightpart;      /* origin for right fringe lookup */

    /* init adress counter */
    addr = v_bas_ad;                    /* start of screen */
    addr += (x1&0xfff0)>>shft_off;      /* add x coordinate part of addr */
    addr += (LONG)y1 * v_lin_wr;                /* add y coordinate part of addr */
    yinc = v_lin_wr;                    /* y coordinate increase */

    switch (WRT_MODE) {
    case 3:  /* nor */
        for (y1 = dy; y1 >= 0; y1-- ) {
            hzline_nor(addr, dx, leftpart, rightmask, leftmask, y1&patmsk);
            addr += yinc;           /* next scanline */
        }
        break;
    case 2:  /* xor */
        for (y1 = dy; y1 >= 0; y1-- ) {
            hzline_xor(addr, dx, leftpart, rightmask, leftmask, y1&patmsk);
            addr += yinc;           /* next scanline */
        }
        break;
    case 1:  /* or */
        for (y1 = dy; y1 >= 0; y1-- ) {
            hzline_or(addr, dx, leftpart, rightmask, leftmask, y1&patmsk);
            addr += yinc;           /* next scanline */
        }
        break;
    default: /* rep */
        for (y1 = dy; y1 >= 0; y1-- ) {
            hzline_rep(addr, dx, leftpart, rightmask, leftmask, y1&patmsk);
            addr += yinc;           /* next scanline */
        }
    }
}
