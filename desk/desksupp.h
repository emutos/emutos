/*
 * desksupp.h - the header for EmuDesk's desksupp.c
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
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

void build_root_path(char *path, WORD drive);
void deselect_all(OBJECT *tree);
void desk_busy_off(void);
void desk_busy_on(void);
void desk_clear(WORD wh);
void desk_clear_all(void);
void desk_verify(WORD wh, WORD changed);
void do_wredraw(WORD w_handle, GRECT *pt);
void do_xyfix(WORD *px, WORD *py);
void do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h);
void do_wfull(WORD wh);
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon,
                char *pathname, GRECT *pt, WORD redraw);
WORD do_aopen(ANODE *pa, WORD isapp, WORD curr, char *pathname, char *pname, char *tail);
WORD do_dopen(WORD curr);
void do_fopen(WNODE *pw, WORD curr, char *pathname, WORD allow_new_win);
WORD do_open(WORD curr);
WORD do_info(WORD curr);
void do_format(void);
void malloc_fail_alert(void);
BOOL print_file(char *name, LONG bufsize, char *iobuf);
void refresh_drive(WORD drive);
void refresh_window(WNODE *pw, BOOL force_mediach);
ANODE *i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp);
WORD set_default_path(char *path);
BOOL valid_drive(char drive);

#if CONF_WITH_DESKTOP_SHORTCUTS
void remove_locate_shortcut(WORD curr);
#endif

#endif  /* _DESKSUPP_H */
