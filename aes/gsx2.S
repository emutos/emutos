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

#include "asmdefs.h"


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
#ifdef __mcoldfire__
        lea     tmp_contrl,a0
        move.l  #_contrl,(a0)
#else
        move.l  #_contrl,tmp_contrl
#endif
        move.l  #tmp_contrl,d1
        moveq   #0x73,d0
        trap    #2
        rts


/*
 * Fill routines for the normal Array pointers
 */
_i_intin:
#ifdef __mcoldfire__
        lea     tmp_intin,a0
        move.l  4(sp),(a0)
#else
        move.l  4(sp),tmp_intin
#endif
        rts
_i_ptsin:
#ifdef __mcoldfire__
        lea     tmp_ptsin,a0
        move.l  4(sp),(a0)
#else
        move.l  4(sp),tmp_ptsin
#endif
        rts
_i_intout:
#ifdef __mcoldfire__
        lea     tmp_intout,a0
        move.l  4(sp),(a0)
#else
        move.l  4(sp),tmp_intout
#endif
        rts
_i_ptsout:
#ifdef __mcoldfire__
        lea     tmp_ptsout,a0
        move.l  4(sp),(a0)
#else
        move.l  4(sp),tmp_ptsout
#endif
        rts


_i_ptr:
_i_lptr1:
#ifdef __mcoldfire__
        lea     _contrl+14,a0
        move.l  4(sp),(a0)
#else
        move.l  4(sp),_contrl+14
#endif
        rts

_i_ptr2:
#ifdef __mcoldfire__
        lea     _contrl+18,a0
        move.l  4(sp),(a0)
#else
        move.l  4(sp),_contrl+18
#endif
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
        .ds.l   1
tmp_intin:
        .ds.l   1
tmp_ptsin:
        .ds.l   1
tmp_intout:
        .ds.l   1
tmp_ptsout:
        .ds.l   1
