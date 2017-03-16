/*
 * coldfire2.S - ColdFire specific assembler functions
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef __mcoldfire__
#error This file must only be compiled on ColdFire targets
#endif

#include "asmdefs.h"

#if CONF_WITH_BAS_MEMORY_MAP
# define __MBAR    0xff000000
# define __RAMBAR0 0xff100000
# define __RAMBAR1 0xff101000
#else
/* dBUG memory map */
# define __MBAR    0x10000000
# define __RAMBAR0 0x20000000
# define __RAMBAR1 0x20001000
#endif

        .globl  coldfire_startup
coldfire_startup:
        lea     __RAMBAR1+0x00001000,sp

#if defined(MACHINE_M548X) && !CONF_WITH_BAS_MEMORY_MAP
        // ACR0: Read-only cache data accesses to 2 MB memory region at address 0x00e00000
        move.l  #0x00e1c424,d0
        movec   d0,ACR0

        // ACR1: Cache data accesses to 64 MB memory region at address 0x00000000
        move.l  #0x0003c020,d0
        movec   d0,ACR1

        // ACR2: Cache instruction accesses to 64 MB memory region at address 0x00000000
        move.l  #0x0003c020,d0
        movec   d0,ACR2

        // ACR3: Region disabled
        move.l  #0x00000000,d0
        movec   d0,ACR3

        // Data Cache enabled (CACR[DEC]=1)
        // Data Store Buffer enabled (CACR[DESB]=1)
        // Data Cache invalidated (CACR[DCINVA]=1)
        // Branch Cache enabled (CACR[BEC]=1)
        // Instruction Cache enabled (CACR[IEC]=1)
        // Instruction Cache invalidated (CACR[ICINVA]=1)
        // Default Instruction Supervisor Protection enabled (CACR[IDSP]=1)
        // USP enabled (CACR[EUSP]=1)
        move.l  #0xa58885a0,d0
        movec   d0,CACR
#endif

        pea     (a6)            // Return address
        jmp     _coldfire_early_init

#if CONF_COLDFIRE_TIMER_C
        .extern _int_timerc

/* GPT1 interrupt */
        .globl _coldfire_int_61
_coldfire_int_61:
        lea     -16(sp),sp
        movem.l d0-d1/a0-a1,(sp)

// Call vector_5ms.
// As it will return with RTE, we must setup a proper stack frame
        pea     coldfire_int_61_ack(pc) // Return address
        move.w  sr,d0
        move.w  d0,-(sp)                // SR
        move.w  #0x40B4,-(sp)           // Format/Vector word

        .extern _vector_5ms
        move.l  _vector_5ms,a0
        jmp     (a0)                    // Call the 200 Hz system timer

coldfire_int_61_ack:
        /* Acknowledge */
        moveq   #8,d0                   // MCF_GPT_GSR_TEXP
        move.l  d0,__MBAR+0x81c         // MCF_GPT_GSR1
        nop                             // Avoid spurious interrupts ???

        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
        rte
#endif

#if CONF_SERIAL_CONSOLE
/* PSC0 interrupt (ColdFire RS-232 port) */
        .globl _coldfire_int_35
_coldfire_int_35:
        lea     -16(sp),sp
        movem.l d0-d1/a0-a1,(sp)

        jbsr    _coldfire_rs232_interrupt_handler

        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
        rte
#endif

#if CONF_WITH_FLEXCAN
/* FlexCAN message buffer interrupt (Eiffel IKBD) */
        .globl _coldfire_int_57
_coldfire_int_57:
        lea     -16(sp),sp
        movem.l d0-d1/a0-a1,(sp)

        jbsr    _coldfire_flexcan_message_buffer_interrupt

        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
        rte
#endif
