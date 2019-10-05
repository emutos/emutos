/*
 * intmath.c - implementation of integer mathematical functions
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "intmath.h"


/*
 * Integer squareroot
 *  y=Isqrt(a) is equivalent to: y = (ULONG) (sqrtl( (double) a ) + 0.5);
 */
ULONG Isqrt(ULONG a)
{
    ULONG x, y;

    if (!a)
        return 0;   /* sqrt(0) = 0 */

    x = --a;        /* This algorithm converges to (a + a_LSB)^1/2. Thus decrement a. */
    y = 8;

    while(x > y)    /* Iterate for first estimate */
    {
        x /= 2;
        y *= 2;
    }

    y = (2 + x + y/2) / 4;  /* y = First estimate */

    /* Positive bias rounded Newton-Raphson square root iterator */
    while(x != y)
    {
        x = y;
        y = (1 + a/y + y) / 2;
    }

    return y;
}
