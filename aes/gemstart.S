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
.globl _deskpexec


.xdef _ad_stail
        .text

//_main:
_ui_start:
        movea.l sp,a5
        movea.l #gemusp,sp
        movea.l 4(a5),a5

        move.l  a5,d0
        add.l   #0x80,d0
        move.l  d0,_ad_stail

        move.l  0xC(a5),d0
        add.l   0x14(a5),d0
        add.l   0x1C(a5),d0
        add.l   #0x100,d0
        move.l  d0,-(sp)
        move.l  a5,-(sp)
        clr.w   -(sp)
        move.w  #0x4A,-(sp)
        trap    #1
        lea     12(sp),sp

        clr.l  -(sp)
        move.w  #0x20,-(sp)
        trap    #1                      // Switch to supervisor mode
        move.l  d0,old_gem_ssp

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
        //movea.l a6,sp
        movea.l #gemssp,sp        // Set up new SSP
        jsr     _gem_main

        move.l  old_gem_ssp,-(sp)
        move.w  #0x20,-(sp)
        trap    #1
        addq.w  #6,sp

        move.l  #0,-(sp)
        trap    #1




_gemdos:
        move.l  (SP)+,saveretaddr

        movem.l d2/a2,saveregs

        move.w  #0,_DOS_ERR
        move.w  #0,_DOS_AX
        trap    #1
        cmp.l   #0,D0
        bge     L134
        move.w  #1,_DOS_ERR
        move.w  D0,_DOS_AX
        cmp.w   #0xFFE0,D0
        bgt     L134
        not.w   _DOS_AX
        subi.w  #0x1E,_DOS_AX
L134:

        movem.l saveregs,d2/a2

        move.l  saveretaddr,-(SP)
        rts


/*
   This function starts the ROM desktop, parameter is its basepage that
   must already have been created.
*/
_deskpexec:
        movem.l d2/a2,-(sp)
        clr.l   -(sp)
        move.l  16(sp),-(sp)    // Push parameter on the stack again for pexec
        clr.l   -(sp)
        move.w  #4,-(sp)
        move.w  #0x4b,-(sp)
        trap    #1              // Pexec
        lea     16(sp),sp
        movem.l (sp)+,d2/a2
        rts


.bss


old_gem_ssp:
        .ds.l   1



saveretaddr:
        .ds.l   1


// Space for the Stacks:

        .ds.b   256
gemusp:

        .ds.b   4096
gemssp:


saveregs:
        .ds.l   2

