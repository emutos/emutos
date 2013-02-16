/*
 * mouse.c - Mouse vector setting for XBIOS 0
 *
 * Copyright (c) 2001 EmuTOS development team
 * Copyright (C) 1995 - 1998 Russell King <linux@arm.linux.org.uk>
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  MAD   Martin Doering
 *        Russell King <linux@arm.linux.org.uk>
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 * Some code I got from Linux m68k, thanks to the authors! (MAD)
 */



#include "config.h"
#include "portab.h"
#include "kprint.h"

#include "tosvars.h"
#include "lineavars.h"
#include "ikbd.h"
#include "mouse.h"
#include "vectors.h"


#define DBG_MOUSE 0



struct param rel_pblock;        /* mouse parameter block */



/*
 * mouse_init - mouse initialization
 *
 */

void Initmous(WORD type, struct param *param, void *newvec)
{
    long retval = -1;           /* ok, if it stays so... */
    struct param *p =
        (struct param*)param;   /* pointer to parameter block */

    switch (type) {

    case 0:
        ikbd_writeb(0x12);      /* disable mouse */
        break;

    case 1:
        /* Parameters for relative mouse movement */
        if (param != NULL) {
            ikbd_writeb(0x08);          /* set relative  mouse mode */

            ikbd_writeb(0x0b);          /* set relative treshhold */
            ikbd_writeb(p->xparam);
            ikbd_writeb(p->yparam);
        }
        break;

    case 2:
        /* Parameters for absolute mouse movement */
        if (param != NULL) {
            ikbd_writeb(0x09);          /* set absolute position */
            ikbd_writew(p->xmax);
            ikbd_writew(p->ymax);

            ikbd_writeb(0x0c);          /* set mouse scale */
            ikbd_writeb(p->xparam);
            ikbd_writeb(p->yparam);

            ikbd_writeb(0x0e);          /* set initial position */
            ikbd_writeb(0x00);          /* dummy */
            ikbd_writew(p->xinitial);
            ikbd_writew(p->yinitial);
        }
        break;

    case 4:
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

        if (param != NULL) {
            ikbd_writeb(0x0a);          /* set keyboard mode */
            ikbd_writeb(p->xparam);
            ikbd_writeb(p->yparam);
        }
        break;
    default:
        retval = 0;             // means error
    }

    if (retval!=0 && type!=0) {         /* if no error */

        if (param != NULL) {
            if (p->topmode == IN_YBOT)
                ikbd_writeb(0x0f);      /* set bottom to y=0 */
            if (p->topmode == IN_YTOP)
                ikbd_writeb(0x10);      /* set top to y=0 */

            ikbd_writeb(0x07);          /* set mouse button reaction */
            ikbd_writeb(p->buttons);
        }
        if (newvec != NULL)
            kbdvecs.mousevec = (void*)newvec;   /* set mouse vector */

    } else {                    /* if error */
        kbdvecs.mousevec = (void*) just_rts;    /* set dummy vector */
    }
}
