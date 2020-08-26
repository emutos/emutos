/*
 * parport.c - limited parallel port support
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/licence.txt for details.
 */

#include "emutos.h"
#include "parport.h"
#if CONF_WITH_PRINTER_PORT
#include "asm.h"
#include "delay.h"
#include "sound.h"
#include "mfp.h"
#include "psg.h"
#include "ikbd.h"
#include "tosvars.h"
#endif

/*
 * known differences with respect to the original TOS:
 * - printer hardcopy is not done
 * - no input
 */

#if CONF_WITH_PRINTER_PORT
/* timing stuff */
#define DELAY_1US       delay_loop(delay1us)
static ULONG delay1us;

static WORD printer_config;

/* timeout stuff */
#define CTL_C           ('C'-0x40)
#define SHORT_TIMEOUT   (5 * CLOCKS_PER_SEC)
#define LONG_TIMEOUT    (30 * CLOCKS_PER_SEC)
static ULONG last_timeout;
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

/*
 * do parallel port output
 */
static LONG prnout(WORD c)
{
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
    last_timeout = 0UL;     /* parallel port: ticks value at last timeout */
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
#if CONF_WITH_PRINTER_PORT
    ULONG now = hz_200;

    /*
     * if we didn't time out 'recently', we check if the port is available,
     * allowing a long timeout; otherwise we just time out immediately.
     * this is how Atari TOS does things.  however, waiting for the long
     * timeout because we accidentally tried to use the printer port is
     * extremely painful, so EmuTOS allows a quick out via ctrl-C.
     */
    if ((last_timeout == 0UL)                   /* first time through? */
     || (now >= (last_timeout+SHORT_TIMEOUT)))
    {
        while(hz_200 < (now+LONG_TIMEOUT))
        {
            if (bcostat0())
                return prnout(c);
            if (bconstat2())
                if ((bconin2() & 0xff) == CTL_C)
                    break;
        }
    }

    last_timeout = hz_200;
#endif
    return 0L;
}
