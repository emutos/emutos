/*
 * clock.h - BIOS time and date routines
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef CLOCK_H
#define CLOCK_H

void clockvec(char *buf);

/* interface for machine.c */

#if CONF_WITH_MEGARTC
void detect_megartc(void);
#endif /* CONF_WITH_MEGARTC */

#if CONF_WITH_ICDRTC
void detect_icdrtc(void);
#endif /* CONF_WITH_ICDRTC */

#if CONF_WITH_MONSTER
void detect_monster_rtc(void);
#endif /* CONF_WITH_MONSTER */

/* internal init */

void clock_init(void);

/* xbios functions */

void settime(LONG time);
LONG gettime(void);

#endif /* CLOCK_H */
