/*
 *  io.h - header file for i/o drivers
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  SCC     Steve C. Cavender
 *  KTB     Karl T. Braun (kral)
 *  JSL     Jason S. Loveman
 *  EWF     Eric W. Fleischman
 *  LTG     Louis T. Garavaglia
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
**  return codes
*/

#define	DEVREADY	-1L		/*  device ready		*/
#define	DEVNOTREADY	0L		/*  device not ready		*/
#define	MEDIANOCHANGE	0L		/*  media def has not changed	*/
#define	MEDIAMAYCHANGE	1L		/*  media may have changed	*/
#define	MEDIACHANGE	2L		/*  media def has changed	*/

/*
**  typedefs
*/

#define	ISR	int
#define	ECODE	LONG

typedef	int	(*PFI)() ;	/*  straight from K & R, pg 141		*/

/*
**  code macros
*/

#define	ADDRESS_OF(x)	x
#define	INP		inp
#define	OUTP		outp

/*
**  externs
*/

EXTERN	BYTE	INP() ;
EXTERN	VOID	OUTP() ;

