/*
 * gemfslib.h - header for EmuTOS AES File Selector Library functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMFSLIB_H
#define GEMFSLIB_H

WORD fs_input(char *pipath, char *pisel, WORD *pbutton, char *pilabel);
void fs_start(void);

#endif
