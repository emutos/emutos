/*      LARGE.S */

/*
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
;                                                                       
;                  Historical Copyright                                 
*/
/*
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/


.globl _LSTCPY
.globl _LBWMOV


*       copy from src to dest until and including a null in the 
*       source string

*       len = LSTCPY(pdst, psrc)

_LSTCPY:
LD6FA:
            movea.l 0x08(SP),A0
            movea.l 0x04(SP),A1
            clr.l   D0
LD704:
            addq.b  #1,D0
            move.b  (A0)+,(A1)+
            bne     LD704
            subq.w  #1,D0
            rts



*       expand from bytes to words until you hit a null also return
*       the number of characters moved
*
*       LBWMOV(pdstoff, pdstseg, psrcoff, psrcseg)
*

_LBWMOV:
LD70E:
            movea.l 0x08(SP),A0
            movea.l 0x04(SP),A1
            clr.l   D0
            clr.l   D1
LD71A:
            move.b  (A0)+,D1
            beq     LD724
            move.w  D1,(A1)+
            addq.b  #1,D0
            bra     LD71A
LD724:
            rts
