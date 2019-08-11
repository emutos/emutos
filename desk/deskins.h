/*
 * EmuTOS desktop
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKINS_H
#define _DESKINS_H

/*
 * function key numbers (as displayed in dialogs)
 */
#define FIRST_FUNKEY    1
#define LAST_FUNKEY     20
#define NUM_FUNKEYS     (LAST_FUNKEY-FIRST_FUNKEY+1)

WORD is_installed(ANODE *pa);
WORD ins_app(WORD curr);
BOOL ins_devices(void);
WORD ins_icon(WORD sobj);
void ins_shortcut(WORD wh, WORD mx, WORD my);
WORD rmv_icon(WORD sobj);
void snap_icon(WORD x, WORD y, WORD *px, WORD *py, WORD sxoff, WORD syoff);

#endif  /* _DESKINS_H */
