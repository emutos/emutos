/*
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define CTL_C   ('C'-0x40)
#define CTL_Q   ('Q'-0x40)
#define CTL_S   ('S'-0x40)

void desk_clear(WORD wh);
void desk_verify(WORD wh, WORD changed);
void do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
void do_xyfix(WORD *px, WORD *py);
void do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h);
WORD do_wfull(WORD wh);
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv,
                BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt, WORD redraw);
void do_fopen(WNODE *pw, WORD curr, WORD drv, BYTE *ppath, BYTE *pname,
              BYTE *pext, WORD chkall, WORD redraw);
WORD do_open(WORD curr);
WORD do_info(WORD curr);
int do_format(WORD curr);
void do_chkall(WORD redraw);
