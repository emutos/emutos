/*
 * ikbd.h - Intelligent keyboard routines
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IKBD_H
#define IKBD_H

#include "biosdefs.h"

/*
 * These bit flags are set in the "shifty" byte based on the state
 * of control keys on the keyboard, like:
 * right shift, left shift, control, alt, ...
 * Public flags are defined in include/biosdefs.h and returned by Kbshift().
 * Private BIOS combinations are defined below.
 */

#define MODE_SHIFT  (MODE_RSHIFT|MODE_LSHIFT)   /* shifted */

#define HOTSWITCH_MODE (MODE_LSHIFT|MODE_ALT)

extern UBYTE shifty;

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

/*
 * bit flags in 'features' in struct keytbl:
 *  DUAL_KEYBOARD indicates that:
 *      a) the "altXXXX" pointers point to arrays of full scancode->ascii
 *         mappings
 *      b) HOTSWITCH_MODE in shifty will toggle between the "XXXX" and
 *         "altXXXX" arrays for scancode decoding
 *
 */
#define DUAL_KEYBOARD   0x0001

struct keytbl {
  /*
   * pointers to arrays with 128 entries each.
   * entry[n] contains the ascii code for scancode n
   */
  const UBYTE *norm;
  const UBYTE *shft;
  const UBYTE *caps;
  /*
   * the following arrays are of two types, depending on the setting
   * of the DUAL_KEYBOARD bit in 'features':
   * 1) DUAL_KEYBOARD is not set (backwards compatibility)
   *    arrays consist of (scan code, char code) pairs, terminated by
   *    a zero byte.  see the French keyboard table as an example.
   * 2) DUAL_KEYBOARD is set (new, for Greek/Russian language support)
   *    arrays contain 128 entries, just like the arrays above
   */
  const UBYTE *altnorm;
  const UBYTE *altshft;
  const UBYTE *altcaps;
  /*
   * pointer to a variable-length array of 1-7 entries, for dead key support.
   * each entry is a pointer to a dead key translation table, consisting
   * of (scan code, char code) pairs, terminated by a zero byte.  see
   * the French keyboard table for a good example.
   */
  const UBYTE * const *dead;
  /* features supported for this keyboard (see above for #defines) */
  WORD features;
};

/* Baud rate used by Atari keyboards */
#define IKBD_BAUD 7812

/*
 * Date/Time to use when the hardware clock is not set.
 * We use the OS creation date at 00:00:00
 */
#define DEFAULT_DATETIME MAKE_ULONG(os_header.os_dosdate, 0)

/* initialise the ikbd */
void kbd_init(void);

/* called by ikbdvec to handle key events */
void kbd_int(UBYTE scancode);

/* called by timer C int to handle key repeat */
void kb_timerc_int(void);

/* some bios functions */
LONG bconstat2(void);
LONG bconin2(void);
LONG bcostat4(void);
LONG bconout4(WORD dev, WORD c);
LONG kbshift(WORD flag);

/* some xbios functions */
LONG keytbl(const UBYTE* norm, const UBYTE* shft, const UBYTE* caps);
WORD kbrate(WORD initial, WORD repeat);
void bioskeys(void);

void ikbdws(WORD cnt, const UBYTE *ptr);
void ikbd_writeb(UBYTE b);
void ikbd_writew(WORD w);

#if CONF_SERIAL_CONSOLE
void push_ascii_ikbdiorec(UBYTE ascii);
#endif

/* the following is in aciavecs.S */
void call_mousevec(SBYTE *packet);
#ifdef MACHINE_AMIGA
void call_joyvec(UBYTE *packet);
#endif
#if CONF_WITH_FLEXCAN || CONF_SERIAL_IKBD || defined(MACHINE_LISA)
void call_ikbdraw(UBYTE b);
#endif

#endif /* IKBD_H */
