/*
 * clock.h - BIOS time and date routines
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
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

#if CONF_WITH_MEGARTC
extern void detect_megartc(void);
#endif /* CONF_WITH_MEGARTC */

#if CONF_WITH_ICDRTC
extern void detect_icdrtc(void);
#endif /* CONF_WITH_ICDRTC */

/* internal init */

extern void clock_init(void);

/* xbios functions */

extern void settime(LONG time);
extern LONG gettime(void);

#endif /* CLOCK_H */
