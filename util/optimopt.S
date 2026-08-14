/*
 * optimopt.S - misc. optimised assembler routines
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

        .globl  _scasb
        .globl  _expand_string

/*
 * const char *scasb(const char *str, char c)
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
        jeq     scasb_end
        cmp.b   d1,d0
        jne     scasb_loop
scasb_end:
        subq.l  #1,a0
        move.l  a0,d0
        rts

/*
 *  WORD expand_string(WORD *dest,const char *src,UWORD dest_words)
 *
 *  expand an unsigned byte string to an array of words, returning the
 *  length of the expanded string.  at most dest_words words are stored
 *  in dest, including the terminating zero word, so an over-long source
 *  string is truncated to dest_words-1 characters instead of running off
 *  the end of the destination array.  note that this code is optimised
 *  for the 68000: in order to obtain the fastest code, we copy the null
 *  byte too, but it is not included in the length value returned.
 */

_expand_string:
        movea.l 8(sp),a0        // a0 -> source
        movea.l 4(sp),a1        // a1 -> dest
        moveq   #0,d0
        move.w  12(sp),d0       // d0 = size of dest in words
        subq.l  #1,d0           // ... as a loop counter
        jmi     expand_none     // no room at all
        clr.w   d1              // work reg
copy_loop:
        move.b  (a0)+,d1        // 68000: 8 cycles
        move.w  d1,(a1)+        // 8 cycles
#ifdef __mcoldfire__
        jeq     copy_done
        subq.l  #1,d0
        jpl     copy_loop
#else
        dbeq    d0,copy_loop    // 10 cycles when taken
        jeq     copy_done
#endif
        moveq   #0,d1           // truncated: overwrite the last character
        move.w  d1,-2(a1)       //  with the terminator
copy_done:
        move.l  a1,d0
        sub.l   4(sp),d0        // bytes stored in dest
        lsr.l   #1,d0
        subq.l  #1,d0           // d0.l = length, excluding terminator
        rts
expand_none:
        moveq   #0,d0
        rts
