/*
 * midi.c - MIDI routines
 *
 * Copyright (C) 2001 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MIDI_H
#define MIDI_H

#include "portab.h"

/* initialise the MIDI ACIA */
extern void midi_init(void);

/* some bios functions */
extern LONG bconstat3(void);
extern LONG bconin3(void);
extern LONG bcostat3(void);
extern LONG bconout3(WORD dev, WORD c);

/* some xbios functions */
extern void midiws(WORD cnt, LONG ptr);

#endif /* MIDI_H */
