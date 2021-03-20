/*
 * mfp.c - handling of the Multi-Function Peripheral MFP 68901
 *
 * Copyright (C) 2001 Martin Doering
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "mfp.h"
#include "tosvars.h"
#include "vectors.h"
#include "coldfire.h"
#include "lisa.h"

#if CONF_WITH_MFP || CONF_WITH_TT_MFP

static void reset_mfp_regs(MFP *mfp)
{
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

    mfp->rsr = 0x00;
    mfp->tsr = 0x00;
}

#endif


#if CONF_WITH_TT_MFP

void tt_mfp_init(void)
{
    MFP *mfp = TT_MFP_BASE; /* set base address of MFP */

    reset_mfp_regs(mfp);    /* reset the MFP registers */
    mfp->vr = 0x58;         /* vectors 0x50 to 0x5F, software end of interrupt */
}

#endif


#if CONF_WITH_MFP

/*==== mfp_init - initialize the MFP ========================================*/

void mfp_init(void)
{
    MFP *mfp = MFP_BASE;    /* set base address of MFP */

    reset_mfp_regs(mfp);    /* reset the MFP registers */
    mfp->vr = 0x48;         /* vectors 0x40 to 0x4F, software end of interrupt */
}


/*==== xbios functions ===========================================*/

void mfpint(WORD num, LONG vector)
{
    num &= 0x0F;
    jdisint(num);
    *(LONG *)((0x40L + num)*4) = vector;
    jenabint(num);
}

void jdisint(WORD num)
{
    MFP *mfp=MFP_BASE;   /* set base address of MFP */
    UBYTE mask;

    num &= 0x0F;
    if(num >= 8) {
        mask = ~(1<<(num-8));
        mfp->imra &= mask;
        mfp->iera &= mask;
        mfp->ipra = mask;   /* note: IPRA/ISRA ignore '1' bits */
        mfp->isra = mask;
    } else {
        mask = ~(1<<num);
        mfp->imrb &= mask;
        mfp->ierb &= mask;
        mfp->iprb = mask;   /* note: IPRA/ISRA ignore '1' bits */
        mfp->isrb = mask;
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
void setup_timer(MFP *mfp, WORD timer, WORD control, WORD data)
{
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

static const WORD timer_num[] = { MFP_TIMERA, MFP_TIMERB, MFP_200HZ, MFP_TIMERD };

void xbtimer(WORD timer, WORD control, WORD data, LONG vector)
{
    if(timer < 0 || timer > 3)
        return;
    setup_timer(MFP_BASE,timer, control, data);
    mfpint(timer_num[timer], vector);
}

/* returns 1 if the timeout (in clock ticks) elapsed before gpip went low */
int timeout_gpip(LONG delay)
{
    MFP *mfp = MFP_BASE;
    LONG next = hz_200 + delay;

    while(hz_200 < next) {
        if((mfp->gpip & 0x20) == 0) {
            return 0;
        }
    }
    return 1;
}

#endif /* CONF_WITH_MFP */

/* "sieve", to get only the fourth interrupt */
WORD timer_c_sieve;

void init_system_timer(void)
{
    timer_c_sieve = 0x1111;
    timer_ms = 20;

#if !CONF_WITH_MFP
    vector_5ms = int_timerc;
#endif

#if CONF_COLDFIRE_TIMER_C
    coldfire_init_system_timer();
#elif CONF_WITH_MFP
    /* Timer C: ctrl = divide 64, data = 192 */
    xbtimer(2, 0x50, 192, (LONG)int_timerc);
#endif

#ifdef MACHINE_LISA
    lisa_init_system_timer();
#endif

    /* The timer will really be enabled when sr is set to 0x2500 or lower. */
}
