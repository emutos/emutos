/*
 * screen.c - low-level screen routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
 
#ifndef _SCREEN_H
#define _SCREEN_H
 
#include "portab.h"
#include "tosvars.h"

/* determine monitor type, ... */
VOID screen_init(VOID);

/* misc routines */


/* xbios routines */

LONG physbase(void);
LONG logbase(void);
WORD getrez(void);
VOID setscreen(LONG logLoc, LONG physLoc, WORD rez);
VOID setpalette(LONG palettePtr);
WORD setcolor(WORD colorNum, WORD color);
VOID vsync(VOID);

#endif /* _SCREEN_H */

