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
#define	DBGBIOSC        TRUE

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
extern BYTE *biosts ;         /*  time stamp string */

extern void c_init();         /* found in conio.c */
extern LONG con_stat();      /* found in conio.c */
extern LONG con_in ();       /* found in conio.c */
extern WORD  shifty;           /* found in conio.c */


#if HAVE_SIO
extern LONG sinstat();		/* found in siostat.c */
#endif

extern void cputc(WORD);     /* found in vt52.c */

extern LONG format();		/* found in disk.c */

extern BPB vme_dpb [];		/* found in disk.c */

extern LONG tticks;		/* found in startup.s */
extern LONG trap_1();		/* found in startup.s */
extern LONG drvbits;            /* found in startup.s */
extern MD b_mdx;                /* found in startup.s */

extern LONG drv_mediach(WORD drive);    /* found in startup.s */
extern LONG drv_bpb(WORD drive);        /* found in startup.s */
extern LONG drv_rw(WORD r_w,            /* found in startup.s */
                   BYTE *adr,
                   WORD numb,
                   WORD first,
                   WORD drive);


extern void mfp_init(void);     /* found in mfp.c */
extern void timer_init(void);   /* found in mfp.c */
extern void usart_init(void);   /* found in mfp.c */
extern void kbd_init(void);     /* found in kbd.c */
extern void midi_init(void);    /* found in midi.c */
extern void kbq_init(void);     /* found in kbq.c */
extern void clk_init(void);     /* found in clock.c */
extern void con_init(void);     /* found in conio.c */

//EXTERN void siolox();
//EXTERN void moulox();
//EXTERN void clklox();

extern LONG oscall();           /* This jumps to BDOS */
extern LONG osinit() ;




#if	DEFDRV == 0
    static BYTE env[] = "COMSPEC=a:\\command.prg\0";
#else
    static BYTE env[] = "COMSPEC=c:\\command.prg\0";
#endif

static WORD defdrv = DEFDRV ;       /* default drive number (0 = a:, 2 = c:) */

/* BIOS table definitions */
static WORD exist[4];          /* 1, if drive present */
static WORD known[4];          /* 1, if disk logged-in */

BYTE secbuf[4][512];          /* sector buffers */

BCB bcbx[4];    /* buffer control block array for each buffer */
BCB *bufl[2];   /* buffer lists - two lists:  fat,dir / data */

PFI charvec[5];     /* array of vectors to logical interrupt handlers */



/*==== BIOS initialization ================================================*/
/*
**	called from startup.s, this routine will do necessary bios initialization
**	that can be done in hi level lang.  startup.s has the rest.
*/

void biosinit()
{
    /*==== set up logical interrupt handlers for character devices =========*/
    charvec[0] = (PFI) 0;
    charvec[1] = (PFI) 0;   /* siolox - disabled */
    charvec[2] = (PFI) 0;
    charvec[3] = (PFI) 0;   /* clklox - disabled */
    charvec[4] = (PFI) 0;   /* moulox - disabled */

    /* initialize drive variables */
    exist[0] = 0;       /* Drive A present */
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
    midi_init();        /* init MIDI acia so that kbd acia irq works */
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

    /* returning to assembler patch OS via cartridge */
}


/*
 *  biosmain - c part of the bios init code
 *	inits the disk buffer cache.
 *	invokes the bdos init code
 *	possibly prints bdos date/version string
 *	exec the CLI
 */

void biosmain()
{


    cputs("[    ] BDOS setup works ....\r");
    if (trap_1( 0x30 ) < 0)     /* initial test, if BDOS works */
        cputs("[FAIL]\n\r");
    else
        cputs("[ OK ]\n\r");

    trap_1( 0x2b, os_dosdate);  /* set initial date in GEMDOS format */
    cputs("[ OK ] Initial system date and time set ...\n\r");

    trap_1( 0x0e , defdrv );    /* Set boot drive */
    cputs("[ OK ] Boot disk drive set ...\n\r");

    /* execute Reset-resistent PRGs */

    /* switch on cursor */
    
    /* autoexec Prgs from AUTO folder */
    
    /* clear environment string */

    
    /* clear commandline */
    
    /* load command.prg */
    
    /* check, if command.prg has loaded correctly */
    cputs("[ OK ] COMMAND.PRG loading and going on ...\n\r");
//    cputs("[ OK ] BENCH1.PRG loading and going on ...\n\r");
    trap_1( 0x4b , 0, "COMMAND.PRG" , "", env);
//    trap_1( 0x4b , 0, "BENCH1.PRG" , "", env);

    cputs("[FAIL] HALT - should never be reached!\n\r");
    while(1) ;
}



/**
 * bios_null - so lint wont complain
 */

void bios_null(UWORD x , UWORD y , BYTE * ptr )
{
    x = y ;
    y = x ;
    ++ptr ;
}

/**
 * bios_0 - (getmpb) Load Memory parameter block
 *
 * Returns values of the initial memory parameter block, which contains the
 * start address and the length of the TPA.
 * Just executed one time, before GEMDOS is loaded.
 *
 * Arguments:
 *   mpb - first memory descriptor, filled from BIOS
 *
 */

void bios_0(MPB *mpb)
{
    mpb->mp_mfl = mpb->mp_rover = &b_mdx; /* free list/rover set to init MD */
    mpb->mp_mal = (MD *)0;                /* allocated list set to NULL */

#if DBGBIOSC
    kprint("BIOS: getmpb m_start = ");
    kputp((LONG*) b_mdx.m_start);
    kprint("\n");
    kprint("BIOS: getmpb m_length = ");
    kputp((LONG*) b_mdx.m_length);
    kprint("\n");
#endif
}



/**
 * bios_1 - (bconstat) Status of input device
 *
 * Arguments:
 *   handle - device handle (0:PRT 1:AUX 2:CON)
 *
 *
 * Returns status in D0.L:
 *  -1	device is ready
 *   0	device is not ready
 */

LONG bios_1(WORD handle)	/* GEMBIOS character_input_status */
{
    switch(handle & 7)          /* strip upper bits */
    {
    case h_PRT :
        return(NULL); 		/* This device does not accept input */

    case h_AUX :
        /*		return( sinstat() );*/
        return(NULL);               /* No yet implemented */

    case h_CON :
        return ( con_stat() );

    default:
        return(NULL);		/* non-existent devices never ready */
    }
}



/**
 * bconin  - Get character from device
 *
 * Arguments:
 *   handle - device handle (0:PRT 1:AUX 2:CON)
 *
 * This function does not return until a character has been
 * input.  It returns the character value in D0.L, with the
 * high word set to zero.  For CON:, it returns the GSX 2.0
 * compatible scan code in the low byte of the high word, &
 * the ASCII character in the lower byte, or zero in the
 * lower byte if the character is non-ASCII.  For AUX:, it
 * returns the character in the low byte.
 */

LONG bios_2(WORD handle)
{
    switch(handle & 3)          /* strip upper bits */
    {
    case h_PRT :
        return(NULL);		/* printer is not an input dev*/
    case h_AUX :
        return(NULL);	        /* not yet implemented */
    case h_CON :
        return( con_in() );	/* read the keyboard */
    default:
        return(NULL);		/*non-existnt devices ret null*/

    }
}



/**
 * bconout  - Print character to output device
 */

void bios_3(WORD handle, BYTE what)
{
    switch(handle & 7)          /* strip upper bits */
    {
    case h_PRT :
#if IMPLEMENTED
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
        cputc(what);		/* output char to the screen	*/
        break;

    default:
        cputc(what);
        break;
    }
}



/**
 * rwabs  - Read or write sectors
 *
 * Returns a 2's complement error number in D0.L.  It
 * is the responsibility of the driver to check for
 * media change before any write to FAT sectors.  If
 * media has changed, no write should take place, just
 * return with error code.
 *
 * r_w   = 0:Read 1:Write
 * *adr  = where to get/put the data
 * numb  = # of sectors to get/put
 * first = 1st sector # to get/put = 1st record # tran
 * drive = drive #: 0 = A:, 1 = B:, etc
 */

LONG bios_4(WORD r_w, BYTE *adr, WORD numb, WORD first, WORD drive)
{
    /* implemented by STonX */
    return(drv_rw(r_w, adr, numb, first, drive));
}


/**
 * Setexec - set exception vector
 *
 */

LONG bios_5(WORD num, LONG vector)
{
  LONG oldvector;
  LONG *addr = (LONG *) (4L * num);
  oldvector = *addr;

  kprintf("Bios 5: Setexec(num = 0x%x, vector = 0x%08lx)\n", num, vector);

  if(vector != -1) {
    *addr = vector;
  }
  return oldvector;
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
#if IMPLEMENTED
    if(drive <= MAXDSK || exist[drive])
        return((LONG)&vme_dpb[drive]);
    else
        return(0L);	/* drive doesn't exist */
#endif
    return(0L);
}



/**
 * bconstat - Read status of output device
 *
 * Returns status in D0.L:
 * -1	device is ready	      
 * 0	device is not ready
 */

/* handle  = 0:PRT 1:AUX 2:CON */

LONG bios_8(WORD handle)	/* GEMBIOS character_output_status */
{
    switch(handle)
    {
    case h_PRT :
        return(dev_RDY);
    case h_AUX :
        return(dev_RDY);
    case h_CON :
        return(dev_RDY);
    default:
        return(NULL);			/* non-existnt devices ret null */
    }
}

/**
 * bios_9 - (mediach) See, if floppy has changed
 *
 * Returns media change status for specified drive in D0.L:
 *   0	Media definitely has not changed
 *   1	Media may have changed
 *   2	Media definitely has changed
 * where "changed" = "removed"
 */

LONG bios_9(WORD drv)
{
    /* Implemented by STonX -  STonX can not change Floppies */
    return(drv_mediach(drv));
}



/**
 * bios_a - (drvmap) Read drive bitmap
 *
 * Returns a long containing a bit map of logical drives on  the system,
 * with bit 0, the least significant bit, corresponding to drive A.
 * Note that if the BIOS supports logical drives A and B on a single
 * physical drive, it should return both bits set if a floppy is present.
 */

LONG bios_a()
{
    return(drvbits);
}



/*
 *  bios_b - (kbshift)  Shift Key mode get/set.
 *  two descriptions:
 *	o	If 'mode' is non-negative, sets the keyboard shift bits
 *		accordingly and returns the old shift bits.  If 'mode' is
 *		less than zero, returns the IBM0PC compatible state of the
 *		shift keys on the keyboard, as a bit vector in the low byte
 *		of D0
 *	o	The flag parameter is used to control the operation of
 *		this function.  If flag is not -1, it is copied into the
 *		state variable(s) for the shift, control and alt keys,
 *		and the previous key states are returned in D0.L.  If
 *		flag is -1, then only the inquiry is done.
 */

LONG bios_b(WORD flag)
{
    WORD oldy;

    if(flag == -1)
        return(shifty);         /* return bitvector of shift state */

    oldy = shifty;
    shifty=flag;

    return(oldy);
}


/**
 * bios_c - character device control channel input
 */

LONG bios_c(WORD handle, WORD length, BYTE *buffer)
{
    bios_null( handle, length, buffer ) ;	/*  for lint		*/
    return(ERR);
}


/**
 * bios_d - character device control channel output
 */

LONG bios_d(WORD handle, WORD length, BYTE *buffer)
{
    bios_null( handle, length, buffer) ;	/* for lint  */
    return(ERR);
}


/**
 * bios_e - block device control channel input
 */

LONG bios_e(WORD drive, WORD length, BYTE *buffer)
{
    bios_null( drive , length , buffer ) ;	/*  for lint	*/
    return(ERR);
}


/**
 * bios_f - block device control channel output
 */

LONG bios_f(WORD drive, WORD length, BYTE *buffer)
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


/**
 * bios_10 - character device exchange logical interrupt vector
 */

PFI bios_10(WORD handle, PFI address)
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
 * nullint - an unexpected interrupt has happened
 *
 * The 'a' parameter is really the first word in the information pushed
 * onto the stack for exception processing.
 */

void nullint(WORD a)
{
    WORD	*b ;

    b = &a ;
}
