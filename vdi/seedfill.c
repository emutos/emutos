/*
 * seedfill.c - Filling algorithm
 *
 * Copyright 1982 Digital Research Inc.  All rights reserved.
 * Copyright 1999 Caldera, Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "vdidef.h"
#include "gsxextrn.h"
#include "lineavars.h"
#include "tosvars.h"

//#include "kprint.h"

#define EMPTY   0xffff
#define DOWN_FLAG 0x8000
#define ABS(v) (v & 0x7FFF)
#define QSIZE 200
#define QMAX QSIZE-1



/* Global variables */
static WORD seed_type;          /* indicates the type of fill   */
UWORD search_color;       /* the color of the border      */


/* some kind of stack for the segments to fill */
WORD queue[QSIZE];       /* storage for the seed points  */
WORD qbottom;            /* the bottom of the queue (zero)   */
WORD qtop;               /* points top seed +3           */
WORD qptr;               /* points to the active point   */
WORD qtmp;
WORD qhole;              /* an empty space in the queue */


/* prototypes */
void crunch_queue();


/*
 * get_color - Get color value of requested pixel.
 */
static inline UWORD
get_color (UWORD mask, UWORD * addr)
{
    UWORD color = 0;			/* clear the pixel value accumulator. */
    WORD plane = v_planes;

    while(1) {
        /* test the bit. */
        if ( *--addr & mask )
            color |= 1;		/* if 1, set color accumulator bit. */

        if ( --plane == 0 )
            break;

        color <<= 1;		/* shift accumulator for next bit_plane. */
    }

    return color; 	/* this is the color we are searching for */
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
pixelread(WORD x, WORD y)
{
    UWORD *addr;
    UWORD mask;

    /* convert x,y to start adress and bit mask */
    addr = (UWORD*)(v_bas_ad + (LONG)y * v_lin_wr + ((x&0xfff0)>>shft_off));
    addr += v_planes;           	/* start at highest-order bit_plane */
    mask = 0x8000 >> (x&0xf);		/* initial bit position in WORD */

    return get_color(mask, addr);       /* return the composed color value */
}

static inline UWORD
search_to_right (WORD x, UWORD mask, const UWORD search_col, UWORD * addr)
{
    /* is x coord < x resolution ? */
    while( x++ < XMX_CLIP ) {
        UWORD color;

        /* need to jump over interleaved bit_plane? */
        mask = mask >> 1 | mask << 15;	/* roll right */
        if ( mask & 0x8000 )
            addr += v_planes;

        /* search, while pixel color != search color */
        color = get_color(mask, addr);
        if ( search_col != color ) {
            break;
        }

    }

    return x - 1;	/* output x coord -1 to endxright. */
}

static inline UWORD
search_to_left (WORD x, UWORD mask, const UWORD search_col, UWORD * addr)
{
    /* Now, search to the left. */
    while (x-- > XMN_CLIP) {
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

    return x + 1;	/* output x coord + 1 to endxleft. */
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
 */

WORD
end_pts(WORD x, WORD y, WORD *xleftout, WORD *xrightout)
{
    UWORD color;
    UWORD * addr;
    UWORD mask;

    /* see, if we are in the y clipping range */
    if ( y < YMN_CLIP || y > YMX_CLIP)
        return 0;

    /* convert x,y to start adress and bit mask */
    addr = (UWORD*)(v_bas_ad + (LONG)y * v_lin_wr + ((x&0xfff0)>>shft_off));
    addr += v_planes;           	/* start at highest-order bit_plane */
    mask = 0x8000 >> (x & 0x000f);   /* fetch the pixel mask. */

    /* get search color and the left and right end */
    color = get_color (mask, addr);
    *xrightout = search_to_right (x, mask, color, addr);
    *xleftout = search_to_left (x, mask, color, addr);

    /* see, if the whole found segment is of search color? */
    if ( color != search_color ) {
        return seed_type ^ 1;	/* return segment not of search color */
    }
    return seed_type ^ 0;	/* return segment is of search color */
}

/* Prototypes local to this module */
WORD get_seed(WORD xin, WORD yin, WORD *xleftout, WORD *xrightout);


void
d_contourfill()
{
    REG WORD fc;
    WORD newxleft;           /* ends of line at oldy +       */
    WORD newxright;          /* the current direction    */
    WORD oldxleft;           /* left end of line at oldy     */
    WORD oldxright;          /* right end                    */
    WORD oldy;               /* the previous scan line       */
    WORD xleft;              /* temporary endpoints          */
    WORD xright;             /* */
    WORD direction;          /* is next scan line up or down */
    BOOL notdone;            /* does seedpoint==search_color */
    BOOL gotseed;            /* a seed was put in the Q      */

    xleft = PTSIN[0];
    oldy = PTSIN[1];

    if (xleft < XMN_CLIP || xleft > XMX_CLIP ||
        oldy < YMN_CLIP  || oldy > YMX_CLIP)
        return;

    search_color = INTIN[0];

    /* Range check the color and convert the index to a pixel value */
    if (search_color >= DEV_TAB[13])
        return;

    if (search_color < 0) {
        search_color = pixelread(xleft,oldy);
        seed_type = 1;
    } else {
        const WORD plane_mask[] = { 1, 3, 7, 15 };

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
    fc = cur_work->fill_color;
    FG_BP_1 = (fc & 1);
    FG_BP_2 = (fc & 2);
    FG_BP_3 = (fc & 4);
    FG_BP_4 = (fc & 8);

    LSTLIN = FALSE;

    notdone = end_pts(xleft, oldy, &oldxleft, &oldxright);

    qptr = qbottom = 0;
    qtop = 3;                   /* one above highest seed point */
    queue[0] = (oldy | DOWN_FLAG);
    queue[1] = oldxleft;
    queue[2] = oldxright;           /* stuff a point going down into the Q */

    if (notdone) {
        /* couldn't get point out of Q or draw it */
        while (1) {
            direction = (oldy & DOWN_FLAG) ? 1 : -1;
            gotseed = get_seed(oldxleft, (oldy + direction),
                               &newxleft, &newxright);

            if ((newxleft < (oldxleft - 1)) && gotseed) {
                xleft = oldxleft;
                while (xleft > newxleft)
                    get_seed(--xleft, oldy ^ DOWN_FLAG, &xleft, &xright);
            }
            while (newxright < oldxright)
                gotseed = get_seed(++newxright, oldy + direction, &xleft,
                                   &newxright);
            if ((newxright > (oldxright + 1)) && gotseed) {
                xright = oldxright;
                while (xright < newxright)
                    get_seed(++xright, oldy ^ DOWN_FLAG, &xleft, &xright);
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

            horzline(oldxleft, oldxright, ABS(oldy));
        }
    }
}                               /* end of fill() */

/*
 * crunch_queue - move qtop down to remove unused seeds
 */
void
crunch_queue()
{
    while ((queue[qtop - 3] == EMPTY) && (qtop > qbottom))
        qtop -= 3;
    if (qptr >= qtop)
        qptr = qbottom;
}

/*
 * get_seed - put seeds into Q, if (xin,yin) is not of search_color
 */
WORD
get_seed(WORD xin, WORD yin, WORD *xleftout, WORD *xrightout)
{
    if (end_pts(xin, ABS(yin), xleftout, xrightout)) {
        /* false if of search_color */
        for (qtmp = qbottom, qhole = EMPTY; qtmp < qtop; qtmp += 3) {
            /* see, if we ran into another seed */
            if ( ((queue[qtmp] ^ DOWN_FLAG) == yin) && (queue[qtmp] != EMPTY) &&
                (queue[qtmp + 1] == *xleftout) )

            {
                /* we ran into another seed so remove it and fill the line */
                horzline(*xleftout, *xrightout, ABS(yin));
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

        queue[qtmp++] = yin;	/* put the y and endpoints in the Q */
        queue[qtmp++] = *xleftout;
        queue[qtmp] = *xrightout;
        return 1;             /* we put a seed in the Q */
    }

    return 0; 		/* we didnt put a seed in the Q */
}



void
v_get_pixel()
{
    WORD pel;
    WORD *int_out;
    const WORD x = PTSIN[0];       /* fetch x coord. */
    const WORD y = PTSIN[1];       /* fetch y coord. */

    /* Get the requested pixel */
    pel = (WORD)pixelread(x,y);

    int_out = INTOUT;
    *int_out++ = pel;

    /* Correct the pel value for the number of planes so it is a standard
       value */

    if ((INQ_TAB[4] == 1 && pel) || (INQ_TAB[4] == 2 && pel == 3))
        pel = 15;

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
get_pix()
{
    const WORD x = PTSIN[0];
    const WORD y = PTSIN[1];

    return pixelread(x,y);	/* return the composed color value */
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
put_pix()
{
    UWORD *addr;
    WORD x,y;
    UWORD color;
    UWORD mask;
    int plane;

    x = PTSIN[0];       /* fetch x coord. */
    y = PTSIN[1];       /* fetch y coord. */
    color = INTIN[0];   /* device dependent encoded color bits */

    /* convert x,y to start adress and bit mask */
    addr = (UWORD*)(v_bas_ad + (LONG)y * v_lin_wr + ((x&0xfff0)>>shft_off));
    mask = 0x8000 >> (x&0xf);            /* initial bit position in WORD */

    for (plane = v_planes-1; plane >= 0; plane-- ) {
        color = color >> 1| color << 15;        /* rotate color bits */
        if (color&0x8000)
            *addr++ |= mask;
        else
            *addr++ &= ~mask;
    }
}
