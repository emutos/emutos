/*
 * fsglob.c - global variables for the file system		  
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  xxx <xxx@xxx>
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
**  Mod #	   who date		modification
**  -------------- --- ---------	------------
**  M01.01.1023.02 scc 10/23/86 	Changed the definition of time and date
**					to unsigned int
**
*/

#include	"gportab.h"
#include	"fs.h"
#include	"bios.h"		/*  M01.01.01			*/
#include	"gemerror.h"

/*
**  drvtbl -
*/

GLOBAL	DMD *drvtbl[16];


/*
**  logmsk -
**	log values of:
**	  1, 2, 4, 8, 16, 32, 64, 128  256, 512, 1024, 2048 ... 
*/

GLOBAL	int logmsk[] = {  0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047,
		 4095, 8191, 16383, 32767 } ;


/*
**  sft -
*/

GLOBAL	FTAB sft[OPNFILES];


/*
**  rwerr -  hard error number currently in progress 
*/

GLOBAL	long rwerr; 

/*
**  errdrv -  drive on which error occurred 
*/

GLOBAL	int errdrv; 


/*
**  time - , date - who knows why this is here?
*/

GLOBAL	unsigned int	time, date ;


