/*
 * conout.h - lowlevel color model dependent screen handling routines
 *
 *
 * Copyright (c) 2004 by Authors:
 *
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/* Defines for cursor */
#define  M_CFLASH       0x0001  // cursor flash         0:disabled 1:enabled
#define  M_CSTATE       0x0002  // cursor flash state   0:off 1:on
#define  M_CVIS         0x0004  // cursor visibility    0:invisible 1:visible

/*
 * The visibility flag is also used as a semaphore to prevent
 * the interrupt-driven cursor blink logic from colliding with
 * escape function/sequence cursor drawing activity.
 */

#define  M_CEOL         0x0008  // end of line handling 0:overwrite 1:wrap
#define  M_REVID        0x0010  // reverse video        0:on        1:off
#define  M_SVPOS        0x0020  // position saved flag. 0:false,    1:true
#define  M_CRIT         0x0040  // reverse video        0:on        1:off

/* Color related linea variables */

extern WORD v_col_bg;           // current background color
extern WORD v_col_fg;           // current foreground color

/* Cursor related linea variables */

extern void *v_cur_ad;          // current cursor address
extern WORD v_cur_of;           // cursor offset
extern WORD v_cur_cx;           // current cursor cell x
extern WORD v_cur_cy;           // current cursor cell y
extern BYTE v_cur_tim;          // cursor blink timer.

extern BYTE v_period;           //
extern WORD disab_cnt;          // disable depth count. (>0 means disabled)
extern BYTE v_stat_0;           // video cell system status
extern WORD sav_cur_x;          // saved cursor cell x
extern WORD sav_cur_y;          // saved cursor cell y

/* Prototypes */

void ascii_out(int);
void move_cursor(int, int);
void blank_out (int, int, int, int);
void invert_cell(int, int);
void scroll_up(int);
void scroll_down(int);
