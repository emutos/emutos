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

void r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph);
BYTE *scasb(BYTE *p, BYTE b);

/* note that the following routine is currently implemented in large.S */
WORD LBWMOV(WORD *pdst, BYTE *psrc);
#endif
