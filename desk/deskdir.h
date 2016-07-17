/*
 * EmuTOS desktop - header for deskdir.c
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKDIR_H
#define _DESKDIR_H

/*
 * typedefs
 */
typedef struct {    /* #files/#folders/total filesize of directory & its subdirectories */
    WORD files;
    WORD dirs;
    LONG size;
} DIRCOUNT;

/*
 * function prototypes
 */
void show_hide(WORD fmd, LONG tree);
void draw_fld(LONG tree, WORD obj);
BYTE *last_separator(BYTE *path);
BYTE *add_fname(BYTE *path, BYTE *new_name);
void restore_path(BYTE *target);
void del_fname(BYTE *pstr);
void add_path(BYTE *path, BYTE *new_name);
WNODE *fold_wind(BYTE *path);
WORD d_errmsg(WORD err);
WORD d_doop(WORD level, WORD op, BYTE *psrc_path, BYTE *pdst_path, LONG tree, DIRCOUNT *count);
WORD dir_op(WORD op, WORD icontype, PNODE *pspath, BYTE *pdst_path, DIRCOUNT *count);

#endif  /* _DESKDIR_H */
