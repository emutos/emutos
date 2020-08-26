/*
 * gemobed.h - header for EmuTOS AES object editing function
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMOBED_H
#define GEMOBED_H

void ob_center(OBJECT *tree, GRECT *pt);
WORD ob_edit(OBJECT *tree, WORD obj, WORD in_char, WORD *idx, WORD kind);
void ins_char(char *str, WORD pos, char chr, WORD tot_len);

#endif
