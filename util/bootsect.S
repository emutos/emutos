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
 * This bootsector loads ramtos.img into a Malloc() buffer.
 * Then it boots it using bootram().
 */

#include "asmdefs.h"

        .extern _bootram

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

        /* We need a temporary buffer to load the ramtos.
         * If TOS is in ROM, we can safely call Malloc().
         * If the ROM only contains the loader for floppy TOS,
         * then only a minimal subset of BIOS/XBIOS functions are available
         * and there is no GEMDOS at all.
         * In this case, we must guess a safe address to load ramtos.
         */

        move.l  0x44e,a4        // _v_bas_ad: logical screen pointer
        cmp.l   #0x00018000,a4  // are we running with floppy TOS loader?
        bne.s   malloc          // no, use Malloc()

        // We are running on the loader for floppy TOS
        // Our temporary buffer will start just after the screen
        lea     32000(a4),a4
        bra.s   read

malloc:
        /* Malloc() ramtos buffer */

        move.l  d3,-(sp)
        move.w  #0x48,-(sp)     // Malloc()
        trap    #1
        addq.l  #6,sp

        tst.l   d0
        beq.s   fail

        move.l  d0,a4           // temporary ramtos buffer

read:
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
        bne.s   fail

        /* get the value of the _CPU cookie */

        moveq   #0,d0           // default value: 68000
        move.l  0x5a0.w,d1      // _p_cookies
        beq.s   cpudone         // no cookie jar

        move.l  d1,a0           // current cookie pointer

nextcookie:
        move.l  (a0)+,d0        // cookie name
        beq.s   cpudone         // end of cookie jar
        move.l  (a0)+,d1        // cookie value

        cmp.l   #0x5f435055,d0  // _CPU cookie?
        bne.s   nextcookie      // no

        move.l  d1,d0           // yes: keep value
cpudone:

        /* boot ramtos */

        move.l  d0,-(sp)        // cpu
        move.l  d3,-(sp)        // size
        move.l  a4,-(sp)        // src
        bsr     _bootram

fail:
        rts
