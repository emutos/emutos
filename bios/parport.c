/*
 * parport.c - limited parallel port support
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
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
#include "delay.h"
#include "sound.h"
#include "mfp.h"
#include "psg.h"
#endif

/*
 * known differences with respect to the original TOS:
 * - printer configuration and hardcopy not done
 * - no input
 * - no 30 seconds delay for printer output.
 */

#if CONF_WITH_PRINTER_PORT
/* timing stuff */
#define DELAY_1US       delay_loop(delay1us)
static ULONG delay1us;
static WORD printer_config;
#endif

#if CONF_WITH_PRINTER_PORT
/*
 * implements xbios_21 - Set/get the desktop printer configuration word
 */
WORD setprt(WORD config)
{
    WORD old_config = printer_config;

    if (config != -1)
        printer_config = config;

    return old_config;
}
#endif

void parport_init(void)
{
#if CONF_WITH_PRINTER_PORT
    /* set Strobe high */
    ongibit(0x20);

    /* initialize delay */
    delay1us = loopcount_1_msec / 1000;

    /* initialize other printer variables */
    printer_config = 0;     /* Setprt() default: output via parallel port */ 
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
        a |= PSG_PORTB_OUTPUT;
        /* write new value in register */
        giaccess(a, PSG_MULTI | GIACCESS_WRITE);
        /* write char in port B */
        giaccess(c, PSG_PORT_B | GIACCESS_WRITE);

        /*
         * according to the Centronics spec, we must leave valid data on
         * the lines for at least 500ns before pulsing the strobe line,
         * so we leave it for 1 microsecond
         */
        DELAY_1US;

        /*
         * according to the spec, the strobe line must be pulsed low
         * for a minimum of 500ns, so we do it for 1 microsecond
         */
        offgibit(~0x20);
        DELAY_1US;
        ongibit(0x20);

        /*
         * at this point, we should wait for ACK to go low, but
         * most drivers don't bother
         */

        /* restore sr */
        set_sr(old_sr);
        return 1L;
#endif
    } else {
        /* Atari TOS waits until the printer is available...
         * We simply cancel here.
         */
    }
    return 0L;
}
