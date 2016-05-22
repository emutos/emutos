/*
 *  initinfo.c - Info screen at startup
 *
 * Copyright (C) 2001-2015 by Authors:
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef INITINFO_H
#define INITINFO_H

/* set if the user asked for an early EmuCON in initinfo() */
extern int early_cli;

/*==== Prototypes =========================================================*/

WORD initinfo(void);

#endif /* INITINFO_H */
