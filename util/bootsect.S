/*
 * bootsect.S - bootsector for fast loading EmuTOS in RAM
 *
 * Copyright (c) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * this bootsector will load a ramtos.img using rwabs into a Malloc() buffer.
 * then a boot routine will be copied after it, and finally
 * the bootsector will jump into this boot routine
 * to avoid erasing itself. The boot routine will copy
 * the TOS from the temporary buffer to the address given in the TOS
 * header
 */

#include "asmdefs.h"

#define BOOTDEV 0x446

#define RWABS 4
#define GETBPB 7

        .text

        /* bootsector for a single-sized floppy */

        .org    0

        bra.s   start         //  0 bra[2]    branch
        .ascii  "Loader"      //  2 loader[6]
        .ascii  "ABC"         //  8 serial[3]
        .dc.b   0x00, 0x02    //  b bps[2]    bytes per sector
        .dc.b   0x02          //  d spc       sectors per cluster
        .dc.b   0x01, 0x00    //  e res[2]    number of reserved sectors
        .dc.b   0x02          // 10 fat       number of FATs
        .dc.b   0x70, 0x00    // 11 dir[2]    number of DIR root entries
        .dc.b   0xd0, 0x02    // 13 sec[2]    total number of sectors
        .dc.b   0xf8          // 15 media     media descriptor
        .dc.b   0x05, 0x00    // 16 spf[2]    sectors per FAT
        .dc.b   0x09, 0x00    // 18 spt[2]    sectors per track
        .dc.b   0x01, 0x00    // 1a sides[2]  number of sides
        .dc.b   0x00, 0x00    // 1c hid[2]    number of hidden sectors
ssect:
        .dc.w   0               // set by mkflop.c
sectcnt:
        .dc.w   0               // set by mkflop.c
start:
        /* compute ramtos size in d3 */

        moveq   #0,d3
        move.w  sectcnt(pc),d3  // number of sectors
        moveq   #9,d0           // 512 bytes per sector
        lsl.l   d0,d3           // ramtos size in bytes

        /* Malloc() ramtos buffer */

        move.l  d3,-(sp)
        move.w  #0x48,-(sp)     // Malloc()
        trap    #1
        addq.l  #6,sp

        tst.l   d0
        beq.s   fail

        move.l  d0,a4           // temporary ramtos buffer

        /* getbpb before rwabs */

        move.w  BOOTDEV,-(sp)
        move.w  #GETBPB,-(sp)
        trap    #13
        addq.l  #4,sp

        /* read the sectors in memory */

        move.w  sectcnt(pc),d6
        move.w  BOOTDEV,-(sp)
        move.w  ssect(pc),-(sp)
        move.w  d6,-(sp)
        move.l  a4,-(sp)        // temporary ramtos buffer
        clr.w   -(sp)
        move.w  #RWABS,-(sp)
        trap    #13
        lea     14(sp),sp
        tst.w   d0
        bne     fail

        /* disable interrupts */

        move.w  #0x2700,sr

        /* tell EmuTOS that it's being started from an auto-booting floppy */

        move.l  #0x1234abcd,0x6f8.w     // AUTOBOOT_MAGIC, _autoboot_magic

        /* this boot is a first boot */

        clr.l   0x6fc.w                 // warm_magic

        /* prevent reset vector being called during EmuTOS startup */

        clr.l   0x426                   // resvalid

        /* if the boot routine is before the destionation address,
         * then it is already in a safe place */

        move.l  8(a4),a5                // destination address
        cmp.l   endboot(pc),a5          // end of boot routine
        bhs.s   boot                    // already safe place

        /* copy boot routine immediately after the destination ramtos */

        lea     boot(pc),a0
        lea     (a4,d3),a3              // new boot routine
        move.l  a3,a1                   // destination
        move.l  #endboot-boot,d0        // size
        bsr     memcpy

        /* jump in boot routine */

        jmp     (a3)

fail:
        rts

boot:
        /* copy from temporary address to destination */

        move.l  a5,a1   // destination
        move.l  a4,a0   // source
        move.l  d3,d0   // size
        bsr     memcpy

        /* jump in TOS */

        jmp     (a5)

        /* copies d0 bytes from a0 to a1 */
memcpy:
        addq.l  #3,d0   // round size to next long
        andi.l  #-4,d0

        cmp.l   a0,a1
        bhi.s   bcopy_backwards

        lsr.l   #2,d0   // number of longs
        subq.l  #1,d0   // subtract 1 for dbra
bcopy:
        move.l  (a0)+,(a1)+
#ifdef __mcoldfire__
        subq.l  #1,d0
        bpl.s   bcopy
#else
        dbra    d0,bcopy
        sub.l   #0x10000,d0
        bhi.s   bcopy
#endif
        rts

bcopy_backwards:
        add.l   d0,a0   // after last source long
        add.l   d0,a1   // after last destination long
        lsr.l   #2,d0   // number of longs
        subq.l  #1,d0   // subtract 1 for dbra
bcopy2:
        move.l  -(a0),-(a1)
#ifdef __mcoldfire__
        subq.l  #1,d0
        bpl.s   bcopy2
#else
        dbra    d0,bcopy2
        sub.l   #0x10000,d0
        bhi.s   bcopy2
#endif
        rts
endboot:
