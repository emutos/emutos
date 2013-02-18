/*
 * ikbd.h - Intelligent keyboard routines
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IKBD_H
#define IKBD_H

#include "portab.h"

/*
 * These bit flags are set in the "shifty" byte based on the state
 * of control keys on the keyboard, like:
 * right shift, left shift, control, alt, ...
 */
#define MODE_RSHIFT 0x01        /* Right Shift key is down */
#define MODE_LSHIFT 0x02        /* Left Shift key is down  */
#define MODE_CTRL   0x04        /* Control is down         */
#define MODE_ALT    0x08        /* Alternate is down       */
#define MODE_CAPS   0x10        /* CapsLock is down        */
#define MODE_CLEAR  0x20        /* Clr/Home is down        */

#define MODE_SHIFT  (MODE_RSHIFT|MODE_LSHIFT)   /* shifted */
#define MODE_SCA    (MODE_RSHIFT|MODE_LSHIFT|MODE_CTRL|MODE_ALT)

extern BYTE shifty;

/*
 * dead key support: i.e. hitting ^ then a yields â.
 * char codes DEADMIN to DEADMAX inclusive are reserved for dead keys.
 * table keytbl->dead[i - DEADMIN], DEADMIN <= i <= DEADMAX, gives the
 * list of couples of (before, after) char codes ended by zero.
 */

/* We use range 1 to 7, as it is best to keep 0 for unallocated keys?
 * and 8 is taken by backspace.
 */
#define DEADMIN 1
#define DEADMAX 7
#define DEAD(i) (i + DEADMIN)

struct keytbl {
  /* 128-sized array giving char codes for each scan code */
  const BYTE *norm;
  const BYTE *shft;
  const BYTE *caps;
  /* couples of (scan code, char code), ended by byte zero */
  const BYTE *altnorm;
  const BYTE *altshft;
  const BYTE *altcaps;
  /* table of at most eight dead key translation tables */
  const BYTE * const *dead;
};

/* initialise the ikbd */
extern void kbd_init(void);

/* called by ikbdvec to handle key events */
extern void kbd_int(WORD scancode);

/* called by timer C int to handle key repeat */
extern void kb_timerc_int(void);

/* some bios functions */
extern LONG bconstat2(void);
extern LONG bconin2(void);
extern LONG bcostat4(void);
extern LONG bconout4(WORD dev, WORD c);
extern LONG kbshift(WORD flag);

/* some xbios functions */
extern LONG keytbl(LONG norm, LONG shft, LONG caps);
extern WORD kbrate(WORD initial, WORD repeat);
extern void bioskeys(void);

extern void ikbdws(WORD cnt, LONG ptr);
extern void ikbd_writeb(UBYTE b);
extern void ikbd_writew(WORD w);

#endif /* IKBD_H */
