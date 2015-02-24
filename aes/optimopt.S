/*
 * optimopt.S - misc. optimised assembler routines
 *
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"


.globl _scasb
.globl _expand_string

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

/*
 *  WORD expand_string(WORD *dest,const UBYTE *src)
 *
 *  expand an unsigned byte string to an array of words, returning the
 *  length of the source string.  note that this code is optimised for
 *  the 68000: in order to obtain the fastest code, we copy the null
 *  byte too, but it is not included in the length value returned.
 */

_expand_string:
        movea.l 8(sp),a0        // a0 -> source
        movea.l 4(sp),a1        // a1 -> dest
        move.l  a0,d0           // remember source start
        clr.w   d1              // work reg
copy_loop:
        move.b  (a0)+,d1        // 68000: 8 cycles
        move.w  d1,(a1)+        // 8 cycles
        bne.b   copy_loop       // 10 cycles when taken
        sub.l   d0,a0
        move.l  a0,d0
        subq.l  #1,d0           // d0.l = length
        rts
