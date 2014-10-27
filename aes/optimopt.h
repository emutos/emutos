/*
 * optimopt.h - misc functions
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#ifndef OPTIMOPT_H
#define OPTIMOPT_H

BYTE *scasb(BYTE *p, BYTE b);
WORD expand_string(WORD *dest, BYTE *src);

/* note that the following routine is currently implemented in large.S */
WORD LBWMOV(WORD *pdst, BYTE *psrc);
#endif
