/*
 *  mfp.h - header file for MFP defines
 *
 * Copyright (c) 2001-2013 Martin Doering
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

#if CONF_WITH_MFP

#define IRQ_SPURIOUS      (IRQ_MACHSPEC | 0)

/* auto-vector interrupts */
#define IRQ_AUTO_1        (IRQ_MACHSPEC | 1)
#define IRQ_AUTO_2        (IRQ_MACHSPEC | 2)
#define IRQ_AUTO_3        (IRQ_MACHSPEC | 3)
#define IRQ_AUTO_4        (IRQ_MACHSPEC | 4)
#define IRQ_AUTO_5        (IRQ_MACHSPEC | 5)
#define IRQ_AUTO_6        (IRQ_MACHSPEC | 6)
#define IRQ_AUTO_7        (IRQ_MACHSPEC | 7)

/* ST-MFP interrupts */
#define IRQ_MFP_BUSY      (IRQ_MACHSPEC | 8)
#define IRQ_MFP_DCD       (IRQ_MACHSPEC | 9)
#define IRQ_MFP_CTS       (IRQ_MACHSPEC | 10)
#define IRQ_MFP_GPU       (IRQ_MACHSPEC | 11)
#define IRQ_MFP_TIMD      (IRQ_MACHSPEC | 12)
#define IRQ_MFP_TIMC      (IRQ_MACHSPEC | 13)
#define IRQ_MFP_ACIA      (IRQ_MACHSPEC | 14)
#define IRQ_MFP_FDC       (IRQ_MACHSPEC | 15)
#define IRQ_MFP_ACSI      IRQ_MFP_FDC
#define IRQ_MFP_FSCSI     IRQ_MFP_FDC
#define IRQ_MFP_IDE       IRQ_MFP_FDC
#define IRQ_MFP_TIMB      (IRQ_MACHSPEC | 16)
#define IRQ_MFP_SERERR    (IRQ_MACHSPEC | 17)
#define IRQ_MFP_SEREMPT   (IRQ_MACHSPEC | 18)
#define IRQ_MFP_RECERR    (IRQ_MACHSPEC | 19)
#define IRQ_MFP_RECFULL   (IRQ_MACHSPEC | 20)
#define IRQ_MFP_TIMA      (IRQ_MACHSPEC | 21)
#define IRQ_MFP_RI        (IRQ_MACHSPEC | 22)
#define IRQ_MFP_MMD       (IRQ_MACHSPEC | 23)

/* SCC interrupts */
#define IRQ_SCCB_TX          (IRQ_MACHSPEC | 40)
#define IRQ_SCCB_STAT        (IRQ_MACHSPEC | 42)
#define IRQ_SCCB_RX          (IRQ_MACHSPEC | 44)
#define IRQ_SCCB_SPCOND      (IRQ_MACHSPEC | 46)
#define IRQ_SCCA_TX          (IRQ_MACHSPEC | 48)
#define IRQ_SCCA_STAT        (IRQ_MACHSPEC | 50)
#define IRQ_SCCA_RX          (IRQ_MACHSPEC | 52)
#define IRQ_SCCA_SPCOND      (IRQ_MACHSPEC | 54)


#define INT_CLK   24576     /* CLK while int_clk =2.456MHz and divide = 100 */
#define INT_TICKS 246       /* to make sched_time = 99.902... HZ */



/*=========================================================================*/
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




/*==== Defines ============================================================*/
#define MFP_BASE        ((MFP *)(0xfffffa00L))

/*==== Xbios functions ====================================================*/

void mfpint(WORD num, LONG vector);
void jdisint(WORD num);
void jenabint(WORD num);

/*==== internal functions =================================================*/

void mfp_init(void);
void setup_timer(WORD timer, WORD control, WORD data);

#endif /* CONF_WITH_MFP */

/*==== Xbios functions ====================================================*/

void xbtimer(WORD timer, WORD control, WORD data, LONG vector);

/*==== internal functions =================================================*/

void init_system_timer(void);

/* "sieve" to get only the fourth interrupt, 0x1111 initially */
extern WORD timer_c_sieve;

#endif /* MFP_H */
