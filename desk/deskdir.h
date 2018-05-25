/*
 * EmuTOS desktop - header for deskdir.c
 *
 * Copyright (C) 2002-2017 The EmuTOS development team
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
    LONG files;
    LONG dirs;
    LONG size;
} DIRCOUNT;

/*
 * function prototypes
 */
void draw_fld(OBJECT *tree, WORD obj);
void draw_dial(OBJECT *tree);
BYTE *add_fname(BYTE *path, BYTE *new_name);
void restore_path(BYTE *target);
void del_fname(BYTE *pstr);
void add_path(BYTE *path, BYTE *new_name);
WORD d_errmsg(WORD err);
WORD d_doop(WORD level, WORD op, BYTE *psrc_path, BYTE *pdst_path, OBJECT *tree, DIRCOUNT *count);
WORD dir_op(WORD op, WORD icontype, PNODE *pspath, BYTE *pdst_path, DIRCOUNT *count);

#endif  /* _DESKDIR_H */
