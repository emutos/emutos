/*
 * optimopt.h - misc functions
 *
 * Copyright (c) 2002 EmuTOS DEvelopment Team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#ifndef OPTIMOPT_H
#define OPTIMOPT_H

void r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph);
void r_set(GRECT *pxywh, WORD x, WORD y, WORD w, WORD h);
BYTE *scasb(BYTE *p, BYTE b);

#endif
