/*
 * lisa2.S - Apple Lisa specific assembler functions
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

        .extern _lisa_int_1_c
        .extern _lisa_int_2_c
        .extern _vector_5ms
        .extern lisa_lba2phys

/******************************************************************************/
/* Interrupts                                                                 */
/******************************************************************************/

// Int 1 is used by VIA2 and VBL
        .globl  _lisa_int_1
_lisa_int_1:
        move.w  #0x2700,sr              // Forbid higher level interrupts
        movem.l d0-d1/a0-a1,-(sp)
        jbsr    _lisa_int_1_c           // Delegate to C implementation
        movem.l (sp)+,d0-d1/a0-a1
        rte

// Int 2 is used by VIA1 (COPS)
        .globl  _lisa_int_2
_lisa_int_2:
        move.w  #0x2700,sr              // Forbid higher level interrupts
        movem.l d0-d1/a0-a1,-(sp)
        jbsr    _lisa_int_2_c           // Delegate to C implementation
        movem.l (sp)+,d0-d1/a0-a1
        rte

// Call vector_5ms (200 Hz system timer callback) from C.
// As it will return with RTE, we must setup a proper stack frame.
        .globl  _lisa_call_5ms
_lisa_call_5ms:
        // The return address is already on the stack.
        // Assume that the CPU is always a 68000 and _longframe is false
        move.w  sr,-(sp)                // Push SR
        move.l  _vector_5ms,a0
        jmp     (a0)                    // Call the 200 Hz system callback

/******************************************************************************/
/* void lisa_read_lba_sector(ULONG lba, UBYTE *buffer);                       */
/* Read a sector from floppy, referred by an absolute sector number.          */
/* FIXME: Write a real driver instead of calling TWGREAD from the Apple ROM!  */
/******************************************************************************/

        .globl  _lisa_read_lba_sector
_lisa_read_lba_sector:
        movem.l d2-d7/a2-a6,-(sp)

        move.l  4*11+4(sp),d6       // sector lba
        move.l  4*11+8(sp),a2       // data buffer
        lea     tags,a1             // tags buffer
        move.l  #0x00fcc001,a0      // Disk shared memory address
        move.l  #0x00fcdd81,a3      // VIA address for FDIR
        moveq   #0,d0               // Speed
        move.l  #0x300000,d2        // Timeout for read
        lea     0x00fe0000,a6       // Start of Boot ROM

        jbsr    lisa_lba2phys       // Convert LBA to physical address into d1
        jsr     0x94(a6)            // TWGREAD, read floppy sector

        movem.l (sp)+,d2-d7/a2-a6
        rts

/******************************************************************************/
/* BSS                                                                        */
/******************************************************************************/

        .bss

tags:   .ds.w   6   // TWGREAD needs a second buffer to store the sector tags

#endif /* MACHINE_LISA */
