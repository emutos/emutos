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
.globl _dos_exec
.globl _ui_mupb


.xdef _ad_stail
        .text

//_main:
_ui_start:
        // obtain our basepage address
        movea.l sp,a5
        movea.l 4(a5),a5

        // set our temporary user stack (needed to call Super ?)
        movea.l #gemusp,sp

        // ad_stail is the address to the cmdline buffer in our basepage
        move.l  a5,d0
        add.l   #0x80,d0
        move.l  d0,_ad_stail

        // mshrink to the needed size for TEXT+DATA+BSS plus 256 bytes.
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

        // old_gem_ssp = super(0)
        clr.l  -(sp)
        move.w  #0x20,-(sp)
        trap    #1                      // Switch to supervisor mode
        move.l  d0,old_gem_ssp

        // ad_envrn = the pointer to the environ string
        move.l  0x2C(a5),_ad_envrn

        // clear the 'global memory' zone whose size is determined by
        // routine size_theglo :-)
        jsr     _size_theglo
        movea.l #_D,a0
        clr.l   d1
        bra     clrglotest
clrgloloop:
        move.w  d1,(a0)+
clrglotest:
        dbmi    d0,clrgloloop

        // _drwaddr ??? set to just_rts
        lea     _justretf,a0
        move.l  a0,_drwaddr

        // _tikaddr ??? set to tikcod
        lea     _tikcod,a0
        move.l  a0,_tikaddr

        // Initiallize the SSP to be _D + 0x74A, and also store
        // this in _D + 0x3E. (_D + 0x3E is ssp in D.uda[0])
        movea.l #_D,a6
        movea.l a6,a5
        adda.l  #0x74A,a6
        move.l  a6,0x3E(a5)
        movea.l a6,sp

        // go in C
        jsr     _gem_main

        // Super() to go back to user more
        move.l  old_gem_ssp,-(sp)
        move.w  #0x20,-(sp)
        trap    #1
        addq.w  #6,sp

        // Pterm0()
        move.l  #0,-(sp)
        trap    #1



// Execute a GEMDOS call (see gemdos.c).
_gemdos:
        move.l  (SP)+,saveretaddr
        movem.l d2/a2,saveregs

        move.w  #0,_DOS_ERR
        move.w  #0,_DOS_AX
        trap    #1
        cmp.l   #0,D0
        bge.s   gd_end
        move.w  #1,_DOS_ERR
        move.w  D0,_DOS_AX
        cmp.w   #0xFFE0,D0
        bgt.s   gd_end
        not.w   _DOS_AX
        subi.w  #0x1E,_DOS_AX
gd_end:
        movem.l saveregs,d2/a2
        move.l  saveretaddr,-(SP)
        rts


// A wrapper around Pexec. We can't use the above function _gemdos for this
// since _gemdos is not reentrant.
_dos_exec:
        jsr     _dsptch                 // Updates rlr->p_uda!
        move.l  sp,d1
        movem.l d2/a2,-(sp)             // Save registers

        bsr     _cli
        movea.l _rlr,a0
        movea.l 8(a0),a0
        move.l  0x3E(a0),-(sp)          // Save rlr->p_uda->u_spsuper
        movea.l sp,a1
        suba.l  #0x40,a1
        move.l  a1,0x3E(a0)
        bsr     _sti

        movea.l d1,a1
        move.l  14(a1),-(sp)            // Push parameters on the stack again...
        move.l  10(a1),-(sp)
        move.l  6(a1),-(sp)
        move.w  4(a1),-(sp)
        move.w  #0x4B,-(sp)
        trap    #1                      // ... and execute the Pexec call!
        lea     16(sp),sp

        bsr     _cli
        movea.l _rlr,a0
        movea.l 8(a0),a0
        move.l  (sp)+,0x3E(a0)          // Restore rlr->p_uda->u_spsuper
        bsr     _sti

        movem.l (sp)+,d2/a2
        move.w  #1,_DOS_ERR
        tst.l   d0
        bmi.s   de_return
        move.w  #0,_DOS_ERR
de_return:
        rts




.data

// The AES MUPB structure:
_ui_mupb:
        .dc.l   0x87654321      // Magic value
        .dc.l   ui_bss_end      // End of AES' BSS (actually a dummy value)
        .dc.l   _ui_start       // Start of AES code


.bss


old_gem_ssp:
        .ds.l   1



saveretaddr:
        .ds.l   1


// Space for the user stack:
        .ds.b   256
gemusp:


saveregs:
        .ds.l   2

ui_bss_end:

