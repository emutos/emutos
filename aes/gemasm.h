/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMASM_H
#define GEMASM_H

/* arranges for codevalue to be pushed to the stack for process p,
 * in a 68000-only RTE stack frame, "in preparation for an RTE that
 * will start this process executing".
 */
extern void psetup(PD *p, void *codevalue);

/* launches the top of rlr list, as if called from within function
 * back(PD *top_of_rlr) 
 */
extern void gotopgm(void) /*NORETURN*/ ;

/* called repeatedly to give hand to another process - actually a
 * wrapper around disp() in gemdisp.c 
 */
extern void dsptch(void);

/* called by disp() to end a dsptch ... switchto sequence */
extern void switchto(UDA *puda) NORETURN ;

#endif
