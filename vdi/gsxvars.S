/*
 * gsxvars.h - GSX system variables
 *
 * Copyright (c) 1999 Caldera, Inc. and Authors:
 *
 *
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "vdiconf.h"


//***************************************************************************//
//      This file contains the variables used by GSX (GEM VDI) gleaned from  //
//      the source files for ROMming.                                        //
//***************************************************************************//
//      Note that most of the variables have gone to bios/lineavars.S!       //
//***************************************************************************//


/////////
//      Overlayable Variables
/////////

                .globl  _COPYTRAN
                .globl  _FLIP_Y
                .globl  _Q
                .globl  _Qbottom
                .globl  _Qhole
                .globl  _Qptr
                .globl  _Qtmp
                .globl  _Qtop
                .globl  _charx
                .globl  _chary
                .globl  _deftxbuf
                .globl  _direction
                .globl  _gotseed
                .globl  _h_align
                .globl  _height
                .globl  _newxleft
                .globl  _newxright
                .globl  _notdone
                .globl  _oldxleft
                .globl  _oldxright
                .globl  _oldy
                .globl  _rmchar
                .globl  _rmcharx
                .globl  _rmchary
                .globl  _rmword
                .globl  _rmwordx
                .globl  _rmwordy
                .globl  _search_color
                .globl  _seed_type
                .globl  _v_align
                .globl  _width
                .globl  _wordx
                .globl  _wordy
                .globl  _xleft
                .globl  _xright

                .globl  _fill_buffer

                .bss
                .even

_fill_buffer:                           // must be 256 words or it will fail
_COPYTRAN:
_seed_type:                             // indicates the type of fill
                .ds.w   1
_search_color:  .ds.w   1               // (+2) the color of the border
_Qbottom:       .ds.w   1               // (+4) the bottom of the Q (zero)
_Qtop:          .ds.w   1               // (+6) points top seed +3
_Qptr:          .ds.w   1               // (+8) points to the active point
_Qtmp:          .ds.w   1               // (+10)
_Qhole:         .ds.w   1               // (+12) an empty space in the Q
_oldy:          .ds.w   1               // (+14) the previous scan line
_oldxleft:      .ds.w   1               // (+16) left end of line at oldy
_oldxright:     .ds.w   1               // (+18) right end
_newxleft:      .ds.w   1               // (+20) ends of line at oldy +
_newxright:     .ds.w   1               // (+22) the current direction
_xleft:         .ds.w   1               // (+24) temporary endpoints
_xright:        .ds.w   1               // (+26)
_direction:     .ds.w   1               // (+28) is next scan line up or down?
_notdone:       .ds.w   1               // (+30) does seedpoint==search_color?
_gotseed:       .ds.w   1               // (+32) a seed was put in the Q
_Q:             .ds.w   3               // (+34) storage for the seed points (.ds.w 200)
_h_align:       .ds.w   1               // (+40)
_v_align:       .ds.w   1               // (+42) scaler alignments
_width:         .ds.w   1               // (+44)
_height:        .ds.w   1               // (+46) extent of string set in dqt_extent
_wordx:         .ds.w   1               // (+48)
_wordy:         .ds.w   1               // (+50) add this to each space for interword
_rmword:        .ds.w   1               // (+52) the number of pixels left over
_rmwordx:       .ds.w   1               // (+54)
_rmwordy:       .ds.w   1               // (+56) add this to use up remainder
_charx:         .ds.w   1               // (+58)
_chary:         .ds.w   1               // (+60) add this to each char for interchar
_rmchar:        .ds.w   1               // (+62) number of pixels left over
_rmcharx:       .ds.w   1               // (+64)
_rmchary:       .ds.w   1               // (+66) add this to use up remainder
_FLIP_Y:        .ds.w   1               // (+68) Non-zero PTSOUT contains magnitueds
                .ds.w   83
_deftxbuf:      .ds.w   138             // (+236) scratch buffer for 8x16 (uses 276 bytes)


/////////
//      Non-Overlayable Variables
/////////

                .globl  m_cdb_bg
                .globl  m_cdb_fg
                .globl  m_pos_hx
                .globl  m_pos_hy
                .globl  mask_form
                .globl  mouse_cdb

// mouse_cdb must be declared out of order since things are equated to it

mouse_cdb:                              // the mouse form storage area  (.ds.w   37)
m_pos_hx:       .ds.w   1               // (+0) Mouse hot spot - x coord
m_pos_hy:       .ds.w   1               // (+2) Mouse hot spot - y coord
                .ds.w   1
m_cdb_bg:       .ds.w   1               // (+6) Mouse background color as pel value
m_cdb_fg:       .ds.w   1               // (+8) Mouse foreground color as pel value
mask_form:      .ds.w   32              // (+10) Storage for mouse mask and cursor


                .end
