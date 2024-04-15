/*
 * conout.c - lowlevel color model dependent screen handling routines
 *
 *
 * Copyright (C) 2004 by Authors (see below)
 * Copyright (C) 2016-2020 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * NOTE: the code currently assumes that the font width is 8 bits.
 * If we ever add a 16x32 font, the code will need changing!
 */

#include "emutos.h"
#include "lineavars.h"
#include "tosvars.h"            /* for v_bas_ad */
#include "sound.h"              /* for bell() */
#include "string.h"
#include "conout.h"
#include "../vdi/vdi_defs.h"    /* for phys_work stuff */

#define PLANE_OFFSET    2       /* interleaved planes */

#define FALCON_BLACK    0x0000
#define FALCON_WHITE    0xffbf

#if CONF_WITH_VDI_16BIT
extern Vwk phys_work;           /* attribute area for physical workstation */
#endif

/*
 * char_addr - retrieve the address of the source cell
 *
 *
 * Given an offset value.
 *
 * in:
 *   ch - source cell code
 *
 * out:
 *   pointer to first byte of source cell if code was valid
 */

static UBYTE *char_addr(WORD ch)
{
    UWORD offs;

    /* test against limits */
    if (ch >= v_fnt_st) {
        if (ch <= v_fnt_nd) {
            /* getch offset from offset table */
            offs = v_off_ad[ch];
            offs >>= 3;                 /* convert from pixels to bytes. */

            /* return valid address */
            return (UBYTE*)v_fnt_ad + offs;
        }
    }

    /* invalid code. no address returned */
    return NULL;
}



/*
 * cell_addr - convert cell X,Y to a screen address.
 *
 * convert cell X,Y to a screen address. also clip cartesian coordinates
 * to the limits of the current screen.
 *
 * input:
 *  x       cell X
 *  y       cell Y
 *
 * returns pointer to first byte of cell
 */

static UBYTE *cell_addr(UWORD x, UWORD y)
{
    ULONG disx, disy;

    /* check bounds against screen limits */
    if (x > v_cel_mx)
        x = v_cel_mx;           /* clipped x */

    if (y > v_cel_my)
        y = v_cel_my;           /* clipped y */

#if CONF_WITH_VIDEL
    if (TRUECOLOR_MODE) {       /* chunky pixels */
        disx = v_planes * x;
    }
    else
#endif
    {
        /*
         * v_planes cannot be more than 8, so as long as there are no more
         * than 4000 characters per line, the result will fit in a word ...
         *
         * X displacement = even(X) * v_planes + Xmod2
         */
        disx = v_planes * (x & ~1);
        if (IS_ODD(x)) {        /* Xmod2 = 0 ? */
            disx++;             /* Xmod2 = 1 */
        }
    }

    /* Y displacement = Y // cell conversion factor */
    disy = (ULONG)v_cel_wr * y;

    /*
     * cell address = screen base address + Y displacement
     * + X displacement + offset from screen-begin (fix)
     */
    return v_bas_ad + disy + disx + v_cur_of;
}



#if CONF_WITH_VIDEL
/*
 * cell_xfer16 - cell_xfer() for Falcon 16-bit graphics
 *
 * see the comments in cell_xfer() for more details
 */
static void cell_xfer16(UBYTE *src, UBYTE *dst)
{
    UWORD *p;
    UWORD fg, fgcol;
    UWORD bg, bgcol;
    WORD fnt_wr, line_wr, i, mask;

    MAYBE_UNUSED(fg);
    MAYBE_UNUSED(bg);

    fnt_wr = v_fnt_wr;
    line_wr = v_lin_wr;

#if CONF_WITH_VDI_16BIT
    if (v_stat_0 & M_REVID) {   /* handle reversed foreground and background colours */
        fg = v_col_bg;
        bg = v_col_fg;
    } else {
        fg = v_col_fg;
        bg = v_col_bg;
    }
    fgcol = phys_work.ext->palette[fg];
    bgcol = phys_work.ext->palette[bg];
#else
    /*
     * the foreground & backround colours should really come from the
     * physical workstation, but that requires 16-bit support in the VDI
     */
    if (v_stat_0 & M_REVID) {   /* handle reversed foreground and background colours */
        fgcol = FALCON_WHITE;
        bgcol = FALCON_BLACK;
    } else {
        fgcol = FALCON_BLACK;
        bgcol = FALCON_WHITE;
    }
#endif
    for (i = v_cel_ht; i--; ) {
        for (mask = 0x80, p = (UWORD *)dst; mask; mask >>= 1) {
            *p++ = (*src & mask) ? fgcol : bgcol;
        }
        dst += line_wr;
        src += fnt_wr;
    }
}
#endif



/*
 * cell_xfer - Performs a byte aligned block transfer.
 *
 *
 * This routine performs a byte aligned block transfer for the purpose of
 * manipulating monospaced byte-wide text. the routine maps a single-plane,
 * arbitrarily-long byte-wide image to a multi-plane bit map.
 * all transfers are byte aligned.
 *
 * in:
 * a0.l      points to contiguous source block (1 byte wide)
 * a1.l      points to destination (1st plane, top of block)
 *
 * out:
 * a4      points to byte below this cell's bottom
 */

static void cell_xfer(UBYTE *src, UBYTE *dst)
{
    UBYTE * src_sav, * dst_sav;
    UWORD fg;
    UWORD bg;
    int fnt_wr, line_wr;
    int plane;

#if CONF_WITH_VIDEL
    if (TRUECOLOR_MODE) {
        cell_xfer16(src, dst);
        return;
    }
#endif

    fnt_wr = v_fnt_wr;
    line_wr = v_lin_wr;

    /* check for reversed foreground and background colors */
    if (v_stat_0 & M_REVID) {
        fg = v_col_bg;
        bg = v_col_fg;
    }
    else {
        fg = v_col_fg;
        bg = v_col_bg;
    }

    src_sav = src;
    dst_sav = dst;

    for (plane = v_planes; plane--; ) {
        int i;

        src = src_sav;                  /* reload src */
        dst = dst_sav;                  /* reload dst */

        if (bg & 0x0001) {
            if (fg & 0x0001) {
                /* back:1  fore:1  =>  all ones */
                for (i = v_cel_ht; i--; ) {
                    *dst = 0xff;                /* inject a block */
                    dst += line_wr;
                }
            }
            else {
                /* back:1  fore:0  =>  invert block */
                for (i = v_cel_ht; i--; ) {
                    /* inject the inverted source block */
                    *dst = ~*src;
                    dst += line_wr;
                    src += fnt_wr;
                }
            }
        }
        else {
            if (fg & 0x0001) {
                /* back:0  fore:1  =>  direct substitution */
                for (i = v_cel_ht; i--; ) {
                    *dst = *src;
                    dst += line_wr;
                    src += fnt_wr;
                }
            }
            else {
                /* back:0  fore:0  =>  all zeros */
                for (i = v_cel_ht; i--; ) {
                    *dst = 0x00;                /* inject a block */
                    dst += line_wr;
                }
            }
        }

        bg >>= 1;                       /* next background color bit */
        fg >>= 1;                       /* next foreground color bit */
        dst_sav += PLANE_OFFSET;        /* top of block in next plane */
    }
}



/*
 * neg_cell - negates
 *
 * This routine negates the contents of an arbitrarily-tall byte-wide cell
 * composed of 1 to 8 Atari-style bit-planes, or of Falcon-style 16-bit
 * graphics.
 * Cursor display can be accomplished via this procedure.  Since a second
 * negation restores the original cell condition, there is no need to save
 * the contents beneath the cursor block.
 *
 * input:
 *  cell    points to destination (1st plane, top of block)
 */

static void neg_cell(UBYTE *cell)
{
    int plane, len;
    int cell_len = v_cel_ht;
    int lin_wr = v_lin_wr;

    v_stat_0 |= M_CRIT;                 /* start of critical section. */

#if CONF_WITH_VIDEL
    if (TRUECOLOR_MODE) {               /* chunky pixels */
        for (len = cell_len; len--; ) {
            WORD i;
            UWORD *addr;
            for (i = 8, addr = (UWORD *)cell; i--; addr++)
                *addr = ~*addr;
            cell += lin_wr;
        }
    }
    else
#endif
    {
        for (plane = v_planes; plane--; ) {
            UBYTE * addr = cell;        /* top of current dest plane */

            /* reset cell length counter */
            for (len = cell_len; len--; ) {
                *addr = ~*addr;
                addr += lin_wr;
            }
            cell += PLANE_OFFSET;       /* a1 -> top of block in next plane */
        }
    }

    v_stat_0 &= ~M_CRIT;                /* end of critical section. */
}



/*
 * next_cell - Return the next cell address.
 *
 * sets next cell address given the current position and screen constraints
 *
 * returns:
 *     false - no wrap condition exists
 *     true  - CR LF required (position has not been updated)
 */

static BOOL next_cell(void)
{
    /* check bounds against screen limits */
    if (v_cur_cx == v_cel_mx) {         /* increment cell ptr */
        if (!(v_stat_0 & M_CEOL)) {
            /* overwrite in effect */
            return 0;                   /* no wrap condition exists */
                                        /* don't change cell parameters */
        }

        /* call carriage return routine */
        /* call line feed routine */
        return 1;                       /* indicate that CR LF is required */
    }

    v_cur_cx += 1;                      /* next cell to right */

#if CONF_WITH_VIDEL
    if (TRUECOLOR_MODE) {               /* chunky pixels */
        v_cur_ad += 16;
        return 0;
    }
#endif

    /* if X is even, move to next word in the plane */
    if (IS_ODD(v_cur_cx)) {
        /* x is odd */
        v_cur_ad += 1;                  /* a1 -> new cell */
        return 0;                       /* indicate no wrap needed */
    }

    /* new cell (1st plane), added offset to next word in plane */
    v_cur_ad += (v_planes << 1) - 1;

    return 0;                           /* indicate no wrap needed */
}



/*
 * invert_cell - negates the cells bits
 *
 * This routine negates the contents of an arbitrarily-tall byte-wide cell
 * composed of an arbitrary number of (Atari-style) bit-planes.
 *
 * Wrapper for neg_cell().
 *
 * in:
 * x - cell X coordinate
 * y - cell Y coordinate
 */

void invert_cell(int x, int y)
{
    /* fetch x and y coords and invert cursor. */
    neg_cell(cell_addr(x, y));
}



/*
 * move_cursor - move the cursor.
 *
 * move the cursor and update global parameters
 * erase the old cursor (if necessary) and draw new cursor (if necessary)
 *
 * in:
 * d0.w    new cell X coordinate
 * d1.w    new cell Y coordinate
 */

void move_cursor(int x, int y)
{
    /* update cell position */

    /* clamp x,y to valid ranges */
    if (x < 0)
        x = 0;
    else if (x > v_cel_mx)
        x = v_cel_mx;

    if (y < 0)
        y = 0;
    else if (y > v_cel_my)
        y = v_cel_my;

    v_cur_cx = x;
    v_cur_cy = y;

    /* is cursor visible? */
    if (!(v_stat_0 & M_CVIS)) {
        /* not visible */
        v_cur_ad = cell_addr(x, y);             /* just set new coordinates */
        return;                                 /* and quit */
    }

    /* is cursor flashing? */
    if (v_stat_0 & M_CFLASH) {
        v_stat_0 &= ~M_CVIS;                    /* yes, make invisible...semaphore. */

        /* is cursor presently displayed ? */
        if (!(v_stat_0 & M_CSTATE)) {
            /* not displayed */
            v_cur_ad = cell_addr(x, y);         /* just set new coordinates */

            /* show the cursor when it moves */
            neg_cell(v_cur_ad);                 /* complement cursor. */
            v_stat_0 |= M_CSTATE;
            v_cur_tim = v_period;               /* reset the timer. */

            v_stat_0 |= M_CVIS;                 /* end of critical section. */
            return;
        }
    }

    /* move the cursor after all special checks failed */
    neg_cell(v_cur_ad);                         /* erase present cursor */

    v_cur_ad = cell_addr(x, y);                 /* fetch x and y coords. */
    neg_cell(v_cur_ad);                         /* complement cursor. */

    /* do not flash the cursor when it moves */
    v_cur_tim = v_period;                       /* reset the timer. */

    v_stat_0 |= M_CVIS;                         /* end of critical section. */
}



/*
 * ascii_out - prints an ascii character on the screen
 *
 * in:
 *
 * ch.w      ascii code for character
 */

void ascii_out(int ch)
{
    UBYTE * src, * dst;
    BOOL visible;                       /* was the cursor visible? */

    src = char_addr(ch);                /* a0 -> get character source */
    if (src == NULL)
        return;                         /* no valid character */

    dst = v_cur_ad;                     /* a1 -> get destination */

    visible = v_stat_0 & M_CVIS;        /* test visibility bit */
    if (visible) {
        v_stat_0 &= ~M_CVIS;                    /* start of critical section */
    }

    /* put the cell out (this covers the cursor) */
    cell_xfer(src, dst);

    /* advance the cursor and update cursor address and coordinates */
    if (next_cell()) {
        UBYTE * cell;
        UWORD y = v_cur_cy;

        /* perform cell carriage return. */
        cell = v_bas_ad + (ULONG)v_cel_wr * y;
        v_cur_cx = 0;                   /* set X to first cell in line */

        /* perform cell line feed. */
        if (y < v_cel_my) {
            cell += v_cel_wr;           /* move down one cell */
            v_cur_cy = y + 1;           /* update cursor's y coordinate */
        }
        else {
            scroll_up(0);               /* scroll from top of screen */
        }
        v_cur_ad = cell;                /* update cursor address */
    }

    /* if visible */
    if (visible) {
        neg_cell(v_cur_ad);             /* display cursor. */
        v_stat_0 |= M_CSTATE;           /* set state flag (cursor on). */
        v_stat_0 |= M_CVIS;             /* end of critical section. */

        /* do not flash the cursor when it moves */
        if (v_stat_0 & M_CFLASH) {
            v_cur_tim = v_period;       /* reset the timer. */
        }
    }
}



#if CONF_WITH_VIDEL
/*
 * blank_out16 - blank_out() for Falcon 16-bit graphics
 *
 * see the header comments in blank_out() for more details
 */
static void blank_out16(int topx, int topy, int botx, int boty)
{
    UWORD *addr;
    UWORD bgcol;
    WORD i, j;
    WORD offs, rows, width;

    width = (botx - topx + 1) * 8;          /* in words */

    /* calculate the offset from the end of row to next row start */
    offs = v_lin_wr/sizeof(WORD) - width;   /* in words */

    rows = (boty - topy + 1) * v_cel_ht;    /* in pixels */

#if CONF_WITH_VDI_16BIT
    /* get pixel value from physical workstation's palette */
    bgcol = phys_work.ext->palette[v_col_bg];
#else
    /*
     * the backround colour should really come from the physical
     * workstation, but that requires 16-bit support in the VDI
     */
     bgcol = FALCON_WHITE;
#endif

    addr = (UWORD *)cell_addr(topx, topy);  /* running pointer to screen */
    for (i = 0; i < rows; i++) {
        for (j = 0; j < width; j++) {
            *addr++ = bgcol;
        }
        addr += offs;
    }
}
#endif



/*
 * blank_out - Fills region with the background color.
 *
 * Fills a cell-word aligned region with the background color.
 *
 * The rectangular region is specified by a top/left cell x,y and a
 * bottom/right cell x,y, inclusive.  Routine assumes top/left x is
 * even and bottom/right x is odd for cell-word alignment. This is,
 * because this routine is heavily optimized for speed, by always
 * blanking as much space as possible in one go.
 *
 * in:
 *   topx - top/left cell x position (must be even)
 *   topy - top/left cell y position
 *   botx - bottom/right cell x position (must be odd)
 *   boty - bottom/right cell y position
 */

void blank_out(int topx, int topy, int botx, int boty)
{
    UWORD color;
    int pair, pairs, row, rows, offs;
    UBYTE *addr;

#if CONF_WITH_VIDEL
    if (TRUECOLOR_MODE) {
        blank_out16(topx, topy, botx, boty);
        return;
    }
#endif

    color = v_col_bg;                   /* bg color value */

    addr = cell_addr(topx, topy);       /* running pointer to screen */

    /*
     * # of cell-pairs per row in region - 1
     *
     * e.g. topx = 2, botx = 5, so pairs = 2
     */
    pairs = (botx - topx + 1) / 2;      /* pairs of characters */

    /* calculate the BYTE offset from the end of one row to next start */
    offs = v_lin_wr - pairs * 2 * v_planes;

    /*
     * # of lines in region - 1
     *
     * see comments re cell-pairs above
     */
    rows = (boty - topy + 1) * v_cel_ht;

    if (v_planes > 1) {
        /* Color modes are optimized for handling 2 planes at once */
        ULONG pair_planes[4];        /* bits on screen for 8 planes max */
        UWORD i;

        /* Precalculate the pairs of plane data */
        for (i = 0; i < v_planes / 2; i++) {
            /* set the high WORD of our LONG for the current plane */
            if (color & 0x0001)
                pair_planes[i] = 0xffff0000;
            else
                pair_planes[i] = 0x00000000;
            color >>= 1;        /* get next bit */

            /* set the low WORD of our LONG for the current plane */
            if (color & 0x0001)
                pair_planes[i] |= 0x0000ffff;
            color >>= 1;        /* get next bit */
        }

        /* do all rows in region */
        for (row = rows; row--;) {
            /* loop through all cell pairs */
            for (pair = pairs; pair--;) {
                for (i = 0; i < v_planes / 2; i++) {
                    *(ULONG*)addr = pair_planes[i];
                    addr += sizeof(ULONG);
                }
            }
            addr += offs;       /* skip non-region area with stride advance */
        }
    }
    else {
        /* Monochrome mode */
        UWORD pl;               /* bits on screen for current plane */

        /* set the WORD for plane 0 */
        if (color & 0x0001)
            pl = 0xffff;
        else
            pl = 0x0000;

        /* do all rows in region */
        for (row = rows; row--;) {
            /* loop through all cell pairs */
            for (pair = pairs; pair--;) {
                *(UWORD*)addr = pl;
                addr += sizeof(UWORD);
            }
            addr += offs;       /* skip non-region area with stride advance */
        }
    }
}



/*
 * scroll_up - Scroll upwards
 *
 *
 * Scroll copies a source region as wide as the screen to an overlapping
 * destination region on a one cell-height offset basis.  Two entry points
 * are provided:  Partial-lower scroll-up, partial-lower scroll-down.
 * Partial-lower screen operations require the cell y # indicating the
 * top line where scrolling will take place.
 *
 * After the copy is performed, any non-overlapping area of the previous
 * source region is "erased" by calling blank_out which fills the area
 * with the background color.
 *
 * in:
 *   top_line - cell y of cell line to be used as top line in scroll
 */

void scroll_up(UWORD top_line)
{
    ULONG count;
    UBYTE * src, * dst;

    /* screen base addr + cell y nbr * cell wrap */
    dst = v_bas_ad + (ULONG)top_line * v_cel_wr;

    /* form source address from cell wrap + base address */
    src = dst + v_cel_wr;

    /* form # of bytes to move */
    count = (ULONG)v_cel_wr * (v_cel_my - top_line);

    /* move BYTEs of memory*/
    memmove(dst, src, count);

    /* exit thru blank out, bottom line cell address y to top/left cell */
    blank_out(0, v_cel_my , v_cel_mx, v_cel_my);
}



/*
 * scroll_down - Scroll (partially) downwards
 */

void scroll_down(UWORD start_line)
{
    ULONG count;
    UBYTE * src, * dst;

    /* screen base addr + offset of start line */
    src = v_bas_ad + (ULONG)start_line * v_cel_wr;

    /* form destination from source + cell wrap */
    dst = src + v_cel_wr;

    /* form # of bytes to move */
    count = (ULONG)v_cel_wr * (v_cel_my - start_line);

    /* move BYTEs of memory*/
    memmove(dst, src, count);

    /* exit thru blank out */
    blank_out(0, start_line , v_cel_mx, start_line);
}
