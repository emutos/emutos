/*	GEMASYNC.C	1/27/84 - 09/12/85	Lee Jay Lorenzen	*/
/*	merge High C vers. w. 2.2 & 3.0		8/19/87		mdf	*/ 

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
						/* in ASM.A86   	*/
EXTERN 		dsptch();

EXTERN VOID	adelay();			/* in INPUT88.C		*/

EXTERN VOID	akbin();			/* in INPUT88.C		*/

EXTERN VOID	amouse();			/* in INPUT88.C		*/
EXTERN VOID	abutton();

/* ---------- added for metaware compiler ---------- */
EXTERN  	bfill();			/* in OPTIMOPT.A86	*/
EXTERN VOID	aqueue();			/* in QUEUE.C		*/
EXTERN VOID 	amutex();			/* in FLAG.C		*/
/* ------------------------------------------------- */

EXTERN UWORD	button, xrat, yrat;

	VOID
signal(e)
	EVB		*e;
{
	REG PD		*p, *p1, *q1;

	p = (PD *) e->e_pd;
	p->p_evflg |= e->e_mask;
						/* off the not-ready	*/
						/*   list 		*/
	for (p1 = (q1 = (PD *) &nrl)->p_link; (p1 != p) && (p1);
	  p1 = (q1=p1)->p_link);
	if ( p != rlr )
	{
	  if ( p->p_evflg & p->p_evwait)
	  {
	    if ( p1 )
	    {
	      p1->p_stat &= ~WAITIN;
						/* onto the drl		*/
	      q1->p_link = p1->p_link;
	      p1->p_link = drl;
	      drl = p1;
	    }
	  }
	}
}


	VOID
azombie(e, ret)
	REG EVB		*e;
	UWORD		ret;
{
						/* must be called with	*/
						/*   dispatching off 	*/
	e->e_return = HW(button) | LW(ret);
	e->e_parm = HW(xrat) | LW(yrat);

	e->e_link = zlr;
	if ( zlr )
	  zlr->e_pred = e;
	e->e_pred = (BYTE *) &zlr - elinkoff;
	zlr = e;
	e->e_flag = COMPLETE;
	signal(e);
}


	VOID
evinsert(e, root)
	REG EVB		*e;
	EVB		**root;
{
	REG EVB		*p, *q;
						/* insert event block	*/
						/*   on list 		*/
	q = (BYTE *) root - elinkoff ;
	p = *root;
	e->e_pred = q;
	q->e_link = e;
	e->e_link = p;
	if ( p )
	  p->e_pred = e;
}

	VOID
takeoff(p)
	REG EVB		*p;
{
	REG LONG	c;

						/* take event p off	*/
						/*   e_link list, must 	*/
						/*   be NODISP 		*/
	p->e_pred->e_link = p->e_link;
	if (p->e_link)
	{
	  p->e_link->e_pred = p->e_pred;
	  if (p->e_flag & EVDELAY)
	  {
	    c = (LONG) p->e_link->e_parm;
	    c += (LONG) p->e_parm;
	    p->e_link->e_parm = (LONG) c;
	  }
	}
	p->e_nextp = eul;
	eul = p;
}


	EVSPEC
mwait(mask)
	EVSPEC		mask;
{
	rlr->p_evwait = mask;
	if ( !(mask & rlr->p_evflg) )
	{
	  rlr->p_stat |= WAITIN;
	  dsptch();
	}
	return(rlr->p_evflg);
}

	EVSPEC 
iasync(afunc, aparm)
	WORD		afunc;
	REG LONG	aparm;
{
	REG EVB		*e;
						/* get an evb		*/
	if ((e = eul) != 0)
	{
	  eul = eul->e_nextp;
	  bfill(sizeof(EVB), 0, e);
	}
						/* put in on list	*/
	e->e_nextp = rlr->p_evlist;
	rlr->p_evlist = e;
	e->e_pd = (BYTE *) rlr;
	e->e_flag = 0;
	e->e_pred = 0;
						/* find a free bit in	*/
						/*   in the mask	*/
	e->e_mask = afunc;
	rlr->p_evbits |= e->e_mask;

	switch(afunc)
	{
	  case MU_KEYBD: akbin(e);		break;
	  case MU_BUTTON:abutton(e,aparm);	break;
	  case MU_M1:
	  case MU_M2:	 amouse(e,aparm);	break;
	  case MU_MESAG: aqueue(FALSE,e,aparm); break;
	  case MU_TIMER: adelay(e,aparm);	break;
	  case MU_SDMSG: aqueue(TRUE,e,aparm);	break;
	  case MU_MUTEX: amutex(e,aparm);	break;
	}
	return(e->e_mask);
}


	UWORD
apret(mask)
	REG EVSPEC	mask;
{
	REG EVB		*p, *q;
	UWORD		erret;
						/* first find the event	*/
						/*   on the process list*/
	for (p = (q = (EVB *) &rlr->p_evlist) -> e_nextp; p;
						 p = (q=p)->e_nextp)
	  if (p->e_mask == mask) 
	    break;
						/* found the event,	*/
						/*   remove it from the	*/
						/*   zombie list 	*/
	p->e_pred->e_link = p->e_link;
	if ( p->e_link )
	  p->e_link->e_pred = p->e_pred;
	q->e_nextp = p->e_nextp;
	rlr->p_evbits &= ~mask;
	rlr->p_evwait &= ~mask;
	rlr->p_evflg &= ~mask;

	erret = LLOWD(p->e_return);

	p->e_nextp = eul;
	eul = p;

	return( erret );
}

	EVSPEC
acancel(m)
	EVSPEC		m;
{
	REG EVSPEC	m1;			/* mask of items not	*/
						/*   cancelled		*/
	REG WORD	f;
	REG EVB		*p, *q;

	m1 = 0;
	for (p = (q = (EVB *) &rlr->p_evlist)->e_nextp; p; p = (q=p)->e_nextp)
	{
	  if ( p->e_mask & m )
	  {
						/* take it off, if not	*/
						/*   completed or in-	*/
						/*   progress 		*/
	    f = p->e_flag;
	    if ( (f & NOCANCEL) || (f & COMPLETE) )
	      m1 |= p->e_mask;
	    else
	    {
	      q->e_nextp = p->e_nextp;
	      takeoff(p);
	      rlr->p_evbits &= ~p->e_mask;
	      rlr->p_evwait &= ~p->e_mask;
	      p = q;				/* contine traversal	*/
	    }
	  }
	}
	return(m1);
}


