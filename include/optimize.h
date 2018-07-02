/*
 * EmuTOS AES
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef OPTIMIZE_H
#define OPTIMIZE_H

void sound(WORD freq, WORD dura);
WORD disable_sound(WORD control);
void fmt_str(BYTE *instr, BYTE *outstr);
void unfmt_str(BYTE *instr, BYTE *outstr);
void inf_sset(OBJECT *tree, WORD obj, BYTE *pstr);
void inf_sget(OBJECT *tree, WORD obj, BYTE *pstr);
WORD inf_gindex(OBJECT *tree, WORD baseobj, WORD numobj);
WORD inf_what(OBJECT *tree, WORD ok, WORD cncl);
BYTE *scan_2(BYTE *pcurr, WORD *pwd);
WORD wildcmp(BYTE *pwld, BYTE *ptst);
void ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len);

#endif
