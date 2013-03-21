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

#ifdef MACHINE_FIREBEE
# define __MBAR    0xff000000
# define __RAMBAR0 0xff100000
# define __RAMBAR1 0xff101000
#else
# error Unknown ColdFire Machine
#endif

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
