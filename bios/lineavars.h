/*
 * lineavars.h - name of linea graphics related variables
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Put in this file only the low-mem vars actually used by
 * C code.
 */

#ifndef _LINEAVARS_H
#define _LINEAVARS_H

#include "portab.h"



// ==== Defines ==============================================================

/* Bits for cursor state */
#define M_CFLASH        0x0001  // cursor flash (0:disabled,  1:enabled)
#define M_CSTATE        0x0002  // cursor flash state  (0:off, 1:on)
#define M_CVIS          0x0004  // cursor visibility (0:invisible, 1:visible)



/* Struct for different video mode parameters */
typedef struct
{
    UBYTE	planes;         // count of color planes (v_planes)
    UBYTE	lin_wr;		// bytes per line (v_lin_wr)
    UWORD       hz_rez;         // screen horizontal resolution (v_hz_rez)
    UWORD       vt_rez;         // screen vertical resolution (v_vt_rez)
} VIDEO_MODE;



/* Color related variables */
extern WORD v_col_bg;           // current background color
extern WORD v_col_fg;		// current foreground color

/* Cursor related variables */
extern VOID os_entry(VOID);
extern VOID *v_cur_ad;		// current cursor address
extern WORD v_cur_of;		// cursor offset
extern WORD v_cur_cx;		// current cursor cell x
extern WORD v_cur_cy;		// current cursor cell y
extern BYTE v_cur_tim;		// cursor blink timer.

extern BYTE v_period;		//
extern WORD disab_cnt;		// disable depth count. (>0 => disabled)
extern BYTE v_stat_0;		// video cell system status


/* Screen related variables */
extern UWORD v_planes;         	// count of color planes
extern UWORD v_lin_wr;		// line wrap = bytes per line
extern UWORD v_hz_rez;         	// screen horizontal resolution
extern UWORD v_vt_rez;         	// screen vertical resolution

/* Font specific variables */
extern UWORD *v_fnt_ad;		// address of current monospace font
extern UWORD *v_off_ad;		// address of font offset table
extern UWORD v_fnt_nd;		// ascii code of last cell in font
extern UWORD v_fnt_st;		// ascii code of first cell in font
extern UWORD v_fnt_wr;		// font cell wrap

extern UWORD font_count;   		// all three fonts and NULL
extern struct font_head *font_ring[4];  // all three fonts and NULL


/* Cell specific stuff */
extern UWORD	v_cel_ht;	// cell height (width is 8)
extern UWORD	v_cel_mx;	// needed by MiNT: columns on the screen minus 1
extern UWORD	v_cel_my;	// needed by MiNT: rows on the screen minus 1
extern UWORD	v_cel_wr;	// needed by MiNT: length (in bytes) of a line of characters

extern WORD cursconf(WORD, WORD);       // XBIOS cursor configuration


/* Mouse related variables */
extern WORD     newx;           // new mouse x&y position
extern WORD     newy;           // new mouse x&y position
extern BYTE     draw_flag;      // non-zero means draw mouse form on vblank
extern BYTE     mouse_flag;     // non-zero, if mouse ints disabled


#define NEEDED 0

/* Use the following for now unused variables, if you need them */

#if NEEDED

// ===========================================================================
//	Negative line-a variables come first
// ===========================================================================

GCURX:		.ds.w	1	// -602	MiNT needs GCURX and GCURY
GCURY:		.ds.w	1	// -600	(the current mouse position)
M_HID_CT:	.ds.w	1	// -598
MOUSE_BT:	.ds.w	1	// -596
REQ_COL:	.ds.w	48	// -594
SIZ_TAB:	.ds.w	15	// -498
TERM_CH:	.ds.w	1	//
chc_mode:	.ds.w	1	//
cur_work:	.ds.l	1	// -464
def_font:	.ds.l	1	// -460
font_ring:	.ds.l	4	// -456
ini_font_count:	.ds.w	1	// -440

		.ds.b	90	// some free space (??)

cur_ms_stat:	.ds.b	1	// -348	current mouse status
		.ds.b	1
newx:		.ds.w	1	// -344	new mouse x&y position
newy:		.ds.w	1
draw_flag:	.ds.b	1	// -340	non-zero means draw mouse form on vblank
mouse_flag:	.ds.b	1	// -339	non-zero if mouse ints disabled
sav_cxy:	.ds.w	2	// -338	save area for cursor cell coords.
save_len:	.ds.w	1	// -330
save_addr:	.ds.l	1	// -328
save_stat:	.ds.w	1	// -324
save_area:	.ds.l	0x40	// -322
tim_addr:	.ds.l	1	// -66
tim_chain:	.ds.l	1	// -62
user_but:	.ds.l	1	// -58	user button vector
user_cur:	.ds.l	1	// -54	user cursor vector
user_mot:	.ds.l	1	// -50	user motion vector
                .ds.b	1	//       dummy
BYTES_LN:	.ds.w	1	// -2



// ===========================================================================
// ==== Normal line-a variables now follow
// ===========================================================================


// ==== Global GSX Variables =================================================

line_a_vars:                            // This is the base line-a pointer

local_pb:
_CONTRL:	ds.l	1		// +4	ptr to the CONTRL array.
_INTIN:		ds.l	1		// +8	ptr to the INTIN array.
_PTSIN:		ds.l	1		// +12	ptr to the PTSIN array.
_INTOUT: 	ds.l	1		// +16	ptr to the INTOUT array.
_PTSOUT: 	ds.l	1		// +20	ptr to the PTSOUT array.

// ===========================================================================
//	The following 4 variables are accessed by the line-drawing routines
//	as an array (to allow post-increment addressing).  They must be contiguous!!
// ===========================================================================

_FG_BP_1:	ds.w	1		// foreground bit_plane #1 value.
_FG_BP_2:	ds.w	1		// foreground bit_plane #2 value.
_FG_BP_3:	ds.w	1		// foreground bit_plane #3 value.
_FG_BP_4:	ds.w	1		// foreground bit_plane #4 value.

_LSTLIN: 	ds.w	1		// 0 => not last line of polyline.
_LN_MASK:	ds.w	1		// line style mask.
_WRT_MODE:	ds.w	1		// writing mode.


_X1:		ds.w	1		// _X1 coordinate for squares
_Y1:		ds.w	1		// _Y1 coordinate for squares
_X2:		ds.w	1		// _X2 coordinate for squares
_Y2:		ds.w	1		// _Y2 coordinate for squares
_patptr: 	ds.l	1		// pointer to fill pattern.
_patmsk: 	ds.w	1		// pattern index. (mask)
_multifill:	ds.w	1		// multi-plane fill flag. (0 => 1 plane)

_CLIP:		ds.w	1		// clipping flag.
_XMN_CLIP:	ds.w	1		// x minimum clipping value.
_YMN_CLIP:	ds.w	1		// y minimum clipping value.
_XMX_CLIP:	ds.w	1		// x maximum clipping value.
_YMX_CLIP:	ds.w	1		// y maximum clipping value.

_XACC_DDA:	ds.w	1		// accumulator for x DDA
_DDA_INC:	ds.w	1		// the fraction to be added to the DDA
_T_SCLSTS:	ds.w	1		// scale up or down flag.
_MONO_STATUS:	ds.w	1		// non-zero - cur font is monospaced
_SOURCEX:	ds.w	1
_SOURCEY:	ds.w	1		// upper left of character in font file
_DESTX:		ds.w	1
_DESTY:		ds.w	1		// upper left of destination on screen
_DELX:		ds.w	1
_DELY:		ds.w	1		// width and height of character
_FBASE:		ds.l	1		// pointer to font data
_FWIDTH: 	ds.w	1		// offset,segment and form with of font
_STYLE:		ds.w	1		// special effects
_LITEMASK:	ds.w	1		// special effects
_SKEWMASK:	ds.w	1		// special effects
_WEIGHT: 	ds.w	1		// special effects
_R_OFF:		ds.w	1
_L_OFF:		ds.w	1		// skew above and below baseline
_DOUBLE: 	ds.w	1		// replicate pixels
_CHUP:		ds.w	1		// character rotation vector
_TEXT_FG:	ds.w	1		// text foreground color
_scrtchp:	ds.l	1		// pointer to base of scratch buffer
_scrpt2: 	ds.w	1		// large buffer base offset

// ==== Additional Atari specific things =====================================
_TEXT_BG:	 ds.w	 1		 // text foreground color
_COPYTRAN:	 ds.w	 1		 // Flag for Copy-raster-form (<>0 = Transparent)
_FILL_ABORT:	 ds.l	 1		 // Adress of Routine for Test of break of contour fill function

#endif /* NEEDED */

#endif /* _LINEAVARS_H */
