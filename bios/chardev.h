/*
 * chardev.h - bios devices
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef H_BIOSDEVS_
#define H_BIOSDEVS_

#include        "portab.h"


/* Prototypes */
LONG bconstat0(void);
LONG bconstat1(void);
LONG bconstat2(void);
LONG bconstat3(void);
LONG bconstat4(void);
LONG bconstat5(void);
LONG bconstat6(void);
LONG bconstat7(void);

LONG bconin0(void);
LONG bconin1(void);
LONG bconin2(void);
LONG bconin3(void);
LONG bconin4(void);
LONG bconin5(void);
LONG bconin6(void);
LONG bconin7(void);

void bconout0(WORD, WORD);
void bconout1(WORD, WORD);
void bconout2(WORD, WORD);
void bconout3(WORD, WORD);
void bconout4(WORD, WORD);
void bconout5(WORD, WORD);
void bconout6(WORD, WORD);
void bconout7(WORD, WORD);

LONG bcostat0(void);
LONG bcostat1(void);
LONG bcostat2(void);
LONG bcostat3(void);
LONG bcostat4(void);
LONG bcostat5(void);
LONG bcostat6(void);
LONG bcostat7(void);

void dummy(void);

/* defined in tosvars.s */

extern LONG (*bconstat_vec[])(void);
extern LONG (*bconin_vec[])(void);
extern void (*bconout_vec[])(WORD, WORD);
extern LONG (*bcostat_vec[])(void);

/* internal init routine */

extern void chardev_init(void);
extern void cputc(WORD);        /* found in conout.s */
 
#endif /* H_BIOSDEVS_ */

