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


/* initialise the ikbd */
extern VOID kbd_init(VOID);

/* called by ikbdvec to handle key events */
extern VOID kbd_int(WORD scancode);

/* some bios functions */
extern LONG bconstat2(VOID);
extern LONG bconin2(VOID);
extern LONG bcostat4(VOID);
extern VOID bconout4(WORD dev, WORD c);

/* some xbios functions */
extern LONG keytbl(LONG norm, LONG shft, LONG caps);
extern VOID bioskeys(VOID);
extern VOID ikbdws(WORD cnt, LONG ptr);

