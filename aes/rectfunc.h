/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef RECTFUNC_H
#define RECTFUNC_H

UWORD inside(WORD x, WORD y, GRECT *pt);
WORD rc_equal(GRECT *p1, GRECT *p2);
void rc_copy(GRECT *psbox, GRECT *pdbox);
WORD min(WORD a, WORD b);
WORD max(WORD a, WORD b);

#endif
