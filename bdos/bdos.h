/*
 * bdos.h - common include file for bdos files
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


/*
 *  system types
 */



/*  FH - File Handle							*/
typedef int	FH ;



/*
 *  common externals
 */

EXTERN	ERROR	xopen(),	ixopen() ;
EXTERN	ERROR	xclose(),	ixclose() ;
EXTERN	ERROR	xread(),	ixread() ;
EXTERN	ERROR	xwrite(),	ixwrite() ;
EXTERN	ERROR	xlseek(),	ixlseek() ;

