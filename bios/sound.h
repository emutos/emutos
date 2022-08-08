/*
 * sound.h - PSG sound routines
 *
 * Copyright (C) 2001-2022 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef SOUND_H
#define SOUND_H

/* xbios functions */

#define GIACCESS_READ  0x00
#define GIACCESS_WRITE 0x80

LONG giaccess(WORD data, WORD reg);
void ongibit(WORD value);
void offgibit(WORD value);
LONG dosound(const UBYTE *table);

/* internal routines */

/* initialize */
void snd_init(void);

/* play bell sound, called by the vt52 handler */
void bell(void);

#if CONF_WITH_YM2149

/* timer C int sound routine */
void sndirq(void);

#endif /* CONF_WITH_YM2149 */

/*
 * the routine below is implemented in assembler in vectors.S, because
 * a user routine hooking into kcl_hook:
 * (a) expects to receive the scancode in d0 rather than on the stack, and
 * (b) might clobber registers d2/a2, which GCC expects to be preserved
 *     across function calls.
 */

/* play key click sound, called by keyboard interrupt */
void keyclick(UBYTE scancode);

/* play cold boot sound */
void coldbootsound(void);

/* play warm boot sound */
void warmbootsound(void);

#endif /* SOUND_H */
