/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef DESKPRO_H
#define DESKPRO_H

WORD pro_chdir(WORD drv, BYTE *ppath);
WORD pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr);
WORD pro_exit(BYTE *pcmd, BYTE *ptail);

#endif
