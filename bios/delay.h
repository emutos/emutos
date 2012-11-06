/*
 * delay.h - header for delay.c
 *
 * Copyright (c) 2012 EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DELAY_H
#define _DELAY_H

#include "portab.h"

/*
 * this is the value to pass to the inline function delay_loop()
 * to get a delay of 1 millisecond.  other delays may be obtained
 * by multiplying or dividing as appropriate.  when calculating
 * shorter delays, rounding up is not necessary: because of the
 * instructions used in the loop (see asm.h), the number of loops
 * executed is one more than this count (iff count >= 0).
 */
extern ULONG loopcount_1_msec;

/*
 * function prototypes
 */
void init_delay(void);
void calibrate_delay(void);

#endif  /* _DELAY_H */
