/*
 * bdosinit.c - 'front end' to bdos.  This is OEM definable module
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  xxx <xxx@xxx>
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "portab.h"
#include "bios.h"



/*
 *  bufl - buffer lists
 *	two lists:  fat,dir / data
 *	these lists should be initialized by the bios.
 *	(in bios.c)
 */

extern BCB *bufl[];


extern		void _osinit();


/*
 *  osinit - the bios calls this routine to initialize the os
 */

void osinit(void)
{
    _osinit(); /*  real routine in rwa.s */

}


