/*
 * sound.h - PSG sound routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef SOUND_H
#define SOUND_H

#include "portab.h"
 
/* xbios functions */

#define GIACCESS_READ  0x00
#define GIACCESS_WRITE 0x80

extern LONG giaccess(WORD data, WORD reg);
extern void ongibit(WORD value);
extern void offgibit(WORD value);
extern LONG dosound(LONG table);

/* internal routines */

/* initialize */
void snd_init(void);

/* timer C int sound routine */
extern void sndirq(void);

/* play bell sound, called by bconout2 */
void bell(void);     

/* play key click sound, called by keyboard interrupt */
void keyclick(void);   

#endif /* SOUND_H */
