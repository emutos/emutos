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
#include	"kprint.h"
#include	"mfp.h"
#include        "tosvars.h"

/* the acia interrupt, in aciavecs.s */
extern void int_acia(void);

/* the timer C interrupt, in startup.s */
extern void int_timerc(void);


/*==== Prototypes =========================================================*/

/* setup the timer, but do not activate the interrupt */
static void setup_timer(WORD timer, WORD control, WORD data);


/*==== Defines ============================================================*/

WORD mfp_ctrl;

/* "sieve", to get only the fourth interrupt */
WORD timer_c_sieve;

/*==== kbinit - initialize the MFP ========================================*/
 
void	mfp_init (void)
{
    MFP *mfp=MFP_BASE;   /* set base address of MFP */

    mfp_ctrl = MFP_CTRL_NONE;  /* no flow control */

    /* reset the MFP registers */
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

    mfp->ucr = 0x00;
    mfp->rsr = 0x00;
    mfp->tsr = 0x00;
    mfp->udr = 0x00;

    /* initialize the MFP */
    mfp->vr = 0x48;      /* vectors 0x40 to 0x4F, software end of interrupt */


    /* setup IKBD and MIDI ACIA interrupt */
#if 0  /* LVL this is not necessary, since the regs are already null */
    mfp->ddr &= ~0x08;
    mfp->aer &= ~0x08;
#endif
    mfpint(6, (LONG)int_acia);
    
    /* timer C */
    timer_c_sieve = 0x1111;
    timer_ms = 20;
    /* ctrl = divide 64, data = 192 */
    xbtimer(2, 0x50, 192, (LONG)int_timerc); 
    
    /* timer D */
    rsconf(B9600, 0, 0x98, 1, 1, 0);
    /* TODO, flow control */
}
 

/*==== Clear keyboard interrupt ===========================================*/

void	clear_kbdint(void)
{
    MFP *mfp=MFP_BASE;  /* set base address of MFP */

    mfp->isrb = ~0x40;  /* signal end of keyboard interrupt */
}


/*==== xbios functions ===========================================*/

void mfpint(WORD num, LONG vector)
{
  num &= 0x0F;
  jdisint(num);
  *(LONG *)((0x40L + num)*4) = vector;
  jenabint(num);
}

static struct {
  BYTE control;
  BYTE data;
} rsconf_data[] = {
  { /* 19200 */  1, 1 }, 
  { /*  9600 */  1, 2 },
  { /*  4800 */  1, 4 },
  { /*  3600 */  1, 5 },
  { /*  2400 */  1, 8 },
  { /*  2000 */  1, 10 },
  { /*  1800 */  1, 11 },
  { /*  1200 */  1, 16 },
  { /*   600 */  1, 32 },
  { /*   300 */  1, 64 },
  { /*   200 */  1, 96 },
  { /*   150 */  1, 128 },
  { /*   134 */  1, 143 },
  { /*   110 */  1, 175 },
  { /*    75 */  2, 64 },
  { /*    50 */  2, 96 }, 
};


void rsconf(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
  MFP *mfp=MFP_BASE;   /* set base address of MFP */

  if(ctrl >= 0) mfp_ctrl = ctrl;
  if(ucr >= 0) mfp->ucr = ucr;
  if(rsr >= 0) mfp->rsr = rsr;
  if(tsr >= 0) mfp->tsr = tsr;
  if(scr >= 0) mfp->scr = scr;

  if(baud >= 0) {
    if(baud < 16) {
      setup_timer(3, rsconf_data[baud].control, rsconf_data[baud].data);
    }
  }
}

void jdisint(WORD num)
{
  MFP *mfp=MFP_BASE;   /* set base address of MFP */
  UBYTE i;

  num &= 0x0F;
  if(num >= 8) {
    i = 1 << (num - 8);
    mfp->imra &= ~i;
    mfp->iera &= ~i;
    mfp->ipra &= ~i;
    mfp->isra &= ~i;
  } else {
    i = 1 << num;
    mfp->imrb &= ~i;
    mfp->ierb &= ~i;
    mfp->iprb &= ~i;
    mfp->isrb &= ~i;
  }
}

void jenabint(WORD num)
{
  MFP *mfp=MFP_BASE;   /* set base address of MFP */
  UBYTE i;

  num &= 0x0F;
  if(num >= 8) {
    i = 1 << (num - 8);
    mfp->iera |= i;
    mfp->imra |= i;
  } else {
    i = 1 << num;
    mfp->ierb |= i;
    mfp->imrb |= i;
  }
}

/* setup the timer, but do not activate the interrupt */
static void setup_timer(WORD timer, WORD control, WORD data)
{
  MFP *mfp=MFP_BASE;   /* set base address of MFP */
  switch(timer) {
  case 0:  /* timer A */
    mfp->tacr = 0;
    mfp->tadr = data;
    mfp->tacr = control;
    break;
  case 1:  /* timer B */
    mfp->tbcr = 0;
    mfp->tbdr = data;
    mfp->tbcr = control;
    break;
  case 2:  /* timer C */
    mfp->tcdcr &= 0x0F;
    mfp->tcdr = data;
    mfp->tcdcr |= control & 0xF0;
    break;
  case 3:  /* timer D */
    mfp->tcdcr &= 0xF0;
    mfp->tddr = data;
    mfp->tcdcr |= control & 0x0F;
    break;
  default:
    return;
  }
}

static WORD timer_num[] = { 13, 8, 5, 4 };

void xbtimer(WORD timer, WORD control, WORD data, LONG vector)
{
  if(timer < 0 || timer > 3) return;
  setup_timer(timer, control, data);
  mfpint(timer_num[timer], vector);
}
