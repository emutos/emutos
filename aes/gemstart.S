/*      GEMSTART.S */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/


.globl _ui_start
.globl _pgmld
.globl _gemdos

.xdef _ad_stail
        .text

//_main:
_ui_start:
        movea.l a7,a5
        movea.l #LEABC,a7
        movea.l 4(a5),a5

        move.l  a5,d0
        add.l   #0x80,d0
        move.l  d0,_ad_stail

        move.l  0xC(a5),d0
        add.l   0x14(a5),d0
        add.l   0x1C(a5),d0
        add.l   #0x100,d0
        move.l  d0,-(a7)
        move.l  a5,-(a7)
        move.w  d0,-(a7)
        move.w  #0x4A,-(a7)
        trap    #1
        adda.l  #0xC,a7

        move.l  a7,-(a7)
        move.w  #0x20,-(a7)
        trap    #1
        addq.w  #6,a7
        move.l  0x2C(a5),_ad_envrn

        jsr     _size_theglo
        movea.l #_D,a0
        clr.l   d1
        bra     LB4
LB2:
        move.w  d1,(a0)+
LB4:
        dbmi    d0,LB2
        lea     _justretf,a0
        move.l  a0,_drwaddr
        lea     _tikcod,a0
        move.l  a0,_tikaddr
        movea.l #_D,a6
        movea.l a6,a5
        adda.l  #0x74A,a6
        move.l  a6,0x3E(a5)
        movea.l a6,a7
        jsr     _gem_main
        move.l  #0,-(a7)
        trap    #1




_gemdos:
LF2:
            move.l  (SP)+,LEA2C

    movem.l d1-d7/a0-a6,saveregs
/*
    move.l  (SP),d0
    move.l  d0,-(SP)
    pea     errormsg
    jsr     _cprintf
    addq.l  #8,sp
*/
            move.w  #0,_DOS_ERR
            move.w  #0,_DOS_AX
            trap    #1
            cmp.l   #0,D0
            bge     L134
            move.w  #0x01,_DOS_ERR
            move.w  D0,_DOS_AX
            cmp.w   #0xFFE0,D0
            bgt     L134
            not.w   _DOS_AX
            subi.w  #0x1E,_DOS_AX
L134:

    movem.l saveregs,d1-d7/a0-a6

            move.l  LEA2C,-(SP)
            rts





.data

LE3E6:
            DC.B 0

errormsg:
        .ascii  "GEMDOS: Nr=0x%x Parm=0x%x\n\0"
        .even



.bss

LEA2C:
            DS.B 4

* Space for the Stack:
        .ds.b   0x80
LEABC:



saveregs:
        .ds.l   14

