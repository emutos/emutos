/*
 * proc.h - processes defines
 *
 * Copyright (c) 2001 EmuTOS Development Team.
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _PROC_H
#define _PROC_H


/*
 *  process management
 */

extern	long	bakbuf[] ;
extern	WORD	supstk[] ;
extern	PD	*run;

/*
 * in proc.c
 */

long xexec(WORD, char *, char *, char *);
void x0term(void);
void xterm(UWORD rc)  NORETURN ;
WORD xtermres(long blkln, WORD rc);

/*
 * in kpgmld.h
 */

ERROR xpgmld(char *s , PD *p);

/*
 * in rwa.S
 */

void gouser(void)  NORETURN;  

#endif /* _PROC_H */


