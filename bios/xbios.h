/*
 * xbios.c - misc XBIOS function prototypes
 *
 * Copyright (c) 2002 by the EmuTOS Development Team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef XBIOS_H
#define XBIOS_H
 
/* initializes the xbios */
void xbiosinit(void);

/* misc XBIOS functions */

LONG iorec(WORD devno);
LONG random(void);
LONG kbdvbase(void);
LONG supexec(LONG codeptr);
LONG bconmap(WORD devno);

#endif /* XBIOS_H */
