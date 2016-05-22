/*
 * EmuTOS aes
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMASM_H
#define GEMASM_H

/* arranges for codevalue to be pushed to the stack for process p,
 * in a standard RTS stack frame, "in preparation for an RTS that
 * will start this process executing".
 */
extern void psetup(AESPD *p, PFVOID codevalue);

/* launches the top of rlr list, as if called from within function
 * back(AESPD *top_of_rlr)
 */
extern void gotopgm(void) /*NORETURN*/ ;

/* called repeatedly to give hand to another process - actually a
 * wrapper around disp() in gemdisp.c
 */
extern void dsptch(void);

/* called by disp() to end a dsptch ... switchto sequence */
extern void switchto(UDA *puda) NORETURN ;

#endif
