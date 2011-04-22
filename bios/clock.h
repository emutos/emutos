/*
 * clock.h - BIOS time and date routines
 *
 * Copyright (c) 2001 by Authors:
 *
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "portab.h"

extern void clockvec(BYTE *buf);

/* interface for machine.c */

extern int has_megartc;
extern void detect_megartc(void);

/* internal init */

extern void clock_init(void);

/* xbios functions */

extern void settime(LONG time);
extern LONG gettime(void);

#endif /* CLOCK_H */

