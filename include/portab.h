/*
 * portab.h - Definitions for writing portable C
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
 * LVL - 28 Nov 2001, commented out most of it.
 * this file is used by bdos and bios, NOT vdi or AES yet.
 */

#ifndef _PORTAB_H
#define _PORTAB_H

/*
 *  17 Jan 86 - ktb
 *	Starting to convert this to 'gportab.h', which will have env defines
 *	in it as well.
 *
 */


/*
**  Independant Constants 
**	These Constants never [1] change over different systems, and make
**	it a little easier to define some of the basic stuff below
**
**	[1] - 'never' implies 'almost never'; remember Capt. Murhpy!
*/

#define FAILURE (-1)			/*	Function failure return val */
#define SUCCESS (0)			/*	Function success return val */
/* unused #define YES	(1)		   	"TRUE"			    */
/* unused #define NO	(0)			"FALSE" 		    */
/* unused #define FOREVER for(;;) 	       	Infinite loop declaration   */
#define NULL	0			/*	Null character value	    */
#define EOF	(-1)			/*	EOF Value		    */
#define TRUE	(1)			/*	Function TRUE  value	    */
#define FALSE	(0)			/*	Function FALSE value	    */

/*
**  Compiler Definitions
**
**	There are two definitions for Alcyon, in hopes that someday they will
**	fix the unsigned char problem.	ALCYON1 doesn't.
**
*/

#ifdef __GNUC__
#define NORETURN __attribute__ ((noreturn))
#else
#define NORETURN
#endif

/* unused 
 * #define ALCYON1 	1
 * #define ALCYON2 	2
 * #define LATTICE 	5
 *
 * #define COMPILER	ALCYON1
 */

/*
**	The constant "ALCYON" (without numeric suffix), implies 'any of the
**	Alcyon compilers and might be used as in "#IF ALCYON"
*/	

/* 
 * unused
 * #if	COMPILER == ALCYON1 
 * #define ALCYON		TRUE
 * #endif
 *
 * #if	COMPILER == ALCYON2
 * #define ALCYON		TRUE
 * #endif
 *
 * #ifndef ALCYON
 * #define ALCYON		FALSE
 * #endif
 */

/*
**  extended data types
*/


#define REG		register		/* register variable	   */
/* unused #define LOCAL		auto		   Local var on 68000	   */
/* unused #define MLOCAL	static	       	   Local to module	   */
/* unused #define GLOBAL		       	   Global variable	   */
/* unused #define EXTERN        extern             Extern variable         */

typedef char		BYTE ;			/*  Signed byte 	*/

/*****/
/*
 * #if	COMPILER == ALCYON1
 *					*  unsgnd not supp'd	*
 * typedef char		UBYTE ; 		*  Unsigned byte	*
 * typedef long		ULONG ; 		*  Unsigned long	*
 * #else
 */
typedef unsigned char	UBYTE ; 		/*  Unsigned byte	*/
typedef unsigned long	ULONG ; 		/*  unsigned 32 bit word*/
/* #endif */

typedef long		PTR ;			/*  32 bit pointer */

typedef int		BOOLEAN ;		/*  boolean		*/
typedef int		BOOL ;			/*  same as boolean	*/
typedef short int	WORD ;			/*  signed 16 bit word	*/
typedef unsigned short int UWORD ;		/*  unsigned 16 bit word*/
typedef long		LONG ;			/*  signed 32 bit word	*/
/* typedef void		VOID ;			    returns no value	*/
/* typedef int		DEFAULT ;		    return def value	*/
/* typedef float       	FLOAT ; 		    floating point	*/
/* typedef double      	DOUBLE ;		    double precision	*/


typedef long		ERROR ; 		/*  error codes 	*/
/* typedef int		(*PFI)() ;		    ptr to func ret int */
/* typedef ERROR       	(*PFE)() ;		    ptr to func ret err */
/* typedef LONG		(*PFL)() ;		    ptr to func ret long*/

/****************************************************************************/
/*	Miscellaneous Definitions:					    */
/****************************************************************************/
#define NULLPTR (char *) 0		/*	Null pointer value	    */
#define STDIN	 0			/*	Standard Input		    */
#define STDOUT	 1			/*	Standard Output 	    */
#define STDERR	 2			/*	Standard Error		    */


/****************************************************************************/
/****************************************************************************/

#endif /* _PORTAB_H */

