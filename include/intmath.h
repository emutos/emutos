/*
 * intmath.c - misc integer math routines
 *
 * Copyright (c) 2002, 2012 by EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

ULONG Isqrt(ULONG x);

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

/*
 * mul_div - signed integer multiply and divide
 * return ( m1 * m2 ) / d1
 * While the operands are WORD, the intermediate result is LONG.
 */
static inline WORD mul_div(WORD m1, WORD m2, WORD d1)
{
    __asm__ (
      "muls %1,%0\n\t"
      "divs %2,%0"
    : "+d"(m1)
    : "idm"(m2), "idm"(d1)
    );

    return m1;
}
