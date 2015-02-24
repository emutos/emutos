/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

WORD true_closewnd(WNODE *pw);
WORD fun_close(WNODE *pw, WORD trueclose);
void snap_disk(WORD x, WORD y, WORD *px, WORD *py);
WORD desk1_drag(WORD wh, WORD dest_wh, WORD sobj, WORD dobj, WORD mx, WORD my, WORD keystate);
