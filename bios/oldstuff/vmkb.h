/*
 *	vmkb.h - Concurrent 4.0 driver header file
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*	mapphys parameter block */
typedef struct
{
	LONG	mpzero;
	BYTE	*pbegaddr;
	LONG	plength;
} MAPPB;
/*	mapphys memory type */
#define MAPCODE	0	/* physical memory contains code	*/
#define MAPDATA	1	/* physical memory contains data	*/

/*
 *	Parameter block used to call the supif supervisor
 *	timer function.  Used in the keyboard driver.
 */

struct timeblk
{
	BYTE	t_async;
	BYTE	t_zero;
	WORD	t_flags;
	LONG	t_swi;
	LONG	t_time;
};

#define F_TIMER	24	/* supif function number for TIMER function */


