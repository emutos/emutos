/*
 * proc.h - processes defines
 *
 * Copyright (C) 2001-2015 The EmuTOS development team.
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef PROC_H
#define PROC_H

#include "pghdr.h"

/*
 *  process management
 */

/*
 * in proc.c
 */

long xexec(WORD, char *, char *, char *);
void x0term(void);
void xterm(UWORD rc)  NORETURN ;
WORD xtermres(long blkln, WORD rc);

/*
 * in kpgmld.c
 */

LONG kpgmhdrld(char *s, PGMHDR01 *hd, FH *h);
LONG kpgmld(PD *p, FH h, PGMHDR01 *hd);
LONG kpgm_relocate( PD *p, long length); /* SOP */

/*
 * in rwa.S
 */

void gouser(void)  NORETURN;
void termuser(void)  NORETURN;

#endif /* PROC_H */
