/*
 * xbios.c - misc XBIOS function prototypes
 *
 * Copyright (c) 2002-2013 by the EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef XBIOS_H
#define XBIOS_H

/* misc XBIOS functions */
LONG iorec(WORD devno);
LONG random(void);
LONG kbdvbase(void);
LONG supexec(LONG codeptr);

LONG xbios_do_unimpl(WORD number);

#endif /* XBIOS_H */
