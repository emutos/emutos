/*
 * kportab.h - run time header file
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
 *	This is an include file for assisting the user to write portable
 *	programs for C.  All processor dependencies should be located here.
 */

/*
 *	Standard type definitions
 */

/*
**	machine dependent 
*/

#define	WORD	int



/*
**	general
*/

#define	BYTE	char				/* Signed byte		   */
#define UBYTE	char				/* Unsigned byte 	   */
#define BOOLEAN	int				/* 2 valued (true/false)   */

#define	LONG	long				/* signed long (32 bits)   */
#define	ULONG	long				/* Unsigned long	   */

#define	REG	register			/* register variable	   */
#define	LOCAL	auto				/* Local var on 68000	   */
#define	EXTERN	extern				/* External variable	   */
#define	MLOCAL	static				/* Local to module	   */
#define	GLOBAL	/**/				/* Global variable	   */
#define	VOID	int				/* Void function return	   */
#define	DEFAULT	int				/* Default size		   */
#define FLOAT	float				/* Floating Point	   */
#define DOUBLE	double				/* Double precision	   */


/****************************************************************************/
/*	Miscellaneous Definitions:					    */
/****************************************************************************/

#define	YES	1			/*	"TRUE"			    */
#define	NO	0			/*	"FALSE"			    */
#define	FOREVER	for(;;)			/*	Infinite loop declaration   */
#define	NULL	'\0'			/*	Null character value	    */
#define NULLPTR (char *) 0		/*	Null pointer value	    */
#define	TRUE	(1)			/*	Function TRUE  value	    */
#define	FALSE	(0)			/*	Function FALSE value	    */
#ifndef	EOF
#define	EOF	(-1)			/*	EOF Value		    */
#endif

#define UBWORD(x) (UWORD)x

/****************************************************************************/
/****************************************************************************/
