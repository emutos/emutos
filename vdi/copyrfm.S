/*
 * copyrfm.S - 
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "vdiconf.h"


        .globl  _COPY_RFM
        .globl  _CLIP,_XMN_CLIP,_YMN_CLIP,_XMX_CLIP,_YMX_CLIP,_COPYTRAN,_patptr
        .globl  _multifill
        .globl  _v_planes,_v_lin_wr,_v_bas_ad
        .globl  _CONTRL,_INTIN,_PTSIN
        .globl  _DEV_TAB,_MAP_COL
        
        .globl  bit_blt

	.xdef   _v_pl_dspl
//
//
// title:
//      
//      copy_rfm
//
// purpose:
//
//      bitblt operation using 1 of 16 logical operations. device dependent
//      format only.
//
// creation date:
//
//      01-nov-84
//
// latest update:
//
//      21-feb-85
//
//
//

//      MEMORY FORM DEFINITION BLOCK (MFDB) OFFSETS

#define MF_BAS_AD                0      // memory form base address (0: physical device)
#define MF_WD_PIX                4      // form width in pixels ( form width in words /16)
#define MF_HT_PIX                6      // form height in pixels
#define MF_WD_WRD                8      // form width in words
#define MF_FORMAT               10      // form format flag (0:device specific 1:standard)
#define MF_PLANES               12      // number of memory planes

//      CONTRL ARRAY OFFSETS

#define C_MFDB_S                14      // pointer to source MFDB
#define C_MFDB_D                18      // pointer to destination MFDB

//      INTIN ARRAY OFFSETS

#define I_LOG_OP                 0      // logic operation code
#define I_COL_1                  2
#define I_COL_0                  4

//      PTSIN ARRAY OFFSET

#define P_XMIN_S                 0      // x-coordinate of upper left of source rectangle
#define P_YMIN_S                 2      // y-coordinate of upper left of source rectangle
#define P_XMAX_S                 4      // x-coordinate of lower right of source rectangle
#define P_YMAX_S                 6      // y-coordinate of lower right of source rectangle

#define P_XMIN_D                 8      // x-coordinate of upper left of destination rectangle
#define P_YMIN_D                10      // y-coordinate of upper left of destination rectangle
#define P_XMAX_D                12      // x-coordinate of lower right of destination rectangle
#define P_YMAX_D                14      // y-coordinate of lower right of destination rectangle


//       FRAME PARAMETERS

#define B_WD                    -76     // width of block in pixels                         +00
#define B_HT                    -74     // height of block in pixels                        +02 

#define PLANE_CT                -72     // number of consequitive planes to blt             +04

#define FG_COL                  -70     // foreground color (logic op table index:hi bit)    +06
#define BG_COL                  -68     // background color (logic op table index:lo bit)    +08
#define OP_TAB                  -66     // logic ops for all fore and background combos     +10
#define S_XMIN                  -62     // minimum X: source                                +14
#define S_YMIN                  -60     // minimum Y: source                                +16
#define S_FORM                  -58     // source form base address                         +18

#if vme10
#define S_NXWD                  -54     // offset to next word in line  (in bytes)          +22
#define S_NXLN                  -52     // offset to next line in plane (in bytes)          +24
#define S_NXPL                  -50     // offset to next plane from start of current plane  +26
#else
#define S_NXLN                  -54     // offset to next line in plane (in bytes)          +22
#define S_NXPL                  -52     // offset to next plane from start of current plane  +24
#endif

#define D_XMIN                  -48     // minimum X: destination                           +28
#define D_YMIN                  -46     // minimum Y: destination                           +30
#define D_FORM                  -44     // destination form base address                            +32

#if vme10
#define D_NXWD                  -40     // offset to next word in line  (in bytes)          +36
#define D_NXLN                  -38     // offset to next line in plane (in bytes)          +38
#define D_NXPL                  -36     // offset to next plane from start of current plane  +40
#else
#define D_NXLN                  -40     // offset to next line in plane (in bytes)          +36
#define D_NXPL                  -38     // offset to next plane from start of current plane  +38
#endif

#define P_ADDR                  -34     // address of pattern buffer   (0:no pattern)       +42
#define P_NXLN                  -30     // offset to next line in pattern  (in bytes)       +46
#define P_NXPL                  -28     // offset to next plane in pattern (in bytes)       +48
#define P_MASK                  -26     // pattern index mask                               +50

//////                                         //////
//////   these parameters are internally set   //////
//////                                         //////

#define P_INDX                  -24     // initial pattern index                                    +52

#define S_ADDR                  -22     // initial source address                           +54
#define S_XMAX                  -18     // maximum X: source                                +58
#define S_YMAX                  -16     // maximum Y: source                                +60

#define D_ADDR                  -14     // initial destination address                      +62
#define D_XMAX                  -10     // maximum X: destination                           +66
#define D_YMAX                   -8     // maximum Y: destination                           +68

#define INNER_CT                 -6     // blt inner loop initial count                     +70
#define DST_WR                   -4     // destination form wrap (in bytes)                 +72
#define SRC_WR                   -2     // source form wrap (in bytes)                      +74


#define FRAME_LEN                76

#define DEV_COL                  26     // offset in _DEV_TAB to number of colors
#define PAT_FLAG                  4     // flag:1 SOURCE and PATTERN   flag:0 SOURCE only
#define mode05                    5     // write mode #5 (D' = D) => do nothing
#define mode15                   15     // maximum write mode opcode

_COPY_RFM:

        movem.l d2-d7/a2-a5,-(sp)
        link    a6,#-FRAME_LEN

//   test for legal logic ops and set color (if op 16)

        move.l  _INTIN,a2               // a2 -> INTIN array
        move.w  I_LOG_OP(a2),d2         // d2 <- logic op #
        blt.w   bad_parameter           // for a negative write mode operand
//???   clr.l   a3                      // No pattern selected
        move.l  #0,a3                   // No pattern selected

#if !pattern
        bclr    #PAT_FLAG,d2            // check the pattern flag and revert to log op #
        beq     no_pattern

        move.l  _patptr,a3
        moveq   #0,d0                   // next plane pattern offset default.
        tst.w   _multifill              // multi-plane pattern?
        beq     pat_parm                // no, set up the parameter.
        moveq   #32,d0                  // yes, next plane pat offset = 32.

pat_parm:
        move.w  d0,P_NXPL(a6)
        
        move.w  #02,P_NXLN(a6)
        move.w  #0xF,P_MASK(a6)
#endif

no_pattern:
        cmpi.w  #mode15,d2              // is user write mode valid?
        bgt.w   bad_parameter           // branch if high
        cmpi.w  #mode05,d2              // is d'=d if yes this is noop
        beq.w   nop_exit
        move.l  a3,P_ADDR(a6)

//   setup planes for each MFDB

        move.l  _CONTRL,a3              // a3 -> CONTRL array

        move.l  C_MFDB_S(a3),a5         // a5 -> source MFDB
        move.l  C_MFDB_D(a3),a4         // a4 -> destination MFDB


src_form:

        move.l  MF_BAS_AD(a5),d6        // if source is screen, use default

        bgt.b   src_calc                // for a positive source address
        blt.w   bad_parameter           // for a negative source address

        move.l  _v_bas_ad,d6
        move.w  _v_planes,d0            // source form is screen
        move.w  _v_lin_wr,d4
#if !vme10
        move.l  _v_pl_dspl,S_NXPL(a6)   // set # bytes between source planes
#endif
        bra     dst_form

src_calc:
#if vme10
        move.w  MF_PLANES(a5),d0        // d0 <- source plane count
        move.w  MF_WD_WRD(a5),d4
        add.w   d4,d4
        mulu    d0,d4                   // d4 <- form wrap (in bytes)
#else
        move.w  MF_PLANES(a5),d0        // d0 <- source plane count
        ble.w   bad_parameter           // pl. count must be a positive integer
        move.w  MF_WD_WRD(a5),d4        // get # of words in source form line
        ble.w   bad_parameter           // must be a positive integer
        add.w   d4,d4                   // get # of bytes in source form line
        clr.l   S_NXPL(a6)              // set # bytes between source planes
#endif

dst_form:
        move.l  MF_BAS_AD(a4),d7        // if destination is screen, use default

        bgt.b   dst_calc                // for a positive destination address
        blt.w   bad_parameter           // for a negative destination address

        move.l  _v_bas_ad,d7
        move.w  _v_planes,d1
        move.w  _v_lin_wr,d5
#if !vme10
        move.l  _v_pl_dspl,D_NXPL(a6)   // set # bytes between dest. planes
#endif
        bra     compare

dst_calc:
#if vme10
        move.w  MF_PLANES(a4),d1        // d1 <- destination plane count
        move.w  MF_WD_WRD(a4),d5
        add.w   d5,d5
        mulu    d1,d5                   // d5 <- form wrap (in bytes)
#else
        move.w  MF_PLANES(a4),d1        // d1 <- destination plane count
        ble.w   bad_parameter           // pl. count must be a positive integer
        move.w  MF_WD_WRD(a4),d5        // get # of words in dest. form line
        ble.w   bad_parameter           // must be a positive integer
        add.w   d5,d5                   // get # of bytes in dest. form line
        clr.l   D_NXPL(a6)              // set # bytes between dest. planes
#endif

compare:
#if vme10
        moveq.l #0x16,d3                // only 4,2, and 1 are valid (destination)
        btst    d1,d3                   // 
        beq     bad_parameter           // 
#else
        andi.w  #3,d1                   // valid dest. plane count is 1,2, or 3
        beq.w   bad_parameter           // for invalid dest. plane count
#endif

        move.w  d1,PLANE_CT(a6)         // save the total plane count

#if vme10
        add.w   d0,d0                   // for device dependent forms NXWD=2//planes
        move.w  d0,S_NXWD(a6)
        add.w   d1,d1
        move.w  d1,D_NXWD(a6)
#endif

        move.w  d4,S_NXLN(a6)
        move.w  d5,D_NXLN(a6)

        andi.b  #0xfe,d6                        // force least significant bit to zero
        move.l  d6,S_FORM(a6)           // source form addr. is now word aligned
        andi.b  #0xfe,d7                        // force least significant bit to zero
        move.l  d7,D_FORM(a6)           // dest. form addr. is now word aligned

#if vme10
        moveq.l #2,d4                   // d4 <- next plane offset (source)
        moveq.l #2,d5                   // d5 <- next plane offset (destination)
#endif
        tst.w   _COPYTRAN
        beq     standard_blt_ops
//
//  COPY RASTER TRANSPARENT - This function copies a monochrome raster area
//  from source form to a color area. A writing mode and color indices for
//  both 0's and 1's are specified in the INTIN array.
//
tran_blt_ops:
#if vme10
        cmpi.w  #02,d0
#else
        cmpi.w  #01,d0                  // is source area one plane?
#endif
        bne     bad_parameter           // source must be mono plane

        clr.w   d4                      // use only the first plane for the source

        lea     _MAP_COL,a0

        move.w  I_COL_0(a2),d6          // d6 <- background color index
        cmp.w   _DEV_TAB+DEV_COL,d6     // index in range ?
        bmi     col_0_ok

        moveq.l #1,d6                   // if out of range, revert to index 1

col_0_ok:

        add.w   d6,d6                   // d6 <- offset into color map
        move.w  (a0,d6.w),d6            // d6 <- background color

        move.w  I_COL_1(a2),d7          // d7 <- foreground color index
        cmp.w   _DEV_TAB+DEV_COL,d7     // index in range ?
        bmi     col_1_ok

        moveq.l #1,d7                   // if out of range, revert to index 1

col_1_ok:       

        add.w   d7,d7                   // d7 <- offset into color map
        move.w  (a0,d7.w),d7            // d7 <- foreground color

        cmpi.w  #01,d2
        beq.b   replace_mode
        cmpi.w  #02,d2
        beq.b   transparent_mode
        cmpi.w  #03,d2
        beq.b   xor_mode
        cmpi.w  #04,d2
        bne.w   bad_parameter

inverse_transparent_mode:
        move.b  #01,OP_TAB+00(a6)       // fg:0 bg:0  D' <- S and D
        move.b  #13,OP_TAB+01(a6)       // fg:0 bg:1  D' <- [not S] or D
        clr.w   FG_COL(a6)              // were only interested in one color
        move.w  d6,BG_COL(a6)           // save the color of interest
        bra     the_clipper

transparent_mode:
        move.b  #04,OP_TAB+00(a6)       // fg:0 bg:0  D' <- [not S] and D
        move.b  #07,OP_TAB+02(a6)       // fg:1 bg:0  D' <- S or D
        clr.w   BG_COL(a6)              // were only interested in one color
        move.w  d7,FG_COL(a6)           // save the color of interest
        bra     the_clipper

replace_mode:
        move.w  d6,BG_COL(a6)           // save fore and background colors
        move.w  d7,FG_COL(a6)
        move.b  #00,OP_TAB+00(a6)       // fg:0 bg:0  D' <- 0
        move.b  #12,OP_TAB+01(a6)       // fg:0 bg:1  D' <- not S
        move.b  #03,OP_TAB+02(a6)       // fg:1 bg:0  D' <- S
        move.b  #15,OP_TAB+03(a6)       // fg:1 bg:1  D' <- 1
        bra     the_clipper

xor_mode:
        clr.w   BG_COL(a6)
        clr.w   FG_COL(a6)
        move.b  #06,OP_TAB+00(a6)       // fg:0 bg:0  D' <- S xor D
        bra     the_clipper
//
//  COPY RASTER OPAQUE - This function copies a rectangular raster area
//  from source form to destination form using the logic operation 
//  specified by the application.
//
//   do the standard logic operations
//
standard_blt_ops:
        cmp.w   d0,d1
        bne     bad_parameter           // planes must be equal in number
        clr.w   BG_COL(a6)              // bg:0 & fg:0 => only first OP_TAB
        clr.w   FG_COL(a6)              // entry will be referenced

        move.b  d2,OP_TAB+00(a6)        // fg:0 bg:0

the_clipper:
#if vme10
        move.w  d4,S_NXPL(a6)           // save inter plane value
        move.w  d5,D_NXPL(a6)
#endif

//   stoke up the stack frame
        move.l  _PTSIN,a2               // a2 -> PTSIN array

        tst.w   _CLIP
        beq     dont_clip               // if clipping is disabled, skip the clip

        tst.l   MF_BAS_AD(a4)           // if destination isn't screen, don't clip
        beq     clip_it

dont_clip:

        move.w  P_XMIN_S(a2),d0
        move.w  P_YMIN_S(a2),d1
        move.w  P_XMIN_D(a2),d2
        move.w  P_YMIN_D(a2),d3

        move.w  d0,S_XMIN(a6)
        move.w  d1,S_YMIN(a6)
        move.w  d2,D_XMIN(a6)
        move.w  d3,D_YMIN(a6)

        move.w  P_XMAX_S(a2),d4
        move.w  P_YMAX_S(a2),d5

        move.w  d4,d6
        sub.w   d0,d6
        addq.w  #1,d6
        move.w  d6,B_WD(a6)

        move.w  d5,d7
        sub.w   d1,d7
        addq.w  #1,d7
        move.w  d7,B_HT(a6)

        move.w  P_XMAX_D(a2),d6
        move.w  P_YMAX_D(a2),d7

        bra     max_save


clip_it:        

//////   clip Xmin source and destination to window

        move.w  P_XMIN_S(a2),d0
        move.w  P_XMIN_D(a2),d2
        move.w  _XMN_CLIP,d4

        cmp.w   d4,d2                   // Xmin dest < Xmin clip => clip Xmin dest
        bcc     xmin_clipped            // Xmin dest > Xmin clip => branch

        exg     d4,d2                   // d2 <- Xmin dest = Xmin clip
        sub.w   d2,d4                   // d4 <- -(amount clipped in x)
        sub.w   d4,d0                   // d0 <- adjusted Xmin src

xmin_clipped:

        move.w  d0,S_XMIN(a6)           // d0 <- clipped Xmin source
        move.w  d2,D_XMIN(a6)           // d2 <- clipped Xmin destination


//////   clip Xmax destination to window

        move.w  P_XMAX_S(a2),d6
        sub.w   d0,d6                   // d6 <- Dx Source
        add.w   d2,d6                   // d6 <- Xmax destination

        move.w  _XMX_CLIP,d4

        cmp.w   d4,d6                   // Xmax dest > Xmax clip => clip Xmax dest
        bls     xmax_clipped            // Xmax dest < Xmax clip => branch

        exg     d4,d6                   // d6 <- Xmax dest = Xmax clip

xmax_clipped:


//////   clip Ymin source and destination to window

        move.w  P_YMIN_S(a2),d1
        move.w  P_YMIN_D(a2),d3
        move.w  _YMN_CLIP,d4

        cmp.w   d4,d3                   // Ymin dest < Ymin clip => clip Ymin
        bcc     ymin_clipped

        exg     d4,d3                   // d1 <- Ymin dest = Ymin clip
        sub.w   d3,d4                   // d4 <- -(amount clipped in Y)
        sub.w   d4,d1                   // d0 <- adjusted source Ymin 

ymin_clipped:

        move.w  d1,S_YMIN(a6)
        move.w  d3,D_YMIN(a6)


////// clip Ymax destination to window

        move.w  P_YMAX_S(a2),d7
        sub.w   d1,d7                   // d7 <- Dy Source
        add.w   d3,d7                   // d7 <- Ymax destination

        move.w  _YMX_CLIP,d4

        cmp.w   d4,d7                   // is Ymax dest > Ymax clip => clip Ymax
        bls     ymax_clipped

        exg     d4,d7                   // d7 <- Ymax dest = Ymax clip

ymax_clipped:


//   match source and destination rectangles

        move.w  d6,d4
        sub.w   d2,d4                   // d4 <- Dx 

        move.w  d4,B_WD(a6)
        addq.w  #1,B_WD(a6)             // block width
        ble     bad_parameter           // quit if block has been entirely clipped

        add.w   d0,d4                   // d4 <- Xmax Source


        move.w  d7,d5
        sub.w   d3,d5                   // d5 <- Dy 

        move.w  d5,B_HT(a6)
        addq.w  #1,B_HT(a6)             // block height
        ble     bad_parameter

        add.w   d1,d5                   // d5 <- Ymax Source


max_save:

//   save all max parameters

        move.w  d4,S_XMAX(a6)
        move.w  d5,S_YMAX(a6)
        move.w  d6,D_XMAX(a6)
        move.w  d7,D_YMAX(a6)

        bsr     bit_blt

nop_exit:
bad_parameter:
        unlk    a6
        movem.l (sp)+,d2-d7/a2-a5
        rts
        .end
