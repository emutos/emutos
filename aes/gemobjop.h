/*
 * gemobjop.h - header for EmuTOS AES miscellaneous object-related functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMOBJOP_H
#define GEMOBJOP_H

typedef void (*EVERYOBJ_CALLBACK)(OBJECT *tree, WORD obj, WORD sx, WORD sy);

char ob_sst(OBJECT *tree, WORD obj, LONG *pspec, WORD *pstate, WORD *ptype,
            WORD *pflags, GRECT *pt, WORD *pth);
void everyobj(OBJECT *tree, WORD this, WORD last, EVERYOBJ_CALLBACK routine,
              WORD startx, WORD starty, WORD maxdep);
WORD get_par(OBJECT *tree, WORD obj, WORD *pnobj);


#endif
