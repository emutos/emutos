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
#include        "acia.h"
#include        "kbd.h"   /* function clear_kbdint() */


/*==== External declarations ==============================================*/
extern VOID clear_kbdint(VOID);        /* from mfp.c */

/*==== Defines ============================================================*/
#define ACIA_MIDI_BASE (0xfffffc04L)

#define acia (*(volatile struct ACIA*)ACIA_MIDI_BASE)


/*==== Send Byte to keyboard ACIA =========================================*/
VOID    send_midi_acia(unsigned char c)
{
    while((acia.ctrl & 0x02)){};
    acia.data = c;
}

 

/*==== midiint - keyboard interrupt service routine =========================*/
/*
 *	MIDI Interrupt Service Routine for
 *	this routine is invoked upon receipt of an interrupt
 *	from the keyboard by the system.  it retrieves the
 *	scan code and, if no error was detected,
 *	puts the scan code into a queue to be retrieved at a more
 *	convenient time.
 */
 
VOID	midiint(VOID)
{
    REG UBYTE data;         /* byte received from MIDI */

    data = acia.data;            /* fetch the byte */
#if IMPLEMENTED
    midiqadd((KBQENTRY)(data & 0xFF));   /* put into queue as WORD */
#endif

    kprintf("BIOS: MIDI event happened ...");
    clear_kbdint();      /* signal end of interrupt */
    
}



/*==== midi_init - initialize the MIDI acia ==================*/
/*
 *	FUNCTION:  This routine is needed for the keyboard to
 *      work, since the IRQ is shared between both ACIAs.
 */
 
void midi_init(VOID)
{
    cprintf("[    ] MIDI ACIA initialized ...\r");

    /* initialize midi ACIA */
    acia.ctrl =
        ACIA_RESET;     /* master reset */

    acia.ctrl =
        ACIA_RID|       /* disable interrupts */
        ACIA_RLTID|     /* RTS low, TxINT disabled */
        ACIA_DIV16|     /* clock/16 */
        ACIA_D8N1S;  /* 8 bit, 1 stop, no parity */

    cstatus(SUCCESS);
}
