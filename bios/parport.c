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

#include "config.h"
#include "portab.h"
#include "parport.h"
#if CONF_WITH_PRINTER_PORT
#include "asm.h"
#include "sound.h"
#include "mfp.h"
#include "psg.h"
#endif

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
#if CONF_WITH_PRINTER_PORT
    /* set Strobe high */
    ongibit(0x20);
#endif
}

LONG bconin0(void)
{
    /* not implemented */
    return 0;
}

LONG bcostat0(void)
{
#if CONF_WITH_PRINTER_PORT
    MFP *mfp=MFP_BASE;

    if(mfp->gpip & 1) {
        return 0;   /* busy high: printer not available */
    } else {
        return -1;
    }
#else
    return 0; /* output not allowed */
#endif
}

LONG bconout0(WORD dev, WORD c)
{
    if(bcostat0()) {
#if CONF_WITH_PRINTER_PORT
        WORD old_sr;
        WORD a;

        /* disable interrupts */
        old_sr = set_sr(0x2700);
        /* read PSG multi-function register */
        a = giaccess(0, PSG_MULTI | GIACCESS_READ);
        /* set port B to output mode */
        a |= 0x80;
        /* write new value in register */
        giaccess(a, PSG_MULTI | GIACCESS_WRITE);
        /* write char in port B */
        giaccess(c, PSG_PORT_B | GIACCESS_WRITE);
        /* set Strobe low */
        offgibit(~0x20);
        /* delay ? */

        /* Strobe high */
        ongibit(0x20);
        /* restore sr */
        set_sr(old_sr);
        return 1L;
#endif
    } else {
        /* the TOS does wait until the printer is available...
         * We simply cancel here.
         */
    }
    return 0L;
}


