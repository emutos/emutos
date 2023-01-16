/*
 * delay.c - initialise values used to provide microsecond-order delays
 *
 * note that the timings are quite imprecise (but conservative) unless
 * you are running on at least a 32MHz 68030 processor
 *
 * Copyright (C) 2013-2022 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "emutos.h"
#include "biosdefs.h"
#include "mfp.h"
#include "serport.h"
#include "biosext.h" /* for is_apollo_68080 */
#include "processor.h"
#include "delay.h"
#include "coldfire.h" /* For cookie jar info. */

/*
 * initial 1 millisecond delay loop values
 */
#define LOOPS_68080         38125   /* Apollo 68080 timing measured on Amiga */
#define LOOPS_68060         110000  /* 68060 timing assumes 110MHz for safety */
#define LOOPS_68030         3800    /* 68030 timing assumes 32MHz */
#define LOOPS_68000         760     /* 68000 timing assumes 16MHz */

#define CALIBRATION_TIME    100     /* target # millisecs to run calibration */

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
#if defined(MACHINE_FIREBEE) || defined(MACHINE_M548X)
    /*
      For coldfire, we don't know cookie_mcf.sysbus_frequency at this point.
      We know it will be between 100 and 133 MHz. Since also at this point
      it is okay for the loop time to be approximate, we just use 133 MHz
      here and set the correct value later in calibrate_delay() below.
    */

    loopcount_1_msec = 133UL * 1000;
#else
# if CONF_WITH_APOLLO_68080
    if (is_apollo_68080)
        loopcount_1_msec = LOOPS_68080;
    else
# endif
    {
        switch((int)mcpu) {
        case 60:
            loopcount_1_msec = LOOPS_68060;
            break;
        case 40:
        case 30:            /* assumes 68030 */
            loopcount_1_msec = LOOPS_68030;
            break;
        default:            /* assumes 68000 */
            loopcount_1_msec = LOOPS_68000;
        }
    }
#endif
}

/*
 * calibrate delay values: must only be called *after* interrupts are allowed
 *
 * NOTE1: we use TimerD so we restore the RS232 stuff
 * NOTE2: some systems (e.g. ARAnyM) do not implement TimerD; we leave
 *        the default delay values as-is in this case
 * NOTE3: ColdFire systems are not calibrated, since there is no
 *        independent clock that can be used to measure time
 */
void calibrate_delay(void)
{
#if CONF_WITH_MFP
    ULONG loopcount, intcount;

    /*
     * disable interrupts then run the calibration
     */
    jdisint(MFP_TIMERD);
    loopcount = CALIBRATION_TIME * loopcount_1_msec;
    intcount = run_calibration(loopcount);

    /*
     * disable interrupts then restore the RS232
     * serial port stuff (in case we're using it)
     */
    jdisint(MFP_TIMERD);
    rsconf1(DEFAULT_BAUDRATE, 0, 0x88, 1, 1, 0);   /* just like init_serport() */

    /*
     * intcount is the number of interrupts that occur during 'loopcount'
     * loops.  an interrupt occurs every 1/960 sec (see delayasm.S).
     * so the number of loops per second = loopcount/(intcount/960).
     * so, loops per millisecond = (loopcount*960)/(intcount*1000)
     * = (loopcount*24)/(intcount*25).
     */
    if (intcount)       /* check for valid */
        loopcount_1_msec = (loopcount * 24) / (intcount * 25);
#else
  #if defined(MACHINE_FIREBEE) || defined(MACHINE_M548X)
    loopcount_1_msec = cookie_mcf.sysbus_frequency * 1000;
  #endif
#endif
}
