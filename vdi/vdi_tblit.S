/*
 * vdi_tblit.S - Blitting of Text
 *
 * Copyright (C) 1999 Caldera, Inc. and Authors:
 *               1984 Dave Staugas
 *               2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

/* Conditionals for textblit */
#define test0           0       /* if give program access to internal variables */
#define bytswap         0       /* if font words are byte swapped! */

/////////////////////////////////////////////////////////////////////////
//                                                                      //
//        start date: 1 Oct 84                                          //
//       last update: 13 Apr 85                                         //
//                                                                      //
//  changes made--                                                      //
//                                                                      //
//  26 Feb -- a. writing mode values table corrected                    //
//            b. _MONO if true stops thicken source form expansion      //
//                                                                      //
//   6 Mar -- a. arbitrary scale of text mod to replicate/double code   //
//                                                                      //
//  13 Apr -- a. fixed text rotation when scaled RJG                    //
//            b. act_siz preserves C registers on call                  //
/////////////////////////////////////////////////////////////////////////

#if test0
//  for test routine access..
        .globl  sgl_loop
        .globl  dbl_loop
        .globl  mlt_left
        .globl  mlt_rite
        .globl  blttype
#endif

/////////////////////////////////////////////////////////////////////////
//  Text bit block transfer - TEXTBLT                                   //
//                                                                      //
//  inputs:     SOURCEX,SOURCEY - word                                  //
//                X,Y coordinate of character relative to UL corner of  //
//                font (in pixels)                                      //
//              LITEMASK - word                                         //
//                for lighter letters                                   //
//                                                                      //
//                added Feb 26, 1985                                    //
//                                                                      //
//              MONO - word                                             //
//                if non-zero (true), thicken effect should not enlarge //
//                source( mono spaced font status flag )                //
//              XDDA - word                                             //
//                DDA accumulator passed externally                     //
//              DDAINC - word                                           //
//                DDA increment passed externally                       //
//              SCALDIR - word                                          //
//                0 if scale down, 1 if enlarge                         //
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//  process:    The main process of textblt does the following:         //
//                { load source word (a0.l)                             //
//                  align bits (with destination bit position)          //
//                  apply writing mode                                  //
//                  store into destination (a1.l)  }                    //
//                                                                      //
//              There are several additional considerations             //
//              1. Most BLTs will be for a character less than          //
//                 16 pixels wide so a special all-fringe case          //
//                 can be used                                          //
//              2. Masks must be used on the source to prevent          //
//                 alteration of the destination outside the            //
//                 fringes.                                             //
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//   Style equates..                                   //
/////////////////////////////////////////////////////////

//
//  Style equates converted to 68k bit numbers
//  (bit number 0 is the least significant bit)
//
#define THICKEN                0
#define LIGHT                  1
#define SKEW                   2
#define UNDER                  3
#define OUTLINE                4
#define SHADOW                 5

/////////////////////////////////////////////////////////
//   Global routines defined in this module.           //
/////////////////////////////////////////////////////////

        .globl  _normal_blit

/////////////////////////////////////////////////////////
//  Global variables referenced.                       //
/////////////////////////////////////////////////////////

        .extern _SOURCEX
        .extern _SOURCEY
        .extern _LITEMASK
        .extern _SKEWMASK
        .extern _SCALDIR
        .extern _XDDA
        .extern _DDAINC
        .extern _SCRPT2
        .extern _SCRTCHP


/////////////////////////////////////////////////////////////////////////
//  _text_blt expects variables SCRTCHP & SCRPT2 to be                  //
//           initialized as follows:                                    //
//                                                                      //
//   _SCRTCHP:  is a long word pointer to the base of a buffer with at  //
//              least buf_siz bytes available for scratch use           //
//                                                                      //
//   _SCRPT2:   is a word offset within the buffer partitioning it into //
//              a small and a large segment. Should be set to cel_siz   //
//                                                                      //
//      NOTE: _SCRTCHP & _SCRPT2 must be initialized to an even pointer //
//               even offset respectively                               //
//   see vdi_text.c for calculations of buf_siz & cel_siz               //
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//      Local variables defined on the user stack.                      //
// NOTE: labels follow storage directives for use with LINK instruction  //
/////////////////////////////////////////////////////////////////////////

        //.offset 0

#define buffa   -2              //current text scratch buffer offset
//#define buffb   -4              //for rotate (now unused)
//#define buffc   -6              //for replicate (now unused)
#define sform   -10             //start of source form
#define dform   -14             //start of destination form
#define s_next  -16             //width of source form (formerly s_width)
#define d_next  -18             //width of dest form (_v_lin_wr formerly used)
#define width   -20             //width of area in pixels
#define height  -22             //height of area in pixels
#define tsdad   -24             //source dot address (pixel address, 0-15 word offset)
#define tddad   -26             //destination dot address
#define dest_wrd -28            //# full words between fringes (destination)
#define src_wrd  -30            //# full words between fringes (source) (before thicken)
#define src_wthk -32            //
#define thk_msk  -34            //right fringe mask, before thicken
#define rite_msk -36            //
#define left_msk -38            //fringes of destination to be affected
#define rota_msk -40            //overlap between words in inner loop
#define shif_cnt -42            //shift count for use by left/right shift routines
#define wrd_cnt  -44            //number inner loop words for left/right

/////////////////////////////////////////////////
//  Vectors that may contain a toptable entry. //
/////////////////////////////////////////////////

#define thknjmp  -48            //vector for function after thicken
#define litejmp  -52            //vector for function after light
#define skewjmp  -56            //vector for function after skew
#define thknjpwf -60            //vector for word fringe function after thicken
#define litejpwf -64            //vector for word fringe function after light

//////////////////////////////////////////////////
//  Vectors that may contain twoptable entries. //
//////////////////////////////////////////////////

#define thknjpw  -68            //vector for word function after thicken
#define litejpw  -72            //vector for word function after light

/////////////////////////////////
//  Masks for special effects. //
/////////////////////////////////

#define smear    -74            //amount to increase width
#define ambient  -76            //ambient temp
#define lite_msk -78            //AND with this to get light effect
#define skew_msk -80            //rotate this to check shift
#define thknover -82            //overflow for word thicken

/////////
//  color temp & # of planes
/////////

#define forecol  -84            //foreground color temp
#define nbrplane -86            //# of planes
#define nextwrd  -88            //offset to next word in same

/////////
//  arbitrary scale of text temps.
/////////

#define tmp_delx -90            //temp DELX used by arbitrary scale of text
#define tmp_dely -92            //temp DELY
#define swap_tmps -94           //Non zero if we had to swap temps

/////////////////////////////////////////////////////////////
//  Working copies of often used external global varibles. //
/////////////////////////////////////////////////////////////

#define STYLE   -96
#define WRT_MOD -98
//#define SKEWMAS -100                  (now unused)
#define DELX    -102
#define DESTX   -104
#define DELY    -106
#define DESTY   -108
//#define CHUP    -110                  (now unused)

///////////////////////////////////////////////
//  Working copies of the clipping variables //
///////////////////////////////////////////////

//#define CLIP    -112                  (now unused)
//#define XMN_CLI -114                  (now unused)
//#define YMN_CLI -116                  (now unused)
//#define XMX_CLI -118                  (now unused)
//#define YMX_CLI -120                  (now unused)

//////////////////////////////////
//  Temporary working variables //
//////////////////////////////////

//#define tmp_style -122                (now unused)
#define blt_flag  -124
//#define chup_flag -126                (now unused)
#define ramlen    -126

        .text

/////////////////////////////////////////////////////////////////////////
//   Interface routines from C to assembler                            //
/////////////////////////////////////////////////////////////////////////

//  interface to 'norm_blt'
_normal_blit:
        movem.l d2-d7/a2-a6,-(sp)       // save registers that GCC expects unchanged
        move.l  44+4(sp),a6             // load ptr to end of vars
        move.l  44+8(sp),a0             // a0 & a1 must also be set
        move.l  44+12(sp),a1
        jbsr    norm_blt
        movem.l (sp)+,d2-d7/a2-a6       // restore saved registers
        rts

        SECTION_RODATA

/*
 * Word Mask Table
 *
 * The table has been compacted by taking the one's complement of the left
 * fringe table and combining it with the right fringe table.
 */
                .even   // Mandatory in ELF section .rodata
lf_tab:         .dc.w   0xFFFF          // origin for not left fringe lookup.
rf_tab:         .dc.w   0x7FFF          // origin for right fringe lookup.
                .dc.w   0x3FFF
                .dc.w   0x1FFF
                .dc.w   0x0FFF
                .dc.w   0x07FF
                .dc.w   0x03FF
                .dc.w   0x01FF
                .dc.w   0x00FF
                .dc.w   0x007F
                .dc.w   0x003F
                .dc.w   0x001F
                .dc.w   0x000F
                .dc.w   0x0007
                .dc.w   0x0003
                .dc.w   0x0001
                .dc.w   0x0000

/*
 * Writing mode mapping tables
 *
 * These are accessed via an offset from 'toptable', allowing the use
 * of indexing mode for speed.
 */
#define op0             4*0
#define op1             4*1
#define op2             4*2
#define op3             4*3
#define op4             4*4
#define op5             4*5
#define op6             4*6
#define op7             4*7
#define op8             4*8
#define op9             4*9
#define opa             4*10
#define opb             4*11
#define opc             4*12
#define opd             4*13
#define ope             4*14
#define opf             4*15

// fore/back    00  01  10  11
wrmappin:                           // VDI modes
        .dc.b   op0,op0,op3,op3         // replace mode
        .dc.b   op4,op4,op7,op7         // transparent mode
        .dc.b   op6,op6,op6,op6         // XOR mode
        .dc.b   op1,op1,opd,opd         // inverse transparent mode
                                    //BitBlt modes
        .dc.b   op0,opf,op0,opf         // mode 0: all zeros
        .dc.b   op0,ope,op1,opf         // mode 1: source AND destination
        .dc.b   op0,opd,op2,opf         // mode 2: source AND (NOT destination)
        .dc.b   op0,opc,op3,opf         // mode 3: source
        .dc.b   op0,opb,op4,opf         // mode 4: (NOT source) AND destination
        .dc.b   op0,opa,op5,opf         // mode 5: destination
        .dc.b   op0,op9,op6,opf         // mode 6: source XOR destination
        .dc.b   op0,op8,op7,opf         // mode 7: source OR destination
        .dc.b   op0,op7,op8,opf         // mode 8: (NOT source) AND (NOT destination)
        .dc.b   op0,op6,op9,opf         // mode 9: (NOT source) XOR destination
        .dc.b   op0,op5,opa,opf         // mode A: NOT destination
        .dc.b   op0,op4,opb,opf         // mode B: source or (NOT destination)
        .dc.b   op0,op3,opc,opf         // mode C: NOT source
        .dc.b   op0,op2,opd,opf         // mode D: (NOT source) OR destination
        .dc.b   op0,op1,ope,opf         // mode E: (NOT source) OR (NOT destination)
        .dc.b   op0,op0,opf,opf         // mode F: all ones

toptable:
        .dc.l   top0,top1,top2,top3,top4,top5,top6,top7
        .dc.l   top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
        .dc.l   twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
        .dc.l   twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

        .text

/////////////////////////////////////////////////////////////////////////
//   normal blt routine                                                 //
//              uses:   a0.l - starting source address                  //
//                      a1.l - starting destination address             //
//       tsdad,tddad - address within word                              //
//             STYLE - special effects mask                             //
//      width,height - width & length of area to copy                   //
//            s_next - add this to get to next line in source           //
//            d_next - add this to get to next line in destination      //
/////////////////////////////////////////////////////////////////////////

norm_blt:
        move.w  tddad(a6),d1            // get destination offset
        sub.w   tsdad(a6),d1            // subtract source offset -> d1
        move.w  d1,d0                   // copy to d0
        jpl     do_rot                  // br if tsdad =< tddad & rotate right
//
//  rotate left
//
        neg.w   d1                      // form 2's cmpliment for positive shift/count
        ori.w   #0x8000,d1              // fake a negative (stripped by ROR or ROL)
        addi.w  #16,d0                  // make word_mask_table index positive
do_rot:
        move.w  d1,shif_cnt(a6)         // save shift count (bit15=1 if ROL, else ROR)

        add.w   d0,d0                   // x2 for index
        lea     lf_tab,a2               // get base addr of mask table
        move.w  (a2,d0.w),d0            // set the overlap for middle words
        not.w   d0
        move.w  d0,rota_msk(a6)         // save as rotate mask
//
//    Set up fringe masks..
//
get_mask:
        move.w  tddad(a6),d0            // get destination dot address
        add.w   d0,d0                   // x2 for index in d0
        move.w  (a2,d0.w),left_msk(a6)  // get mask for destination dot address

        lsr.w   #1,d0                   // d0 back to tddad
        add.w   width(a6),d0            // add to form right side

//  thicken bug fix..
        move.w  d0,d2                   // copy possibly thickened width to temp d2
        sub.w   smear(a6),d2            // get original before thickened
        andi.w  #0xF,d2
        add.w   d2,d2
        move.w  (a2,d2.w),d2
        not.w   d2
        move.w  d2,thk_msk(a6)
        clr.w   d4
        move.w  #0x8000,skew_msk(a6)
        moveq.l #-1,d3                // assume sgl_loop
//  end bug fix

        cmpi.w  #0x10,d0                // more than a word?
        jhi     doub_des                // br if bx => 0x10, needs more than 1 word
//
//  Fits in one word
//
        add.w   d0,d0                   // x2 for index
        move.w  (a2,d0.w),d1
        not.w   d1
        and.w   d1,left_msk(a6)         // put the two masks together

        moveq.l #-4,d7                  // set blttype flag
        jra     msk_done                // exit mask stuff
//
//  Two fringe masks needed..
//
doub_des:
        move.w  d0,d1                   // get tddad +_DELX to d1
        lsr.w   #4,d1                   // divide by 16
        subq.w  #1,d1
        move.w  d1,d3                   // number of words to write for middle
        jne     mlt_dest                // br if # of words is non-zero
//
//  # of middle words is zero
//
        move.w  tsdad(a6),d1
        add.w   width(a6),d1
        cmpi.w  #0x20,d1
        jcc     mlt_dest                // br if source fits in two words too
        moveq.l #0,d7                   // set blttype flag for double
        jra     do_ritem

mlt_dest:
        tst.w   shif_cnt(a6)            // check sign of shift count
        jlt     mltleft
        moveq.l #4,d7
        jra     do_ritem
mltleft:
        moveq.l #8,d7
do_ritem:
        andi.w  #0x000F,d0
        jne     not_null
        subq.w  #1,d3                   // last word is full so its a fringe
        moveq.l #0x10,d0
not_null:
        add.w   d0,d0
        move.w  (a2,d0.w),d4
        not.w   d4

msk_done:
        move.w  d7,blt_flag(a6)
        move.w  d3,dest_wrd(a6)
        move.w  d4,rite_msk(a6)
        addq.w  #2,d3
        cmp.w   d2,d4
        jcs     msk0
        addq.w  #1,d3
msk0:
        move.w  d3,src_wthk(a6)
        move.w  d3,src_wrd(a6)

plane_loop:
        move.w  WRT_MOD(a6),d0          // d0.b = 000xxxxx (0x00-0x13 are valid)
        lsr.w   forecol(a6)             // this plane's foreground bit
        addx.w  d0,d0                   // d0.b = 00xxxxxf
        lsr.w   ambient(a6)             // this plane's background bit
        addx.w  d0,d0                   // d0.b = 0xxxxxfb
//
//  d0.w is index into 'wrmappin'
//
        lea     toptable,a4             // allows us to use indexing mode for speed
        move.b  wrmappin-toptable(a4,d0.w),d0
        movea.l toptable-toptable(a4,d0.w),a2
        movea.l a2,a3                   // call this address to do tlogicop
        movea.l a2,a5                   // logicop or special effects
                                        // word fringe special effects
        movea.l twoptble-toptable(a4,d0.w),a4   // logicop or special effects for words
//
//  Do special effects..
//
        move.w  STYLE(a6),d0            // special effects mask
        jeq     wrt_char

        btst.l  #LIGHT,d0
        jeq     no_lite
//
//  Light special effect..
//
        move.w  _LITEMASK,lite_msk(a6)   // reload the mask for this char
        move.l  a5,litejmp(a6)          // endpoint of light routine
        lea.l   liteop,a5               // insert this in the loop

        move.l  a2,litejpwf(a6)         // endpoint of light routine
        lea.l   liteopwf,a2             // insert this in the loop

        move.l  a4,litejpw(a6)          // endpoint of light routine
        lea.l   liteopw,a4              // insert this in the loop

no_lite:
        btst.l  #THICKEN,d0
        jeq     no_thick
//
//  Thicken special effect..
//
        clr.w   thknover(a6)

        move.l  a5,thknjmp(a6)          // endpoint of thicken routine
        lea.l   thknop,a5               // insert routine into the loop

        move.l  a2,thknjpwf(a6)         // endpoint of light routine
        lea.l   thknopwf,a2             // insert routine into loop

        move.l  a4,thknjpw(a6)          // endpoint of light routine
        lea.l   thknopw,a4              // insert routine into the loop

no_thick:
        btst.l  #SKEW,d0
        jeq     wrt_char
//
//  Skew special effect..
//
        move.w  _SKEWMASK,skew_msk(a6)  // reload mask for this char
        move.l  a5,skewjmp(a6)
        lea.l   skewop,a5

        tst.w   blt_flag(a6)
        jgt     wrt_char
        jeq     not_sngl
        clr.w   dest_wrd(a6)
        moveq.l #0,d7
        jra     chng_blt
not_sngl:
        cmpi.w  #0x10,width(a6)
        jls     wrt_char                // br if source is at most two words

        tst.w   shif_cnt(a6)
        jlt     chng_left               // br if mlt_rite assumption correct
        moveq.l #4,d7
        jra     chng_blt
chng_left:
        moveq.l #8,d7
chng_blt:
        move.w  d7,blt_flag(a6)

wrt_char:
        move.w  blt_flag(a6),d7
        jle     set_chr_hgt
        movea.l a2,a5
        move.w  d_next(a6),d5
set_chr_hgt:
        move.w  height(a6),d3           // set loop count
        subq.w  #1,d3

        movea.l blttype+4(pc,d7.w),a2
        jmp     (a2)                    // do the fastest one
next_plane:
        subq.w  #1,nbrplane(a6)         // decrement plane count
        jle     no_more_planes          // loop for next plane
        movea.l sform(a6),a0
        movea.l dform(a6),a1
        addq.l  #2,a1                   // advance destination plane
        move.l  a1,dform(a6)            // save dest. starting address
        btst.b  #SKEW,STYLE+1(a6)       // only skew screws up other planes
        jeq     plane_loop              // br to short init if not skew
        jra     norm_blt                // else, do big init
no_more_planes:
        rts
blttype:
        .dc.l   sgl_loop, dbl_loop, mlt_rite, mlt_left

///////////////////////
//   Single loop - destination is a single word //
///////////////////////
sgl_loop:
        move.w  left_msk(a6),d2         // get the first mask
        move.w  s_next(a6),d6
        move.w  d_next(a6),d7
        lea.l   sgl_rtn,a3
        jra     sgl_1st_scan

sgl_lp:
        adda.w  d6,a0                   // get to next line above in font
        adda.w  d7,a1                   // and on screen
sgl_1st_scan:
        move.w  (a1),d4                 // get dest

#if bytswap
        move.w  (a0),d0
        ror.w   #8,d0
        swap    d0
        move.w  2(a0),d0
        ror.w   #8,d0
#else
        move.l  (a0),d0                 // get 2 source words (may only use 1)
#endif

        move.w  shif_cnt(a6),d1
        jmi     left_rol
        lsr.l   d1,d0
        jra     end_ro
left_rol:
        lsl.l   d1,d0
end_ro:
        swap    d0
        move.w  d0,d1
        swap    d0

        jmp     (a5)                    //do special effect or just logicop

sgl_rtn:
        move.w  d1,(a1)                 //store the result
        dbra    d3,sgl_lp               //do next scan line of character
        jra     next_plane

///////////////////////
//  Double loop - destination is two words      //
///////////////////////
dbl_loop:
        movea.l a3,a4
        move.w  s_next(a6),d6
        move.w  d_next(a6),d7
        lea.l   dbl_2wrd,a2
        move.l  a2,d5
        lea.l   dbl_1wrd,a3
        jra     dbl_1st_scan

dbl_lp:
        adda.w  d6,a0           //get to next line above in font
        adda.w  d7,a1           //   and on screen
        exg     a3,d5           //set write mode return address
dbl_1st_scan:
        move.w  (a1),d4                 //get destination word

#if bytswap
        move.w  (a0),d0
        ror.w   #8,d0
        swap    d0
        move.w  2(a0),d0
        ror.w   #8,d0
#else
        move.l  (a0),d0                 //do a line (two fringes)
#endif

        move.w  shif_cnt(a6),d1
        jmi     dbleft
        lsr.l   d1,d0                   //align source & destination
        jra     dblendr
dbleft:
        lsl.l   d1,d0
dblendr:
        swap    d0
        move.w  d0,d1
        swap    d0
        move.w  left_msk(a6),d2         //get the first one back

        jmp     (a5)                    //call special

dbl_1wrd:
        move.w  d1,(a1)                 //write 1st scan word
        move.w  d0,d1                   //get the other scan word
                                        // (it got shifted in)
        movea.w nextwrd(a6),a2          //get offset to next word
        move.w  (a1,a2.w),d4            //get next destination word

        move.w  rite_msk(a6),d2         //use right mask

        exg     a3,d5                   //set write mode return address
        jmp     (a4)                    //call logic op

dbl_2wrd:
        move.w  nextwrd(a6),a2
        move.w  d1,(a1,a2.w)            //save the result

        dbra    d3,dbl_lp               //do next scan line of character
        jra     next_plane

/////////////////////////
//    Multi - Left     //
/////////////////////////

left_loop:
        movea.l d6,a0
        movea.l d7,a1
        adda.w  s_next(a6),a0           //get to next line above in font
        adda.w  d5,a1                   //   and on screen
        clr.w   thknover(a6)
        rol.w   lite_msk(a6)

        btst.b  #SKEW,STYLE+1(a6)
        jne     skewopw
mlt_left:
        move.l  a0,d6
        move.l  a1,d7
        move.w  dest_wrd(a6),wrd_cnt(a6) //# of full words between fringes
#if bytswap
        move.w  (a0)+,d0
        ror.w   #8,d0
        swap    d0
        move.w  (a0)+,d0
        ror.w   #8,d0
#else
        move.l  (a0)+,d0                //get two words of source
#endif

        move.w  (a1),d4                 //get destination word

        move.w  shif_cnt(a6),d1
        lsl.l   d1,d0

        swap    d0
        move.w  d0,d1
        swap    d0                      //source aligned to destination

        move.w  left_msk(a6),d2         //get the mask for left fringe

        lea.l   lft_lfrng,a3            //set write mode return address
        jmp     (a5)                    //call specialwf

lft_lfrng:
        move.w  d1,(a1)                 //store the result
        adda.w  nextwrd(a6),a1          //advance destination ptr to next word

        move.w  rota_msk(a6),d2         //get mask for inner full words
        lea.l   lft_wrds,a3             //set write mode return address
//
//  inner loop for non-fringe words..
//
word_lef:
        move.w  d0,d4                   //save what's left of this word
        and.w   d2,d4                   //clear out garbage at end of word
        move.w  (a0)+,d0                //get next source word

#if bytswap
        ror.w   #8,d0
#endif

        swap    d0
        move.w  d1,d0                   //pack d0:  d0.h="ax" d0.l="dx"
        swap    d0

        move.w  shif_cnt(a6),d1
        lsl.l   d1,d0

        swap    d0
        move.w  d0,d1
        swap    d0                      //source aligned to destination

        not.w   d2
        and.w   d2,d1                   //strip off garbage
        not.w   d2
        eor.w   d4,d1                   //put left-overs in front of word

        move.w  (a1),d4                 //get another destination word

        subq.w  #1,wrd_cnt(a6)          //decrement inner loop count
        jlt     lef_don                 //br if we have to mask the last word

        jmp     (a4)                    //call specialw

lft_wrds:
        move.w  d1,(a1)                 //store the result
        adda.w  nextwrd(a6),a1          //advance destination to next word

        jra     word_lef                //go for more

lef_don:
        move.w  rite_msk(a6),d2         //load the mask we need

        lea.l   lft_rfrng,a3            //set write mode return address
        jmp     (a5)                    //call specialwf
                                        //feature - this clears thickenover
lft_rfrng:
        move.w  d1,(a1)                 //store the result

        dbra    d3,left_loop            //decrement # of lines to move
        jra     next_plane

/////////////////////////
//   Multi - right     //
/////////////////////////

rite_loop:
        move.l  d6,a0
        move.l  d7,a1
        adda.w  s_next(a6),a0           //get to next line above in font
        adda.w  d5,a1                   //   and on screen
        clr.w   thknover(a6)
        rol.w   lite_msk(a6)

        btst.b  #SKEW,STYLE+1(a6)
        jne     skewopw
mlt_rite:
        move.l  a0,d6
        move.l  a1,d7
        move.w  dest_wrd(a6),wrd_cnt(a6) //# of full words between fringes
        move.w  (a0)+,d0                //get one word of source
#if bytswap
        ror.w   #8,d0
#endif
        swap    d0                      //put in hi word

        move.w  (a1),d4                 //get destination word

        move.w  shif_cnt(a6),d1
        lsr.l   d1,d0                   //align source & destination
        swap    d0
        move.w  d0,d1
        swap    d0                      //source aligned to destination

        move.w  left_msk(a6),d2         //get the mask for left fringe

        lea.l   rgt_lfrng,a3            //set write mode return address
        jmp     (a5)                    //call specialwf

rgt_lfrng:
        move.w  d1,(a1)                 //store the result
        adda.w  nextwrd(a6),a1          //advance destination ptr to next word

        move.w  rota_msk(a6),d2         //get mask for inner full words
        lea.l   rgt_wrds,a3             //set write mode return address
//
//  inner loop for non-fringe words..
//
word_rit:
        move.w  d0,d4                   //save what's left of this word
        and.w   d2,d4                   //clear out garbage at end of word
        swap    d0
        move.w  (a0)+,d0                //get next source word
#if bytswap
        ror.w   #8,d0
#endif
        swap    d0

        move.w  shif_cnt(a6),d1
        lsr.l   d1,d0                   //align source & destination
        swap    d0
        move.w  d0,d1
        swap    d0                      //source aligned to destination

        not.w   d2
        and.w   d2,d1                   //strip off garbage
        not.w   d2
        eor.w   d4,d1                   //put left-overs in front of word

        move.w  (a1),d4                 //get another destination word

        subq.w  #1,wrd_cnt(a6)          //decrement inner loop count
        jlt     rite_don                //br if we have to mask the last word

        jmp     (a4)                    //call specialw

rgt_wrds:
        move.w  d1,(a1)                 //store the result
        adda.w  nextwrd(a6),a1          //advance destination to next word

        jra     word_rit                //go for more

rite_don:
        move.w  rite_msk(a6),d2         //load the mask we need

        lea.l   rgt_rfrng,a3
        jmp     (a5)                    //call specialwf
                                        //feature - this clears thickenover
rgt_rfrng:
        move.w  d1,(a1)                 //store the result

        dbra    d3,rite_loop            //decrement # of lines to move
        jra     next_plane

//////////////////////////////////////////////////
//  Writing Mode Operations using fringe mask   //
//                                              //
//  On entry to these routines:                 //
//      d2.w    fringe mask                     //
//      d1.w    source                          //
//      d4.w    destination                     //
//  On exit:                                    //
//      d2.w    unchanged                       //
//      d1.w    result (new destination)        //
//      d4.w    (may be changed)                //
//////////////////////////////////////////////////

top0:
        not.w   d2                      //mode 0 - D' = 0
        and.w   d2,d4
        not.w   d2
        move.w  d4,d1
        jmp     (a3)

top1:
        not.w   d2                      //mode 1 - D' = S and D
        or.w    d2,d1
        not.w   d2
        and.w   d4,d1
        jmp     (a3)

top2:
        not.w   d1                      //mode 2 - D' = S and [not D]
        and.w   d2,d1
        or.w    d4,d1
        eor.w   d2,d1
        jmp     (a3)

top3:
        eor.w   d4,d1                   //mode 3 - D' = S (replace mode)
        and.w   d2,d1
        eor.w   d4,d1
        jmp     (a3)

top4:
        and.w   d2,d1                   //mode 4 - D' = [not S] and D
        not.w   d1
        and.w   d4,d1
        jmp     (a3)

top5:
        move.w  d4,d1                   //mode 5 - D' = D
        jmp     (a3)

top6:
        and.w   d2,d1                   //mode 6 - D' = S xor D (xor mode)
        eor.w   d4,d1
        jmp     (a3)

top7:
        and.w   d2,d1                   //mode 7 - D' = S or D (or mode)
        or.w    d4,d1
        jmp     (a3)

top8:
        and.w   d2,d1                   //mode 8 - D' = [not S] and [not D]
        or.w    d4,d1                   // (implemented as: D' = not [S or D])
        eor.w   d2,d1
        jmp     (a3)

top9:
        and.w   d2,d1                   //mode 9 - D' = [not S] xor D
        eor.w   d4,d1                   // (implemented as: D' = not [S xor D])
        eor.w   d2,d1
        jmp     (a3)

topa:
        move.w  d4,d1                   //mode 10 - D' = not D
        eor.w   d2,d1
        jmp     (a3)

topb:
        eor.w   d2,d4                   //mode 11 - D' = S or [not D]
        and.w   d2,d1
        or.w    d4,d1
        jmp     (a3)

topc:
        eor.w   d4,d1                   //mode 12 - D' = not S
        and.w   d2,d1
        eor.w   d4,d1
        eor.w   d2,d1
        jmp     (a3)

topd:
        not.w   d1                      //mode 13 - D' = [not S] or D
        and.w   d2,d1
        or.w    d4,d1
        jmp     (a3)

tope:
        not.w   d1                      //mode 14 - D' = [not S] or [not D]
        and.w   d2,d1                   // (implemented as: D' = not [S and D])
        eor.w   d2,d4
        or.w    d4,d1
        jmp (a3)

topf:
        or.w    d2,d4                   //mode 15 - D' = 1
        move.w  d4,d1
        jmp     (a3)

//////////////////////////////////////////////////
//      Word Writing Mode Operations            //
//                                              //
//  On entry to these routines:                 //
//      d1.w    source                          //
//      d4.w    destination                     //
//  On exit:                                    //
//      d1.w    result (new destination)        //
//      d4.w    (may be changed)                //
//////////////////////////////////////////////////

twop0:
        moveq.l #0,d1                   //mode 0 - D' = 0
        jmp     (a3)

twop1:
        and.w   d4,d1                   //mode 1 - D' = S and D
        jmp     (a3)

twop2:
        not.w   d4                      //mode 2 - D' = S and [not D]
        and.w   d4,d1
        jmp     (a3)

twop3:
        jmp     (a3)                    //mode 3 - D' = S (replace mode)

twop4:
        not.w   d1                      //mode 4 - D' = [not S] and D
        and.w   d4,d1
        jmp     (a3)

twop5:
        move.w  d4,d1                   //mode 5 - D' = D
        jmp     (a3)

twop6:
        eor.w   d4,d1                   //mode 6 - D' = S xor D
        jmp     (a3)

twop7:
        or.w    d4,d1                   //mode 7 - D' = S or D
        jmp     (a3)

twop8:
        or.w    d4,d1                   //mode 8 - D' = [not S] and [not D]
        not.w   d1                      // (implemented as: D' = not [S or D]
        jmp     (a3)

twop9:
        eor.w   d4,d1                   //mode 9 - D' = [not S] xor D
        not.w   d1                      // (implemented as: D' = not [S xor D]
        jmp     (a3)

twopa:
        move.w  d4,d1                   //mode 10 - D' = not D
        not.w   d1
        jmp     (a3)

twopb:
        not.w   d4                      //mode 11 - D' = S or [not D]
        or.w    d4,d1
        jmp     (a3)

twopc:
        not.w   d1                      //mode 12 - D' = not S
        jmp     (a3)

twopd:
        not.w   d1                      //mode 13 - D' = [not S] or D
        or.w    d4,d1
        jmp     (a3)

twope:
        and.w   d4,d1                   //mode 14 - D' = [not S] or [not D]
        not.w   d1                      // (implemented as: D' = not [S and D])
        jmp     (a3)

twopf:
        moveq.l #-1,d1                  //mode 15 - D' = 1
        jmp     (a3)

/////////////////////////
//    special effect THICKEN
//      on entry        68000   Description             8086
//
//                      d1.w    source word             ax
//                      d0.w    next source word        dx
//                      d2.w    current mask            bp
//
//      on exit
//                      d1.w    thickened source        ax
//                      d0.w    thickened next source   dx
//
//      destroyed:      d5.w    bx
//                      d6.w    cx
//                      d7.w    bp'
//
thknop:
        movem.l d5-d7,-(sp)
        and.w   d2,d1
        move.w  thk_msk(a6),d6
        btst.b  #0,skew_msk+1(a6)       //was a skew performed?
        jeq     thk00                   //br if not
        ori.b   #0x10,ccr
        roxr.w  #1,d6
        jcc     thk01
        move.w  #0x8000,d6
thk01:
        move.w  d6,thk_msk(a6)
thk00:
        tst.w   dest_wrd(a6)
        jmi     thk0
        cmp.w   rite_msk(a6),d6
        jcc     thk0
        and.w   d6,d0
        jra     thk1
thk0:
        clr.w   d0
        and.w   d6,d1
thk1:
        move.w  smear(a6),d6
        move.w  d2,d5
        swap    d5
        move.w  rite_msk(a6),d5
        lsl.l   d6,d5

        swap    d5
        move.w  d5,d7
        swap    d5

        and.w   d7,d1
        and.w   d5,d0
        swap    d1
        move.w  d0,d1
        subq.w  #1,d6
thkoplp:
        move.l  d1,d0
        lsr.l   #1,d0
        or.l    d0,d1
        dbra    d6,thkoplp

        move.w  d1,d0
        swap    d1
        movem.l (sp)+,d5-d7
        movea.l thknjmp(a6),a2
        jmp     (a2)

/////////////////////////
//    special effect THICKEN
//      on entry        68000   Description             8086
//
//                      d1.w    hi source word          ax
//
//      on exit
//                      d1.w    thickened hi source     ax
//                      thknover        bits that spill out
//
//      destroyed:      d5.w    bx
//                      d6.w    cx
//
thknopw:
        movem.l d5-d6,-(sp)
        subq.w  #1,src_wrd(a6)
        jne     thk2
        and.w   thk_msk(a6),d1
thk2:
        move.w  smear(a6),d6
        swap    d1
        clr.w   d1
        move.l  d1,d5
        subq.w  #1,d6
thkopwlp:
        lsr.l   #1,d5
        or.l    d5,d1
        dbra    d6,thkopwlp

        move.w  d1,d5
        swap    d1
        or.w    thknover(a6),d1
        move.w  d5,thknover(a6)
        movem.l (sp)+,d5-d6
        movea.l thknjpw(a6),a2
        jmp     (a2)

/////////////////////////
//    special effect THICKEN
//      on entry        68000   Description             8086
//
//                      d1.w    hi source word          ax
//
//      on exit
//                      d1.w    thickened hi source     ax
//                      thknover        bits that spill out
//
//      destroyed:      d5.w    bx
//                      d6.w    cx
//                      d7.hi w
//
thknopwf:
        movem.l d5-d7,-(sp)
        move.w  thk_msk(a6),d6
        subq.w  #1,src_wrd(a6)
        jmi     thk3
        jeq     thk4
//  starting left fringe, do left mask only
        subq.w  #1,src_wrd(a6)
        jne     thk11
        and.w   d6,d1
thk11:
        and.w   left_msk(a6),d1
        jra     thk7
//  right fringe, source data invalid, erase..
thk3:
        clr.w   d1
        jra     thk5
//  right fringe, source data AND'd with source mask..
thk4:
        and.w   d6,d1
//  compute mask & count for next line..
thk5:
        tst.w   skew_msk(a6)
        jmi     thk6
        move.w  dest_wrd(a6),d5
        addq.w  #2,d5
        move.w  rite_msk(a6),d7
        ori.b   #0x10,ccr
        roxr.w  #1,d7
        jcc     thk33
        move.w  #0x8000,d7
        addq.w  #1,d5
thk33:
        cmpi.w  #1,left_msk(a6)
        jne     thk34
        subq.w  #1,d5
thk34:
        ori.b   #0x10,ccr
        roxr.w  #1,d6
        jcc     thk31
        move.w  #0x8000,d6
thk31:
        cmp.w   d6,d7
        jcs     thk35
        addq.w  #1,d5
thk35:
        move.w  d6,thk_msk(a6)
        move.w  d5,src_wthk(a6)
thk6:
        move.w  src_wthk(a6),src_wrd(a6)
thk7:
        move.w  smear(a6),d6
        swap    d1
        clr.w   d1
        move.l  d1,d5
        subq.w  #1,d6
thkopwl:
        lsr.l   #1,d5
        or.l    d5,d1
        dbra    d6,thkopwl

        move.w  d1,d5
        swap    d1
        or.w    thknover(a6),d1
        move.w  d5,thknover(a6)
        and.w   d2,d1
        movem.l (sp)+,d5-d7
        movea.l thknjpwf(a6),a2
        jmp     (a2)

///////////////////////////////
//   special effect LIGHT
//      on entry,       68000   Description             8086
//
//                      d1.w    hi source word          ax
//                      d0.w    lo source word          dx
//
//      on exit,        d1.w    hi lite source word     ax
//                      d0.w    lo lite source word     dx
//
//
liteop:
        and.w   lite_msk(a6),d1
        and.w   lite_msk(a6),d0
        rol.w   lite_msk(a6)
        movea.l litejmp(a6),a2
        jmp     (a2)

liteopw:
        and.w   lite_msk(a6),d1
        movea.l litejpw(a6),a2
        jmp     (a2)

liteopwf:
        and.w   lite_msk(a6),d1
        movea.l litejpwf(a6),a2
        jmp     (a2)

///////////////////////////////
//   special effect SKEW
//
//      on entry,       68000   Description             8086
//
//                      d1.w    hi source word          ax
//                      d0.w    lo source word          dx
//                      d2.w    mask                    bp
//
//
//      on exit,        d1.w    hi skewized source word ax
//                      d0.w    lo skewized source word dx
//                      left_msk, rite_msk = rotated masks
//
//
skewop:
        rol.w   skew_msk(a6)
        jcc     no_shift

        swap    d1
        move.w  d0,d1                   //pack source words into long one (d1)
        lsr.l   #1,d1

        swap    d2
        move.w  rite_msk(a6),d2         //pack mask words into long one (d2)
        lsr.l   #1,d2
        move.w  d2,rite_msk(a6)
        swap    d2                      //unpack mask words
        move.w  d2,left_msk(a6)
        jeq     nxt_word
        move.w  shif_cnt(a6),d0
        jmi     dec_rol
ror_add:
        addq.w  #1,d0
new_shif:
        move.w  d0,shif_cnt(a6)
        move.w  d1,d0                   //unpack skewized source words
        swap    d1
no_shift:
        movea.l skewjmp(a6),a2
        jmp     (a2)

dec_rol:
        tst.b   d0
        jeq     begn_ror
        subq.w  #1,d0
        jra     new_shif
begn_ror:
        clr.w   d0
        jra     ror_add
//
//  we crossed a word boundary..
//
nxt_word:
        move.w  d2,rite_msk(a6)         //0h to right mask
        swap    d2
        move.w  d2,left_msk(a6)         //move right mask to left mask
        adda.w  nextwrd(a6),a1          //bump next destination address
        move.w  (a1),d4                 //get the word we're really doing

        moveq.l #15,d0
        sub.w   shif_cnt(a6),d0
        ori.w   #0x8000,d0
        move.w  d0,shif_cnt(a6)
        jra     no_shift

/////////////////////////////////////////////////////////////////////////
//    special effect SKEW for words                                     //
//    recomputes rotation and jumps to proper routine to finish char    //
/////////////////////////////////////////////////////////////////////////

skewopw:
        rol.w   skew_msk(a6)
        jcc     do_shift

        tst.w   shif_cnt(a6)
        jmi     mlt_left
        jra     mlt_rite
do_shift:
        ori     #0x10,ccr                       //set x bit
        roxr.w  rota_msk(a6)                    //one more bit into next word

        move.w  rite_msk(a6),d0
        cmpi.w  #0xFFFF,d0                      //if mask is full on
        jeq     inc_rite

        ori     #0x10,ccr                       //set x bit
        roxr.w  #1,d0                           //rotate in a 1
        move.w  d0,rite_msk(a6)

do_left:
        move.w  shif_cnt(a6),d0
        tst.b   d0
        jne     no_rota
        move.w  #0x8000,rota_msk(a6)            //these are the bits that are good
no_rota:
        move.w  left_msk(a6),d1
        lsr.w   #1,d1                           //rotate in a 0
        jeq     wnxt_wrd                        //br if mask inoperative, inc addr

        move.w  d1,left_msk(a6)

        tst.w   d0
        jmi     wdec_rol
        addq.w  #1,shif_cnt(a6)
        jra     mlt_rite
wdec_rol:
        tst.b   d0
        jeq     set_msk
        subq.w  #1,shif_cnt(a6)                 //do 1 less rol
        jra     mlt_left
set_msk:
        move.w  #1,shif_cnt(a6)
        jra     mlt_rite

inc_rite:
        addq.w  #1,dest_wrd(a6)                 //spilled out of a word to get here
        move.w  #0x8000,rite_msk(a6)
        jra     do_left                         //go back & finish up

wnxt_wrd:
        move.w  #0xFFFF,left_msk(a6)            //ran out of word

        subq.w  #1,dest_wrd(a6)                 //so more is in the fringe
        adda.w  nextwrd(a6),a1                  //bump next destination address

        moveq.l #15,d2
        sub.w   d0,d2                           //d0 has old shif_cnt(a6)
        ori.w   #0x8000,d2
        move.w  d2,shif_cnt(a6)

        jra     mlt_left
