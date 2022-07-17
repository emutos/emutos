/*
 * miscasm.S - Some small helper functions in assembler
 *
 * Copyright (C) 2001 by following authors
 * Copyright (C) 2013-2022 by The EmuTOS development team.
 *
 * Authors:
 *  SCC     Steven C. Cavender
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

/*
 * trap1 - trap 1 (GEMDOS) entry point
 */
        .globl  _trap1
_trap1:
        move.l  (sp)+,t1spsav   // save return address
#ifdef __mcoldfire__
        move.l  d2,t1regsav
        move.l  a2,t1regsav+4
        trap    #1              // call bdos call
        move.l  t1regsav,d2
        move.l  t1regsav+4,a2
#else
        movem.l d2/a2,t1regsav
        trap    #1              // call bdos call
        movem.l t1regsav,d2/a2
#endif
        move.l  t1spsav,-(sp)   // restore return address
        rts

        .bss
        .even
t1spsav:
        .ds.l   1       // Save space for _trap_1
t1regsav:
        .ds.l   2

        .text

/*
 *  GEMDOS Pexec() call. This version is reentrant unlike the trap1 function!
 */
        .globl  _trap1_pexec    // Reentrant Pexec call
_trap1_pexec:
#ifdef __mcoldfire__
        move.l  a2,-(sp)
        move.l  d2,-(sp)
#else
        movem.l d2/a2,-(sp)
#endif
        move.l  22(sp),-(sp)    // Push parameter on the stack again for pexec
        move.l  22(sp),-(sp)
        move.l  22(sp),-(sp)
        move.w  24(sp),-(sp)
        move.w  #0x4b,-(sp)
        trap    #1              // Pexec
        lea     16(sp),sp
#ifdef __mcoldfire__
        move.l  (sp)+,d2
        move.l  (sp)+,a2
#else
        movem.l (sp)+,d2/a2
#endif
        rts

#if USE_STOP_INSN_TO_FREE_HOST_CPU

/* void stop_until_interrupt(void)
 * Stop the CPU until an interrupt occurs.
 * This may save some host CPU time on emulators (i.e. ARAnyM).
 */
        .globl  _stop_until_interrupt
_stop_until_interrupt:
        move.w  sr,d0
        move.w  d0,d1           // Backup
#ifdef __mcoldfire__
        andi.l  #0x0700,d1      // Isolate IPL bits
#else
        andi.w  #0x0700,d1      // Isolate IPL bits
#endif

        // Convert the IPL into an offset in the stop block below
#ifdef __mcoldfire__
        lsr.l   #8-3,d1         // 8 bits right, then multiply by 4
        jmp     stop0(pc,d1.l)
#else
        lsr.w   #8-3,d1         // 8 bits right, then multiply by 4
        jmp     stop0(pc,d1.w)
#endif

        // Each stop case must be exactly 8 byte long.
        // Preserving sr is not strictly mandatory, but:
        // - it allows preserving additional special bits (trace?)
        // - it is a padding instruction to reach a block multiple of 2 bytes

stop0:
        stop    #0x2000
        move.w  d0,sr
        rts

stop1:
        stop    #0x2100
        move.w  d0,sr
        rts

stop2:
        stop    #0x2200
        move.w  d0,sr
        rts

stop3:
        stop    #0x2300
        move.w  d0,sr
        rts

stop4:
        stop    #0x2400
        move.w  d0,sr
        rts

stop5:
        stop    #0x2500
        move.w  d0,sr
        rts

stop6:
        stop    #0x2600
        move.w  d0,sr
        rts

stop7:
        stop    #0x2700
        move.w  d0,sr

        // Fall through _just_rts

#endif /* USE_STOP_INSN_TO_FREE_HOST_CPU */

// The RTS below is shared for other purposes.
        .globl  _just_rts
_just_rts:
        rts

/*
 *  WORD mul_div_round(WORD mult1,WORD mult2,WORD divisor)
 *
 *  returns (mult1 * mult2 / divisor), rounded away from zero
 *
 *  this is actually calculated as:
 *      ((mult1 * mult2 * 2 / divisor) +/- 1) / 2
 *  where mult1 & divisor are signed 16-bit integers
 *        mult2 is a signed 15-bit integer
 *
 *  this version of the code is derived (with some tweaks) from the
 *  original imported AES code (function mul_div() in aes/gsx2.S)
 *
 *  if you do not need rounding, use mul_div() from intmath.h instead!
 */
        .globl  _mul_div_round
_mul_div_round:
#ifdef __mcoldfire__
        moveq.l #0,d0
        move.w  6(sp),d0        // d0 = mult2
        add.l   d0,d0           // d0 = mult2 * 2
        muls.w  4(sp),d0        // d0 = mult1 * mult2 * 2
        divs.w  8(sp),d0        // d0 = (mult1 * mult2 * 2) / divisor
        ext.l   d0
        jmi     mdrminus
        addq.l  #1,d0           // d0 = (mult1 * mult2 * 2) / divisor + 1
        asr.l   #1,d0           // d0 = ((mult1 * mult2 * 2) / divisor + 1) / 2
        rts
mdrminus:
        subq.l  #1,d0           // d0 = (mult1 * mult2 * 2) / divisor - 1
        asr.l   #1,d0           // d0 = ((mult1 * mult2 * 2) / divisor - 1) / 2
        rts
#else
        move.w  6(sp),d0        // d0 = mult2
        add.w   d0,d0           // d0 = mult2 * 2
        muls.w  4(sp),d0        // d0 = mult1 * mult2 * 2
        divs.w  8(sp),d0        // d0 = (mult1 * mult2 * 2) / divisor
        jmi     mdrminus
        addq.w  #1,d0           // d0 = (mult1 * mult2 * 2) / divisor + 1
        asr.w   #1,d0           // d0 = ((mult1 * mult2 * 2) / divisor + 1) / 2
        rts
mdrminus:
        subq.w  #1,d0           // d0 = (mult1 * mult2 * 2) / divisor - 1
        asr.w   #1,d0           // d0 = ((mult1 * mult2 * 2) / divisor - 1) / 2
        rts
#endif

/* ==== Glue for external vectors ===========================================
 *
 * EmuTOS uses d0-d1/a0-a1 as scratch registers. However the original TOS
 * used d0-d2/a0-a2 as scratch registers. For this reason, when jumping into
 * external user-supplied code we need to save/restore d2/a2. The routines
 * below provide the necessary assembler support.
 */

        .globl   _protect_v
        .globl   _protect_w
        .globl   _protect_ww
        .globl   _protect_wlwwwl

/*
 * LONG protect_v(LONG (*func)(void));
 */
_protect_v:
        move.l   4(sp),a0
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        jsr      (a0)
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts

/*
 * LONG protect_w(LONG (*func)(WORD), WORD);
 */
_protect_w:
        move.l   4(sp),a0
        move.w   8(sp),d0
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        move.w   d0,-(sp)
        jsr      (a0)
        addq.l   #2,sp
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts

/*
 * LONG protect_ww(LONG (*func)(WORD), WORD, WORD);
 */
_protect_ww:
        move.l   4(sp),a0
        move.l   8(sp),d0
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        move.l   d0,-(sp)
        jsr      (a0)
        addq.l   #4,sp
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts

/*
 * LONG protect_wlwwwl(LONG (*func)(), WORD, LONG, WORD, WORD, WORD, LONG);
 */
_protect_wlwwwl:
        movem.l  8(sp),d0-d1/a0-a1
        move.l   a2,-(sp)
        move.l   d2,-(sp)
#ifdef __mcoldfire__
        lea      -16(sp),sp
        movem.l  d0-d1/a0-a1,(sp)
#else
        movem.l  d0-d1/a0-a1,-(sp)
#endif
        move.l   28(sp),a0
        jsr      (a0)
        lea      16(sp),sp
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts
