/*
 * bootram.S - Run EmuTOS from RAM
 *
 * Copyright (c) 2016 The EmuTOS development team
 *
 * Authors:
 *  LVL  Laurent Vogel
 *  VRI  Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * This code is common for all RAM loaders.
 * It may be included in a bootsector, so everything must be pc-relative.
 * It may also run on foreign operating system, so there must not be any usage
 * of TOS features.
 */

 #include "asmdefs.h"

        .globl  _bootram
/*
 * void bootram(const UBYTE *src, ULONG size, ULONG cpu);
 *
 * Copy ramtos.img from a temporary buffer to its hardcoded location,
 * then run it.
 * The temporary ramtos starts at 'src', for 'size' bytes.
 * 'cpu' is 0 for 68000, 30 for 68030, etc.
 *
 * This needs to be done in assembly language, because the copy routine
 * may need to be copied to a safe place to avoid beeing overwritten
 * when ramtos is copied to its final destination.
 */

_bootram:
        // Disable the interrupts, since we are going to destroy everything
        move.w  #0x2700,sr

        // Get the parameters from the stack
        move.l  4(sp),a4        // src
        move.l  8(sp),d4        // size
        move.l  12(sp),d5       // cpu

        // Get destination address from the OS header
        move.l  8(a4),a5        // dest = _os_beg

        // Round size to next long
        addq.l  #3,d4
        andi.l  #-4,d4

        // If the end of the boot routine is before the destionation buffer,
        // then it is already in a safe place. It will not be overwritten
        // so it can be run from its current location.
        lea     endboot(pc),a1
        cmp.l   a1,a5           // compare dest to end of boot routine
        bhs.s   boot            // already safe place

        move.l  a5,a0           // dest
        add.l   d4,a0           // + size

        // If the start of the boot routine is after the destionation buffer,
        // then it is already in a safe place. It will not be overwritten
        // so it can be run from its current location.
        lea     boot(pc),a1
        cmp.l   a1,a0           // compare end of dest to begin of boot routine
        bls.s   boot            // already safe place

        // The boot routine is going to be overwritten by ramtos copy.
        // Move it after both the destination buffer and the source buffer.
        // Compute the highest address in a1 for destination of boot routine.
        move.l  a4,a1           // src
        add.l   d4,a1           // + size

        cmp.l   a0,a1           // compare end of src to end of dest
        bhs.s   copyboot        // end of src is the highest address

        move.l  a0,a1           // end of dest is the highest address

copyboot:
        lea     boot(pc),a2     // src boot
        move.l  a1,a3           // dest boot
        moveq   #((endboot-boot)+3)&-4,d2   // size boot rounded to next long
        move.l  a3,a6           // return to destination
        bra.s   copy_a2_a3_d2_sync_caches

boot:
        // This boot is a first boot
        clr.l   0x6fc.w         // warm_magic

        // Prevent reset vector being called during ramtos startup.
        // This must be done here, because ramtos checks for magic values
        // very early, even before clearing sysvars.
        clr.l   0x426.w         // resvalid

        // Copy ramtos.img to its final destination
        move.l  a4,a2           // src
        move.l  a5,a3           // dest
        move.l  d4,d2           // size
        move.l  4(a2),a6        // return to ramtos reset vector

        // Fall into copy_a2_a3_d2_sync_caches

/*
 * Copy buffer from a2 to a3 with size d2,
 * synchronize the CPU instruction and data caches,
 * then return to a6.
 */
copy_a2_a3_d2_sync_caches:
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
        bhi.s   bcopy
#endif
        bra.s   sync_caches

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
        bhi.s   bcopy2
#endif

sync_caches:
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
        moveq   #30,d0
        cmp.l   d0,d5
        bne.s   not68030
        MOVEC_CACR_D0
        ori.b   #0x08,d0        // invalidate instruction cache
        MOVEC_D0_CACR
not68030:
#endif
        jmp     (a6)            // return without using the stack

endboot:
