/*
 * gemoblib.h - header for EmuTOS AES Object Library functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMOBLIB_H
#define GEMOBLIB_H

void ob_format(WORD just, char *raw_str, char *tmpl_str, char *fmt_str);
void ob_draw(OBJECT *tree, WORD obj, WORD depth);
WORD ob_find(OBJECT *tree, WORD currobj, WORD depth, WORD mx, WORD my);
void ob_add(OBJECT *tree, WORD parent, WORD child);
WORD ob_delete(OBJECT *tree, WORD obj);
void ob_order(OBJECT *tree, WORD mov_obj, WORD new_pos);
void ob_change(OBJECT *tree, WORD obj, UWORD new_state, WORD redraw);
UWORD ob_fs(OBJECT *tree, WORD ob, WORD *pflag);
void ob_actxywh(OBJECT *tree, WORD obj, GRECT *pt);
void ob_relxywh(OBJECT *tree, WORD obj, GRECT *pt);
void ob_setxywh(OBJECT *tree, WORD obj, GRECT *pt);
void ob_offset(OBJECT *tree, WORD obj, WORD *pxoff, WORD *pyoff);


#endif
