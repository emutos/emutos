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
extern VOID kbd_init(VOID);

/* called by ikbdvec to handle key events */
extern VOID kbd_int(WORD scancode);

/* some bios functions */
extern LONG bconstat2(VOID);
extern LONG bconin2(VOID);
extern LONG bcostat4(VOID);
extern VOID bconout4(WORD dev, WORD c);
extern LONG kbshift(WORD flag);

/* advanced ikbd functions */
extern VOID ikbd_reset(VOID);
extern VOID ikbd_mouse_button_action(WORD);
extern VOID ikbd_mouse_rel_pos(VOID);
extern VOID ikbd_mouse_abs_pos(WORD , WORD);
extern VOID ikbd_mouse_kbd_mode(WORD, WORD);
extern VOID ikbd_mouse_thresh(WORD, WORD);
extern VOID ikbd_mouse_scale(WORD, WORD);
extern VOID ikbd_mouse_pos_get(WORD *, WORD *);
extern VOID ikbd_mouse_pos_set(WORD , WORD);
extern VOID ikbd_mouse_y0_bot(VOID);
extern VOID ikbd_mouse_y0_top(VOID);
extern VOID ikbd_resume(VOID);
extern VOID ikbd_mouse_disable(VOID);
extern VOID ikbd_pause(VOID);
extern VOID ikbd_joystick_event_on(VOID);
extern VOID ikbd_joystick_event_off(VOID);
extern VOID ikbd_joystick_get_state(VOID);
extern VOID ikbd_joystick_disable(VOID);
extern VOID ikbd_clock_set(WORD , WORD , WORD , WORD , WORD , WORD );
extern VOID ikbd_clock_get(WORD *, WORD *, WORD *, WORD *, WORD *, WORD);
extern VOID ikbd_mem_write(WORD , WORD , BYTE *);
extern VOID ikbd_mem_read(WORD , BYTE []);
extern VOID ikbd_exec(WORD );
extern VOID atari_kbd_leds (UWORD );

/* some xbios functions */
extern LONG keytbl(LONG norm, LONG shft, LONG caps);
extern VOID bioskeys(VOID);
extern VOID ikbdws(WORD cnt, UBYTE *ptr);
extern VOID mouse_init(WORD , LONG , VOID *);

#endif /* _IKBD_H */


