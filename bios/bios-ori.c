/*
 *  biosc.c - C portion of BIOS initialization and front end
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  SCC     Steve C. Cavender
 *  KTB     Karl T. Braun (kral)
 *  JSL     Jason S. Loveman
 *  EWF     Eric W. Fleischman
 *  LTG     Louis T. Garavaglia
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "bios.h"
#include "abbrev.h"
#include "gemerror.h"
#include "config.h"
#include "kprint.h"

/*==== Defines ============================================================*/
#define	DBGBIOSC	FALSE

#define	 HAVE_CON  TRUE
#define	 HAVE_SER  FALSE
#define	 HAVE_PAR  FALSE
#define	 HAVE_MOU  FALSE
#define	 HAVE_DSK  FALSE

#ifndef	COMMENT
#define	COMMENT	0
#endif

#define	M0101060301     FALSE

#define	PRTVER 0        /* set true to print bdos & bios time/date version */

#define SUPSIZ 1024	/* common supervisor stack size (in words) */


/*==== External declarations ==============================================*/
extern WORD os_dosdate;    /* Time in DOS format */
extern char *biosts ;         /*  time stamp string */

extern VOID c_init();         /* found in conio.c */
extern long cons_stat();      /* found in conio.c */
extern long cons_in ();       /* found in conio.c */
extern int  shifty;           /* found in conio.c */


#if HAVE_SIO
extern long sinstat();		/* found in siostat.c */
#endif

extern void cons_out(char);     /* found in vt52.c */

extern long format();		/* found in disk.c */

extern BPB vme_dpb [];		/* found in disk.c */

extern MD b_mdx;		/* found in startup.s */
extern LONG tticks;		/* found in startup.s */
extern LONG trap_1();		/* found in startup.s */

extern VOID mfp_init(VOID);     /* found in mfp.c */
extern VOID timer_init(VOID);   /* found in mfp.c */
extern VOID usart_init(VOID);   /* found in mfp.c */
extern VOID kbd_init(VOID);     /* found in kbd.c */
extern VOID kbq_init(VOID);     /* found in kbq.c */
extern VOID clk_init(VOID);     /* found in clock.c */
extern VOID con_init(VOID);     /* found in conio.c */

//EXTERN VOID siolox();
//EXTERN VOID moulox();
//EXTERN VOID clklox();

extern long oscall();           /* This jumps to BDOS */
extern long osinit() ;




/*==== BIOS table definitions =============================================*/

#if 0
GLOBAL	WORD	supstk[SUPSIZ]; /* sup stack for BIOS */
#endif

#if	DEFDRV == 0
    static char env[] = "COMSPEC=a:\\command.prg\0";
#else
    static char env[] = "COMSPEC=c:\\command.prg\0";
#endif

//static int defdrv = DEFDRV ;       /* default drive number (0 = a:, 2 = c:) */

static int exist[4];          /* 1, if drive present */
static int known[4];          /* 1, if disk logged-in */

char secbuf[4][512];          /* sector buffers */

BCB bcbx[4];    /* buffer control block array for each buffer */
BCB *bufl[2];   /* buffer lists - two lists:  fat,dir / data */

PFI charvec[5];     /* array of vectors to logical interrupt handlers */



/*==== BIOS initialization ================================================*/
/*
**	called from startup.s, this routine will do necessary bios initialization
**	that can be done in hi level lang.  startup.s has the rest.
*/

VOID biosinit()
{
    /*==== set up logical interrupt handlers for character devices =========*/
    charvec[0] = (PFI) 0;
    charvec[1] = (PFI) 0;   /* siolox - disabled */
    charvec[2] = (PFI) 0;
    charvec[3] = (PFI) 0;   /* clklox - disabled */
    charvec[4] = (PFI) 0;   /* moulox - disabled */

    /* initialize drive variables */
    exist[0] = 1;       /* Drive A present */
    exist[1] = 0;       /* Drive B not present */
    exist[2] = 1;       /* Drive C present */
    exist[3] = 0;       /* Drive D not present */

    known[0] = 0;       /* not loggend in */
    known[1] = 0;       /* not loggend in */
    known[2] = 0;       /* not loggend in */
    known[3] = 0;       /* not loggend in */


/*==== initialize components ==============================================*/

    con_init();         /* initialize the system console */

    /* print, what has been done till now (fake) */
    cputs("[ OK ] Entered supervisormode ...\n\r");
    cputs("[ OK ] Configured memory ...\n\r");
    cputs("[ OK ] Initialized video shifter ...\n\r");
    cputs("[ OK ] Soundchip initialized ...\n\r");
    cputs("[ OK ] Floppy deselected ...\n\r");

    /* now do the remaining things */
    mfp_init();         /* init MFP and ACIAs */
    timer_init();       /* init MFP Timer A, B, C, D */
    usart_init();       /* init USART */
    kbq_init() ;        /* init keyboard queue */
    kbd_init();         /* init keyboard, disable mouse and joystick */
    clk_init();         /* init clock (dummy for emulator) */


#if HAVE_SIO
    m400init();	  /* initialize the serial I/O ports */
#endif

#if HAVE_PAR
    m410_init();  /* initialize the parallel printer ports */
#endif

#if HAVE_DSK
    initdsks();   /* send disk config info to disk controller */
#endif

    /* returning to assembler patch OS via cartridge */
}


/*
 *  biosmain - c part of the bios init code
 *	inits the disk buffer cache.
 *	invokes the bdos init code
 *	possibly prints bdos date/version string
 *	exec the CLI
 */

VOID biosmain()
{

    /* set up sector buffers */

    bcbx[0].b_link = &bcbx[1];
    bcbx[2].b_link = &bcbx[3];

    /* make BCBs invalid */

    bcbx[0].b_bufdrv = -1;
    bcbx[1].b_bufdrv = -1;
    bcbx[2].b_bufdrv = -1;
    bcbx[3].b_bufdrv = -1;

    /* initialize buffer pointers in BCBs */

    bcbx[0].b_bufr = &secbuf[0][0];
    bcbx[1].b_bufr = &secbuf[1][0];
    bcbx[2].b_bufr = &secbuf[2][0];
    bcbx[3].b_bufr = &secbuf[3][0];

    /* initialize the buffer list pointers */
    
    bufl[BI_FAT] = &bcbx[0]; 			/* fat buffers */
    bufl[BI_DATA] = &bcbx[2]; 			/* dir/data buffers */

    /* An now the BDOS part... */
    osinit();           /*  Init BDOS */
    kprint("BIOS: osinit - GEMDOS successfully initialized ...\n");

    cputs("[    ]: BDOS setup works ....\r");
    if (trap_1( 0x30 ) < 0)     /* initial test, if BDOS works */
        cputs("[FAIL]\n\r");
    else
        cputs("[ OK ]\n\r");

    trap_1( 0x2b, os_dosdate);  /* set initial date in GEMDOS format */
    cputs("[ OK ]: Initial system date and time set ...\n\r");

//    trap_1( 0x0e , defdrv );    /* Set boot drive */
    cputs("[ OK ]: Boot disk drive set ...\n\r");
    kprint("BIOS: Testpoint reached ...\n");

    /* execute Reset-resistent PRGs */

    /* switch on cursor */
    
    /* autoexec Prgs from AUTO folder */
    
    /* clear environment string */

    
    /* clear commandline */
    
    /* load command.prg */
    
    /* check, if command.prg has loaded correctly */
    cputs("[    ]: COMMAND.PRG loaded\r");
    if (trap_1( 0x4b , 0, "COMMAND.PRG" , "", env) < 0)
        cputs("[FAIL]\n\r");
    else
        cputs("[ OK ]\n\r");

    cputs("[FAIL] HALT - should never be reached!\n\r");
    while(1) ;
    kprint("[FAIL] Error in execution\n");
}


/*****************************************************************************
 kpanic - throw out a panic message and halt
 *****************************************************************************/

VOID kpanic(char * s)
{
    kprint( "BIOS: Panic: \n");
    kprint (s);
    kprint( "\n");
    while(1);
}

/*****************************************************************************
 bios_null - so lint wont complain
 *****************************************************************************/

VOID bios_null(UWORD x , UWORD y , BYTE * ptr )
{
    x = y ;
    y = x ;
    ++ptr ;
}

/*****************************************************************************
 getmpb - Load Memory parameter block
 *****************************************************************************/

void bios_0(MPB *m)
{
    m->mp_mfl = m->mp_rover = &b_mdx;
    m->mp_mal = (MD *)0;
}


/*****************************************************************************
 bconstat - Status of input device
 Returns status in D0.L:
 -1	device is ready
 0	device is not ready
 *****************************************************************************/

LONG bios_1(handle)	/* GEMBIOS character_input_status */
WORD	handle;		/* 0:PRT 1:AUX 2:CON */
{
    switch(handle & 7)
    {
    case h_PRT :
        return(NULL); 		/* This device does not accept input */

    case h_AUX :
        /*		return( sinstat() );*/
        return(NULL);               /* No yet implemented */

    case h_CON :
        return ( cons_stat() );

    default:
        return(NULL);		/* non-existent devices never ready */
    }
}


/*****************************************************************************
 bconin  - Get character from device
 *****************************************************************************/
/* This function does not return until a character has been */
/* input.  It returns the character value in D0.L, with the */
/* high word set to zero.  For CON:, it returns the GSX 2.0 */
/* compatible scan code in the low byte of the high word, & */
/* the ASCII character in the lower byte, or zero in the    */
/* lower byte if the character is non-ASCII.  For AUX:, it  */
/* returns the character in the low byte.		    */

LONG bios_2(int handle)
{
    switch(handle)      /* 0:PRT 1:AUX 2:CON */
    {
    case h_PRT :
        return(NULL);		/* printer is not an input dev*/

    case h_AUX :
        /*return(sgetc()); */     /* read input f serial port A */
        return(NULL);	/* not yet implemented */

    case h_CON :
        return( cons_in() );		/* read the keyboard */

    default:
        return(NULL);			/*non-existnt devices ret null*/

    }
}

/*****************************************************************************
 bconout  - Print character to output device
 *****************************************************************************/

VOID bios_3(int handle, char what)
{
    switch(handle & 3)
    {
    case h_PRT :
#if 0
#if MVME410
        m410_out(what);		/* output char to parallel printer */
#else
        sputc(what);		/* output char to printer	   */
#endif
#endif
        break;

    case h_AUX :
        /*        sputc(what);*/		/* output char to AUX:		*/
        break;

    case h_CON :
        cons_out(what);		/* output char to the screen	*/
        break;

    default:
        cons_out(what);
        break;
    }
}


/*****************************************************************************
 rwabs  - Read or write sectors
 *****************************************************************************/
/* Returns a 2's complement error number in D0.L.  It */
/* is the responsibility of the driver to check for   */
/* media change before any write to FAT sectors.  If  */
/* media has changed, no write should take place, just*/
/* return with error code.			      */

/* r_w   = 0:Read 1:Write */
/* *adr  = where to get/put the data */
/* numb  = # of sectors to get/put */
/* first = 1st sector # to get/put = 1st record # tran */
/* drive = drive #: 0 = A:, 1 = B:, etc */

LONG bios_4(WORD r_w, BYTE *adr, WORD numb, WORD first, WORD drive)
{
#if 0
    WORD	n;
    LONG	error;
#endif
    LONG        adr_inc;

#if	DBGBIOSC
    fkprintf( 1 , "Entering biosc:bios_4(%x,%lx,%x,%x,%x)" ,
              r_w, adr, numb, first, drive) ;
#endif

    kprint("BIOS: bios_4 - not implemented rwabs routine ...\n");

    if( ( drive > MAXDSK ) ||  (! exist[drive] ) )
        return(EDRVNR);	/* T if asking for a drive we ain't got */

    known[drive] = 1;

    /* we told gemdos we have 512 byte sectors */

    if (drive < 2)		/* True iff it is a floppy else h.d.  */
    {
        adr_inc = 0x1FF00L;
        /*tticks = 0;*/		/* reinitiz timer tick count cause new*/
    }				/*     disk access is now taking place*/
    else	/* Get here iff it is a H. D. access  */
    {
        adr_inc = 0xFF00L;
    }

#if 0   /* Implemented by STonX */
    while (numb)
    {
        n = (numb > 255) ? 255 : numb;

        if (r_w == 0)
        {
            if (error = ReadSector(drive, (long)first, n, adr))
                return(error);
        }
        else if (r_w == 1)
        {
            if (error = WriteSector(drive, (long)first, n, adr))
                return(error);
        }
        else
            return (EBADRQ);

        numb -= n;
        first += n;
        adr += adr_inc;
    }
#endif
    return(E_OK);
}

/**
 * tickcal - Time between two systemtimer calls
 */

LONG bios_6()
{
    return(5L);	/* Intterupt is 200 Hz so 5 ms is the period */
}


/**
 * get_bpb - Get BIOS parameter block
 * Returns a pointer to the BIOS Parameter Block for
 * the specified drive in D0.L.  If necessary, it
 * should read boot header information from the media
 * in the drive to determine BPB values.
 *
 * Arguments:
 *  drive - drive  (0 = A:, 1 = B:, etc)
 */

LONG bios_7(WORD drive)
{

#if 0                         /* Implemented by STonX */
#if	M0101060301
    return(  ( drive>MAXDSK  ||  !exist[drive] ) ? 0L : &vme_dpb[drive]  );
#else
    if(exist[drive])
        return((LONG)&vme_dpb[drive]);
    else
#endif
#endif
    kprint("BIOS: bios_7 - not implemented getbpb routine ...\n");
    return(0L);	/* drive doesn't exist */
}

/*****************************************************************************
 bconstat - Read status of output device
 *****************************************************************************/
/* Returns status in D0.L:	*/
/* -1	device is ready		*/
/* 0	device is not ready	*/

/* handle  = 0:PRT 1:AUX 2:CON */

LONG bios_8(WORD handle)	/* GEMBIOS character_output_status */
{
    switch(handle)
    {
    case h_PRT :
        return(dev_RDY);
#if 0
#if MVME410
        if (m410_stat ())
            return (dev_RDY);
        else return (NULL);
#else
        return(dev_RDY);
#endif
#endif
    case h_AUX :
        return(dev_RDY);
#if 0
        return( soutstat() );
#endif
    case h_CON :
        return(dev_RDY);

    default:
        return(NULL);			/*non-existnt devices ret null*/
    }
}

/*****************************************************************************
 mediach - See, if floppy has changed
 *****************************************************************************/
/* Returns media change status for specified drive in D0.L: */
/* 0	Media definitely has not changed		    */
/* 1	Media may have changed				    */
/* 2	Media definitely has changed			    */
/* where "changed" = "removed" 				    */

LONG bios_9(WORD drv)
{
#if 0           
    if(drv < 2)			/* True, if is a floppy disk drive */
    {
        if (tticks > 320)	/* True, if no disk access in last 5 sec */
            return(1L);	        /* No floppy disk access in past 5 sec */
        /* Assume, that no media changed if floppy */
        /* disk access in last 5 sec; 64 ticks/sec */
    }
    return(0L);		/* winchesters cannot have media chngd and    */
#endif
    return(0L);         /* STonX can not change Floppies */
}



/*****************************************************************************
 drvmap - Read drive bitmap
 *****************************************************************************/
/* Returns a long containing a bit map of logical drives on */
/* the system, with bit 0, the least significant bit,       */
/* corresponding to drive A.  Note that if the BIOS supports*/
/* logical drives A and B on a single physical drive, it    */
/* should return both bits set if a floppy is present.	    */

LONG bios_a()
{
    return(exist[0] | (exist[1]<<1) | (exist[2]<<2) | (exist[3]<<3));
}



/*****************************************************************************
 kbshift - Shift Key mode get/set
 *****************************************************************************/
/*
 **  bios_b - (kbshift)  Shift Key mode get/set.
**  two descriptions:
**	o	If 'mode' is non-negative, sets the keyboard shift bits
**		accordingly and returns the old shift bits.  If 'mode' is
**		less than zero, returns the IBM0PC compatible state of the
**		shift keys on the keyboard, as a bit vector in the low byte
**		of D0
**	o	The flag parameter is used to control the operation of
**		this function.  If flag is not -1, it is copied into the
**		state variable(s) for the shift, control and alt keys,
**		and the previous key states are returned in D0.L.  If
**		flag is -1, then only the inquiry is done.
*/

LONG    bios_b(WORD flag)
{
    WORD oldy;

    if(flag == -1)
        return(shifty);         /* return bitvector of shift state */

    oldy = shifty;

    shifty=flag;

    return(oldy);
}


/*****************************************************************************
 **
 ** bios_c - character device control channel input
 */

LONG	bios_c(WORD handle, WORD length, BYTE *buffer)
{
    bios_null( handle, length, buffer ) ;	/*  for lint		*/
    return(ERR);
}


/*****************************************************************************
 **
 ** bios_d - character device control channel output
 */

LONG	bios_d(WORD handle, WORD length, BYTE *buffer)
{
    bios_null( handle, length, buffer) ;	/* for lint  */
    return(ERR);
}


/*****************************************************************************
 **
 ** bios_e - block device control channel input
 */

LONG	bios_e(WORD drive, WORD length, BYTE *buffer)
{
    bios_null( drive , length , buffer ) ;	/*  for lint	*/
    return(ERR);
}


/*****************************************************************************
 **
 ** bios_f - block device control channel output
 */

LONG	bios_f(WORD drive, WORD length, BYTE *buffer)
{
    if ((drive == 0) || (drive == 1))
    {
        if ((length == 1) && (buffer[0] == 1))	/* disk format */
        {
#if IMPLEMENTED
            return(format(drive));
#else
            return(E_OK);
#endif
        }
    }
    return( EBADRQ ) ;
}


/*****************************************************************************
 **
 ** bios_10 - character device exchange logical interrupt vector
 **
 ******************************************************************************
 */

PFI	bios_10(WORD handle, PFI address)
{
    PFI	temp;

    if (handle < 5)	/* note hard-coded device count */
    {
        temp = charvec[handle];
        if (address != (PFI)-1)
            charvec[handle] = address;
        return(temp);
    }
    return((PFI)ERR);
}

/*
 **  nullint - an unexpected interrupt has happened
 **	the 'a' parameter is really the first word in the information pushed
 **	onto the stack for exception processing.
 */

VOID	nullint(WORD a)
{
    WORD	*b ;

    b = &a ;
#if 0
    kprintf("\n\rUnexpected Interrupt: %x %x %x %x\n\r",
            b[0],b[1],b[2],b[3]) ;
    kpanic("System Crash") ;
#endif
}
