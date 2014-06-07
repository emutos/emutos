/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

void show_hide(WORD fmd, LONG tree);
void draw_fld(LONG tree, WORD obj);
BYTE *last_separator(BYTE *path);
BYTE *add_fname(BYTE *path, BYTE *new_name);
void restore_path(BYTE *target);
void del_fname(BYTE *pstr);
void add_path(BYTE *path, BYTE *new_name);
WNODE *fold_wind(BYTE *path);
WORD d_errmsg(void);
WORD d_doop(WORD level, WORD op, BYTE *psrc_path, BYTE *pdst_path,
            LONG tree, WORD *pfcnt, WORD *pdcnt);
WORD source_is_parent(BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path);
WORD dir_op(WORD op, BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path,
            WORD *pfcnt, WORD *pdcnt, LONG *psize);
