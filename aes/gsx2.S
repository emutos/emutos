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

        .text
        
_gsx2:
            move.l  #_contrl,tmp_contrl
            move.l  #tmp_contrl,d1
            moveq   #0x73,d0
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
        move.w  6(sp),d0
        asl.w   #1,d0
        muls    4(sp),d0
        divs    8(sp),d0
        bmi     mul_div_neg
        addq.w  #1,d0
        bra     mul_div_end
mul_div_neg:
        subq.w  #1,d0
mul_div_end:    
        asr.w   #1,d0
        rts
        

_i_intin:
        move.l  4(sp),tmp_intin
        rts
_i_ptsin:
        move.l  4(sp),tmp_ptsin
        rts
_i_intout:
        move.l  4(sp),tmp_intout
        rts
_i_ptsout:
        move.l  4(sp),tmp_ptsout
        rts


_i_ptr:
_i_lptr1:
        move.l  4(sp),_contrl+14
        rts

_i_ptr2:
        move.l  4(sp),_contrl+18
        rts

_m_lptr2:
        move.l  4(sp),a0
        move.l  _contrl+18,(a0)
        rts

.bss

tmp_contrl:
        ds.l    1
tmp_intin:
        ds.l    1
tmp_ptsin:
        ds.l    1
tmp_intout:
        ds.l    1
tmp_ptsout:
        ds.l    1
