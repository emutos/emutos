/*
 *  kbq.h - keyboard queue header
 * ...
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
**  KBQENTRY - type of data in a kbq.  
**	This is a word, with the hi byte being the status value, and the
**	low byte being the actual scan code.
*/

typedef	UWORD	KBQENTRY ;

/*
**  KBSCNCODE - turns a kbq entry into a scan code
*/

#define	KBSCNCODE(x)	(  (UBYTE)(x & 0x00ff)  )

/*
**  KBSTATUS - turns a kbq entry into the keyboard status assoc'd with the
**	scan code
*/

#define	KBSTATUS(x)	(  (UBYTE)( (x<<8) & 0x00ff )  )


/*
 * Prototypes
 */
BOOLEAN	kbq_add(UWORD item );     /*  item to put in queue	*/
BOOLEAN	kbq_del(UWORD * itemp );  /*  ptr to where to put item	*/
VOID	kbq_init(VOID);
