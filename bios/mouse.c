/*
 * mouse.c - Intelligent keyboard routines
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

#include "bios.h"
#include "tosvars.h"
#include "lineavars.h"
#include "ikbd.h"
#include "mouse.h"
#include "vectors.h"


#define DBG_MOUSE 0

#define	MIN_THRESHOLD 1
#define	MAX_THRESHOLD 20	/* more seems not reasonable... */

/* Begin of mouse storage area */
extern struct mouse_data mdata;   /* from lineavars.S */

WORD oldbutt;

/**
 * mouse_change - notification of a change of mouse position
 *
 * Updates the mouse position and button information. The movement
 * information is updated, and the new button state is saved.
 *
 * @dx: delta X movement
 * @dy: delta Y movement
 * @buttons: new button state
 */
 
void mouse_change(WORD dx, WORD dy, WORD buttons)
{
    struct mouse_data *mse = &mdata;

    WORD changed;

//    spin_lock(&mse->lock);
    changed = (dx != 0 || dy != 0 || mse->buttons != buttons);

    if (changed) {
//        add_mouse_randomness((buttons << 16) + (dy << 8) + dx);

        mse->buttons = buttons;
        mse->dxpos += dx;
        mse->dypos += dy;

        /*
         * keep dx/dy reasonable, but still able to track when X (or
         * whatever) must page or is busy (i.e. long waits between
         * reads)
         */
        if (mse->dxpos < -2048)
            mse->dxpos = -2048;
        if (mse->dxpos > 2048)
            mse->dxpos = 2048;
        if (mse->dypos < -2048)
            mse->dypos = -2048;
        if (mse->dypos > 2048)
            mse->dypos = 2048;
#ifdef DBG_MOUSE
        kprintf("mouse: %d, %d, %x\n", mse->dxpos, mse->dypos, mse->buttons);
#endif
    }

//    spin_unlock(&mse->lock);

    if (changed) {
        /* call button change vector from VDI mouse driver */
    }
}

/**
 * mouse_add_movement - notification of a change of mouse position
 *
 * Updates the mouse position. The movement information is updated.
 *
 * @dx: delta X movement
 * @dy: delta Y movement
 */
 
void mouse_add_movement(WORD dx, WORD dy)
{
    struct mouse_data *mse = &mdata;

    mouse_change(dx, dy, mse->buttons);
}

/**
 * mouse_add_buttons - notification of a change of button state
 *
 * Updates the button state. The buttons are updated by:
 *     	new_state = (old_state & ~clear) ^ eor
 *
 * mousedev - mouse number
 * clear    - mask of buttons to clear
 * eor      - mask of buttons to change
 */
 
void mouse_add_buttons(WORD clear, WORD eor)
{
    struct mouse_data *mse = &mdata;

    mouse_change(0, 0, (mse->buttons & ~clear) ^ eor);
}



/*
 * mouse_int - mouse interrupt vector
 *
 * This routine decodes the mouse packets. Here we just go, if the asm
 * part did see an relative mouse packet.
 */

void mouse_int(BYTE * buf)
{
    WORD buttons;

    struct mouse_data *mse = &mdata;

    /* Mouse unvisible?? */
    if (mse->hide_cnt)
        return;

    /* Set just changed buttons as bitmap */
    buttons = ((buf[0] & 1) | ((buf[0] & 2) << 1) | (oldbutt & 2));
    oldbutt = buttons;

    mouse_change(buf[1], -buf[2], buttons ^ 7);
}



void mouse_init(void)
{
    struct param parm;
    struct mouse_data *mse = &mdata;


    /* Still running? */
#if 0
    if (mse->hide_cnt++)
        return 0;
#endif

    /* These are the vex_* vectors from VDI, called from mouse driver */
    user_but=&just_rts;	        // user button vector
    user_cur=&just_rts;	        // user cursor vector
    user_mot=&just_rts;	        // user motion vector
    tim_addr=&just_rts;	        // user timer vector

    /* reset mouse state */
    mse->dxpos   = 0;
    mse->dypos   = 0;
    mse->buttons = 0;

    parm.topmode = 0;
    parm.buttons = 0;
    parm.xparam = 1;
    parm.yparam = 1;


    Initmous(1, (PTR)&parm, (PTR)&mouse_int);
}


/*
 * mouse_enable - switch on mouse
 */

void mouse_enable(void)
{
    struct mouse_data *mse = &mdata;
}



/*
 * mouse_disable - switch off mouse
 */

void mouse_disable(void)
{
    struct mouse_data *mse = &mdata;

    UBYTE cmd[1] = { 0x12 };

    ikbdws(1, (PTR)cmd);
}



    /* Set mouse button action */
void mouse_button_action(WORD mode)
{
    UBYTE cmd[2] = { 0x07, mode };

    ikbdws(2, (PTR)cmd);
}

/* Set relative mouse position reporting */
void mouse_rel_pos(void)
{
    UBYTE cmd[1] = { 0x08 };

    ikbdws(1, (PTR)cmd);
}

/* Set absolute mouse position reporting */
void mouse_abs_pos(WORD xmax, WORD ymax)
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

void mouse_kbd_mode(WORD deltax, WORD deltay)
{
    BYTE cmd[3] = { 0x0A, deltax, deltay };

    ikbdws(3, (PTR)cmd);
}

/* Set mouse threshold */
void mouse_thresh(WORD x, WORD y)
{
    BYTE cmd[3] = { 0x0B, x, y };

    ikbdws(3, (PTR)cmd);
}

/* Set mouse scale */
void mouse_scale(WORD x, WORD y)
{
    BYTE cmd[3] = { 0x0C, x, y };

    ikbdws(3, (PTR)cmd);
}

/* Interrogate mouse position */
void mouse_pos_get(WORD *x, WORD *y)
{
    UBYTE cmd[1] = { 0x0D };

    ikbdws(1, (PTR)cmd);

    /* wait for returning bytes */
}

/* Load mouse position */
void mouse_pos_set(WORD x, WORD y)
{
    BYTE cmd[6] = { 0x0E, 0x00, x>>8, x&0xFF, y>>8, y&0xFF };

    ikbdws(6, (PTR)cmd);
}

/* Set Y=0 at bottom */
void mouse_y0_bot(void)
{
    UBYTE cmd[1] = { 0x0F };

    ikbdws(1, (PTR)cmd);
}

/* Set Y=0 at top */
void mouse_y0_top(void)
{
    UBYTE cmd[1] = { 0x10 };

    ikbdws(1, (PTR)cmd);
}

/* Resume */
void resume(void)
{
    UBYTE cmd[1] = { 0x11 };

    ikbdws(1, (PTR)cmd);
}

/* Pause output */
void pause(void)
{
    UBYTE cmd[1] = { 0x13 };

    ikbdws(1, (PTR)cmd);
}



/*
 * mouse_init - mouse initialization
 *
 */

void Initmous(WORD type, PTR param, PTR newvec)
{
    struct param *p = (struct param*)param;     /* pointer to parameter block */

    if (newvec != NULL) {
        kbdvecs.mousevec = (VOID*)newvec;      /* set new IKBD Mouse interrupt vector */
    }

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
            if (p->topmode == IN_YBOT)
                mouse_y0_bot();
            if (p->topmode == IN_YTOP)
                mouse_y0_top();
            mouse_button_action(p->buttons);

            /* Set mouse keycode mode */
            mouse_kbd_mode(p->xparam, p->yparam);
        }
        break;
    }
}
