/*      GEMSTART.S */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002, 2007 The EmuTOS development team
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


.extern _ad_stail
        .text

//////////////////////////////////////////////////////////////////////////////
// AES start-up code
//////////////////////////////////////////////////////////////////////////////
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
#ifdef __mcoldfire__
        lea     _ad_envrn,a0
        move.l  0x2C(a5),(a0)
#else
        move.l  0x2C(a5),_ad_envrn
#endif

aes_restart:
        // clear the 'global memory' zone whose size is determined by
        // routine size_theglo :-)
        jsr     _size_theglo
        movea.l #_D,a0
        clr.l   d1
        bra     clrglotest
clrgloloop:
        move.w  d1,(a0)+
clrglotest:
#ifdef __mcoldfire__
        tst.l   d0
        beq.s   clrglotestend
        subq.l  #1,d0
        bra.s   clrgloloop
clrglotestend:
#else
        dbmi    d0,clrgloloop
#endif

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

        // Check for resolution change - if so start over again
        move.w  _gl_changerez,d0
        tst.w   d0
        bne     aes_restart

        // Super() to go back to user more
        move.l  old_gem_ssp,-(sp)
        move.w  #0x20,-(sp)
        trap    #1
        addq.l  #6,sp

        // Pterm0()
        move.l  #0,-(sp)
        trap    #1



//////////////////////////////////////////////////////////////////////////////
// Execute a GEMDOS call (see gemdos.c).
//////////////////////////////////////////////////////////////////////////////
_gemdos:
        move.l  (SP)+,saveretaddr
#ifdef __mcoldfire__
        move.l  d2,saveregs
        move.l  a2,saveregs+4
#else
        movem.l d2/a2,saveregs
#endif

        clr.w   _DOS_ERR
        clr.w   _DOS_AX
        trap    #1
        cmp.l   #0,D0
        bge.s   gd_end
#ifdef __mcoldfire__
        moveq   #1,d1
        move.w  d1,_DOS_ERR
#else
        move.w  #1,_DOS_ERR
#endif
        move.w  D0,_DOS_AX
        cmp.w   #0xFFE0,D0
        bgt.s   gd_end
#ifdef __mcoldfire__
        move.w  d0,d1
        ext.l   d1
        not.l   d1
        subi.l  #0x1E,d1
        move.w  d1,_DOS_AX
#else
        not.w   _DOS_AX
        subi.w  #0x1E,_DOS_AX
#endif
gd_end:
#ifdef __mcoldfire__
        move.l  saveregs,d2
        move.l  saveregs+4,a2
#else
        movem.l saveregs,d2/a2
#endif
        move.l  saveretaddr,-(SP)
        rts


//////////////////////////////////////////////////////////////////////////////
// A wrapper around Pexec. We can't use the above function _gemdos for this
// since _gemdos is not reentrant.
//////////////////////////////////////////////////////////////////////////////
_dos_exec:
        move.l  sp,d1
#ifdef __mcoldfire__
        move.l  a2,-(sp)                // Save registers
        move.l  d2,-(sp)
#else
        movem.l d2/a2,-(sp)             // Save registers
#endif

        // *** WARNING ***
        // We are here on the private supervisor stack of the AES process 0.
        // We are going to call Pexec() from the supervisor mode.
        // The GEMDOS will backup some data on our stack, then it will
        // switch to its private supervisor stack. Then it will switch
        // to the child program (desktop or other) which will call the AES
        // through a trap #2.
        // The AES trap prologue always switch to rlr->p_uda->u_spsuper.
        // That variable currently points to somewhere on our stack.
        // Thus, if we don't take care, the stack pointer may go up in
        // the next trap #2 and cause data corruption.
        // In other words, there is a reentrency problem. The AES context
        // will not be switched when entering the child program, so this
        // AES process will reenter itself. The workaround is to set
        // rlr->p_uda->u_spsuper to a value low enough on the current stack
        // to avoid a stack smashing. We have to estimate the amount of
        // data that will be stored on the stack between here and the next
        // stack switch in trap #2.
        // From the rlr->p_uda->u_spsuper backup below to the next trap #2
        // stack switch, the following data will be pushed on the stack:
        //   - the Pexec() arguments (16 bytes)
        //   - the trap #1 stack frame (0 byte, popped by trap #1 prologue)
        //   - the backup of registers in the trap #1 prologue (50 bytes)
        // Currently, this sums to 66 bytes. The stack has to be adjusted
        // by this value, or more.
        // You must keep an eye on the trap #1 prologue for GEMDOS,
        // when entering in supervisor mode and calling a function
        // other than 0x20. It is located in bdos/rwa.S, label "systk".
        // If some additional data is pushed on the stack before
        // the stack switch, you must add the equivalent room here.

        jsr     _cli
        movea.l _rlr,a0
        movea.l 8(a0),a0
        move.l  0x3E(a0),-(sp)          // Save rlr->p_uda->u_spsuper
        movea.l sp,a1
        suba.l  #0x50,a1                // Adjust the stack (see above)
        move.l  a1,0x3E(a0)
        jsr     _sti

        movea.l d1,a1
        move.l  14(a1),-(sp)            // Push parameters on the stack again...
        move.l  10(a1),-(sp)
        move.l  6(a1),-(sp)
        move.w  4(a1),-(sp)
        move.w  #0x4B,-(sp)
        trap    #1                      // ... and execute the Pexec call!
        lea     16(sp),sp

        jsr     _cli
        movea.l _rlr,a0
        movea.l 8(a0),a0
        move.l  (sp)+,0x3E(a0)          // Restore rlr->p_uda->u_spsuper
        jsr     _sti

#ifdef __mcoldfire__
        move.l  (sp)+,d2
        move.l  (sp)+,a2
        moveq   #1,d1
        move.w  d1,_DOS_ERR
#else
        movem.l (sp)+,d2/a2
        move.w  #1,_DOS_ERR
#endif
        tst.l   d0
        bmi.s   de_return
        clr.w   _DOS_ERR
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

