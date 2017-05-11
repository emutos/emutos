/*
 * EmuTOS AES
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef RECTFUNC_H
#define RECTFUNC_H

BOOL inside(WORD x, WORD y, const GRECT *pt);
void rc_constrain(const GRECT *pc, GRECT *pt);
WORD rc_equal(const GRECT *p1, const GRECT *p2);
WORD rc_intersect(const GRECT *p1, GRECT *p2);
void rc_union(const GRECT *p1, GRECT *p2);

/*
 *  rc_copy(): copy source to destination rectangle
 */
static __inline__
void rc_copy(const GRECT *psbox, GRECT *pdbox)
{
    *pdbox = *psbox;
}

#endif
