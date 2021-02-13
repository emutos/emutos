/*
 * lisa.c - Apple Lisa specific assembler functions
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

#endif /* MACHINE_LISA */
