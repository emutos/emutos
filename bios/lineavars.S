/*
 * lineavars.S - Graphics related system variables
 *
 * Copyright (c) 2001 by Authors:
 *
 * Authors:
 *  THO  Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"


                .bss

// ===========================================================================
//      Negative line-a variables come first
// ===========================================================================

                .globl  _mdata
                .globl  _cur_font
                .globl  _cur_ms_stat
                .globl  _disab_cnt
                .globl  _draw_flag

                .globl  _mouse_cdb
                .globl  _m_pos_hx
                .globl  _m_pos_hy
                .globl  _m_cdb_bg
                .globl  _m_cdb_fg
                .globl  _mask_form

                .globl  _mouse_flag
                .globl  _newx
                .globl  _newy
                .globl  retsav
                .globl  _sav_cur_x
                .globl  _sav_cur_y

                .globl  _save_addr
                .globl  _save_area
                .globl  _save_len
                .globl  _save_stat

                .globl  _tim_addr
                .globl  _tim_chain
                .globl  _user_but
                .globl  _user_cur
                .globl  _user_mot
                .globl  _v_cel_ht
                .globl  _v_cel_mx
                .globl  _v_cel_my
                .globl  _v_cel_wr
                .globl  _v_col_bg
                .globl  _v_col_fg
                .globl  _v_cur_ad
                .globl  _v_cur_of
                .globl  _v_cur_cx
                .globl  _v_cur_cy
                .globl  _v_period
                .globl  _v_cur_tim
                .globl  _v_fnt_ad
                .globl  _v_fnt_nd
                .globl  _v_fnt_st
                .globl  _v_fnt_wr
                .globl  _v_hz_rez
                .globl  _v_off_ad
                .globl  _v_stat_0
                .globl  _v_vt_rez
                .globl  _v_bytes_lin
                .globl  _GCURX
                .globl  _GCURY
                .globl  _HIDE_CNT
                .globl  _MOUSE_BT
                .globl  _REQ_COL
                .globl  _SIZ_TAB
                .globl  _TERM_CH
                .globl  _chc_mode
                .globl  _cur_work
                .globl  _def_font
                .globl  _font_ring
                .globl  _font_count
                .globl  _loc_mode
                .globl  _str_mode
                .globl  _val_mode
                .globl  _num_qc_lines
                .globl  _line_cw

                .globl  _DEV_TAB
                .globl  _INQ_TAB


/* Font related VDI variables */

                .ds.l   1       // -910 start angle
_cur_font:      .ds.l   1       // -906 current VDI font
                .ds.w   23      // -902


/* The mouse form storage area: */

_mouse_cdb:
_m_pos_hx:       .ds.w   1       // -856 (cdb+0) Mouse hot spot - x coord
_m_pos_hy:       .ds.w   1       // -854 (cdb+2) Mouse hot spot - y coord
_m_planes:       .ds.w   1       // -852 (cdb+4) Writing mode for mouse pointer
_m_cdb_bg:       .ds.w   1       // -850 (cdb+6) Mouse background color as pel value
_m_cdb_fg:       .ds.w   1       // -848 (cdb+8) Mouse foreground color as pel value
_mask_form:      .ds.w   32      // -846 (cdb+10) Storage for mouse mask and cursor


/* Extended workstation information */

_INQ_TAB:
                .ds.w   45      // -782 information returned from a _vq_extnd()


/* Workstation information */

_DEV_TAB:
                .ds.w   45      // -692 information returned from a _v_opnwk()


/* Mouse data */

_mdata:                         // Block of mouse pointer data
_GCURX:         .ds.w   1       // -602 mouse X position
_GCURY:         .ds.w   1       // -600 mouse Y position
_HIDE_CNT:      .ds.w   1       // -598 Number of levels the mouse is hidden
_MOUSE_BT:      .ds.w   1       // -596 mouse button state

/* RGB values for colors 0-15 */

_REQ_COL:       .ds.w   48      // -594 48 WORDs of RGB data (color registers)

/* Workstation PTSOUT information */

_SIZ_TAB:       .ds.w   15      // -498 size table

_TERM_CH:       .ds.w   1       // -468 pressed key, aciii + scancode
_chc_mode:      .ds.w   1       // -466 input mode choice
_cur_work:      .ds.l   1       // -464 parameter block of current workst.
_def_font:      .ds.l   1       // -460 default font
_font_ring:     .ds.l   4       // -456 three pointers to sysfonts and NULL
_font_count:    .ds.w   1       // -440 total numbers of fonts in font_ring

_line_cw:       .ds.w   1       // -438 Width associated with q_circle data
_loc_mode:      .ds.w   1       // -436 input mode, locator
                .ds.b   80      // some space (input mode???)
_num_qc_lines:  .ds.w   1       // -354 Number of lines making up wide line - not sure if right here

_str_mode:      .ds.w   1       // -352 input mode, string
_val_mode:      .ds.w   1       // -350 input mode, valuator
_cur_ms_stat:   .ds.b   1       // -348 current mouse status
                .ds.b   1
_disab_cnt:     .ds.w   1       // -346 disable depth count. (>0 => disabled)
_newx:          .ds.w   1       // -344 new mouse x&y position
_newy:          .ds.w   1
_draw_flag:     .ds.b   1       // -340 non-zero means draw mouse form on vblank
_mouse_flag:    .ds.b   1       // -339 non-zero if mouse ints disabled

_sav_cur_x:     .ds.w   1       // -338 save area for cursor cell coords.
_sav_cur_y:     .ds.w   1       // -336 save area for cursor cell coords.
retsav:         .ds.l   1       // -334 I'm not sure if this is right here

_save_len:      .ds.w   1       // -330
_save_addr:     .ds.l   1       // -328 return address GEMDOS call
_save_stat:     .ds.w   1       // -324

/* Sprite save buffer */

_save_area:      .ds.l   0x40   // -322

/* Timer vectors */

_tim_addr:      .ds.l   1       // -66  timer interrupt vector
_tim_chain:     .ds.l   1       // -62  timer interrupt vector save
_user_but:      .ds.l   1       // -58  user button vector
_user_cur:      .ds.l   1       // -54  user cursor vector
_user_mot:      .ds.l   1       // -50  user motion vector

/* VDI ESC variables */

_v_cel_ht:      .ds.w   1       // -46  cell height (width is 8)
_v_cel_mx:      .ds.w   1       // -44  columns on the screen minus 1
_v_cel_my:      .ds.w   1       // -42  rows on the screen minus 1
_v_cel_wr:      .ds.w   1       // -40  length (in bytes) of a line of characters
_v_col_bg:      .ds.w   1       // -38  current background color
_v_col_fg:      .ds.w   1       // -36  current foreground color
_v_cur_ad:      .ds.l   1       // -34  current cursor address
_v_cur_of:      .ds.w   1       // -30  offset from begin of screen
_v_cur_cx:      .ds.w   1       // -28  current cursor column
_v_cur_cy:      .ds.w   1       // -26  current cursor row
_v_period:      .ds.b   1       // -24  cursor blink rate
_v_cur_tim:     .ds.b   1       // -23  cursor blink timer.
_v_fnt_ad:      .ds.l   1       // -22  pointer to current monospace font
_v_fnt_nd:      .ds.w   1       // -18  ascii code of last cell in font
_v_fnt_st:      .ds.w   1       // -16  ascii code of first cell in font
_v_fnt_wr:      .ds.w   1       // -14  font cell wrap
_v_hz_rez:      .ds.w   1       // -12  horizontal resolution in pixels
_v_off_ad:      .ds.l   1       // -10  pointer to font offset table
_v_stat_0:      .ds.b   1       // -6   video cell system status (was in words)
                .ds.b   1       //      dummy
_v_vt_rez:      .ds.w   1       // -4   vertical resolution in pixels
_v_bytes_lin:   .ds.w   1       // -2   byte per screen line



// ===========================================================================
// ==== Normal line-a variables now follow
// ===========================================================================

                .globl  line_a_vars

                .globl  _local_pb  // local parameter block for VDI
                .globl  _CONTRL
                .globl  _INTIN
                .globl  _PTSIN
                .globl  _INTOUT
                .globl  _PTSOUT

                .globl  _fg_bp
                .globl  _FG_BP_1
                .globl  _FG_BP_2
                .globl  _FG_BP_3
                .globl  _FG_BP_4

                .globl  _X1
                .globl  _X2
                .globl  _Y1
                .globl  _Y2
                .globl  _LSTLIN
                .globl  _LN_MASK
                .globl  _WRT_MODE

                .globl  _v_planes
                .globl  _v_lin_wr

                .globl  _patptr
                .globl  _patmsk

                .globl  _CLIP
                .globl  _XMN_CLIP
                .globl  _XMX_CLIP
                .globl  _YMN_CLIP
                .globl  _YMX_CLIP

                .globl  _multifill
                .globl  _XACC_DDA
                .globl  _DDA_INC
                .globl  _T_SCLSTS
                .globl  _MONO_STATUS
                .globl  _SOURCEX
                .globl  _SOURCEY
                .globl  _DESTX
                .globl  _DESTY
                .globl  _DELX
                .globl  _DELY
                .globl  _FBASE
                .globl  _FWIDTH
                .globl  _STYLE
                .globl  _LITEMASK
                .globl  _SKEWMASK
                .globl  _WEIGHT
                .globl  _R_OFF
                .globl  _L_OFF
                .globl  _DOUBLE
                .globl  _CHUP
                .globl  _TEXT_FG
                .globl  _scrtchp
                .globl  _scrpt2


// ==== Global GSX Variables =================================================

line_a_vars:                            // This is the base line-a pointer

_v_planes:      ds.w    1               // +0   number of video planes.
_v_lin_wr:      ds.w    1               // +2   number of bytes/video line.

_local_pb:
_CONTRL:        ds.l    1               // +4   ptr to the CONTRL array.
_INTIN:         ds.l    1               // +8   ptr to the INTIN array.
_PTSIN:         ds.l    1               // +12  ptr to the PTSIN array.
_INTOUT:        ds.l    1               // +16  ptr to the INTOUT array.
_PTSOUT:        ds.l    1               // +20  ptr to the PTSOUT array.

// ===========================================================================
//      The following 4 variables are accessed by the line-drawing routines
//      as an array (to allow post-increment addressing).  They must be contiguous!!
// ===========================================================================

_fg_bp:
_FG_BP_1:       ds.w    1               // foreground bit_plane #1 value.
_FG_BP_2:       ds.w    1               // foreground bit_plane #2 value.
_FG_BP_3:       ds.w    1               // foreground bit_plane #3 value.
_FG_BP_4:       ds.w    1               // foreground bit_plane #4 value.

_LSTLIN:        ds.w    1               // 0 => not last line of polyline.
_LN_MASK:       ds.w    1               // line style mask.
_WRT_MODE:      ds.w    1               // writing mode.


_X1:            ds.w    1               // _X1 coordinate for squares
_Y1:            ds.w    1               // _Y1 coordinate for squares
_X2:            ds.w    1               // _X2 coordinate for squares
_Y2:            ds.w    1               // _Y2 coordinate for squares
_patptr:        ds.l    1               // unused by linea now
_patmsk:        ds.w    1               // unused by linea now
_multifill:     ds.w    1               // multi-plane fill flag. (0 => 1 plane)

_CLIP:          ds.w    1               // clipping flag.
_XMN_CLIP:      ds.w    1               // x minimum clipping value.
_YMN_CLIP:      ds.w    1               // y minimum clipping value.
_XMX_CLIP:      ds.w    1               // x maximum clipping value.
_YMX_CLIP:      ds.w    1               // y maximum clipping value.

_XACC_DDA:      ds.w    1               // accumulator for x DDA
_DDA_INC:       ds.w    1               // the fraction to be added to the DDA
_T_SCLSTS:      ds.w    1               // scale up or down flag.
_MONO_STATUS:   ds.w    1               // non-zero - cur font is monospaced
_SOURCEX:       ds.w    1
_SOURCEY:       ds.w    1               // upper left of character in font file
_DESTX:         ds.w    1
_DESTY:         ds.w    1               // upper left of destination on screen
_DELX:          ds.w    1
_DELY:          ds.w    1               // width and height of character
_FBASE:         ds.l    1               // pointer to font data
_FWIDTH:        ds.w    1               // offset,segment and form with of font
_STYLE:         ds.w    1               // special effects
_LITEMASK:      ds.w    1               // special effects
_SKEWMASK:      ds.w    1               // special effects
_WEIGHT:        ds.w    1               // special effects
_R_OFF:         ds.w    1
_L_OFF:         ds.w    1               // skew above and below baseline
_DOUBLE:        ds.w    1               // replicate pixels
_CHUP:          ds.w    1               // character rotation vector
_TEXT_FG:       ds.w    1               // text foreground color
_scrtchp:       ds.l    1               // pointer to base of scratch buffer
_scrpt2:        ds.w    1               // large buffer base offset


// ==== Additional Atari specific things =====================================
// (unused in our VDI, but we have to keep the variables just in case that
//  a program tries to write values there...)
_TEXT_BG:       ds.w    1               // text background color
_COPYTRAN:      ds.w    1               // Flag for Copy-raster-form (<>0 = Transparent)
_FILL_ABORT:    ds.l    1               // Adress of Routine for Test of break of contour fill function


                .end
