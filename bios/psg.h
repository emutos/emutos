/*
 * psg.h - Programmable Sound Generator YM-2149 
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef H_PSG_
#define H_PSH_
 
struct psg {
  UBYTE control;
  UBYTE pad0;
  UBYTE data;
};

#define PSG ((volatile struct psg *) 0xffff8800)

/* bits in PSG_MULTI register: */

/* TODO */

/* PSG registers */

#define PSG_MULTI  0x7
#define PSG_PORT_A 0xE
#define PSG_PORT_B 0xF

 
#endif /* H_PSG_ */
