/*      GSX2.S  */

/*
       Copyright 1999, Caldera Thin Clients, Inc.                      
       This software is licenced under the GNU Public License.         
       Please see LICENSE.TXT for further information.                 
                                                                       
                  Historical Copyright                                 
*/
/*
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/



.globl _gsx2
.globl _mul_div
.globl _i_ptsin
.globl _i_intin
.globl _i_ptsout
.globl _i_intout
.globl _i_ptr
.globl _i_ptr2
.globl _i_lptr1
.globl _m_lptr2

_gsx2:
LD628:
            move.l  #_contrl,LEE5C
            move.l  #LEE5C,D1
            moveq   #0x73,D0
            trap    #2
            rts

/*
***************************************************************************
;
; MUL_DIV (m1,m2,d1)
;
;        ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2   
;       m1 = signed 16 bit integer
;       m2 = unsigned 15 bit integer
;       d1 = signed 16 bit integer
;
***************************************************************************
*/
_mul_div:
LD63E:
            link    A6,#0x00
            move.w  0x0A(A6),D0
            asl.w   #1,D0
            muls    0x08(A6),D0
            divs    0x0C(A6),D0
            bmi     LD656
            addq.w  #1,D0
            bra     LD658
LD656:
            subq.w  #1,D0
LD658:
            asr.w   #1,D0
            unlk    A6
            rts


_i_ptsin:
LD678:
            link    A6,#0x00
            move.l  0x08(A6),LEE64
            unlk    A6
            rts
_i_intin:
LD688:
            link    A6,#0x00
            move.l  0x08(A6),LEE60
            unlk    A6
            rts
_i_ptsout:
LD698:
            link    A6,#0x00
            move.l  0x08(A6),LEE6C
            unlk    A6
            rts
_i_intout:
LD6A8:
            link    A6,#0x00
            move.l  0x08(A6),LEE68
            unlk    A6
            rts

_i_ptr:
        move.l  4(sp),_contrl+14
        rts

_i_ptr2:
        move.l  4(sp),_contrl+18
        rts

_i_lptr1:
LD6D8:
            link    A6,#0x00
            move.l  0x08(A6),_contrl+14  /*L1332A*/
            unlk    A6
            rts

_m_lptr2:
LD6E8:
            link    A6,#0x00
            movea.l 0x08(A6),A0
            move.l  _contrl+18,(A0)  /*L1332E*/
            unlk    A6
            rts


.bss

LEE5C:
            DS.B 4
LEE60:
            DS.B 4
LEE64:
            DS.B 4
LEE68:
            DS.B 4
LEE6C:
            DS.B 4
