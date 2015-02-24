/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef RECTFUNC_H
#define RECTFUNC_H

UWORD inside(WORD x, WORD y, GRECT *pt);
void rc_copy(GRECT *psbox, GRECT *pdbox);
void rc_constrain(GRECT *pc, GRECT *pt);
WORD rc_equal(GRECT *p1, GRECT *p2);
WORD rc_intersect(GRECT *p1, GRECT *p2);
void rc_union(GRECT *p1, GRECT *p2);
WORD min(WORD a, WORD b);
WORD max(WORD a, WORD b);

#endif
