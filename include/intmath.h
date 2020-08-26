/*
 * intmath.h - misc integer math routines
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

ULONG Isqrt(ULONG x);

/*
 *  min(): return minimum of two values
 *  Implemented as a macro to allow any type
 */
#define min(a,b) \
({ \
    __typeof__(a) _a = (a); /* Copy to avoid double evaluation */ \
    __typeof__(b) _b = (b); /* Copy to avoid double evaluation */ \
    _a <= _b ? _a : _b; \
})

/*
 *  max(): return maximum of two values
 *  Implemented as a macro to allow any type
 */
#define max(a,b) \
({ \
    __typeof__(a) _a = (a); /* Copy to avoid double evaluation */ \
    __typeof__(b) _b = (b); /* Copy to avoid double evaluation */ \
    _a >= _b ? _a : _b; \
})

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
static __inline__ WORD mul_div(WORD m1, WORD m2, WORD d1)
{
    __asm__ (
      "muls %1,%0\n\t"
      "divs %2,%0"
    : "+d"(m1)
    : "idm"(m2), "idm"(d1)
    );

    return m1;
}

/*
 * umul_shift - unsigned integer multiply and divide-via-shift, with rounding
 *
 * returns (m1 * m2 + 32768) / 65536
 *
 * while the operands are UWORD, the intermediate result is ULONG.
 *
 * WARNING: the technique used presupposes that the absolute
 * value of the intermediate result is < 2**31.
 */
static __inline__ UWORD umul_shift(UWORD m1, UWORD m2)
{
    __asm__ (
      "mulu %1,%0\n\t"
      "addi.l #32768,%0\n\t"
      "swap %0"
    : "+d"(m1)
    : "idm"(m2)
    );

    return m1;
}

/*
 * muls - signed integer multiply
 *
 * multiply two signed shorts, returning a signed long
 */
static __inline__ LONG muls(WORD m1, WORD m2)
{
    LONG ret;

    __asm__ (
      "muls %2,%0"
    : "=d"(ret)
    : "%0"(m1), "idm"(m2)
    );

    return ret;
}

/*
 * divu - unsigned integer divide
 *
 * divide an unsigned long by an unsigned short, returning an unsigned short
 * (assumes that the result will always fit in an unsigned short)
 */
static __inline__ UWORD divu(ULONG d1, UWORD d2)
{
    __asm__ (
      "divu %1,%0"
    : "+d"(d1)
    : "idm"(d2)
    : "cc"
    );

    return (UWORD)d1;
}
