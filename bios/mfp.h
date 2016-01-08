/*
 *  mfp.h - header file for MFP defines
 *
 * Copyright (c) 2001 Martin Doering
 * Copyright (c) 2001-2014 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MFP_H
#define MFP_H

#include "portab.h"

#define CLOCKS_PER_SEC  200UL


#if CONF_WITH_MFP || CONF_WITH_TT_MFP

/*==== MFP memory mapping =================================================*/
typedef struct
{
        UBYTE   dum1;
        volatile UBYTE  gpip;   /* general purpose .. register */
        UBYTE   dum2;
        volatile UBYTE  aer;    /* active edge register              */
        UBYTE   dum3;
        volatile UBYTE  ddr;    /* data direction register           */
        UBYTE   dum4;
        volatile UBYTE  iera;   /* interrupt enable register A       */
        UBYTE   dum5;
        volatile UBYTE  ierb;   /* interrupt enable register B       */
        UBYTE   dum6;
        volatile UBYTE  ipra;   /* interrupt pending register A      */
        UBYTE   dum7;
        volatile UBYTE  iprb;   /* interrupt pending register B      */
        UBYTE   dum8;
        volatile UBYTE  isra;   /* interrupt in-service register A   */
        UBYTE   dum9;
        volatile UBYTE  isrb;   /* interrupt in-service register B   */
        UBYTE   dum10;
        volatile UBYTE  imra;   /* interrupt mask register A         */
        UBYTE   dum11;
        volatile UBYTE  imrb;   /* interrupt mask register B         */
        UBYTE   dum12;
        volatile UBYTE  vr;     /* vector register                   */
        UBYTE   dum13;
        volatile UBYTE  tacr;   /* timer A control register          */
        UBYTE   dum14;
        volatile UBYTE  tbcr;   /* timer B control register          */
        UBYTE   dum15;
        volatile UBYTE  tcdcr;  /* timer C + D control register      */
        UBYTE   dum16;
        volatile UBYTE  tadr;   /* timer A data register             */
        UBYTE   dum17;
        volatile UBYTE  tbdr;   /* timer B data register             */
        UBYTE   dum18;
        volatile UBYTE  tcdr;   /* timer C data register             */
        UBYTE   dum19;
        volatile UBYTE  tddr;   /* timer D data register             */
        UBYTE   dum20;
        volatile UBYTE  scr;    /* syncronous character register     */
        UBYTE   dum21;
        volatile UBYTE  ucr;    /* USART control register            */
        UBYTE   dum22;
        volatile UBYTE  rsr;    /* receiver status register          */
        UBYTE   dum23;
        volatile UBYTE  tsr;    /* transmitter status register       */
        UBYTE   dum24;
        volatile UBYTE  udr;    /* USART data register               */
} MFP;

#endif


#if CONF_WITH_TT_MFP

#define TT_MFP_BASE     ((MFP *)(0xfffffa80L))

void tt_mfp_init(void);

#endif


#if CONF_WITH_MFP

/*==== Defines ============================================================*/
#define MFP_BASE        ((MFP *)(0xfffffa00L))

/*==== Xbios functions ====================================================*/

void mfpint(WORD num, LONG vector);
void jdisint(WORD num);
void jenabint(WORD num);

/*==== internal functions =================================================*/

void mfp_init(void);
void setup_timer(MFP *mfp,WORD timer, WORD control, WORD data);

/* function which returns 1 if the timeout elapsed before the gpip changed */
int timeout_gpip(LONG delay);   /* delay in ticks */

/*==== Xbios functions ====================================================*/

void xbtimer(WORD timer, WORD control, WORD data, LONG vector);

#endif /* CONF_WITH_MFP */

/*==== internal functions =================================================*/

void init_system_timer(void);

/* "sieve" to get only the fourth interrupt, 0x1111 initially */
extern WORD timer_c_sieve;

#endif /* MFP_H */
