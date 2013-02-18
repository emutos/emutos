/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMDISP_H
#define GEMDISP_H

void forkq(void (*fcode)(), ...);
void forker(void);
void chkkbd(void);

#endif

