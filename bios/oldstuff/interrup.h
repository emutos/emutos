/*
 * interrup.h - Header file for interrupt handlers
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  SCC     Steve C. Cavender
 *  KTB     Karl T. Braun (kral)
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



EXTERN PFI charvec[];

/*
**  definition of bits in flag word parm to interrupt handlers
**	FLG()  will take the bit number being defined and turn it into
**	a mask for the flag.
*/

#define	FLG(x)	(  (LONG) ( 1 << x )  )
	

#define	IF_RPKT		FLG(00)	/*  packet received			*/
				/*  if this is 1, then the parameter blk*/
				/*    contains:				*/
				/*      for con:, aux:, or prn:		*/
				/*	  the LONG character info that	*/
				/*	  BIOS function 0x?? would have	*/
				/*	  returned.			*/
				/*	for clock:			*/
				/*	  a WORD number of milliseconds	*/
				/*	  since last tick		*/
				/*	for mouse:			*/
				/*	  a BYTE of button status	*/
				/*	  a BYTE of delta-x		*/
				/*	  a BYTE of delta-y		*/

#define	IF_ERROR	FLG(01)	/*  If no other error status bit is on,	*/
				/*  this is just an error that couldn't	*/
				/*  be described any better with the 	*/
				/*  available defined status bits	*/

#define	IF_PAPER	FLG(02)	/*  1 = out of paper			*/

#define	IF_OFFLINE	FLG(03)	/*  1 = device off line			*/

#define	IF_TIMEOUT	FLG(04)	/*  1 = device has timed out		*/

#define	IF_FRAME	FLG(05)	/*  1 = framing error 			*/

#define	IF_PARITY	FLG(06)	/*  1 = parity error			*/

#define IF_OVERRUN	FLG(07)	/*  1 = receiver overrun error		*/

	/*
	**  if no packet received and no error then state changed on one
	**  of the following
	*/

#define	IF_DCD		FLG(16)	/*  1 = dcd is present			*/
#define	IF_CTS		FLG(17)	/*  1 = cts is present			*/

