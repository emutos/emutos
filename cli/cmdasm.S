/*
 * EmuCON2 assembler routines
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifdef STANDALONE_CONSOLE
#define _coma_start _start
#else
#include "asmdefs.h"
#endif

        .globl  _coma_start
        .globl  _getwh,_getht
        .globl  _jmp_gemdos,_jmp_bios,_jmp_xbios
        .extern _cmdmain

        .text
_coma_start:
        move.l  4(sp),a5                // a5 -> basepage
        lea     stack,sp
        move.l  12(a5),d0               // calc memory required
        add.l   20(a5),d0
        add.l   28(a5),d0
        addi.l  #256,d0
        move.l  d0,-(sp)                // Mshrink()
        move.l  a5,-(sp)
        move.w  #0,-(sp)
        move.w  #74,-(sp)
        trap    #1
        lea     12(sp),sp

        jsr     _cmdmain

        clr.w   -(sp)                   // Pterm0()
        trap    #1

_getwh:
        move.l  a2,-(sp)                // not a scratch reg for gcc
#ifdef __mcoldfire__
        .dc.w   0xa920                  // lineA init
#else
        .dc.w   0xa000                  // lineA init
#endif
        move.l  -44(a0),d0              // v_cel_mx/v_cel_my
        movea.l (sp)+,a2
        rts

_getht:
        move.l  a2,-(sp)                // not a scratch reg for gcc
#ifdef __mcoldfire__
        .dc.w   0xa920                  // lineA init
#else
        .dc.w   0xa000                  // lineA init
#endif
        moveq   #0,d0
        move.w  -46(a0),d0              // v_cel_ht
        movea.l (sp)+,a2
        rts

/*
 * system call interface
 */
_jmp_gemdos:
        move.l  (sp)+,ret_addr          // so parameters are in the right place
        move.l  a2,save_a2              // must save a2/d2 because someone may
        move.l  d2,save_d2              //  intercept the trap & clobber them
        trap    #1
        move.l  save_a2,a2
        move.l  save_d2,d2
        move.l  ret_addr,-(sp)
        rts

_jmp_bios:
        move.l  (sp)+,ret_addr
        move.l  a2,save_a2
        move.l  d2,save_d2
        trap    #13
        move.l  save_a2,a2
        move.l  save_d2,d2
        move.l  ret_addr,-(sp)
        rts

_jmp_xbios:
        move.l  (sp)+,ret_addr
        move.l  a2,save_a2
        move.l  d2,save_d2
        trap    #14
        move.l  save_a2,a2
        move.l  save_d2,d2
        move.l  ret_addr,-(sp)
        rts


        .bss
ret_addr:
        .ds.l   1
save_a2:
        .ds.l   1
save_d2:
        .ds.l   1

        .ds.l   1024
stack:
