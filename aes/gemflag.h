/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMFLAG_H
#define GEMFLAG_H

void tchange(LONG c);
WORD tak_flag(SPB *sy);
void amutex(EVB *e, LONG ls);
void unsync(SPB *sy);

#endif
