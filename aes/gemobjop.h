/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMOBJOP_H
#define GEMOBJOP_H

BYTE ob_sst(LONG tree, WORD obj, LONG *pspec, WORD *pstate, WORD *ptype,
            WORD *pflags, GRECT *pt, WORD *pth);
void everyobj(LONG tree, WORD this, WORD last, void (*routine)(),
              WORD startx, WORD starty, WORD maxdep);
WORD get_par(LONG tree, WORD obj, WORD *pnobj);


#endif
