/*
 * screen.c - low-level screen routines
 *
 * Copyright (c) 2001, 2007 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  THH   Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
 
#ifndef SCREEN_H
#define SCREEN_H
 
#include "portab.h"
#include "tosvars.h"

/* determine monitor type, ... */
void screen_init(void);

/* misc routines */
UWORD get_videl_bpp(void);
UWORD get_videl_width(void);
UWORD get_videl_height(void);


/* xbios routines */

LONG physbase(void);
LONG logbase(void);
WORD getrez(void);
void setscreen(LONG logLoc, LONG physLoc, WORD rez, WORD videlmode);
void setpalette(LONG palettePtr);
WORD setcolor(WORD colorNum, WORD color);
void vsync(void);
WORD esetshift(WORD mode);
WORD egetshift(void);
UWORD vsetmode(WORD mode);
WORD vmontype(void);


/* pallette color definitions */

#define RGB_BLACK     0x0000
#define RGB_BLUE      0x000f
#define RGB_GREEN     0x00f0
#define RGB_CYAN      0x00ff
#define RGB_RED       0x0f00
#define RGB_MAGENTA   0x0f0f
#define RGB_LTGRAY    0x0555
#define RGB_GRAY      0x0333
#define RGB_LTBLUE    0x033f
#define RGB_LTGREEN   0x03f3
#define RGB_LTCYAN    0x03ff
#define RGB_LTRED     0x0f33
#define RGB_LTMAGENTA 0x0f3f
#define RGB_YELLOW    0x0ff0
#define RGB_LTYELLOW  0x0ff3
#define RGB_WHITE     0x0fff
                    
#define FRGB_BLACK     0x00000000
#define FRGB_BLUE      0x000000ff
#define FRGB_GREEN     0x00ff0000
#define FRGB_CYAN      0x00ff00ff
#define FRGB_RED       0xff000000
#define FRGB_MAGENTA   0xff0000ff
#define FRGB_LTGRAY    0xbbbb00bb
#define FRGB_GRAY      0x88880088
#define FRGB_LTBLUE    0x000000aa
#define FRGB_LTGREEN   0x00aa0000
#define FRGB_LTCYAN    0x00aa00aa
#define FRGB_LTRED     0xaa000000
#define FRGB_LTMAGENTA 0xaa0000aa
#define FRGB_YELLOW    0xffff0000
#define FRGB_LTYELLOW  0xaaaa0000
#define FRGB_WHITE     0xffff00ff

#endif /* SCREEN_H */

