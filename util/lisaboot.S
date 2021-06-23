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

        .extern lisa_lba2phys

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
        jsr     lisa_lba2phys(pc)   // Convert LBA to physical address into d1
        jsr     0x94(a6)            // TWGREAD, read floppy sector

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
/* Data                                                                       */
/******************************************************************************/

message:
        .asciz   "LOADING EMUTOS..."

#endif /* MACHINE_LISA */
