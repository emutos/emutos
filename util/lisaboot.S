/*
 * lisaboot.S - Apple Lisa bootsector to load EmuTOS in RAM
 *
 * Copyright (C) 2021 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#ifdef MACHINE_LISA

#ifndef GENERATING_DEPENDENCIES
/* Defines generated from emutos.map */
#include "../obj/ramtos.h"
#endif

/******************************************************************************/
/* Lisa Boot ROM system variables                                             */
/******************************************************************************/

#define SCRNBASE 0x110 /* Screen base address */
#define TOTLMEM 0x2a8 /* Total amount of RAM */

/******************************************************************************/
/* Boot sector header, patched by mkrom                                       */
/******************************************************************************/

start:
        bra.s   bootmain

        .org    2                           // Offset referred by mkrom
ramtos_size:
        .dc.l   0                           // ramtos size, patched by mkrom

/******************************************************************************/
/* Boot program                                                               */
/******************************************************************************/

bootmain:
// This is called by the Boot ROM. No relocations allowed!
// Actually, this bootsector is loaded at address 0x00020000.
// So we can assume there will be enough space afterwards to load ramtos.

        lea     0x00fe0000,a6       // Start of Boot ROM

// Clear the screen
        move.l  SCRNBASE,a0
        moveq   #0,d0
        move.w  #720*364/8/4-1,d1
clear:
        move.l  d0,(a0)+
        dbra    d1,clear

// Display the boot message
        lea     message(pc),a3
        moveq   #0,d4               // Left margin
        moveq   #0,d5               // Row
        moveq   #1,d6               // Column
        jsr     0x88(a6)            // CONVRTD5, display string

// Read ramtos from next sectors
        move.l  ramtos_size(pc),d4  // Number of bytes to load
        moveq   #1,d6               // LBA sector (absolute sector on disk)
        lea     start+512(pc),a2    // Address to load ramtos

        move.l  #0x00fcc001,a0      // Disk shared memory address
        move.l  #0x00fcdd81,a3      // VIA address for FDIR
        moveq   #0,d0               // Speed
        move.l  #0x300000,d2        // Timeout for read

nextsector:
        lea     512(a2),a1          // Load the header (tag) after data buffer
        jbsr    lba2phys            // Convert LBA to physical address into d1
        jbsr    0x94(a6)            // TWGREAD, read floppy sector

        sub.l   #512,d4             // We have read a sector
        bls.s   loaded              // Exit when ramtos is completely loaded

        addq.w  #1,d6               // Next LBA sector
        lea     512(a2),a2          // Load it just after
        jra     nextsector          // Loop

loaded:
        // Disable interrupts
        move.w  #0x2700,sr

/******************************************************************************/
/* Now we can take over the machine!                                          */
/******************************************************************************/

        // ramtos expects TOS RAM settings to be already valid, so we do.

        // Set ST-RAM size
        clr.b   0x424.w                     // Fake memctrl
        move.l  TOTLMEM,0x42e.w             // _phystop: Total RAM size
        move.l  #0x752019f3,0x420.w         // Validate memvalid
        move.l  #0x237698aa,0x43a.w         // Validate memval2
        move.l  #0x5555aaaa,0x51a.w         // Validate memval3

        // Set TT-RAM size
        clr.l   0x5a4.w                     // _ramtop: No TT-RAM
        move.l  #0x1357bd13,0x5a8.w         // Validate _ramvalid

        // Boot ramtos
        clr.l   -(sp)                       // CPU = 68000
        move.l  ramtos_size(pc),-(sp)       // ramtos size
        pea     start+512(pc)               // Temporary ramtos address
        bsr     _bootram                    // Boot ramtos
        // We will never return here

/******************************************************************************/
/* Subroutines                                                                */
/******************************************************************************/

// Convert absolute sector number (LBA) from d6
// into physical sector address in d1 : Drive/Side/Sector/Track
// The floppy has 1 side, 80 tracks with a variable number of sectors.
// Tracks 00-15 : 12 sectors, LBA   0 to 191
// Tracks 16-31 : 11 sectors, LBA 192 to 367
// Tracks 32-47 : 10 sectors, LBA 368 to 527
// Tracks 48-63 :  9 sectors, LBA 528 to 671
// Tracks 64-79 :  8 sectors, LBA 672 to 799
lba2phys:
        movem.l d5/d6,-(sp)         // Preserve d6 input value
        move.l  #0x80000000,d1      // Drive/Side/Sector/Track

// Determine one of the 5 track areas from LBA.
// First check the 8-sector area, then 9-sector area, and so on.

.test8:
        move.w  #672,d5             // Start of 8-sector area
        cmp.w   d5,d6               // LBA is after?
        jls     .test9              // No, check previous area
        sub.w   d5,d6               // Convert to offset from start of area
        divu    #8,d6               // Divide by number of sectors of the area
        swap    d6                  // d6.w is the remainder = sector in track
        lsl.w   #8,d6               // Move sector number to the right bits
        move.w  d6,d1               // Then to the destination register
        swap    d6                  // d6.w is the quotient = track offset
        add.w   #64,d6              // + first track of area = absolute track
        move.b  d6,d1               // Move track number to destination
        jra     .endlba             // Done

.test9:
        move.w  #528,d5             // ... and so on.
        cmp.w   d5,d6
        jls     .test10
        sub.w   d5,d6
        divu    #9,d6
        swap    d6
        lsl.w   #8,d6
        move.w  d6,d1
        swap    d6
        add.w   #48,d6
        move.b  d6,d1
        jra     .endlba

.test10:
        move.w  #368,d5
        cmp.w   d5,d6
        jls     .test11
        sub.w   d5,d6
        divu    #10,d6
        swap    d6
        lsl.w   #8,d6
        move.w  d6,d1
        swap    d6
        add.w   #32,d6
        move.b  d6,d1
        jra     .endlba

.test11:
        move.w  #192,d5
        cmp.w   d5,d6
        jls     .test12
        sub.w   d5,d6
        divu    #11,d6
        swap    d6
        lsl.w   #8,d6
        move.w  d6,d1
        swap    d6
        add.w   #16,d6
        move.b  d6,d1
        jra     .endlba

.test12:
        divu    #12,d6
        swap    d6
        lsl.w   #8,d6
        move.w  d6,d1
        swap    d6
        move.b  d6,d1

.endlba:
        movem.l (sp)+,d5/d6
        rts

/******************************************************************************/
/* Data                                                                       */
/******************************************************************************/

message:
        .asciz   "LOADING EMUTOS..."

#endif /* MACHINE_LISA */
