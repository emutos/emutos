/*
 * stringasm.S - simple implementation of <string.h>-like routines
 *
 * Copyright (C) 2002 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

.global _strlencpy
.global _strchr

/*
 * WORD strlencpy(char *dest, const char *src)
 * as strcpy but returns the length ot the string instead.
 */

_strlencpy:                
        movea.l 8(sp),a0
        movea.l 4(sp),a1
        clr.l   d0
lstcpy_loop:
        addq.w  #1,d0
        move.b  (a0)+,(a1)+
        bne     lstcpy_loop
        subq.w  #1,d0
        rts

/*
 * char *strchr(const char *s, int c)
 *
 * returns a pointer to the first occurrence of char c in string s, or NULL.
 */

_strchr:
        move.l  4(sp),a0
        move.w  8(sp),d1
strchr_loop:
        move.b  (a0)+,d0
        beq     strchr_null
        cmp.b   d1,d0
        bne     strchr_loop
        move.l  a0,d0
        subq.l  #1,d0
        rts
strchr_null:
        move.l  #0,d0
        rts
        
