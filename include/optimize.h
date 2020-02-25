/*
 * EmuTOS AES
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef OPTIMIZE_H
#define OPTIMIZE_H

char *filename_start(char *path);
void fmt_str(char *instr, char *outstr);
void unfmt_str(char *instr, char *outstr);
void inf_sset(OBJECT *tree, WORD obj, char *pstr);
void inf_sget(OBJECT *tree, WORD obj, char *pstr);
WORD inf_gindex(OBJECT *tree, WORD baseobj, WORD numobj);
WORD inf_what(OBJECT *tree, WORD ok, WORD cncl);
char *scan_2(char *pcurr, WORD *pwd);
WORD wildcmp(char *pwld, char *ptst);

#endif
