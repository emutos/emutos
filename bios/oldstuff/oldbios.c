/*
 *  oldbios.c - obsolete stuff taken out from bios.c
 *
 * Copyright (c) 2001 Lineo, Inc. and
 *
 * Authors:
 *  SCC     Steve C. Cavender
 *  KTB     Karl T. Braun (kral)
 *  JSL     Jason S. Loveman
 *  EWF     Eric W. Fleischman
 *  LTG     Louis T. Garavaglia
 *  MAD     Martin Doering
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#define OLDSTUFF 0

#if OLDSTUFF
#define DBGBIOSC        TRUE

#define  HAVE_CON  TRUE
#define  HAVE_SER  FALSE
#define  HAVE_PAR  FALSE
#define  HAVE_MOU  FALSE
#define  HAVE_DSK  FALSE

#ifndef COMMENT
#define COMMENT 0
#endif

#define M0101060301     FALSE

#define PRTVER 0        /* set true to print bdos & bios time/date version */

#define SUPSIZ 1024     /* common supervisor stack size (in words) */

#endif /* OLDSTUFF */



/*==== External declarations ==============================================*/


#if OLDSTUFF

#if HAVE_SIO
extern LONG sinstat();          /* found in siostat.c */
#endif

extern LONG format();           /* found in disk.c */

extern BPB vme_dpb [];          /* found in disk.c */

#endif /* OLDSTUFF */

extern LONG drv_mediach(WORD drive);    /* found in startup.s */
extern LONG drv_bpb(WORD drive);        /* found in startup.s */
extern LONG drv_rw(WORD r_w,            /* found in startup.s */
                   BYTE *adr,
                   WORD numb,
                   WORD first,
                   WORD drive);

/*==== Declarations =======================================================*/

#if OLDSTUFF

/* BIOS drive table definitions */
static WORD exist[4];          /* 1, if drive present */
static WORD known[4];          /* 1, if disk logged-in */

PFI charvec[5];     /* array of vectors to logical interrupt handlers */

#endif /* OLDSTUFF */


/*==== BIOS initialization ================================================*/
/*
 *   called from startup.s, this routine will do necessary bios initialization
 *   that can be done in hi level lang.  startup.s has the rest.
 */


#if 0 /* useless biosinit */

void biosinit()
{
    /*==== set up logical interrupt handlers for character devices =========*/
    chardev_init();             /* Initialize character devices */

#if OLDSTUFF

    /* Are these still needed? (MAD) */
    charvec[0] = (PFI) 0;
    charvec[1] = (PFI) 0;   /* siolox - disabled */
    charvec[2] = (PFI) 0;
    charvec[3] = (PFI) 0;   /* clklox - disabled */
    charvec[4] = (PFI) 0;   /* moulox - disabled */

#endif /* OLDSTUFF */

#if OLDSTUFF

    /* initialize drive variables */
    exist[0] = 0;       /* Drive A present */
    exist[1] = 0;       /* Drive B not present */
    exist[2] = 1;       /* Drive C present */
    exist[3] = 0;       /* Drive D not present */

    known[0] = 0;       /* not loggend in */
    known[1] = 0;       /* not loggend in */
    known[2] = 0;       /* not loggend in */
    known[3] = 0;       /* not loggend in */

#endif /* OLDSTUFF */

    bufl_init();

    /* initialize components */

    mfp_init();         /* init MFP, timers, USART */
    kbd_init();         /* init keyboard, disable mouse and joystick */
    midi_init();        /* init MIDI acia so that kbd acia irq works */
    clk_init();         /* init clock (dummy for emulator) */

  
#if OLDSTUFF

#if HAVE_SIO
    m400init();   /* initialize the serial I/O ports */
#endif

#if HAVE_PAR
    m410_init();  /* initialize the parallel printer ports */
#endif

#if HAVE_DSK
    initdsks();   /* send disk config info to disk controller */
#endif

#endif /* OLDSTUFF */

    /* returning to assembler patch OS via cartridge */
}

#endif /* useless biosinit */




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
 * bios_c - character device control channel input
 */

LONG bios_c(WORD handle, WORD length, BYTE *buffer)
{
    return(ERR);
}



/**
 * bios_d - character device control channel output
 */

LONG bios_d(WORD handle, WORD length, BYTE *buffer)
{
    return(ERR);
}



/**
 * bios_e - block device control channel input
 */

LONG bios_e(WORD drive, WORD length, BYTE *buffer)
{
    return(ERR);
}



/**
 * bios_f - block device control channel output
 */

LONG bios_f(WORD drive, WORD length, BYTE *buffer)
{
#if OLDSTUFF
    if ((drive == 0) || (drive == 1))
    {
        if ((length == 1) && (buffer[0] == 1))  /* disk format */
        {
#if IMPLEMENTED
            return(format(drive));
#else
            return(E_OK);
#endif
        }
    }
#endif /* OLDSTUFF */
    return( EBADRQ ) ;
}



/**
 * bios_10 - character device exchange logical interrupt vector
 */

PFI bios_10(WORD handle, PFI address)
{
#if OLDSTUFF
    PFI temp;

    if (handle < 5)     /* note hard-coded device count */
    {
        temp = charvec[handle];
        if (address != (PFI)-1)
            charvec[handle] = address;
        return(temp);
    }
#endif /* OLDSTUFF */
    return((PFI)ERR);
}


