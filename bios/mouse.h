/*
 * mouse.h - mouse routines header
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 * Some code I got from Linux m68k, thanks to the authors! (MAD)
 */

#ifndef _MOUSE_H
#define _MOUSE_H

#include "config.h"

#include "portab.h"


/* Defines for mouse configuration in IKBD */
#define IN_YBOT 0       /* Y=0 means bottom of screen */
#define IN_YTOP 1       /* Y=0 means top of screen */
#define IN_PACKETS 3    /* absolute mouse position reported on button press */
#define IN_KEYS 4       /* mouse buttons generate keycodes */


/* External declarations */
extern void Initmous(WORD , PTR , PTR);

extern void mouse_init(void);
extern void mouse_change(WORD dx, WORD dy, WORD buttons);







/*
 * struct param - Used for parameter passing to mouse_init() function:
 *
 * topmode - indicates, if Y=0 is top or bottom of screen.
 * buttons - bit array, which affect the way mouse clicks are handled.
 *
 * xparam - number of mouse X increments between position report packets.
 * yparam - number of mouse Y increments between position report packets.
 *
 * xmax - maximum X position the mouse should be allowed to move to.
 * ymax - maximum Y position the mouse should be allowed to move to.
 *
 * xinital - The mouse's initial x location.
 * yinital - The mouse's initial y location.
 */

struct param
{
    BYTE      topmode;
    BYTE      buttons;
    BYTE      xparam;
    BYTE      yparam;
    WORD     xmax;
    WORD     ymax;
    WORD     xinitial;
    WORD     yinitial;
};


struct mouse_data {
    WORD	dxpos;          /* current X position */
    WORD	dypos;          /* current Y position */
    WORD        hide_cnt;       /* 0 = mouse visible */
    WORD	buttons;        /* current mouse button state */
};




#endif /* _MOUSE_H */

