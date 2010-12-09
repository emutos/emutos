/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMWRECT_H
#define GEMWRECT_H

extern ORECT    *rul;
extern ORECT    gl_mkrect;


void or_start();
ORECT *get_orect();
void mkrect(LONG tree, WORD wh);
void newrect(LONG tree, WORD wh);

#endif
