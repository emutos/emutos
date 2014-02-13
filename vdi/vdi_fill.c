/*
 *
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2013 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "vdi_defs.h"
#include "tosvars.h"
#include "lineavars.h"

#define EMPTY   0xffff
#define DOWN_FLAG 0x8000
#define QSIZE 200
#define QMAX QSIZE-1



#define ABS(v) (v & 0x7FFF)



/* prototypes */
static void crunch_queue(void);
static BOOL clipbox(Vwk * vwk, Rect * rect);



/* Global variables */
static UWORD search_color;       /* the color of the border      */


/* some kind of stack for the segments to fill */
static WORD queue[QSIZE];       /* storage for the seed points  */
static WORD qbottom;            /* the bottom of the queue (zero)   */
static WORD qtop;               /* points top seed +3           */
static WORD qptr;               /* points to the active point   */
static WORD qtmp;
static WORD qhole;              /* an empty space in the queue */


/* the storage for the used defined fill pattern */
const UWORD ROM_UD_PATRN[16] = {
    0x07E0, 0x0FF0, 0x1FD8, 0x1808, 0x1808, 0x1008, 0x1E78, 0x1348,
    0x1108, 0x0810, 0x0B70, 0x0650, 0x07A0, 0x1E20, 0x1BC0, 0x1800
};

static const UWORD OEMMSKPAT = 7;
static const UWORD OEMPAT[128] = {
    /* Brick */
    0xFFFF, 0x8080, 0x8080, 0x8080, 0xFFFF, 0x0808, 0x0808, 0x0808,
    /* Diagonal Bricks */
    0x2020, 0x4040, 0x8080, 0x4141, 0x2222, 0x1414, 0x0808, 0x1010,
    /* Grass */
    0x0000, 0x0000, 0x1010, 0x2828, 0x0000, 0x0000, 0x0101, 0x8282,
    /* Trees */
    0x0202, 0x0202, 0xAAAA, 0x5050, 0x2020, 0x2020, 0xAAAA, 0x0505,
    /* Dashed x's */
    0x4040, 0x8080, 0x0000, 0x0808, 0x0404, 0x0202, 0x0000, 0x2020,
    /* Cobble Stones */
    0x6606, 0xC6C6, 0xD8D8, 0x1818, 0x8181, 0x8DB1, 0x0C33, 0x6000,
    /* Sand */
    0x0000, 0x0000, 0x0400, 0x0000, 0x0010, 0x0000, 0x8000, 0x0000,
    /* Rough Weave */
    0xF8F8, 0x6C6C, 0xC6C6, 0x8F8F, 0x1F1F, 0x3636, 0x6363, 0xF1F1,
    /* Quilt */
    0xAAAA, 0x0000, 0x8888, 0x1414, 0x2222, 0x4141, 0x8888, 0x0000,
    /* Paterned Cross */
    0x0808, 0x0000, 0xAAAA, 0x0000, 0x0808, 0x0000, 0x8888, 0x0000,
    /* Balls */
    0x7777, 0x9898, 0xF8F8, 0xF8F8, 0x7777, 0x8989, 0x8F8F, 0x8F8F,
    /* Verticle Scales */
    0x8080, 0x8080, 0x4141, 0x3E3E, 0x0808, 0x0808, 0x1414, 0xE3E3,
    /* Diagonal scales */
    0x8181, 0x4242, 0x2424, 0x1818, 0x0606, 0x0101, 0x8080, 0x8080,
    /* Checker Board */
    0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0, 0x0F0F, 0x0F0F, 0x0F0F, 0x0F0F,
    /* Filled Diamond */
    0x0808, 0x1C1C, 0x3E3E, 0x7F7F, 0xFFFF, 0x7F7F, 0x3E3E, 0x1C1C,
    /* Herringbone */
    0x1111, 0x2222, 0x4444, 0xFFFF, 0x8888, 0x4444, 0x2222, 0xFFFF
};

static const UWORD DITHRMSK = 3;              /* mask off all but four scans */
static const UWORD DITHER[32] = {
    0x0000, 0x4444, 0x0000, 0x1111,     /* intensity level 2 */
    0x0000, 0x5555, 0x0000, 0x5555,     /* intensity level 4 */
    0x8888, 0x5555, 0x2222, 0x5555,     /* intensity level 6 */
    0xAAAA, 0x5555, 0xAAAA, 0x5555,     /* intensity level 8 */
    0xAAAA, 0xDDDD, 0xAAAA, 0x7777,     /* intensity level 10 */
    0xAAAA, 0xFFFF, 0xAAAA, 0xFFFF,     /* intensity level 12 */
    0xEEEE, 0xFFFF, 0xBBBB, 0xFFFF,     /* intensity level 14 */
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF      /* intensity level 16 */
};

static const UWORD HAT_0_MSK = 7;
static const UWORD HATCH0[48] = {
    /* narrow spaced + 45 */
    0x0101, 0x0202, 0x0404, 0x0808, 0x1010, 0x2020, 0x4040, 0x8080,
    /* medium spaced thick 45 deg */
    0x6060, 0xC0C0, 0x8181, 0x0303, 0x0606, 0x0C0C, 0x1818, 0x3030,
    /* medium +-45 deg */
    0x4242, 0x8181, 0x8181, 0x4242, 0x2424, 0x1818, 0x1818, 0x2424,
    /* medium spaced vertical */
    0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080,
    /* medium spaced horizontal */
    0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* medium spaced cross */
    0xFFFF, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080
};

static const UWORD HAT_1_MSK = 0xF;
static const UWORD HATCH1[96] = {
    /* wide +45 deg */
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
    /* widely spaced thick 45 deg */
    0x8003, 0x0007, 0x000E, 0x001C, 0x0038, 0x0070, 0x00E0, 0x01C0,
    0x0380, 0x0700, 0x0E00, 0x1C00, 0x3800, 0x7000, 0x0E000, 0x0C001,
    /* widely +- 45 deg */
    0x8001, 0x4002, 0x2004, 0x1008, 0x0810, 0x0420, 0x0240, 0x0180,
    0x0180, 0x0240, 0x0420, 0x0810, 0x1008, 0x2004, 0x4002, 0x8001,
    /* widely spaced vertical */
    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
    /* widely spaced horizontal */
    0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* widely spaced horizontal/vert cross */
    0xFFFF, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080,
    0xFFFF, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080,
};

const UWORD HOLLOW = 0;
const UWORD SOLID = 0xFFFF;



/*
 * dsf_udpat - Update pattern
 */

void
dsf_udpat(Vwk * vwk)
{
    WORD *sp, *dp, i, count;

    count = CONTRL[3];

    if (count == 16)
        vwk->multifill = 0;        /* Single Plane Pattern */
    else if (count == (INQ_TAB[4] * 16))
        vwk->multifill = 1;        /* Valid Multi-plane pattern */
    else
        return;             /* Invalid pattern, return */

    sp = INTIN;
    dp = &vwk->ud_patrn[0];
    for (i = 0; i < count; i++)
        *dp++ = *sp++;
}



/*
 * _vsf_interior - Set fill style
 */

void
_vsf_interior(Vwk * vwk)
{
    WORD fs;

    CONTRL[4] = 1;
    fs = *INTIN;
    if ((fs > MX_FIL_STYLE) || (fs < 0))
        fs = 0;
    *INTOUT = vwk->fill_style = fs;
    st_fl_ptr(vwk);
}



/* S_FILL_INDEX: */
void
_vsf_style(Vwk * vwk)
{
    WORD fi;

    CONTRL[4] = 1;
    fi = *INTIN;

    if (vwk->fill_style == 2) {
        if ((fi > MX_FIL_PAT_INDEX) || (fi < 1))
            fi = 1;
    } else {
        if ((fi > MX_FIL_HAT_INDEX) || (fi < 1))
            fi = 1;
    }
    vwk->fill_index = (*INTOUT = fi) - 1;
    st_fl_ptr(vwk);
}



/* S_FILL_COLOR: */
void
_vsf_color(Vwk * vwk)
{
    WORD fc;

    *(CONTRL + 4) = 1;
    fc = *INTIN;
    if ((fc >= DEV_TAB[13]) || (fc < 0))
        fc = 1;

    *INTOUT = fc;
    vwk->fill_color = MAP_COL[fc];
}



/* ST_FILLPERIMETER: */
void
_vsf_perimeter(Vwk * vwk)
{
    WORD *int_out;

    int_out = INTOUT;

    if (*INTIN == 0) {
        *int_out = 0;
        vwk->fill_per = FALSE;
    } else {
        *(int_out) = 1;
        vwk->fill_per = TRUE;
    }
    CONTRL[4] = 1;
}


/*
 * dr_recfl - draw filled rectangle
 */

void
dr_recfl(Vwk * vwk)
{
    Rect * rect = (Rect*)PTSIN;

    if (vwk->clip)
        if (!clipbox(vwk, rect))
            return;

    /* do the real work... */
    draw_rect(vwk, rect, vwk->fill_color);
}



/*
 * _v_cellarray - Draw a square of sqares (just color devices)
 */
void
_v_cellarray(Vwk * vwk)
{
    /* not implemented */
}



/*
 * _vq_cellarray -
 */
void
_vq_cellarray(Vwk * vwk)
{
    /* not implemented */
}



/*
 * vql_attr - Inquire current fill area attributes
 */

void
vqf_attr(Vwk * vwk)
{
    WORD *pointer;

    pointer = INTOUT;
    *pointer++ = vwk->fill_style;
    *pointer++ = REV_MAP_COL[vwk->fill_color];
    *pointer++ = vwk->fill_index + 1;
    *pointer++ = vwk->wrt_mode + 1;
    *pointer = vwk->fill_per;

    CONTRL[4] = 5;
}



/*
 * st_fl_ptr - set fill pattern?
 */

void
st_fl_ptr(Vwk * vwk)
{
    WORD fi, pm;
    const UWORD *pp = NULL;

    fi = vwk->fill_index;
    pm = 0;
    switch (vwk->fill_style) {
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
        pp = (UWORD *)&vwk->ud_patrn[0];
        break;
    }
    vwk->patptr = (UWORD *)pp;
    vwk->patmsk = pm;
}



/*
 * bub_sort - sorts an array of words
 *
 * This routine bubble-sorts an array of words into ascending order.
 *
 * input:
 *     buf   - ptr to start of array.
 *     count - number of words in array.
 */

static void
bub_sort (WORD * buf, WORD count)
{
    int i, j;

    for (i = count-1; i > 0; i--) {
        WORD * ptr = buf;               /* reset pointer to the array */
        for (j = 0; j < i; j++) {
            WORD val = *ptr++;   /* word */    /* get next value */
            if ( val > *ptr ) {    /* yes - do nothing */
                *(ptr-1) = *ptr;   /* word */    /* nope - swap them */
                *ptr = val;   /* word */
            }
        }
    }
}



/*
 * clc_flit - draw a filled polygon
 *
 * (Sutherland and Hodgman Polygon Clipping Algorithm)
 *
 * For each non-horizontal scanline crossing poly, do:
 *   - find intersection points of scan line with poly edges.
 *   - Sort intersections left to right
 *   - Draw pixels between each pair of points (x coords) on the scan line
 */
/*
 * the buffer used by clc_flit() has been temporarily moved from the
 * stack to a local static area.  this avoids some cases of stack
 * overflow when the VDI is called from the AES (and the stack is the
 * small one located in the UDA).  this fix allows GemAmigo to run.
 *
 * this change restores the situation that existed in the original
 * DRI code, when clc_flit() was written in assembler; the buffer
 * was moved to the stack when clc_flit() was re-implemented in C.
 */
#define MAX_INTERSECTIONS   256
static WORD fill_buffer[MAX_INTERSECTIONS];

void
clc_flit (const VwkAttrib * attr, const VwkClip * clipper, const Point * point, WORD y, int vectors)
{
//    WORD fill_buffer[256];      /* must be 256 words or it will fail */
    WORD * bufptr;              /* point to array of x-values. */
    int intersections;          /* count of intersections */
    int i;

    /* Initialize the pointers and counters. */
    intersections = 0;  /* reset counter */
    bufptr = fill_buffer;

    /* find intersection points of scan line with poly edges. */
    for (i = vectors - 1; i >= 0; i--) {
        WORD x1, x2, y1, y2, dy;

        x1 = point->x;          /* fetch x-value of 1st endpoint. */
        y1 = point->y;          /* fetch y-value of 1st endpoint. */
        point++;
        x2 = point->x;          /* fetch x-value of 2nd endpoint. */
        y2 = point->y;          /* fetch y-value of 2nd endpoint. */

        /* if the current vector is horizontal, ignore it. */
        dy = y2 - y1;
        if ( dy ) {
            LONG dy1, dy2;

            /* fetch scan-line y. */
            dy1 = y - y1;       /* d4 - delta y1. */
            dy2 = y - y2;       /* d3 - delta y2. */

            /*
             * Determine whether the current vector intersects with the scan
             * line we wish to draw.  This test is performed by computing the
             * y-deltas of the two endpoints from the scan line.
             * If both deltas have the same sign, then the line does
             * not intersect and can be ignored.  The origin for this
             * test is found in Newman and Sproull.
             */
            if ((dy1 < 0) != (dy2 < 0)) {
                int dx = (x2 - x1) << 1;    /* so we can round by adding 1 below */
                if (++intersections > MAX_INTERSECTIONS)
                    break;
                /* fill edge buffer with x-values */
                if ( dx < 0 ) {
                    *bufptr++ = ((dy2 * dx / dy + 1) >> 1) + x2;
                }
                else {
                    *bufptr++ = ((dy1 * dx / dy + 1) >> 1) + x1;
                }
            }
        }
    }

    /*
     * All of the points of intersection have now been found.  If there
     * were none then there is nothing more to do.  Otherwise, sort the
     * list of points of intersection in ascending order.
     * (The list contains only the x-coordinates of the points.)
     */

    /* anything to do? */
    if (intersections == 0)
        return;

    /* bubblesort the intersections, if it makes sense */
    if ( intersections > 1 )
        bub_sort(fill_buffer, intersections);

    if (attr->clip) {
        /* Clipping is in force.  Once the endpoints of the line segment have */
        /* been adjusted for the border, clip them to the left and right sides */
        /* of the clipping rectangle. */

        /* The x-coordinates of each line segment are adjusted so that the */
        /* border of the figure will not be drawn with the fill pattern. */

        /* loop through buffered points */
        WORD * ptr = fill_buffer;
        for (i = intersections / 2 - 1; i >= 0; i--) {
            WORD x1, x2;
            Rect rect;

            /* grab a pair of adjusted intersections */
            x1 = *ptr++ + 1;
            x2 = *ptr++ - 1;

            /* do nothing, if starting point greater than ending point */
            if ( x1 > x2 )
                continue;

            if ( x1 < clipper->xmn_clip ) {
                if ( x2 < clipper->xmn_clip )
                    continue;           /* entire segment clipped left */
                x1 = clipper->xmn_clip; /* clip left end of line */
            }

            if ( x2 > clipper->xmx_clip ) {
                if ( x1 > clipper->xmx_clip )
                    continue;           /* entire segment clippped */
                x2 = clipper->xmx_clip; /* clip right end of line */
            }
            rect.x1 = x1;
            rect.y1 = y;
            rect.x2 = x2;
            rect.y2 = y;

            /* rectangle fill routine draws horizontal line */
            draw_rect_common(attr, &rect);
        }
    }
    else {
        /* Clipping is not in force.  Draw from point to point. */

        /* This code has been modified from the version in the screen driver. */
        /* The x-coordinates of each line segment are adjusted so that the */
        /* border of the figure will not be drawn with the fill pattern.  If */
        /* the starting point is greater than the ending point then nothing is */
        /* done. */

        /* loop through buffered points */
        WORD * ptr = fill_buffer;
        for (i = intersections / 2 - 1; i >= 0; i--) {
            WORD x1, x2;
            Rect rect;

            /* grab a pair of adjusted endpoints */
            x1 = *ptr++ + 1 ;   /* word */
            x2 = *ptr++ - 1 ;   /* word */

            /* If starting point greater than ending point, nothing is done. */            /* is start still to left of end? */
            if ( x1 <= x2 ) {
                rect.x1 = x1;
                rect.y1 = y;
                rect.x2 = x2;
                rect.y2 = y;

                /* rectangle fill routine draws horizontal line */
                draw_rect_common(attr, &rect);
            }
        }
    }
}


/*
 * polygon - draw a filled polygon
 */

void
polygon(Vwk * vwk, Point * ptsin, int count)
{
    WORD i, k, y;
    WORD fill_maxy, fill_miny;
    Point * point, * ptsget, * ptsput;
    VwkClip *clipper;
    VwkAttrib attr;

    LSTLIN = FALSE;

    /* find out the total min and max y values */
    point = ptsin;
    fill_maxy = fill_miny = point->y;
    for (i = count - 1; i > 0; i--) {
        point++;
        k = point->y;

        if (k < fill_miny)
            fill_miny = k;
        else
            if (k > fill_maxy)
                fill_maxy = k;
    }

    if (vwk->clip) {
        if (fill_miny < vwk->ymn_clip) {
            if (fill_maxy >= vwk->ymn_clip) {
                /* polygon starts before clip */
                fill_miny = vwk->ymn_clip - 1;       /* polygon partial overlap */
                if (fill_miny < 1)
                    fill_miny = 1;
            } else
                return;         /* polygon entirely before clip */
        }
        if (fill_maxy > vwk->ymx_clip) {
            if (fill_miny <= vwk->ymx_clip)  /* polygon ends after clip */
                fill_maxy = vwk->ymx_clip;   /* polygon partial overlap */
            else
                return;         /* polygon entirely after clip */
        }
    }

    /* close the polygon, connect last and first point */
    ptsget = ptsin;
    ptsput = ptsin + count;
    ptsput->x = ptsget->x;
    ptsput->y = ptsget->y;

    /* cast structure needed by clc_flit */
    clipper = VDI_CLIP(vwk);
    /* copy data needed by clc_flit -> draw_rect_common */
    Vwk2Attrib(vwk, &attr, vwk->fill_color);

    /* really draw it */
    for (y = fill_maxy; y > fill_miny; y--) {
        clc_flit(&attr, clipper, ptsin, y, count);
    }
    if (vwk->fill_per == TRUE) {
        LN_MASK = 0xffff;
        polyline(vwk, ptsin, count+1, vwk->fill_color);
    }
}



/*
 * _v_fillarea - Fill an area
 */

void
_v_fillarea(Vwk * vwk)
{
    Point * point = (Point*)PTSIN;
    int count = CONTRL[1];

#if 0
#if HAVE_BEZIER
    /* check, if we want to draw a filled bezier curve */
    if (CONTRL[5] == 13 && vwk->bez_qual )
        v_bez_fill(vwk, point, count);
    else
#endif
#endif
        polygon(vwk, point, count);
}



/*
 * clipbox - Just clips and copies the inputs for use by "rectfill"
 *
 * input:
 *     X1        = x coord of upper left corner.
 *     Y1        = y coord of upper left corner.
 *     X2        = x coord of lower right corner.
 *     Y2        = y coord of lower right corner.
 *     vwk->clip = clipping flag. (0 => no clipping.)
 *     vwk->xmn_clip = x clipping minimum.
 *     vwk->xmx_clip = x clipping maximum.
 *     vwk->ymn_clip = y clipping minimum.
 *     vwk->ymx_clip = y clipping maximum.
 *
 * output:
 *     X1 = x coord of upper left corner.
 *     Y1 = y coord of upper left corner.
 *     X2 = x coord of lower right corner.
 *     Y2 = y coord of lower right corner.
 */

static BOOL
clipbox(Vwk * vwk, Rect * rect)
{
    WORD x1, y1, x2, y2;

    x1 = rect->x1;
    y1 = rect->y1;
    x2 = rect->x2;
    y2 = rect->y2;

    /* clip x coordinates */
    if ( x1 < vwk->xmn_clip) {
        if (x2 < vwk->xmn_clip) {
            return(FALSE);             /* clipped box is null */
        }
        rect->x1 = vwk->xmn_clip;
    }
    if ( x2 > vwk->xmx_clip) {
        if (x1 > vwk->xmx_clip) {
            return(FALSE);             /* clipped box is null */
        }
        rect->x2 = vwk->xmx_clip;
    }
    /* clip y coordinates */
    if ( y1 < vwk->ymn_clip) {
        if (y2 < vwk->ymn_clip) {
            return(FALSE);             /* clipped box is null */
        }
        rect->y1 = vwk->ymn_clip;
    }
    if ( y2 > vwk->ymx_clip) {
        if (y1 > vwk->ymx_clip) {
            return(FALSE);             /* clipped box is null */
        }
        rect->y2 = vwk->ymx_clip;
    }
    return (TRUE);
}


/*
 * get_color - Get color value of requested pixel.
 */
static UWORD
get_color (UWORD mask, UWORD * addr)
{
    UWORD color = 0;                    /* clear the pixel value accumulator. */
    WORD plane = v_planes;

    while(1) {
        /* test the bit. */
        if ( *--addr & mask )
            color |= 1;         /* if 1, set color accumulator bit. */

        if ( --plane == 0 )
            break;

        color <<= 1;            /* shift accumulator for next bit_plane. */
    }

    return color;       /* this is the color we are searching for */
}

/*
 * pixelread - gets a pixel's color index value
 *
 * input:
 *     PTSIN(0) = x coordinate.
 *     PTSIN(1) = y coordinate.
 * output:
 *     pixel value
 */

static UWORD
pixelread(const WORD x, const WORD y)
{
    UWORD *addr;
    UWORD mask;

    /* convert x,y to start adress and bit mask */
    addr = get_start_addr(x, y);
    addr += v_planes;                   /* start at highest-order bit_plane */
    mask = 0x8000 >> (x&0xf);           /* initial bit position in WORD */

    return get_color(mask, addr);       /* return the composed color value */
}

static UWORD
search_to_right (Vwk * vwk, WORD x, UWORD mask, const UWORD search_col, UWORD * addr)
{
    /* is x coord < x resolution ? */
    while( x++ < vwk->xmx_clip ) {
        UWORD color;

        /* need to jump over interleaved bit_plane? */
        mask = mask >> 1 | mask << 15;  /* roll right */
        if ( mask & 0x8000 )
            addr += v_planes;

        /* search, while pixel color != search color */
        color = get_color(mask, addr);
        if ( search_col != color ) {
            break;
        }

    }

    return x - 1;       /* output x coord -1 to endxright. */
}

static UWORD
search_to_left (Vwk * vwk, WORD x, UWORD mask, const UWORD search_col, UWORD * addr)
{
    /* Now, search to the left. */
    while (x-- > vwk->xmn_clip) {
        UWORD color;

        /* need to jump over interleaved bit_plane? */
        mask = mask >> 15 | mask << 1;  /* roll left */
        if ( mask & 0x0001 )
            addr -= v_planes;

        /* search, while pixel color != search color */
        color = get_color(mask, addr);
        if ( search_col != color )
            break;

    }

    return x + 1;       /* output x coord + 1 to endxleft. */
}

/*
 * end_pts - find the endpoints of a section of solid color
 *
 *   (for the _seed_fill routine.)
 *
 * input:  4(sp) = xstart.
 *         6(sp) = ystart.
 *         8(sp) = ptr to endxleft.
 *         C(sp) = ptr to endxright.
 *
 * output: endxleft  := left endpoint of solid color.
 *         endxright := right endpoint of solid color.
 *         d0        := success flag.
 *             0 => no endpoints or xstart on edge.
 *             1 => endpoints found.
 *         seed_type  indicates the type of fill
 */

static WORD
end_pts(Vwk * vwk, WORD x, WORD y, WORD *xleftout, WORD *xrightout,
        BOOL seed_type)
{
    UWORD color;
    UWORD * addr;
    UWORD mask;

    /* see, if we are in the y clipping range */
    if ( y < vwk->ymn_clip || y > vwk->ymx_clip)
        return 0;

    /* convert x,y to start adress and bit mask */
    addr = get_start_addr(x, y);
    addr += v_planes;                   /* start at highest-order bit_plane */
    mask = 0x8000 >> (x & 0x000f);   /* fetch the pixel mask. */

    /* get search color and the left and right end */
    color = get_color (mask, addr);
    *xrightout = search_to_right (vwk, x, mask, color, addr);
    *xleftout = search_to_left (vwk, x, mask, color, addr);

    /* see, if the whole found segment is of search color? */
    if ( color != search_color ) {
        return seed_type ^ 1;   /* return segment not of search color */
    }
    return seed_type ^ 0;       /* return segment is of search color */
}

/* Prototypes local to this module */
static WORD
get_seed(Vwk * vwk, WORD xin, WORD yin, WORD *xleftout, WORD *xrightout,
             BOOL seed_type);


void
d_contourfill(Vwk * vwk)
{
    WORD newxleft;              /* ends of line at oldy +       */
    WORD newxright;             /* the current direction    */
    WORD oldxleft;              /* left end of line at oldy     */
    WORD oldxright;             /* right end                    */
    WORD oldy;                  /* the previous scan line       */
    WORD xleft;                 /* temporary endpoints          */
    WORD xright;                /* */
    WORD direction;             /* is next scan line up or down */
    BOOL notdone;               /* does seedpoint==search_color */
    BOOL gotseed;               /* a seed was put in the Q      */
    BOOL seed_type;             /* indicates the type of fill */

    xleft = PTSIN[0];
    oldy = PTSIN[1];

    if (xleft < vwk->xmn_clip || xleft > vwk->xmx_clip ||
        oldy < vwk->ymn_clip  || oldy > vwk->ymx_clip)
        return;

    search_color = INTIN[0];

    if ((WORD)search_color < 0) {
        search_color = pixelread(xleft,oldy);
        seed_type = 1;
    } else {
        const WORD plane_mask[] = { 1, 3, 7, 15 };

        /* Range check the color and convert the index to a pixel value */
        if (search_color >= DEV_TAB[13])
            return;

        /*
         * We mandate that white is all bits on.  Since this yields 15
         * in rom, we must limit it to how many planes there really are.
         * Anding with the mask is only necessary when the driver supports
         * move than one resolution.
         */
        search_color =
            (MAP_COL[search_color] & plane_mask[INQ_TAB[4] - 1]);
        seed_type = 0;
    }

    /* Initialize the line drawing parameters */
    LSTLIN = FALSE;

    notdone = end_pts(vwk, xleft, oldy, &oldxleft, &oldxright, seed_type);

    qptr = qbottom = 0;
    qtop = 3;                   /* one above highest seed point */
    queue[0] = (oldy | DOWN_FLAG);
    queue[1] = oldxleft;
    queue[2] = oldxright;           /* stuff a point going down into the Q */

    if (notdone) {
        /* couldn't get point out of Q or draw it */
        while (1) {
            Rect rect;

            direction = (oldy & DOWN_FLAG) ? 1 : -1;
            gotseed = get_seed(vwk, oldxleft, (oldy + direction),
                               &newxleft, &newxright, seed_type);

            if ((newxleft < (oldxleft - 1)) && gotseed) {
                xleft = oldxleft;
                while (xleft > newxleft) {
                    --xleft;
                    get_seed(vwk, xleft, oldy ^ DOWN_FLAG,
                             &xleft, &xright, seed_type);
                }
            }
            while (newxright < oldxright) {
                ++newxright;
                gotseed = get_seed(vwk, newxright, oldy + direction,
                                   &xleft, &newxright, seed_type);
            }
            if ((newxright > (oldxright + 1)) && gotseed) {
                xright = oldxright;
                while (xright < newxright) {
                    ++xright;
                    get_seed(vwk, xright, oldy ^ DOWN_FLAG,
                             &xleft, &xright, seed_type);
                }
            }

            /* Eventually jump out here */
            if (qtop == qbottom)
                break;

            while (queue[qptr] == EMPTY) {
                qptr += 3;
                if (qptr == qtop)
                    qptr = qbottom;
            }

            oldy = queue[qptr];
            queue[qptr++] = EMPTY;
            oldxleft = queue[qptr++];
            oldxright = queue[qptr++];
            if (qptr == qtop)
                crunch_queue();

            rect.x1 = oldxleft;
            rect.y1 = ABS(oldy);
            rect.x2 = oldxright;
            rect.y2 = ABS(oldy);

            /* rectangle fill routine draws horizontal line */
            draw_rect(vwk, &rect, vwk->fill_color);
        }
    }
}                               /* end of fill() */

/*
 * crunch_queue - move qtop down to remove unused seeds
 */
static void
crunch_queue(void)
{
    while ((queue[qtop - 3] == EMPTY) && (qtop > qbottom))
        qtop -= 3;
    if (qptr >= qtop)
        qptr = qbottom;
}

/*
 * get_seed - put seeds into Q, if (xin,yin) is not of search_color
 */
static WORD
get_seed(Vwk * vwk, WORD xin, WORD yin, WORD *xleftout, WORD *xrightout,
         BOOL seed_type)
{
    if (end_pts(vwk, xin, ABS(yin), xleftout, xrightout, seed_type)) {
        /* false if of search_color */
        for (qtmp = qbottom, qhole = EMPTY; qtmp < qtop; qtmp += 3) {
            /* see, if we ran into another seed */
            if ( ((queue[qtmp] ^ DOWN_FLAG) == yin) && (queue[qtmp] != EMPTY) &&
                (queue[qtmp + 1] == *xleftout) )

            {
                /* we ran into another seed so remove it and fill the line */
                Rect rect;

                rect.x1 = *xleftout;
                rect.y1 = ABS(yin);
                rect.x2 = *xrightout;
                rect.y2 = ABS(yin);

                /* rectangle fill routine draws horizontal line */
                draw_rect(vwk, &rect, vwk->fill_color);

                queue[qtmp] = EMPTY;
                if ((qtmp + 3) == qtop)
                    crunch_queue();
                return 0;
            }
            if ((queue[qtmp] == EMPTY) && (qhole == EMPTY))
                qhole = qtmp;
        }

        if (qhole == EMPTY) {
            if ((qtop += 3) > QMAX) {
                qtmp = qbottom;
                qtop -= 3;
            }
        } else
            qtmp = qhole;

        queue[qtmp++] = yin;    /* put the y and endpoints in the Q */
        queue[qtmp++] = *xleftout;
        queue[qtmp] = *xrightout;
        return 1;             /* we put a seed in the Q */
    }

    return 0;           /* we didnt put a seed in the Q */
}



void
_v_get_pixel(Vwk * vwk)
{
    WORD pel;
    WORD *int_out;
    const WORD x = PTSIN[0];       /* fetch x coord. */
    const WORD y = PTSIN[1];       /* fetch y coord. */

    /* Get the requested pixel */
    pel = (WORD)pixelread(x,y);

    int_out = INTOUT;
    *int_out++ = pel;

    *int_out = REV_MAP_COL[pel];
    CONTRL[4] = 2;
}



/*
 * get_pix - gets a pixel (just for linea!)
 *
 * input:
 *     PTSIN(0) = x coordinate.
 *     PTSIN(1) = y coordinate.
 * output:
 *     pixel value
 */
WORD
get_pix(void)
{
    /* return the composed color value */
    return pixelread(PTSIN[0], PTSIN[1]);
}

/*
 * put_pix - plot a pixel (just for linea!)
 *
 * input:
 *     INTIN(0) = pixel value.
 *     PTSIN(0) = x coordinate.
 *     PTSIN(1) = y coordinate.
 */
void
put_pix(void)
{
    UWORD *addr;
    UWORD color;
    UWORD mask;
    int plane;

    const WORD x = PTSIN[0];
    const WORD y = PTSIN[1];

    /* convert x,y to start adress */
    addr = get_start_addr(x, y);
    /* co-ordinates can wrap, but cannot write outside screen,
     * alternatively this could check against v_bas_ad+vram_size()
     */
    if (addr < (UWORD*)v_bas_ad || addr >= get_start_addr(v_hz_rez, v_vt_rez)) {
        return;
    }
    color = INTIN[0];           /* device dependent encoded color bits */
    mask = 0x8000 >> (x&0xf);   /* initial bit position in WORD */

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        color = color >> 1| color << 15;        /* rotate color bits */
        if (color&0x8000)
            *addr++ |= mask;
        else
            *addr++ &= ~mask;
    }
}
