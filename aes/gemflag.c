/*	GEMFLAG.C	1/27/84 - 08/26/85	Lee Jay Lorenzen	*/
/*	merge High C vers. w. 2.2 & 3.0		8/20/87		mdf	*/ 

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <portab.h>
#include <machine.h>
#include <struct.h>
#include <basepage.h>
#include <obdefs.h>
#include <gemlib.h>
						/* in ASYNC88.C		*/
EXTERN VOID	evinsert();
						/* in AINTS88.C		*/
EXTERN VOID	azombie();

/* ----------- added for metaware compiler ---------- */
EXTERN VOID 	evremove();			/* in INPUT.C		*/
EXTERN  	cli();				/* in DOSIF.A86		*/
EXTERN 		sti();
EXTERN 		dsptch();			/* in ASM.A86		*/
/* -------------------------------------------------- */

EXTERN LONG	NUM_TICK;			/* number of ticks	*/
						/*   since last sample	*/
						/*   while someone was	*/
						/*   waiting		*/
EXTERN LONG	CMP_TICK;			/* indicates to tick 	*/
						/*   handler how much	*/
						/*   time to wait before*/
						/*   sending the first	*/
						/*   tchange		*/

VOID tchange(REG LONG c)			/* c=number of ticks that have gone by	*/
{
	REG EVB		*d;
	REG LONG	c1;
						/* pull pd's off the	*/
						/*   delay list that 	*/
						/*   have waited long	*/
						/*   enough		*/
	d = dlr;
	while (d)
	{
						/* take a bite out of	*/
						/*   the amount of time	*/
						/*   the pd is waiting	*/
	  c1 = c - d->e_parm;
	  d->e_parm -= c;
	  c = c1;
						/* finished waiting	*/
	  if ( d->e_parm <= 0x0L )
	  {
	    d->e_parm = 0x0L;
	    evremove(d, 0);
	    d = dlr;
 	  }
	  else
	  {
						/* set compare tick 	*/
						/*   time to the amount	*/
						/*   the first guy is	*/
						/*   waiting		*/
	    cli();
	    CMP_TICK = d->e_parm;
	    NUM_TICK = 0x0L;
	    sti();
	    break;
	  }
	}
}


WORD tak_flag(REG SPB *sy)
{
						/* count up		*/
	sy->sy_tas++;
						/* if we didn't already	*/
						/*   own it and it 	*/
						/*   wasn't free	*/
						/*   then wait for it	*/
						/*   else claim ownership*/
	if ( (sy->sy_owner != rlr) &&
	     (sy->sy_tas != 1) )
	  sy->sy_tas--;
	else
	  sy->sy_owner = rlr;

	return( sy->sy_owner == rlr );
}


VOID amutex(REG EVB *e, LONG ls)
{
	REG SPB		*sy;
						/* sy -	points to sync	*/
						/*   parameter block for*/
						/*   which mutex is	*/
						/*   desired		*/
	sy = (SPB *) ls;
	if ( tak_flag(sy) )
	  azombie(e, 0);
	else
	  evinsert(e, &sy->sy_wait);
}


VOID unsync(REG SPB *sy)
{
	REG EVB		*p;
						/* internal unsync must	*/
						/*   be in dispatcher	*/
						/*   context or NODISP	*/
						/* count down		*/
	sy->sy_tas--;
						/* if it went to 0 then	*/
						/*   give up the sync	*/
						/*   to the next guy	*/
						/*   if there is one	*/
	if (sy->sy_tas == 0)
	{
	  if (( p = sy->sy_wait ) != 0)	 		
	  {
						/* next off the wait	*/
						/*   list 		*/
	    sy->sy_wait = p->e_link;
	    sy->sy_owner = (PD *) p->e_pd;
						/* restart counting sema*/
	    sy->sy_tas = 1;
	    azombie(p, 0);
	   dsptch();
	  }
	  else
	    sy->sy_owner = 0;			/* reset owner field	*/
	}
}



