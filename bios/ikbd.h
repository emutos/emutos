/*
 * ikbd.h - Intelligent keyboard routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _IKBD_H
#define _IKBD_H

#include "portab.h"

extern BYTE shifty;

struct keytbl {
  /* 128-sized array giving ascii codes for each scan code */
  BYTE *norm;
  BYTE *shft;
  BYTE *caps;
  /* couples of (scan code, ascii code), ended by byte zero */
  BYTE *altnorm;
  BYTE *altshft;
  BYTE *altcaps;
};

/* initialise the ikbd */
extern void kbd_init(void);

/* called by ikbdvec to handle key events */
extern void kbd_int(WORD scancode);

/* some bios functions */
extern LONG bconstat2(void);
extern LONG bconin2(void);
extern LONG bcostat4(void);
extern void bconout4(WORD dev, WORD c);
extern LONG kbshift(WORD flag);

/* advanced ikbd functions */
extern void ikbd_pause(void);
extern void ikbd_resume(void);
extern void ikbd_reset(void);
extern void atari_kbd_leds (UWORD );

/* some xbios functions */
extern LONG keytbl(LONG norm, LONG shft, LONG caps);
extern WORD kbrate(WORD initial, WORD repeat);
extern void bioskeys(void);

extern void ikbdws(WORD cnt, LONG ptr);
extern void ikbd_writeb(BYTE b);
extern void ikbd_writew(WORD w);

#endif /* _IKBD_H */


