/*
 * EmuTOS desktop
 *
 * Copyright (C) 2002, 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKINS_H
#define _DESKINS_H

BYTE *filename_start(BYTE *path);
WORD is_installed(ANODE *pa);
WORD ins_app(WORD curr);
WORD ins_devices(void);
WORD rmv_icon(WORD sobj);

#endif  /* _DESKINS_H */
