/*
 * gsx2.S - assembler part of AES's VDI interface
 *
 * Copyright 2002, The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



.globl _gsx2
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
 * Fill routines for the normal Array pointers 
 */
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



/*
 * Storage for the normal Array pointers of AES
 */
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
