/*
 * midi.c - MIDI routines
 *
 * Copyright (C) 2001, 2019 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MIDI_H
#define MIDI_H

/* initialise the MIDI ACIA */
void midi_init(void);

/* some bios functions */
LONG bconstat3(void);
LONG bconin3(void);
LONG bcostat3(void);
LONG bconout3(WORD dev, WORD c);

/* some xbios functions */
void midiws(WORD cnt, const UBYTE *ptr);

#endif /* MIDI_H */
