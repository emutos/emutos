/*
 * midi.c - MIDI routines
 *
 * Copyright (c) 2001 Martin Doering
 *
 * Authors:
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "bios.h"
#include "kprint.h"
#include "acia.h"
#include "iorec.h"
#include "asm.h"
#include "midi.h"


/*==== MIDI bios functions =========================================*/

LONG bconstat3(void)
{
  if(midiiorec.head == midiiorec.tail) {
    return 0;   /* iorec empty */
  } else {
    return -1;  /* not empty => input available */
  }
}

LONG bconin3(void)
{
  WORD old_sr;
  LONG value;

  while(!bconstat3()) 
    ;
  /* disable interrupts */
  old_sr = set_sr(0x2700);
  
  midiiorec.head ++;
  if(midiiorec.head >= midiiorec.size) {
    midiiorec.head = 0;
  }
  value = *(UBYTE *)(midiiorec.buf+midiiorec.head);
  
  /* restore interrupts */
  set_sr(old_sr);
  return value;
}


/* can we send a byte to the MIDI ACIA ? */
LONG bcostat3(void)
{
  if(midi_acia.ctrl & ACIA_TDRE) {
    return -1;  /* OK */
  } else {
    /* Data register not empty */
    return 0;   /* not OK */
  }
}

/* send a byte to the MIDI ACIA */
void bconout3(WORD dev, WORD c)
{
  while(! bcostat3())
    ;
  midi_acia.data = c;
}

/*==== MIDI xbios function =========================================*/

/* cnt = number of bytes to send less one */
void midiws(WORD cnt, LONG ptr)
{
  UBYTE *p = (UBYTE *)ptr;
  while(cnt-- >= 0) {
    bconout3(0, *p++);
  }
}


/*==== midi_init - initialize the MIDI acia ==================*/
/*
 *	FUNCTION:  This routine is needed for the keyboard to
 *      work, since the IRQ is shared between both ACIAs.
 */
 
void midi_init(void)
{
    /* initialize midi ACIA */
    midi_acia.ctrl =
        ACIA_RESET;     /* master reset */

    midi_acia.ctrl =
        ACIA_RID|       /* disable interrupts */
        ACIA_RLTID|     /* RTS low, TxINT disabled */
        ACIA_DIV16|     /* clock/16 */
        ACIA_D8N1S;  /* 8 bit, 1 stop, no parity */
}
