/*
 * setjmp.h - EmuTOS's own version of the ANSI standard header
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef SETJMP_H
#define SETJMP_H

typedef long jmp_buf[13];

int setjmp(jmp_buf state);
void longjmp(jmp_buf state, WORD value) NORETURN;

#endif /* SETJMP_H */
