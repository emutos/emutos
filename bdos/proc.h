/*
 * proc.h - processes defines
 *
 * Copyright (C) 2001-2022 The EmuTOS development team.
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
#include "program_loader.h"
#include "sysconf.h"



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
 * program memory management
 */
UBYTE *alloc_tpa(ULONG flags, LONG needed, LONG *allocated_size
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
    , UBYTE *start_address
#endif
);


/*
 * in kpgmld.c
 */

LONG read_program_header(FH h, LOAD_STATE *lstate);
LONG load_program_into_memory(PD *p, FH h, LOAD_STATE *lstate);

#if DETECT_NATIVE_FEATURES
LONG kpgm_relocate( PD *p, long length); /* SOP */
#endif

/*
 * in rwa.S
 */

void gouser(void)  NORETURN;
void termuser(void)  NORETURN;

#endif /* PROC_H */
