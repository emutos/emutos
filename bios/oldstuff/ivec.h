/*
 *  ivec.h - interrupt vector info for vme-10
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
**  warning: this file is also used as input to assembler files which are
**	fed to the pre processor.  be careful what you write.
**	for example: the numbers below are in decimal, so that asm68 won't
**	gag on them.
*/


/*
**  vector numbers
*/

#define	KBDVECNO	0x46

#if IMPLEMENTED
#define	SIOVECNO	69
				/*  0x045		*/
#define	TIMVECNO	76
				/*  0x04c		*/
#endif

/*
**  vector addresses
*/

#define	KBDVADDR	(KBDVECNO*4)

#if IMPLEMENTED
#define	SIOVADDR	(SIOVECNO*4)
#define	TIMVADDR	(TIMVECNO*4)
#endif
