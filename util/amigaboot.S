/*
 * amigaboot.S - AmigaOS bootsector to load EmuTOS in RAM
 *
 * Copyright (C) 2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#ifndef GENERATING_DEPENDENCIES
/* Defines generated from emutos.map */
#include "../obj/ramtos.h"
#endif

/******************************************************************************/
/* AmigaOS defines                                                            */
/******************************************************************************/

// Exec Library Vector Offsets
#define _LVOAllocMem -198
#define _LVOAvailMem -216
#define _LVODoIO -456
#define _LVOSuperState -150

// AllocMem/AvailMem flags
#define MEMF_CHIP  0x00000002
#define MEMF_FAST  0x00000004
#define MEMF_TOTAL 0x00080000 // Since Kickstart 2.0

// Trackdisk Device defines
#define CMD_READ 2
#define io_Command 32
#define io_Flags 34
#define io_Error 35
#define io_Length 36
#define io_Data 40
#define io_Offset 44

/******************************************************************************/
/* Boot block header                                                          */
/******************************************************************************/

        .dc.l   0x444f5300                  // 'DOS\0'
        .dc.l   0                           // Checksum fixup, patched by mkrom
        .dc.l   880                         // Floppy

bootentry:
        bra.s   bootmain

        .org    14                          // Offset referred by mkrom
ramtos_size:
        .dc.l   0                           // ramtos size, patched by mkrom

/******************************************************************************/
/* Boot program                                                               */
/******************************************************************************/

bootmain:
// This is called by Kickstart. No relocations allowed!
// Inputs:
// a1 = trackdisk.device I/O request pointer
// a6 = SysBase

        move.l  a1,a5                       // Save I/O request pointer

// Detect Chip RAM size ////////////////////////////////////////////////////////

        // Get total amount of Chip RAM, to be used as ST-RAM
        // FIXME: This also mistakenly detects WinUAE's 32-bit chip RAM
        move.l  #MEMF_CHIP+MEMF_TOTAL,d1
        jsr     _LVOAvailMem(a6)

        // Round to next 256 KB boundary
        add.l   #256*1024-1,d0
        andi.l  #-256*1024,d0

        move.l  d0,d7                       // Save total amount of Chip RAM

// Allocate temporary buffer ///////////////////////////////////////////////////

        // Round ramtos size to next sector boundary
        move.l  ramtos_size(pc),d2
        add.l   #511,d2
        andi.l  #-512,d2

        // Allocate a temporary buffer for ramtos
        move.l  d2,d0                       // Buffer size
        moveq   #MEMF_CHIP,d1               // Attributes
        // FIXME: should allow any kind of RAM for temporary buffer
        jsr     _LVOAllocMem(a6)

        tst.l   d0                          // Address returned?
        jbeq    error                       // No

        move.l  d0,a4                       // Save buffer address

// Load ramtos /////////////////////////////////////////////////////////////////

        // Load ramtos into temporary buffer
        move.l  a5,a1                       // I/O request pointer
        move.w  #CMD_READ,io_Command(a1)    // Read
        move.l  #1024,io_Offset(a1)         // Offset of ramtos on floppy
        move.l  d2,io_Length(a1)            // ramtos length rounded to next sector
        move.l  a4,io_Data(a1)              // Buffer
        jsr     _LVODoIO(a6)

        tst.l   d0                          // Error?
        jbne    error                       // Yes

// Detect Alt-RAM //////////////////////////////////////////////////////////////
// This code is inspired from Linux bootstrap
// http://lxr.free-electrons.com/source/arch/m68k/boot/amiga/bootstrap.c?v=2.0.40

        move.l  a4,a1                       // Buffer
        adda.l  #OFFSETOF(ADR_ALTRAM_REGIONS),a1    // Alt-RAM region list

        // Unscrupulously examine Exec's private data to detect Alt-RAM
        lea     322(a6),a0                  // MemList

next_memheader:
        move.l  (a0),d0                     // Next MemHeader?
        beq.s   endaltram                   // No

        move.l  d0,a0                       // MemHeader

        move.l  20(a0),d0                   // Region start address
        andi.l  #-64*1024,d0                // Round to lower 64 KB boundary

        // If this region starts at address 0, it is the ST-RAM. Skip it.
        beq.s   next_memheader

        move.w  14(a0),d1                   // mh_Attributes
        andi.w  #MEMF_CHIP+MEMF_FAST,d1     // Only allow this memory type

        // Skip unknown memory type (such as bogus MemHeader at end of list)
        beq.s   next_memheader

        // Store this region in the Alt-RAM region list
        move.l  d0,(a1)+                    // Store region start address
        move.l  24(a0),d1                   // Region end address
        add.l   #64*1024-1,d1               // Round to next 64 KB boundary
        andi.l  #-64*1024,d1
        sub.l   d0,d1                       // Subtract start to get size
        move.l  d1,(a1)+                    // Store region size

        bra.s   next_memheader

endaltram:
        // Terminate the Alt-RAM region list with NULL pointer
        clr.l   (a1)

// Leave AmigaOS ///////////////////////////////////////////////////////////////

        // Switch to Supervisor mode
        jsr     _LVOSuperState(a6)

        // Disable interrupts
        move.w  #0x2700,sr

/******************************************************************************/
/* Now we can destroy AmigaOS and take over the machine!                      */
/******************************************************************************/

        // ramtos expects TOS RAM settings to be already valid, so we do.

        // Set ST-RAM size
        clr.b   0x424.w                     // Fake memctrl
        move.l  d7,0x42e.w                  // _phystop: Chip RAM size
        move.l  #0x752019f3,0x420.w         // Validate memvalid
        move.l  #0x237698aa,0x43a.w         // Validate memval2
        move.l  #0x5555aaaa,0x51a.w         // Validate memval3

        // Set TT-RAM size
        clr.l   0x5a4.w                     // _ramtop: No TT-RAM
        move.l  #0x1357bd13,0x5a8.w         // Validate _ramvalid

        // Boot ramtos
        clr.l   -(sp)                       // CPU = 68000
        // FIXME: Detect CPU for proper cache management in _bootram
        move.l  ramtos_size(pc),-(sp)       // ramtos size
        move.l  a4,-(sp)                    // Temporary ramtos address
        bsr     _bootram                    // Boot ramtos

        // We will never return here

/******************************************************************************/
/* Boot block error handling. This will produce a Software Failure.           */
/******************************************************************************/

error:
        moveq   #1,d0                       // Error
        rts
