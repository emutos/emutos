/*
 * intmath.c - simple implementation of <string.h> ANSI routines
 *
 * Copyright (c) 2002 by EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



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
    ((s1 &= ~7) & ~63) ? : (s1 = s2); /* Improve first estimate further */

    s2 = (1 + x/s1 + s1) / 2;
    if (s1 == s2) /* First iteration */
        return s2;

    do {
        s1 = s2;
        s2 = (1 + x/s1 + s1) / 2;
    } while (s1 > s2);

    return s2;
}


/*
 * mul_div - signed integer multiply and divide
 *
 * mul_div (m1,m2,d1)
 * 
 * ( ( m1 * m2 ) / d1 ) + 1/2
 *
 * m1 = signed 16 bit integer
 * m2 = unsigned 15 bit integer
 * d1 = signed 16 bit integer
 */

WORD mul_div(WORD m1, UWORD m2, WORD d1)
{
    return (WORD)(((WORD)(m1)*(LONG)((WORD)(m2))) / (WORD)d1);
}
