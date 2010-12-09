/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMRSLIB_H
#define GEMRSLIB_H

void rs_obfix(LONG tree, WORD curob);
BYTE *rs_str(UWORD stnum);
LONG get_sub(WORD rsindex, WORD rtype, WORD rsize);
WORD rs_free(LONG pglobal);
WORD rs_gaddr(LONG pglobal, UWORD rtype, UWORD rindex, LONG *rsaddr);
WORD rs_saddr(LONG pglobal, UWORD rtype, UWORD rindex, LONG rsaddr);
WORD rs_readit(LONG pglobal, LONG rsfname);
void rs_fixit(LONG pglobal);
WORD rs_load(LONG pglobal, LONG rsfname);

#endif
