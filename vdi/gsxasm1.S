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
        .xdef   _patmsk
        .xdef   _patptr

        .xdef   _v_bas_ad
        .xdef   _v_planes
        .xdef   _v_lin_wr

        .xdef   _shft_off
        .xdef   _habline

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

	move.w  d0,-(sp)        // save the number of pairs
        move.l  a1,-(sp)        // save the pointer to the array
#if 0
	move.w  d1,_X1          // store the adjusted endpoints
        move.w  d2,_X2
        jsr     _habline       // draw the line segment
#else
        move.w  _Y1,d3          // fetch scan-line y.
        move.w  d3,-(sp)        // Y1
        move.w  d2,-(sp)        // X2
	move.w  d1,-(sp)        // X1
        jsr     _horzline       // draw the line segment
	addq #6,sp
#endif
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
        move.w  d0,-(sp)        // save the number of pairs
        move.w  d1,-(sp)        // save the clip values
        move.w  d2,-(sp)
        move.l  a1,-(sp)        // save pointer to the array

#if 0
        move.w  d3,_X1          // save the endpoints
        move.w  d4,_X2
        jsr     _habline        // draw the line segment
#else
        move.w  _Y1,d2          // fetch scan-line y.
        move.w  d2,-(sp)        // Y1
        move.w  d4,-(sp)        // X2
	move.w  d3,-(sp)        // X1
        jsr     _horzline       // draw the line segment
	addq #6,sp
#endif
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



#if 0
/*
 * HABLINE - draw a horizontal line
 *
 * This routine draws a line between (_X1,_Y1) and (_X2,_Y1)
 * using a left fringe, inner loop, right fringe bitblt algor-
 * ithm.  The line is modified by the pattern and _WRT_MODE
 * variables.  This routine handles all 3 video resolutions.
 * Note that 2 entry points are provided for ABLINE.
 *
 * input:
 *     _X1,_Y1,_X2  = coordinates.
 *     _v_planes    = number of video planes. (resolution)
 *     _patmsk      = index into pattern.
 *     _patptr      = ptr to pattern.
 *     _WRT_MODE    = writing mode:
 *                        0 => replace mode.
 *                        1 => or mode.
 *                        2 => xor mode.
 *                        3 => not mode.
 *
 * output:
 *     nothing.
 *
 * destroys:
 *     everything, but is just called from assembler
 */

//
//      This is the entry point used by ABLINE when a horizontal line is
//      to be drawn using the current line style.
//
xl_noswap:
        lea     _LN_MASK,a0     // get the current line style
        movea.l a1,a3           // get # of bytes between graphic planes
        move.w  #0,a1           // line style is monoplaned - no offset
        move.w  _X1,d4          // get starting x-coordinate
        move.w  _X2,d2          // get ending x-coordinate
        bra     xline

//
//      This is the entry point used by ABLINE when a horizontal line is
//      to be drawn using the current line style but when the two endpoints
//      need to be swapped.
//
xl_swap:
        lea     _LN_MASK,a0     // get the current line style
        movea.l a1,a3           // get # of bytes between graphic planes
        move.w  #0,a1           // line style is monoplaned - no offset
        move.w  _X2,d4          // get starting x-coordinate
        move.w  _X1,d2          // get ending x-coordinate
        bra     xline

//
//      Main entry point to _HABLINE
//
//      Compute the address of the word containing the starting point of
//      the line to be drawn and the offset of the starting bit within the
//      word.
//
_HABLINE:
        movea.l _v_bas_ad,a5    // get base address of first video plane
        lea     _FG_BP_1,a4     // get address of bit plane mask table
#if vme10
        move.w  _v_planes,d3    // get number of planes - 1
        subq.w  #1,d3
        move.l  _v_pl_dspl,a3   // get offset to next video plane
#else
        move.l  #0,a3
        move.w  _v_planes,d3    // get number of planes
        move.w  d3,a3
        subq.w  #1,d3           // get number of planes - 1
        adda.w  a3,a3           // get offset to next word in same plane
#endif
        move.w  _X1,d0          // get starting x-coordinate (input to concat)
        move.w  d0,d4           // save it for later use
        move.w  _Y1,d1          // get y-coordinate (input to concat)
        move.w  d1,d5           // save it for later use
        bsr     concat          // compute word offset and bit offset
        adda.l  d1,a5           // compute actual address of starting word
        move.w  _X2,d2          // get ending x-coordinate

//
//      Get the pattern with which the line is to be drawn.
//
        and.w   _patmsk,d5      // get index into pattern
        asl.w   #1,d5           // convert to offset into pattern def table
        move.l  _patptr,a0      // get pointer to start of pattern definition
        adda.w  d5,a0           // get pointer to desired word of pattern def
        move.w  #0,a1           // init offset to next plane of pattern to 0
        tst.w   _multifill      // using multi-planed patterns?
        beq     xline           // nope
        move.w  #32,a1          // yes - offset = length of 1 plane of pat def

//
//      At this point the register usages are as follows:
//
//      d7 = scratch.           a5 = ptr to destination.
//      d6 = scratch.           a4 = ptr to _FG_BP_1.
//      d5 = scratch.           a3 = offset to next plane
//      d4 = _X1.               a2 = scratch
//      d3 = # of bit_planes - 1.
//      d2 = _X2.               a1 = offset to next plane of fill pattern
//      d1 = scratch            a0 = ptr to fill pattern
//      d0 = _X1 and 0x000F.
//

//
//      Compute the left and right fringe masks for the line.
//
xline:
        asl.w   #1,d0           // convert bit offset of 1st bit to word index
        move.w  lf_tab(pc,d0.w),d0      // get not of left fringe mask
        not.w   d0              // invert bits of mask to proper value
        move.w  d2,d7           // get ending x-coordinate
        andi.w  #0x000F,d7      // get bit offset into its word
        asl.w   #1,d7           // convert bit offset of last bit to word index
        move.w  rf_tab(pc,d7.w),d7      // get right fringe mask

//
//      Compute the number of entire words to be written with the line.
//
        asr.w   #4,d4           // compute # of words preceeding starting point
        asr.w   #4,d2           // compute # of words preceeding ending point
        sub.w   d4,d2           // compute word offset between the endpoints
        subq.w  #1,d2           // compute # of full words in line

//
//      If the number of full words in the line segment is negative then the
//      two endpoints lie in the same word.  Combine the two fringe masks,
//      set the inner loop count to 0, and modify the right fringe mask so
//      that it will have no effect.
//
        bpl     hab_decode      // count >= 0 so this is a normal case
        or.w    d7,d0           // combine fringe masks
        clr.w   d2              // set inner loop count to 0
        moveq   #-1,d7          // prevent right mask from affecting screen
//
//      Jump to the appropriate handling routine for the current write mode
//      for both the general and special cases.
//
hab_decode:
        move.w  _WRT_MODE,d5    // write mode used to select HABLINE entry 
        asl.w   #2,d5           // convert to longword index
        move.l  htab(pc,d5.w),a2        // get address of handling routine
        jmp     (a2)            // jump to it

//
//      At this point the register usages are as follows:
//
//      d7 = right mask.        a5 = ptr to destination.
//      d6 = scratch.           a4 = ptr to _FG_BP_1.
//      d5 = scratch.           a3 = offset to next plane or next word in plane
//      d4 = scratch.           a1 = offset to next bitplane's fill pattern
//      d3 = # of bit_planes - 1.
//      d2 = inner loop count.  a0 = ptr to fill pattern
//      d1 = scratch
//      d0 = left mask (or left "and" right masks).
//

#endif



/*
 * Word Mask Table                                                
 * 
 * The table has been compacted by taking the one's complement of the left
 * fringe table and combining it with the right fringe table.                                                  
 */

lf_tab:         dc.w    0xFFFF          // origin for not left fringe lookup.
rf_tab:         dc.w    0x7FFF          // origin for right fringe lookup.
                dc.w    0x3FFF
                dc.w    0x1FFF
                dc.w    0x0FFF
                dc.w    0x07FF
                dc.w    0x03FF
                dc.w    0x01FF
                dc.w    0x00FF
                dc.w    0x007F
                dc.w    0x003F
                dc.w    0x001F
                dc.w    0x000F
                dc.w    0x0007
                dc.w    0x0003
                dc.w    0x0001
                dc.w    0x0000


/*
 * HABLINE Mode Address Table 
 *
 * This table contains the jump addresses for the line drawing routines
 * for the four writing modes.  
 */

htab:
#if rev_vid
        dc.l    rep_x
        dc.l    or_x
        dc.l    xor_x
        dc.l    nor_x
#else
        dc.l    rep_x
        dc.l    nor_x
        dc.l    xor_x
        dc.l    or_x
#endif



/*
 * rep_x - Line Drawing Routine for Replace Mode  
 */
                           
//
//      Save the registers that are clobbered and test whether the current
//      plane should be written with the line style or 0's.
//
rep_x:
        move.w  d2,-(sp)        // save registers that get clobbered
        move.l  a5,-(sp)
        move.w  (a0),d1         // get line style or fill pattern

        adda.w  a1,a0           // update line style/fill pattern pointer
        tst.w   (a4)+           // write this plane with pattern or 0's?
        bne     r_x_lf          // use pattern
        clr.w   d1              // use 0's

//
//      Draw the left fringe.
//
r_x_lf:
#if rev_vid
        move.w  (a5),d5         // get source data
#else
        not.w   d1              // invert line style or fill pattern
        move.w  (a5),d5         // get source data
#endif

        eor.w   d1,d5           // xor the pattern with the source
        and.w   d0,d5           // isolate the bits outside the fringe
        eor.w   d1,d5           // restore the bits outside the fringe
#if vme10
        move.w  d5,(a5)+        // write the fringe and advance to next word
#else
        move.w  d5,(a5)         // write the fringe
        adda.w  a3,a5           // update pointer to next word in plane
#endif

//
//      Inner Loop -- Draw the full words contained in the line.
//
        bra     rx_lend         // test if any entire words are to be drawn
rx_ltop:
#if vme10
        move.w  d1,(a5)+        // write current word and advance to next
#else
        move.w  d1,(a5)         // write current word
        adda.w  a3,a5           // update pointer to next word in plane
#endif
rx_lend:
        dbra    d2,rx_ltop      // loop until done

//
//      Draw the right fringe.
//
        cmp.l   #-1,d7          // Do we really have to draw this part?
        beq.s   rx_no_right     // If not skip it! (THH)

        move.w  (a5),d5         // get source data
        eor.w   d1,d5           // xor the pattern with the source
        and.w   d7,d5           // isolate the bits outside the fringe
        eor.w   d1,d5           // restore the bits outside the fringe
        move.w  d5,(a5)         // write the fringe and advance to next word

//
//      We are now done drawing the line in the current plane.  Restore the
//      registers that were clobbered, update the pointer to the next plane,
//      and test whether any additional planes need to be drawn.
//
rx_no_right:
        move.l  (sp)+,a5        // restore the clobbered registers
        move.w  (sp)+,d2
#if vme10
        adda.l  a3,a5           // update pointer to next plane
#else
        addq.w  #2,a5           // update pointer to next plane
#endif
        dbra    d3,rep_x        // loop until all planes are drawn
        rts

/*
 * or_x - Line Drawing Routine for Transparent Mode    
 *
 * This mode is not to be confused with a true 'or' mode.
 */

//
// If the foreground color requires that bits in the current plane
// be set then 'or' the mask with the source.  Otherwise, the fore-
// ground color requires that bits in the current plane be cleared
// so 'and' the complement of the mask with the source.  Bits that
// would be drawn with the background color (black) are left unchanged.
//

//
// Call a subroutine common to the transparent and reverse transparent
// modes to draw the line segment.
//
or_x:
        move.w  (a0),d1         // get current scan of pattern
        bsr     or_nor          // draw the line segment
        dbra    d3,or_x         // loop until all planes drawn
        rts



/*
 * nor_x - Line Drawing Routine for Reverse Transparent Mode    
 */
                           
//
//      Get the one's complement of the pattern and call a subroutine
//      common to the transparent and reverse transparent modes to draw
//      the line segment.
//
nor_x:
        move.w  (a0),d1         // get current scan of pattern
        not.w   d1              // get one's complement of the pattern
        bsr     or_nor          // draw the line segment
        dbra    d3,nor_x        // loop until all planes drawn
        rts



/*
 * or_nor - General Purpose Line Drawing Routine
 *
 * For Both Transparent and Reverse Transparent Writing Modes  
 */

//
//      Save the registers that get clobbered and test whether the current
//      plane should be set or cleared for the current foreground color.
//
or_nor:
        movem.l d2/a5,-(sp)     // save registers that get clobbered
        adda.w  a1,a0           // update pointer to next plane's pattern
#if !rev_vid
        not.w   d1              // get complement of mask with source
#endif
        tst.w   (a4)+           // set bits in this plane for the fgnd color?
#if rev_vid
        bne     o_x_lf1         // yes - or mask with source
#else
        beq     o_x_lf1         // yes - or mask with source
#endif
        not.w   d1              // nope - and complement of mask with source
//
//      Draw the left fringe by clearing bits in this plane.
//
o_x_lf0:
        move.w  (a5),d5         // get source data
        move.w  d5,d4           // save it
        and.w   d1,d5           // and complement of mask with source
        eor.w   d5,d4           // isolate changed bits
        and.w   d0,d4           // isolate changed bits outside of fringe
        eor.w   d4,d5           // restore them to original states
#if vme10
        move.w  d5,(a5)+        // write left fringe and step to next word
#else
        move.w  d5,(a5)         // write left fringe
        adda.w  a3,a5           // update pointer to next word in plane
#endif

//
//      Inner Loop -- Draw the full words contained in the line.
//
        bra     oxc_lend        // first test for any full words to be drawn
oxc_ltop:
#if vme10
        and.w   d1,(a5)+        // clear all bits to contain foreground color
#else
        and.w   d1,(a5)         // clear all bits to contain foreground color
        adda.w  a3,a5           // update pointer to next word in plane
#endif

oxc_lend:
        dbra    d2,oxc_ltop     // loop until done

//
//      Draw the right fringe.
//
        cmp.l   #-1,d7          // Do we really have to draw this part?
        beq.s   ox_no_right     // If not skip it! (THH)

        move.w  (a5),d5         // get source data
        move.w  d5,d4           // save it
        and.w   d1,d5           // nope - and complement of mask with source
ox_merge:
        eor.w   d5,d4           // isolate changed bits
        and.w   d7,d4           // isolate changed bits outside of pattern
        eor.w   d4,d5           // restore them to original states
        move.w  d5,(a5)         // write out right fringe

//
//      We are now done drawing the line in the current plane.  Restore the
//      registers that were clobbered, update the pointer to the next plane,
//      and test whether any additional planes need to be drawn.
//
ox_no_right:
        movem.l (sp)+,d2/a5     // restore the clobbered registers
#if vme10
        adda.l  a3,a5           // update pointer to next plane
#else
        addq.w  #2,a5           // update pointer to next plane
#endif
        rts

//
//      Draw the left fringe by setting bits in this plane.
//
o_x_lf1:
        move.w  (a5),d5         // get source data
        move.w  d5,d4           // save it
        or.w    d1,d5           // or mask with source
        eor.w   d5,d4           // isolate changed bits
        and.w   d0,d4           // isolate changed bits outside of fringe
        eor.w   d4,d5           // restore them to original states
#if vme10
        move.w  d5,(a5)+        // write left fringe and step to next word
#else
        move.w  d5,(a5)         // write left fringe
        adda.w  a3,a5           // update pointer to next word in plane
#endif

//
//      Inner Loop -- Draw the full words contained in the line.
//
        bra     oxs_lend        // first test for any full words to be drawn

oxs_ltop:
#if vme10
        or.w    d1,(a5)+        // set all bits to contain forground color
#else
        or.w    d1,(a5)         // clear all bits to contain foreground color
        adda.w  a3,a5           // update pointer to next word in plane
#endif

oxs_lend:
        dbra    d2,oxs_ltop     // loop until done

//
//      Draw the right fringe by setting bits in this plane.
//
        cmp.l   #-1,d7          // Do we really have to draw this part?
        beq.s   ox_no_right     // If not skip it! (THH)

        move.w  (a5),d5         // get source data
        move.w  d5,d4           // save it
        or.w    d1,d5           // or mask with source
        bra     ox_merge        // jump to common code for setting and clearing



/*
 * xor_x - Line Drawing Routine for Exclusive Or Mode  
 */

//
//      Save the registers that get clobbered.
//
xor_x:
        movem.l d2/a5,-(sp)     // save registers that get clobbered
        move.w  (a0),d1         // get line style or fill pattern
        adda.w  a1,a0           // update line style/fill pattern pointer

//
//      Draw the left fringe.
//
x_x_lf:
        move.w  (a5),d5         // get the source
        move.w  d5,d4           // save it
        eor.w   d1,d5           // xor the pattern with the source
        eor.w   d5,d4           // xor result with source - now have pattern
        and.w   d0,d4           // isolate changed bits outside of fringe
        eor.w   d4,d5           // restore states of bits outside of fringe
#if vme10
        move.w  d5,(a5)+        // write left fringe and advance to next word
#else
        move.w  d5,(a5)         // write left fringe
        adda.w  a3,a5           // update pointer to next word in plane
#endif

//
//      Inner Loop -- Draw the full words contained in the line.
//
        bra     xx_lend         // test if any entire words are to be drawn

xx_ltop:
#if vme10
        eor.w   d1,(a5)+        // draw current word and advance to next
#else
        eor.w   d1,(a5)         // draw current word
        adda.w  a3,a5           // update pointer to next word in plane
#endif
xx_lend:
        dbra    d2,xx_ltop      // loop until done

//
//      Draw the right fringe.
//
        cmp.l   #-1,d7          // Do we really have to draw this part?
        beq.s   xx_no_right     // If not skip it! (THH)

        move.w  (a5),d5         // get the source
        move.w  d5,d4           // save it
        eor.w   d1,d5           // xor the pattern with the source
        eor.w   d5,d4           // xor result with source - now have pattern
        and.w   d7,d4           // isolate changed bits outside of fringe
        eor.w   d4,d5           // restore states of bits outside of fringe
        move.w  d5,(a5)         // write out right fringe

//
//      We are now done drawing the line in the current plane.  Restore the
//      registers that were clobbered, update the pointer to the next plane,
//      and test whether any additional planes need to be drawn.
//
xx_no_right:
        movem.l (sp)+,d2/a5     // restore the clobbered registers
#if vme10
        adda.l  a3,a5           // update pointer to next plane
#else
        addq.w  #2,a5           // update pointer to next plane
#endif
        dbra    d3,xor_x        // loop until all bit planes are drawn
        rts

        .end
