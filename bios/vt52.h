/*
 * vt52.h - vt52 like screen handling routine headers
 *
 *
 * Copyright (c) 2004 by Authors:
 *
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef VT52_H
#define VT52_H

#include "portab.h"

BOOL vt52_initialized;          /* checked by kprintf for safety */

void vt52_init();               /* initialize the vt52 console */
WORD cursconf(WORD, WORD);      /* XBIOS cursor configuration */

#endif /* VT52_H */
