/*
 * btools.h - header file for routines which call the block oriented tools
 *
 * Copyright (c) 2001 Martin Doering
 *
 * Authors:
 *  MAD    Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */





#define STRCPY( d , s ) 	strcpy( d , s ) 	/* use std.	*/
#define STRNCPY( d , s , n )	bmove( s , d , n )
#define STRLEN( s )		blength( s , '\0' )
#define bzero( s , n )		bfill( s , 0 , n )


EXTERN	VOID	bmove() ;
EXTERN	VOID	lbmove() ;
EXTERN	VOID	bfill() ;
EXTERN	BYTE	*bsrch() ;
EXTERN	WORD	blength() ;

