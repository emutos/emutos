/*
 * miscasm.S - Some small helper functions in assembler
 *
 * Copyright (C) 2001 by following authors
 * Copyright (C) 2013 by The EmuTOS development team.
 *
 * Authors:
 *  SCC     Steven C. Cavender
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

        .globl  _trap1          // call GEMDOS
        .globl  _trap1_pexec    // Reentrant Pexec call
        .globl  _stop_until_interrupt   // Wrapper around the STOP instruction


/*
 * trap1 - trap 1 (GEMDOS) entry point
 */
        .text
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



/*
 *  GEMDOS Pexec() call. This version is reentrant unlike the trap1 function!
 */
        .text
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
        rts

#endif /* USE_STOP_INSN_TO_FREE_HOST_CPU */
