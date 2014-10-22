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


.globl _r_get
.globl _r_set
.globl _scasb


/*
 * void r_get(WORD *pxywh, WORD *x, WORD *y, WORD *w, WORD *h)
 * extract components from the rectangle
 */

_r_get:
        lea     4(sp),a0
        move.l  a2,d0
        move.l  (a0)+,a1
        move.l  (a0)+,a2
        move.w  (a1)+,(a2)
        move.l  (a0)+,a2
        move.w  (a1)+,(a2)
        move.l  (a0)+,a2
        move.w  (a1)+,(a2)
        move.l  (a0),a2
        move.w  (a1),(a2)
        move.l  d0,a2
        rts


/*
 * void r_set(WORD *pxywh, WORD x, WORD y, WORD w, WORD h)
 * fills the rectangle with passed parameters
 */

_r_set:
        lea     4(sp),a0
        move.l  (a0)+,a1
        move.l  (a0)+,(a1)+
        move.l  (a0),(a1)
        rts


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
