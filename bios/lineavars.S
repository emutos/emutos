// ===========================================================================
// ==== linea vars - Graphics related system variables
// ===========================================================================



		.bss

// ===========================================================================
//	Negative line-a variables come first
// ===========================================================================

		.globl	cur_ms_stat
		.globl	disab_cnt
		.globl	draw_flag
		.globl	m_cdb_bg
		.globl	m_cdb_fg
		.globl	m_pos_hx
		.globl	m_pos_hy
		.globl	mask_form
		.globl	mouse_cdb
		.globl	mouse_flag
		.globl	newx
		.globl	newy
		.globl	retsav
		.globl	sav_cxy
		.globl	save_addr
		.globl	save_area
		.globl	save_len
		.globl	save_stat
		.globl	tim_addr
		.globl	tim_chain
		.globl	user_but
		.globl	user_cur
		.globl	user_mot
		.globl	v_cel_ht
		.globl	v_cel_mx
		.globl	v_cel_my
		.globl	v_cel_wr
		.globl	v_col_bg
		.globl	v_col_fg
		.globl	v_cur_ad
		.globl	v_cur_of
		.globl	v_cur_cx
		.globl	v_cur_cy
		.globl	v_period
		.globl	v_cur_tim
		.globl	v_fnt_ad
		.globl	v_fnt_nd
		.globl	v_fnt_st
		.globl	v_fnt_wr
		.globl	v_hz_rez
		.globl	v_off_ad
		.globl	v_stat_0
		.globl	v_vt_rez
		.global font_ring
	
// **FIXME: There are still lots of variables missing here...**

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
disab_cnt:	.ds.w	1	// -346  disable depth count. (>0 => disabled)
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
v_cel_ht:	.ds.w	1	// -46   cell height (width is 8)
v_cel_mx:	.ds.w	1	// -44	needed by MiNT: columns on the screen minus 1
v_cel_my:	.ds.w	1	// -42	needed by MiNT: rows on the screen minus 1
v_cel_wr:	.ds.w	1	// -40	needed by MiNT: length (in bytes) of a line of characters
v_col_bg:	.ds.w	1	// -38   current background color 
v_col_fg:	.ds.w	1	// -36   current foreground color 
v_cur_ad:	.ds.l	1	// -34   current cursor address  
v_cur_of:	.ds.w	1	// -30
v_cur_cx:	.ds.w	1	// -28   current cursor cell x 
v_cur_cy:	.ds.w	1	// -26   current cursor cell y
v_period:	.ds.b	1	// -24
v_cur_tim:	.ds.b	1	// -23   cursor blink timer.
v_fnt_ad:	.ds.l	1	// -22   address of current monospace font
v_fnt_nd:	.ds.w	1	// -18   ascii code of last cell in font
v_fnt_st:	.ds.w	1	// -16   ascii code of first cell in font
v_fnt_wr:	.ds.w	1	// -14   font cell wrap 
v_hz_rez:	.ds.w	1	// -12   horizontal pixel resolution
v_off_ad:	.ds.l	1	// -10   address of font offset table
v_stat_0:	.ds.b	1	// -6    video cell system status (was in words)
                .ds.b	1	//       dummy
v_vt_rez:	.ds.w	1	// -4    vertical pixel resolution 
BYTES_LN:	.ds.w	1	// -2



// ===========================================================================
// ==== Normal line-a variables now follow
// ===========================================================================

		.globl	line_a_vars

		.globl	local_pb
		.globl	_CONTRL
		.globl	_INTIN
		.globl	_PTSIN
		.globl	_INTOUT
		.globl	_PTSOUT

		.globl	_FG_BP_1
		.globl	_FG_BP_2
		.globl	_FG_BP_3
		.globl	_FG_BP_4

		.globl	_X1
		.globl	_X2
		.globl	_Y1
		.globl	_Y2
		.globl	_LSTLIN
		.globl	_LN_MASK
		.globl	_WRT_MODE

		.globl	_v_planes
		.globl	_v_lin_wr
                
		.globl	_patptr
		.globl	_patmsk

		.globl	_CLIP
		.globl	_XMN_CLIP
		.globl	_XMX_CLIP
		.globl	_YMN_CLIP
		.globl	_YMX_CLIP

		.globl	_multifill
		.globl	_XACC_DDA
		.globl	_DDA_INC
		.globl	_T_SCLSTS
		.globl	_MONO_STATUS
		.globl	_SOURCEX
		.globl	_SOURCEY
		.globl	_DESTX
		.globl	_DESTY
		.globl	_DELX
		.globl	_DELY
		.globl	_FBASE
		.globl	_FWIDTH
		.globl	_STYLE
		.globl	_LITEMASK
		.globl	_SKEWMASK
		.globl	_WEIGHT
		.globl	_R_OFF
		.globl	_L_OFF
		.globl	_DOUBLE
		.globl	_CHUP
		.globl	_TEXT_FG
		.globl	_scrtchp
		.globl	_scrpt2

// ==== Global GSX Variables =================================================

line_a_vars:                            // This is the base line-a pointer

_v_planes:	ds.w	1		// +0	number of video planes.
_v_lin_wr:	ds.w	1		// +2	number of bytes/video line.

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



		.end
