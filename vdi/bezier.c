/*
 * bezier.c - Fast Bezier approximation using four control points.
 *
 * Copyright (C) 1976-1992 Digital Research Inc.
 *               1999 Caldera, Inc.
 *
 * Authors:
 *   William D. Herndon, CCP Software, 29.04.88
 *   AWIGHTMA
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
 *   $Log$
 *   Revision 1.2  2002/04/27 08:48:05  lvl
 *   make cvsready
 *
 *   Revision 1.1  2002/04/24 20:31:56  mdoering
 *   added raw bezier code from PC GEM's GDOS - not yet working
 *
 *   BEZIER.C 1.1 92/07/23 18:04:31 AWIGHTMA
 *   
 *   Convert MWC intrinsic functions _abs, _max and _min for BC++ 2.0 atw
 *   Eliminate warning messages on prototypes and ambiguity atw
 *    * 06/Jun/92       Pass in the GEM/DR_DESKTOP definition from makefile
 *    * 06/Apr/92       Converted to Turbo C
 *    * 08.08.88        Converted do_bez4 to recursion,
 *    *                 and commented out lmult and bshift, since not needed now. -CJLT
 *    * 15.7.88 Modified for GDOS use. - DAO
 *    * 24.12.87        Adapted for use in GEM screen/printer drivers
 *    *                 and lattice C, from Jeremy's version from 1.12.
 *    *         Lattice C won't eat parameter definitions in
 *    *                 function declarations and voids or unsigned
 *    *         longs anywhere at all. -WDH
 */



#define DBG_BEZIER 0

#include "portab.h"


#define BEZ 0

#if BEZ         /* USED to enable this code */

#define FP_SEG(fp) ( *(((unsigned *)&fp) + 1) )
#define FP_OFF(fp) ( *((unsigned *)&fp) )

#define BEZIER_START    0x01
#define POINT_MOVE      0x02

#define MIN_BEZIER_DEPTH        2       /*** CJLT  was 1 ***/
#define MAX_BEZIER_DEPTH        7

#define MIN_DEPTH_SCALE         9
#define MAX_DEPTH_SCALE         0

#define MINVERTSIN              129
#define MININTIN                56

extern  VOID    GEXT_DCALL(WORD *parmblock[5]);
extern  UWORD   GEXT_ALLMEM(UWORD npara, UWORD *segment);
extern  WORD    GEXT_RELMEM(UWORD segment);
extern  LONG    MULT(WORD a, WORD b);

/* To allow this code to be ported to Borland C++ from Metaware 'C'
   We must define these three functions which are intrinsic functions
   in Metaware 'C'. NOTE: _max and _min allow variable number of parameters.
   This may reduce performance but it does still allow the functionality
   to be tested in case it is ever needed again.
*/
#ifdef MWC              /* use intrinsic functions in Metaware */
#define _max4 _max
#define _max3 _max
#define _min4 _min
#define _min3 _min
#else           /* use functions for BCC20 as it seems that there is  */
                /* a limit to the complexity of definitions allowed   */
                /* and we get pretty efficient assembly code this way */
WORD    _abs(WORD x)
{
    if (x < 0)
        return(-x);
    else
        return(x);
}



WORD    _max(WORD x, WORD y)
{
    if (x < y)
        return(y);
    else
        return(x);
}



WORD    _max3(WORD x, WORD y, WORD z)
{
    if (x < y)
        x = y;
    if (x < z)
        return(z);
    else
        return(x);
}



WORD    _max4(WORD w, WORD x, WORD y, WORD z)
{
    if (w < x)
        w = x;
    if (w < y)
        w = y;
    if (w < z)
        return(z);
    else
        return(w);
}



WORD    _min3(WORD x, WORD y, WORD z)
{
    if (x > y)
        x = y;
    if (x > z)
        return(z);
    else
        return(x);
}



WORD    _min4(WORD w, WORD x, WORD y, WORD z)
{
    if (w > x)
        w = x;
    if (w > y)
        w = y;
    if (w > z)
        return(z);
    else
        return(w);
}
#endif

/*================================================================
        Internal static variables
================================================================*/

WORD    pnt_mv_cnt;
WORD    x_used;
WORD    *XPTS;
WORD    *XMOV;
WORD    depth_scale     = MAX_DEPTH_SCALE;
WORD    *appbuff        = NULLPTR;
UWORD   appsize         = 0;
WORD    *dosbuff        = NULLPTR;
UWORD   dossize         = 0;
WORD    max_dos_used    = 0;
WORD    bez_capable     = 0;
WORD    max_poly_points = 127;
WORD    is_metafile     = 0;

static  WORD    *CONTRL;
static  WORD    *INTIN;
static  WORD    *PTSIN;
static  WORD    *INTOUT;
static  WORD    *PTSOUT;
/**** New structures added by CLT *****/

struct coords {LONG x;LONG y;}; /* all co-ordinate pairs are LONGs */
struct node   {struct coords anchor,in,out; WORD onscreen,level;};
          /* "node" is a misnomer as each node is one section of Bezier.
             anchor is its end point. Its start point is the anchor from the
               next node (where next=this+1)
             in is the coordinates of the direction point in this section
               belonging to its anchor
             out is the direction point in this section of the next anchor
             onscreen is a flag. TRUE if Bezier entirely onscreen/page 
             level is the depth of division before we should stop and print
          */
static  struct coords mid;  /* Used to save a temporary value */
static  struct node bez[MAX_BEZIER_DEPTH+1];
static  struct node *this;
static int xres = 0, yres = 0, xmin, xmax, ymin, ymax;



/*================================================================
        bez_depth:      find the bezier depth necessary for
                        a reasonable looking curve.
================================================================*/
int     bez_depth(int *pts_ptr)
{
    int xdiff, ydiff, depth;
    long        bez_size;

/* Estimate area. May be to high, but that's better than too low */

    xdiff = _max3( _abs(pts_ptr[0] - pts_ptr[2]),
                   _abs(pts_ptr[0] - pts_ptr[4]),
                   _abs(pts_ptr[0] - pts_ptr[6]));
    ydiff = _max3( _abs(pts_ptr[1] - pts_ptr[3]),
                   _abs(pts_ptr[1] - pts_ptr[5]),
                   _abs(pts_ptr[1] - pts_ptr[7]));

    bez_size = MULT( xdiff, ydiff );

    if ( !(bez_size >>= 4*MIN_BEZIER_DEPTH+depth_scale-2) )
        return MIN_BEZIER_DEPTH;
    for ( depth=MIN_BEZIER_DEPTH+1 ; depth<MAX_BEZIER_DEPTH ; depth++ )
        if ( !(bez_size >>= 4) ) return depth; /*** CLT was 3 ***/
    return MAX_BEZIER_DEPTH;
}



/*================================================================
        do_bez4:        calculate a bezier curve for xyin using
                        exactly four control points. See also
                        the best book so far on beziers:
                        Algorithms for graphics & image processing,
                        Theo Pavlidis, Bell Labs,
                        Computer Science Press ISBN 0-914894-65-X.

        Input:  x & y control point coords in array xyin.
                Max # of points of bezier curve to calculate m.
        Output: bezier curve x & y coords in array xyout.

        The first point is already done and the last point
        will be done for us in the drivers. -WDH
================================================================*/
int     do_bez4 (WORD *xyin, WORD depth, WORD *xyout)
{
    int count;
        
/* Initialize our local data structures */

    count=0;
    this=bez;  /* Point to first node */
    this->onscreen=FALSE;  /* Assume it may be partly or all off */
    this->level=depth-1; /* ... and initialize it. Depth-1 because the last
                            step includes a hidden depth and a half */
    this->anchor.x=((((LONG) xyin[6]) <<16) >>1) +0x4000;/* It may look back-to-front, but that ensures*/ 
    this->anchor.y=((((LONG) xyin[7]) <<16) >>1) +0x4000; /* it prints in the right direction. */
    this->in.x=((((LONG) xyin[4]) <<16) >>1) +0x4000;/* We have one bit to guard*/
    this->in.y=((((LONG) xyin[5]) <<16) >>1) +0x4000;/*against overflow and 15 to */
    this->out.x=((((LONG) xyin[2])<<16) >>1) +0x4000;/*guard against rounding error*/
    this->out.y=((((LONG) xyin[3])<<16) >>1) +0x4000;/*Also, we add 4000H to bias to*/
    (this+1)->anchor.x=((((LONG) xyin[0]) <<16) >>1) +0x4000;/*the centre of the pixel*/
    (this+1)->anchor.y=((((LONG) xyin[1]) <<16) >>1) +0x4000;
        
/* Now recursively divide the bez into small segments that can be printed
   as straight lines */
        
    while (1)   /* until break, near the middle of the loop */
    {
        if (this->onscreen == FALSE)
        {       /* Test if on-screen. */
            xmin= _min4( (WORD) (this->anchor.x>>16), (WORD) (this->in.x>>16),
                         (WORD) (this->out.x>>16), (WORD)( (this+1)->anchor.x>>16));
            xmax= _max4( (WORD) (this->anchor.x>>16), (WORD) (this->in.x>>16),
                         (WORD) (this->out.x>>16), (WORD)( (this+1)->anchor.x>>16));
            ymin= _min4( (WORD) (this->anchor.y>>16), (WORD) (this->in.y>>16),
                         (WORD) (this->out.y>>16), (WORD)( (this+1)->anchor.y>>16));
            ymax= _max4( (WORD) (this->anchor.y>>16), (WORD) (this->in.y>>16),
                         (WORD) (this->out.y>>16), (WORD)( (this+1)->anchor.y>>16));
            if (xmax<0 || xmin>xres || ymax<0 || ymin>yres)
                this->level=0;/*offscreen*/
            else if (xmin>0 && xmax<xres && ymin>0 && ymax<yres)
                this->onscreen = TRUE;
        }

        if (this->level <= 0)
        {
            /* We've reached the last stage. Send off the section as three
             lines. No time to explain now. See CJLT's notes. */

            *xyout++=(int)(((((this+1)->anchor.x+this->in.x)>>1)+this->out.x) >> 16);
            *xyout++=(int)(((((this+1)->anchor.y+this->in.y)>>1)+this->out.y) >> 16);
            *xyout++=(int)((((this->anchor.x+this->out.x)>>1)+this->in.x) >> 16);
            *xyout++=(int)((((this->anchor.y+this->out.y)>>1)+this->in.y) >> 16);
            count+=3;
            if (this == bez) break;  /* ... from the infinite while
            loop, since we have done it.*/
            *xyout++=(int)((this->anchor.x << 1) >> 16);
            *xyout++=(int)((this->anchor.y << 1) >> 16);

            this--;
        }
        else  /* recursively divide the section into two pieces */
        {
            mid.x=(this->in.x + this->out.x) >>1;
            (this+2)->anchor.x=(this+1)->anchor.x;
            (this+1)->out.x=(this->out.x + (this+1)->anchor.x) >>1;
            (this+1)->in.x=(mid.x + (this+1)->out.x ) >>1;
            this->in.x=(this->in.x + this->anchor.x) >>1;
            this->out.x=(this->in.x + mid.x) >>1;
            (this+1)->anchor.x=((this+1)->in.x + this->out.x) >>1;

            /* Now do the same for y */

            mid.y=(this->in.y + this->out.y) >>1;
            (this+2)->anchor.y=(this+1)->anchor.y;
            (this+1)->out.y=(this->out.y + (this+1)->anchor.y) >>1;
            (this+1)->in.y=(mid.y + (this+1)->out.y ) >>1;
            this->in.y=(this->in.y + this->anchor.y) >>1;
            this->out.y=(this->in.y + mid.y) >>1;
            (this+1)->anchor.y=((this+1)->in.y + this->out.y) >>1;

            /* and set the levels */
            this->level -- ;
            (this+1)->level = this->level;
            (this+1)->onscreen = this->onscreen;
            this++;     /* and point to the top of the stack of Bezier bits */
        }
    }
    return count;
}



/*================================================================
        bezier4:        calculate a bezier curve for xyin using
                        exactly four control points. See also
                        the best book so far on beziers:
                        Algorithms for graphics & image processing,
                        Theo Pavlidis, Bell Labs,
                        Computer Science Press ISBN 0-914894-65-X.

        Input:  x & y control point coords in array xyin.
                power of two of # of points of bezier curve to
                        calculate. Must be 4, 5 or 6.
        Output: bezier curve x & y coords in array xyout.
================================================================*/
VOID    bezier4 (int *xyin,int **xyout)
{
    int depth, cnt;

    depth = bez_depth(xyin);
    cnt = do_bez4 (xyin, depth, *xyout);  /*CJLT changed to new parameters
    and returned value */
    *xyout += 2*(cnt-1);
}



/*================================================================
        calc_bez:       Calculate Bezier curves and moves (when
                        necessary).

        Input:  PTSIN           - The points (actual or reference).
                INTIN           - Marks beziers & move points.
                close_loop      - if first point should be copied
                                  to last point (also done in
                                  point moves). For polygons.

        Output: XPTS            - The resulting points.
                XMOV            - An ordered list of the
                                  indices of move points in XPTS.
                CONTRL[1]       - The number of points in XPTS.
                pnt_mv_cnt      - The size of XMOV.
                x_used          - If Beziers or moves occured.
================================================================*/
WORD    calc_bez(WORD close_loop)
{
    WORD        maxchk, maxpnt, maxin, movptr, i;
    WORD        *pts_ptr, *last_pnt, *pts_out;
    BYTE        *chk_ptr;
    UWORD       memneeded;

#if DEBUG >= 30
    printf("\nCalc_bez appbuff: %p appsize: %x depth scale: %d\n",
           appbuff, appsize, depth_scale);
#endif
    pnt_mv_cnt = 0;

    /* Calculate the number of points we will actually need with all the        */
    /* Bezier curves and point moves.                                   */

    maxpnt = CONTRL[1];
    maxin = CONTRL[3] << 1;
    if (maxin > (maxpnt-1)) maxin = maxpnt-1;
    x_used = FALSE;

    if (close_loop)
        maxpnt++;

    for ( chk_ptr = (BYTE*) &INTIN[0], pts_ptr = &PTSIN[0], i=0;
          i < maxin; chk_ptr++, (pts_ptr += 2), i++ ) {
        if (*chk_ptr & POINT_MOVE) {
            pnt_mv_cnt++;
            x_used = TRUE;
        }
        if (*chk_ptr & BEZIER_START) {
            if (i >= maxin-1) break; /* disallow 2nd to last pnt */
            x_used = TRUE;
            maxpnt += (3 << (bez_depth(pts_ptr)-1) ) - 3;
            chk_ptr += 2;
            pts_ptr += 4;
            i += 2;
        }
    }

    if (close_loop)
        maxpnt += pnt_mv_cnt;
    if (maxpnt > max_poly_points || !x_used) {
#if DEBUG >= 30
        printf("calc_bez no Beziers - returning\n");
#endif
        return 1;
    }

    memneeded = _max(maxpnt, MINVERTSIN) * 2 * sizeof(XPTS[0]) +
        _max(pnt_mv_cnt, MININTIN) * sizeof(XMOV[0]);
    memneeded = (memneeded + 15) >> 4;

#if DEBUG >= 30
    printf("calc_bez memneeded: %x\n", memneeded);
#endif

    XPTS = NULLPTR;
    if (appbuff) {
        if (appsize >= memneeded) {
            XPTS = appbuff;
#if DEBUG >= 30
            printf("calc_bez using APP buffer\n");
#endif
        }
    } else if (dossize >= memneeded) {
        XPTS = dosbuff;
#if DEBUG >= 30
        printf("calc_bez using DOS buffer\n");
#endif
    } else if (!max_dos_used) {
        if (dossize) {
            GEXT_RELMEM(FP_SEG(dosbuff));
            dossize = 0;
#if DEBUG >= 30
            printf("calc_bez released DOS buffer\n");
#endif
        }
        dossize = GEXT_ALLMEM(memneeded, (UWORD *) ((LONG) &dosbuff + 2));
        if (dossize == memneeded)
            XPTS = dosbuff;
        else
            max_dos_used = 1;
#if DEBUG >= 30
        printf("calc_bez allocated DOS buffer req: %x act: %x\n",
               memneeded, dossize);
#endif
    }
    if (XPTS) {
        XMOV = XPTS + _max(maxpnt, MINVERTSIN) * 2;     /* dao #0012 */
    } else {
#if DEBUG >= 30
        printf("calc_bez NO buffer available\n");
#endif
        return 1;
    }

    pts_ptr =  &PTSIN[0];
    last_pnt = pts_ptr;
    pts_out = XPTS;
    maxchk = CONTRL[1];
    /** CJLT commented out. See below.  CONTRL[1] = maxpnt;  */
    movptr = pnt_mv_cnt;
    for ( i=0, chk_ptr=(BYTE*) &INTIN[0] ; i < maxchk ; i++, chk_ptr++) {
        if (i < maxin) {
            if (*chk_ptr & POINT_MOVE) {
                if (close_loop) {
                    *pts_out++ = *last_pnt++;
                    *pts_out++ = *last_pnt++;
                    last_pnt = pts_ptr;
                }
                XMOV[--movptr] = (WORD) (pts_out-XPTS - 4);
            }
            *pts_out++ = *pts_ptr++;
            *pts_out++ = *pts_ptr++;
            if (*chk_ptr & BEZIER_START) {
                if (i >= maxin-1) break; /* disallow 2nd to last pnt */
                pts_ptr -= 2;
                bezier4(pts_ptr, &pts_out);
                pts_ptr += 6;
                chk_ptr += 2;
                i += 2;
            }
        } else {
            *pts_out++ = *pts_ptr++;
            *pts_out++ = *pts_ptr++;
        }
    }
    if (close_loop) {
        *pts_out++ = *last_pnt++;
        *pts_out++ = *last_pnt++;
    }
    CONTRL[1]=(WORD)(pts_out-XPTS)>>1;  /* Added by CJLT */

#if DEBUG >= 30
    printf("calc_bez successful: %x points\n", maxpnt);
#endif
    return 0;
}



void bez_call(WORD *parmblock[5], WORD *tappbuff, UWORD tappsize)
{
    WORD        close_loop;
    WORD        save;
    WORD        savecontrl3;
    WORD        minx, maxx, miny, maxy, i, k, savenpts;
    WORD        *ptsptr;

    CONTRL = parmblock[0];
    INTIN = parmblock[1];
    PTSIN = parmblock[2];
    INTOUT = parmblock[3];
    PTSOUT = parmblock[4];
    if (CONTRL[0] == 5) {
        GEXT_DCALL(parmblock);
        if (INTIN[0] == 32 && INTIN[1] == 1 && CONTRL[3] == 3) {
#if DEBUG >= 30
            printf("Depth scale escape: %x\n", INTIN[2]);
#endif
            if (INTIN[2] < 0 || INTIN[2] > 99)
                depth_scale = MAX_DEPTH_SCALE;
            else {
#if     MIN_DEPTH_SCALE<MAX_DEPTH_SCALE
                depth_scale = ((INTIN[2] * (MAX_DEPTH_SCALE-MIN_DEPTH_SCALE+1)) / 100) + MIN_DEPTH_SCALE;

#else
                depth_scale = ((INTIN[2] * (MAX_DEPTH_SCALE-MIN_DEPTH_SCALE-1)) / 100) + MIN_DEPTH_SCALE;
#endif
            }
            /* return depth actually set */
            CONTRL[4] = 1;
            INTOUT[0] = ((depth_scale-MIN_DEPTH_SCALE) * 100) /
                (MAX_DEPTH_SCALE-MIN_DEPTH_SCALE);
        }
    } else if (CONTRL[1] <= 1 && CONTRL[0] == 11) {
#if DEBUG >= 10
        printf("BEZIERS ON/OFF # ptsin: %x\n", CONTRL[1]);
#endif
        if (dossize) {
            GEXT_RELMEM(FP_SEG(dosbuff));
            dossize = 0;
        }
        max_dos_used = 0;
        depth_scale = MAX_DEPTH_SCALE;
        dosbuff = NULLPTR;
        dossize = 0;
        bez_capable = 0;
        is_metafile = 0;
        if ((save = CONTRL[1]) != 0) {          /* dao #0013 */
            CONTRL[0] = 102;
            CONTRL[1] = 0;
            CONTRL[3] = 1;
            INTIN[0] = 0;
            GEXT_DCALL(parmblock);
            xres=INTOUT[0] >>1;  /* Get resolution */
            yres=INTOUT[1] >>1;  /* >>1 because we shift <16 >15 */
            is_metafile = INTOUT[44] == 4;
            INTIN[0] = 1;
            GEXT_DCALL(parmblock);
            bez_capable = INTOUT[28] & 2;
            max_poly_points = INTOUT[14] - 2;
        }
        CONTRL[0] = 11;
        CONTRL[1] = save;
        CONTRL[3] = 0;
        GEXT_DCALL(parmblock);
        CONTRL[2] = 0;
        CONTRL[4] = 1;
        INTOUT[0] = MAX_BEZIER_DEPTH;
    } else if (bez_capable || is_metafile) {
        if (!is_metafile) {
            if (CONTRL[0] == 6)
                CONTRL[5] = 12;
            else
                CONTRL[5] = 13;
            CONTRL[0] = 11;
        }
        GEXT_DCALL(parmblock);
    } else {
#if DEBUG > 0
        if (CONTRL[3]) {
            printf("Bezier call CONTRL[0]: %x CONTRL[1]: %x CONTRL[3]: %x\n",
                   CONTRL[0], CONTRL[1], CONTRL[3]);
            prarray("PTSIN", PTSIN, CONTRL[1] << 1);
            prarray("INTIN", INTIN, CONTRL[3]);
        }
#endif
        appbuff = tappbuff;
        appsize = tappsize;
        close_loop = CONTRL[0] == 9 || CONTRL[0] == 11 && CONTRL[5] == 13;
        save = depth_scale;
        savecontrl3 = CONTRL[3];

        while (depth_scale <= MIN_DEPTH_SCALE) {
            if (!calc_bez(close_loop)) {
                parmblock[1] = XMOV;
                parmblock[2] = XPTS;
                CONTRL[3] = pnt_mv_cnt;
                if (close_loop)
                    CONTRL[0] = 137;
                else
                    CONTRL[0] = 136;
                break;
            } else if (!x_used) {
                CONTRL[3] = 0;
                break;
            }
            depth_scale++;
        }
        depth_scale = save;

#if DEBUG > 0
        if (CONTRL[3]) {
            printf("Driver call CONTRL[0]: %x CONTRL[1]: %x CONTRL[3]: %x\n",
                   CONTRL[0], CONTRL[1], CONTRL[3]);
            prarray("PTSIN", parmblock[2], CONTRL[1] << 1);
            prarray("INTIN", parmblock[1], CONTRL[3]);
        }
#endif

        ptsptr = parmblock[2];                          /* dao #0011 s*/
        minx = maxx = *ptsptr++;
        miny = maxy = *ptsptr++;
        for (i = CONTRL[1] - 1; i > 0; i--) {
            k = *ptsptr++;
            if ( k < minx )             minx = k;
            else if ( k > maxx )        maxx = k;
            k = *ptsptr++;
            if ( k < miny )             miny = k;
            else if ( k > maxy )        maxy = k;
        }

        savenpts = CONTRL[1];

        GEXT_DCALL(parmblock);

        CONTRL[2] = 2;
        PTSOUT[0] = minx;
        PTSOUT[1] = miny;
        PTSOUT[2] = maxx;
        PTSOUT[3] = maxy;
        CONTRL[4] = 6;
        INTOUT[0] = savenpts;
        INTOUT[1] = pnt_mv_cnt;
        INTOUT[2] = FP_OFF(XPTS);
        INTOUT[3] = FP_SEG(XPTS);
        INTOUT[4] = FP_OFF(XMOV);
        INTOUT[5] = FP_SEG(XMOV);                       /* dao #0011 s*/
        CONTRL[3] = savecontrl3;
    }
}

#else

void BEZ_CALL()
{
}

#endif /* if GEM */
