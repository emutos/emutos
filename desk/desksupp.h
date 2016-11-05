/*
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKSUPP_H
#define _DESKSUPP_H

#define CTL_C   ('C'-0x40)
#define CTL_D   ('D'-0x40)
#define CTL_Q   ('Q'-0x40)
#define CTL_S   ('S'-0x40)

void build_root_path(BYTE *path, WORD drive);
void deselect_all(OBJECT *tree);
void desk_clear(WORD wh);
void desk_verify(WORD wh, WORD changed);
void do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
void do_xyfix(WORD *px, WORD *py);
void do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h);
WORD do_wfull(WORD wh);
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon,
                BYTE *pathname, GRECT *pt, WORD redraw);
WORD do_aopen(ANODE *pa, WORD isapp, WORD curr, BYTE *pathname, BYTE *pname);
void do_fopen(WNODE *pw, WORD curr, BYTE *pathname, WORD redraw);
WORD do_open(WORD curr);
WORD do_info(WORD curr);
int do_format(WORD curr);
void do_refresh(WNODE *pw);
ANODE *i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp);
void remove_one_level(BYTE *pathname);
WORD set_default_path(BYTE *path);

#endif  /* _DESKSUPP_H */
