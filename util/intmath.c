/*
 * intmath.c - simple implementation of <string.h> ANSI routines
 *
 * Copyright (C) 2002-2013 by EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "intmath.h"


/*
 * Integer squareroot
 *  y=Isqrt(x) is equivalent to: y = (ULONG)(sqrtl( (long double)x ) + 0.5);
 */

ULONG Isqrt(ULONG x)
{
    ULONG s1, s2;

    if (x < 2)
        return x;

    s1 = x--; /* This algorithm converges to (x+LSB)^1/2. Thus decrement x */
    s2 = 2;
    do {
        s1 /= 2;
        s2 *= 2;
    } while (s1 > s2);

    s1 = s2 = (s1 + (s2 / 2)) / 2;
    if (!((s1 &= ~7) & ~63))
        s1 = s2; /* Improve first estimate further */

    s2 = (1 + x/s1 + s1) / 2;
    if (s1 == s2) /* First iteration */
        return s2;

    do {
        s1 = s2;
        s2 = (1 + x/s1 + s1) / 2;
    } while (s1 > s2);

    return s2;
}
