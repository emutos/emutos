/*
 * EmuTOS aes
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMOBLIB_H
#define GEMOBLIB_H

void ob_format(WORD just, BYTE *raw_str, BYTE *tmpl_str, BYTE *fmt_str);
void ob_draw(LONG tree, WORD obj, WORD depth);
WORD ob_find(LONG tree, WORD currobj, WORD depth, WORD mx, WORD my);
void ob_add(LONG tree, WORD parent, WORD child);
WORD ob_delete(LONG tree, WORD obj);
void ob_order(LONG tree, WORD mov_obj, WORD new_pos);
void ob_change(LONG tree, WORD obj, UWORD new_state, WORD redraw);
UWORD ob_fs(LONG tree, WORD ob, WORD *pflag);
void ob_actxywh(LONG tree, WORD obj, GRECT *pt);
void ob_relxywh(LONG tree, WORD obj, GRECT *pt);
void ob_setxywh(LONG tree, WORD obj, GRECT *pt);
void ob_offset(LONG tree, WORD obj, WORD *pxoff, WORD *pyoff);


#endif
