/*
 * parport.c - limited parallel port support
 *
 * Copyright (c) 2002 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your 
 * option any later version.  See doc/licence.txt for details.
 */

#include "portab.h"
#include "parport.h"
#include "asm.h"
#include "sound.h"
#include "mfp.h"
#include "psg.h"

/*
 * known differences with respect to the original TOS:
 * - Setprt() not implemented. One cannot access the serial port through
 *   the device 0 BIOS functions.
 * - printer configuration and hardcopy not done
 * - no input
 * - no 30 seconds delay for printer output.
 */

void parport_init(void)
{
    /* set Strobe high */
    ongibit(0x20);
}

LONG bconstat0(void)
{
    /* input not implemented */
    return 0;  /* no char available */
}

LONG bconin0(void)
{
    /* not implemented */
    return 0;
}

LONG bcostat0(void)
{
    MFP *mfp=MFP_BASE;

    if(mfp->gpip & 1) {
        return -1;  /* busy high: printer not available */
    } else {
        return 0;
    }
}

void bconout0(WORD dev, WORD c)
{
    WORD old_sr;
    WORD a;

    if(bcostat0()) {
        /* disable interrupts */
        old_sr = set_sr(0x2700);
        /* read PSG multi-function register */
        a = giaccess(PSG_MULTI | GIACCESS_READ, 0);
        /* set port B to output mode */
        a |= 0x80;
        /* write new value in register */
        giaccess(PSG_MULTI | GIACCESS_WRITE, a);
        /* write char in port B */
        giaccess(PSG_PORT_B | GIACCESS_WRITE, c);
        /* set Strobe low */
        offgibit(~0x20);
        /* delay ? */
                
        /* Strobe high */
        ongibit(0x20);
        /* restore sr */
        set_sr(old_sr);
    } else {
        /* the TOS does wait until the printer is available... 
         * We simply cancel here. 
         */
    }
}


