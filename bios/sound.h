/*
 * sound.h - PSG sound routines
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
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

#if CONF_WITH_YM2149

/* timer C int sound routine */
void sndirq(void);

#endif /* CONF_WITH_YM2149 */

/* the routines below are implemented in assembler in vectors.S, because
 * a user routine hooked in these vectors might clobber registers D2/A2.
 */

/* play bell sound, called by bconout2 */
void bell(void);

/* play key click sound, called by keyboard interrupt */
void keyclick(UBYTE scancode);

#endif /* SOUND_H */
