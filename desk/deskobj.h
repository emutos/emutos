/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

void obj_init(void);
WORD obj_walloc(WORD x, WORD y, WORD w, WORD h);
void obj_wfree(WORD obj, WORD x, WORD y, WORD w, WORD h);
WORD obj_ialloc(WORD wparent, WORD x, WORD y, WORD w, WORD h);


