/*
 * optimopt.S - misc. optimised assembler routines
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"


.globl _scasb


/*
 * char *scasb(char *str, char c)
 * returns the pointer to the first occurrence of char c in string str,
 * or the pointer to the ending '\0' of this string.
 * (see also strchr, which returns NULL instead when the char is not found)
 *
 */

_scasb:
        move.l  4(sp),a0
        move.w  8(sp),d1
scasb_loop:
        move.b  (a0)+,d0
        beq     scasb_end
        cmp.b   d1,d0
        bne     scasb_loop
scasb_end:
        subq.l  #1,a0
        move.l  a0,d0
        rts
