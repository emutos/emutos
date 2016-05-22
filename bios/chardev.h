/*
 * chardev.h - bios devices
 *
 * Copyright (C) 2001-2013 The EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _CHARDEV_H
#define _CHARDEV_H

#include        "portab.h"


/* Prototypes */
LONG bconstat1(void);
LONG bconstat2(void);
LONG bconstat3(void);

LONG bconin0(void);
LONG bconin1(void);
LONG bconin2(void);
LONG bconin3(void);

LONG bconout0(WORD, WORD);
LONG bconout1(WORD, WORD);
LONG bconout2(WORD, WORD);
LONG bconout3(WORD, WORD);
LONG bconout4(WORD, WORD);
LONG bconout5(WORD, WORD);

LONG bcostat0(void);
LONG bcostat1(void);
LONG bcostat2(void);
LONG bcostat3(void);
LONG bcostat4(void);

LONG char_dummy(void);
LONG charout_dummy(WORD dev, WORD x);


/* internal init routine */

extern void chardev_init(void);

#endif /* _CHARDEV_H */
