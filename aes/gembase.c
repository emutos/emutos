/*	GEMBASE.C	1/28/84	- 01/07/85	Lee Jay Lorenzen	*/
/*	merge High C vers. w. 2.2 & 3.0		8/19/87		mdf	*/ 

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <portab.h>
#include <machine.h>
#include <struct.h>

GLOBAL PD	*rlr, *drl, *nrl;
GLOBAL EVB	*eul, *dlr, *zlr;

#if I8086
GLOBAL UWORD	elinkoff;
#else
GLOBAL LONG	elinkoff;
#endif
GLOBAL BYTE	indisp;

GLOBAL WORD	fpt, fph, fpcnt;		/* forkq tail, head, 	*/
						/*   count		*/
GLOBAL SPB	wind_spb;
GLOBAL WORD	curpid;


