/*
 * fshand.c - file handle routines for the file system
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  xxx <xxx@xxx>
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include	"portab.h"
#include	"fs.h"
#include	"bios.h"		/*  M01.01.01			*/
#include	"gemerror.h"

/* 
 * forward prototypes
 */


/*
**  xforce -
**
**	Function 0x46	f_force
**
**	Error returns
**		EIHNDL
**
**	Last modified	SCC	5 Apr 85
*/

long	xforce(int std, int h)
{
	return(ixforce(std,h,run));
}

/*
 *  ixforce - force a std handle to a non-std handle.
 *	if the std handle is for an open non-char device, close it
 *
 * std must	be a standard handle
 * h   must NOT be a standard handle	
 */

long	ixforce(int std, int h, PD *p)	
{
	long fh;

	if ((std < 0) || (std >= NUMSTD))
		return(EIHNDL);
	
	if( p->p_uft[std] > 0 )
		xclose( std ) ;

	if (h < 0)
		p->p_uft[std] = h;
	else
	{
		if (h < NUMSTD)
			return(EIHNDL);

		if ((fh = (long) sft[h-NUMSTD].f_ofd) < 0L)
			p->p_uft[std] = fh;
		else
		{
			p->p_uft[std] = h;
			sft[h-NUMSTD].f_use++;
		}
	}
	return(E_OK);
}



/*
**  syshnd -
*/

int syshnd(int h)
{
	if (h >= NUMSTD)
		return(h-NUMSTD);

	if ((h = run->p_uft[h]) > 0 )
		return(h-NUMSTD);

	return(h);
}



/*
 *  ixdirdup -
 *
 * h: file handle
 * dn: directory number
 */

void ixdirdup(int h, int dn, PD *p)
{
	p->p_curdir[h] = dn;
	diruse[dn]++;
}


/*
**  dup -
**	duplicate a file handle.
**
**	Function 0x45	f_dup
**
**	Error returns
**		EIHNDL
**		ENHNDL
**
**	Last modified	SCC	 5 Apr 85
*/

/*+ h must be a standard handle (checked) */

long	dup(long h)	
{
	register int i;

	if ((h<0) || (h >= NUMSTD))
		return(EIHNDL); 	/* only dup standard */

	for (i = 0; i < OPNFILES; i++)	/* find the first free handle */
		if (!sft[i].f_own)
			break;

	if (i == OPNFILES)
		return(ENHNDL); 	/* no free handles */

	sft[i].f_own = run;

	if ((h = run->p_uft[h]) > 0)
		sft[i].f_ofd = sft[h-NUMSTD].f_ofd;
	else
		sft[i].f_ofd = (OFD *) h;

	sft[i].f_use = 1;

	return(i+NUMSTD);
}


