/*
 * gsxasm1.S - Graphics kernel part 1
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "vdiconf.h"



        .text

/*
 * Globally used labels 
 */

        .globl  _CLC_FLIT       // calculate fill intersections of vector list
        .globl  _clc_flit       // draw a filled polygone (for linea)
        .globl  _vec_len        // compute length of a vector
        .globl  _habline        // draw a horizontal line
        .globl  _HABLINE        // draw a horizontal line
        .globl  concat          // convert x,y coordinates to buffer offsets
        .globl  xline           // alternate entry to HABLINE
        .globl  ortbl           // OR mask table for line drawing
        .globl  lf_tab          // left fringe look-up table
        .globl  htab            // jump table for drawing horizontal lines



/*
 * Externally defined labels 
 */

        .xdef   _CONTRL
        .xdef   _INTOUT
        .xdef   _PTSIN
        .xdef   _CLIP
        .xdef   _XMN_CLIP
        .xdef   _XMX_CLIP
        .xdef   _fill_buffer
        .xdef   _fil_intersect
        .xdef   _multifill
        .xdef   _TERM_CH

        .xdef   _FG_BP_1
        .xdef   _FG_BP_2
        .xdef   _FG_BP_3
        .xdef   _FG_BP_4

        .xdef   _X1
        .xdef   _X2
        .xdef   _Y1
        .xdef   _Y2
        .xdef   _LSTLIN
        .xdef   _LN_MASK
        .xdef   _WRT_MODE

        .xdef   _v_bas_ad
        .xdef   _v_planes
        .xdef   _v_lin_wr

        .xdef   _shft_off

        .bss
        .even
                .globl  _fill_buffer
_fill_buffer:   .ds.w   256             // must be 256 words or it will fail

        .text



/*
 * Local constants 
 */

#define x1              0               // offsets used by CLC_FLIT for accessing
#define y1              2               // parameters passed on the stack
#define x2              4
#define y2              6


/*
 * _CLC_FLIT - draw a filled polygone
 *
 * This routine calculates the fill intersections for a list
 * of vectors.  The x-intersection of each vector with the
 * scan-line of interest is calculated and inserted into a
 * buffer which is then sorted in ascending order.  The re-
 * sulting array of x-values are then pulled out pair-wise
 * and used as inputs to "_HABLINE".
 *
 * This version of the routine has several changes from the
 * original version contained in the screen driver.  The code
 * has been optimized in several places for improvements in
 * both size and execution time.  It also has been changed to
 * prevent the drawing of the outline of a figure with the
 * fill pattern.  These changes are all included as condition-
 * ally assembled code.
 *
 * input:
 *     CONTRL[1] = number of vectors.
 *     PTSIN[]   = array of vertices.
 *     Y1        = scan-line to calculate intersections for.
 *     fil_intersect = 0.
 *
 * output:
 *     fil_intersect = number of intersections.
 *     fill_buffer  = array of x-values.
 *
 * destroys:
 *     nothing (because of added wrapper)
 */
 
_CLC_FLIT:
        // A wrapper, so save the registers
        movem.l d2-d7/a2-a6,-(sp)
        bsr.s   _clc_flit
        movem.l (sp)+,d2-d7/a2-a6
        rts

//
//      Initialize the pointers and counters.
//
_clc_flit:
        move.l  _CONTRL,a0      // fetch number of vectors.
        move.w  2(a0),d0
        subq.w  #1,d0           // minus 1 for dbra
        move.l  _PTSIN,a0       // point to array of vertices.
        lea     _fill_buffer,a1 // point to array of x-values.
        move.w  #0,_fil_intersect       // reset counter
//
//      Determine if the current vector is horizontal.  If it is then
//      ignore it.
//
//      This may not be safe.  It could in some instances cause part of the
//      border to be drawn with the interior style.  This will cause prob-
//      lems in the exclusive or mode with border visibility on and in all
//      modes with border visibility off.
//
flit_lp:
        move.w  y2(a0),d1       // fetch y-value of 2nd endpoint.
        move.w  y1(a0),d2       // fetch y-value of 1st endpoint.
        sub.w   d2,d1           // dy.
        beq     no_fill_int     // dy = 0 => ignore this vector.

//
//      Determine whether the current vector intersects with the scan
//      line we wish to draw.  This test is performed by computing the
//      y-deltas of the two endpoints from the scan line.  If they both
//      have the same sign then the line does not intersect and can be
//      ignored.  The origin for this test is found in Newman and Sproull.
//
//      This test may not be entirely safe either for the same reasons as
//      above.
//
        move.w  _Y1,d3          // fetch scan-line y.
        move.w  d3,d4
        sub.w   d2,d4           // delta y1.
        sub.w   y2(a0),d3       // delta y2.
        move.w  d4,d2
        eor.w   d3,d2           // are the signs equal?
        bpl     no_fill_int     // yes, ignore this vector.

//
//      Compute the x-coordinate of the point of intersection of the vector
//      with the current scan line.  The law of similar triangles is used.
//
        move.w  x2(a0),d2       // fetch x2.
        sub.w   x1(a0),d2       // dx.
        bmi.b   rtol            // negative delta - use x2 and delta y2
        move.w  x1(a0),d3       // get x1
        bra.b   getdx
rtol:
        move.w  d3,d4           // replace delta y1 with delta y2
        move.w  x2(a0),d3       // get x2
        neg.w   d1              // negate dy
        neg.w   d2              // negate dx
getdx:
        asl.w   #1,d2           // 2*dx. (for more precision)
        muls    d2,d4           // (2*dx) * delta y1.
        divs    d1,d4           // (2*dx) * delta y1 / dy.
        bmi     int_neg
        addq.w  #1,d4           // rounding.
        bra     round_x
int_neg:
        subq.w  #1,d4           // rounding
round_x:
        asr.w   #1,d4
ld_fill_int:
        add.w   d3,d4           // add in x1.
        move.w  d4,(a1)+        // put in fill_buffer.
        addq.w  #1,_fil_intersect  // increment # of intersections.
no_fill_int:
        addq.w  #4,a0           // next pair of endpoints. (next vector)
        dbra    d0,flit_lp

//
//      All of the points of intersection have now been found.  If there were
//      none then there is nothing more to do.  Otherwise, sort the list of
//      points of intersection in ascending order.  (The list contains only
//      the x-coordinates of the points.)
//
sort_fill_int:
        move.w  _fil_intersect,d0     // how many intersections?
        bne     sfi_cont        // 2 or more => sort them.
        rts                     // 0 => nothing else to do.
sfi_cont:
        lea     _fill_buffer,a0
        bsr     bub_sort        // bubble-sort the fill buffer.

//
//      The points are now sorted.  If a clipping rectangle is in use then
//      branch to special code.
//
        move.w  _fil_intersect,d0     // get number of buffered points
        asr.w   #1,d0           // convert to number of pairs of points
        subq.w  #1,d0           // adjust count for loop
        tst.w   _CLIP           // is clipping on?
        bne     dr_clip         // yes, branch.

//
//      Clipping is not in force.  Draw from point to point.
//
//      This code has been modified from the version in the screen driver.
//      The x-coordinates of each line segment are adjusted so that the
//      border of the figure will not be drawn with the fill pattern.  If
//      the starting point is greater than the ending point then nothing is
//      done.
//
draw_lp:
        move.w  (a1)+,d1        // grab a pair of endpoints
        move.w  (a1)+,d2
        addq.w  #1,d1           // adjust the endpoints
        subq.w  #1,d2
        cmp.w   d1,d2           // is start still to left of end?
        blt     dr_lp1          // nope - nothing to draw
        move.w  d1,_X1          // store the adjusted endpoints
        move.w  d2,_X2
        move.w  d0,-(sp)        // save the number of pairs
        move.l  a1,-(sp)        // save the pointer to the array
        jsr     _habline        // draw the line segment
        move.l  (sp)+,a1        // restore the pointer to the array
        move.w  (sp)+,d0        // restore the number of pairs
dr_lp1:
        dbra    d0,draw_lp      // loop until done
        rts

//
//      Clipping is in force.  Once the endpoints of the line segment have
//      been adjusted for the border, clip them to the left and right sides
//      of the clipping rectangle.
//
//      This code has been modified from the version in the screen driver.
//      The x-coordinates of each line segment are adjusted so that the
//      border of the figure will not be drawn with the fill pattern.  If
//      the starting point is greater than the ending point then nothing is
//      done.
//

dr_clip:
        move.w  _XMN_CLIP,d1     // get clip minimum
        move.w  _XMX_CLIP,d2     // get clip maximum
drc_0:
        move.w  (a1)+,d3        // grab a pair of intersections
        move.w  (a1)+,d4
        addq.w  #1,d3           // adjust the endpoints
        subq.w  #1,d4
        cmp.w   d4,d3           // is start still to left of end?
        bgt     drc_end         // nope - nothing to draw
        cmp.w   d1,d3           // is x1 < xmn_clip?
        bge     drc_1           // nope - check for clip on right
        cmp.w   d1,d4           // entire segment to left of clip rect?
        blt     drc_end         // yes - nothing to draw
        move.w  d1,d3           // nope - clip left end of line
drc_1:
        cmp.w   d2,d4           // is x2 > xmx_clip?
        ble     drc_2           // nope - ready to draw now
        cmp.w   d2,d3           // entire segment to right of clip rect?
        bgt     drc_end         // yes - nothing to draw
        move.w  d2,d4           // nope - clip right end of line
drc_2:
        move.w  d3,_X1          // save the endpoints
        move.w  d4,_X2
        move.w  d0,-(sp)        // save the number of pairs
        move.w  d1,-(sp)        // save the clip values
        move.w  d2,-(sp)
        move.l  a1,-(sp)        // save pointer to the array
        jsr     _habline        // fill the line segment
        move.l  (sp)+,a1        // restore the pointer to the array
        move.w  (sp)+,d2        // restore the clip values
        move.w  (sp)+,d1
        move.w  (sp)+,d0        // restore the number of pairs
drc_end:
        dbra    d0,drc_0        // loop until done
        rts


/*
 * bub_sort - sorts an array of words
 *
 * This routine bubble-sorts an array of words into ascending order.
 *
 * input:
 *     a0 = ptr to start of array.
 *     d0 = number of words in array.
 *
 * output:
 *     a1 = ptr to start of sorted array.
 *
 * destroys: 
 *     d0, d1, d2, a0, a1
 *
 * MAD: Maybe, this could be replaced by a faster sort algorithm
 */
 
//
//      If the array is empty or if there is only one entry to sort then
//      return.  Otherwise, initialize the necessary pointers and counters.
//
bub_sort:
        subq.w  #2,d0           // compute number of compares - 1
        blt.b   bs_out          // array empty or only one entry - done
        move.w  d0,d1           // save the number of compares
        move.l  a0,a1           // save pointer to the array

//
//      Initialize the counter and pointer necessary for this pass through
//      the sort.
//
bsl0_init:
        move.w  d1,d0           // get the number of compares to perform
        move.l  a1,a0           // get the pointer to the array

//
//      Make a single pass through the array comparing pairs of values. If
//      the nth value is greater than the n+1th value then swap them.  Each
//      time this loop is completed the next largest value will be moved to
//      its appropriate place at the end of the array.
//
bs_lp0:
        move.w  (a0)+,d2        // get next value
        cmp.w   (a0),d2         // is it <= the next one?
        ble     bs_noswap       // yes - do nothing
        move.w  (a0),-2(a0)     // nope - swap them
        move.w  d2,(a0)
bs_noswap:
        dbra    d0,bs_lp0       // loop until an entire pass is complete

//
//      The next largest value has been sorted to its place int the array.
//      loop until all the values are sorted.
//
bsl1_end:
        dbra    d1,bsl0_init    // one less value to sort next time
bs_out:
        rts



/*
 * concat - calculate screen offset
 *
 *
 *
 * This routine converts x and y coordinates into a physical
 * offset to a word in the screen buffer and an index to the
 * desired bit within that word.
 *
 * input:
 *    d0.w = x coordinate.
 *    d1.w = y coordinate.
 *
 * output:
 *    d0.w = word index. (x mod 16)
 *    d1.l = physical offset -- (y * bytes_per_line) + (x & xmask)>>xshift
 *
 * destroys:       nothing
 */

concat:
        movem.w d2/d3,-(sp)     // save the registers that get clobbered

/* Convert the y-coordinate into an offset to the start of the scan row. */

        mulu.w  _v_lin_wr,d1    // compute offset to start of scan row

//
// Compute the bit offset into the desired word, save it, and remove
// these bits from the x-coordinate.
//
        move.w  d0,d2           // save the x-coordinate for later
        andi.w  #0x000f,d0      // bit offset = x-coordinate mod 16
        andi.w  #0xfff0,d2      // clear bits for offset into word

//
// Convert the adjusted x-coordinate to a word offset into the current        
// scan line.  If the planes are arranged in an interleaved fashion with      
// a word for each plane then shift the x-coordinate by a value contained     
// in the shift table.  If the planes are arranged as separate, consecu-      
// tive entities then divide the x-coordinate by 8 to get the number of bytes.                                                                     
//
#if vme10
        lsr.w   #3,d2           // convert x-coordinate to offset
#else
        move.b  _shft_off,d3    // get pre-calculated shift factor
        lsr.w   d3,d2           // convert x-coordinate to offset
#endif

//
// Compute the offset to the desired word by adding the offset to the
// start of the scan line to the offset within the scan line, restore
// the clobbered registers, and exit.
//
        ext.l   d2
        add.l   d2,d1           // compute total offset into screen buffer
        movem.w (sp)+,d2/d3     // restore the clobbered registers
        rts




/*
 * OR Mask Table                               
 */

ortbl:          dc.w    0x8000
                dc.w    0x4000
                dc.w    0x2000
                dc.w    0x1000
                dc.w    0x0800
                dc.w    0x0400
                dc.w    0x0200
                dc.w    0x0100
                dc.w    0x0080
                dc.w    0x0040
                dc.w    0x0020
                dc.w    0x0010
                dc.w    0x0008
                dc.w    0x0004
                dc.w    0x0002
                dc.w    0x0001


        .end
