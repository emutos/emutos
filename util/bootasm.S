/*
 * bootasm.S - assembler stuff for boot.prg
 *
 * Copyright (c) 2001 The EmuTOS development team
 *
 * Authors:
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"

        .globl  _bootasm
        .globl  _getbootsize
        .extern _cpu

        .text
/*
 * void bootasm(long dest, long src, long count);
 * will move count bytes from address src to address dest,
 * and will jump in dest.
 *
 * It needs to be done in assembler, because the copy
 * routine needs to be first copied out of the destination
 * zone in memory.
 */

_getbootsize:
        move.l  #bootend-boot, d0
        rts

_bootasm:
        move.w  #0x2700,sr
        move.l  4(sp),a3
        move.l  8(sp),a2
        move.l  12(sp),d2

        /* align the size to a multiple of 4 */

        btst    #0,d2
        beq     even
        add.l   #1,d2
even:
        btst    #1,d2
        beq     four
        add.l   #2,d2
four:

        /* copy the boot routine after the TOS buffer */

        lea     (a2,d2),a4
        move.l  a4,a1
        lea     boot,a0
        bsr     _getbootsize
        lsr.l   #2,d0
copy:
        move.l  (a0)+,(a1)+
#ifdef __mcoldfire__
        subq.l  #1,d0
        bpl.s   copy
#else
        dbra    d0,copy
#endif

        /* When the TOS is copied to its final destination,
         * it may override the current stack.
         * We could setup a safe stack here.
         * Instead, we just avoid using the stack.
         */

        /* jump into boot routine */

        bra.s   _invalidate_icache
        // it will return to a4

/* The boot routine has been copied to another location.
 * As a result, it must use only PC-relative code.
 */
boot:
        /* copy the TOS to destination */

        lsr.l   #2,d2
        sub.l   #1,d2
        move.l  a3,a4
bcopy:
        move.l  (a2)+,(a3)+
#ifdef __mcoldfire__
        subq.l  #1,d2
        bpl.s   bcopy
#else
        dbra    d2,bcopy
        sub.l   #0x10000,d2
        bhi     bcopy
#endif

        /* jump into it */

        // fall into _invalidate_icache
        // it will return to a4

_invalidate_icache:
#ifdef __mcoldfire__
        // This code comes from the MCF547x Reference Manual
        // Section 7.11 Cache Management
        //
        // The ColdFire has no cinva instruction.
        // Instead, cpushl writes the modified cache data to the RAM
        // then invalidates the caches (data + instruction) except if
        // the bits DDPI and IDPI have been set in the CACR.
        //
        // The ColdFire V4e core has a 32 KB instruction cache
        // and a 32 KB data cache. Both caches have the same structure.
        // The data is stored in "Line" elements of 16 bytes.
        // The Lines are stored in a 2D array of 4 Ways * 512 Sets.
        //
        // The following changes have been made to the original code:
        // - flush both caches with "cpushl bc" instead of "cpushl dc"
        // - flush the 512 Sets (original code forgot the last one)
        
        nop                     // synchronize-flush store buffer
        moveq.l #0,d0           // initialize way counter
        moveq.l #0,d1           // initialize set counter
        move.l  d0,a0           // initialize cpushl pointer

setloop:
        cpushl  bc,(a0)         // push cache line a0 (both caches)
        add.l   #0x0010,a0      // increment set index by 1
        addq.l  #1,d1           // increment set counter
        cmpi.l  #512,d1         // are sets for this way done?
        bne.s   setloop

        moveq.l #0,d1           // set counter to zero again
        addq.l  #1,d0           // increment to next way
        move.l  d0,a0           // set = 0, way = d0
        cmpi.l  #4,d0           // flushed all the ways?
        bne.s   setloop
#else
        cmpi.l   #30,_cpu
        bne.s    not68030
        MOVEC_CACR_D0
        ori.b   #0x08,d0        // invalidate instruction cache
        MOVEC_D0_CACR
not68030:
#endif
        jmp     (a4)            // return without using the stack

bootend:
