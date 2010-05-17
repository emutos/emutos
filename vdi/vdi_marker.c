/*
 * vdi_marker.c - Marker
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "vdi_defs.h"



/* Marker definitions */
static WORD m_dot[] = { 1, 2, 0, 0, 0, 0 };
static WORD m_plus[] = { 2, 2, 0, -3, 0, 3, 2, -4, 0, 4, 0 };
static WORD m_star[] = { 3, 2, 0, -3, 0, 3, 2, 3, 2, -3, -2, 2, 3, -2, -3, 2};
static WORD m_square[] = { 1, 5, -4, -3, 4, -3, 4, 3, -4, 3, -4, -3 };
static WORD m_cross[] = { 2, 2, -4, -3, 4, 3, 2, -4, 3, 4, -3 };
static WORD m_dmnd[] = { 1, 5, -4, 0, 0, -3, 4, 0, 0, 3, -4, 0 };



/*
 * _vsm_height - Sets the height of markers
 */

void _vsm_height(Vwk * vwk)
{
    WORD h, *pts_out;

    /* Limit the requested marker height to a reasonable value. */
    h = PTSIN[1];
    if (h < DEF_MKHT)
        h = DEF_MKHT;

    else if (h > MAX_MKHT)
        h = MAX_MKHT;

    /* Set the marker height internals and the return parameters. */
    vwk->mark_height = h;
    h = (h + DEF_MKHT / 2) / DEF_MKHT;
    vwk->mark_scale = h;
    CONTRL[2] = 1;
    pts_out = PTSOUT;
    *pts_out++ = h * DEF_MKWD;
    *pts_out = h * DEF_MKHT;
    flip_y = 1;
} 



/*
 * _vsm_type - Sets the current type of marker
 */
void _vsm_type(Vwk * vwk)
{
    WORD i;

    i = INTIN[0] - 1;
    i = ((i >= MAX_MARK_INDEX) || (i < 0)) ? 2 : i;
    INTOUT[0] = (vwk->mark_index = i) + 1;
    CONTRL[4] = 1;
}



/*
 * _vsm_color - Set mark color
 */

void _vsm_color(Vwk * vwk)
{
    WORD i;

    i = INTIN[0];
    i = ((i >= DEV_TAB[13]) || (i < 0)) ? 1 : i;
    INTOUT[0] = i;
    vwk->mark_color = MAP_COL[i];
    CONTRL[4] = 1;
}



/*
 * _v_pmarker - Polymarker
 */

void _v_pmarker(Vwk * vwk)
{
/* If this constant goes greater than 5, you must increase size of sav_points */
#define MARKSEGMAX 5

    extern WORD m_dot[], m_plus[], m_star[];
    extern WORD m_square[], m_cross[], m_dmnd[];

    static WORD *markhead[] = {
        m_dot, m_plus, m_star, m_square, m_cross, m_dmnd
    };

    WORD i, j, num_lines, num_vert, x_center, y_center, sav_points[10];
    WORD sav_index, sav_color, sav_width, sav_beg, sav_end;
    WORD *mrk_ptr, *old_ptsin, scale, num_points, *src_ptr;
    WORD h, *pts_in, *m_ptr;

    /* Save the current polyline attributes which will be used. */
    sav_index = vwk->line_index;
    sav_color = vwk->line_color;
    sav_width = vwk->line_width;
    sav_beg = vwk->line_beg;
    sav_end = vwk->line_end;

    /* Set the appropriate polyline attributes. */
    vwk->line_index = 0;
    vwk->line_color = vwk->mark_color;
    vwk->line_width = 1;
    vwk->line_beg = 0;
    vwk->line_end = 0;
    vwk->clip = 1;

    scale = vwk->mark_scale;

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
        m_ptr = markhead[vwk->mark_index];
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
            _v_pline(vwk);
            m_ptr = mrk_ptr;
        }                       /* End for:  over the number of polylines
                                   defining the marker. */

    }                           /* End for:  over marker points. */

    /* Restore the PTSIN pointer */
    PTSIN = old_ptsin;

    /* Restore the current polyline attributes. */
    vwk->line_index = sav_index;
    vwk->line_color = sav_color;
    vwk->line_width = sav_width;
    vwk->line_beg = sav_beg;
    vwk->line_end = sav_end;
}



/*
 * vql_attr - Inquire current polymarker attributes
 */

void vqm_attr(Vwk * vwk)
{
    INTOUT[0] = vwk->mark_index;
    INTOUT[1] = REV_MAP_COL[vwk->mark_color];
    INTOUT[2] = vwk->wrt_mode + 1;

    PTSOUT[0] = 0;
    PTSOUT[1] = vwk->mark_height;

    CONTRL[4] = 3;
    CONTRL[2] = 1;
    flip_y = 1;
}



