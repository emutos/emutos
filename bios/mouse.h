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
#define IN_YTOP 0       /* Y=0 means top of screen */
#define IN_YBOT 1       /* Y=0 means bottom of screen */
#define IN_PACKETS 3    /* absolute mouse position reported on button press */
#define IN_KEYS 4       /* mouse buttons generate keycodes */


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
    WORD        dxpos;          /* current X position */
    WORD        dypos;          /* current Y position */
    WORD        hide_cnt;       /* 0 = mouse visible */
    WORD        buttons;        /* current mouse button state */
};


/* External declarations */
extern void Initmous(WORD , struct param *, PTR);

extern void mouse_init(void);   /* Initialize mouse */
extern void mouse_int(void);    /* mouse interrupt vector */

/* Mouse specific externals */
extern WORD GCURX;              // mouse X position
extern WORD GCURY;              // mouse Y position
extern WORD HIDE_CNT;           // Number of levels the mouse is hidden
extern WORD MOUSE_BT;           // mouse button state

extern BYTE     draw_flag;      // non-zero means draw mouse form on vblank
extern BYTE     mouse_flag;     // non-zero, if mouse ints disabled
extern BYTE     mouse_flag;     // non-zero, if mouse ints disabled
extern BYTE     cur_ms_stat;    /* current mouse status */


#endif /* _MOUSE_H */

