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


/* VT52 color definitions */

#define VT_BLACK     0x0000
#define VT_BLUE      0x0007
#define VT_GREEN     0x0070
#define VT_CYAN      0x0077
#define VT_RED       0x0700
#define VT_MAGENTA   0x0707
#define VT_LTGRAY    0x0555
#define VT_GRAY      0x0333
#define VT_LTBLUE    0x0337
#define VT_LTGREEN   0x0373
#define VT_LTCYAN    0x0377
#define VT_LTRED     0x0733
#define VT_LTMAGENTA 0x0737
#define VT_YELLOW    0x0770
#define VT_LTYELLOW  0x0773
#define VT_WHITE     0x0777


#endif /* _SCREEN_H */

