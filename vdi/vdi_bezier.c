/*
 * bezier.c - Fast Bezier approximation using four control points.
 *
 * Copyright 1998-2002, Trevor Blight
 * Copyright 2004, EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "tosvars.h"
#include "lineavars.h"
#include "vdi_defs.h"
#include "biosbind.h"
#include "asm.h"        /* for malloc */
#include "kprint.h"


#if HAVE_BEZIER

/*
 * We do not yet use dynamic memory allocation!
 *
 * We conform to the PC-GEM/3 file standard with the inclusion of
 * bezier curve rendering capability with the v_bez() and v_bez_fill()
 * calls. v_bez_on() must be used to allow us to allocate the memory
 * necessary for bezier rendering. Likewise v_bez_off() should be used
 * before an application exits to free any memory used for bezier calls.
 */

#define MIN_QUAL 0
#define IS_BEZ(f) ((f&1)!=0)
#define IS_JUMP(f) ((f&2)!=0)



/*
 * labs - absulute for LONG
 */
LONG
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
         WORD * pmin, WORD * pmax, WORD rc_flag)
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

    if (!rc_flag && q >= 3) {
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
        if( x < *pmin )
            *pmin = x;
        if( x > *pmax )
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
    } /* for */

    /** add the last point .. */
    *px = x = array[6];
    if( x < *pmin )
        *pmin = x;
    if( x > *pmax )
        *pmax = x;
}



/*
 * draw_segs - draw segments in xptsin array
 *
 * do piece-wise if input array is too big
 * not guaranteed to work if FILL, but still better than anything else
 */

/* mode values for draw_segs() */
#define FILL    9	/* v_pline() */
#define NO_FILL 6	/* v_fill_area() */

static void
draw_segs(Vwk * vwk, WORD nr_vertices, WORD * xptsin, WORD mode)
{
    //WORD * ptsin_sav = PTSIN;

    //assert( mode == FILL || mode == NO_FILL );
    while( nr_vertices >= 2 ) {
        CONTRL[0] = mode;        /* opcode for v_pline/v_fill_area */
        CONTRL[2] = 0;           /* entries in intin[] */
        CONTRL[1] = nr_vertices > INQ_TAB[14] ? INQ_TAB[14]: nr_vertices; /* entries in ptsin[] */

        /* output to driver, converting ndc if necessary */
        if (mode == FILL)
            v_fillarea(vwk);
        else
            draw_pline(vwk);

        if( nr_vertices > INQ_TAB[14] ) {
            // FIXME??
            PTSIN += 2 * (INQ_TAB[14]-1); /* include end point in next call */
        }
        nr_vertices -= INQ_TAB[14]-1;
    }
    //PTSIN = ptsin_sav;          // really needed?
}


/*
 * v_bez - draw a bezier curve
 */
void
v_bez(Vwk * vwk)
{
    int i;
    WORD  const nr_ptsin = CONTRL[1];
    UBYTE * bezarr = (UBYTE*)INTIN;
    WORD bez_qual;
    WORD xmin, xmax, ymin, ymax;
    WORD total_vertices = nr_ptsin;
    WORD total_jumps = 0;
    UWORD vertices_per_bez;
    WORD *ptsin = PTSIN;
    UWORD xptsin[256];

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
            gen_segs(ptsin, xptsin, bez_qual, &xmin, &xmax, vwk->xfm_mode);	/* x coords */
            gen_segs(ptsin+1, xptsin+1, bez_qual, &ymin, &ymax, vwk->xfm_mode);	/* y coords */
            i += 3;	/* skip to coord pairs at end of bez curve */
            ptsin += 6;
            total_vertices += vertices_per_bez-3;
            draw_segs(vwk, vertices_per_bez+1, xptsin, NO_FILL );
        }
        else {
            /* polyline */
            WORD output_vertices = 0;
            WORD *const ptsin0 = ptsin;
            do {
                int t;

                t = ptsin[0];
                if( t < xmin )
                    xmin = t;
                if( t > xmax )
                    xmax = t;
                t = ptsin[1];
                if( t < ymin )
                    ymin = t;
                if( t > ymax )
                    ymax = t;
                output_vertices++;
                if( IS_BEZ(flag) )
                    break;

                /* continue polyline, stop if a jump point is next */
                i++;
                if (i >= nr_ptsin)
                    break;
                ptsin += 2;
                {
                    int old_flag = flag;
                    flag = bezarr[i^1];
                    if (!IS_JUMP(old_flag) && IS_JUMP(flag))
                        total_jumps++;   /* count jump point */
                }
            } while( !IS_JUMP(flag) );
            draw_segs(vwk, output_vertices, ptsin0, NO_FILL);
        }

    }

    INTOUT[0] = total_vertices; /* total nr points */
    INTOUT[1] = total_jumps;    /* total moves */
    PTSOUT[0] = xmin;
    PTSOUT[1] = ymin;
    PTSOUT[2] = xmax;
    PTSOUT[3] = ymax;

    return;
}


/*
 * v_bez_fill - draw a filled bezier curve
 */
void
v_bez_fill(Vwk * vwk)
{
    WORD  const nr_ptsin = CONTRL[1];
    const UBYTE *const bezarr = (UBYTE*)INTIN;	/* index with xor 1 to byte swap !! */
    WORD bez_qual;
    WORD xmin, xmax, ymin, ymax;
    WORD total_vertices = nr_ptsin;
    WORD total_jumps = 0;
    WORD vertices_per_bez;
    WORD i, i0;
    WORD *ptsin;
    WORD *ptsin0;
    WORD *xptsin;
    //WORD *xptsin0;
    WORD xptsin0[256];
    WORD nr_bez;
    UWORD output_vertices = 0;

    //kprintf("called v_bez_fill...\n");

    bez_qual = vwk->bez_qual;
    vertices_per_bez = 1 << bez_qual;
    xmin = ymin = 32767;
    xmax = ymax = 0;

    xptsin = xptsin0;

    nr_bez = 0;
    i = i0 = 0;
    ptsin0 = ptsin = PTSIN;
    while( i<nr_ptsin ) {
        int flag = bezarr[i^1];

        if( IS_BEZ( flag ) ) {
            if( i+3 >= nr_ptsin ) break;   /* incomlete curve, omit it */

            if( IS_JUMP(flag) ) total_jumps++;   /* count jump point */

            /* keep this curve within nr vertices for the driver's ptsin[]
             ** with one spare for the end point */
            if( output_vertices+vertices_per_bez+1 > INQ_TAB[14] ) {
                if( bez_qual > MIN_QUAL ) {
                    /* try reduce bezier quality & start this polygon again */
                    bez_qual--;
                    i = i0;
                    ptsin = ptsin0;
                    xptsin = xptsin0;
                    output_vertices = 0;
                    continue;
                }
                /* too bad if we get here. refuse to add vertices to output */
            }
            else {
                if( i!=i0 ) {
                    /* the end point will be copied in again */
                    xptsin -= 2;
                    output_vertices--;
                } /* if */

                output_vertices += vertices_per_bez+1;
                total_vertices += vertices_per_bez-3;
                gen_segs( ptsin, xptsin, bez_qual, &xmin, &xmax, vwk->xfm_mode);	/* x coords */
                gen_segs( ptsin+1, xptsin+1, bez_qual, &ymin, &ymax, vwk->xfm_mode);	/* y coords */
                xptsin = xptsin0 + 2*output_vertices;
            } /* if */
            //assert( PTSIN + 2*i == ptsin );
            i+=3;
            ptsin += 6;
            flag = bezarr[i^1];
        }
        else {     /** polyline **/

            if( i!=i0 ) {
                /* the end point will be copied in again */
                xptsin -= 2;
                output_vertices--;
            } /* if */

            do {
                int t;
                if( output_vertices < INQ_TAB[14] ) {	/* need room for at least one more */
                    t = ptsin[0];
                    if( t < xmin ) xmin = t;
                    if( t > xmax ) xmax = t;
                    *xptsin++ = t;
                    t = ptsin[1];
                    if( t < ymin ) ymin = t;
                    if( t > ymax ) ymax = t;
                    *xptsin++ = t;
                    output_vertices++;
                }

                //assert( xptsin0 + 2*output_vertices == xptsin );
                //assert( PTSIN + 2*i == ptsin );
                if( IS_BEZ(flag) )
                    break;
                i++;
                ptsin += 2;
                if( i>=nr_ptsin )
                    break;
                /* continue polyline, stop if a jump point is next */
                {
                    int old_flag = flag;
                    flag = bezarr[i^1];
                    if( !IS_JUMP(old_flag) && IS_JUMP(flag) ) total_jumps++;   /* count jump point */
                }
            } while( !IS_JUMP(flag) );
        }

        if( i>=nr_ptsin || IS_JUMP(flag) ) {
            draw_segs(vwk, output_vertices, xptsin0, FILL);
            bez_qual = vwk->bez_qual;
            i0 = i; ptsin0 = ptsin; xptsin = xptsin0; output_vertices = 0;
        }
    }

    INTOUT[0] = total_vertices; /* total nr points */
    INTOUT[1] = total_jumps;    /* total moves */
    PTSOUT[0] = xmin;
    PTSOUT[1] = ymin;
    PTSOUT[2] = xmax;
    PTSOUT[3] = ymax;

    return;

}

/********************* end of bezier.c *********************/




/*
 * v_bez_control - implement vdi function v_bez_control
 *
 * just return bez quality to indicate bezier functions are available
 */
void
v_bez_control(Vwk * vwk)
{
    //kprintf("called v_bez_control...\n");
    INTOUT[0] = vwk->bez_qual;
    CONTRL[4] = 1;
}


/*
 * v_bez_qual -  set bezier quality
 *
 * note: bez_qual > 7 will cause overflow in gen_segs()
 */
#define MIN_QUAL 0
static const WORD pcarr[] = {0, 10, 23, 39, 55, 71, 86, 100};
void
v_bez_qual(Vwk * vwk)
{
    int q = INTIN[2];
    //kprintf("called v_bez_qual...\n");
    if( q >= 95 )
        q = 7;
    else if( q<5 )
        q = MIN_QUAL;
    else
        q = (q>>4) + 1;

    vwk->bez_qual = q;
    INTOUT[0] = pcarr[q - MIN_QUAL];
    CONTRL[4] = 1;
}

#endif

