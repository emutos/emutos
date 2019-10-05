/*
 * miscasm.S - Some small helper functions in assembler
 *
 * Copyright (C) 2001 by following authors
 * Copyright (C) 2013-2019 by The EmuTOS development team.
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
 *  multiplies two WORDs together and divides by a WORD, returning a WORD.
 *  the result is rounded away from zero if the remainder is greater than
 *  or equal to half the divisor.
 *
 *  if you do not need rounding, use mul_div() from intmath.h instead!
 */
        .globl  _mul_div_round
_mul_div_round:
        move.l  d2,-(sp)        // ColdFire friendly
        move.l  d3,-(sp)
        moveq   #1,d1           // preload increment
        move.w  8+6(sp),d0
        muls    8+4(sp),d0      // d0 = mult1 * mult2
        jpl     mulplus
        neg.l   d1              // negate increment
mulplus:
        move.w  8+8(sp),d2
        divs    d2,d0           // d0 = mult1 * mult2 / divisor
        ext.l   d2              // extend divisor to longword for ColdFire
        jpl     divplus         // branch if divisor positive
        neg.l   d1              // else negate increment
        neg.l   d2              //  & make divisor positive
divplus:
        move.l  d0,d3
        swap    d3              // d3.w = remainder
        ext.l   d3              // extend remainder to longword for ColdFire
        jpl     remplus         // branch if positive
        neg.l   d3              // else make remainder positive
remplus:
        asl.l   #1,d3           // d3.l = 2 * remainder
        cmp.l   d2,d3           // bigger than divisor?
        jlt     noround         // no, branch
        add.l   d1,d0           // else add increment
noround:
        move.l  (sp)+,d3
        move.l  (sp)+,d2
        rts
