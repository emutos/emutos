/*
 * gemwrect.h - header for EmuTOS AES window rectangle functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMWRECT_H
#define GEMWRECT_H

void or_start(void);
ORECT *get_orect(void);
void newrect(OBJECT *tree, WORD wh);

#endif
