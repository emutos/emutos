/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMAPLIB_H
#define GEMAPLIB_H

extern WORD     gl_play;
extern WORD     gl_recd;
extern WORD     gl_rlen;
extern FPD      *gl_rbuf;

WORD ap_init(void);
void ap_rdwr(WORD code, AESPD *p, WORD length, LONG pbuff);
WORD ap_find(LONG pname);
void ap_tplay(FPD *pbuff, WORD length, WORD scale);
WORD ap_trecd(FPD *pbuff, WORD length);
void ap_exit(void);

#endif
