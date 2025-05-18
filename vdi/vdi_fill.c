/*
 * vdi_fill.c - filled polygons
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2025 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "asm.h"
#include "intmath.h"
#include "aesext.h"
#include "vdi_defs.h"
#include "vdistub.h"
#include "tosvars.h"
#include "lineavars.h"
#include "vdi_inline.h"

extern Vwk phys_work;           /* attribute area for physical workstation */

#define OVERLAY_BIT 0x0020      /* for 16-bit resolutions */

/* special values used in y member of SEGMENT */
#define EMPTY       0xffff          /* this entry is unused */
#define DOWN_FLAG   0x8000
#define ABS(v)      ((v) & 0x7FFF)  /* strips DOWN_FLAG if present */

/* Global variables */
static UWORD search_color;      /* selected colour for contourfill() */
static BOOL seed_type;          /* 1 => fill until selected colour is NOT found */
                                /* 0 => fill until selected colour is found */

/* the following point to segments within vdishare.queue[] (see below) */
static SEGMENT *qbottom;        /* the bottom of the queue      */
static SEGMENT *qtop;           /* the last segment in use +1   */
static SEGMENT *qptr;           /* points to the active point   */

/*
 * a shared area for the VDI
 */
VDISHARE vdishare;


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
    /* Patterned Cross */
    0x0808, 0x0000, 0xAAAA, 0x0000, 0x0808, 0x0000, 0x8888, 0x0000,
    /* Balls */
    0x7777, 0x9898, 0xF8F8, 0xF8F8, 0x7777, 0x8989, 0x8F8F, 0x8F8F,
    /* Vertical Scales */
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
 * vdi_vsf_udpat - Set user-defined fill pattern
 */
void vdi_vsf_udpat(Vwk * vwk)
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
 * vdi_vsf_interior - Set fill style
 */
void vdi_vsf_interior(Vwk * vwk)
{
    WORD fs;

    fs = ((INTIN[0]<MIN_FILL_STYLE) || (INTIN[0]>MAX_FILL_STYLE)) ? DEF_FILL_STYLE : INTIN[0];

    INTOUT[0] = vwk->fill_style = fs;
    st_fl_ptr(vwk);
}



/* S_FILL_INDEX: */
void vdi_vsf_style(Vwk * vwk)
{
    WORD fi;

    fi = INTIN[0];

    if (vwk->fill_style == FIS_PATTERN) {
        if ((fi > MAX_FILL_PATTERN) || (fi < MIN_FILL_PATTERN))
            fi = DEF_FILL_PATTERN;
    } else {
        if ((fi > MAX_FILL_HATCH) || (fi < MIN_FILL_HATCH))
            fi = DEF_FILL_HATCH;
    }
    vwk->fill_index = (INTOUT[0] = fi) - 1;
    st_fl_ptr(vwk);
}



/* S_FILL_COLOR: */
void vdi_vsf_color(Vwk * vwk)
{
    WORD fc;

    fc = validate_color_index(INTIN[0]);

    INTOUT[0] = fc;
    vwk->fill_color = MAP_COL[fc];
}



/* ST_FILLPERIMETER: */
void vdi_vsf_perimeter(Vwk * vwk)
{
    if (INTIN[0] == 0) {
        INTOUT[0] = 0;
        vwk->fill_per = FALSE;
    } else {
        INTOUT[0] = 1;
        vwk->fill_per = TRUE;
    }
}



/*
 * clipbox - Just clips and copies the inputs for use by "rectfill"
 *
 * input:
 *     clip->xmn_clip = x clipping minimum.
 *         ->xmx_clip = x clipping maximum.
 *         ->ymn_clip = y clipping minimum.
 *         ->ymx_clip = y clipping maximum.
 *     rect->x1       = x coord of upper left corner.
 *         ->y1       = y coord of upper left corner.
 *         ->x2       = x coord of lower right corner.
 *         ->y2       = y coord of lower right corner.
 *
 * output:
 *     FALSE -> everything clipped
 *     rect->x1 = x coord of upper left corner.
 *         ->y1 = y coord of upper left corner.
 *         ->x2 = x coord of lower right corner.
 *         ->y2 = y coord of lower right corner.
 */
static BOOL clipbox(const VwkClip *clip, Rect *rect)
{
    WORD x1, y1, x2, y2;

    x1 = rect->x1;
    y1 = rect->y1;
    x2 = rect->x2;
    y2 = rect->y2;

    /* clip x coordinates */
    if (x1 < clip->xmn_clip) {
        if (x2 < clip->xmn_clip)
            return FALSE;           /* clipped box is null */
        rect->x1 = clip->xmn_clip;
    }
    if (x2 > clip->xmx_clip) {
        if (x1 > clip->xmx_clip)
            return FALSE;           /* clipped box is null */
        rect->x2 = clip->xmx_clip;
    }

    /* clip y coordinates */
    if (y1 < clip->ymn_clip) {
        if (y2 < clip->ymn_clip)
            return FALSE;           /* clipped box is null */
        rect->y1 = clip->ymn_clip;
    }
    if (y2 > clip->ymx_clip) {
        if (y1 > clip->ymx_clip)
            return FALSE;           /* clipped box is null */
        rect->y2 = clip->ymx_clip;
    }

    return TRUE;
}



/*
 * vdi_vr_recfl - draw filled rectangle
 */
void vdi_vr_recfl(Vwk * vwk)
{
    Rect rect;

    /*
     * exchange corners to EmuTOS preferred format (ll, ur) if necessary
     */
    arb_corner((Rect *) PTSIN);

    /*
     * make temporary copy to prevent the clipping code from damaging
     * the PTSIN values we might need later on for perimeter draws
     */
    rect = * (Rect *) PTSIN;

    if (vwk->clip)
        if (!clipbox(VDI_CLIP(vwk), &rect))
            return;

    /* do the real work... */
    draw_rect(vwk, &rect, vwk->fill_color);
}



/*
 * vdi_vqf_attributes - Inquire current fill area attributes
 */
void vdi_vqf_attributes(Vwk * vwk)
{
    WORD *pointer;

    pointer = INTOUT;
    *pointer++ = vwk->fill_style;
    *pointer++ = REV_MAP_COL[vwk->fill_color];
    *pointer++ = vwk->fill_index + 1;
    *pointer++ = vwk->wrt_mode + 1;
    *pointer = vwk->fill_per;
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
    case FIS_HOLLOW:
        pp = &HOLLOW;
        break;

    case FIS_SOLID:
        pp = &SOLID;
        break;

    case FIS_PATTERN:
        if (fi < 8) {
            pm = DITHRMSK;
            pp = &DITHER[fi * (pm + 1)];
        } else {
            pm = OEMMSKPAT;
            pp = &OEMPAT[(fi - 8) * (pm + 1)];
        }
        break;
    case FIS_HATCH:
        if (fi < 6) {
            pm = HAT_0_MSK;
            pp = &HATCH0[fi * (pm + 1)];
        } else {
            pm = HAT_1_MSK;
            pp = &HATCH1[(fi - 6) * (pm + 1)];
        }
        break;
    case FIS_USER:
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
 * For each scan line:
 *  For each non-horizontal scanline crossing poly, do:
 *   - Find intersection points of scan line with poly edges
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

void clc_flit(const VwkAttrib *attr, const VwkClip *clipper, const Point *point, WORD vectors, WORD start, WORD end)
{
    WORD * bufptr;              /* point to array of x-values. */
    int intersections;          /* count of intersections */
    int i;
    WORD y;                     /* current scan line */

    for (y = start; y > end; y--) {
        /* Initialize the pointers and counters. */
        intersections = 0;  /* reset counter */
        bufptr = vdishare.main.fill_buffer;

        /* find intersection points of scan line with poly edges. */
        for (i = 0; i < vectors; i++) {
            WORD y1, y2, dy;

            y1 = point[i].y;        /* fetch y-value of 1st endpoint. */
            y2 = point[i+1].y;      /* fetch y-value of 2nd endpoint. */

            /* if the current vector is horizontal, ignore it. */
            dy = y2 - y1;
            if (dy) {
                WORD dy1, dy2;

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
                if ((dy1^dy2) < 0) {
                    int dx;
                    WORD x1, x2;
                    x1 = point[i].x;        /* fetch x-value of 1st endpoint. */
                    x2 = point[i+1].x;      /* fetch x-value of 2nd endpoint. */
                    dx = (x2 - x1) << 1;    /* so we can round by adding 1 below */
                    if (intersections >= MAX_VERTICES)
                        break;
                    intersections++;
                    /* fill edge buffer with x-values */
                    if (dx < 0) {
                        /* does ((dy2 * dx / dy + 1) >> 1) + x2; */
                        *bufptr++ = ((mul_div(dy2, dx, dy) + 1) >> 1) + x2;
                    }
                    else {
                        /* does ((dy1 * dx / dy + 1) >> 1) + x1; */
                        *bufptr++ = ((mul_div(dy1, dx, dy) + 1) >> 1) + x1;
                    }
                }
            }
        }

        /*
         * All of the points of intersection have now been found.  If there
         * were none (or one, which I think is impossible), then there is
         * nothing more to do.  Otherwise, sort the list of points of
         * intersection in ascending order.
         * (The list contains only the x-coordinates of the points.)
         */
        if (intersections < 2)
            continue;

        /*
         * Sort the intersections.  There are almost always exactly 2, except
         * for weird shapes (if this wasn't true, bubble sort would be a bad
         * choice).
         */
        bub_sort(vdishare.main.fill_buffer, intersections);

        /*
         * Testing under Atari TOS shows that the fill area always *includes*
         * the left & right perimeter (for those functions that allow the
         * perimeter to be drawn separately, it is drawn on top of the edge
         * pixels).  We now conform to Atari TOS.
         */

        /*
         * Loop through points, calling draw_rect_common() for each pair
         */
        bufptr = vdishare.main.fill_buffer;
        i = intersections / 2;
        while(i--) {
            WORD x1, x2;
            Rect rect;

            /* grab a pair of endpoints */
            x1 = *bufptr++;
            x2 = *bufptr++;

            /* handle clipping */
            if (attr->clip) {
                if (x1 < clipper->xmn_clip) {
                    if (x2 < clipper->xmn_clip)
                        continue;           /* entire segment clipped left */
                    x1 = clipper->xmn_clip; /* clip left end of line */
                }

                if (x2 > clipper->xmx_clip) {
                    if (x1 > clipper->xmx_clip)
                        continue;           /* entire segment clipped right */
                    x2 = clipper->xmx_clip; /* clip right end of line */
                }
            }
            rect.x1 = x1;
            rect.y1 = y;
            rect.x2 = x2;
            rect.y2 = y;

            /* rectangle fill routine draws horizontal line */
            draw_rect_common(attr, &rect);
        }
    }
}



/*
 * polygon - draw a filled polygon
 */
void
polygon(Vwk * vwk, Point * ptsin, int count)
{
    WORD i, k;
    WORD fill_maxy, fill_miny;
    Point * point, * ptsget, * ptsput;
    const VwkClip *clipper;
    VwkAttrib attr;

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

    /* cast structure needed by clc_flit */
    clipper = VDI_CLIP(vwk);
    if (vwk->clip) {
        if ((fill_maxy < clipper->ymn_clip)     /* polygon entirely before clip */
         || (fill_miny > clipper->ymx_clip))    /* polygon entirely after clip */
            return;
        if (fill_miny < clipper->ymn_clip)
            fill_miny = clipper->ymn_clip - 1;  /* polygon partial overlap */
        if (fill_maxy > clipper->ymx_clip)
            fill_maxy = clipper->ymx_clip;      /* polygon partial overlap */
    }

    /* close the polygon, connect last and first point */
    ptsget = ptsin;
    ptsput = ptsin + count;
    ptsput->x = ptsget->x;
    ptsput->y = ptsget->y;

    /* copy data needed by clc_flit -> draw_rect_common */
    Vwk2Attrib(vwk, &attr, vwk->fill_color);

    /* really draw it */
    clc_flit(&attr, clipper, ptsin, count, fill_maxy, fill_miny);

    if (vwk->fill_per == TRUE) {
        LN_MASK = 0xffff;
        polyline(vwk, ptsin, count+1, vwk->fill_color);
    }
}



/*
 * vdi_v_fillarea - Fill an area
 */
void vdi_v_fillarea(Vwk * vwk)
{
    Point * point = (Point*)PTSIN;
    int count = CONTRL[1];

#if HAVE_BEZIER
    /* check, if we want to draw a filled bezier curve */
    if (CONTRL[5] == 13 && vwk->bez_qual )
        v_bez_fill(vwk, point, count);
    else
#endif
        polygon(vwk, point, count);
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
 * pixelread - gets a pixel's colour
 *
 * For palette-based resolutions, this returns the colour index; for
 * Truecolor resolutions, this returns the 16-bit RGB colour.
 *
 * input:
 *     PTSIN(0) = x coordinate.
 *     PTSIN(1) = y coordinate.
 * output:
 *     pixel colour
 */
static UWORD
pixelread(const WORD x, const WORD y)
{
    UWORD *addr;
    UWORD mask;

#if CONF_WITH_VDI_16BIT
    if (TRUECOLOR_MODE)
    {
        addr = get_start_addr16(x, y);
        return *addr;                   /* just return the data at the address */
    }
#endif

    /* convert x,y to start address and bit mask */
    addr = get_start_addr(x, y);
    addr += v_planes;                   /* start at highest-order bit_plane */
    mask = 0x8000 >> (x&0xf);           /* initial bit position in WORD */

    return get_color(mask, addr);       /* return the composed color value */
}



#if CONF_WITH_VDI_16BIT
/*
 * search_to_right16() - Truecolor version of search_to_right()
 */
static UWORD search_to_right16(const VwkClip *clip, WORD x, const UWORD search_col, UWORD *addr)
{
    UWORD pixel, search;

    search = search_col & ~OVERLAY_BIT; /* ignore overlay bit in search colour */

    /*
     * scan upwards until pixel of different colour found
     */
    for ( ; x <= clip->xmx_clip; x++)
    {
        pixel = *addr++ & ~OVERLAY_BIT; /* ignore overlay bit on screen */
        if (pixel != search)
            break;
    }

    return x - 1;
}



/*
 * search_to_left16() - Truecolor version of search_to_left()
 */
static UWORD search_to_left16(const VwkClip *clip, WORD x, const UWORD search_col, UWORD *addr)
{
    UWORD pixel, search;

    search = search_col & ~OVERLAY_BIT; /* ignore overlay bit in search colour */

    /*
     * scan downwards until pixel of different colour found
     */
    for ( ; x >= clip->xmn_clip; x--)
    {
        pixel = *addr-- & ~OVERLAY_BIT; /* ignore overlay bit on screen */
        if (pixel != search)
            break;
    }

    return x + 1;
}



/*
 * end_pts16() - Truecolor version of end_pts()
 */
static WORD end_pts16(const VwkClip *clip, WORD x, WORD y, WORD *xleftout, WORD *xrightout)
{
    UWORD color;
    UWORD *addr;

    /*
     * convert x,y to start address and get colour
     */
    addr = get_start_addr16(x, y);
    color = *addr & ~OVERLAY_BIT;    /* ignore overlay bit on screen */

    /*
     * get left and right end
     */
    *xrightout = search_to_right16(clip, x, color, addr);
    *xleftout = search_to_left16(clip, x, color, addr);

    if (color != search_color)
        return seed_type ^ 1;   /* return segment not of search color */

    return seed_type ^ 0;       /* return segment is of search color */
}
#endif



static UWORD
search_to_right (const VwkClip * clip, WORD x, UWORD mask, const UWORD search_col, UWORD * addr)
{
    /* is x coord < x resolution ? */
    while( x++ < clip->xmx_clip ) {
        UWORD color;

        /* need to jump over interleaved bit_plane? */
        rorw1(mask);    /* rotate right */
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
search_to_left (const VwkClip * clip, WORD x, UWORD mask, const UWORD search_col, UWORD * addr)
{
    /* Now, search to the left. */
    while (x-- > clip->xmn_clip) {
        UWORD color;

        /* need to jump over interleaved bit_plane? */
        rolw1(mask);    /* rotate left */
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
 *           (for the contourfill() routine.)
 *
 * input:   clip        ptr to clipping rectangle
 *          x           starting x value
 *          y           y coordinate of line
 *
 * output:  xleftout    ptr to variable to receive leftmost point of this colour
 *          xrightout   ptr to variable to receive rightmost point of this colour
 *
 * returns success flag:
 *          0 => no endpoints or starting x value on edge
 *          1 => endpoints found
 */
static WORD end_pts(const VwkClip *clip, WORD x, WORD y, WORD *xleftout, WORD *xrightout)
{
    UWORD color;
    UWORD * addr;
    UWORD mask;

    /* see, if we are in the y clipping range */
    if ( y < clip->ymn_clip || y > clip->ymx_clip)
        return 0;

#if CONF_WITH_VDI_16BIT
    if (TRUECOLOR_MODE)
    {
        return end_pts16(clip, x, y, xleftout, xrightout);
    }
#endif

    /* convert x,y to start address and bit mask */
    addr = get_start_addr(x, y);
    addr += v_planes;                   /* start at highest-order bit_plane */
    mask = 0x8000 >> (x & 0x000f);   /* fetch the pixel mask. */

    /* get search color and the left and right end */
    color = get_color (mask, addr);
    *xrightout = search_to_right (clip, x, mask, color, addr);
    *xleftout = search_to_left (clip, x, mask, color, addr);

    /* see, if the whole found segment is of search color? */
    if ( color != search_color ) {
        return seed_type ^ 1;   /* return segment not of search color */
    }
    return seed_type ^ 0;       /* return segment is of search color */
}



/*
 * crunch_queue - move qtop down to remove unused seeds
 */
static void crunch_queue(void)
{
    while (((qtop-1)->y == EMPTY) && (qtop > qbottom))
        qtop--;
    if (qptr >= qtop)
        qptr = qbottom;
}



/*
 * get_seed - put seeds into Q, if (xin,yin) is not of search_color
 */
static WORD get_seed(const VwkAttrib *attr, const VwkClip *clip,
                        WORD xin, WORD yin, WORD *xleftout, WORD *xrightout)
{
    SEGMENT *qhole;         /* an empty space in the queue */
    SEGMENT *qtmp;

    if (end_pts(clip, xin, ABS(yin), xleftout, xrightout)) {
        /* false if of search_color */
        for (qtmp = qbottom, qhole = NULL; qtmp < qtop; qtmp++) {
            /* skip holes, remembering the first hole we find */
            if (qtmp->y == EMPTY)
            {
                if (qhole == NULL)
                    qhole = qtmp;
                continue;
            }
            /* see, if we ran into another seed */
            if ( ((qtmp->y ^ DOWN_FLAG) == yin) && (qtmp->xleft == *xleftout) )
            {
                /* we ran into another seed so remove it and fill the line */
                Rect rect;

                rect.x1 = *xleftout;
                rect.y1 = ABS(yin);
                rect.x2 = *xrightout;
                rect.y2 = ABS(yin);

                /* rectangle fill routine draws horizontal line */
                draw_rect_common(attr, &rect);

                qtmp->y = EMPTY;
                if ((qtmp+1) == qtop)
                    crunch_queue();
                return 0;
            }
        }

        /*
         * there were no holes, so raise qtop if we can
         */
        if (qhole == NULL) {
            if (++qtop > vdishare.queue+QSIZE) { /* can't raise qtop ... */
                KDEBUG(("contourfill(): queue overflow\n"));
                return -1;      /* error */
            }
        } else
            qtmp = qhole;

        qtmp->y = yin;      /* put the y and endpoints in the Q */
        qtmp->xleft = *xleftout;
        qtmp->xright = *xrightout;
        return 1;           /* we put a seed in the Q */
    }

    return 0;           /* we didn't put a seed in the Q */
}



/* common function for line-A linea_fill() and VDI d_countourfill() */
void contourfill(const VwkAttrib * attr, const VwkClip *clip)
{
    WORD newxleft;              /* ends of line at oldy +       */
    WORD newxright;             /* the current direction    */
    WORD oldxleft;              /* left end of line at oldy     */
    WORD oldxright;             /* right end                    */
    WORD oldy;                  /* the previous scan line       */
    WORD xleft;                 /* temporary endpoints          */
    WORD xright;                /* */
    WORD direction;             /* is next scan line up or down */
    WORD gotseed;               /* 1 => seed was put in the Q */
                                /* 0 => no seed was put in the Q */
                                /* -1 => queue overflowed */

    xleft = PTSIN[0];
    oldy = PTSIN[1];

    if (xleft < clip->xmn_clip || xleft > clip->xmx_clip ||
        oldy < clip->ymn_clip  || oldy > clip->ymx_clip)
        return;

    search_color = INTIN[0];

    if ((WORD)search_color < 0) {
        search_color = pixelread(xleft,oldy);
        seed_type = 1;
    } else {
        /* Range check the color and convert the index to a pixel value */
        if (search_color >= numcolors)
            return;
        search_color = MAP_COL[search_color];
#if CONF_WITH_VDI_16BIT
        if (TRUECOLOR_MODE)
        {
            /* convert search_color to 16-bit pixel value */
            search_color = phys_work.ext->palette[search_color];
        }
#endif
        seed_type = 0;
    }

    /* check if anything to do */
    if (!end_pts(clip, xleft, oldy, &oldxleft, &oldxright))
        return;

    /*
     * from this point on we must NOT access PTSIN[], since the area
     * is overwritten by the queue of seeds!
     */
    qptr = qbottom = vdishare.queue;
    qptr->y = (oldy | DOWN_FLAG);   /* stuff a point going down into the Q */
    qptr->xleft = oldxleft;
    qptr->xright = oldxright;
    qtop = qptr + 1;                /* one above highest seed point */

    while (1) {
        Rect rect;

        direction = (oldy & DOWN_FLAG) ? 1 : -1;
        gotseed = get_seed(attr, clip, oldxleft, oldy+direction, &newxleft, &newxright);
        if (gotseed < 0)
            return;         /* error, quit */

        if ((newxleft < (oldxleft - 1)) && gotseed) {
            xleft = oldxleft;
            while (xleft > newxleft) {
                --xleft;
                if (get_seed(attr, clip, xleft, oldy^DOWN_FLAG, &xleft, &xright) < 0)
                    return; /* error, quit */
            }
        }
        while (newxright < oldxright) {
            ++newxright;
            gotseed = get_seed(attr, clip, newxright, oldy+direction, &xleft, &newxright);
            if (gotseed < 0)
                return;     /* error, quit */
        }
        if ((newxright > (oldxright + 1)) && gotseed) {
            xright = oldxright;
            while (xright < newxright) {
                ++xright;
                if (get_seed(attr, clip, xright, oldy^DOWN_FLAG, &xleft, &xright) < 0)
                    return; /* error, quit */
            }
        }

        /* Eventually jump out here */
        if (qtop == qbottom)
            break;

        while (qptr->y == EMPTY) {
            qptr++;
            if (qptr == qtop)
                qptr = qbottom;
        }

        oldy = qptr->y;
        oldxleft = qptr->xleft;
        oldxright = qptr->xright;
        qptr->y = EMPTY;
        if (++qptr == qtop)
            crunch_queue();

        rect.x1 = oldxleft;
        rect.y1 = ABS(oldy);
        rect.x2 = oldxright;
        rect.y2 = ABS(oldy);

        /* rectangle fill routine draws horizontal line */
        draw_rect_common(attr, &rect);

        /* after every line, check for early abort */
        if ((*SEEDABORT)())
            break;
    }
}                               /* end of fill() */



/*
 * no_abort
 *
 * the VDI routine v_contourfill() calls the line-A routine contourfill()
 * to do its work.  contourfill() calls the routine pointed to by SEEDABORT
 * on a regular basis to determine whether to prematurely abort the fill.
 * we initialise SEEDABORT to point to the routine below, which never
 * requests an early abort.
 */
static WORD no_abort(void)
{
    return 0;
}



/* VDI version */
void vdi_v_contourfill(Vwk * vwk)
{
    VwkAttrib attr;

    SEEDABORT = no_abort;
    Vwk2Attrib(vwk, &attr, vwk->fill_color);
    contourfill(&attr, VDI_CLIP(vwk));
}



void vdi_v_get_pixel(Vwk * vwk)
{
    WORD pel;
    const WORD x = PTSIN[0];       /* fetch x coord. */
    const WORD y = PTSIN[1];       /* fetch y coord. */

    /* Get the requested pixel */
    pel = (WORD)pixelread(x,y);

#if CONF_WITH_VDI_16BIT
    if (TRUECOLOR_MODE)
    {
        INTOUT[0] = 0;
        INTOUT[1] = pel;
        return;
    }
#endif

    INTOUT[0] = pel;
    INTOUT[1] = REV_MAP_COL[pel];
}



/*
 * get_pix - gets a pixel (just for line-A)
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
 * put_pix - plot a pixel (just for line-A)
 *
 * NOTE: this does not work for Truecolor modes in TOS4 due to a bug.
 * Register a4 is used to reference the lineA pointer table, but has
 * never been set; the code should be using a1 instead.  So we can
 * safely assume that no existing program is expecting this to work.
 *
 * However, because EmuTOS aims to be better than TOS, a functioning
 * Truecolor mode has been implemented.  The EmuTOS Truecolor code
 * is based on what TOS4 apparently intends to do, i.e. just stores
 * the word passed in INTIN[0] as-is.  This also meshes with the
 * operation of linea2 in TOS4 Truecolor modes, which just retrieves
 * the word at the specified address.
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

#if CONF_WITH_VDI_16BIT
    if (TRUECOLOR_MODE)
    {
        /*
         * convert x,y to start address & validate
         */
        addr = get_start_addr16(x, y);
        if (addr < (UWORD*)v_bas_ad || addr >= get_start_addr16(V_REZ_HZ, V_REZ_VT))
            return;
        *addr = INTIN[0];   /* store 16-bit Truecolor value */
        return;
    }
#endif

    /* convert x,y to start address */
    addr = get_start_addr(x, y);
    /* co-ordinates can wrap, but cannot write outside screen,
     * alternatively this could check against v_bas_ad+vram_size()
     */
    if (addr < (UWORD*)v_bas_ad || addr >= get_start_addr(V_REZ_HZ, V_REZ_VT)) {
        return;
    }
    color = INTIN[0];           /* device dependent encoded color bits */
    mask = 0x8000 >> (x&0xf);   /* initial bit position in WORD */

    for (plane = v_planes; plane; plane--) {
        if (color&0x0001)
            *addr++ |= mask;
        else
            *addr++ &= ~mask;
        color >>= 1;
    }
}
