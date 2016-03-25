/*
 * psg.h - Programmable Sound Generator YM-2149
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef PSG_H
#define PSG_H

struct psg {
  UBYTE control;
  UBYTE pad0;
  UBYTE data;
};

#define PSG ((volatile struct psg *) 0xffff8800)

/* bits in PSG_MULTI register */

#define PSG_PORTB_OUTPUT    0x80
#define PSG_PORTA_OUTPUT    0x40
#define PSG_PORT_MASK       (PSG_PORTB_OUTPUT|PSG_PORTA_OUTPUT)
#define PSG_NOISE_MASK      0x38
#define PSG_TONE_MASK       0x07
#define PSG_MIXER_MASK      (PSG_NOISE_MASK|PSG_TONE_MASK)

/* PSG registers */

#define PSG_MULTI  0x7
#define PSG_PORT_A 0xE
#define PSG_PORT_B 0xF


#endif /* PSG_H */
