/*
 * delay.c - initialise values used to provide microsecond-order delays
 *
 * note that the timings are quite imprecise (but conservative) unless
 * you are running on at least a 32MHz 68030 processor
 *
 * Copyright (c) 2013 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "config.h"
#include "portab.h"
#include "mfp.h"
#include "serport.h"
#include "processor.h"
#include "delay.h"

/*
 * initial 1 millisecond delay loop values
 */
#define LOOPS_FIREBEE       131000
#define LOOPS_68060         110000  /* 68060 timing assumes 110MHz for safety */
#define LOOPS_68030         3800    /* 68030 timing assumes 32MHz */
#define LOOPS_68000         760     /* 68000 timing assumes 16MHz */

#define CALIBRATION_TIME    100     /* target # millisecs to run calibration */

#define TIMERD_INTNUM       4       /* for jdisint() etc */

/*
 * global variables
 */
ULONG loopcount_1_msec;

/*
 * function prototypes (functions in delayasm.S)
 */
ULONG run_calibration(ULONG loopcount);
void calibration_timer(void);

/*
 * initialise delay values
 *
 * NOTE: this is called before interrupts are allowed, so initialises
 * the delay values based on processor type.  the main reason for having
 * an early init is to be able to use the Falcon SCC early for debugging
 * purposes.
 */
void init_delay(void)
{
#ifdef __mcoldfire__
    loopcount_1_msec = LOOPS_FIREBEE;
#else
    switch((int)mcpu) {
    case 60:
        loopcount_1_msec = LOOPS_68060;
        break;
    case 40:
    case 30:                /* assumes 68030 */
        loopcount_1_msec = LOOPS_68030;
        break;
    default:                /* assumes 68000 */
        loopcount_1_msec = LOOPS_68000;
    }
#endif
}

/*
 * calibrate delay values: must only be called *after* interrupts are allowed
 *
 * NOTE1: we use TimerD so we restore the RS232 stuff
 * NOTE2: some systems (e.g. ARAnyM) do not implement TimerD; we leave
 *        the default delay values as-is in this case
 */
void calibrate_delay(void)
{
#if CONF_WITH_MFP
    ULONG loopcount, intcount;

    /*
     * disable interrupts then run the calibration
     */
    jdisint(TIMERD_INTNUM);
    loopcount = CALIBRATION_TIME * loopcount_1_msec;
    intcount = run_calibration(loopcount);

    /*
     * disable interrupts then restore the RS232
     * serial port stuff (in case we're using it)
     */
    jdisint(TIMERD_INTNUM);
    rsconf1(B9600, 0, 0x88, 1, 1, 0);   /* just like init_serport() */

    /*
     * intcount is the number of interrupts that occur during 'loopcount'
     * loops.  an interrupt occurs every 1/960 sec (see delayasm.S).
     * so the number of loops per second = loopcount/(intcount/960).
     * so, loops per millisecond = (loopcount*960)/(intcount*1000)
     * = (loopcount*24)/(intcount*25).
     */
    if (intcount)       /* check for valid */
        loopcount_1_msec = (loopcount * 24) / (intcount * 25);
#endif
}
