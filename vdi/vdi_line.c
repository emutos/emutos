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


/* the six predefined line styles */
UWORD LINE_STYLE[6] = { 0xFFFF, 0xFFF0, 0xC0C0, 0xFF18, 0xFF00, 0xF191 };


/* Wide line attribute save areas */
WORD s_begsty, s_endsty, s_fil_col, s_fill_per;
WORD *s_patptr;

/* ST_UD_LINE_STYLE: */
void vsl_udsty(Vwk * vwk)
{
    vwk->ud_ls = *INTIN;
}


/*
 * vsl_type - Set line style for line-drawing functions
 */

void vsl_type(Vwk * vwk)
{
    WORD li;

    CONTRL[4] = 1;

    li = (*INTIN - 1);
    if ((li >= MX_LN_STYLE) || (li < 0))
        li = 0;

    *INTOUT = (vwk->line_index = li) + 1;
}



/*
 * vsl_width - Set line width
 */

void vsl_width(Vwk * vwk)
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
 * vsl_ends - sets the style of end point for line starting and ending points
 */

void vsl_ends(Vwk * vwk)
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
 * vsl_color - sets the color for line-drawing
 */

void vsl_color(Vwk * vwk)
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
 * hzline_rep - draw a horizontal line in replace mode
 *
 * This routine is used by habline() and rectfill()
 */

void hzline_rep(Vwk * vwk, UWORD *addr, int dx, int leftpart,
                UWORD rightmask, UWORD leftmask, WORD patind)
{
    int planes;
    int plane;
    WORD *color;
    int patadd;                         /* advance for multiplane patterns */

    /* precalculate, what to draw */
    patadd = vwk->multifill ? 16 : 0;        /* multi plane pattern offset */
    color = &FG_BP_1;
    planes = v_planes;

    for (plane = planes-1; plane >= 0; plane-- ) {
        UWORD *adr;
        UWORD pattern;
        int pixels;                   /* counting down the rest of dx */
        int bw;

        adr = addr;
        pixels = dx-16;

        /* load values fresh for this bitplane */
        if (*color++)
            pattern = vwk->patptr[patind];
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
                bits = *adr;            /* get data from screen address */
                bits ^= pattern;        /* xor the pattern with the source */
                bits &= leftmask;       /* isolate the bits outside the fringe */
                bits ^= pattern;        /* restore the bits outside the fringe */
                *adr = bits;            /* write back the result */

                adr += planes;;
                pixels -= 16;
                pixels += leftpart;
            }
            /* Full WORDs */
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

void hzline_or(Vwk * vwk, UWORD *addr, int dx, int leftpart,
               UWORD rightmask, UWORD leftmask, int patind)
{
    WORD *color;
    int planes;
    int plane;
    int patadd;               /* advance for multiplane patterns */

    /* init start values */
    planes = v_planes;
    patadd = vwk->multifill ? 16 : 0;     /* multi plane pattern offset */
    color = &FG_BP_1;

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        UWORD *adr;
        UWORD pattern;
        int pixels;                   /* counting down the rest of dx */
        int bw;

        /* load values fresh for this bitplane */
        adr = addr;
        pixels = dx-16;
        pattern = vwk->patptr[patind];

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

void hzline_xor(Vwk * vwk, UWORD *addr, int dx, int leftpart,
                UWORD rightmask, UWORD leftmask, int patind)
{
    int planes;
    int plane;
    int patadd;               /* advance for multiplane patterns */

    /* init adress counter */
    planes = v_planes;
    patadd = vwk->multifill ? 16 : 0;     /* multi plane pattern offset */

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        UWORD *adr;
        UWORD pattern;
        int pixels;                   /* counting down the rest of dx */
        int bw;

        /* load values fresh for this bitplane */
        pattern = vwk->patptr[patind];
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

void hzline_nor(Vwk * vwk, UWORD *addr, int dx, int leftpart,
                UWORD rightmask, UWORD leftmask, int patind)
{
    WORD *color;
    int planes;
    int plane;
    int patadd;                 /* advance for multiplane patterns */

    /* init adress counter */
    planes = v_planes;
    patadd = vwk->multifill ? 16 : 0;     /* multi plane pattern offset */
    color = &FG_BP_1;

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        UWORD *adr;
        UWORD pattern;
        int pixels;             /* counting down the rest of dx */
        int bw;

        /* load values fresh for this bitplane */

        adr = addr;
        pixels = dx-16;
        pattern = vwk->patptr[patind];

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

void habline(Vwk * vwk) {
    horzline(vwk, X1, X2, Y1);
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

void horzline(Vwk * vwk, WORD x1, WORD x2, WORD y) {
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
    patind = vwk->patmsk & y;             /* which pattern to start with */
    patadd = vwk->multifill ? 16 : 0;     /* multi plane pattern offset */

    /* init adress counter */
    addr = v_bas_ad;                    /* start of screen */
    addr += (x1&0xfff0)>>shft_off;      /* add x coordinate part of addr */
    addr += (LONG)y * v_lin_wr;         /* add y coordinate part of addr */

    /* precalculate, what to draw */
    leftpart = x&0xf;
    rightpart = (x+dx)&0xf;
    leftmask = ~(0xffff>>leftpart);     /* origin for not left fringe lookup */
    rightmask = 0x7fff>>rightpart;      /* origin for right fringe lookup */

    switch (vwk->wrt_mode) {
    case 3:  /* nor */
        hzline_nor(vwk, addr, dx, leftpart, rightmask, leftmask, patind);
        break;
    case 2:  /* xor */
        hzline_xor(vwk, addr, dx, leftpart, rightpart, leftmask, patind);
        break;
    case 1:  /* or */
        hzline_or(vwk, addr, dx, leftpart, rightpart, leftmask, patind);
        break;
    default: /* rep */
        hzline_rep(vwk, addr, dx, leftpart, rightmask, leftmask, patind);
    }
}



/*
 * v_pline -
 */

void v_pline(Vwk * vwk)
{
    WORD l;

    l = vwk->line_index;
    LN_MASK = (l < 6) ? LINE_STYLE[l] : vwk->ud_ls;

    l = vwk->line_color;
    FG_BP_1 = (l & 1);
    FG_BP_2 = (l & 2);
    FG_BP_3 = (l & 4);
    FG_BP_4 = (l & 8);

    if (vwk->line_width == 1) {
        polyline(vwk);
        if ((vwk->line_beg | vwk->line_end) & ARROWED)
            draw_arrow(vwk);
    } else
        wideline(vwk);
}



/*
 * clip_code - helper function
 */

WORD clip_code(Vwk * vwk, WORD x, WORD y)
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

BOOL clip_line(Vwk * vwk)
{
    WORD deltax, deltay, x1y1_clip_flag, x2y2_clip_flag, line_clip_flag;
    WORD *x, *y;

    while ((x1y1_clip_flag = clip_code(vwk, X1, Y1)) |
           (x2y2_clip_flag = clip_code(vwk, X2, Y2))) {
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
            *y = Y1 + mul_div(deltay, (vwk->xmn_clip - X1), deltax);
            *x = vwk->xmn_clip;
        } else if (line_clip_flag & 2) {        /* right ? */
            *y = Y1 + mul_div(deltay, (vwk->xmx_clip - X1), deltax);
            *x = vwk->xmx_clip;
        } else if (line_clip_flag & 4) {        /* top ? */
            *x = X1 + mul_div(deltax, (vwk->ymn_clip - Y1), deltay);
            *y = vwk->ymn_clip;
        } else if (line_clip_flag & 8) {        /* bottom ? */
            *x = X1 + mul_div(deltax, (vwk->ymx_clip - Y1), deltay);
            *y = vwk->ymx_clip;
        }
    }
    return (TRUE);              /* segment now clipped  */
}


/*
 * pline - draw a poly-line
 */

void polyline(Vwk * vwk)
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
        if (!vwk->clip || clip_line(vwk))
            abline(vwk);
    }
}



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
 * perp_off -
 */

void perp_off(WORD * px, WORD * py)
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
}



/*
 * cir_dda - Used in wideline()
 */

void cir_dda(Vwk * vwk)
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

void do_circ(Vwk * vwk, WORD cx, WORD cy)
{
    WORD k;
    WORD *pointer;

    /* Only perform the act if the circle has radius. */
    if (num_qc_lines > 0) {
        /* Do the horizontal line through the center of the circle. */

        pointer = q_circle;
        X1 = cx - *pointer;
        X2 = cx + *pointer;
        Y1 = Y2 = cy;
        if (clip_line(vwk))
            abline(vwk);

        /* Do the upper and lower semi-circles. */

        for (k = 1; k < num_qc_lines; k++) {
            /* Upper semi-circle. */

            pointer = &q_circle[k];
            X1 = cx - *pointer;
            X2 = cx + *pointer;
            Y1 = Y2 = cy - k;
            if (clip_line(vwk)) {
                abline(vwk);
                pointer = &q_circle[k];
            }

            /* Lower semi-circle. */

            X1 = cx - *pointer;
            X2 = cx + *pointer;
            Y1 = Y2 = cy + k;
            if (clip_line(vwk))
                abline(vwk);
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
    vwk->fill_color = vwk->line_color;
    s_fill_per = vwk->fill_per;
    vwk->fill_per = TRUE;
    vwk->patptr = &SOLID;
    vwk->patmsk = 0;
    s_begsty = vwk->line_beg;
    s_endsty = vwk->line_end;
    vwk->line_beg = SQUARED;
    vwk->line_end = SQUARED;
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

void wideline(Vwk * vwk)
{
    WORD i, k, box[10];         /* box two high to close polygon */
    WORD numpts, wx1, wy1, wx2, wy2, vx, vy;
    WORD *old_ptsin, *src_ptr;
    WORD *pointer, x, y, d, d2;

    /* Don't attempt wide lining on a degenerate polyline */
    if ((numpts = *(CONTRL + 1)) < 2)
        return;

    if (vwk->line_width != line_cw) {
        cir_dda(vwk);
    }

    /* If the ends are arrowed, output them. */
    if ((vwk->line_beg | vwk->line_end) & ARROWED)
        draw_arrow(vwk);
    s_fa_attr(vwk);

    /* Initialize the starting point for the loop. */
    old_ptsin = pointer = PTSIN;
    wx1 = *pointer++;
    wy1 = *pointer++;
    src_ptr = pointer;

    /* If the end style for the first point is not squared, output a circle. */
    if (s_begsty != SQUARED) {
        do_circ(vwk, wx1, wy1);
    }

    /* Loop over the number of points passed in. */
    for (i = 1; i < numpts; i++) {
        /* Get ending point for line segment */
        pointer = src_ptr;
        wx2 = *pointer++;
        wy2 = *pointer++;
        src_ptr = pointer;

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

        polygon(vwk);

        /* restore the PTSIN pointer */
        PTSIN = old_ptsin;

        /* If the terminal point of the line segment is an internal joint, */
        /* output a filled circle.                                         */
        if ((i < numpts - 1) || (s_endsty != SQUARED))
            do_circ(vwk, wx2, wy2);
        /* end point becomes the starting point for the next line segment. */
        wx1 = wx2;
        wy1 = wy2;
    } /* End for:  over number of points. */

    /* Restore the attribute environment. */
    r_fa_attr(vwk);
} /* End "wline". */



/*
 * do_arrow - Draw an arrow
 */

void draw_arrow(Vwk * vwk)
{
    WORD x_start, y_start;
    WORD new_x_start, new_y_start;
    WORD *pts_in;

    /* Set up the attribute environment. */
    s_fa_attr(vwk);

    /* Function "arrow" will alter the end of the line segment.  Save the */
    /* starting point of the polyline in case two calls to "arrow" are    */
    /* necessary.                                                         */
    pts_in = PTSIN;
    new_x_start = x_start = *pts_in;
    new_y_start = y_start = *(pts_in + 1);

    if (s_begsty & ARROWED) {
        arrow(vwk, pts_in, 2);
        pts_in = PTSIN;         /* arrow calls plygn which trashes regs */
        new_x_start = *pts_in;
        new_y_start = *(pts_in + 1);
    }
    /* End if:  beginning point is arrowed. */
    if (s_endsty & ARROWED) {
        *pts_in = x_start;
        *(pts_in + 1) = y_start;
        arrow(vwk, (pts_in + 2 ** (CONTRL + 1) - 2), -2);
        pts_in = PTSIN;         /* arrow calls plygn which trashes regs */
        *pts_in = new_x_start;
        *(pts_in + 1) = new_y_start;
    }

    /* End if:  ending point is arrowed. */
    /* Restore the attribute environment. */
    r_fa_attr(vwk);
}                               /* End "do_arrow". */



/*
 * vec_len
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
 * arrow - Draw an arrow
 */

void arrow(Vwk * vwk, WORD * xy, WORD inc)
{
    WORD arrow_len, arrow_wid, line_len;
    WORD *xybeg, sav_contrl, triangle[8];       /* triangle 2 high to close
                                                   polygon */
    WORD dx, dy;
    WORD base_x, base_y, ht_x, ht_y;
    WORD *old_ptsin;
    WORD *ptr1, *ptr2, temp, i;

    line_len = dx = dy = 0;

    /* Set up the arrow-head length and width as a function of line width. */

    temp = vwk->line_width;
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
        dy = mul_div(*(ptr2 + 1) - *(ptr1 + 1), ysize, xsize);

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

    ht_x = mul_div(arrow_len, mul_div(dx, 1000, line_len), 1000);
    ht_y = mul_div(arrow_len, mul_div(dy, 1000, line_len), 1000);
    base_x = mul_div(arrow_wid, mul_div(dy, -1000, line_len), 1000);
    base_y = mul_div(arrow_wid, mul_div(dx, 1000, line_len), 1000);

    /* Transform the y offsets back to the correct aspect ratio space. */

    ht_y = mul_div(ht_y, xsize, ysize);
    base_y = mul_div(base_y, xsize, ysize);

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
    polygon(vwk);
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

void abline (Vwk * vwk)
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

#if 0
    if (Y1 == Y2) {
        kprintf("Y = %d, MODE = %d.\n", Y1, vwk->wrt_mode);
        //horzline(X1, X2, Y1);
        return;
    }
#endif

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

            switch (vwk->wrt_mode) {
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
            case 0:              /* rep */
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
                if (*color) {
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
        color++;
    }
    LN_MASK = linemask;
}



