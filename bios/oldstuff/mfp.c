/*
 * mfp.c - MFP Initialization
 *
 * Copyright (c) 2001 Laurent Vogel, Martin Doering
 *
 * Authors:
 *  Laurent Vogel
 *  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include <portab.h>

/*==== Defines ============================================================*/
#define acia_ikbd_scr   (*(UBYTE *)0xfffc00);
#define acia_ikbd_dr    (*(UBYTE *)0xfffc02);
#define acia_midi_scr   (*(UBYTE *)0xfffc04);
#define acia_midi_dr    (*(UBYTE *)0xfffc08);

#define mfp_gpip        (*(UBYTE *)0xfffa01)
#define mfp_aer         (*(UBYTE *)0xfffa03)
#define mfp_ddr         (*(UBYTE *)0xfffa05)
#define mfp_ierb        (*(UBYTE *)0xfffa09)
#define mfp_iprb        (*(UBYTE *)0xfffa0d)
#define mfp_isrb        (*(UBYTE *)0xfffa11)
#define mfp_imrb        (*(UBYTE *)0xfffa15)
#define mfp_vr          (*(UBYTE *)0xfffa17)

/*==== Prototypes =========================================================*/
void    exc_keyb(void);

void    send_ikbd_acia(unsigned char c)
{
    while(acia_ikbd_scr & 0x02);
    acia_ikbd_dr = c;
}

/*==== Prototypes =========================================================*/
void    kbd_init(void) {
    /* initialize acias */
    acia_ikbd_cr = 0x03; /* master reset */
    acia_ikbd_cr = 0x96; /* interrupts, clock/64, 8 bit, 1 stop, no parity */
    acia_midi_cr = 0x03; /* master reset */
    acia_midi_cr = 0x15; /* no interrupts, clock/16, 8 bit, 1 stop, no parity */

    /* initialize the IKBD */
    send_ikbd_acia(0x80);   /* reset IKBD */
    send_ikbd_acia(0x01);   /* also... */

    send_ikbd_acia(0x12);  /* disable mouse */
    send_ikbd_acia(0x1A);  /* disable joystick */

    /* initialize the MFP */
    mfp_vr = 0x48;      /* vectors 0x40 to 0x4F, software end of interrupt */

    mfp_ddr &= ~0x08;
    mfp_aer &= ~0x08;

    mfp_imrb |= 0x40;
    mfp_isrb = ~0x40;
    mfp_iprb = ~0x40;
    mfp_ierb |= 0x40;

    /* set the vector */
    (*(void (*)())0x118) = my_0x118_irq;
}

void    exc_keyb(void) {
    unsigned char c;

    /* fetch the character */
    c = acia_ikbd_dr;
    /* do something with it */

    /* signal end of interrupt */
    mfp_isrb = ~0x40;
}

