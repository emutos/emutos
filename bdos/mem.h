/*
 * mem.h - header file for memory and process management routines  
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
 *  conditional compile switches
 */

#define OSMPANIC	FALSE
#define OSMLIST 	FALSE


/*
 *  externals
 */

extern	FTAB	sft[] ;
extern	long	errbuf[3] ;			/*  sup.c  */
extern	MPB	pmd ;
extern	int	osmem[] ;
extern	int	osmlen ;
extern	int	*root[20];
extern	int	osmptr;

extern	long	trap13() ;
extern	long	xmalloc() ;
extern	long	xsetblk() ;
extern	MD	*ffit() ;

/*
 *  process management
 */

extern	long	bakbuf[] ;
extern	WORD	supstk[] ;
extern	PD	*run;

extern	long	xexec(WORD, char *, char *, char *) ;
extern	void	xterm() ;
extern	void	x0term() ;

