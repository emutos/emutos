/*
 * midi.c - MIDI routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _MIDI_H
#define _MIDI_H

#include "portab.h"

/* initialise the MIDI ACIA */
extern void midi_init(VOID);

/* some bios functions */
extern LONG bconstat3(VOID);
extern LONG bconin3(VOID);
extern LONG bcostat3(VOID);
extern VOID bconout3(WORD dev, WORD c);

/* some xbios functions */
extern VOID midiws(WORD cnt, LONG ptr);

#endif /* _MIDI_H */
