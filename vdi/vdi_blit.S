/*
 * vdi_blit.S - Assembler implementation of blitting
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "vdi_asm.h"


#ifndef __mcoldfire__

        .globl  _bit_blt

        .xdef  _info

#define    F1OP       8
#define    F2OP      22

/* Frame parameters - just for copyfrm.S and bitblt.S */

#define FRAME_LEN   76

#define B_WD       -76  // +00 width of block in pixels
#define B_HT       -74  // +02 height of block in pixels

#define PLANE_CT   -72  // +04 number of consequitive planes to blt

#define FG_COL     -70  // +06 foreground color (logic op table index:hi bit)
#define BG_COL     -68  // +08 background color (logic op table index:lo bit)
#define OP_TAB     -66  // +10 logic ops for all fore and background combos
#define S_XMIN     -62  // +14 minimum X: source
#define S_YMIN     -60  // +16 minimum Y: source
#define S_FORM     -58  // +18 source form base address

#define S_NXWD     -54  // +22 offset to next word in line  (in bytes)
#define S_NXLN     -52  // +24 offset to next line in plane (in bytes)
#define S_NXPL     -50  // +26 offset to next plane from start of current plane

#define D_XMIN     -48  // +28 minimum X: destination
#define D_YMIN     -46  // +30 minimum Y: destination
#define D_FORM     -44  // +32 destination form base address

#define D_NXWD     -40  // +36 offset to next word in line  (in bytes)
#define D_NXLN     -38  // +38 offset to next line in plane (in bytes)
#define D_NXPL     -36  // +40 offset to next plane from start of current plane

#define P_ADDR     -34  // +42 address of pattern buffer   (0:no pattern)
#define P_NXLN     -30  // +46 offset to next line in pattern  (in bytes)
#define P_NXPL     -28  // +48 offset to next plane in pattern (in bytes)
#define P_MASK     -26  // +50 pattern index mask

/* these frame parameters are internally set */

#define P_INDX     -24  // +52 initial pattern index

#define S_ADDR     -22  // +54 initial source address
#define S_XMAX     -18  // +58 maximum X: source
#define S_YMAX     -16  // +60 maximum Y: source

#define D_ADDR     -14  // +62 initial destination address
#define D_XMAX     -10  // +66 maximum X: destination
#define D_YMAX      -8  // +68 maximum Y: destination

#define INNER_CT    -6  // +70 blt inner loop initial count
#define DST_WR      -4  // +72 destination form wrap (in bytes)
#define SRC_WR      -2  // +74 source form wrap (in bytes)



// name:
//      bltfrag.s
//
// purpose:
//
//      these fragments may be threaded together to perform all the prescribed
//      operations of the GSX bitblt.
//
// latest update:
//
//      25-jan-85
//
//
//  in:
//      d2.w            signed offset to next word in SOURCE
//      d3.w            signed offset to next word in DESTINATION
//      d4.w            shift count
//      d5(15:00)       iterations of inner loop
//      d5(31:16)       row counter
//      d6(15:00)       initial fringe mask
//      d6(31:16)       final fringe mask
//
//
//      a0.l            points to first word of SOURCE
//      a1.l            points to first word of DESTINATION
//      a2.l            thread from inner loop to logic op fragment
//      a3.l            thread from logic op fragment back to inner loop
//      a4.l            thread from update fragment back to 1st fringe fragment
//      a6.l            frame pointer
//
//      frame offsets
//
//      SRC_WR          offset from current word to first word of next row of source
//      DST_WR          offset from current word to first word of next row of dest
//      INNER_CT        inner loop count
//




//////                             //////
//////   PATTERN PARAMETER UPDATING   //////
//////                             //////

p1_update:

        swap    d7              // d7(15:00) <- raw pattern index pointer        4
        move.w  d7,d0           // d0 <- pattern index                           4
        add.w   P_NXLN(a6),d7   // advance index counter to next line           12
        swap    d7              // d7(15:00) <- old pattern word                         4

        and.w   P_MASK(a6),d0   // d0 <- index into pattern                     12

        move.l  a6,d1           // d1 <- temporary home for frame pointer        4
        move.l  P_ADDR(a6),a6   // a6 -> top of pattern block (for this plane)   16
        move.w  0(a6,d0.w),d7   // d7 <- new pattern word                       14
        move.l  d1,a6           // a6 -> frame                                   4

        jmp     (a5)            // thread to first fringe fragment               8



//////                                         //////
//////   FIRST FRINGE FRAGMENTS:single transfer   ////// 
//////                                         //////


f0_left:

        move.w  (a0),d0         // d0(31:16) <- most recent SOURCE word          8
        rol.w   d4,d0           // d0(15:00) <- aligned word                  6-22
        jmp     F1OP(a2)        // send to DESTINATION via logic op             10



f0_right:

        move.w  (a0),d0         // d0(31:16) <- most recent SOURCE word          8
        ror.w   d4,d0           // d0(15:00) <- aligned word                  6-22
        jmp     F1OP(a2)        // send to DESTINATION via logic op             10



//////                            //////
//////   FIRST FRINGE FRAGMENTS   ////// 
//////                            //////


//      move from right to left.
//      fetch 2 Source words.
//      store 1 Destination fringe.
//      shift right.

f1_r2l_rt:

        move.w  (a0),d0         // d0 <- SOURCE 1st word                                 8
        adda.w  d2,a0           // a0 -> SOURCE 2nd word                                 8
        swap    d0              // d0(31:16) <- 1st SRC word                     4


//      move from right to left.
//      fetch 1 Source word.
//      store 1 Destination fringe.
//      shift right to achieve a net left shift.
f0_r2l_rt:

        move.w  (a0),d0         // d0(31:16) <- most recent SOURCE word          8
        swap    d0              // d0(15:00) <- 1st SOURCE word or garbage       4

        move.l  d0,d1           // d1(31:16) <- set up for INNER LOOP            4

        ror.l   d4,d0           // d0(15:00) <- aligned right fringe          8-24

        jmp     F1OP(a2)        // send to DESTINATION via logic op             10




//      move from right to left.
//      fetch 2 Source words.
//      store 1 Destination fringe.
//      rotate left to achieve a net right shift.

f1_r2l_lf:

        move.w  (a0),d0         // d0 <- 1st SOURCE word                                 8
        adda.w  d2,a0           // a0 -> 2nd SOURCE word                                 8
        swap    d0              // d0(31:16) <- SRC word                                 4


//      move from right to left.
//      fetch 1 Source word.
//      store 1 Destination fringe.
//      rotate left.
f0_r2l_lf:
        move.w  (a0),d0         // d0(15:00) <- SRC wrd                          8
        move.l  d0,d1           // set up d1(31:16) for inner loop               4
        swap    d1              // d1(31:16) <- 1st word for bit pump            4

        rol.l   d4,d0           // d0(15:00) <- aligned SRC right fringe              8-24

        jmp     F1OP(a2)        // thread to logic op fragment for 1st fringe   10



//      move from left to right.
//      fetch 2 Source words.
//      store 1 Destination fringe.
//      rotate right to achieve a net left shift.
f1_l2r_rt:

        move.w  (a0),d0         // d0 <- 1st SOURCE word                                 8
        adda.w  d2,a0           // a0 -> SOURCE 2nd word                                 8
        swap    d0              // d0(31:16) <- SRC 1st word (old)               4


//      move from left to right.
//      fetch 1 Source word.
//      store 1 Destination fringe.
//      rotate right.
f0_l2r_rt:

        move.w  (a0),d0         // d0(15:00) <- SRC 2nd word (new)               8
        move.l  d0,d1           // set up d1(31:16) for inner loop               4
        swap    d1              // d1(31:16) <- SOURCE 2nd word                  4

        ror.l   d4,d0           // d0(15:00) <- aligned left fringe           8-24

        jmp     F1OP(a2)        // mask and write to DESTINATION                        10



//      move from left to right.
//      fetch 2 Source words.
//      store 1 Destination fringe.
//      shift left.
f1_l2r_lf:

        move.w  (a0),d0         // d0 <- SOURCE 1st word                                 8
        adda.w  d2,a0           // a0 -> SOURCE 2nd word                                 8
        swap    d0              //                                               4


//      move from left to right.
//      fetch 1 Source word.
//      store 1 Destination fringe.
//      shift left to achieve a net right shift.
f0_l2r_lf:

        move.w  (a0),d0         // d0(31:16) <- SOURCE 2nd word (new)            8
        swap    d0              // d0(15:00) <- SOURCE 1st word (old)            4

        move.l  d0,d1           // d1(31:16) <- set up for INNER LOOP            4

        rol.l   d4,d0           // d0(15:00) <- aligned SRC left fringe       8-24

        jmp     F1OP(a2)        // write mode and store to DESTINATION          10




//////                          //////
//////   INNER LOOP FRAGMENTS   //////
//////                          //////
i1_r2l_rt:

        adda.w  d2,a0           // a0 -> new SOURCE word                                 8
        adda.w  d3,a1           // a1 -> new DESTINATION word                    8
        move.l  d1,d0           // d0(31:16) <- old SOURCE word                  4
        move.w  (a0),d0         // d0(15:00) <- old SOURCE word                  8
        swap    d0              // d0(31:16) <- new SOURCE word                  4
        move.l  d0,d1           // d1(31:16) <- new SOURCE word (unaligned)      4

        ror.l   d4,d0           // d0(15:00) <- alligned SOURCE: new|old              8-24

        jmp     (a2)            // write aligned SOURCE to DESTINATION           8


i1_r2l_lf:

        adda.w  d2,a0           // a0 -> new SOURCE word                                 8
        adda.w  d3,a1           // a1 -> new DESTINATION word                    8
        move.l  d1,d0           // d0(31:16) <- old SOURCE word                  4
        move.w  (a0),d0         // d0(15:00) <- new SOURCE word                  8
        move.l  d0,d1           // d0(31:16) <- old SOURCE word                  4
        swap    d1              // d1(31:16) <- new SOURCE word (unaligned)      4

        rol.l   d4,d0           // d0(15:00) <- alligned SOURCE: new|old              8-24

        jmp     (a2)            // write aligned SOURCE to DESTINATION           8


i1_l2r_rt:

        adda.w  d2,a0           // a0 -> new SOURCE word                                 8
        adda.w  d3,a1           // a1 -> new DESTINATION word                    8
        move.l  d1,d0           // d0(31:16) <- old SOURCE word                  4
        move.w  (a0),d0         // d0(15:00) <- new SOURCE word                  8
        move.l  d0,d1           // d0(31:16) <- old SOURCE word                  4
        swap    d1              // d1(31:16) <- new SOURCE word (unaligned)      4

        ror.l   d4,d0           // d0(15:00) <- alligned SOURCE: old|new              8-24

        jmp     (a2)            // write aligned SOURCE to DESTINATION           8


i1_l2r_lf:

        adda.w  d2,a0           // a0 -> new SOURCE word                                 8
        adda.w  d3,a1           // a1 -> new DESTINATION word                    8
        move.l  d1,d0           // d0(31:16) <- old SOURCE word                  4
        move.w  (a0),d0         // d0(15:00) <- old SOURCE word                  8
        swap    d0              // d0(31:16) <- new SOURCE word                  4
        move.l  d0,d1           // d1(31:16) <- new SOURCE word (unaligned)      4

        rol.l   d4,d0           // d0(15:00) <- alligned SOURCE: old|new              8-24

        jmp     (a2)            // write aligned SOURCE to DESTINATION           8






//////                                                          //////
//////   INNER LOOP COUNTER and SECOND FRINGE REQUIRING FETCH   //////
//////                                                          //////

f2_r2l_rt:

        dbra    d5,i1_r2l_rt    // do another INNER LOOP cycle             (10)/14
        adda.w  d2,a0           // a0 -> last SOURCE word                        8

        move.l  d1,d0           // d0(31:16) <- next to last SOURCE word                 4
        move.w  (a0),d0         // d0(15:00) <- last SOURCE word                         8
        ror.l   d4,d0           // d0(31:16) <- alligned SRC: new|old         8-24

        bra     f2_flush        // send fringe to DESTINATION                   10



f2_r2l_lf:

        dbra    d5,i1_r2l_lf    // do another INNER LOOP cycle             (10)/14
        adda.w  d2,a0           // a0 -> new SOURCE word                                 8

        move.l  d1,d0           // d0(31:16) <- next to last SOURCE word                 4
        move.w  (a0),d0         // d0(15:00) <- last SOURCE word                         8
        rol.l   d4,d0           // d0(15:00) <- alligned SRC: new|old         8-24

        bra     f2_out          // send fringe to DESTINATION                   10



f2_l2r_rt:

        dbra    d5,i1_l2r_rt    // do another INNER LOOP cycle             (10)/14

        adda.w  d2,a0           // a0 -> new SOURCE word                                 8

        move.l  d1,d0           // d0(31:16) <- next to last SOURCE word                 4
        move.w  (a0),d0         // d0(15:00) <- last SOURCE word                         8
  
        ror.l   d4,d0           // d0(15:00) <- alligned SRC: old|new         8-24

        bra     f2_out          // send fringe to DESTINATION                   10



f2_l2r_lf:

        dbra    d5,i1_l2r_lf    // do another INNER LOOP cycle             (10)/14

        adda.w  d2,a0           // a0 -> new SOURCE word                                 8

        move.l  d1,d0           // d0(31:16) <- next to last SOURCE word                 4
        move.w  (a0),d0         // d0(15:00) <- last SOURCE word                         8

        rol.l   d4,d0           // d0(31:16) <- alligned SRC left fringe              8-24

        bra     f2_flush        // send fringe to DESTINATION                   10




//////                                                          //////
//////   INNER LOOP COUNTER and SECOND FRINGE REQUIRING FLUSH   //////
//////                                                          //////

fl_r2l_rt:

        dbra    d5,i1_r2l_rt    // do another INNER LOOP cycle             (10)/14
        bra     f2_flush        //                                              10

fl_r2l_lf:

        dbra    d5,i1_r2l_lf    // do another INNER LOOP cycle             (10)/14
        bra     f2_flush        //                                              10

fl_l2r_rt:

        dbra    d5,i1_l2r_rt    // do another INNER LOOP cycle             (10)/14
        bra     f2_flush        //                                              10

fl_l2r_lf:

        dbra    d5,i1_l2r_lf    // do another INNER LOOP cycle             (10)/14

////////        bra     f2_flush        // fall through to f2_flush                     10





//////                                         //////
//////   FINAL FRINGE OUTPUT and ROW CONTROL   //////
//////                                         //////

f2_flush:

        swap    d0              // d0(15:00) <- the target fringe                4

f2_out:
        adda.w  d3,a1           // a1 -> last DESTINATION word                   8

        swap    d6              // d6(15:00) <- 2nd fringe mask                  4
        jmp     F2OP(a2)        // process 2nd fringe                                   10
f2_rtn:
        swap    d6              // d6(15:00) <- 1st fringe mask                  4

        move.w  INNER_CT(a6),d5 // reinit inner loop counter                    12

f2_update:

        swap    d5              // access row counter                            4
        subq.w  #1,d5           // decrement row counter                                 4
        beq     f2_end          //                                          (10)/8
        swap    d5              //                                               4

        adda.w  SRC_WR(a6),a0   // a0 -> last word of next line: SRC            16
        adda.w  DST_WR(a6),a1   // a1 -> last word of next line: DST            16
  
        jmp     (a4)            // thread to first fringe fragment               8


f2_end: rts





//////                                  //////
//////   UNARY ALTERATION OF DESTINATION   //////
//////  logic ops: 00, 05, 10, 15       //////
//////                                  //////

f1_dst:
        jmp     F1OP(a2)        // write 1st fringe                             10

i1_dst:
        adda.w  d3,a1           // a1 -> new DESTINATION word                    8
        jmp     (a2)            // write to DESTINATION                          8

f2_dst:
        dbra    d5,i1_dst       // do another INNER LOOP cycle             (10)/14

        adda.w  d3,a1           // a1 -> last DESTINATION word                   8
        swap    d6              // d6(15:00) <- 2nd fringe mask                  4
        jmp     F2OP(a2)        // process 2nd fringe                                   10
f2_drt: swap    d6              // d6(15:00) <- 1st fringe mask                  4

        move.w  INNER_CT(a6),d5 // reinit inner loop counter                    12

f1_drt:
        swap    d5              //                                               4
        subq.w  #1,d5           // decrement row counter                                 4
        beq     f2_end          //                                          (10)/8
        swap    d5              //                                               4

        adda.w  DST_WR(a6),a1   // a1 -> initial word of next line: DST         16
  
        jmp     (a4)            // thread to first fringe fragment               8





//////                                       //////
//////  INNER LOOP WRITING MODE FRAGMENTS    //////
//////                                       //////

//  in:
//      a1.l    points to destination word
//      a3.l    thread to next fragment
//      d0.w    contains aligned source

//////                                         //////
//////  FIRST FRINGE WRITING MODE FRAGMENTS    //////
//////                                         //////

//  in:
//      a1.l    points to destination word
//      a3.l    thread to next fragment
//      d0.w    contains aligned source
//      d1.w    scratch
//      d6.w    fringe mask

//////                                          //////
//////  SECOND FRINGE WRITING MODE FRAGMENTS    //////
//////                                          //////

//  in:
//      a1.l    points to destination word
//      d0.w    contains aligned source
//      d1.w    scratch
//      d6.w    fringe mask
//
//              this fragment threads to "f2_flush"

//  NOTE: all "f1op_xx" fragments are offset 06 from "mode_xx" fragments
//        all "f2op_xx" fragments are offset 18 from "mode_xx" fragments


        and.w   d7,d0           // S and P                                       4
mode_00:
        clr.w   (a1)            // D' <- S=0                                    12
        jmp     (a3)            //                                               8
        nop                     // Must be here to make fixed length block
        and.w   d7,d0           // S and P                                       4
f1op_00:
        not.w   d6              // D' <- 0                                       4
        and.w   d6,(a1)         //                                              12
        not.w   d6              //                                               4
        jmp     (a3)            //                                               8

        ds.w    2

        and.w   d7,d0           // S and P                                       4
f2op_00:
        not.w   d6              // D' <- 0
        and.w   d6,(a1)
        not.w   d6
        bra     f2_drt          // //%&0x@!




        and.w   d7,d0           // S and P                                       4
mode_01:
        and.w   d0,(a1)         // D' <- S and D                                12
        jmp     (a3)            //                                               8

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f1op_01:
        not.w   d6
        or.w    d6,d0
        not.w   d6
        and.w   d0,(a1)
        jmp     (a3)

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f2op_01:
        not.w   d6
        or.w    d6,d0
        not.w   d6
        and.w   d0,(a1)
        bra     f2_rtn



        and.w   d7,d0           // S and P                                       4
mode_02:
        not.w   (a1)            // D' <- S and [not D]                          12
        and.w   d0,(a1)         //                                              12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_02:
        not.w   d0
        and.w   d6,d0   
        or.w    d0,(a1)         // D' <- not {[not S] or D}
        eor.w   d6,(a1)
        jmp     (a3)

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f2op_02:
        not.w   d0
        and.w   d6,d0   
        or.w    d0,(a1)         // D' <- not {[not S] or D}
        eor.w   d6,(a1)
        bra     f2_rtn



        and.w   d7,d0           // S and P                                       4
mode_03:
        move.w  d0,(a1)         // D' <- S                                       8
        jmp     (a3)            //                                               8

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f1op_03:
        move.w  (a1),d1         // D' <- S
        eor.w   d1,d0
        and.w   d6,d0
        eor.w   d1,d0
        move.w  d0,(a1)
        jmp     (a3)

        and.w   d7,d0           // S and P                                       4
f2op_03:
        move.w  (a1),d1         // D' <- S
        eor.w   d1,d0
        and.w   d6,d0
        eor.w   d1,d0
        move.w  d0,(a1)
        bra     f2_rtn


        and.w   d7,d0           // S and P                                       4
mode_04:
        not.w   d0              // D' <- [not S] and D                           4
        and.w   d0,(a1)         //                                              12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_04:
        and.w   d6,d0
        not.w   d0
        and.w   d0,(a1)
        jmp     (a3)

        ds.w    2

        and.w   d7,d0           // S and P                                       4
f2op_04:
        and.w   d6,d0
        not.w   d0
        and.w   d0,(a1)
        bra     f2_rtn




mode_05:                        // dummy label. Never called!!!!
        

        and.w   d7,d0           // S and P                                       4
mode_06:
        eor.w   d0,(a1)         // D' <- S xor D                                12
        jmp     (a3)            //                                               8

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f1op_06:
        and.w   d6,d0
        eor.w   d0,(a1)
        jmp     (a3)

        ds.w    3

        and.w   d7,d0           // S and P                                       4
f2op_06:
        and.w   d6,d0
        eor.w   d0,(a1)
        bra     f2_rtn



        and.w   d7,d0           // S and P                                       4
mode_07:
        or.w    d0,(a1)         // D' <- S or D                                 12
        jmp     (a3)            //                                               8

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f1op_07:
        and.w   d6,d0
        or.w    d0,(a1)
        jmp     (a3)

        ds.w    3

        and.w   d7,d0           // S and P                                       4
f2op_07:
        and.w   d6,d0
        or.w    d0,(a1)
        bra     f2_rtn


        and.w   d7,d0           // S and P                                       4
mode_08:
        or.w    d0,(a1)         // D' <- not [S or D]                           12
        not.w   (a1)            //                                              12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_08:
        and.w   d6,d0
        or.w    d0,(a1)
        eor.w   d6,(a1)
        jmp     (a3)

        ds.w    2

        and.w   d7,d0           // S and P                                       4
f2op_08:
        and.w   d6,d0
        or.w    d0,(a1)
        eor.w   d6,(a1)
        bra     f2_rtn


        and.w   d7,d0           // S and P                                       4
mode_09:
        not.w   d0              // D' <- [not S] xor D                           4
        eor.w   d0,(a1)         //                                              12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_09:
        and.w   d6,d0
        eor.w   d0,(a1)
        eor.w   d6,(a1)
        jmp     (a3)

        ds.w    2

        and.w   d7,d0           // S and P                                       4
f2op_09:
        and.w   d6,d0
        eor.w   d0,(a1)
        eor.w   d6,(a1)
        bra     f2_rtn


        and.w   d7,d0           // S and P                                       4
mode_10:
        not.w   (a1)            // D' <- not D                                  12
        jmp     (a3)            //                                               8

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f1op_10:
        eor.w   d6,(a1)
        jmp     (a3)

        ds.w    4

        and.w   d7,d0           // S and P                                       4
f2op_10:
        eor.w   d6,(a1)
        bra     f2_drt          // %0x#@//!



        and.w   d7,d0           // S and P                                       4
mode_11:
        not.w   (a1)            // D' <- S or [not D]                           12
        or.w    d0,(a1)         //                                              12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_11:
        eor.w   d6,(a1)
        and.w   d6,d0
        or.w    d0,(a1)
        jmp     (a3)

        ds.w    2

        and.w   d7,d0           // S and P                                       4
f2op_11:
        eor.w   d6,(a1)
        and.w   d6,d0
        or.w    d0,(a1)
        bra     f2_rtn



        and.w   d7,d0           // S and P                                       4
mode_12:
        not.w   d0              // D' <- not S                                   4
        move.w  d0,(a1)         //                                               8
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_12:
        or.w    d6,(a1)
        and.w   d6,d0
        eor.w   d0,(a1)
        jmp     (a3)

        ds.w    2

        and.w   d7,d0           // S and P                                       4
f2op_12:
        or.w    d6,(a1)
        and.w   d6,d0
        eor.w   d0,(a1)
        bra     f2_rtn


        and.w   d7,d0           // S and P                                       4
mode_13:
        not.w   d0              // D' <- [not S] or D                           12
        or.w    d0,(a1)         //                                              12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_13:
        not.w   d0
        and.w   d6,d0
        or.w    d0,(a1)
        jmp     (a3)

        ds.w    2

        and.w   d7,d0           // S and P                                       4
f2op_13:
        not.w   d0
        and.w   d6,d0
        or.w    d0,(a1)
        bra     f2_rtn


        and.w   d7,d0           // S and P                                       4
mode_14:
        and.w   d0,(a1)         // D' <- not [S and D]                          12
        not.w   (a1)            //                                              12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_14:
        not.w   d0              // D' <- [not S] and [not D]
        eor.w   d6,(a1)
        and.w   d6,d0
        or.w    d0,(a1)
        jmp     (a3)

        ds.w    1

        and.w   d7,d0           // S and P                                       4
f2op_14:
        not.w   d0              // D' <- [not S] and [not D]
        eor.w   d6,(a1)
        and.w   d6,d0
        or.w    d0,(a1)
        bra     f2_rtn



        and.w   d7,d0           // S and P                                       4
mode_15:
        move.w  #0xFFFF,(a1)    // D' <- S=1                                    12
        jmp     (a3)            //                                               8

        and.w   d7,d0           // S and P                                       4
f1op_15:
        or.w    d6,(a1)
        jmp     (a3)

        ds.w    4

        and.w   d7,d0           // S and P                                       4
f2op_15:
        or.w    d6,(a1)
        bra     f2_drt          // %0x#@*!



//
// _bit_blt - wrapper for bit_blt
//
// It emulates the link opcode by adding the FRAME_LEN onto the a6
// register. That way the rest of the assembler stuff can stay, as
// it is for now - some of it is also used for text blitting.

_bit_blt:
        // A wrapper, so save and set up registers 
        movem.l d2-d7/a2-a6,-(sp)

        lea     _info, a6               // get info structure

        add.l   #FRAME_LEN, a6          // emulate link opcode 

        move.w  S_XMIN(a6),d0           // clipped Xmin source
        move.w  D_XMIN(a6),d2           // clipped Xmin destination
        move.w  S_XMAX(a6),d4           // clipped Xmax source
        move.w  D_XMAX(a6),d6           // clipped Xmax destination

        move.l  S_FORM(a6),a0           // a0 -> start of source form (0,0)
        move.l  D_FORM(a6),a1           // a1 -> start of dest.  form (0,0)
        move.l  _PTSIN,a2               // a2 -> PTSIN array
        
        bsr.s   bit_blt

        movem.l (sp)+,d2-d7/a2-a6
        rts

//
// bit_blt - set up parameters for bitblt and 
//           thread together the appropriate bitblt fragments
//
// in:
//      d0.w    X min source
//      d2.w    X min destination
//      d4.w    X max source
//      d6.w    X max destination
//
//      a6.l    points to frame with following parameters set:
//              points to 76 byte parameter block
//

bit_blt:

        moveq.l #0x0F,d5                        // d5 <- mod 16 mask

        move.w  d0,d1
        and.w   d5,d1                   // d1 <- Xmin src mod 16 
        move.w  d2,d3
        and.w   d5,d3                   // d3 <- Xmin dst mod 16

        moveq.l #4,d7                   // shift count to do quick divide by 16
        lsr.w   d7,d0                   // d0 <- Xmin source word offset
        lsr.w   d7,d2                   // d2 <- Xmin destination word offset
        lsr.w   d7,d4                   // d4 <- Xmax source word offset
        lsr.w   d7,d6                   // d6 <- Xmax destination word offset

        sub.w   d0,d4                   // d4 <- # source words -1
        sub.w   d2,d6                   // d6 <- # destination words -1

        move.w  d4,d5
        sub.w   d6,d5                   // d5 <- # source words - # destination words 

        sub.w   d3,d1                   // d1 <- (Sxmin mod 16) - (Dxmin mod 16)        
        bne     std_blt                 // if Xmin src mod 16 = Xmin dst mod 16 ...

        tst.l   FG_COL(a6)              // and fore and background colors are both 0 ...
        bne     std_blt 
        cmpi.b  #03,OP_TAB+00(a6)       // and D'<- S is the logic operation    

        bne     std_blt         

        move.w  d4,d7                   // and both source and destination occupy
        add.w   d6,d7                   // three or more words apiece ...

        cmpi.w  #4,d7
        bcc     word_blt                // do the special word aligned blt


std_blt:

//  in:
//      d1.w    (source Xmin mod 16)-(destination Xmin mod 16)
//      d4.w    words in source -1
//      d5.w    source words - destination words
//      d6.w    words in destination -1
//
// produce:
//
//      d4.w    rotation count
//      d5.w    configuration code:
//
//                      bit  0     logical shift direction      rt:1   lf:0
//
//                      bit  1     actual shift direction:
//
//                                 1: opposite of logical shift 
//                                 0: same direction as shift
//
//
//                      bit  2     blt direction                r2l:1 l2r:0
//
//                      bit  3    source width : destination width
//                                 
//                                 0: source words =  destination words
//                                 1: source words <> destination words 
//
//              note: width delta is either 1 word or 0 words


        move.w  d6,d0                   // d6 <- destination words -1
        subq.w  #1,d0                   // d0 <- initial inner loop counter value  !@#//&?
        move.w  d0,INNER_CT(a6)

        andi.w  #1,d5                   //    0:S=D     1:S<>D

        lsl.w   #3,d5                   // 0000:S=D  1000:S<>D

        move.w  d4,d7                   // d7 <- source words -1

        move.w  d1,d4                   // d4 <- (Sxmin mod 16) - (Dxmin mod 16)
        move.w  d1,d2                   // d2 might be used as tie breaker


//   set logical shift direction

        tst.w   d4
        bgt     act_shift               // Smod16 - Dmod16 > 0 => logical left shift
        beq     start_addr              // zero is special case (pure left rotate)

        neg.w   d4                      // d4 <- positive shift value
        addq.w  #1,d5                   // Smod16 - Dmod16 < 0 => logical right shift

act_shift:      
        cmpi.w  #8,d4
        blt     start_addr

        addq.w  #2,d5                   // actual shift is opposite direction
        neg.w   d4                      // from logical shift direction

        addi.w  #16,d4

start_addr:

//   calculate starting addresses for source and destination. use these
//   addresses to determine direction of transfer.

        move.w  S_XMIN(a6),d0           // compute address of destination block
        move.w  S_YMIN(a6),d1
        bsr     s_xy2addr               // a0 -> start of block

        mulu    S_NXWD(a6),d7           // d7 <- row width offset (source)
        sub.w   d3,d7                   // d7 <- SRC_WR (for r2l xfer)


        move.w  D_XMIN(a6),d0           // compute address of destination block
        move.w  D_YMIN(a6),d1
        move.w  d1,P_INDX(a6)           // save destination Y for pattern index
        bsr     d_xy2addr               // a1 -> start of block

        mulu    D_NXWD(a6),d6           // d6 <- row width offset (destination)

        sub.w   d3,d6                   // d6 <- DST_WR (row width-form wrap: r2l xfer)

        cmp.l   a1,a0                   // a0 -> start of source block
        bhi     t2b_l2r                 // SRC addr > DST addr  =>  t2b/l2r
        bne     b2t_r2l                 // SRC addr < DST addr  =>  b2t/r2l

        tst.w   d2                      // position within word => xfer direction
        bge     t2b_l2r                 // t2b/l2r when source addr = destination addr


b2t_r2l:

        addq.w  #4,d5                   // set the "right to left" flag

        move.w  S_XMAX(a6),d0           // compute address of source low right corner
        move.w  S_YMAX(a6),d1
        bsr     s_xy2addr               // a0 -> end of source block

        move.w  D_XMAX(a6),d0           // compute address of dest low right corner
        move.w  D_YMAX(a6),d1
        move.w  d1,P_INDX(a6)           // save destination Y for pattern index
        bsr     d_xy2addr               // a1 -> end of destination block

        neg.w   P_NXLN(a6)              // reverse direction of pattern traversal

        bra     big_fringe


t2b_l2r:

        neg.w   d7                      // d7 <- SRC_WR (form wrap-row width: l2r xfer)
        neg.w   d6                      // d6 <- DST_WR


big_fringe:

        move.w  d7,SRC_WR(a6)           // source wrap
        move.w  d6,DST_WR(a6)           // destination wrap

        bsr     get_fringe

//
//  Without expanding jump tables and adding duplicate code to BLTFRAG.S
//  predecrement and postincrement addressing modes can't be used to
//  manipulate the destination address.
//
        move.w  S_NXWD(a6),d2           // d2 <- source increment
        move.w  D_NXWD(a6),d3           // d3 <- destination increment

        btst.l  #2,d5                   // r2l case => swap fringes
        beq     save_addr               // and negate inter word increments

        swap    d6

        neg.w   d2
        neg.w   d3

save_addr:

        move.l  a0,S_ADDR(a6)           // save source address for other planes
        move.l  a1,D_ADDR(a6)           // save destination address for other planes

        asl.w   #3,d5                   // d5(07:00) offset to fragment record

        move.l  frag_tab+04(pc,d5.w),a3 // a3 <- thread from logic op frag to inner loop
        move.l  frag_tab+00(pc,d5.w),a4 // a4 <- thread from update frag to 1st fringe
        
        tst.w   INNER_CT(a6)            // INNER_CT = -1  =>  Destination is 
        bge     pre_flight              // only one word wide. (a special case)

        move.l  d6,d0
        swap    d0
        and.w   d0,d6                   // d6(15:00) <- single word fringe mask

        lea     f2_update,a3            // a3 <- thread that bypasses 2nd fringe

        btst.l  #6,d5                   
        bne     pre_flight              // skip if source is 2 words wide

        lsr.w   #1,d5                   // entries are 4 bytes wide

        andi.w  #0xC,d5

        move.l  solo_tab(pc,d5.w),a4    // a4 <- thread from update frag to 1st fringe
        bra     pre_flight


solo_tab:

        dc.l    f0_left                 // no reverse logical left  physical left
        dc.l    f0_right                // no reverse logical right physical right
        dc.l    f0_right                //    reverse logical left  physical right
        dc.l    f0_left                 //    reverse logical right physical left


                                                   
//      0000:l2rf1fll  0001:l2rf0f2r  0010:l2rf1flr  0011:l2rf0f2l
//      0100:r2lf0f2l  0101:r2lf1flr  0110:r2lf0f2r  0111:r2lf1fll
//      1000:l2rf1f2l  1001:l2rf0flr  1010:l2rf1f2r  1011:l2rf0fll
//      1100:r2lf1f2l  1101:r2lf0flr  1110:r2lf1f2r  1111:r2lf0fll
                                                   
frag_tab:                                          
//                  a4        a3

        dc.l    f1_l2r_lf,fl_l2r_lf     // l2rf1fll
        dc.l    f0_l2r_rt,f2_l2r_rt     // l2rf0f2r
        dc.l    f1_l2r_rt,fl_l2r_rt     // l2rf1flr
        dc.l    f0_l2r_lf,f2_l2r_lf     // l2rf0f2l

        dc.l    f0_r2l_lf,f2_r2l_lf     // r2lf0f2l
        dc.l    f1_r2l_rt,fl_r2l_rt     // r2lf1flr
        dc.l    f0_r2l_rt,f2_r2l_rt     // r2lf0f2r
        dc.l    f1_r2l_lf,fl_r2l_lf     // r2lf1fll

        dc.l    f1_l2r_lf,f2_l2r_lf     // l2rf1f2l
        dc.l    f0_l2r_rt,fl_l2r_rt     // l2rf0flr
        dc.l    f1_l2r_rt,f2_l2r_rt     // l2rf1f2r
        dc.l    f0_l2r_lf,fl_l2r_lf     // l2rf0fll

        dc.l    f1_r2l_lf,f2_r2l_lf     // r2lf1f2l
        dc.l    f0_r2l_rt,fl_r2l_rt     // r2lf0flr
        dc.l    f1_r2l_rt,f2_r2l_rt     // r2lf1f2r
        dc.l    f0_r2l_lf,fl_r2l_lf     // r2lf0fll


pre_flight:

        tst.l   P_ADDR(a6)              // pattern and source ?
        beq     next_plane              // no pattern if pointer is null

        lea     p1_update,a5            // a4 -> pattern controller
        exg     a5,a4                   // a5 -> first fringe

        move.w  P_NXLN(a6),d0           // set up initial pattern line index value:
        bge     first_index

        neg     d0

first_index:

        mulu    P_INDX(a6),d0           // initial Y * delta Y
        move.w  d0,P_INDX(a6)


next_plane:

        clr.w   d0                      // select the logic op based on current
        lsr.w   FG_COL(a6)              // background and foreground color for
        addx.w  d0,d0                   // the given plane. logic ops (word wide)
        lsr.w   BG_COL(a6)              // are located sequentially in OP_TAB table
        addx.w  d0,d0                   // as fg0/bg0, fg0/bg1, fg1/bg0, and fg1/bg1

        move.b  OP_TAB(a6,d0.w),d0      // d0 <- appropriate logic op
        move.w  d0,d1
        lsl.w   #2,d1                   // d1 <- offset into logic op table
        move.l  log_op(pc,d1.w),a2      // a2 <- thread to appropriate logic op

        move.w  B_HT(a6),d5             // d5(31:16) <- row count
        swap    d5
        move.w  INNER_CT(a6),d5         // d5(15:00) <- inner loop counter

        move.l  #0x8421,d1              // logic ops 15,10,5, and 0 are special cases ($8421=%1000010000100001)
        btst    d0,d1                   // where operation is performed directly upon
        bne     unary_blt               // the destination independent of the source

        tst.l   P_ADDR(a6)              // skip this stuff if no pattern
        beq     do_the_blt
        subq.w  #2,a2                   // addr. of (pattern & source) entry for
//                                      // logic operation

        move.w  P_INDX(a6),d7           // d7(31:16) <- initial pattern line index
        swap    d7

do_the_blt:

        jsr     (a4)                    // blt the plane

np_cont:

        subq.w  #1,PLANE_CT(a6)
        beq     quit_blt

        move.l  S_ADDR(a6),a0           // a0 -> next source plane

        add.w   S_NXPL(a6),a0

        move.l  a0,S_ADDR(a6)

        move.l  D_ADDR(a6),a1           // a1 -> next destination plane

        add.w   D_NXPL(a6),a1

        move.l  a1,D_ADDR(a6)

        move.l  P_ADDR(a6),d0           // update pattern plane base pointer
        beq     next_plane              // if pointer isn't null

        move.l  d0,a2
        add.w   P_NXPL(a6),a2
        move.l  a2,P_ADDR(a6)
        bra     next_plane

quit_blt:
        rts
log_op:
        dc.l    mode_00,mode_01,mode_02,mode_03,mode_04,mode_05,mode_06,mode_07
        dc.l    mode_08,mode_09,mode_10,mode_11,mode_12,mode_13,mode_14,mode_15

unary_blt:
        movem.l a3-a4,-(sp)             // save routine threads

        lea     f1_dst,a4               // a4 -> start of unary blt
        lea     f2_dst,a3               // a3 <- logic op return thread (width >1)
        tst.w   d5                      // different thread if width =1

        bge     call_unary

        lea     f1_drt,a3               // a3 <- logic op return thread (width =1)

call_unary:

        jsr     (a4)

        movem.l (sp)+,a3-a4
        bra     np_cont


//   get fringe masks for right and left sides
//
//  in:
//      a6.l            frame pointer
//      D_XMAX(a6)      destination X max
//      D_XMIN(a6)      destination X min
//
// out:
//      d0.w            trash
//
//      d6(15:00)       left fringe mask
//      d6(31:16)       right fringe mask

get_fringe:

//   right mask first


        move.w  D_XMAX(a6),d0           // d0 <- Xmax of DESTINATION

        andi.w  #0xF,d0                 // d0 <- Xmax mod 16

        add.w   d0,d0                   // d0 <- offset to right fringe mask
        move.w  fr_r_mask(pc,d0.w),d6   // d6 <- right fringe mask

        swap    d6                      // d6(31:16) <- right fringe mask

//   now the left mask

        move.w  D_XMIN(a6),d0           // d0 <- Xmin of DESTINATION

        andi.w  #0xF,d0                 // d0 <- Xmax mod 16

        add.w   d0,d0                   // d0 <- offset to left fringe mask
        move.w  fr_l_mask(pc,d0.w),d6   // d6 <- left fringe mask

        not.w   d6                      // d6(15:00) <- left fringe mask

        rts

fr_l_mask:

        dc.w    0x0000  

fr_r_mask:

        dc.w    0x8000
        dc.w    0xC000
        dc.w    0xE000
        dc.w    0xF000
        dc.w    0xF800
        dc.w    0xFC00
        dc.w    0xFE00
        dc.w    0xFF00
        dc.w    0xFF80
        dc.w    0xFFC0
        dc.w    0xFFE0
        dc.w    0xFFF0
        dc.w    0xFFF8
        dc.w    0xFFFC
        dc.w    0xFFFE
        dc.w    0xFFFF





/***************************************************************************
 *
 *      s_xy2addr:
 *
 *              input:  d0.w =  x coordinate.
 *                      d1.w =  y coordinate.
 *                      a6.l -> frame
 *
 *              output:
 *                      d3.w =  line wrap (in bytes)
 *                      a0.l -> address of word containing x,y
 *
 *
 *      d_xy2addr:
 *
 *              input:  d0.w =  x coordinate.
 *                      d1.w =  y coordinate.
 *                      a6.l -> frame
 *
 *              output:
 *                      d3.w =  line wrap (in bytes)
 *                      a1.l -> address of word containing x,y
 *
 *
 *              physical offset =  (y*bytes_per_line) + (x/16)*word_offset
 *
 *              destroys: d0,d1
 *
 ***************************************************************************/


s_xy2addr:

        move.l  S_FORM(a6),a0           // a0 -> start of source form (0,0)
        move.w  S_NXLN(a6),d3           // d3 <- inter line offset

        lsr.w   #4,d0                   // d0 <- X word count
        mulu    S_NXWD(a6),d0           // d0 <- x portion of offset

        mulu    d3,d1                   // d1 <- y portion of offset
        add.l   d0,d1                   // d1 <- byte offset into memory form
        adda.l  d1,a0                   // a0 -> word containing (x,y)

        rts

d_xy2addr:

        move.l  D_FORM(a6),a1           // a0 -> start of destination form (0,0)
        move.w  D_NXLN(a6),d3           // d3 <- inter line offset

        lsr.w   #4,d0                   // d0 <- X word count
        mulu    D_NXWD(a6),d0           // d0 <- x portion of offset
        mulu    d3,d1                   // d1 <- y portion of offset
        add.l   d0,d1                   // d1 <- byte offset into memory form
        adda.l  d1,a1                   // a1 -> word containing (x,y)

        rts


word_blt:

//  in:
//      d4.w    words in source -1
//      d6.w    words in destination -1
//

//   1st.  get initial address of transfer and calculate wrap values

        move.w  d4,d0
        subq.w  #2,d0                   // d0 <- inner loop counter (count-1)
        move.w  d0,INNER_CT(a6)         // save initial count

        move.w  S_NXWD(a6),d5           // d5 <- next word increment (l2r src)
        mulu    d5,d4                   // d4 <- row width offset in bytes (src)
        move.w  D_NXWD(a6),d7           // d7 <- next word increment (l2r dest)
        mulu    d7,d6                   // d6 <- row width offset in bytes (dst)

        move.w  S_XMIN(a6),d0           // compute address of destination block
        move.w  S_YMIN(a6),d1
        bsr     s_xy2addr               // a0 -> start of source block

        sub.w   d3,d4                   // d4 <- SRC_WR (r2l)

        move.w  D_XMIN(a6),d0           // compute address of source block
        move.w  D_YMIN(a6),d1
        bsr     d_xy2addr               // a1 -> start of destination block

        sub.w   d3,d6                   // d6 <- DST_WR (r2l)


        cmp.l   a1,a0                   // which address is larger: source or dest
        bcc     l2r_t2b                 // select direction based on order of addresses


//
//  Source to destination copy direction will either be from:
//  left to right-top to bottom(l2r_t2b) or right to left-bottom to top(r2l_t2b)
//  The method selected is based on whether the source starting address is
//  larger than the destination starting address. The intent is not to alter
//  the source area before it is copied to the destination area.
//  
        bcc     l2r_t2b                 // select direction based on order of addresses

r2l_b2t:

        move.w  S_XMAX(a6),d0           // compute address of source low right corner
        move.w  S_YMAX(a6),d1
        bsr     s_xy2addr               // a0 -> end of source block

        move.w  D_XMAX(a6),d0           // compute address of dest low right corner
        move.w  D_YMAX(a6),d1
        bsr     d_xy2addr               // a1 -> end of destination block

        neg.w   d5                      // d2 <- next word increment (r2l source)
        neg.w   d7                      // d5 <- next word increment (r2l destination)

        bra     set_fringe

l2r_t2b:

        neg.w   d4                      // d4 <- SRC_WR (l2r)
        neg.w   d6                      // d6 <- DST_WR (l2r)

set_fringe:

        move.w  d5,d2                   // d2 <- source inter word increment
        move.w  d7,d3                   // d3 <- destination inter word increment

        move.w  d4,a4                   // source wrap       (SRC_WR)
        move.w  d6,a5                   // destination wrap  (DST_WR)

        bsr     get_fringe

        tst.w   d2                      // d2<0 => r2l. swap masks
        bpl     fringe_ok

        swap    d6

fringe_ok:

//   set up word to word increment values  

        move.l  a0,S_ADDR(a6)           // save source address for other planes
        move.l  a1,D_ADDR(a6)           // save destination address for other planes

        move.w  B_HT(a6),d7             // d7 <- row count

        move.w  PLANE_CT(a6),d4

        bra     sc_plane


//  fast word aligned blt. general case: multiple planes


sc_mp_f1:

        move.w  (a0),d0         // d0 <- 1st SOURCE word                                 8
        move.w  (a1),d1         // d1 <- 1st DESTINATION word                    8
        eor.w   d1,d0           //                                               4
        and.w   d6,d0           //                                               4
        eor.w   d1,d0           //                                               4
        move.w  d0,(a1)         // D' <- S                                       8

        adda.w  d2,a0           // a0 -> 2nd SOURCE word                                 8
        adda.w  d3,a1           // a1 -> 2nd DESTINATION word                    8

sc_mp_loop:

        move.w  (a0),(a1)       // DEST <- SOURCE                               12

        adda.w  d2,a0           // a0 -> next SOURCE word                        8
        adda.w  d3,a1           // a1 -> next DESTINATION word                   8

        dbra    d5,sc_mp_loop   // (10)/14

sc_mp_f2:

        swap    d6              // d6 <- second fringe                           4
        move.w  (a0),d0         // d0 <- SOURCE last word                        8
        move.w  (a1),d1         // d1 <- DESTINATION last word                   8
        eor.w   d1,d0           //                                               4
        and.w   d6,d0           //                                               4
        eor.w   d1,d0           //                                               4
        move.w  d0,(a1)         // D' <- S                                       8
        swap    d6              // d6 <- first fringe                            4

        add.w   a4,a0           // a0 -> next SOURCE row                                 8
        add.w   a5,a1           // a1 -> next DESTINATION row                    8

sc_mp_enter:

        move.w  INNER_CT(a6),d5 // reinitialize inner loop counter              12
        dbra    d7,sc_mp_f1     // do next row                             (10)/14


        move.w  B_HT(a6),d7     // d7 <- row counter

        move.l  S_ADDR(a6),a0   // advance to next plane
        move.l  D_ADDR(a6),a1

        add.w   S_NXPL(a6),a0
        add.w   D_NXPL(a6),a1   

        move.l  a0,S_ADDR(a6)
        move.l  a1,D_ADDR(a6)

sc_plane:

        dbra    d4,sc_mp_enter
        rts

#endif /* __mcoldfire__ */

        .end
