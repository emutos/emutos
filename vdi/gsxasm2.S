/*
 * gsxasm2.S - Graphics kernel part 2
 *
 * Copyright (c) 1999 Caldera, Inc. and Authors:
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "vdiconf.h"


/* Globally used labels */

                .globl   _end_pts

/* Externally defined labels, which are referenced here */

                .xdef    concat

                .xdef   _search_color
                .xdef   _seed_type
                .xdef   _v_bas_ad
                .xdef   _v_planes

                .xdef   _XMN_CLIP
                .xdef   _XMX_CLIP
                .xdef   _YMN_CLIP
                .xdef   _YMX_CLIP


                .text                   // open program segment.



//////////////
//////////////
////                                                                          ////
////    _end_pts:                                                             ////
////            This routine finds the endpoints of a section of solid color  ////
////    for the _seed_fill routine.                                           ////
////                                                                          ////
////            input:  4(sp) = xstart.                                       ////
////                    6(sp) = ystart.                                       ////
////                    8(sp) = ptr to endxleft.                              ////
////                    C(sp) = ptr to endxright.                             ////
////                                                                          ////
////            output: endxleft  := left endpoint of solid color.            ////
////                    endxright := right endpoint of solid color.           ////
////                    d0        := success flag.                            ////
////                                    0 => no endpoints or xstart on edge.  ////
////                                    1 => endpoints found.                 ////
////                                                                          ////
////            destroys: everything.                                         ////
////                                                                          ////
//////////////
//////////////
//
//
_end_pts:       move.w  6(sp),d1        // fetch y coordinate. (input to concat)
                cmp.w   _YMN_CLIP, d1   // Clip to the present viewport
                bmi     ep_ret0         // if above the top, return 0.

                cmp.w   _YMX_CLIP,d1
                bgt     ep_ret0         // if below the bottom, return 0.
//
                move.l  _v_bas_ad,a5    // fetch video base address.
#if vme10
                move.w  _v_planes,d3    // d3 := # of planes
                subq.w  #1,d3           // d3 := (# of planes) -1.
                move.l  _v_pl_dspl,a3   // # of bytes between video planes
#else
                move.l  #0,a3
                move.w  _v_planes,a3    // a3 := xinc. (# of planes)
                move.w  a3,d3
                adda.w  a3,a3           // word xinc.
                subq.w  #1,d3           // d3 := (# of planes) -1.
#endif
                move.w  4(sp),d0        // fetch x coordinate. (input to concat)
                move.w  d0,d7           // save for finding right endpoints
                jsr     concat          // form physical offset.

                adda.l  d1,a5           // base + physical offset.
//
//      d4 = scratch.                   a5 = ptr to word containing low-order pixel bit.
//      d3 = # of bit_planes - 1        a4 = scratch.
//      d1 = scratch.
//      d0 = x coord and 0x000F.
//
                add.w   d0,d0           // word indexing.
                lea     ortbl,a4        // table of pixel masks.
                move.w  0(a4,d0.w),d0   // fetch the pixel mask.
                move.w  d0,a0           // save the pixel mask.
                moveq.l #0,d1           // clear the pixel value accumulator.
                move.w  d3,a1           // save # of bit_planes - 1.
                move.l  a5,a2           // save pixel word pointer.
#if vme10
                moveq.l #1,d6           // init for faster shift and or instrs.
#else
                adda.w  a3,a5           // point to highest-order bit_plane. (+2)
#endif
                bra     pxl_in
//
//  Get color value of requested pixel.
//
px_lp:
#if vme10
                asl.w   d6,d1           // shift accumulator for next bit_plane.
                adda.l  a3,a5           // point to same word in next bit plane
pxl_in:         move.w  (a5),d4         // fetch a bit.
#else
                asl.w   #1,d1           // shift accumulator for next bit_plane.
pxl_in:         move.w  -(a5),d4        // fetch a bit.
#endif
                and.w   d0,d4           // test the bit.
                beq     pxl_bot         // if 0, branch.
#if vme10
                or.w    d6,d1           // if 1, set value accumulator bit.
#else
                ori.w   #1,d1           // if 1, set value accumulator bit.
#endif
pxl_bot:        dbra    d3,px_lp
//
                move.l  a2,a5           // restore the pixel word pointer.
                move.w  a1,d3           // restore the # of bit_planes - 1.
#if vme10
                move.l  a6,-(sp)        // save 'C' frame pointer
                movea.w _XMX_CLIP,a6    // fetch right side of x viewport
#else
                move.w  _XMX_CLIP,d6    // fetch right side of x viewport
#endif
                move.l  a5,-(sp)        // push a5. (this adds 4 to the parameter offsets)

//      d7 = x coordinate.
////////
// for VME10 software:                                                      //
//      d6 = 1 for faster instr. types  a6 = right side of viewport         //
////////
//      d6 = right side of viewport
//      d5 = scratch.                   a5 = ptr to word containing low-order pixel bit.
//      d4 = scratch.                   a4 = scratch.
//      d3 = # of bit_planes - 1        a3 = xinc.
//////////
//       for VME10 software              a3 = # of bytes between video planes //
//////////
//      d2 = color we are looking for   a2 = saved a5.
//      d1 = pixel value accumulator.   a1 = saved d3.
//      d0 = pixel mask.                a0 = saved d0.

                move.w  d1,d2           // this is the color we are searching for
                bra     rnl1            // branch around redundant test.
//
rnedg_lp:       cmp.w   d1,d2           // while (pixel value <> search color)
                bne     rt_nedge
rnl1:
#if vme10
                cmp.w   a6,d7           // and (x coord < x resolution).
#else
                cmp.w   d6,d7           // and (x coord < x resolution).
#endif
                bgt     rt_nedge

                addq.w  #1,d7           // increment x coord.
#if vme10
                ror.w   d6,d0           // "increment" the pixel mask and a5.
#else
                ror.w   #1,d0           // "increment" the pixel mask and a5.
#endif
                bcc     rn_gpx          // if no wrap, branch.
#if vme10
                addq.w  #2,a5           // point to next word in same bit_plane 
#else
                adda.w  a3,a5           // else point to next bit_plane 0.
#endif
rn_gpx:         movea.l a5,a2           // save a5.

//      Now, search to the right.

                moveq.l #0,d1           // clear the pixel value.
#if !vme10
                adda.w  a3,a5           // point to highest-order bit_plane. (+2)
#endif
                bra     rnpxl_in

#if vme10
rnpx_lp:        asl.w   d6,d1           // shift accumulator for next bit_plane.
                adda.l  a3,a5           // point to same word in next bit_plane.
rnpxl_in:       move.w  (a5),d4         // fetch a bit
#else
rnpx_lp:        asl.w   #1,d1           // shift accumulator for next bit_plane.
rnpxl_in:       move.w  -(a5),d4        // fetch a bit.
#endif
                and.w   d0,d4           // test the bit.
                beq     rnpxl_bot       // if 0, branch.
#if vme10
                or.w    d6,d1           // if 1, set value accumulator bit.
#else
                ori.w   #1,d1           // if 1, set value accumulator bit.
#endif            
rnpxl_bot:      dbra    d3,rnpx_lp
                move.l  a2,a5           // restore the pixel word pointer.
                move.w  a1,d3           // restore the # of bit_planes - 1.
                bra     rnedg_lp

rt_nedge:       move.l  (sp)+,a5        // pop a5. (this subtracts 4 from offsets)
#if !vme10
                adda.w  a3,a5           // to use predecrement addressing.
#endif
                subq.w  #1,d7           // backup the x coord by 1.
#if vme10
                movea.l 16(sp),a4       // fetch ptr to endxright.
#else
                movea.l 12(sp),a4       // fetch ptr to endxright.
#endif
                move.w  d7,(a4)         // output x coord to endxright.
                move.l  a5,a2           // save a5.
#if vme10
                move.w  8(sp),d7        // fetch x coord again.
                move.w  _XMN_CLIP, a6   // fetch left side of x viewport
#else
                move.w  4(sp),d7        // fetch x coord again.
                move.w  _XMN_CLIP, d6   // fetch left side of x viewport
#endif
                move.w  a0,d0           // restore pixel mask.

//      Now, search to the left.

//      d7 = x coordinate.
////////
// for VME10 software:                                                      //
//      d6 = 1 for faster instr. types  a6 = left side of viewport          //
////////
//      d6 = left side of x viewport
//      d5 = scratch.                   a5 = ptr to word containing low-order pixel bit.
//      d4 = scratch.                   a4 = scratch.
//      d3 = # of bit_planes - 1        a3 = xinc.
//////////
//       for VME10 software             a3 = # of bytes between video planes //
//////////
//      d2 = color we are looking for   a2 = saved a5.
//      d1 = pixel value accumulator.   a1 = saved d3.
//      d0 = pixel mask.                a0 = saved d0.

                bra     lnlp1           // d1 isn't ready yet.

lnedg_lp:       cmp.w   d1,d2           // while (pixel value <> search color)
                bne     lt_nedge
lnlp1:          
#if vme10
                cmp.w   a6, d7          // and (x coord inside viewport).
#else
                cmp.w   d6, d7          // and (x coord inside viewport).
#endif
                bmi     lt_nedge
                subq.w  #1,d7           // decrement x coord.
                rol.w   d6,d0           // "decrement" the pixel mask and a5.
                bcc     ln_gpx          // if no wrap, branch.
#if vme10
                subq.w  #2,a5           // point to next word in same bit_plane 
#else
                suba.w  a3,a5           // else point to previous bit_plane 0.
#endif
ln_gpx:         movea.l a5,a2           // save a5.

//      Now, get the next pixel value.

                moveq.l #0,d1           // clear the pixel value.
                bra     lnpxl_in

lnpx_lp:
#if vme10
                asl.w   d6,d1           // shift accumulator for next bit_plane.
                adda.l  a3,a5           // point to same word in next bit_plane.
lnpxl_in:       move.w  (a5),d4         // fetch a bit.
#else
                asl.w   #1,d1           // shift accumulator for next bit_plane.
lnpxl_in:       move.w  -(a5),d4        // fetch a bit.
#endif
                and.w   d0,d4           // test the bit.
                beq     lnpxl_bot       // if 0, branch.
#if vme10
                or.w    d6,d1           // if 1, set value accumulator bit.
#else
                ori.w   #1,d1           // if 1, set value accumulator bit.
#endif
lnpxl_bot:      dbra    d3,lnpx_lp
                move.l  a2,a5           // restore the pixel word pointer.
                move.w  a1,d3           // restore the # of bit_planes - 1.
                bra     lnedg_lp

lt_nedge:       addq.w  #1,d7           // backup the x coord by 1.
#if vme10
                movea.l (sp)+,a6        // restore 'C' frame pointer
#endif    
                movea.l 8(sp),a4        // fetch ptr to endxleft.
                move.w  d7,(a4)         // output x coord to endxleft.

                cmp.w   _search_color, d2
                beq     ep_is_search

                move.w  _seed_type, d0  // return segment not of search color
                eori.w  #1,d0
                rts

ep_is_search:   move.w  _seed_type, d0  // return segment is of search color
                eori.w  #0,d0
                rts

ep_ret0:        moveq.l #0,d0           // return(1). (success)
                rts



/*
 * OR Mask Table for line drawing
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
