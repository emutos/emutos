/*
 * bootasm.S - assembler stuff for boot.c
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
 *
 * Authors:
 *  LVL  Laurent Vogel
 *  VRI  Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"

        .globl  _bootasm

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

_bootasm:
        move.w  #0x2700,sr
        move.l  4(sp),a3    // dest
        move.l  8(sp),a2    // src
        move.l  12(sp),d2   // count
        move.l  16(sp),d3   // cpu

        /* align the size to a multiple of 4 */

        addq.l  #3,d2
        andi.l  #-4,d2

        /* if the destination address is lower than the source address,
         * then the boot routine will not be overwritten.
         * we can safely run it in place */

        cmp.l   a2,a3
        bls.s   boot

        /* copy the boot routine after the destination ramtos */

        lea     (a3,d2),a4  // after destination ramtos

        /* if the new boot location overlaps with the current location,
         * then copy it just after the current location. */

        lea     bootend(pc),a5
        cmp.l   a5,a4
        bhs.s   copyboot    // no collision

        move.l  a5,a4       // collision: new boot location is bootend

copyboot:
        move.l  a4,a1
        lea     boot,a0
        move.l  #bootend-boot, d0
        addq.l  #3,d0       // round up to next long
        lsr.l   #2,d0
        subq.l  #1,d0       // one less for dbra
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
        move.l  a3,a4       // destination will be jump address

        /* copy the TOS to destination */

        cmp.l   a2,a3
        bhi.s   bcopy_backwards

        lsr.l   #2,d2
        subq.l  #1,d2
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
        bra.s   run

bcopy_backwards:
        add.l   d2,a2   // after last source long
        add.l   d2,a3   // after last destination long
        lsr.l   #2,d2
        subq.l  #1,d2
bcopy2:
        move.l  -(a2),-(a3)
#ifdef __mcoldfire__
        subq.l  #1,d2
        bpl.s   bcopy2
#else
        dbra    d2,bcopy2
        sub.l   #0x10000,d2
        bhi     bcopy2
#endif

run:
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
        cmpi.l   #30,d3
        bne.s    not68030
        MOVEC_CACR_D0
        ori.b   #0x08,d0        // invalidate instruction cache
        MOVEC_D0_CACR
not68030:
#endif
        jmp     (a4)            // return without using the stack

bootend:
