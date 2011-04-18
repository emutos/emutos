/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMFMLIB_H
#define GEMFMLIB_H

extern WORD   ml_ocnt;


void fm_own(WORD beg_ownit);

WORD fm_keybd(LONG tree, WORD obj, WORD *pchar, WORD *pnew_obj);
WORD fm_button(LONG tree, WORD new_obj, WORD clks, WORD *pnew_obj);
WORD fm_do(LONG tree, WORD start_fld);
WORD fm_dial(WORD fmd_type, GRECT *pt);
WORD fm_show(WORD string, WORD *pwd, WORD level);
WORD eralert(WORD n, WORD d);
WORD fm_error(WORD n);

#endif
