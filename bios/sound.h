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

#ifndef _SOUND_H
#define _SOUND_H

#include "portab.h"
 
/* xbios functions */

#define GIACCESS_READ  0x00
#define GIACCESS_WRITE 0x80

extern LONG giaccess(WORD data, WORD reg);
extern VOID ongibit(WORD value);
extern VOID offgibit(WORD value);
extern LONG dosound(LONG table);

/* internal routines */

/* initialize */
VOID snd_init(VOID);

/* timer C int sound routine */
extern VOID sndirq(VOID);

/* play bell sound, called by bconout2 */
VOID bell(VOID);     

/* play key click sound, called by keyboard interrupt */
VOID keyclick(VOID);   

#endif /* _SOUND_H */
