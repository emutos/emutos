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

#ifndef _CLOCK_H
#define _CLOCK_H

#include "portab.h"

/* BDOS interface */

#define GET_TIME 0
#define SET_TIME 1
#define GET_DATE 2
#define SET_DATE 3

extern void date_time(WORD flag, WORD *dt);

/* interface for machine.c */

extern int has_megartc;
void detect_megartc(void);

/* internal init */

extern void clock_init(void);

/* xbios functions */

extern void settime(LONG time);
extern LONG gettime(void);

#endif /* _CLOCK_H */

