/*
 * intmath.c - misc integer math routines
 *
 * Copyright (c) 2002 by EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



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

WORD mul_div(WORD m1, UWORD m2, WORD d1);

