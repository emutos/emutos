/*      LARGE.S */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/


.globl _LBWMOV

/*
 *       WORD LBWMOV(WORD *pdst, const UBYTE *psrc)
 *
 *       expand from unsigned bytes to words until you hit a null. 
 *       return the number of characters moved
 */

_LBWMOV:
            movea.l 0x08(SP),A0
            movea.l 0x04(SP),A1
            clr.l   D0
            clr.w   D1
lbwmov_loop:
            move.b  (A0)+,D1
            beq     lbwmov_exit
            move.w  D1,(A1)+
            addq.w  #1,D0
            bra     lbwmov_loop
lbwmov_exit:
            rts
