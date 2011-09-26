/*
 * vdi_line.c - Line drawing
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "intmath.h"
#include "vdi_defs.h"
#include "tosvars.h"
#include "lineavars.h"



#define ABS(x) ((x) >= 0 ? (x) : -(x))



/* prototypes*/
static void arrow(Vwk * vwk, Point * point, int count);

/* the six predefined line styles */
const UWORD LINE_STYLE[6] = { 0xFFFF, 0xFFF0, 0xC0C0, 0xFF18, 0xFF00, 0xF191 };
static WORD q_circle[MX_LN_WIDTH];     /* Holds the circle DDA */

/* Wide line attribute save areas */
static WORD s_begsty, s_endsty, s_fil_col, s_fill_per;


/* ST_UD_LINE_STYLE: */
void _vsl_udsty(Vwk * vwk)
{
    vwk->ud_ls = *INTIN;
}


/*
 * _vsl_type - Set line style for line-drawing functions
 */

void _vsl_type(Vwk * vwk)
{
    WORD li;

    CONTRL[4] = 1;

    li = (*INTIN - 1);
    if ((li >= MX_LN_STYLE) || (li < 0))
        li = 0;

    *INTOUT = (vwk->line_index = li) + 1;
}



/*
 * _vsl_width - Set line width
 */

void _vsl_width(Vwk * vwk)
{
    WORD w, *pts_out;

    /* Limit the requested line width to a reasonable value. */
    w = PTSIN[0];
    if (w < 1)
        w = 1;
    else if (w > SIZ_TAB[6])
        w = SIZ_TAB[6];

    /* Make the line width an odd number (one less, if even). */
    w = ((w - 1) / 2) * 2 + 1;

    /* Set the line width internals and return parameters */
    CONTRL[2] = 1;
    pts_out = PTSOUT;
    *pts_out++ = vwk->line_width = w;
    *pts_out = 0;
}



/*
 * _vsl_ends - sets the style of end point for line starting and ending points
 */

void _vsl_ends(Vwk * vwk)
{
    WORD lb, le;
    WORD *pointer;

    *(CONTRL + 4) = 2;

    pointer = INTIN;
    lb = *pointer++;
    if (lb < 0 || lb > 2)
        lb = 0;

    le = *pointer;
    if (le < 0 || le > 2)
        le = 0;

    pointer = INTOUT;
    *pointer++ = vwk->line_beg = lb;
    *pointer = vwk->line_end = le;
}



/*
 * _vsl_color - sets the color for line-drawing
 */

void _vsl_color(Vwk * vwk)
{
    WORD lc;

    *(CONTRL + 4) = 1;
    lc = *(INTIN);
    if ((lc >= DEV_TAB[13]) || (lc < 0))
        lc = 1;
    *(INTOUT) = lc;
    vwk->line_color = MAP_COL[lc];
}



/*
 * vql_attr - Inquire current polyline attributes
 */

void vql_attr(Vwk * vwk)
{
    INTOUT[0] = vwk->line_index + 1;
    INTOUT[1] = REV_MAP_COL[vwk->line_color];
    INTOUT[2] = vwk->wrt_mode + 1;

    PTSOUT[0] = vwk->line_width;
    PTSOUT[1] = 0;

    CONTRL[2] = 1;
    CONTRL[4] = 3;
}



/*
 * habline - draw a horizontal line (just for linea needed)
 *
 * This routine is just a wrapper for horzline.
 */
#if 0
void habline(Vwk * vwk) {
    Line * line = (Line*)PTSIN;

    horzline(vwk, line);
}
#endif


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

void horzline(const Vwk * vwk, Line * line)
{
    /* a horizontal line is a rectangle with one pixel height */
    arb_line(line);
    draw_rect(vwk, (Rect*)line, vwk->line_color);
}

/*
 * draw_rect - draw one or more horizontal lines
 *
 * Rewritten in C instead of assembler this routine is now looking
 * very complicated. Since in assembler you can pass parameters in
 * registers, you have an easy game to be both, fast and flexible.
 *
 * In C we need to double the code a bit, like loops and such things.
 * So, the following code works as follows:
 *
 * 1.  Fill variables with all values for masks, fringes etc.
 * 2.  Decide, if we have the situation of writing just one WORD
 * 3.  In any case we choose the writing mode now
 * 4.  Then loop through the y axis (just one pass for horzline)
 * 5.  Inner loop through the planes
 * 6a. If just one WORD, mask out the bits and process the action
 * 6b. else mask out left fringe, write WORDS, process the right fringe
 */

void draw_rect(const Vwk * vwk, const Rect * rect, const UWORD fillcolor) {
    UWORD leftmask, rightmask, patmsk;
    UWORD *addr;
    int dx, y, yinc;
    int patadd;               /* advance for multiplane patterns */
    int leftpart;
    int rightpart;
    int planes;

    /* Get the pattern with which the line is to be drawn. */
    planes = v_planes;
    dx = rect->x2 - rect->x1 - 16;      /* width of line - one WORD */
    addr = get_start_addr(rect->x1, rect->y1);  /* init adress counter */
    patmsk = vwk->patmsk;                       /* which pattern to start with */
    patadd = vwk->multifill ? 16 : 0;           /* multi plane pattern offset */
    leftpart = rect->x1 & 0xf;              /* left fringe */
    rightpart = rect->x2 & 0xf;         /* right fringe */
    leftmask = ~(0xffff>>leftpart);     /* origin for not left fringe lookup */
    rightmask = 0x7fff>>rightpart;      /* origin for right fringe lookup */
    yinc = (v_lin_wr >> 1) - planes;

    /* check, if we have to process just one single WORD on screen */
    if (dx+leftpart < 0) {
        switch (vwk->wrt_mode) {
        case 3:  /* nor */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int plane;
                int patind = patmsk&y;          /* pattern to start with */
                UWORD color = fillcolor;

                /* init adress counter */
                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    UWORD pattern = vwk->patptr[patind];

                    if (color & 0x0001) {
                        /* Isolate the necessary pixels */
                        UWORD bits = *addr;     /* get data from screen address */
                        UWORD help = bits;
                        bits |= ~pattern;       /* and complement of mask with source */
                        help ^= bits;           /* isolate changed bits */
                        help &= leftmask | rightmask;          /* isolate changed bits outside of fringe */
                        bits ^= help;           /* restore them to original states */
                        *addr = bits;            /* write back the result */
                    } else {
                        /* Isolate the necessary pixels */
                        UWORD bits = *addr;     /* get data from screen address */
                        UWORD help = bits;
                        bits &= pattern;        /* and complement of mask with source */
                        help ^= bits;           /* isolate changed bits */
                        help &= leftmask | rightmask;           /* isolate changed bits outside of fringe */
                        bits ^= help;           /* restore them to original states */
                        *addr = bits;            /* write back the result */
                    }
                    addr++;                     /* advance one WORD to next plane */
                    patind += patadd;
                    color >>= 1;
                }
                addr += yinc;                   /* next scanline */
            }
            break;
        case 2:  /* xor */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int plane;
                int patind = patmsk&y;          /* pattern to start with */

                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    UWORD help,bits;
                    UWORD pattern = vwk->patptr[patind];

                    /* Isolate the necessary pixels */
                    bits = *addr;        /* get data from screen address */
                    help = bits;
                    bits ^= pattern;  /* xor the pattern with the source */
                    help ^= bits;       /* xor result with source - now have pattern */
                    help &= leftmask | rightmask;       /* isolate bits */
                    bits ^= help;       /* restore states of bits outside of fringe */
                    *addr = bits;      /* write back the result */
                    addr++; /* advance one WORD to next plane */
                    patind += patadd;
                }
                addr += yinc;           /* next scanline */
            }
            break;
        case 1:  /* or */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int plane;
                int patind = patmsk&y;          /* pattern to start with */
                UWORD color = fillcolor;

                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    UWORD pattern = vwk->patptr[patind];

                    if (color & 0x0001) {
                        UWORD help,bits;

                        /* Isolate the necessary pixels */
                        bits = *addr;            /* get data from screen address */
                        help = bits;
                        bits |= pattern;        /* and complement of mask with source */
                        help ^= bits;           /* isolate changed bits */
                        help &= ~(leftmask | rightmask);        /* isolate bits */
                        bits ^= help;           /* restore them to original states */
                        *addr = bits;            /* write back the result */
                    } else {
                        /* Isolate the necessary pixels */
                        UWORD bits = *addr;     /* get data from screen address */
                        UWORD help = bits;
                        bits &= ~pattern;        /* and complement of mask with source */
                        help ^= bits;           /* isolate changed bits */
                        help &= leftmask | rightmask;   /* isolate bits */
                        bits ^= help;           /* restore them to original states */
                        *addr = bits;            /* write back the result */
                    }
                    addr++; /* advance one WORD to next plane */
                    patind += patadd;
                    color >>= 1;
                }
                addr += yinc;           /* next scanline */
            }
            break;
        default: /* rep */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int patind = patmsk&y;          /* pattern to start with */
                int plane;
                UWORD color = fillcolor;

                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    UWORD bits;
                    UWORD pattern = 0;

                    if (color & 0x0001)
                        pattern = vwk->patptr[patind];

                    /* Isolate the necessary pixels */
                    bits = *addr;        /* get data from screen address */
                    bits ^= pattern;    /* xor the pattern with the source */
                    bits &= leftmask|rightmask; /* isolate the bits outside the fringe */
                    bits ^= pattern;    /* restore the bits outside the fringe */
                    *addr = bits;        /* write back the result */
                    addr++; /* advance one WORD to next plane */
                    patind += patadd;
                    color >>= 1;
                }
                addr += yinc;           /* next scanline */
            }
        }

    }
    /* write the left fringe, then some whole WORDs, then the right fringe */
    else {
        leftpart = 16 - leftpart;       /* precalculate delta for the left */

        switch (vwk->wrt_mode) {
        case 3:  /* nor */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int plane;
                int patind = patmsk&y;          /* pattern to start with */
                UWORD color = fillcolor;

                /* init adress counter */
                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    UWORD * adr = addr;
                    int pixels = dx;
                    UWORD pattern = vwk->patptr[patind];

                    if (color & 0x0001) {
                        int bw;
                        pattern = ~pattern;
                        /* check, if the line is completely contained within one WORD */

                        /* Draw the left fringe */
                        if (leftmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits |= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= leftmask;   /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */

                            adr += planes;
                            pixels -= leftpart;
                        }
                        /* Full bytes */
                        for (bw = pixels >> 4; bw >= 0; bw--) {
                            *adr |= pattern;
                            adr += planes;
                        }
                        /* Draw the right fringe */
                        if (~rightmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits |= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= rightmask;  /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */
                        }
                        pattern = ~pattern;
                    } else {
                        int bw;

                        /* Draw the left fringe */
                        if (leftmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits &= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= leftmask;   /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */

                            adr += planes;
                            pixels -= leftpart;
                        }
                        /* Full bytes */
                        for (bw = pixels >> 4; bw >= 0; bw--) {
                            *adr &= pattern;
                            adr += planes;
                        }
                        /* Draw the right fringe */
                        if (~rightmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits &= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= rightmask;  /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */
                        }
                    }
                    addr++;                     /* advance one WORD to next plane */
                    patind += patadd;
                    color >>= 1;
                }
                addr += yinc;           /* next scanline */
            }
            break;
        case 2:  /* xor */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int plane;
                int patind = patmsk&y;          /* pattern to start with */

                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    UWORD * adr = addr;
                    int pixels = dx;
                    UWORD pattern = vwk->patptr[patind];

                    int bw;
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

                        adr += planes;
                        pixels -= leftpart;
                    }
                    /* Full bytes */
                    for (bw = pixels >> 4; bw >= 0; bw--) {
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
                    addr++; /* advance one WORD to next plane */
                    patind += patadd;
                }
                addr += yinc;           /* next scanline */
            }
            break;
        case 1:  /* or */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int plane;
                int patind = patmsk&y;          /* pattern to start with */
                UWORD color = fillcolor;

                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    UWORD * adr = addr;
                    int pixels = dx;
                    UWORD pattern = vwk->patptr[patind];

                    if (color & 0x0001) {
                        int bw;

                        /* Draw the left fringe */
                        if (leftmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits |= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= ~leftmask;  /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */

                            adr += planes;
                            pixels -= leftpart;
                        }
                        /* Full bytes */
                        for (bw = pixels >> 4; bw >= 0; bw--) {
                            *adr |= pattern;
                            adr += planes;
                        }
                        /* Draw the right fringe */
                        if (~rightmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits |= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= ~rightmask; /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */
                        }
                    } else {
                        int bw;

                        pattern = ~pattern;

                        /* Draw the left fringe */
                        if (leftmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits &= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= leftmask;   /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */

                            adr += planes;
                            pixels -= leftpart;
                        }
                        /* Full bytes */
                        for (bw = pixels >> 4; bw >= 0; bw--) {
                            *adr &= pattern;
                            adr += planes;
                        }
                        /* Draw the right fringe */
                        if (~rightmask) {
                            UWORD bits = *adr;  /* get data from screen address */
                            UWORD help = bits;
                            bits &= pattern;    /* and complement of mask with source */
                            help ^= bits;       /* isolate changed bits */
                            help &= rightmask;  /* isolate changed bits outside of fringe */
                            bits ^= help;       /* restore them to original states */
                            *adr = bits;        /* write back the result */
                        }
                        pattern = ~pattern;
                    }
                    addr++; /* advance one WORD to next plane */
                    patind += patadd;
                    color >>= 1;
                }
                addr += yinc;           /* next scanline */
            }
            break;
        default: /* rep */
            for (y = rect->y1; y <= rect->y2; y++ ) {
                int patind = patmsk&y;          /* pattern to start with */
                int plane;
                UWORD color = fillcolor;

                for (plane = planes-1; plane >= 0; plane-- ) {
                    /* load values fresh for this bitplane */
                    int bw;
                    UWORD * adr = addr;
                    int pixels = dx;
                    UWORD pattern = 0;

                    if (color & 0x0001)
                        pattern = vwk->patptr[patind];

                    /* Draw the left fringe */
                    if (leftmask) {
                        UWORD bits = *adr;      /* get data from screen address */
                        bits ^= pattern;        /* xor the pattern with the source */
                        bits &= leftmask;       /* isolate the bits outside the fringe */
                        bits ^= pattern;        /* restore the bits outside the fringe */
                        *adr = bits;            /* write back the result */

                        adr += planes;
                        pixels -= leftpart;
                    }
                    /* Full WORDs */
                    for (bw = pixels >> 4; bw >= 0; bw--) {
                        *adr = pattern;
                        adr += planes;
                    }
                    /* Draw the right fringe */
                    if (~rightmask) {
                        UWORD bits = *adr;      /* get data from screen address */
                        bits ^= pattern;        /* xor the pattern with the source */
                        bits &= rightmask;      /* isolate the bits outside the fringe */
                        bits ^= pattern;        /* restore the bits outside the fringe */
                        *adr = bits;            /* write back the result */
                    }
                    addr++; /* advance one WORD to next plane */
                    patind += patadd;
                    color >>= 1;
                }
                addr += yinc;           /* next scanline */
            }
        }
    }
}



/*
 * _v_pline - the wrapper
 */

void _v_pline(Vwk * vwk)
{
    WORD l;
    Point * point = (Point*)PTSIN;
    int count = CONTRL[1];

    l = vwk->line_index;
    LN_MASK = (l < 6) ? LINE_STYLE[l] : vwk->ud_ls;

#if HAVE_BEZIER
    /* check, if we want to draw a bezier curve */
    if (CONTRL[5] == 13 && vwk->bez_qual )        //FIXME: bez_qual ok??
        v_bez(vwk, point, count);
    else 
#endif
    {
        if (vwk->line_width == 1) {
            polyline(vwk, point, count);
            if ((vwk->line_beg | vwk->line_end) & ARROWED)
                arrow(vwk, point, count);
        } else
            wideline(vwk, point, count);
    }
}



/*
 * clip_code - helper function
 */
static WORD clip_code(Vwk * vwk, WORD x, WORD y)
{
    WORD clip_flag;

    clip_flag = 0;
    if (x < vwk->xmn_clip)
        clip_flag = 1;
    else if (x > vwk->xmx_clip)
        clip_flag = 2;
    if (y < vwk->ymn_clip)
        clip_flag += 4;
    else if (y > vwk->ymx_clip)
        clip_flag += 8;
    return (clip_flag);
}


/*
 * clip_line - helper function
 */

BOOL clip_line(Vwk * vwk, Line * line)
{
    WORD deltax, deltay, x1y1_clip_flag, x2y2_clip_flag, line_clip_flag;
    WORD *x, *y;

    while ((x1y1_clip_flag = clip_code(vwk, line->x1, line->y1)) |
           (x2y2_clip_flag = clip_code(vwk, line->x2, line->y2))) {
        if ((x1y1_clip_flag & x2y2_clip_flag))
            return (FALSE);
        if (x1y1_clip_flag) {
            line_clip_flag = x1y1_clip_flag;
            x = &line->x1;
            y = &line->y1;
        } else {
            line_clip_flag = x2y2_clip_flag;
            x = &line->x2;
            y = &line->y2;
        }
        deltax = line->x2 - line->x1;
        deltay = line->y2 - line->y1;
        if (line_clip_flag & 1) {               /* left ? */
            *y = line->y1 + mul_div(deltay, (vwk->xmn_clip - line->x1), deltax);
            *x = vwk->xmn_clip;
        } else if (line_clip_flag & 2) {        /* right ? */
            *y = line->y1 + mul_div(deltay, (vwk->xmx_clip - line->x1), deltax);
            *x = vwk->xmx_clip;
        } else if (line_clip_flag & 4) {        /* top ? */
            *x = line->x1 + mul_div(deltax, (vwk->ymn_clip - line->y1), deltay);
            *y = vwk->ymn_clip;
        } else if (line_clip_flag & 8) {        /* bottom ? */
            *x = line->x1 + mul_div(deltax, (vwk->ymx_clip - line->y1), deltay);
            *y = vwk->ymx_clip;
        }
    }
    return (TRUE);              /* segment now clipped  */
}


/*
 * pline - draw a poly-line
 */

void polyline(Vwk * vwk, Point * point, int count)
{
    int i;
    Line line;

    for(i = count - 1; i > 0; i--) {
        line.x1 = point->x;
        line.y1 = point->y;
        point++;                /* advance point by point */
        line.x2 = point->x;
        line.y2 = point->y;

        if (!vwk->clip || clip_line(vwk, &line))
            abline(vwk, &line);
    }
}



/*
 * quad_xform - Transform according to the quadrant.
 */
static void quad_xform(WORD quad, WORD x, WORD y, WORD *tx, WORD *ty)
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
    }

    switch (quad) {
    case 1:
    case 2:
        *ty = y;
        break;

    case 3:
    case 4:
        *ty = -y;
        break;
    }
}



/*
 * perp_off -
 */

static void perp_off(WORD * px, WORD * py)
{
    WORD *vx, *vy, *pcircle, u, v;
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

    /*
     * Traverse the circle in a dda-like manner and find the coordinate
     * pair (u, v) such that the magnitude of (u*y - v*x) is minimized.
     * In case of a tie, choose the value which causes (u - v) to be
     * minimized.  If not possible, do something.
     */
    min_val = 32767;
    u = *pcircle;
    v = 0;
    while (TRUE) {
        /* Check for new minimum, same minimum, or finished. */
        magnitude = ABS(u * y - v * x);
        if ((magnitude < min_val) ||
            ((magnitude == min_val) && (ABS(x_val - y_val) > ABS(u - v)))) {
            min_val = magnitude;
            x_val = u;
            y_val = v;
        }
        else
            break;

        /* Step to the next pixel. */
        if (v == num_qc_lines - 1) {
            if (u == 1)
                break;
            else
                u--;
        }
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
}



/*
 * cir_dda - Used in wideline()
 */

static void cir_dda(Vwk * vwk)
{
    WORD i, j;
    WORD *xptr, *yptr, x, y, d;

    /* Calculate the number of vertical pixels required. */
    d = vwk->line_width;
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
 * do_circ - draw a circle
 */

static void do_circ(Vwk * vwk, WORD cx, WORD cy)
{
    WORD k;
    WORD *pointer;

    /* Only perform the act if the circle has radius. */
    if (num_qc_lines > 0) {
        Line line;

        /* Do the horizontal line through the center of the circle. */
        pointer = q_circle;
        line.x1 = cx - *pointer;
        line.x2 = cx + *pointer;
        line.y1 = cy;
        line.y2 = cy;
        if (clip_line(vwk, &line))
            horzline(vwk, &line);

        /* Do the upper and lower semi-circles. */
        for (k = 1; k < num_qc_lines; k++) {
            /* Upper semi-circle. */
            pointer = &q_circle[k];
            line.x1 = cx - *pointer;
            line.x2 = cx + *pointer;
            line.y1 = cy - k;
            line.y2 = cy - k;
            if (clip_line(vwk, &line)) {
                horzline(vwk, &line);
                pointer = &q_circle[k];
            }

            /* Lower semi-circle. */
            line.x1 = cx - *pointer;
            line.x2 = cx + *pointer;
            line.y1 = cy + k;
            line.y2 = cy + k;
            if (clip_line(vwk, &line))
                horzline(vwk, &line);
        }                       /* End for. */
    }                           /* End if:  circle has positive radius. */
}



/*
 * s_fa_attr - Save the fill area attribute
 */

void s_fa_attr(Vwk * vwk)
{
    /* Set up the fill area attribute environment. */
    LN_MASK = LINE_STYLE[0];
    s_fil_col = vwk->fill_color;
    s_fill_per = vwk->fill_per;
    s_begsty = vwk->line_beg;
    s_endsty = vwk->line_end;

    vwk->fill_color = vwk->line_color;
    vwk->line_beg = SQUARED;
    vwk->line_end = SQUARED;
    vwk->fill_per = TRUE;
    vwk->patptr = (UWORD *)&SOLID;
    vwk->patmsk = 0;
}                               /* End "s_fa_attr". */



/*
 * r_fa_attr - Restore the fill area attribute
 */

void r_fa_attr(Vwk * vwk)
{
    /* Restore the fill area attribute environment. */
    vwk->fill_color = s_fil_col;
    vwk->fill_per = s_fill_per;
    vwk->line_beg = s_begsty;
    vwk->line_end = s_endsty;
}                               /* End "r_fa_attr". */



/*
 * wideline - draw a line with width >1
 */

void wideline(Vwk * vwk, Point * point, int count)
{
    WORD i, k;         
    WORD wx1, wy1, wx2, wy2, vx, vy;
    Point *ptr, box[5];      /* box too high, to close polygon */

    /* Don't attempt wide lining on a degenerate polyline */
    if (count < 2)
        return;

    if (vwk->line_width != line_cw) {
        cir_dda(vwk);
    }

    /* If the ends are arrowed, output them. */
    if ((vwk->line_beg | vwk->line_end) & ARROWED)
        arrow(vwk, point, count);

    s_fa_attr(vwk);

    /* Initialize the starting point for the loop. */
    wx1 = point->x;
    wy1 = point->y;

    /* If the end style for the first point is not squared, output a circle. */
    if (s_begsty != SQUARED) {
        do_circ(vwk, wx1, wy1);
    }

    /* Loop over the number of points passed in. */
    for (i = 1; i < count; i++) {
        /* Get ending point for line segment */
        point++;
        wx2 = point->x;
        wy2 = point->y;

        /* Get vector from start to end of the segment. */
        vx = wx2 - wx1;
        vy = wy2 - wy1;

        /* Ignore lines of zero length. */
        if ((vx == 0) && (vy == 0))
            continue;

        /* Calculate offsets to fatten the line. */
        if (vx == 0) {
            /* line is horizontal - do it the simple way */
            vx = q_circle[0];
            vy = 0;
        }
        else if (vy == 0) {
            /* line is vertical - do it the simple way */
            vx = 0;
            vy = num_qc_lines - 1;
        }
        else {
            /* Find the offsets in x and y for a point perpendicular */
            /* to the line segment at the appropriate distance. */
            k = mul_div(-vy, ysize, xsize);
            vy = mul_div(vx, xsize, ysize);
            vx = k;
            perp_off(&vx, &vy);
        }                       /* End else:  neither horizontal nor
                                   vertical. */

        /* Prepare the control and points parameters for the polygon call. */
        ptr = box;
        ptr->x = wx1 + vx;
        ptr->y = wy1 + vy;

        ptr++;
        ptr->x = wx1 - vx;
        ptr->y = wy1 - vy;

        ptr++;
        ptr->x = wx2 - vx;
        ptr->y = wy2 - vy;

        ptr++;
        ptr->x = wx2 + vx;
        ptr->y = wy2 + vy;

        polygon(vwk, box, 4);

        /* If the terminal point of the line segment is an internal joint, */
        /* output a filled circle.                                         */
        if ((i < count - 1) || (s_endsty != SQUARED))
            do_circ(vwk, wx2, wy2);

        /* end point becomes the starting point for the next line segment. */
        wx1 = wx2;
        wy1 = wy2;
    }

    /* Restore the attribute environment. */
    r_fa_attr(vwk);
}


/*
 * Integer squareroot
 *  y=Isqrt(x) is equivalent to: y = (ULONG)(sqrtl( (long double)x ) + 0.5);
 */

static ULONG Isqrt(ULONG x)
{
    ULONG s1, s2;

    if (x < 2)
        return x;

    s1 = x--; /* This algorithm converges to (x+LSB)^1/2. Thus decrement x */
    s2 = 2;
    do {
        s1 /= 2;
        s2 *= 2;
    } while (s1 > s2);

    s1 = s2 = (s1 + (s2 / 2)) / 2;
    ((s1 &= ~7) & ~63) ? : (s1 = s2); /* Improve first estimate further */

    s2 = (1 + x/s1 + s1) / 2;
    if (s1 == s2) /* First iteration */
        return s2;

    do {
        s1 = s2;
        s2 = (1 + x/s1 + s1) / 2;
    } while (s1 > s2);

    return s2;
}

/*
 * arrow - Draw an arrow
 */

static void draw_arrow(Vwk * vwk, Point * point, int inc)
{
    WORD arrow_len, arrow_wid, line_len, line_len2;
    WORD dx, dy;
    WORD base_x, base_y, ht_x, ht_y;
    WORD temp, i;
    Point triangle[8];       /* triangle 2 high to close polygon */
    Point *ptr1, *ptr2, *xybeg;

    line_len2 = dx = dy = 0;

    /* Set up the arrow-head length and width as a function of line width. */
    temp = vwk->line_width;
    arrow_len = (temp < 4) ? 8 : (3 * temp - 1);
    arrow_wid = arrow_len / 2;

    /* Initialize the beginning pointer. */
    ptr1 = ptr2 = point;

    /* Find the first point which is not so close to the end point that it */
    /* will be obscured by the arrowhead.                                  */
    temp = CONTRL[1];
    for (i = 1; i < temp; i++) {
        /* Find the deltas between the next point and the end point.
           Transform */
        /* to a space such that the aspect ratio is uniform and the x axis */
        /* distance is preserved. */

        ptr1 += inc;
        dx = ptr2->x - ptr1->x;
        dy = mul_div(ptr2->y - ptr1->y, ysize, xsize);

        /* Get length of vector connecting the point with the end point. */
        /* If the vector is of sufficient length, the search is over. */
        line_len2 = dx*dx + dy*dy;
        if (line_len2 >= arrow_len*arrow_len)
            break;
    }                           /* End for:  over i. */
    line_len = Isqrt(line_len2);

    /* Set xybeg to the point we found */
    xybeg = ptr1;

    /* If the longest vector is insufficiently long, don't draw an arrow. */
    if (line_len < arrow_len)
        return;

    /* Rotate the arrow-head height and base vectors.  Perform calculations */
    /* in 1000x space.                                                      */

    ht_x = mul_div(arrow_len, mul_div(dx, 1000, line_len), 1000);
    ht_y = mul_div(arrow_len, mul_div(dy, 1000, line_len), 1000);
    base_x = mul_div(arrow_wid, mul_div(dy, -1000, line_len), 1000);
    base_y = mul_div(arrow_wid, mul_div(dx, 1000, line_len), 1000);

    /* Transform the y offsets back to the correct aspect ratio space. */

    ht_y = mul_div(ht_y, xsize, ysize);
    base_y = mul_div(base_y, xsize, ysize);

    /* Build a polygon into a local array first */
    ptr1 = triangle;
    ptr2 = point;

    ptr1->x = ptr2->x + base_x - ht_x;
    ptr1->y = ptr2->y + base_y - ht_y;
    ptr1++;
    ptr1->x = ptr2->x - base_x - ht_x;
    ptr1->y = ptr2->y - base_y - ht_y;
    ptr1++;
    ptr1->x = ptr2->x;
    ptr1->y = ptr2->y;

    polygon(vwk, triangle, 3);

    /* Adjust the end point and all points skipped. */
    ptr1 = point;
    ptr2 = xybeg;

    ptr1->x -= ht_x;
    ptr1->y -= ht_y;

    while ((ptr2 -= inc) != ptr1) {
        ptr2->x = ptr1->x;
        ptr2->y = ptr1->y;
    }
}



/*
 * arrow - Draw an arrow
 *
 * Will alter the end of the line segment.
 */

static void arrow(Vwk * vwk, Point * point, int count)
{
    /* Set up the attribute environment. */
    s_fa_attr(vwk);

    /* beginning point is arrowed. */
    if (s_begsty & ARROWED) {
        draw_arrow(vwk, point, 1);
    }

    /* ending point is arrowed. */
    point += count - 1;
    if (s_endsty & ARROWED) {
        draw_arrow(vwk, point, -1);
    }

    /* Restore the attribute environment. */
    r_fa_attr(vwk);
}                               /* End "do_arrow". */



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

void abline (Vwk * vwk, Line * line)
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
    UWORD color;                /* color index */

#if 0
    if (line->y1 == line->y2) {
        kprintf("Y = %d, MODE = %d.\n", line->y1, vwk->wrt_mode);
        horzline(X1, line);
        return;
    }
#endif

    /* Make x axis always goind up */
    if (line->x2 < line->x1) {
        /* if delta x < 0 then draw from point 2 to 1 */
        x1 = line->x2;
        y1 = line->y2;
        x2 = line->x1;
        y2 = line->y1;
    } else {
        /* positive, start with first point */
        x1 = line->x1;
        y1 = line->y1;
        x2 = line->x2;
        y2 = line->y2;
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

    adr = get_start_addr(x1, y1);      /* init adress counter */
    msk = 0x8000 >> (x1&0xf);           /* initial bit position in WORD */
    linemask = LN_MASK;                 /* to avoid compiler warning */
    color = vwk->line_color;            /* initial load of color index */

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

            switch (vwk->wrt_mode) {
            case 3:              /* reverse transparent  */
                if (color & 0x0001) {
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
                if (color & 0x0001) {
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
            case 0:              /* rep */
                if (color & 0x0001) {
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
                else {
                    for (loopcnt=dx;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
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
            eps = -dy;
            e2 = 2*dy;

            switch (vwk->wrt_mode) {
            case 3:              /* reverse transparent */
                if (color & 0x0001) {
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
                if (color & 0x0001) {
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
                if (color & 0x0001) {
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
                }
                else {
                    for (loopcnt=dy;loopcnt >= 0;loopcnt--) {
                        linemask = linemask >> 15|linemask << 1;     /* get next bit of line style */
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
            }
        }
        adr+=2;
        color >>= 1;    /* shift color index: next plane */
    }
    LN_MASK = linemask;
}

