/*
 * geminit.h - header for EmuTOS AES initialisation functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMINIT_H
#define GEMINIT_H

#define DEF_DESKTOP "EMUDESK"   /* default desktop */

extern char     *ad_envrn;

extern MFORM    *mouse_cursor[];

extern MFORM    gl_mouse;
#if CONF_WITH_GRAF_MOUSE_EXTENSION
extern MFORM    gl_prevmouse;
#endif

extern WORD     totpds;
extern WORD     num_accs;

extern THEGLO   D;


void all_run(void);
void set_mouse_to_arrow(void);
void set_mouse_to_hourglass(void);
void wait_for_accs(WORD bitmask);

#endif
