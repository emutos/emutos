/*
 * portab.h -  include file for to write portable programs for C
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright (c) 1999 Caldera, Inc. and Authors:
 *
 *  SCC
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#define mc68k 0

#define UCHARA 1		/* if char is unsigned     */
/*
 *	Standard type definitions
 */
#define	BYTE	char		/* Signed byte         */
#define BOOLEAN	int		/* 2 valued (true/false)   */
#define	WORD	int		/* Signed word (16 bits)   */
#define	UWORD	unsigned int	/* unsigned word       */

#define	LONG	long		/* signed long (32 bits)   */
#define	ULONG	long		/* Unsigned long       */

#define	REG	register	/* register variable       */
#define	LOCAL	auto		/* Local var on 68000      */
#define	EXTERN	extern		/* External variable       */
#define	MLOCAL	static		/* Local to module     */
#define	GLOBAL	/**/		/* Global variable     */
#define	VOID	/**/		/* Void function return    */
#define	DEFAULT	int		/* Default size        */

#ifdef UCHARA
#define UBYTE	char		/* Unsigned byte       */
#else
#define	UBYTE	unsigned char	/* Unsigned byte       */
#endif

/****************************************************************************/
/*	Miscellaneous Definitions:					    */
/****************************************************************************/
#define	FAILURE	(-1)		/* Function failure return val */
#define SUCCESS	(0)		/* Function success return val */
#define	YES	1		/* "TRUE"              */
#define	NO	0		/* "FALSE"             */
#define	FOREVER	for(;;)		/* Infinite loop declaration   */
#define	NULL	0		/* Null pointer value      */
#define NULLPTR (char *) 0	/* */
#define	EOF	(-1)		/* EOF Value           */
#define	TRUE	(1)		/* Function TRUE  value        */
#define	FALSE	(0)		/* Function FALSE value        */
