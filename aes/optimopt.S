/*
 * optimopt.S - misc. optimised assembler routines
 *
 * Copyright (c) 2002 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
 * this is a rewrite of old code previouly covered by this mention:
 * I suggest to remove the old code and this mention soon.
 *
 *       Copyright 1999, Caldera Thin Clients, Inc.                      
 *       This software is licenced under the GNU Public License.         
 *       Please see LICENSE.TXT for further information.                 
 */

.globl _r_get
.globl _r_set
.globl _scasb


#if 0
/*
 *       VOID
 * r_get(pxywh, px, py, pw, ph)
 *       WORD            *pxywh;
 *       WORD            *px, *py, *pw, *ph;
 */
old_r_get:
            link    A6,#0
            movem.l D7/A5,-(SP)
            movea.l 0x08(A6),A5
            movea.l 0x0C(A6),A0
            move.w  (A5),(A0)
            movea.l 0x10(A6),A0
            move.w  0x02(A5),(A0)
            movea.l 0x14(A6),A0
            move.w  0x04(A5),(A0)
            movea.l 0x18(A6),A0
            move.w  0x06(A5),(A0)
            tst.l   (SP)+
            movem.l (SP)+,A5
            unlk    A6
            rts
#endif
        
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
        

#if 0
/*
 *       VOID
 * r_set(pxywh, x, y, w, h)
 *       WORD            *pxywh;
 *       WORD            x, y, w, h;
 */

old_r_set:
            link    A6,#0x00
            movem.l D7/A5,-(SP)
            movea.l 0x08(A6),A5
            move.w  0x0C(A6),(A5)
            move.w  0x0E(A6),0x02(A5)
            move.w  0x10(A6),0x04(A5)
            move.w  0x12(A6),0x06(A5)
            tst.l   (SP)+
            movem.l (SP)+,A5
            unlk    A6
            rts
#endif
        
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
        

#if 0

old_scasb:
LDB94:
            link    A6,#0x00
            movem.l D6-D7/A5,-(SP)
            movea.l 0x08(A6),A5
            move.b  0x0D(A6),D7
            bra     LDBA8
LDBA6:
            addq.l  #1,A5
LDBA8:
            tst.b   (A5)
            beq     LDBB0
            cmp.b   (A5),D7
            bne     LDBA6
LDBB0:
            move.l  A5,D0
            tst.l   (SP)+
            movem.l (SP)+,D7/A5
            unlk    A6
            rts

#endif
        
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
        move.l  a0,d0
        subq.l  #1,d0
        rts

