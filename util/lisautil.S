/*
 * lisautil.S - Apple Lisa routines shared by bootsector and EmuTOS
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

// Convert absolute sector number (LBA) from d6
// into physical sector address in d1 : Drive/Side/Sector/Track
// The floppy has 1 side, 80 tracks with a variable number of sectors.
// Tracks 00-15 : 12 sectors, LBA   0 to 191
// Tracks 16-31 : 11 sectors, LBA 192 to 367
// Tracks 32-47 : 10 sectors, LBA 368 to 527
// Tracks 48-63 :  9 sectors, LBA 528 to 671
// Tracks 64-79 :  8 sectors, LBA 672 to 799
        .globl  lisa_lba2phys
lisa_lba2phys:
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

#endif /* MACHINE_LISA */
