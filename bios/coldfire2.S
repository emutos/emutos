/*
 * coldfire2.S - ColdFire specific assembler functions
 *
 * Copyright (c) 2013 The EmuTOS development team
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

#ifdef MACHINE_M548X
# define __MBAR    0x10000000
# define __RAMBAR0 0x20000000
# define __RAMBAR1 0x20001000
#elif defined(MACHINE_FIREBEE)
# define __MBAR    0xff000000
# define __RAMBAR0 0xff100000
# define __RAMBAR1 0xff101000
#else
# error Unknown ColdFire Machine
#endif

        .globl  coldfire_startup
coldfire_startup:
        lea     __RAMBAR1+0x00001000,sp

#ifdef MACHINE_M548X
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

        jbsr     _int_timerc

        /* Acknowledge */
        moveq   #8,d0                   // MCF_GPT_GSR_TEXP
        move.l  d0,__MBAR+0x81c         // MCF_GPT_GSR1
        nop                             // Avoid spurious interrupts ???
        
        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
        rte
#endif
