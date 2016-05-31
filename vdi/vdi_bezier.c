/*
 * bezier.c - Fast Bézier approximation using four control points.
 *
 * Copyright 1998-2002, Trevor Blight
 * Copyright 2004-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "vdi_defs.h"
#include "biosbind.h"
#include "asm.h"        /* for malloc */
//#include "kprint.h"


#if HAVE_BEZIER

/*
 * We conform to the PC-GEM/3 file standard with the inclusion of bezier
 * curve rendering capability with the v_bez() and v_bez_fill() calls.
 *
 * In 2D geometry, a vertex is a corner of a polygon (where two sides meet)
 *
 * Quadratic Bézier curves
 *
 * A quadratic Bézier curve is the path traced by the function P(t). For
 * points A, B, and C.
 *
 * Points on a quadratic Bézier curve can be computed recursively:
 *
 * 1. Let A, B and C be the startpoint, control point and endpoint of the curve.
 * 2. Let D be the midpoint of the line AB.
 * 3. Let E be the midpoint of the line BC.
 * 4. Let F be the midpoint of the line DE. F is a point on the curve.
 * 5. Recurse with A = A, B = D and C = F.
 * 6. Recurse with A = F, B = E and C = C.
 *
 * You can easily try this out on a piece of paper.
 */

#define MIN_QUAL 0
#define IS_BEZ(f) ((f&1)!=0)
#define IS_JUMP(f) ((f&2)!=0)



/*
 * labs - absolute for LONG
 */
static LONG
labs(LONG x)
{
    if (x < 0)
        return(-x);
    else
        return(x);
}



/*
 * gen_segs -  compute bezier function by difference method
 *
 * last point is included
 * one dimension only, so use alternate elements of array & px
 * array[0] : anchor 1
 * array[2] : control 1
 * array[4] : control 2
 * array[6] : anchor 2
 */
static void
gen_segs(WORD *const array, WORD *px, const int bez_qual,
         WORD * pmin, WORD * pmax, WORD fill)
{
    LONG d3x, d2x, d1x;
    LONG x0;
    int q = 3 * bez_qual;
    int qd = 0;
    int i;
    WORD x;

    /*** calculate 1st, 2nd & 3rd order differences ***/
    d1x = (LONG)array[2]-array[0];
    d2x = (LONG)array[4]-array[2];
    d3x = -array[0] - 3*d2x + array[6];
    d2x -= d1x;

    if (!fill && q >= 3) {
        d1x>>=1;
        d2x>>=1;
        d3x>>=1;
        q--;
    }

    d1x = ((3L * d1x) << (2 * bez_qual)) + ((3L * d2x) << bez_qual) + d3x;
    /* assert( d1x <=  0x5f408000 ); */
    /* assert( d1x >= -0x5f408000 ); */

    d3x = 6L*d3x;
    //assert( d3x <=  0xbffe8 );
    //assert( d3x >= -0xbffe8 );

    d2x = ((6L*d2x)<<bez_qual) + d3x;
    /* assert( d2x <=  0x2f6fa12 ); */
    /* assert( d2x >= -0x2f6fa12 ); */

    x0 = labs(array[0]);
    while( x0 >= (0x7fffffffL>>q) )
        q--, qd++;

    x0 = (((LONG)array[0]) << q) + (1L << (q - 1));

    for(i = 1 << bez_qual; i > 0; i--) {
        x = (WORD)(x0 >> q);
        *px = x;
        if ( x < *pmin )
            *pmin = x;
        if ( x > *pmax )
            *pmax = x;
        px+=2;

        if (labs( (x0 >> 1) + (d1x >> (qd + 1)) ) >= 0x3ffffffeL) {
            /** halve scale to avoid overflow **/
            x0 = x0 >> 1;
            q--;
            qd++;
            //assert( labs(x0+(d1x>>qd)) >= 0x40000000L );
        }

        x0 += d1x >> qd;
        if ( qd > 0 && labs(x0) < 0x40000000L ) {
            /** double scale to maximise accuracy **/
            x0 = (x0 << 1)|1;
            q++, qd--;
        }

        //assert( d2x<0 || d1x <=  0x7fffffff-d2x );
        //assert( d2x>0 || d1x >= -0x7fffffff-d2x );
        d1x += d2x;
        d2x += d3x;
        //assert( d2x <=  0x30bf9e8 );
        //assert( d2x >= -0x30bf9e8 );
    }

    /** add the last point .. */
    *px = x = array[6];
    if ( x < *pmin )
        *pmin = x;
    if ( x > *pmax )
        *pmax = x;
}



/*
 * draw_segs - draw segments in xptsin array
 *
 * do piece-wise if input array is too big
 * not guaranteed to work if FILL, but still better than anything else
 */

/* mode values for draw_segs() */
#define FILL    9       /* v_pline() */
#define NO_FILL 6       /* v_fill_area() */

static void
draw_segs(Vwk * vwk, WORD nr_vertices, Point * point, WORD mode)
{
    if ( nr_vertices >= 2 ) {
        if (nr_vertices > INQ_TAB[14])
            nr_vertices = INQ_TAB[14];

        /* output to driver, converting ndc if necessary */
        if (mode == FILL) {
            polygon(vwk, point, nr_vertices);
        }
        else
        {
            short i;
            Line line;

            LSTLIN = FALSE;
            for(i = nr_vertices - 1; i > 0; i--) {
                line.x1 = point->x;
                line.y1 = point->y;
                point++;                /* advance point by point */
                line.x2 = point->x;
                line.y2 = point->y;

                if (!vwk->clip || clip_line(vwk, &line))
                    abline(&line, vwk->wrt_mode, vwk->line_color);
            }
        }
    }
}


/*
 * v_bez - draw a bezier curve
 *
 * outputs a (possibly disjoint) series of bezier curves & polylines.
 *
 * Each element in bezarr[] is a flag that controls the behaviour of the
 * corresponding input point.
 *
 * If bit 0 (ie ls bit) of the flag is set to one, the corresponding
 * point and the next three points define a bezier curve:
 *         1. start point
 *         2. 1st control point
 *         3. 2nd control point
 *         4. end point
 *
 * If bit 0 is zero, the corresponding point is part of a polyline.
 *
 * If bit 1 is set, the corresponding point starts a new disconnected
 * bezier curve.
 *
 * Note: The C function calls are as described here, but internally the C
 * libraries byte swap bezarr[] for intel compatible format.
 * If you are not using the C library, but directly programming the VDI
 * interface, you need to do the byte swapping yourself.
 */

void
v_bez(Vwk * vwk, Point * ptsget, int nr_ptsin)
{
    int i;
    //WORD  const nr_ptsin = CONTRL[1];
    UBYTE * bezarr = (UBYTE*)INTIN;
    WORD bez_qual;
    WORD xmin, xmax, ymin, ymax;
    WORD total_vertices = nr_ptsin;
    WORD total_jumps = 0;
    UWORD vertices_per_bez;
    Point ptsbuf[MAX_PTSIN];
    //Point * ptsget = (Point*)PTSIN;
    Point * ptsput = ptsbuf;

    bez_qual = vwk->bez_qual;
    vertices_per_bez = 1 << bez_qual;
    xmin = ymin = 32767;
    xmax = ymax = 0;

    i = 0;
    while(i < nr_ptsin) {
        int flag = bezarr[i^1];         /* index with xor 1 to byte swap !! */

        if (IS_BEZ(flag)) {
            /* bezier start point found */
            if (i+3 >= nr_ptsin)
                break;                  /* incomlete curve, omit it */

            if (IS_JUMP(flag))
                total_jumps++;          /* count jump point */

            /* generate line segments from bez points */
            gen_segs(&ptsget->x, &ptsput->x, bez_qual, &xmin, &xmax, vwk->xfm_mode);    /* x coords */
            gen_segs(&ptsget->y, &ptsput->y, bez_qual, &ymin, &ymax, vwk->xfm_mode);    /* y coords */

            /* skip to coord pairs at end of bez curve */
            i += 3;
            ptsget += 3;
            total_vertices += vertices_per_bez-3;
            draw_segs(vwk, vertices_per_bez+1, ptsbuf, NO_FILL );
        }
        else {
            /* polyline */
            WORD output_vertices = 0;
            Point * point = ptsget;
            do {
                int t;

                t = point->x;
                if ( t < xmin )
                    xmin = t;
                if ( t > xmax )
                    xmax = t;

                t = point->y;
                if ( t < ymin )
                    ymin = t;
                if ( t > ymax )
                    ymax = t;

                output_vertices++;
                if ( IS_BEZ(flag) )
                    break;              /* stop if a jump point is next */

                /* continue polyline */
                i++;
                if (i >= nr_ptsin)
                    break;

                ptsget += 1;
                {
                    int old_flag = flag;
                    flag = bezarr[i^1];
                    if (!IS_JUMP(old_flag) && IS_JUMP(flag))
                        total_jumps++;   /* count jump point */
                }
            } while( !IS_JUMP(flag) );
            draw_segs(vwk, output_vertices, point, NO_FILL);
        }
    }

    INTOUT[0] = total_vertices; /* total nr points */
    INTOUT[1] = total_jumps;    /* total moves */
    CONTRL[4] = 2;
    CONTRL[2] = 2;
    PTSOUT[0] = xmin;
    PTSOUT[1] = ymin;
    PTSOUT[2] = xmax;
    PTSOUT[3] = ymax;

    return;
}


/*
 * v_bez_fill - draw a filled bezier curve
 *
 * It is similar to v_bez(), but it forms a closed contour and fills
 * it with the current fill pattern.
 */
#if 1
void
v_bez_fill(Vwk * vwk, Point * ptsget, int nr_ptsin)
{
    int i;
    //WORD  const nr_ptsin = CONTRL[1];
    UBYTE * bezarr = (UBYTE*)INTIN;
    WORD bez_qual;
    WORD xmin, xmax, ymin, ymax;
    WORD total_vertices = nr_ptsin;
    WORD total_jumps = 0;
    UWORD vertices_per_bez;
    WORD output_vertices = 0;
    Point ptsbuf[MAX_PTSIN];
    //Point * ptsget = (Point*)PTSIN;
    Point * ptsput = ptsbuf;

    bez_qual = vwk->bez_qual;
    vertices_per_bez = 1 << bez_qual;
    xmin = ymin = 32767;
    xmax = ymax = 0;

    i = 0;
    while(i < nr_ptsin) {
        int flag = bezarr[i^1]; /* index with xor 1 to byte swap !! */

        if (IS_BEZ(flag)) {
            /* bezier start point found */
            if (i+3 >= nr_ptsin)
                break;

            if (IS_JUMP(flag))
                total_jumps++;   /* count jump point */

            /* generate line segments from bez points */
            gen_segs(&ptsget->x, &ptsput->x, bez_qual, &xmin, &xmax, vwk->xfm_mode);
            gen_segs(&ptsget->y, &ptsput->y, bez_qual, &ymin, &ymax, vwk->xfm_mode);

            /* skip to coord pairs at end of bez curve */
            i += 3;
            ptsget += 3;
            total_vertices += vertices_per_bez-3;

            output_vertices += vertices_per_bez+1;
            ptsput = ptsbuf + output_vertices;
            //draw_segs(vwk, vertices_per_bez+1, ptsbuf, FILL );
        }
        else {
            /* polyline */
            //WORD output_vertices = 0;
            Point * point = ptsget;
            do {
                int t;

                t = point->x;
                if ( t < xmin )
                    xmin = t;
                if ( t > xmax )
                    xmax = t;

                t = point->y;
                if ( t < ymin )
                    ymin = t;
                if ( t > ymax )
                    ymax = t;

                output_vertices++;
                if ( IS_BEZ(flag) )
                    break;              /* stop if a jump point is next */

                /* continue polyline */
                i++;
                if (i >= nr_ptsin)
                    break;

                ptsget += 1;
                {
                    int old_flag = flag;
                    flag = bezarr[i^1];
                    if (!IS_JUMP(old_flag) && IS_JUMP(flag))
                        total_jumps++;   /* count jump point */
                }
            } while( !IS_JUMP(flag) );
        }

        /* draw segments and reset all vertex information */
        draw_segs(vwk, output_vertices, ptsbuf, FILL);
        bez_qual = vwk->bez_qual;
        //ptsget0 = ptsget;
        ptsput = ptsbuf;
        output_vertices = 0;
    }

    INTOUT[0] = total_vertices; /* total nr points */
    INTOUT[1] = total_jumps;    /* total moves */
    CONTRL[4] = 2;
    CONTRL[2] = 2;
    PTSOUT[0] = xmin;
    PTSOUT[1] = ymin;
    PTSOUT[2] = xmax;
    PTSOUT[3] = ymax;

    return;
}

#else

void
v_bez_fill(Vwk * vwk, Point * ptsget, int nr_ptsin)
{
    //WORD  const nr_ptsin = CONTRL[1];
    const UBYTE *const bezarr = (UBYTE*)INTIN;  /* index with xor 1 to byte swap !! */
    WORD bez_qual;
    WORD xmin, xmax, ymin, ymax;
    WORD total_vertices = nr_ptsin;
    WORD total_jumps = 0;
    WORD vertices_per_bez;
    WORD i, i0;
    WORD output_vertices = 0;
    Point ptsbuf[MAX_PTSIN];
    //Point * ptsget = (Point*)PTSIN;
    Point * ptsget0 = ptsget;
    Point * ptsput = ptsbuf;

    bez_qual = vwk->bez_qual;
    vertices_per_bez = 1 << bez_qual;
    xmin = ymin = 32767;
    xmax = ymax = 0;

    i = i0 = 0;
    while (i < nr_ptsin) {
        int flag = bezarr[i^1];

        if (IS_BEZ( flag )) {
            if (i+3 >= nr_ptsin)
                break;   /* incomplete curve, omit it */

            if (IS_JUMP(flag))
                total_jumps++;   /* count jump point */

            /* keep this curve within nr vertices for the driver's ptsget[]
             ** with one spare for the end point */
            if ( output_vertices+vertices_per_bez+1 > INQ_TAB[14] ) {
                if ( bez_qual > MIN_QUAL ) {
                    /* reduce bezier quality & start this polygon again */
                    bez_qual--;
                    i = i0;
                    ptsget = (Point*)PTSIN;
                    ptsget0 = ptsget;
                    ptsput = ptsbuf;
                    output_vertices = 0;
                    continue;
                }
                /* too bad if we get here. refuse to add vertices to output */
            }
            else {
                if ( i != i0 ) {
                    /* the end point will be copied in again */
                    ptsput -= 1;
                    output_vertices--;
                }

                output_vertices += vertices_per_bez+1;
                total_vertices += vertices_per_bez-3;
                gen_segs(&ptsget->x, &ptsput->x, bez_qual, &xmin, &xmax, vwk->xfm_mode);        /* x coords */
                gen_segs(&ptsget->y, &ptsput->y, bez_qual, &ymin, &ymax, vwk->xfm_mode);        /* y coords */
                ptsput = ptsbuf + output_vertices;
            }
            //assert( PTSIN + 2*i == ptsget );
            i+=3;
            ptsget += 3;
            flag = bezarr[i^1];
        }
        else {
            /** polyline **/
            if (i != i0) {
                /* the end point will be copied in again */
                ptsput -= 1;
                output_vertices--;
            }

            do {
                int t;
                if ( output_vertices < INQ_TAB[14] ) {  /* need room for at least one more */
                    t = ptsget->x;
                    if ( t < xmin )
                        xmin = t;
                    if ( t > xmax )
                        xmax = t;
                    ptsput->x = t;

                    t = ptsget->y;
                    if ( t < ymin )
                        ymin = t;
                    if ( t > ymax )
                        ymax = t;
                    ptsput->y = t;

                    ptsput++;
                    output_vertices++;
                }

                //assert( ptsbuf + output_vertices == ptsput );
                //assert( PTSIN + 2*i == ptsget );
                if ( IS_BEZ(flag) )
                    break;
                i++;
                if (i >= nr_ptsin)
                    break;

                ptsget += 1;
                /* continue polyline, stop if a jump point is next */
                {
                    int old_flag = flag;
                    flag = bezarr[i^1];
                    if (!IS_JUMP(old_flag) && IS_JUMP(flag))
                        total_jumps++;   /* count jump point */
                }
            } while( !IS_JUMP(flag) );
        }

        if ( i >= nr_ptsin || IS_JUMP(flag) ) {
            draw_segs(vwk, output_vertices, ptsbuf, FILL);
            bez_qual = vwk->bez_qual;
            i0 = i;
            ptsget0 = ptsget;
            ptsput = ptsbuf;
            output_vertices = 0;
        }
    }

    INTOUT[0] = total_vertices; /* total nr points */
    INTOUT[1] = total_jumps;    /* total moves */
    CONTRL[4] = 2;
    CONTRL[2] = 2;
    PTSOUT[0] = xmin;
    PTSOUT[1] = ymin;
    PTSOUT[2] = xmax;
    PTSOUT[3] = ymax;

    return;

}
#endif


/*
 * v_bez_control - implement vdi function v_bez_control
 *
 * simply returns the bezier quality set for the current workstation.
 * It can be used by your application to test if the GDOS supports
 * bezier functions. We do not support the use of this function to
 * enable or disable bezier curves.
 */

void
v_bez_control(Vwk * vwk)
{
    INTOUT[0] = vwk->bez_qual;
    CONTRL[4] = 1;
}


/*
 * v_bez_qual -  set bezier quality
 *
 * sets the quality of a bezier curve.  A bezier curve is generated
 * as a series of short straight line segments.  A high quality
 * bezier curve is made from many short straight lines, whereas a
 * lower quality bezier curve has fewer longer straight line segments.
 * Higher quality bezier curves thus appear smoother, but are slower.
 *
 * note: bez_qual > 7 will cause overflow in gen_segs()
 */
#define MIN_QUAL 0
static const WORD pcarr[] = {0, 10, 23, 39, 55, 71, 86, 100};
void
v_bez_qual(Vwk * vwk)
{
    int q = INTIN[2];
    if ( q >= 95 )
        q = 7;
    else if ( q<5 )
        q = MIN_QUAL;
    else
        q = (q>>4) + 1;

    vwk->bez_qual = q;
    INTOUT[0] = pcarr[q - MIN_QUAL];
    CONTRL[4] = 1;
}

#endif
