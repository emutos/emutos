/*
 * bdosmain.c - GEMDOS main function dispatcher
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  EWF  Eric W. Fleischman
 *  JSL  Jason S. Loveman
 *  SCC  Steven C. Cavender
 *  LTG  Louis T. Garavaglia
 *  KTB  Karl T. Braun (kral)
 *  ACH  Anthony C. Hay (DR UK)
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#define DBGOSIF 0



#include	"portab.h"		/*  M01.01.05  */
#include	"fs.h"
#include	"bios.h"
#include	"gemerror.h"
#include	"../bios/kprint.h"

/*
**  local constants
*/

#define LENOSM 4000



/*
**  externals
*/

extern long x0term(),xterm(),conin(),tabout();
extern long rawconio(),prt_line(),readline(),constat();
extern long xsetdrv(),xgetdrv(),xsetdta(),xgetdta(),xgetfree();
extern long xmkdir(),xrmdir(),xchdir(),xcreat(),xopen(),xclose();
extern long xread(),xwrite(),xunlink(),xlseek(),xchmod();
extern long xgetdir(),xmalloc(),xmfree(),xsfirst(),xsnext();
extern long xrename(),xgsdtof(),xexec(),xtabout(),xconin();
extern long xconstat(),xprt_line(),xtermres(),dup();
extern long xforce(), xauxin(), xauxout(), xprtout();
extern long x7in(), x8in(), xconostat(), xprtostat();
extern long xauxistat(), xauxostat();
extern long xgetdate(), xsetdate(), xgettime(), xsettime();
extern long xsuper();

extern	int	add[3];
extern	int	remove[3];
extern	long	xsetblk();
extern	long	glbkbchar[3][KBBUFSZ];	/* typeahead buffer */


extern PD *run;


/*
 *  prototypes / forward declarations
 */

long	ni() , xgetver() ;



/*
 *  globals
 */

long	S_SetVec(), S_GetVec();
int oscnt;
long uptime;
int msec;
long errbuf[3];

/* 0 J	F  M  A  M  J  J  A  S	O  N  D */
int nday[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


MPB pmd;
int osmlen;
int osmem[LENOSM];



/*
 * FND - Function Descriptor
 *
 * Each entry in the function table (below) consists of the address of
 * the function which corresponds to the function number, and a function
 * type.
 */

#define FND struct _fnd
FND
{
	long	(*fncall)();
	int	fntyp;
};



/*
 * funcs - table of os functions, indexed by function number
 *
 * Each entry is for an FND structure. The function 'ni' is used
 * as the address for functions not implemented.
 */

FND funcs[0x58] =
{

    
    x0term,0,	/* 0x00 */

    /*
     * console functions
     *
     * on these functions, the 0x80 flag indicates std file used
     * 0x80 is std in, 0x81 is stdout, 0x82 is stdaux, 0x83 stdprn
     */

    xconin,	0x80,	/* 0x01 */
    xtabout,	0x81,	/* 0x02 */
    xauxin,	0x82,	/* 0x03 */
    xauxout,	0x82,	/* 0x04 */
    xprtout,	0x83,	/* 0x05 */
    rawconio,	0,	/* 0x06 */
    x7in,	0x80,	/* 0x07 */
    x8in,	0x80,	/* 0x08 */
    xprt_line,	0x81,	/* 0x09 */
    readline,	0x80,	/* 0x0A */
    xconstat,	0x80,	/* 0x0B */

    /*
     * disk functions
     *
     * on these functions the 0x80 flag indicates whether a handle
     * is required, the low bits represent the parameter ordering,
     * as usual.
     */

    ni, 	0,
    ni, 	0,

    xsetdrv,	0,	/* 0x0E */

    ni, 	0,

    /*
     * extended console functions
     *
     * Here the 0x80 flag indicates std file used, as above
     */

    xconostat,	0x81,	/* 0x10 */
    xprtostat,	0x83,	/* 0x11 */
    xauxistat,	0x82,	/* 0x12 */
    xauxostat,	0x82,	/* 0x13 */

    ni, 	0,
    ni, 	0,
    ni, 	0,
    ni, 	0,
    ni, 	0,

    xgetdrv,	0,	/* 0x19 */
    xsetdta,	1,	/* 0x1A */

    xsuper,	0,	/* 0x20 - switch to supervisor mode */
    ni, 	0,
    ni, 	0,
    ni, 	0,
    ni, 	0,

    /* xgsps */

    ni, 	0,	/* 0x20 */
    ni, 	0,
    ni, 	0,
    ni, 	0,
    ni, 	0,

    S_SetVec,	1,	/* 0x25 */

    ni, 	0,
    ni, 	0,
    ni, 	0,
    ni, 	0,

    xgetdate,	0,	/* 0x2A */
    xsetdate,	0,	/* 0x2B */
    xgettime,	0,	/* 0x2C */
    xsettime,	0,	/* 0x2D */

    ni, 	0,

    xgetdta,	0,	/* 0x2F */
    xgetver,	0,	/* 0x30 */
    xtermres,	1,	/* 0x31 */

    ni, 	0,
    ni, 	0,
    ni, 	0,

    S_GetVec,	0,	/* 0x35 */
    xgetfree,	1,	/* 0x36 */

    ni, 	0,
    ni, 	0,

    xmkdir,	1,	/* 0x39 */
    xrmdir,	1,	/* 0x3A */
    xchdir,	1,	/* 0x3B */
    xcreat,	1,	/* 0x3C */
    xopen,	1,	/* 0x3D */
    xclose,	0x0,	/* 0x3E - will handle its own redirection */
    xread,	0x82,	/* 0x3F */
    xwrite,	0x82,	/* 0x40 */
    xunlink,	1,	/* 0x41 */
    xlseek,	0x81,	/* 0x42 */
    xchmod,	1,	/* 0x43 */
    ni,		0,	/* 0x44 */
    dup,	0,	/* 0x45 */
    xforce,	0,	/* 0x46 */
    xgetdir,	1,	/* 0x47 */
    xmalloc,	1,	/* 0x48 */
    xmfree,	1,	/* 0x49 */
    xsetblk,	2,	/* 0x4A */
    xexec,	3,	/* 0x4B */
    xterm,	0,	/* 0x4C */

    ni, 	0,
		
    xsfirst,	1,	/* 0x4E */
    xsnext,	0,	/* 0x4F */

    ni, 	0,	/* 0x50 */
    ni, 	0,
    ni, 	0,
    ni, 	0,
    ni, 	0,
    ni, 	0,

    xrename,	2,	/* 0x56 */
    xgsdtof,	1	/* 0x57 */
};



char	*bdosver = "GEMDOS Version 01.MAD" ; /* bdos version string */



/*
 *  xgetver -
 *	return current version number
 */

long	xgetver()
{
	return(0x0101L);		/*  minor.major */
#if DBGOSIF
	kprintf("BDOS: xgetver - Get version  successful ...\n");
#endif
}


/*
 *  ni -
 */

long	ni()
{
	return(EINVFN);
}




/*
 *  cinit - C part of osinit().
 */

void	cinit()
{

    getmpb(&pmd);
    osmlen = LENOSM;
    run = MGET(PD);
#if DBGOSIF
    kprintf("BDOS: Address of basepage = %08lx\n", (LONG)&run);
#endif

    /* set up system initial standard handles */

    run->p_uft[0] = H_Console;		/* stdin	=	con:	*/
    run->p_uft[1] = H_Console;		/* stdout	=	con:	*/
    run->p_uft[2] = H_Console;		/* stderr	=	con:	*/
    run->p_uft[3] = H_Aux;		/* stdaux	=	aux:	*/
    run->p_uft[4] = H_Print;		/* stdprn	=	prn:	*/

    add[0] = remove[0] = add[1] = remove[1] = add[2] = remove[2] = 0 ;


    date_time(GET_DATE, &date); 	/* allow bios to initialise date and */
    date_time(GET_TIME, &time); 	/* time from hardware, if supported */

#if DBGOSIF
    kprintf("BDOS: cinit - osinit successful ...\n");
#endif
}



/*
 *  ncmps -  compare two text strings, ingoreing case.
 */

int	ncmps(int n, char *s, char *d)
{
    while (n--)
	if (uc(*s++) != uc(*d++))
	    return(0);

    return(1);
}



/*
 *  freetree -	free the directory node tree
 */

void	freetree(DND *d)
{
    int i;

    if (d->d_left) freetree(d->d_left);
    if (d->d_right) freetree(d->d_right);
    if (d->d_ofd)
    {
	xmfreblk(d->d_ofd);
    }
    for (i = 0; i < NCURDIR; i++)
    {
	if (dirtbl[i] == d)
	{
	    dirtbl[i] = 0;
	    diruse[i] = 0 ;	/*  M01.01.06		*/
	}
    }
    xmfreblk(d);
}



/*
 *  offree -
 */

void	offree(DMD *d)
{
    int i;
    OFD *f;
    for (i=0; i < OPNFILES; i++)
	if( ((long) (f = sft[i].f_ofd)) > 0L )
	    if (f->o_dmd == d)
	    {
		xmfreblk(f);
		sft[i].f_ofd = 0;
		sft[i].f_own = 0;
		sft[i].f_use = 0;
	    }
}

/*
 *  osif -
 */

#if	DBGOSIF
/*
 * if in debug mode, use this 'front end' so we can tell if we exit
 * from osif
 */

long	osif(int *pw )
{
    long	osif2() ;
    char	*p ;
    long	r ;

    p = (char *) &pw ;
    osifdmp( p-4 , pw ) ;		/*  pass return addr and pw ptr */

    r = osif2( pw ) ;

    osifret() ;
    return( r ) ;
}
#else
/*
 * if not in debug mode, go directory to 'osif2'.  Do not pass go, do
 * not collect $200, and do not spend time on an extra call
 */
#define osif2	osif

#endif

long	osif2(int *pw)
{
    char **pb,*pb2,*p,ctmp;
    BPB *b;
    BCB *bx;
    DND *dn;
    int typ,h,i,fn;
    int num,max;
    long rc,numl;
    FND *f;


    oscnt = 0;
restrt:
    oscnt++;
    fn = pw[0];


    if (fn > 0x57)
	return(EINVFN);

    if (rc = setjmp(errbuf))
    {
	/* hard error processing */
	/* is this a media change ? */

	if (rc == E_CHNG)
	{	/* first, out with the old stuff */
	    dn = drvtbl[errdrv]->m_dtl;
	    offree(drvtbl[errdrv]);
	    xmfreblk(drvtbl[errdrv]);
	    drvtbl[errdrv] = 0;

	    if (dn)
		freetree(dn);

	    for (i = 0; i < 2; i++)
		for (bx = bufl[i]; bx; bx = bx->b_link)
		    if (bx->b_bufdrv == errdrv)
			bx->b_bufdrv = -1;

	    /* then, in with the new */

	    b = (BPB *) getbpb(errdrv);
	    if ( (long)b <= 0 ) /* M01.01.1007.01 */ /* M01.01.1024.01 */
	    {				/*  M01.01.01	*/
		drvsel &= ~(1<<errdrv); /*  M01.01.01	*/
		if ( (long)b )		/* M01.01.1024.01 */
		    return( (long)b );	/* M01.01.1024.01 */
		return(rc);
	    }				/*  M01.01.01	*/

	    if(  log(b,errdrv)	)
		return (ENSMEM);

	    rwerr = 0;
	    errdrv = 0;
	    goto restrt;
	}

	/* else handle as hard error on disk for now */

	for (i = 0; i < 2; i++)
	    for (bx = bufl[i]; bx; bx = bx->b_link)
		if (bx->b_bufdrv == errdrv)
		    bx->b_bufdrv = -1;
	return(rc);
    }

    f = &funcs[fn];
    typ = f->fntyp;

    if (typ && fn && ((fn<12) ||
		      ((fn>=16) && (fn<=19)))) /* std funcs */
    {
	if ((h = run->p_uft[typ & 0x7f]) > 0)
	{ /* do std dev function from a file */
	    switch(fn)
	    {
	    case 6:
		if (pw[1] != 0xFF)
		    goto rawout;
	    case 1:
	    case 3:
	    case 7:
	    case 8:
		xread(h,1L,&ctmp);
		return(ctmp);

	    case 2:
	    case 4:
	    case 5:
		/*  M01.01.07  */
		/*  write the char in the int at
		 pw[1]	*/
	    rawout:
		xwrite( h , 1L , ((char*) &pw[1])+1 ) ;
		return;

	    case 9:
		pb2 = *((char **) &pw[1]);
		while (*pb2) xwrite(h,1L,pb2++);
		return;

	    case 10:
		pb2 = *((char **) &pw[1]);
		max = *pb2++;
		p = pb2 + 1;
		for (i = 0; max--; i++,p++)
		{
		    if (xread(h,1L,p) == 1)
		    {
			oscall(0x40,1,1L,p);
			if (*p == 0x0d)
			{	/* eat the lf */
			    xread(h,1L,&ctmp);
			    break;
			}
		    }
		    else
			break;
		}
		*pb2 = i;
		return(0);

	    case 11:
	    case 16:
	    case 17:
	    case 18:
	    case 19:
		return(0xFF);
	    }
	}

	if ((fn == 10) || (fn == 9))
	    typ = 1;
	else
	    typ = 0;
    }

    if (typ & 0x80)
    {
	if (typ == 0x81)
	    h = pw[3];
	else
	    h = pw[1];

	if (h >= NUMSTD)
	    numl = (long) sft[h-NUMSTD].f_ofd;
	else if (h >= 0)
	{
	    if ((h = run->p_uft[h]) > 0)
		numl = (long) sft[h-NUMSTD].f_ofd;
	    else
		numl = h;
	}
	else
	    numl = h;

	if (!numl)
	    return(EIHNDL); /* invalid handle: media change, etc */

	if (numl < 0)
	{	/* prn, aux, con */
		/* -3   -2   -1	 */

	    num = numl;

	    /*	check for valid handle	*/ /* M01.01.0528.01 */
	    if( num < -3 )
		return( EIHNDL ) ;

	    pb = (char **) &pw[4];

	    /* only do things on read and write */

	    if (fn == 0x3f) /* read */
	    {
		if (pw[2])	/* disallow HUGE reads	    */
		    return(0);

		if (pw[3] == 1)
		{
		    **pb = conin(HXFORM(num));
		    return(1);
		}

		return(cgets(HXFORM(num),pw[3],*pb));
	    }

	    if (fn == 0x40) /* write */
	    {
		if (pw[2])	/* disallow HUGE writes     */
		    return(0);

		pb2 = *pb;	/* char * is buffer address */


		for (i = 0; i < pw[3]; i++)
		{
		    if( num == H_Console )
			tabout( HXFORM(num) , *pb2++ ) ;
		    else
		    {		/* M01.01.1029.01 */
			rc = bconout( HXFORM(num), *pb2++ ) ;
			if (rc < 0)
			    return(rc);
		    }
		}

		return(pw[3]);
	    }

	    return(0);
	}
    }
    rc = 0;
    if ((fn == 0x3d) || (fn == 0x3c))  /* open, create */
    {
	p = *((char **) &pw[1]);
	if (ncmps(5,p,"CON:"))
	    rc = 0xFFFFL;
	else if (ncmps(5,p,"AUX:"))
	    rc = 0xFFFEL;
	else if (ncmps(5,p,"PRN:"))
	    rc = 0xFFFDL;
    }
    if (!rc)
    {
	typ &= 0x07f;
	switch(typ)
	{
	case 0:
	    rc = (*f->fncall)(pw[1],pw[2]);
	    break;

	case 1:
	    rc = (*f->fncall)(pw[1],pw[2],pw[3],pw[4]);
	    break;

	case 2:
	    rc = (*f->fncall)(pw[1],pw[2],pw[3],pw[4],pw[5],pw[6]);
	    break;

	case 3:
	    rc = (*f->fncall)(pw[1],pw[2],pw[3],pw[4],pw[5],pw[6],pw[7]);
	}
    }
    return(rc);
}



/******************************************************************************
**
** S_SetVec - Function 0x25:  Set exception vector n to address
**
**	Last modified	SCC	8 Aug 85
**
*******************************************************************************
*/

long	S_SetVec(n, address)
int	n;
long	address;
{
	if (address == -1L)			/* disallow GET value		*/
		return (EINVFN);

	return (trap13(5, n, address)); 	/* pass on to BIOS to set it in */
}

/******************************************************************************
**
** S_GetVec - Function 0x35:  Get exception vector n
**
**	Last modified	SCC	8 Aug 85
**
*******************************************************************************
**/

long	S_GetVec(n)
int n;
{
	return (trap13(5, n, -1L));	/* pass to BIOS to get it	*/
}



/*
 *  tikfrk -
 */

void	tikfrk(int n)
{
    int curmo;

    uptime += n;
    msec += n;
    if (msec >= 2000)
    {
	/* update time */

	msec -= 2000;
	time++;

	if ((time & 0x1F) != 30)
	    return;

	time &= 0xFFE0;
	time += 0x0020;

	if ((time & 0x7E0) != (60 << 5))
	    return;

	time &= 0xF81F;
	time += 0x0800;

	if ((time & 0xF800) != (24 << 11))
	    return;

	time = 0;

	/* update date */

	if ((date & 0x001F) == 31)
	    goto datok;

	date++; 		/* bump day */

	if ((date & 0x001F) <= 28)
	    return;

	if ((curmo = (date >> 5) & 0x0F) == 2)
	{
	    /* 2100 is the next non-leap year divisible by 4, so OK */
	    if (!(date & 0x0600))
		if ((date & 0x001F) <= 29)
		    return;
		else
		    goto datok;
	}

	if ((date & 0x001F) <= nday[curmo])
	    return;

    datok:
	date &= 0xFFE0; 	/* bump month */
	date += 0x0021;

	if ((date & 0x01E0) <= (12 << 5))
	    return;

	date &= 0xFE00; 	/* bump year */
	date += 0x0221;
    }
}


