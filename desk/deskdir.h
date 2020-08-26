/*
 * deskdir.h - header for EmuDesk's deskdir.c
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
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
char *add_fname(char *path, char *new_name);
void restore_path(char *target);
void del_fname(char *pstr);
void add_path(char *path, char *new_name);
WORD d_doop(WORD level, WORD op, char *psrc_path, char *pdst_path, OBJECT *tree, DIRCOUNT *count);
WORD dir_op(WORD op, WORD icontype, PNODE *pspath, char *pdst_path, DIRCOUNT *count);
WORD illegal_op_msg(void);

#endif  /* _DESKDIR_H */
