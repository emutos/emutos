/*
 * gemaplib.h - header for EmuTOS AES Application Services Library functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMAPLIB_H
#define GEMAPLIB_H

/*
 * structure of the entries read/written by appl_tplay()/appl_trecord()
 * this is a TOS API
 */
typedef struct
{
    LONG ap_event;      /* see below */
    LONG ap_value;
} EVNTREC;

/* valid values for 'ap_event' */
#define TCHNG   0       /* timer: ap_value = time (msec) */
#define BCHNG   1       /* button: ap_value: high word = state, low word = #clicks */
#define MCHNG   2       /* mouse movement: ap_value: high word = x, low word = y */
#define KCHNG   3       /* keyboard: ap_value: bits 0-15 = shift key state, */
                        /*  bits 16-23 = ASCII code, bits 24-31 = scan code */

extern BOOL     gl_play;
extern BOOL     gl_recd;
extern WORD     gl_rlen;
extern FPD      *gl_rbuf;

WORD ap_init(void);
WORD ap_rdwr(WORD code, AESPD *p, WORD length, WORD *pbuff);
WORD ap_find(char *pname);
void ap_tplay(const EVNTREC *pbuff, WORD length, WORD scale);
WORD ap_trecd(EVNTREC *pbuff, WORD length);
void ap_exit(void);

#endif
