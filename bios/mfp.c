/*
 * ikbd.c - Intelligent keyboard routines
 *
 * Copyright (c) 2001 Laurent Vogel, Martin Doering
 *
 * Authors:
 *  LAV   Laurent Vogel
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include	"portab.h"
#include	"bios.h"
#include	"mfp.h"
#include	"kprint.h"


/*==== Prototypes =========================================================*/



/*==== Defines ============================================================*/



/*==== kbinit - initialize the MFP ========================================*/
 
VOID	mfp_init (VOID)
{
    MFP *mfp=MFP_BASE;   /* set base address of MFP */

    cputs("[    ] MFP initialized ...\r");

    /* reset the MFP */
    mfp->gpip = 0x00;
    mfp->aer = 0x00;
    mfp->ddr = 0x00;

    mfp->iera = 0x00;
    mfp->ierb = 0x00;
    mfp->ipra = 0x00;
    mfp->iprb = 0x00;
    mfp->isra = 0x00;
    mfp->isrb = 0x00;
    mfp->imra = 0x00;
    mfp->imrb = 0x00;
    mfp->vr = 0x00;

    mfp->tacr = 0x00;
    mfp->tbcr = 0x00;
    mfp->tcdcr = 0x00;

    mfp->tadr = 0x00;
    mfp->tbdr = 0x00;
    mfp->tcdr = 0x00;
    mfp->tddr = 0x00;

    mfp->scr = 0x00;
#if 0
    mfp->ucr = 0x00;
    mfp->rsr = 0x00;
    mfp->tsr = 0x00;
    mfp->udr = 0x00;
#endif
    

    /* initialize the MFP */
    mfp->vr = 0x48;      /* vectors 0x40 to 0x4F, software end of interrupt */

    mfp->ddr &= ~0x08;
    mfp->aer &= ~0x08;

    mfp->imrb |= 0x40;
    mfp->isrb = ~0x40;
    mfp->iprb = ~0x40;
    mfp->ierb |= 0x40;

    cstatus(SUCCESS);
}
 

/*==== Setup timer ========================================================*/


VOID	timer_init(VOID)
{
    MFP *mfp=MFP_BASE;   /* set base address of MFP */

    BYTE mfp_reg;             /* One of the MFP's registers */
    
    cputs("[    ] Timer D initialized ...\r");

    mfp->tddr = 0x02;          /* set Timer D to 9600 baud */
    mfp_reg=mfp->tcdcr;        /* Get timer C/D-Configuration */
    mfp_reg&=0x70;            /* keep Timer C stuff intact */
    mfp_reg|=0x01;            /* timer D on /4 division mode */
    mfp->tcdcr=mfp_reg;        /* write it back */

    cstatus(SUCCESS);
}

/*==== Setup usart transmitter ============================================*/

VOID	usart_init(VOID)
{
    MFP *mfp=MFP_BASE;   /* set base address of MFP */

    cputs("[    ] USART initialized ...\r");

    mfp->tddr = 0x02;          /* set tsr register */
    mfp->tddr = 0x0a;          /* set ucr: 8bits, 1 start, 1 stop, no parity */

    cstatus(SUCCESS);
}

/*==== Clear keyboard interrupt ===========================================*/

VOID	clear_kbdint(VOID)
{
    MFP *mfp=MFP_BASE;  /* set base address of MFP */

    mfp->isrb = ~0x40;  /* signal end of keyboard interrupt */
}
