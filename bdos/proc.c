/*
 * proc.c - process management routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  KTB     Karl T. Braun (kral)
 *  MAD     Martin Doering
 *  ACH     ???
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "fs.h"
#include "bios.h"				/*  M01.01.02	*/
#include "mem.h"
#include "gemerror.h"
#include "btools.h"
#include "../bios/kprint.h"

#define DBGPROC 1



PD	*run;           /* ptr to PD for current process */
WORD	supstk[SUPSIZ]; /* common sup stack for all processes*/
long	bakbuf[3];      /*  longjump buffer */


/**
 * ixterm - terminate a process
 *
 * terminate process with PD 'r'.
 *
 * @r: PD of process to terminate
 */

void	ixterm( PD *r )
{
    REG MD *m,
    **q;
    REG WORD h;
    REG WORD i;

    /* check the standard devices in both file tables  */

    for( i = 0 ; i < NUMSTD ; i++ )
        if( (h = r->p_uft[i]) > 0 )
            xclose(h);

    for (i = 0; i < OPNFILES; i++)
        if (r == sft[i].f_own)
            xclose(i+NUMSTD);


    /* check directory usage  M01.01.07 */

    for( i = 0 ; i < NUMCURDIR ; i++ )
    {
        if( (h = r->p_curdir[i]) != 0 )
            diruse[h]-- ;
    }

    /*
     *  for each item in the allocated list that is owned by 'r',
     *	free it
     */

    for( m = *( q = &pmd.mp_mal ) ; m ; m = *q )
	{
            if (m->m_own == r)
            {
                *q = m->m_link;
                freeit(m,&pmd);
            }
            else
                q = &m->m_link;
        }
}



/*
 * envsize - determine size of env area
 *
 * counts bytes starting at 'env' upto and including the terminating
 * double null.
 */

WORD	envsize( char *env )
{
    REG char	*e ;
    REG WORD 	cnt ;

    for( e = env, cnt = 0 ; !(*e == NULL && *(e+1) == NULL) ; ++e, ++cnt )
        ;

    return( cnt + 2 ) ;		/*  count terminating double null  */
}



/** xexec - (p_exec - 0x4b) execute a new process
 *
 *	
 * load&go(cmdlin,cmdtail), load/nogo(cmdlin,cmdtail), justgo(psp)
 * create psp - user receives a memory partition
 *
 * @flg: flag = 0: load&go, 3:load/nogo, 4:justgo, 5:create psp
 * @s:   command
 * @t:   tail
 * @v:   environment
 */


long	xexec(WORD flg, char *s, char *t, char *v)
{	
    PD	*p;
    char *b, *e;
    WORD i, h;			/*  M01.01.04		*/
    WORD cnt ;
    long rc, max;
    MD	*m, *env;
    long *spl;
#if	!M0101082703
    WORD	*spw;
#endif

    m = env = 0L ;

    /*
     *  check validity of flg - 1,2 or >5 is not allowed
     */

#if	DBGPROC
    kprintf("BDOS: xexec - flag or mode = %lx \n", flg);
#endif

    if (flg == 6)   /* (not really) implement newer mode 6 */
        flg = 4;
    
    if(   flg && (	flg < 3 || flg > 5  )	 )
        return(EINVFN);

    if ((flg == 0) || (flg == 3))       /* load (execute) a file */
    {
#if	DBGPROC
        kprintf("BDOS: xexec - trying to find the command ...\n");
#endif
        if (ixsfirst(s,0,0L)) {
#if	DBGPROC
            kprintf("BDOS: Command %s not found!!!\n", s);
#endif
            return(EFILNF); 	/*  file not found	*/
        }
    }

    /* LVL xmovs(sizeof(errbuf),errbuf,bakbuf); */
    memcpy(bakbuf, errbuf, sizeof(errbuf));

    if (rc = setjmp(errbuf))
    {
        /* Free any memory allocated to this program. */
        if (flg != 4)		/* did we allocate any memory? */
            ixterm((PD*)t);		/*  yes - free it */

        longjmp(bakbuf,rc);
    }

    /* will we need memory and a psp ? */

    if (flg != 4)
    {
        /* get largest memory partition available */

        if (!v)
            v = run->p_env;

        /*
         **  determine minimum
         */

        i = envsize( v ) ;
        if( i & 1 )			/*  must be even	*/
            ++i ;
        /*
         **  allocate environment
         */

        if (!(env = ffit((long) i,&pmd)))
        {
#if	DBGPROC
            kprintf("xexec: Not Enough Memory!\n") ;
#endif
            return(ENSMEM) ;
        }

        e = (char *) env->m_start;

        /*
         **  now copy it
         */

        /* LVL bmove( v , e , i ) ; */
	memcpy(e, v, i);

        /*
         **  allocate base page
         */

        max = (long) ffit( -1L , &pmd ) ;	/*  amount left */

        if( max < sizeof(PD) )
        {	/*  not enoufg even for PD  */
            freeit(env,&pmd);
#if	DBGPROC
            kprintf("xexec: No Room For Base Pg\n") ;
#endif
            return(ENSMEM);
        }

        /*  allocate the base page.  The owner of it is either the
         new process being created, or the parent  */

        m = ffit(max,&pmd);

        p = (PD *) m->m_start;		/*  PD is first in bp	*/

        env->m_own =  flg == 0 ? p : run ;
        m->m_own = env->m_own ;

        max = m->m_length;		/*  length of tpa	*/

        /*
         * We know we have at least enough room for the PD (room
         * for the rest of the pgm checked for in pgmld)
         * initialize the PD (first, by zero'ing it out)
         */

        bzero( (char *) p , sizeof(PD)	) ;

        p->p_lowtpa = (long) p ;		/*  M01.01.06	*/
        p->p_hitpa  = (long) p	+  max ;	/*  M01.01.06	*/
        p->p_xdta = &p->p_cmdlin[0] ;	/* default p_xdta is p_cmdlin */
        p->p_env = (char *) env->m_start ;


        /* now inherit standard files from me */

        for (i = 0; i < NUMSTD; i++)
        {
            if ((h = run->p_uft[i]) > 0)
                ixforce(i,run->p_uft[i],p);
            else
                p->p_uft[i] = h;
        }

        /* and current directory set */

        for (i = 0; i < 16; i++)
            ixdirdup(i,run->p_curdir[i],p);

        /* and current drive */

        p->p_curdrv = run->p_curdrv;

        /* copy tail */

        b = &p->p_cmdlin[0] ;
        for( i = 0 ; (i < PDCLSIZE)  && (*t) ; i++ )
            *b++ = *t++;

        *b++ = 0;
        t = (char *) p;
    }

    /*
     * for 3 or 0, need to load, supply baspage containing:
     * tpa limits, filled in with start addrs,lens
     */

    if((flg == 0) || (flg == 3))
        if (rc = xpgmld(s,t))
        {
#if	DBGPROC
            kprintf("cmain: error returned from xpgmld = %lx \n", rc);
#endif
            ixterm((PD*)t);
            return(rc);
        }

    if ((flg == 0) || (flg == 4))
    {
#if	DBGPROC
        kprintf("BDOS: xexec - trying to load (and execute) a command ...\n");
#endif
        p = (PD *) t;
        p->p_parent = run;
        spl = (long *) p->p_hitpa;
        *--spl = (long) p;
        *--spl = 0L; /* bogus retadd */

        /* 10 regs (40 bytes) of zeroes  */

        for (i = 0; i < 10; i++)
            *--spl = 0L;

        *--spl = p->p_tbase; /* text start */
#if	!M0101082703
        spw = (WORD *) spl;
        *--spw = 0; /* startup status reg */
        spl = (long *) spw;
#else
        *--(WORD *)spl = 0 ;
#endif
        *--spl = (long) &supstk[SUPSIZ];
        p->p_areg[6-3] = p->p_areg[7-3] = (long) spl;
        p->p_areg[5-3] = p->p_dbase;
        p->p_areg[4-3] = p->p_bbase;
        run = (PD *) p;

        gouser() ;
    }

    /* sub-func 3 and 5 return here */

#if	DBGPROC
    kprintf("BDOS: xexec - return code = %lx \n", t);
#endif
    return( (long) t );
}



/*
** [1]	The limit on this loop should probably be changed to use sizeof(PD)
*/



/*
 *  x0term - (p_term0 - 0x00)Terminate Current Process
 *
 *  terminates the calling process and returns to the parent process
 *  without a return code
 */

void	x0term()
{
    xterm(0);
}

/*
 *  xterm - terminate a process
 *	terminate the current process and transfer control to the colling
 *	process.  All files opened by the terminating process are closed.
 *
 *	Function 0x4C	p_term
 */

void	xterm(rc)
	UWORD	rc;
{
	PD *r;

	(* (WORD(*)()) trap13(5,0x102,-1L))() ;	/*  call user term handler */

	run = (r = run)->p_parent;
	ixterm( r );
	run->p_dreg[0] = rc;
	gouser();
}


/*	
**  xtermres - 
**	Function 0x31	p_termres
*/

WORD	xtermres(blkln,rc)
	WORD	rc;
	long	blkln;
{
	MD *m,**q;

	xsetblk(0,run,blkln);

	for (m = *(q = &pmd.mp_mal); m ; m = *q)
		if (m->m_own == run)
		{
			*q = m->m_link; /* pouf ! like magic */
			xmfreblk(m);
		}
		else
			q = &m->m_link;

	xterm(rc);
}	



