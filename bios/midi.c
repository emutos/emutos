/*
 * midi.c - MIDI routines
 *
 * Copyright (c) 2001 Martin Doering
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include	"portab.h"
#include	"bios.h"
#include	"kprint.h"


/*==== Prototypes =========================================================*/

/*==== Defines ============================================================*/
#define ACIA_IKBD_BASE (0xfffffc04L)

#define acia (*(volatile struct ACIA*)ACIA_BASE)

/*==== Global variables ===================================================*/
BYTE    scancode;



/*==== Send Byte to keyboard ACIA =========================================*/
VOID    send_midi_acia(unsigned char c)
{
    while((acia.ctrl & 0x02)){};
    acia.data = c;
}

 


/****************************************************************************
**  kbdecode - DECODE A SCANCODE FROM THE KEYBOARD
**
**	FUNCTION:  decode the scancode received from the keyboard
**		   and return the corresponding character.
**
*/

KBCHAR	mididecode(KBQENTRY kbqe , UWORD *status )
{
        return(0);
}


/*==== midiint - keyboard interrupt service routine =========================*/
/*
 *	keyboard Interrupt Service Routine for
 *	this routine is invoked upon receipt of an interrupt
 *	from the keyboard by the system.  it retrieves the
 *	scan code and, if no error was detected,
 *	puts the scan code into a queue to be retrieved at a more
 *	convenient time.
 */
 
VOID	midiint(VOID)
{
    MFP *mfp=mfp_base;          /* set base address of MFP */
    REG UBYTE scancode;         /* scan code received from keyboard */

    scancode = acia.data;            /* fetch the character */
#if IMPLEMENTED
    midiqadd((KBQENTRY)(scancode=0x00ff));/* put into queue as WORD */
#endif

    kprint("BIOS: MIDI event happened ...");
    mfp->isrb = ~0x40;                   /* signal end of interrupt */
}



/*==== kbread - get a character from the keyboard queue. ==================*/
/*
 *	Someone wants to read the keyboard.  So we see if there is any
 *	chars in the keyboard queue.  If so, we decode it and give it
 *	to the caller.  Otherwise, we return -1.
 */

KBCHAR	kbread(VOID)
{
    KBQENTRY	kbqent ;
    KBCHAR	kbchar ;
    UWORD	status ;

    while( kbqdel( &kbqent ) )
    {
        kbchar = kbdecode( kbqent , &status ) ;
        if( status == IN_CHAR )
            return( kbchar ) ;
    }

    /*  no valid characters available  */

    return( -1L ) ;
}



/*==== kbinit - initialize the keyboard ===================================*/
/*
 *	FUNCTION:  This routine resets the keyboard,
 *	  configures the MFP so we can get interrupts
 */
 
ERROR	kbd_init(VOID)
{

    /* initialize queue and set interrupt vector */
//    midiqinit() ;         /* init kbq  			*/

    /* initialize acias */
    acia.ctrl = 0x03; /* master reset */
    acia.ctrl = 0x15; /* no interrupts, clock/16, 8 bit, 1 stop, no parity */


    return(SUCCESS);
}
