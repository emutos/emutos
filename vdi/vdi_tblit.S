/*
 * textblt.S - Blitting of Text
 *
 * Copyright (c) 1999 Caldera, Inc. and Authors:
 *               1984 Dave Staugas
 *               2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "asmdefs.h"
#include "vdi_asm.h"




/* Conditionals for textblit */
#define test0           0       /* if give program access to internal variables */
#define test1           0       /* if using very large fonts (else, 8x16) */
#define bytswap         0       /* if font words are byte swapped! */



/////////////////////////////////////////////////////////////////////////
//                                                                      //
//        start date: 1 Oct 84                                          //
//       last update: 13 Apr 85                                         //
//                                                                      //
//  changes made--                                                      //
//                                                                      //
//  26 Feb -- a. writing mode values table corrected                    //
//            b. _MONO_STATUS if true stops thicken source form expansion //
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
        .globl  ramlen
        .globl  blttype
        .globl  rite_msk
        .globl  left_msk
        .globl  tsdad
        .globl  tddad
#endif

/////////////////////////////////////////////////////////////////////////
//  Text bit block transfer - TEXTBLT                                   //
//                                                                      //
//  inputs:     SOURCEX,SOURCEY - word                                  //
//                X,Y coordinate of character relative to UL corner of  //
//                font (in pixels)                                      //
//              DESTX,DESTY - word                                      //
//                UL corner of screen area to operate on (in pixels)    //
//              DELX,DELY - word                                        //
//                size of area to operate on (in pixels)                //
//              FBASE - long word                                       //
//                base address of font                                  //
//              FWIDTH - word                                           //
//                Form width of font in bytes                           //
//              WRT_MODE - word                                         //
//                writing mode operation(0-3) 0-replace,  1-transparent //
//                                            2-XOR mode                //
//                                            3-inverse transparent     //
//              STYLE - word                                            //
//                special effects select (thicken,light,skew,underline, //
//                outline, etc.)                                        //
//              R_OFF,L_OFF - word                                      //
//                right & left offset between top & bottom when         //
//                italicizing                                           //
//              WEIGHT - word                                           //
//                used by thicken (magnitude of thickness???)           //
//              LITEMASK,SKEWMASK - word                                //
//                for lighter letters                                   //
//              DOUBLE - word                                           //
//                if non-zero make character twice height & width       //
//              CHUP - word                                             //
//                rotation tenths of degrees (0,900,1800,or 2700 only)  //
//              CLIP - word                                             //
//                if non-zero, clipping bounds (below) in effect        //
//              XMN_CLIP,XMX_CLIP - word                                //
//                clipping region, x coordinate min & max               //
//              YMN_CLIP,YMX_CLIP - word                                //
//                clipping region, y coordinate min & max               //
//                                                                      //
//                added Feb 26, 1985                                    //
//                                                                      //
//              MONO_ST - word                                          //
//                if non-zero (true), thicken effect should not enlarge //
//                source( mono spaced font status flag )                //
//              XACCC_DDA - word                                        //
//                DDA accumulator passed externally                     //
//              DDA_INC - word                                          //
//                DDA increment passed externally                       //
//              T_SCLST - word                                          //
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

#define THICKM                 0x1
#define LIGHTM                 0x2
#define SKEWM                  0x4
#define UNDERM                 0x8
#define OUTLINEM               0x10
#define SHADOWM                0x20
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

        .globl  _text_blt
        .globl  concat          // convert x,y coordinates to buffer offsets

/////////////////////////////////////////////////////////
//  Global routines referenced by this module.         //
/////////////////////////////////////////////////////////

        .extern _act_siz

/////////////////////////////////////////////////////////
//  Global variables referenced.                       //
/////////////////////////////////////////////////////////

        .extern _DEV_TAB
        .extern _SOURCEX,_SOURCEY
        .extern _DESTX,_DESTY
        .extern _DELX,_DELY
        .extern _FBASE
        .extern _FWIDTH
        .extern _WRT_MODE
        .extern _STYLE
        .extern _R_OFF,_L_OFF
        .extern _WEIGHT,_LITEMASK,_SKEWMASK
        .extern _DOUBLE
        .extern _CHUP
        .extern _CLIP
        .extern _MONO_STATUS
        .extern _T_SCLSTS
        .extern _TEXT_FG        //foreground color, text
        .extern _XMN_CLIP,_XMX_CLIP
        .extern _YMN_CLIP,_YMX_CLIP
        .extern _XACC_DDA
        .extern _DDA_INC
        .extern _scrpt2
        .extern _scrtchp
        .extern _shift_offset


/////////////////////////////////////////////////////////
//  Hardware dependent variables referenced.           //
/////////////////////////////////////////////////////////

        .extern _v_bas_ad       //base address of graphic plane
        .extern _v_lin_wr       //size of a scan line in bytes
        .extern _v_planes       //number of graphic planes

/////////////////////////////////////////////////////////
//  Character buffer variables defined in this module. //
/////////////////////////////////////////////////////////

        .globl  _deftxbuf       //base of scratch buffer
        .globl  _scrtsiz

/////////////////////////////////////////////////////////
//  NOTE: The calculations below should serve as an example for //
//        determining the cell size and buffer size required for //
//       creating a scratch character buffer for various sized  //
//        fonts.                                                        //
/////////////////////////////////////////////////////////

//////
//  A larger scratch buffer must be used for character rotation/replication.//
//  Size requirement calculations for this buffer are outlined below.       //
//  NOTE: font dependent equates would normally be found in the font header //
//////

#if test1
////////////////////////////////////////////////////////////////////////
//  test for very large font        //
////////////////////////////////////////////////////////////////////////
#define l_off                   4               //left offset from skew
#define r_off                   17              //right offset from skew
#define form_ht                 43              //form height
#define mxcelwd                 150             //max.cell width (very wide for testing)
#else
//////////////////////////////////////////
//  8x16 font data   //
//////////////////////////////////////////
#define l_off                   2               //left offset from skew
#define r_off                   6               //right offset from skew
#define form_ht                 16              //form height
#define mxcelwd                 8               //maximum cell width
#endif

//////
//  Since a character cell may be rotated 90 or 270 degrees the cell height //
//  and width may be interchanged. The width must be word multiples (ie. 3  //
//  pixel widths requires the 1 word minimum of 16 bits), but the height    //
//  needn't be rounded up in a similiar fashion (since it represents the    //
//  number of rows). Cell width and cell height must be calculated two      //
//  different ways in order to accommodate rotation.                       //
//////

#define cel_ww  ((l_off+r_off+mxcelwd+15)/16)*2 //worst case # bytes/row
//                                              //if width
#define cel_wh  l_off+r_off+mxcelwd     //cell "width" if used as height
                                        //(90 rotation)
#define cel_hh  form_ht                 //cell height if used as height
#define cel_hw  ((form_ht+15)/16)*2     //cell "height" if used as width
                                        //(90 rotation)
/////////////////////////////////////////////////////////
// The maximum of either:                                       //
//                                                              //
//              cell width (as width) * cell height (as height) //
//                                   or                         //
//              cell width (as height) * cell height (as width) //
//                                                              //
//  will be used for the basic buffer size.                     //
/////////////////////////////////////////////////////////

#define cel_sz0 cel_ww*cel_hh   //cell size if no rotation
#define cel_sz9 cel_wh*cel_hw   //cell size for 90 deg rotation

/////////////////////////////////////////////////////////
//  Select the maximum for the character buffer size.           //
/////////////////////////////////////////////////////////

//ifge    cel_sz0-cel_sz9
#if cel_sz0>=cel_sz9
#define cel_siz         cel_sz0*2
//iflt    cel_sz0-cel_sz9
#else
#define cel_siz         cel_sz9*2
#endif

/////////////////////////////////////////////////////////
//  Doubled cell dimension calculations.                        //
/////////////////////////////////////////////////////////

#define cel2_ww         (((2*(l_off+r_off+mxcelwd))+3+15)/16)*2
#define cel2_wh         (2*(l_off+r_off+mxcelwd))+2
#define cel2_hh         (2*form_ht)+2
#define cel2_hw         (((2*form_ht)+3+15)/16)*2

#define cel2_sz0        cel2_ww*cel2_hh //doubled cell size, no rotation
#define cel2_sz9        cel2_wh*cel2_hw //doubled cell size, 90 deg rotation

/////////////////////////////////////////////////////////////////////////
//  Select the maximum size character buffer for doubled characters.     //
/////////////////////////////////////////////////////////////////////////

//ifge    cel2_sz0-cel2_sz9
#if cel2_sz0>=cel2_sz9
#define cel2_siz        cel2_sz0
#else
//iflt    cel2_sz0-cel2_sz9
#define cel2_siz             cel2_sz9
#endif

/////////////////////////////////////////////////////////
//  Determine the maximum horizontal line (from width or height) //
//  which is required for outlining the character buffer. For    //
//  worst case add two bytes.                                   //
/////////////////////////////////////////////////////////

//ifge    cel2_ww-cel2_hw
#if cel2_ww>=cel2_hw
#define out_add         cel2_ww+2
#else
//iflt    cel2_ww-cel2_hw
#define out_add         cel2_hw+2
#endif

/////////////////////////////////////////////////////////
//  Total buffer requirements are: cel_siz + cel2_siz + out_add //
/////////////////////////////////////////////////////////

#define buf_siz         cel_siz+cel2_siz+out_add    //total byte requirement =
//                                                  // small + large buffer

/////////////////////////////////////////////////////////////////////////
//  _text_blt expects variables scratchp & scrpt2 to be                 //
//           initialized as follows:                                    //
//                                                                      //
//   _scrtchp:  is a long word pointer to the base of a buffer with at  //
//              least buf_siz bytes available for scratch use           //
//                                                                      //
//   _scrpt2:   is a word offset within the buffer partitioning it into //
//              a small and a large segment. Should be set to cel_siz   //
//                                                                      //
//      NOTE: _scrtchp & _scrpt2 must be initialized to an even pointer //
//               even offset respectively                               //
/////////////////////////////////////////////////////////////////////////



        .bss
_deftxbuf:
        .ds.b   buf_siz         // char. buffer

        .text // .rodata

#define _scrpt1 0               // small buffer base offset

_scrtsiz:
        .dc.w   cel_siz         // eat up some rom so value can be passed



/////////////////////////////////////////////////////////////////////////
//      Local variables defined on the user stack.                      //
// NOTE: labels follow storage directives for use with LINK instruction  //
/////////////////////////////////////////////////////////////////////////

        //.offset 0

#define buffa   -2              //for clip & prerotate blt
#define buffb   -4              //for rotate
#define buffc   -6              //for replicate
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
#define SKEWMAS -100
#define DELX    -102
#define DESTX   -104
#define DELY    -106
#define DESTY   -108
#define CHUP    -110

///////////////////////////////////////////////
//  Working copies of the clipping variables //
///////////////////////////////////////////////

#define CLIP    -112
#define XMN_CLI -114
#define YMN_CLI -116
#define XMX_CLI -118
#define YMX_CLI -120

//////////////////////////////////
//  Temporary working variables //
//////////////////////////////////

#define tmp_style -122
#define blt_flag  -124
#define chup_flag -126
#define ramlen    -126

        .text
/////////////////////////////////////////////////////////////////////////
//   Global TEXTBLT entry point                                         //
/////////////////////////////////////////////////////////////////////////

_text_blt:
        link    a6,#ramlen              //allocate stack for local variables
        movem.l d2-d7/a2-a5,-(sp)

        clr.w   swap_tmps(a6)           //temps not swapped yet RJG 4/13/85
        move.w  #_scrpt1,buffc(a6)
        lea.l   STYLE(a6),a1
        move.w  _STYLE,(a1)
        move.w  _WRT_MODE,-(a1)
        move.w  _SKEWMASK,-(a1)

        move.w  _DELX,-(a1)
        move.w  _DESTX,-(a1)
        move.w  _DELY,-(a1)
        move.w  _DESTY,-(a1)
        move.w  _CHUP,-(a1)

        move.w  _scrpt2,a0              //use big buffer
        move.w  DELY(a6),d3
        tst.w   _DOUBLE                 //doubling?
        bne.b   adr_dbl                 //br if no
        move.w  DELX(a6),d1
        bra.b   adr_nodb
//
//  added 5 mar 85
//
adr_dbl:
        movem.l a0/a1,-(sp)             //backup used scratch registers
        move.w  d3,-(sp)                //pass DELY to act_siz on stack
        move.l  8(a6),-(sp)             //pass also the parameter Vwk *
        jsr     _act_siz                //
        addq.l  #6,sp                   //remove act_siz parameters from stack
        movem.l (sp)+,a0/a1             //restore used scratch registers
        move.w  d0,d3                   //set new DELY
        move.w  d3,tmp_dely(a6)         //save in temp for replication use
        move.w  DELX(a6),d2             //DELX to d2
        move.w  _XACC_DDA,d4            //get xacc_dda to d4
        move.w  _DDA_INC,d0             //dda_inc to d0
        moveq.l #0,d1                   //d1 = 0
        bra.b   clc_n_su
doub_wid:
        add.w   d0,d4
        bcc.b   no_db_wd
        addq.w  #1,d1
no_db_wd:
        btst.b  #0,_T_SCLSTS+1
        beq.b   clc_n_su
        addq.w  #1,d1
clc_n_su:
        dbra    d2,doub_wid
        move.w  d1,tmp_delx(a6)
//
//  end 5 mar additions
//
//   replacing these 2 instructions..
//
//      lsl.w   d1                      //we'll double in x (_DELX)
//      lsl.w   d3                      //we'll double in y (_DELY)
//
        tst.w   CHUP(a6)                //is character rotated?
        bne.b   adr_nodb                //for a rotated character
//
//  No character rotation
//
        move.w  #_scrpt1,a0             //have to use small buffer 1st
adr_nodb:
        move.w  a0,buffa(a6)            //set up clip & prerotate blt
        move.w  DESTY(a6),d2
        move.w  DESTX(a6),d0
        move.w  _L_OFF,d6
        move.w  _R_OFF,d7
        clr.w   smear(a6)               //in case no thicken init
        btst.b  #THICKEN,STYLE+1(a6)
        beq.b   adr_notk
        move.w  _WEIGHT,d4
        bne.b   adr_th1
        and.w   #0xFFFF-THICKM,STYLE(a6)        //cancel thicken if WEIGHT=0
adr_th1:
        tst.w   _MONO_STATUS                // 26-Feb skip source expand
        bne.b   adr_notk                // 26-Feb if monospaced font

        add.w   d4,d1                   //we'll smear this amount (DELX=DELX+WEIGHT)
adr_notk:
        btst.b  #SKEW,STYLE+1(a6)               //skewin'?
        beq.b   chk_addr                //skip DELX adjust if not

        add.w   d6,d1                   //DELX = DELX + L_OFF + R_OFF
        add.w   d7,d1                   //width increases
//
//  Check_address:
//  Recalculates address of upper left corner of blt area
//  90  address passed is for lower left (move up by DELX)
//  180 address passed is for upper right (move right by DELX)
//  270 address passed is for upper left
//
chk_addr:
        move.w  CHUP(a6),d4             //get rotate degree-tenths
        beq.b   chk_clip                //skip rotate adjust if not rotating
        move.w  #_scrpt1,buffb(a6)      //use the small buffer
        subi.w  #1800,d4                //convert rotation to 3 position flag
        move.w  d4,chup_flag(a6)        //90 = -900, 180 = 0, 270 = 180
        beq.b   add180                  //for 180 degree rotation
        bgt.b   add270                  //for 270 degree rotation
//
//  90 degree rotation
//
        sub.w   d1,d2                   //move up by DELX
        move.w  d2,DESTY(a6)            //DESTY = DESTY - DELX
        exg     d1,d3                   //DELX <-> DELY (swap x & y to check clipping)
        bra.b   chk_clip
//
//  180 degree rotation
//
add180:
        sub.w   d1,d0                   //DESTX = DESTX-DELX(move right by DELX)
        move.w  d0,DESTX(a6)
        bra.b   chk_clip
//
//  270 degree rotation
//
add270:
        exg     d1,d3                   //DELX <-> DELY

//////
//   Check_clip
//
//     trivial accept and reject and set up buffer
//     blt if char must be clipped
//     buffer blt if clip in x or y and skew
//     buffer blt if clip in x and thicken
//////

chk_clip:
        moveq.l #0,d5                   //clear x clip flag (assume no clip)
        move.w  _CLIP,-(a1)             //clip required?
        beq.b   clp_done                //no, skip over clip-stuff
        move.w  _XMN_CLIP,-(a1)
        move.w  _YMN_CLIP,-(a1)
        move.w  _XMX_CLIP,-(a1)
        move.w  _YMX_CLIP,-(a1)
//
//  y -coord clip check..
//
        cmp.w   YMN_CLI(a6),d2          //top edge >= Y-MIN?
        bge.b   chk_ymx                 //br if so, min ok
//
//  partially above clip window, check if totally above..
//
        add.w   d3,d2                   //DESTY = DESTY + DELY
        cmp.w   YMN_CLI(a6),d2          //bottom edge > Y-MIN?
        bgt.b   chk_xmn                 //br to x check if so, still got a piece in window
        bra.w   upda_dst                //else, skip further ado & exit, its gone
chk_ymx:
        cmp.w   YMX_CLI(a6),d2          //top edge > Y-MAX?
        bgt.w   upda_dst                //br to exit if so, its totally off window

        add.w   d3,d2                   //DESTY = DESTY + DELY
        subq.w  #1,d2                   //DESTY 0 based
        sub.w   YMX_CLI(a6),d2          //bottom edge <= Y-MAX
        ble.b   chk_xmn                 //br if so, y-coord wholly within window
//
//  partial clipping of bottom portion..
//
        andi.w  #1,d2                   //clipping odd # of lines?
        beq.b   chk_xmn                 //br if even
        btst.b  #OUTLINE,STYLE+1(a6)    //skip adjust if outlining
        bne.b   chk_xmn
        rol.w   SKEWMAS(a6)             //odd kinda skew mask alignment
//
//  x - coord clip check..
//
chk_xmn:
        cmp.w   XMN_CLI(a6),d0          //left edge >= X-MIN?
        bge.b   chk_xmx                 //br if so, go check X-MAX
//
//  partially to left of clip window, check if totally left..
//
        add.w   d1,d0                   //DESTX = DESTX + DELX
        not.w   d5                      //must clip in x
        cmp.w   XMN_CLI(a6),d0          //right edge > X-MIN?
        bgt.b   clp_done                //br if so, some stuff still in window
        bra.w   upda_dst                //else, totally off screen, exit
chk_xmx:
        cmp.w   XMX_CLI(a6),d0          //left edge > X-MAX?
        bgt.w   upda_dst                //br to exit if so, its totally off window

        add.w   d1,d0                   //DESTX = DESTX + DELX
        subq.w  #1,d0                   //DESTX 0 based
        cmp.w   XMX_CLI(a6),d0          //right edge <= X-MAX?
        ble.b   clp_done                //br if so, no x-clip
        not.w   d5                      //clip we must
//
//  clip check done..
//
clp_done:
        clr.w   dest_wrd(a6)            //use dest_wrd as preblt flag

        move.w  _FWIDTH,s_next(a6)      //add this to go down one line
        move.l  _FBASE,sform(a6)        //start of font

chk_skw:
        move.w  STYLE(a6),d1
        andi.w  #SKEWM+THICKM+OUTLINEM,d1 //skew, thicken, or outline special effects
        beq     chk_rota                //br if not, skip preblt into buffer
//
//  thicken or skew or outline in effect..
//
chk_chup:
        tst.w   CHUP(a6)                //we may have to preblt into buffer
        bne.b   preblt                  //br to preblt if rotation in effect

        btst    #SKEW,d1                //skew bit set in STYLE?
        beq.b   preskew                 //br if not, skip x-clip test
        tst.w   d5                      //did we clip in x
        bne.b   preblt                  //br to preblt if yes
preskew:
        btst    #OUTLINE,d1             //outlining?
        beq     chk_rota                //br if not, no preblt

/////////////////////////////////////////////////////////////////////////
//  Copy the font character into the character buffer. Then manipulate //
//  the copy of the character as necessary for outlining, rotating or  //
//  replicating.                                                       //
/////////////////////////////////////////////////////////////////////////
preblt:
        move.w  _SOURCEX,d0
        move.w  d0,d2
        andi.w  #0x0F,d2
        move.w  d2,tsdad(a6)            // save source dot address
        lsr.w   #4,d0                   // make byte address
        lsl.w   #1,d0                   // make even-byte address (word aligned)

        move.w  _SOURCEY,d2             // source y
        move.w  DELY(a6),height(a6)
        add.w   height(a6),d2           // bottom of source + 1
        subq.w  #1,d2                   // 0 based

        mulu    s_next(a6),d2           // d2 = form offset address for bottom of char
        neg.w   s_next(a6)              // use negative increment to move up
        add.w   d0,d2                   // add x
        movea.l sform(a6),a0
        lea     (a0,d2.w),a0            // a0 -> font source for bottom of char

        move.w  DELX(a6),d0             // char width
        move.w  _WEIGHT,d1              // thicken amount
        add.w   d6,d7                   // d7 = L_OFF + R_OFF
        tst.w   _DOUBLE                 // doubling?
        beq.b   full_wt                 // br if no
        lsr.w   #1,d1                   // only thicken by half the regular amount
        bne.b   ful_wtok                // 26 Feb
        addq.w  #1,d1                   // 26 Feb if thickness = 0 then make = 1
ful_wtok:
        lsr.w   #1,d7
full_wt:
        btst.b  #THICKEN,STYLE+1(a6)    // smear?
        beq.b   no_smear                // br if no

        add.w   d1,d0                   // add adjusted WEIGHT to DELX
        move.w  d1,smear(a6)            // save smear
no_smear:
//  outline code..
        clr.w   tddad(a6)
        move.w  DELY(a6),d1
        move.w  STYLE(a6),d2
        btst    #OUTLINE,d2             // check for outline mode
        beq.b   no_out
        tst.w   _DOUBLE                 // doubling too?
        bne.b   no_out                  // br if yes, outline after doubling

        addq.w  #3,d0                   // add 3 pixels to delx (1 left, 2 right)
        addq.w  #1,tddad(a6)            // make leftmost column blank
        addq.w  #2,DELY(a6)             // add 2 rows (outline adds 2 to height & width)
        addq.w  #3,d1                   // add 3 rows for buffer clear (line buffer)
no_out:
//  end additions
        move.w  d0,width(a6)
        add.w   d7,d0                   // add in skew factor
        move.w  d0,DELX(a6)             // now the char is bigger
        lsr.w   #4,d0
        lsl.w   #1,d0
        addq.w  #2,d0                   // this is the dest width in bytes
        neg.w   d0
        move.w  d0,d_next(a6)           // move upward in buffer
        neg.w   d0
        subq.w  #1,d1                   // start of bottom line
        mulu    d1,d0
        movea.l _scrtchp,a1
        adda.w  buffa(a6),a1            // use this logical buffer
        move.l  a1,sform(a6)            // this will be new source address
        btst    #OUTLINE,d2
        bne.b   do_clear
        btst    #SKEW,d2
        beq.b   no_clear
do_clear:
//  clear buffer first..
        movea.l a1,a2
        move.w  d0,d3
        sub.w   d_next(a6),d3           // count the bottom line
        lsr.w   #1,d3                   // bytes to words
        subq.w  #1,d3                   // adjust words to clear for loop
        moveq.l #0,d1
replp:
        move.w  d1,(a2)+                // clear out buffer for skew
        dbra    d3,replp

        btst    #OUTLINE,d2
        beq.b   no_clear
        tst.w   _DOUBLE
        bne.b   no_clear

        subq.w  #3,width(a6)
        subq.w  #1,DELX(a6)
        add.w   d_next(a6),d0
//
//  buffer now clear..
//
no_clear:
        adda.w  d0,a1                   // start at the bottom
//
//      clr.w   tddad(a6)
//
        move.w  STYLE(a6),tmp_style(a6)

        clr.w   WRT_MOD(a6)             // replace mode for this blt
        move.w  #1,forecol(a6)          // foreground 1 for this blt
        clr.w   ambient(a6)             // background 0 for this blt

        move.w  #1,nbrplane(a6)         // 1 plane, natch
        move.w  #2,nextwrd(a6)          // plane offset is 2
        andi.w  #SKEWM+THICKM,STYLE(a6) // only do thicken & skew

        bsr     norm_blt                // blt source into buffer

        move.w  tmp_style(a6),STYLE(a6)
        move.w  _WRT_MODE,WRT_MOD(a6)
no_mode:
        move.w  d_next(a6),d7           // reset the source to the buffer
        neg.w   d7
        move.w  d7,s_next(a6)
//
//  outline additions
//
        btst.b  #OUTLINE,STYLE+1(a6)    // check outline
        beq.b   skip_out                // br to skip outline code
        tst.w   _DOUBLE
        bne.b   skip_out

        movea.l sform(a6),a0            // get starting address of character
        ext.l   d7
        add.l   d7,sform(a6)            // use 2nd down for buffer top (top is temp buf)
        bsr     outlin                  // outline buffer please
skip_out:
//
//  end additions
//
        clr.w   _SOURCEX
        clr.w   _SOURCEY
//      move.w  STYLE(a6),dest_wrd(a6)  //not used with arbitrarily scaled text
                                        //use as skew preblt indicator for double
        andi.w  #0xFFFF-(SKEWM+THICKM),STYLE(a6)        //cancel effects
//
//  Preblt is done
//
chk_rota:
        tst.w   CHUP(a6)
        beq.b   chk_db

        bsr     rotation                // perform rotation if 90, 180, or 270

chk_db:
        tst.w   _DOUBLE
        beq.b   do_clip

        bsr     replicat                // double if DOUBLE nonzero

///////////////////////////////////////////////////////////
//  clipping                                             //
//      change SOURCEX, SOURCEY, DELX, DELY              //
//      based on DESTX, DESTY (UL corner of destination) //
///////////////////////////////////////////////////////////

do_clip:
        btst.b  #THICKEN,STYLE+1(a6)
        beq.b   no_thik
        move.w  _WEIGHT,d0
        tst.w   _MONO_STATUS                // 26 Feb
        bne.b   do_cl_nt                // 26 Feb
        add.w   d0,DELX(a6)
do_cl_nt:
        move.w  d0,smear(a6)
no_thik:
        tst.w   CLIP(a6)                // clip requested?
        bne.b   clip_adjust             // yes
        clr.w   XMN_CLI(a6)             // otherwise we set up to clip to screen
        clr.w   YMN_CLI(a6)
        move.w  _DEV_TAB,XMX_CLI(a6)
        move.w  _DEV_TAB+2,YMX_CLI(a6)

clip_adjust:
        move.w  DESTY(a6),d0
        cmp.w   YMN_CLI(a6),d0
        bge.b   ymn_fine
        add.w   DELY(a6),d0
        cmp.w   YMN_CLI(a6),d0
        ble.w   upda_dst
mn_clipy:
        sub.w   YMN_CLI(a6),d0
        move.w  DELY(a6),d1
        move.w  d0,DELY(a6)
        sub.w   d0,d1
        add.w   d1,_SOURCEY
        move.w  YMN_CLI(a6),d0
        move.w  d0,DESTY(a6)
ymn_fine:
        cmp.w   YMX_CLI(a6),d0
        bgt.w   upda_dst
mx_clipy:
        add.w   DELY(a6),d0
        subq.w  #1,d0           // make 0 relative
        cmp.w   YMX_CLI(a6),d0
        ble.b   ymx_fine
//clip y
        sub.w   YMX_CLI(a6),d0
        sub.w   d0,DELY(a6)
ymx_fine:
        move.w  DESTX(a6),d0
        cmp.w   XMN_CLI(a6),d0
        bge.b   xmn_fine
        add.w   DELX(a6),d0
        cmp.w   XMN_CLI(a6),d0
        ble     upda_dst
//clip x
mn_clipx:
        sub.w   XMN_CLI(a6),d0
        move.w  DELX(a6),d1
        move.w  d0,DELX(a6)
        sub.w   d0,d1
        add.w   d1,_SOURCEX
        move.w  XMN_CLI(a6),d0
        move.w  d0,DESTX(a6)
xmn_fine:
        cmp.w   XMX_CLI(a6),d0
        bgt     upda_dst
mx_clipx:
        add.w   DELX(a6),d0
        subq.w  #1,d0           // make 0 relative
        cmp.w   XMX_CLI(a6),d0
        ble     scrn_blt
//clip x
        sub.w   XMX_CLI(a6),d0
        sub.w   d0,DELX(a6)

        bra     scrn_blt



/////////////////////////////////////////////////////////////////////////
//  screen blt                                                          //
//      put a block defined by s_form, _SOURCEX, _SOURCEY, _DELX, _DELY //
//      out to screen                                                   //
/////////////////////////////////////////////////////////////////////////

scrn_blt:
        move.w  _TEXT_FG,forecol(a6)    // copy foreground color to temp
        move.w  buffc(a6),ambient(a6)   //
        move.w  _v_planes,d0            // copy # of planes
        move.w  d0,nbrplane(a6)         // save in local
        add.w   d0,d0                   // *2 = offset to next word in same plane
        move.w  d0,nextwrd(a6)          // save in local

        move.w  _SOURCEX,d0
        move.w  d0,d2
        andi.w  #0x0F,d2
        move.w  d2,tsdad(a6)            // save source dot address
        lsr.w   #3,d0                   // make byte address
        andi.w  #0xFFFE,d0              // make even-byte address (word aligned)
        move.w  _SOURCEY,d2

        move.w  DELY(a6),height(a6)
        add.w   height(a6),d2
        subq.w  #1,d2
        mulu    s_next(a6),d2
        neg.w   s_next(a6)
        add.w   d0,d2
//
//  Calculate the starting address for the character to be copied.
//
        movea.l sform(a6),a0
        adda.w  d2,a0                   // calculate character source address
        move.l  a0,sform(a6)            // save source address

        move.w  DELX(a6),width(a6)

        move.w  DESTY(a6),d1
        add.w   DELY(a6),d1
        subq.w  #1,d1                   // we draw from bottom up
        bmi.b   upda_dst                // don't draw if outside screen area!

        move.w  DESTX(a6),d0
        bmi.b   upda_dst                // don't draw if outside screen area!

        jsr     concat                  // get memory address & dot address
//
//  d1.w is screen offset of destination
//
        move.l  _v_bas_ad,a1
        adda.l  d1,a1                   // screen dest. addr. for x,y coordinate

        move.l  a1,dform(a6)            // save character destination address
        move.w  d0,tddad(a6)            // save dot address

        move.w  _v_lin_wr,d0
        neg.w   d0
        move.w  d0,d_next(a6)

        bsr     norm_blt

upda_dst:
        move.w  _DELX,d1

        tst.w   _DOUBLE                 // set up dest address
        beq.b   upda_ndb
//
// added 5 mar 85
//
        move.w  tmp_delx(a6),d1         // get actual char width
        move.w  tmp_dely(a6),d3         // get actual char height

        tst.w   swap_tmps(a6)           // are we swapped
        beq.b   upda_ndb                // if yes swap back rjg 4/14/85
        exg     d1, d3
//
// end additions// following instruction deleted...
//
//      lsl.w   d1                      // we doubled in x
//
upda_ndb:
        btst.b  #THICKEN,_STYLE+1
        beq.b   upda_ntk
        tst.w   _MONO_STATUS                // 26 Feb
        bne.b   upda_ntk                // 26 Feb
        add.w   _WEIGHT,d1              // we smeared this amount
upda_ntk:
        tst.w   CHUP(a6)
        bgt.b   ck90
        add.w   d1,_DESTX               //move left by DELX
        bra.b   blt_done
ck90:
        tst.w   chup_flag(a6)
        beq.b   ck180
        bgt.b   ck270
        sub.w   d1,_DESTY               // move up by DELX
        bra.b   blt_done
ck180:
        sub.w   d1,_DESTX               // move right by DELX
        bra.b   blt_done
ck270:
        add.w   d1,_DESTY               // move down by DELX
blt_done:
        movem.l (sp)+,d2-d7/a2-a5
        unlk    a6
        rts                             // return to "C"

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
        bpl.b   do_rot                  // br if tsdad =< tddad & rotate right
//
//  rotate left
//
        neg.w   d1                      // form 2's cmpliment for positive shift/count
        ori.w   #0x8000,d1              // fake a negative (stripped by ROR or ROL)
        addi.w  #16,d0                  // make word_mask_table index positive
do_rot:
        move.w  d1,shif_cnt(a6)         // save shift count (bit15=1 if ROL, else ROR)

        lsl.w   #1,d0                   // x2 for index
        lea     lf_tab,a2               // get base addr of mask table
        move.w  (a2,d0.w),d0            // set the overlap for middle words
        not.w   d0
        move.w  d0,rota_msk(a6)         // save as rotate mask
//
//    Set up fringe masks..
//
get_mask:
        move.w  tddad(a6),d0            // get destination dot address
        lsl.w   #1,d0                   // x2 for index in d0
        move.w  (a2,d0.w),left_msk(a6)  // get mask for destination dot address

        lsr.w   #1,d0                   // d0 back to tddad
        add.w   width(a6),d0            // add to form right side

//  thicken bug fix..
        move.w  d0,d2                   // copy possibly thickened width to temp d2
        sub.w   smear(a6),d2            // get original before thickened
        andi.w  #0xF,d2
        lsl.w   #1,d2
        move.w  (a2,d2.w),d2
        not.w   d2
        move.w  d2,thk_msk(a6)
        clr.w   d4
        move.w  #0x8000,skew_msk(a6)
        moveq.l #-1,d3                // assume sgl_loop
//  end bug fix

        cmpi.w  #0x10,d0                // more than a word?
        bhi.b   doub_des                // br if bx => 0x10, needs more than 1 word
//
//  Fits in one word
//
        lsl.w   #1,d0                   // x2 for index
        move.w  (a2,d0.w),d1
        not.w   d1
        and.w   d1,left_msk(a6)         // put the two masks together

        moveq.l #-4,d7                  // set blttype flag
        bra.b   msk_done                // exit mask stuff
//
//  Two fringe masks needed..
//
doub_des:
        move.w  d0,d1                   // get tddad +_DELX to d1
        lsr.w   #4,d1                   // divide by 16
        subq.w  #1,d1
        move.w  d1,d3                   // number of words to write for middle
        bne.b   mlt_dest                // br if # of words is non-zero
//
//  # of middle words is zero
//
        move.w  tsdad(a6),d1
        add.w   width(a6),d1
        cmpi.w  #0x20,d1
        bcc.b   mlt_dest                // br if source fits in two words too
        moveq.l #0,d7                   // set blttype flag for double
        bra.b   do_ritem

mlt_dest:
        tst.w   shif_cnt(a6)            // check sign of shift count
        blt.b   mltleft
        moveq.l #4,d7
        bra.b   do_ritem
mltleft:
        moveq.l #8,d7
do_ritem:
        andi.w  #0x000F,d0
        bne.b   not_null
        subq.w  #1,d3                   // last word is full so its a fringe
        moveq.l #0x10,d0
not_null:
        lsl     #1,d0
        move.w  (a2,d0.w),d4
        not.w   d4

msk_done:
        move.w  d7,blt_flag(a6)
        move.w  d3,dest_wrd(a6)
        move.w  d4,rite_msk(a6)
        addq.w  #2,d3
        cmp.w   d2,d4
        bcs.b   msk0
        addq.w  #1,d3
msk0:
        move.w  d3,src_wthk(a6)
        move.w  d3,src_wrd(a6)
        bra.w   plane_loop

////////////////////////////////////
//  writing mode mapping tables:  //
//                                //
//      fb=     00  01  10  11    //
////////////////////////////////////

#define op0             4*0
#define op1             4*1
#define op2             4*2
#define op3             4*3
#define op4             4*4
#define op5             4*5
#define op6             4*6
#define op7             4*7

wrmappin:
        .dc.b   op0,op0,op1,op1         // replace mode
        .dc.b   op2,op2,op3,op3         // transparent mode
        .dc.b   op4,op4,op4,op4         // XOR mode
        .dc.b   op5,op5,op6,op6         // inverse transparent mode

toptable:
        .dc.l   top0,top3,top4,top7,top6,top1,topd
twoptble:
        .dc.l   twop0,twop3,twop4,twop7,twop6,twop1,twopd


plane_loop:
        move.w  WRT_MOD(a6),d0          // d0 = 0000xxxx ,_WRT_MODE
        lsr.w   forecol(a6)             // this plane's foreground bit
        roxl.w  #1,d0
        lsr.w   ambient(a6)
        roxl.w  #1,d0
//
//  d0.w is index
//
        move.b  wrmappin(pc,d0.w),d0
        move.l  toptable(pc,d0.w),a2
        movea.l a2,a3                   // call this address to do tlogicop
        movea.l a2,a5                   // logicop or special effects
                                        // word fringe special effects
        movea.l twoptble(pc,d0.w),a4    // logicop or special effects for words
//
//  Do special effects..
//
        move.w  STYLE(a6),d0            // special effects mask
        beq.w   wrt_char

        btst.l  #LIGHT,d0
        beq.b   no_lite
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
        beq.b   no_thick
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
        beq.b   wrt_char
//
//  Skew special effect..
//
        move.w  SKEWMAS(a6),skew_msk(a6) // reload mask for this char
        move.l  a5,skewjmp(a6)
        lea.l   skewop,a5

        tst.w   blt_flag(a6)
        bgt.b   wrt_char
        beq.b   not_sngl
        clr.w   dest_wrd(a6)
        moveq.l #0,d7
        bra.b   chng_blt
not_sngl:
        cmpi.w  #0x10,width(a6)
        bls.b   wrt_char                // br if source is at most two words

        tst.w   shif_cnt(a6)
        blt.b   chng_left               // br if mlt_rite assumption correct
        moveq.l #4,d7
        bra.b   chng_blt
chng_left:
        moveq.l #8,d7
chng_blt:
        move.w  d7,blt_flag(a6)

wrt_char:
        move.w  blt_flag(a6),d7
        ble.b   set_chr_hgt
        movea.l a2,a5
        move.w  d_next(a6),d5
set_chr_hgt:
        move.w  height(a6),d3           // set loop count
        subq.w  #1,d3

        movea.l blttype+4(pc,d7.w),a2
        jmp     (a2)                    // do the fastest one
next_plane:
        subq.w  #1,nbrplane(a6)         // decrement plane count
        ble.b   no_more_planes          // loop for next plane
        movea.l sform(a6),a0
        movea.l dform(a6),a1
        addq.l  #2,a1                   // advance destination plane
        move.l  a1,dform(a6)            // save dest. starting address
        btst.b  #SKEW,STYLE+1(a6)       // only skew screws up other planes
        beq.w   plane_loop              // br to short init if not skew
        bra.w   norm_blt                // else, do big init
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
        bra.b   sgl_1st_scan

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
        bmi.b   left_rol
        lsr.l   d1,d0
        bra.b   end_ro
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
        bra.w   next_plane

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
        bra.b   dbl_1st_scan

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
        bmi.b   dbleft
        lsr.l   d1,d0                   //align source & destination
        bra.b   dblendr
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
        bra.w   next_plane

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
        bne.w   skewopw
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
        blt.b   lef_don                 //br if we have to mask the last word

        jmp     (a4)                    //call specialw

lft_wrds:
        move.w  d1,(a1)                 //store the result
        adda.w  nextwrd(a6),a1          //advance destination to next word

        bra.b   word_lef                //go for more

lef_don:
        move.w  rite_msk(a6),d2         //load the mask we need

        lea.l   lft_rfrng,a3            //set write mode return address
        jmp     (a5)                    //call specialwf
                                        //feature - this clears thickenover
lft_rfrng:
        move.w  d1,(a1)                 //store the result

        dbra    d3,left_loop            //decrement # of lines to move
        bra.w   next_plane

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
        bne.w   skewopw
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
        blt.b   rite_don                //br if we have to mask the last word

        jmp     (a4)                    //call specialw

rgt_wrds:
        move.w  d1,(a1)                 //store the result
        adda.w  nextwrd(a6),a1          //advance destination to next word

        bra.b   word_rit                //go for more

rite_don:
        move.w  rite_msk(a6),d2         //load the mask we need

        lea.l   rgt_rfrng,a3
        jmp     (a5)                    //call specialwf
                                        //feature - this clears thickenover
rgt_rfrng:
        move.w  d1,(a1)                 //store the result

        dbra    d3,rite_loop            //decrement # of lines to move
        bra.w   next_plane

///////////////////////////////////////////////////
//      Writing Mode Operations using fringe mask //
///////////////////////////////////////////////////

top0:
        not.w   d2                      //mode 0 - D' = 0
        and.w   d2,d4
        not.w   d2
        move.w  d4,d1
        jmp     (a3)

top1:
        not.w   d2                      //mode 1        - D' = S and D
        or.w    d2,d1
        not.w   d2
        and.w   d4,d1
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

top6:
        and.w   d2,d1                   //mode 6 - D' = S xor D (xor mode)
        eor.w   d4,d1
        jmp     (a3)

top7:
        and.w   d2,d1                   //mode 7 - D' = S or D (or mode)
        or.w    d4,d1
        jmp     (a3)

topd:
        not.w   d1                      //mode 13 - D' = [not S] or D
        and.w   d2,d1
        or.w    d4,d1
        jmp     (a3)

///////////////////////////////////////////////////
//      Word Writing Mode Operations             //
///////////////////////////////////////////////////

twop0:
        moveq.l #0,d1                   //mode 0 - D' = 0
        jmp     (a3)

twop1:
        and.w   d4,d1                   //mode 1 - D' = S and D
        jmp     (a3)

twop3:
        jmp     (a3)                    //mode 3 - D' = S (replace mode)

twop4:
        not.w   d1                      //mode 4 - D' = [not S] and D
        and.w   d4,d1
        jmp     (a3)

twop6:
        eor.w   d4,d1                   //mode 6 - D' = S xor D
        jmp     (a3)

twop7:
        or.w    d4,d1                   //mode 7 - D' = S or D
        jmp     (a3)

twopd:
        not.w   d1                      //mode 13 - D' = [not S] or D
        or.w    d4,d1
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
        beq.b   thk00                   //br if not
        ori.b   #0x10,ccr
        roxr.w  #1,d6
        bcc.b   thk01
        move.w  #0x8000,d6
thk01:
        move.w  d6,thk_msk(a6)
thk00:
        tst.w   dest_wrd(a6)
        bmi.b   thk0
        cmp.w   rite_msk(a6),d6
        bcc.b   thk0
        and.w   d6,d0
        bra.b   thk1
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
        bne.b   thk2
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
        bmi.b   thk3
        beq.b   thk4
//  starting left fringe, do left mask only
        subq.w  #1,src_wrd(a6)
        bne.b   thk11
        and.w   d6,d1
thk11:
        and.w   left_msk(a6),d1
        bra.b   thk7
//  right fringe, source data invalid, erase..
thk3:
        clr.w   d1
        bra.b   thk5
//  right fringe, source data AND'd with source mask..
thk4:
        and.w   d6,d1
//  compute mask & count for next line..
thk5:
        tst.w   skew_msk(a6)
        bmi.b   thk6
        move.w  dest_wrd(a6),d5
        addq.w  #2,d5
        move.w  rite_msk(a6),d7
        ori.b   #0x10,ccr
        roxr.w  #1,d7
        bcc.b   thk33
        move.w  #0x8000,d7
        addq.w  #1,d5
thk33:
        cmpi.w  #1,left_msk(a6)
        bne.b   thk34
        subq.w  #1,d5
thk34:
        ori.b   #0x10,ccr
        roxr.w  #1,d6
        bcc.b   thk31
        move.w  #0x8000,d6
thk31:
        cmp.w   d6,d7
        bcs.b   thk35
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
        bcc.b   no_shift

        swap    d1
        move.w  d0,d1                   //pack source words into long one (d1)
        lsr.l   #1,d1

        swap    d2
        move.w  rite_msk(a6),d2         //pack mask words into long one (d2)
        lsr.l   #1,d2
        move.w  d2,rite_msk(a6)
        swap    d2                      //unpack mask words
        move.w  d2,left_msk(a6)
        beq.b   nxt_word
        move.w  shif_cnt(a6),d0
        bmi.b   dec_rol
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
        beq.b   begn_ror
        subq.w  #1,d0
        bra.b   new_shif
begn_ror:
        clr.w   d0
        bra.b   ror_add
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
        bra.b   no_shift

/////////////////////////////////////////////////////////////////////////
//    special effect SKEW for words                                     //
//    recomputes rotation and jumps to proper routine to finish char    //
/////////////////////////////////////////////////////////////////////////

skewopw:
        rol.w   skew_msk(a6)
        bcc.b   do_shift

        tst.w   shif_cnt(a6)
        bmi     mlt_left
        bra     mlt_rite
do_shift:
        ori     #0x10,ccr                       //set x bit
        roxr.w  rota_msk(a6)                    //one more bit into next word

        move.w  rite_msk(a6),d0
        cmpi.w  #0xFFFF,d0                      //if mask is full on
        beq.b   inc_rite

        ori     #0x10,ccr                       //set x bit
        roxr.w  #1,d0                           //rotate in a 1
        move.w  d0,rite_msk(a6)

do_left:
        move.w  shif_cnt(a6),d0
        tst.b   d0
        bne.b   no_rota
        move.w  #0x8000,rota_msk(a6)            //these are the bits that are good
no_rota:
        move.w  left_msk(a6),d1
        lsr.w   #1,d1                           //rotate in a 0
        beq.b   wnxt_wrd                        //br if mask inoperative, inc addr

        move.w  d1,left_msk(a6)

        tst.w   d0
        bmi.b   wdec_rol
        addq.w  #1,shif_cnt(a6)
        bra     mlt_rite
wdec_rol:
        tst.b   d0
        beq.b   set_msk
        subq.w  #1,shif_cnt(a6)                 //do 1 less rol
        bra     mlt_left
set_msk:
        move.w  #1,shif_cnt(a6)
        bra     mlt_rite

inc_rite:
        addq.w  #1,dest_wrd(a6)                 //spilled out of a word to get here
        move.w  #0x8000,rite_msk(a6)
        bra.b   do_left                         //go back & finish up

wnxt_wrd:
        move.w  #0xFFFF,left_msk(a6)            //ran out of word

        subq.w  #1,dest_wrd(a6)                 //so more is in the fringe
        adda.w  nextwrd(a6),a1                  //bump next destination address

        moveq.l #15,d2
        sub.w   d0,d2                           //d0 has old shif_cnt(a6)
        ori.w   #0x8000,d2
        move.w  d2,shif_cnt(a6)

        bra     mlt_left

/////////////////////////
//      rotation in 90 degree increments        //
/////////////////////////
rotation:
        move.w  _SOURCEX,d1
        move.w  d1,d2
        andi.w  #0x0F,d2
        move.w  d2,tsdad(a6)            //save source dot address
        lsr.w   #4,d1                   //make byte address
        moveq.l #1,d5                   //use d5 for quick shift instrs.
        lsl.w   d5,d1

        movea.l sform(a6),a0
        adda.w  d1,a0                   //a0 -> source

        move.w  DELX(a6),width(a6)
        move.w  _SOURCEY,d0             //ax=d0
        move.w  DELY(a6),d1             //bx=d1
        move.w  d1,height(a6)
        move.w  s_next(a6),d2           //cx=d2

        tst.w   chup_flag(a6)           //determine character rotation
        beq     upsd_dwn
rot90:
        blt.b   top_src
        neg.w   s_next(a6)              //go up 1 line
        subq.w  #1,d1
        add.w   d1,d0                   //start at bottom
        mulu    d2,d0                   //get mem address of start corner
        adda.w  d0,a0
top_src:
        move.w  DELY(a6),d0
        lsr.w   #4,d0
        lsl.w   d5,d0
        addq.w  #2,d0                   //form width is height / 8 + 1
        move.w  d0,d_next(a6)
        movea.l _scrtchp,a1
        adda.w  buffb(a6),a1

        tst.w   chup_flag(a6)           //determine if 90 or 270 rotation
        bgt.b   top_dwn                 //
        neg.w   d_next(a6)              //bottom working up
        move.w  DELX(a6),d1             //DELX is the height
        subq.w  #1,d1
        mulu    d1,d0
        adda.w  d0,a1
top_dwn:
        move.w  tsdad(a6),d2
        move.w  #0x8000,d3              //d3=dx
        move.w  d3,d4                   //d4=bp   1st bit of scratch area
        movea.w d3,a4
        lsr.w   d2,d3
        moveq.l #0,d0                   //d0=bx
        move.w  width(a6),d2            //d2=cx   pixels in source row
        move.w  s_next(a6),d6
        movea.l a0,a2
        movea.l a1,a3
        bra.b   rot_nsrc
rot_ylp:
        move.w  height(a6),d1           //d1=cx'
        bra.b   rot_srt
rot_xlp:
        move.w  (a0),d7
        and.w   d3,d7
        beq.b   rot_nor
        or.w    d4,d0
rot_nor:
        ror.w   d5,d4
        bcc.b   rot_isrc
rot_ndst:
        move.w  d0,(a1)+
        moveq.l #0,d0
rot_isrc:
        adda.w  d6,a0                   //add source_next to source ptr
rot_srt:
        dbra    d1,rot_xlp

        move.w  d0,(a1)
        moveq.l #0,d0
        adda.w  d_next(a6),a3
        movea.l a3,a1
        move.w  a4,d4
        ror.w   d5,d3
        bcc.b   rnew_src
        addq.l  #2,a2
rnew_src:
        movea.l a2,a0
rot_nsrc:
        dbra    d2,rot_ylp

rot_done:
        move.w  DELX(a6),d0
        move.w  DELY(a6),d1
        move.w  d1,width(a6)
        move.w  d1,DELX(a6)
        move.w  d0,height(a6)
        move.w  d0,DELY(a6)

        move.w  tmp_dely(a6), d0        //Must swap tmps too RJG 4/13/85
        move.w  tmp_delx(a6), tmp_dely(a6)
        move.w  d0, tmp_delx(a6)
        move.w  #1, swap_tmps(a6)

        move.w  d_next(a6),d0
        tst.w   chup_flag(a6)
        bgt.b   rot_nneg
        neg.w   d0
rot_nneg:
        move.w  d0,s_next(a6)
        move.w  buffb(a6),d0
repexit:
        clr.w   _SOURCEX
repexit3:
        movea.l _scrtchp,a2
        adda.w  d0,a2
        move.l  a2,sform(a6)
        clr.w   _SOURCEY
        rts

upsd_dwn:
        move.w  DELX(a6),d0             //a0 -> top of source
        add.w   tsdad(a6),d0
        subq.w  #1,d0                   //make width instead of address
        lsr.w   #4,d0
        lsl.w   d5,d0                   //make it even byte address
        addq.w  #2,d0                   //form width is DELX / 8 + 1

        move.w  d0,d_next(a6)
        move.w  d0,d2
        lsr.w   d5,d2                   //number of words to move per line
        subq.w  #1,d2                   //for dbra, my sweetie
        mulu    d1,d0                   //d0 pts to bottom of new form
        add.w   buffb(a6),d0
        movea.l _scrtchp,a1
        adda.w  d0,a1                   //a1 -> last word in form
        bra.b   strtflip
upsd_lp:
        movea.l a0,a2                   //use a2 as working source
        move.w  d2,d3                   //copy words per line to temp
line_lp:
        move.w  (a2)+,d0
        moveq.l #0,d6
        moveq.l #15,d4
flip_lp:
        lsr.w   d5,d0
        roxl.w  d5,d6
        dbra    d4,flip_lp

        move.w  d6,-(a1)                //store in buffer pre-decrement
        dbra    d3,line_lp

        adda.w  s_next(a6),a0           //dest (a2) is already updated
strtflip:
        dbra    d1,upsd_lp

        move.w  d_next(a6),s_next(a6)
        movea.l _scrtchp,a1
        adda.w  buffb(a6),a1
        move.l  a1,sform(a6)
        move.w  _SOURCEX,d0
        add.w   DELX(a6),d0
        neg.w   d0
        andi.w  #0xF,d0                 //location of last bit in original
        move.w  d0,_SOURCEX
        clr.w   _SOURCEY
        rts

/////////////////////////
//                                              //
//      replication                             //
//                                              //
/////////////////////////
replicat:
        move.w  _SOURCEX,d0
        move.w  d0,d4                   //d4=si
        andi.w  #0x0F,d0
        move.w  d0,tsdad(a6)            //save source dot address
        lsr.w   #4,d4                   //make byte address
        lsl.w   #1,d4

        move.w  _SOURCEY,d0
        mulu    s_next(a6),d0           //get mem addr of start corner
        add.w   d4,d0
        movea.l sform(a6),a0
        adda.w  d0,a0                   //a0 -> source

        move.w  tsdad(a6),d2            //cx
        move.w  #0x8000,d3              //d3=dx
        move.w  d3,d4                   //d4=bp
        lsr.w   d2,d3

        move.w  DELY(a6),d2
        move.w  DELX(a6),d1

        move.w  d2,height(a6)           //# of rows to duplicate
        move.w  d1,width(a6)
        move.l  _scrtchp,a1
        adda.w  _scrpt2,a1              //dest ptr

        btst    #OUTLINE,STYLE+1(a6)    //outline after double?
        beq.b   noline                  //br if not
//
//  outlining, expand buffer size all around perimeter
//
        addq.w  #1,DELY(a6)             //will add 2 to height when doubled
        addq.w  #1,DELX(a6)             //same with width

        lsl.w   #1,d1                   //buffer width * 2 -> new buffer width
        addq.w  #3,d1                   //new buffer is 3 pixels wider
        lsr.w   #4,d1
        lsl.w   #1,d1                   //# (even) of bytes per row
        addq.w  #2,d1                   //# minimum of 1 word
        move.w  d1,d5
        moveq.l #0,d6
oklear:
        move.w  d6,(a1)+                //clear top 2 rows
        dbra    d5,oklear
        lea     -2(a1),a2
        move.w  d2,d5
        lsl.w   #1,d5
oklear1:
        adda.w  d1,a2
        move.w  d6,(a2)                 //clear 3 pixels on right side
        dbra    d5,oklear1

        move.w  d1,d5
        lsr.w   #1,d5
        subq.w  #1,d5
oklear3:
        move.w  d6,-(a2)
        dbra    d5,oklear3
        bra.b   noline1

noline:
        lsr.w   #3,d1
        lsl.w   #1,d1
        addq.w  #2,d1
noline1:
        move.w  d1,d_next(a6)
//      move.l  a1,a3                   //not used with arbitrarily scaled text
//
//--6 mar --V
        move.w  _T_SCLSTS,d7
        roxr.l  #1,d7
        moveq.l #0,d7
        roxr.l  #1,d7

        move.w  s_next(a6),d7
        move.w  _DDA_INC,d2
        move.w  height(a6),d5
        subq.w  #1,d5
        move.w  #32767,d6
        tst.l   d7
        bmi.b   rep_ylop
y_dwn_lp:
        add.w   d2,d6
        bcc.b   y_no_drw
        bsr     yloop
y_no_drw:
        adda.w  d7,a0
        dbra    d5,y_dwn_lp
        bra.b   y_rep_don
rep_ylop:
        add.w   d2,d6
        bcc.b   y_no_rep
        bsr     yloop
y_no_rep:
        bsr     yloop
        adda.w  d7,a0
        dbra    d5,rep_ylop
y_rep_don:
        move.w  DELX(a6),d2
        subq.w  #1,d2                   //adjust for dbra
        move.w  _XACC_DDA,d1
        move.w  _DDA_INC,d0
        moveq.l #0,d3
repwidcl:
        add.w   d0,d1
        bcc.b   nrepdoub
        addq.w  #1,d3
nrepdoub:
        tst.l   d7
        bpl.b   nrpndoub
        addq.w  #1,d3
nrpndoub:
        dbra    d2,repwidcl
        move.w  d1,_XACC_DDA
        move.w  d3,DELX(a6)
        move.w  tmp_dely(a6),DELY(a6)
//--6 mar --^

        move.w  d_next(a6),d1
        move.w  d1,s_next(a6)
//
//  this don't work with arbitrary scale of text
//
//      andi.w  #SKEWM,dest_wrd(a6)     //did we skew-preblt before doubling?
//      bne     skpreb                  //br to preblt_skew adjust if so
//
//
repexit1:
        btst    #OUTLINE,STYLE+1(a6)
        beq.b   repexit2
        move.l  _scrtchp,a0
        adda.w  _scrpt2,a0
        move.w  d1,d7
        lea     2(a0,d7.w),a1
        bsr     outlin1
        move.w  _scrpt2,d0
        add.w   d_next(a6),d0
        move.w  #15,_SOURCEX
        bra     repexit3
repexit2:
        move.w  _scrpt2,d0
        bra     repexit
//
/////////////////////////////
//
//  yloop routine
//
//
//  entry:      d1 = d_next             preserved
//              d2 = _DDA_INC           preserved
//              d3 = source bitmask     destroyed
//              d4 = dest bitmask       destroyed
//
//              a0 = source ptr         preserved
//              a1 = destination ptr    adjusted for next
//
//                                         8086 Equiv
//  reg use:    d0 = grafix build               bx
//              d1 = d_next                     n/a
//              d2 = _DDA_INC                   di
//              d3 = source bitmask             dx
//              d4 = dest bitmask               bp
//              d5 = width                      cx
//              d6 = temp                       n/a
//              d7 = _XACC_DDA                  si
//
//              a0 = source ptr                 si
//              a1 = destination ptr            di
//              a2 = source data temp           n/a
//
//
//
yloop:
        movem.l d3-d4/d6,-(sp)                  //save some regs
        movea.l a0,a3
        movea.l a1,a4
        movea.l d7,a5
        move.w  d5,wrd_cnt(a6)
        moveq.l #0,d0                           //clear grafix
        move.w  width(a6),d5
        subq.w  #1,d5                           //adjust for dbra

        move.w  _XACC_DDA,d7
        bra.b   nextsrc
innerlp:
        ror.w   #1,d3
        bcc.b   reploop
nextsrc:
        movea.w (a0)+,a2
reploop:
        move.w  a2,d6
        and.w   d3,d6
        bne.b   nrepnor

//--6 mar --V
repnor:
        tst.l   d7
        bmi.b   repnorup
        add.w   d2,d7
        bcc.b   incsrc
        bra.b   ordone
repnorup:
        add.w   d2,d7
        bcc.b   ordone
//--6 mar --^

        ror.w   #1,d4

//--6 mar --V
        bcc.b   ordone
        move.w  d0,(a1)+
        moveq.l #0,d0
//--6 mar --^

        bra.b   ordone
nrepnor:

//--6 mar --V
        add.w   d2,d7
        bcc.b   o_no_rep
//--6 mar --^

        or.w    d4,d0
        ror.w   #1,d4
//--6 mar --V
        bcc.b   o_no_rep
        move.w  d0,(a1)+
        moveq.l #0,d0
o_no_rep:
        tst.l   d7
        bpl.b   incsrc
//--6 mar --^

        or.w    d4,d0
ordone:
        ror.w   #1,d4
        bcc.b   incsrc
nextdst:
        move.w  d0,(a1)+
        moveq.l #0,d0
incsrc:
        dbra    d5,innerlp
repdone:
        move.w  d0,(a1)
        movem.l (sp)+,d3-d4/d6
        movea.l a3,a0
        movea.l a4,a1
        adda.w  d1,a1
        move.l  a5,d7
        move.w  wrd_cnt(a6),d5
        rts

/////////////////////////////////////////////////////////
//   Outline the contents of buffer                             //
//                                                              //
//  a0 -> top line of buffer                                    //
//        (to be used as temp line buffer, assumed cleared)     //
//  d6 = # of vertical lines                                    //
//  d7 = form width in bytes (must be even)                     //
/////////////////////////////////////////////////////////
outlin:
        lea     (a0,d7.w),a1            //bump mid line to "real" top line
outlin1:
        lea     (a1,d7.w),a2            //set up a2 to point to 1 line below current
        lsr.w   #1,d7                   //# of words in horz line
        subq.w  #1,d7                   //for "dbra" sweetie
        move.w  d7,wrd_cnt(a6)          //save char. buffer width( in words )
        move.w  DELY(a6),d6             //# of vertical lines

        movea.l a0,a5
        subq.w  #1,d6
out_edge:
        move.w  d6,a4
        movea.l a2,a3                   //save ptrs & counters
        moveq.l #0,d5
        moveq.l #0,d6
        move.l  (a2),d1                 //get bottom line/left edge grafix data
        lsr.l   #1,d1                   //clear left-most bit too
//
//  within line loop entry point..
//
out_loop:
        move.l  (a0),d0                 //get next top line grafix data
        move.b  d5,d0                   //put bit to left of current data in bit 0
        ror.l   #1,d0                   //now its L 15 14 13 ... 1 0 R X X X...

        move.l  (a1),d2                 //get current line data
        move.b  d6,d2                   //same trick
        move.l  d2,d3                   //d2 is left-shifted current
        ror.l   #1,d3                   //d3 is 0 shifted current
        move.l  d3,d4
        ror.l   #1,d4                   //d4 is right-shifted current

        move.l  d0,d5                   //get copy of top line
        move.l  d0,d6                   //get 2nd copy
        eor.l   d2,d0                   //exclusive neighbor #1
        eor.l   d3,d5                   //exclusive neighbor #2
        eor.l   d4,d6                   //exclusive neighbor #3
        rol.l   #1,d5                      //adjust 0 shifted for final
        rol.l   #2,d6                   //adjust right shifted too
        or.l    d5,d0                   //form exclusive accumulator
        or.l    d6,d0

        move.l  d1,d5                   //now start with a copy of bottom line
        move.l  d1,d6                   //need second copy
        eor.l   d2,d1                   //exclusive neighbor #4
        eor.l   d3,d5                   //exclusive neighbor #5
        eor.l   d4,d6                   //exclusive neighbor #6
        rol.l   #1,d5                   //adjust 0 shifted for final
        rol.l   #2,d6                   //adjust right shifted too
        or.l    d1,d0
        or.l    d5,d0
        or.l    d6,d0

        eor.l   d3,d2                   //exclusive neighbor #7
        eor.l   d3,d4                   //exclusive neighbor #8
        rol.l   #2,d4
        or.l    d2,d0
        or.l    d4,d0
        swap    d0

        move.w  (a1),d6
        move.w  d6,d5
        eor.w   d0,d5
        and.w   d0,d5

        addq.l  #2,a2                   //advance bottom line to next word
        move.l  (a2),d1                 //get next bottom line grafix data
        move.b  -1(a2),d1               //same trick as with top line
        ror.l   #1,d1                   //now its L 15 14 13 ... 1 0 R X X X...

        move.w  d5,(a1)+
        move.w  (a0),d5
        move.w  d6,(a0)+

        dbra    d7,out_loop             //finish rest of line

        move.w  wrd_cnt(a6),d7          //restore buffer width word counter
        movea.l a5,a0                   //restore buffer scratch line addr.
        move.w  a4,d6                   //restore vertical line counter
        movea.l a3,a2                   //save ptrs & counters
        movea.l a2,a1
        adda.w  s_next(a6),a2
        cmpi.w  #1,d6
        bne.b   srt_lin
        movea.l a1,a2
srt_lin:
        dbra    d6,out_edge
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
        movem.l d2/d3/a0,-(sp)  // save the registers that get clobbered

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
        lea     _shift_offset,a0
        add.w   _v_planes,a0
        move.b  (a0),d3         // get pre-calculated shift factor
        lsr.w   d3,d2           // convert x-coordinate to offset

//
// Compute the offset to the desired word by adding the offset to the
// start of the scan line to the offset within the scan line, restore
// the clobbered registers, and exit.
//
        ext.l   d2
        add.l   d2,d1           // compute total offset into screen buffer
        movem.l (sp)+,d2/d3/a0  // restore the clobbered registers
        rts
