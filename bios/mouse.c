/*
 * ikbd.c - Intelligent keyboard routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 * Some code I got from Linux m68k, thanks to the authors! (MAD)
 */

#include "config.h"

#include "portab.h"



#include "bios.h"
#include "kprint.h"
#include "ikbd.h"
#include "mouse.h"


#define DBG_MOUSE 0



/* Set mouse button action */
VOID mouse_button_action(WORD mode)
{
    UBYTE cmd[2] = { 0x07, mode };

    ikbdws(2, (PTR)cmd);
}

/* Set relative mouse position reporting */
VOID mouse_rel_pos(VOID)
{
    UBYTE cmd[1] = { 0x08 };

    ikbdws(1, (PTR)cmd);
}

/* Set absolute mouse position reporting */
VOID mouse_abs_pos(WORD xmax, WORD ymax)
{
    BYTE cmd[5] = { 0x09, xmax>>8, xmax&0xFF, ymax>>8, ymax&0xFF };

    ikbdws(5, (PTR)cmd);
}

/*
 * mouse_kbd_mode - Set mouse keycode mode
 *
 * Set mouse monitoring routines to return cursor motion keycodes instead of
 * either RELATIVE or ABSOLUTE motion records. The ikbd returns the
 * appropriate cursor keycode after mouse travel exceeding the user specified
 * deltas in either axis. When the keyboard is in key scan code mode, mouse
 * motion will cause the make code immediately followed by the break code.
 * Note that this command is not affected by the mouse motion origin.
 *
 * deltax - distance in X clicks to return (LEFT) or (RIGHT)
 * deltay - distance in Y clicks to return (UP) or (DOWN)
 */

VOID mouse_kbd_mode(WORD deltax, WORD deltay)
{
    BYTE cmd[3] = { 0x0A, deltax, deltay };

    ikbdws(3, (PTR)cmd);
}

/* Set mouse threshold */
VOID mouse_thresh(WORD x, WORD y)
{
    BYTE cmd[3] = { 0x0B, x, y };

    ikbdws(3, (PTR)cmd);
}

/* Set mouse scale */
VOID mouse_scale(WORD x, WORD y)
{
    BYTE cmd[3] = { 0x0C, x, y };

    ikbdws(3, (PTR)cmd);
}

/* Interrogate mouse position */
VOID mouse_pos_get(WORD *x, WORD *y)
{
    UBYTE cmd[1] = { 0x0D };

    ikbdws(1, (PTR)cmd);

    /* wait for returning bytes */
}

/* Load mouse position */
VOID mouse_pos_set(WORD x, WORD y)
{
    BYTE cmd[6] = { 0x0E, 0x00, x>>8, x&0xFF, y>>8, y&0xFF };

    ikbdws(6, (PTR)cmd);
}

/* Set Y=0 at bottom */
VOID mouse_y0_bot(VOID)
{
    UBYTE cmd[1] = { 0x0F };

    ikbdws(1, (PTR)cmd);
}

/* Set Y=0 at top */
VOID mouse_y0_top(VOID)
{
    UBYTE cmd[1] = { 0x10 };

    ikbdws(1, (PTR)cmd);
}

/* Resume */
VOID resume(VOID)
{
    UBYTE cmd[1] = { 0x11 };

    ikbdws(1, (PTR)cmd);
}

/* Disable mouse */
VOID mouse_disable(VOID)
{
    UBYTE cmd[1] = { 0x12 };

    ikbdws(1, (PTR)cmd);
}

/* Pause output */
VOID pause(VOID)
{
    UBYTE cmd[1] = { 0x13 };

    ikbdws(1, (PTR)cmd);
}



/*
 * mouse_init - mouse initialization
 *
 * The parameter block is not yet implemented. Instead the maximal
 * values of the resolution are used.
 */

VOID mouse_init(WORD type, PTR param, PTR vec)
{
    struct param *p = (struct param*)param;     /* pointer to parameter block */

    switch (type) {

    case 0:
        /* disable mouse */
        mouse_disable();
        break;

    case 1:
        /* Parameters for relative mouse movement */
        if (param != NULL) {
            if (p->topmode == IN_YBOT)
                mouse_y0_bot();
            if (p->topmode == IN_YTOP)
                mouse_y0_top();
            mouse_button_action(p->buttons);

            /* set relative mouse position reporting */
            mouse_thresh(p->xparam, p->yparam);
            mouse_rel_pos();
        }
        break;

    case 2:
        /* Parameters for absolute mouse movement */
        if (param != NULL) {
            if (p->topmode == IN_YBOT)
                mouse_y0_bot();
            if (p->topmode == IN_YTOP)
                mouse_y0_top();
            mouse_button_action(p->buttons);

            /* Set absolute mouse position reporting */
            mouse_scale(p->xparam, p->yparam);
            mouse_pos_set(p->xinitial, p->yinitial);
            mouse_abs_pos(p->xmax, p->ymax);
        }
        break;

    case 4:
        if (param != NULL) {
            mouse_button_action(p->buttons);
            /* Set mouse keycode mode */
            mouse_kbd_mode(p->xparam, p->yparam);
        }
        break;
    }

    if (vec != NULL) {
        mousevec = vec;    /* set new IKBD Mouse interrupt vector */
    }

}
