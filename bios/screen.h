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
 
#ifndef SCREEN_H
#define SCREEN_H
 
#include "portab.h"
#include "tosvars.h"

/* determine monitor type, ... */
void screen_init(void);

/* misc routines */
UWORD get_videl_bpp();
UWORD get_videl_width();
UWORD get_videl_height();
void set_videl_vga640x480(int bitplanes);


/* xbios routines */

LONG physbase(void);
LONG logbase(void);
WORD getrez(void);
void setscreen(LONG logLoc, LONG physLoc, WORD rez);
void setpalette(LONG palettePtr);
WORD setcolor(WORD colorNum, WORD color);
void vsync(void);



/* pallette color definitions */

#define RGB_BLACK     0x0000
#define RGB_BLUE      0x0007
#define RGB_GREEN     0x0070
#define RGB_CYAN      0x0077
#define RGB_RED       0x0700
#define RGB_MAGENTA   0x0707
#define RGB_LTGRAY    0x0555
#define RGB_GRAY      0x0333
#define RGB_LTBLUE    0x0337
#define RGB_LTGREEN   0x0373
#define RGB_LTCYAN    0x0377
#define RGB_LTRED     0x0733
#define RGB_LTMAGENTA 0x0737
#define RGB_YELLOW    0x0770
#define RGB_LTYELLOW  0x0773
#define RGB_WHITE     0x0777

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

