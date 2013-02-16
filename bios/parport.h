/*
 * parport.h - limited parallel port support
 *
 * Copyright (c) 2002 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/licence.txt for details.
 */

#include "portab.h"

LONG bconstat0(void);
LONG bconin0(void);
LONG bcostat0(void);
LONG bconout0(WORD dev, WORD c);

void parport_init(void);
