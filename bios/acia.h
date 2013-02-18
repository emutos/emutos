/*
 * acia.h - ACIA 6850 related things
 *
 * Copyright (c) 2001 Martin Doering
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef ACIA_H
#define ACIA_H

#include        "portab.h"

#if CONF_WITH_IKBD_ACIA || CONF_WITH_MIDI_ACIA

/*==== Defines ============================================================*/

/* constants for the ACIA registers */

/* baudrate selection and reset (Baudrate = clock/factor) */
#define ACIA_DIV1  0
#define ACIA_DIV16 1
#define ACIA_DIV64 2
#define ACIA_RESET 3

/* character format */
#define ACIA_D7E2S (0<<2)       /* 7 data, even parity, 2 stop */
#define ACIA_D7O2S (1<<2)       /* 7 data, odd parity, 2 stop */
#define ACIA_D7E1S (2<<2)       /* 7 data, even parity, 1 stop */
#define ACIA_D7O1S (3<<2)       /* 7 data, odd parity, 1 stop */
#define ACIA_D8N2S (4<<2)       /* 8 data, no parity, 2 stop */
#define ACIA_D8N1S (5<<2)       /* 8 data, no parity, 1 stop */
#define ACIA_D8E1S (6<<2)       /* 8 data, even parity, 1 stop */
#define ACIA_D8O1S (7<<2)       /* 8 data, odd parity, 1 stop */

/* transmit control */
#define ACIA_RLTID (0<<5)       /* RTS low, TxINT disabled */
#define ACIA_RLTIE (1<<5)       /* RTS low, TxINT enabled */
#define ACIA_RHTID (2<<5)       /* RTS high, TxINT disabled */
#define ACIA_RLTIDSB (3<<5)     /* RTS low, TxINT disabled, send break */

/* receive control */
#define ACIA_RID (0<<7)         /* RxINT disabled */
#define ACIA_RIE (1<<7)         /* RxINT enabled */

/* status fields of the ACIA */
#define ACIA_RDRF 1             /* Receive Data Register Full */
#define ACIA_TDRE (1<<1)        /* Transmit Data Register Empty */
#define ACIA_DCD  (1<<2)        /* Data Carrier Detect */
#define ACIA_CTS  (1<<3)        /* Clear To Send */
#define ACIA_FE   (1<<4)        /* Framing Error */
#define ACIA_OVRN (1<<5)        /* Receiver Overrun */
#define ACIA_PE   (1<<6)        /* Parity Error */
#define ACIA_IRQ  (1<<7)        /* Interrupt Request */

struct ACIA
{
    UBYTE ctrl;
    UBYTE dummy1;
    UBYTE data;
    UBYTE dummy2;
};

#endif /* CONF_WITH_IKBD_ACIA || CONF_WITH_MIDI_ACIA */

#if CONF_WITH_IKBD_ACIA
#define ACIA_IKBD_BASE (0xfffffc00L)
#define ikbd_acia (*(volatile struct ACIA*)ACIA_IKBD_BASE)
#endif

#if CONF_WITH_MIDI_ACIA
#define ACIA_MIDI_BASE (0xfffffc04L)
#define midi_acia (*(volatile struct ACIA*)ACIA_MIDI_BASE)
#endif

#endif /* ACIA_H */
