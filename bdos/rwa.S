/*
 * rwa.s - GEMDOS assembler interface
 *
 * Copyright (c) 1999 by Caldera Inc.
 * Copyright (c) 1985, 2001 by Authors (see below)
 * Copyright (c) 2016 The EmuTOS development team
 *
 * Authors:
 *  EWF  Eric W. Fleischman
 *  JSL  Jason S. Loveman
 *  SCC  Steven C. Cavender
 *  ACH  Anthony C. Hay
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



//============================================================================
//
// Originally written by JSL.
//
// MODIFICATION HISTORY
//
//      11 Mar 85       SCC     Added xgetsup() functionality as a hack in
//                              _entry().
//                              Removed get sup functionality from exit().
//      11 Mar 85       JSL     Changed timer vector number.
//      12 Mar 85       SCC     Modified xgetsup() functionality in _entry().
//      13 Mar 85       SCC     Changed 'xgetsup()' type functionality to
//                              'toggle/inquire processor/stack state' type
//                              functionality.
//      25 Mar 85       SCC     Modified 'tikhnd:'.  It didn't need to save
//                              the registers it was saving.
//       1 Apr 85       SCC     Modified 'tikhnd:'.  It needed to get the
//                              number of milliseconds since last tick into
//                              the right place for the stack frame for call
//                              to tikfrk().
//      10 Apr 85       SCC     Modified longjmp() to use long return value
//                              on stack.
//      12 Apr 85       SCC     Modified exit() to check for function -1
//                              (return address of screen driver.
//      14 Apr 85       SCC     Modified osinit() to disable interrupts while
//                              initializing the tick vector.
//      19 Apr 85       SCC     Modified osinit() to preserve the previous
//                              state of the SR.
//      22 Apr 85       SCC     Modified the saving of SR to store it
//                              temporarily in a static.
//
//      31 May 85       EWF     Added in conditional assembly for 68010-based
//                              systems.
//
//      19 Jun 85       EWF     Added additional conditional assemblies for
//                              68010-based systems.
//
//       9 Aug 85       SCC     Modified osinit() to get tick vector from
//                              clock device handle.
//
//                              NOTE:   STACK LOOKS DIFFERENT THAT ATARI'S ON
//                                      INVOCATION OF TICK HANDLER!
//
//                              Modified tikhnd: to work with new clock format
//                              (tick value in long above long flag value on
//                              stack)
//
//      15 Aug 85       SCC     Modified tikhnd:.  It was picking up the
//                              number of milliseconds from the wrong location
//                              on the stack.
//
//      18 Aug 85       SCC     Added copyright message.
//
//       1 Nov 85       SCC     Converted to runtime determination of
//                              processor type (68000 v. 68010)
//
//                              Added 68010 'MOVE SR,...' to 'MOVE CCR,...'
//                              illegal instruction handler
//
//                              Converted all exception vector handling to
//                              use BIOS calls, rather than direct addresses.
//
//       4 Nov 85       SCC     Cleaned up a stack imbalance problem in
//                              'ii_handler'.  It was not POPing D0/A0 off
//                              the stack before going off to the OEM's
//                              Illegal Instruction handler.
//
//      11 Nov 85       KTB     put trap2 handler back in for the nonce
//
//      14 May 86       KTB     removed dojmp
//
//      02 Jul 86       KTB     M01.01a.0702.01 osinit now returns address of
//                              date/time stamp for bdos
//
//      23 Jul 86       KTB     M01.01.0723.01 osinit needs to save stack ptr
//                              in some other reg besides d1.
//
//      24 Jul 86       KTB     M01.01.0724.01 osinit no longer returns addr
//                              of date/time stamp.  this is all handled in
//                              startup.a
//
//      04 Aug 86       KTB     M01.01.0804.02 osinit => _osinit
//
//      28 Aug 86       SCC     M01.01.0828.01 changed TRAP #2 handler back
//                              similar to the way it was in the olden days
//                              for Atari's benefit in bringing up a system
//                              where the VDI and AES are already in memory.
//
//      19 May 87       ACH     The 68070 has an exception stack frame similar
//                              to the 68010 but it does not impliment the
//                              "move ccr,d0" instruction. Hence the 68000 vs.
//                              68010 detection would give the oposite result
//                              to the one desired. Force mc68010 flag to true.
//
//      19 Nov 87       ACH     Moved _lbmove, _bfill, _bmove into this module.
//
//============================================================================

#include "asmdefs.h"
#include "asmpd.h"

// ==== Definitions ==========================================================

        .globl  _termuser
        .globl  _gouser
        .globl  _oscall
        .globl  _enter
        .globl  _old_trap2
        .globl  _bdos_trap2
        .globl  _xsuper

// ==== References ===========================================================

        .extern _osif
        .extern _xterm
        .extern _run
        .extern _longframe              // If not 0, use long stack frames

//==== bdos_trap2 - trap 2 entry point =======================================
//
// This minimal interface connects the entry point for a system where the AES
// and VDI are already in memory.
//

_bdos_trap2:
        tst.w   d0
        beq     oterm

        move.l  _old_trap2,-(sp)
        rts

oterm:  move.l  #fstrt,sp
        clr     -(sp)
        jsr     _xterm
        rte


        .bss
        .even


_old_trap2:
        .ds.l   1                       // old TRAP #2 handler

        .text



//==== _enter - Front end of TRAP #1 handler =================================

_enter:
        bsr     fix_trap

        btst.b  #5,(sp)         // are we in supervisor mode?
        bne     enter_sup       // yes, go check sup stack

        move.l  usp,a0          // no, check user stack
#ifdef __mcoldfire__
        move.w  (a0),d0
        cmp.w   #0x20,d0        // toggle/inquire state?
#else
        cmp.w   #0x20,(a0)      // toggle/inquire state?
#endif
        beq     x20_usr         // yes, go perform function

        bra     enter_2

enter_sup:
                                // next line is 68000 specific
#ifdef __mcoldfire__
        move.w  6(sp),d0
        cmp.w   #0x20,d0        // toggle/inquire state?
#else
        cmp.w   #0x20,6(sp)     // toggle/inquire state?
#endif
        beq     x20_sup         // yes, go perform function

enter_2:
        move.l  a6,-(sp)        // save temporarily so we can use it
        move.l  _run,a6         // a6 -> current basepage
        movem.l d0/a3-a5,P_REGSAVE(a6)
        move.l  (sp)+,P_A6SAVE(a6)  // save original a6
        move.w  (sp)+,d0        // status reg
        move.l  (sp)+,a4        // retadd
        btst    #13,d0          // if he is in system state, use his stack
        bne     systk
        move.l  usp,a5          // a5 -> user stack
#ifdef __mcoldfire__
        lea     -40(a5),a5
        movem.l d1-d7/a0-a2,(a5)
#else
        movem.l d1-d7/a0-a2,-(a5)
#endif
        move.l  a4,-(a5)        // retadd
        move.w  d0,-(a5)        // sr
        move.l  sp,a0
        move.l  a0,-(a5)
        move.l  a5,P_A7SAVE(a6) // update saved stack pointer
// now switch to fs stack
        move.l  #fstrt,sp
        lea     50(a5),a0
        bra     callos
// he was using his own ssp
systk:
#ifdef __mcoldfire__
        lea     -40(sp),sp
        movem.l d1-d7/a0-a2,(sp)
#else
        movem.l d1-d7/a0-a2,-(sp)
#endif
        move.l  a4,-(sp)
        move.w  d0,-(sp)
        move.l  usp,a0
        move.l  a0,-(sp)
        move.l  sp,P_A7SAVE(a6) // update saved stack pointer
        lea     50(sp),a0
        move.l  #fstrt,sp
callos: move.l  a0,-(sp)        // push parm pointer
        jsr     _osif
        addq.l  #4,sp

//...fall into gouser....



//==== _gouser ===============================================================

_gouser:
        move.l  _run,a5         // a5 -> current basepage
        move.l  d0,P_D0SAVE(a5)
ggusr:  move.l  P_A7SAVE(a5),a6 // stack pointer (maybe usp, maybe ssp)
        move.l  (a6)+,a4        // other stack pointer
        move.w  (a6)+,d0
        move.l  (a6)+,a3        // retadd
#ifdef __mcoldfire__
        movem.l (a6),d1-d7/a0-a2
        lea     40(a6),a6
#else
        movem.l (a6)+,d1-d7/a0-a2
#endif
        btst    #13,d0
        bne     retsys          // a6 is (user-supplied) system stack
        move.l  a4,sp
        move.l  a6,usp
gousr:  move.l  a3,-(sp)
        move    d0,-(sp)
        movem.l P_REGSAVE(a5),d0/a3-a6

        bra     fix_rte

retsys: move.l  a6,sp
        move.l  a4,usp
        bra     gousr

        // attempting to fix a problem where return codes provided by
        // xterm [ 0x4C ] aren't correctly passed back to the parent.
        // this is entered from C code as termuser()
        // aug 2005, RCL
_termuser:
        move.l  _run,a5
        bra     ggusr

        .bss
        .even

        .ds.w   1000
fstrt:
        .ds.l   1

        .text

//==== function 0x20 handler - toggle/inquire processor/stack state ==========

//msg_super:
//      .ascii "BDOS: xsuper - switching to supervisor mode ...\n\0"
//      .even
_xsuper:
//      pea msg_super
//      bsr _kprint
//      addq #4,sp
x20_usr:                        // user mode entry point
        move.l  2(a0),d1        // get parameter
        beq     xu_0            // already have new SSP from
                                // old USP, go handle 0L entry

        subq.l  #1,d1           // check for +1L entry
        beq     x20_inq         // go handle inquiry

        move.l  2(a0),a0        // get new SSP from parameter
xu_0:
        move.w  (sp)+,d0        // pop SR
        move.l  (sp)+,-(a0)     // transport the return address
#ifdef __mcoldfire__
        ori.l   #0x2000,d0      // set supervisor mode
#else
        ori.w   #0x2000,d0      // set supervisor mode
#endif
        move.w  d0,-(a0)        // push SR
        move.l  sp,d0           // set return value (old SSP)
        move.l  a0,sp           // set new SSP

        bra     fix_rte





// supervisor mode gemdos(20) entry point

x20_sup:
        move.l  8(sp),d1        // get parameter
        beq     xs_0            // go handle 0L entry

        subq.l  #1,d1           // check for +1L entry
        beq     x20_inq         // go handle inquiry

        move.l  8(sp),a1        // point to new SSP
        move.w  (sp)+,d0        // pop SR
        move.l  (sp)+,-(a1)     // transport the return address
        move.w  d0,-(a1)        // push SR
        move.l  usp,a0          // get USP
        cmpa.l  a0,sp           // already at old USP?
        beq     xs_usp          // don't play with stack

        move.l  (sp)+,-(a0)     // transport func # and parm
        move.l  a1,sp           // update SSP
        bra     xs_exit

xs_usp:
        move.l  a1,sp           // update SSP
        bra     xs_scram

xs_0:
        move.l  sp,a0           // create USP
        addq.l  #6,a0           // this is 68000 specific
xs_exit:
        move.l  a0,usp          // update USP
xs_scram:
        bclr.b  #5,(sp)         // set user mode

        bra     fix_rte

// gemdos(20) inquiry mode entry point

x20_inq:
        moveq.l #0,d0           // return 0L for user mode
        btst.b  #5,(sp)         // supervisor mode?
        beq     xi_exit         // no, user mode
        subq.l  #1,d0           // return -1L for sup mode
xi_exit:
        bra     fix_rte

//==== call dosjr from within itself (or from linked-in shell) ===============

_oscall:
        link    a6,#0
        move.l  a0,-(sp)
        lea     8(a6),a0
        move.l  a0,-(sp)
        jsr     _osif
        addq.l  #4,sp
        move.l  (sp)+,a0
        unlk    a6
        rts

//==== fix_trap - make exception stack frame look like a 68000 frame =========

fix_trap:
#ifdef __mcoldfire__
        move.w  4(sp),d0        // Format/Vector Word
        andi.l  #0x3000,d0      // Keep the filler bits
        beq.s   ft_cf_nofiller

        // Assume a 2-byte filler after the exception frame
        move.w  10(sp),12(sp)   // lsw of return address of TRAP
        move.w  8(sp),10(sp)    // msw of return address of TRAP
        move.w  6(sp),8(sp)     // sr
        move.l  (sp),4(sp)      // return address of BSR
        addq.l  #4,sp           // discard the additional words
        rts

ft_cf_nofiller:
        move.w  2(sp),4(sp)     // lsw of return address of BSR
        move.w  (sp),2(sp)      // msw of return address of BSR
        addq.l  #2,sp           // discard the Format/Vector Word
#else
        tst.w   _longframe      // check processor type
        beq     ft_exit

        move.w  8(sp),10(sp)    // lsw of return address of TRAP
        move.w  6(sp),8(sp)     // msw of return address of TRAP
        move.w  4(sp),6(sp)     // sr
        move.w  2(sp),4(sp)     // lsw of return address of BSR
        move.w  (sp),2(sp)      // msw of return address of BSR
        addq.l  #2,sp           // discard the missing word
#endif
ft_exit:
        rts



//==== fix_rte - re-build exception stack frame for current processor ========

fix_rte:
#ifdef __mcoldfire__
        move.w  #0x4084,-(sp)   // store a fake Format/Vector Word
#else
        tst.w   _longframe      // check processor type
        beq     fr_exit

        subq.l  #2,sp           // reserve a hole on top of the stack
        move.w  2(sp),(sp)      // move SR
        move.w  4(sp),2(sp)     // msw of return address of TRAP
        move.w  6(sp),4(sp)     // lsw of return address of TRAP
        clr.w   6(sp)           // store a bogus exception frame type
#endif
fr_exit:
        rte
