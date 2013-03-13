/*
 * bootsect.S - bootsector for fast loading EmuTOS in RAM
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * this bootsector will load a ramtos.img using rwabs.
 * (load COUNT sectors starting at sector number START)
 * the ramtos will be loaded at address TMPADDR, then
 * a boot routine will be copied after it, and finally
 * the bootsector will jump into this boot routine
 * to avoid erasing itself. The boot routine will copy
 * the TOS from TMPADDR to the address given in the TOS
 * header
 */

#include "asmdefs.h"

#define START 1
#define COUNT 384

/* The temporary load address must be:
 *   - high enough to not override the TOS internal variables
 *   - low enough so ramtos.img will not run over the screen
 */
#define TMPADDR 0x38000

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
execflg:
        .dc.w   0
ldmode:
        .dc.w   0
ssect:
        .dc.w   START
sectcnt:
        .dc.w   COUNT
ldaddr:
        .dc.l   TMPADDR
fatbuf:
        .dc.l   0
fname:
        .ds.b   11
resvd:
        .ds.b   1
start:
        /* getbpb before rwabs */

        move.w  BOOTDEV,-(sp)
        move.w  #GETBPB,-(sp)
        trap    #13
        addq.l  #4,sp

        /* read the sectors in memory */

        move.l  ldaddr(pc),a4
        move.w  sectcnt(pc),d6
        move.w  BOOTDEV,-(sp)
        move.w  ssect(pc),-(sp)
        move.w  d6,-(sp)
        move.l  a4,-(sp)
        clr.w   -(sp)
        move.w  #RWABS,-(sp)
        trap    #13
        lea     14(sp),sp
        tst.w   d0
        bne     fail

        /* tell EmuTOS to ask for floppy removal before going on */

        move.l  #0x1234abcd,0x6f8.w     // EJECT_MAGIC, _eject_magic

        /* d4 = max size of TOS */

        clr.l   d4
        move.w  d6,d4
        lsl.l   #8,d4
        add.l   d4,d4

        /* copy boot routine immediately after */

        lea     boot(pc),a0
        lea     (a4,d4),a3
        move.l  a3,a1
        move.l  #endboot-boot,d0
        bsr     memcpy

        /* jump in boot routine */

        jmp     (a3)

fail:
        rts

boot:
        move.w  #0x2700,sr

        /* copy from temporary address to destination */

        move.l  8(a4),a3
        move.l  a3,a1
        move.l  a4,a0
        move.l  d4,d0
        bsr     memcpy

        /* jump in TOS */

        jmp     (a3)

        /* copies d0 bytes from a0 to a1 */
memcpy:
        add.l   #3,d0
        lsr.l   #2,d0
        sub.l   #1,d0
cpy:
        move.l  (a0)+,(a1)+
#ifdef __mcoldfire__
        subq.l  #1,d0
        bpl.s   cpy
#else
        dbra    d0,cpy
        sub.l   #0x10000,d0
        bhi     cpy
#endif
        rts
endboot:
