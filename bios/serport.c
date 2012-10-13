/*
 * serport.c - handle serial port(s)
 *
 * This file exists to centralise the handling of serial port hardware.
 *
 * Copyright (c) 2012 EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "config.h"
#include "portab.h"
#include "iorec.h"
#include "mfp.h"
#include "serport.h"
#include "string.h"

#if CONF_WITH_MFP_RS232
#define RS232_BUFSIZE 256
#else
#define RS232_BUFSIZE 4     /* save space if unused */
#endif

IOREC *rs232iorecptr;

/*
 * input-output buffers for rs232 in, rs232 out
 */
static char rs232ibuf[RS232_BUFSIZE];
static char rs232obuf[RS232_BUFSIZE];

/*
 * IOREC structures for rs232
 */
typedef struct {
    IOREC in;
    IOREC out;
    char baudrate;
    char flowctrl;  /* TODO, flow control */
} EXT_IOREC;

static EXT_IOREC rs232iorec;
static const EXT_IOREC iorec_init = {
    { rs232ibuf, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    { rs232obuf, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    B9600, FLOW_CTRL_NONE };


/*
 * initialise the serial port(s)
 */
void init_serport(void)
{
    /* initialise the IOREC */
    memcpy(&rs232iorec,&iorec_init,sizeof(EXT_IOREC));
    rs232iorecptr = &rs232iorec.in;
    rsconf(B9600, 0, 0x88, 1, 1, 0);
}

#if CONF_WITH_MFP_RS232

struct mfp_rs232_table {
    BYTE control;
    BYTE data;
};

static const struct mfp_rs232_table mfp_rs232_init[] = {
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

#endif /* CONF_WITH_MFP_RS232 */

ULONG rsconf(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if CONF_WITH_MFP_RS232
    MFP *mfp;
    const struct mfp_rs232_table *init;
    ULONG old;

    if (baud == -2)     /* wants current baud rate */
        return rs232iorec.baudrate;

    mfp = MFP_BASE;     /* set base address of MFP */

    /*
     * remember old ucr/rsr/tsr; note that we don't bother with scr, despite
     * the docs, because it's not useful and TOS doesn't return it either ...
     */
    old = ((ULONG)mfp->ucr<<24) | ((ULONG)mfp->rsr<<16) | (ULONG)mfp->tsr<<8;

    if ((baud >= MIN_BAUDRATE_CODE ) && (baud <= MAX_BAUDRATE_CODE)) {
        rs232iorec.baudrate = baud;
        init = &mfp_rs232_init[baud];
        setup_timer(3,init->control,init->data);
    }

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
        rs232iorec.flowctrl = ctrl;
    if (ucr >= 0)
        mfp->ucr = ucr;
    if (rsr >= 0)
        mfp->rsr = rsr;
    if (tsr >= 0)
        mfp->tsr = tsr;
    if (scr >= 0)
        mfp->scr = scr;

    return old;
#else
    return 0UL;
#endif
}

LONG bconstat1(void)
{
#if CONF_WITH_MFP_RS232
    /* Character available in the serial input buffer? */
    /* FIXME: We ought to use Iorec() for this... */
    if (MFP_BASE->rsr & 0x80)
        return -1;
    else
        return 0;
#else
    return 0;
#endif
}

LONG bconin1(void)
{
#if CONF_WITH_MFP_RS232
    /* Wait for character at the serial line */
    while(!bconstat1())
        ;

    /* Return character...
     * FIXME: We ought to use Iorec() for this... */
    return MFP_BASE->udr;
#else
    return 0;
#endif
}

LONG bcostat1(void)
{
#if CONF_WITH_MFP_RS232
    if (MFP_BASE->tsr & 0x80)
        return -1;
    else
        return 0;
#else
    return -1;
#endif
}

LONG bconout1(WORD b)
{
#if CONF_WITH_MFP_RS232
    /* Wait for transmit buffer to become empty */
    while(!bcostat1())
        ;

    /* Output to RS232 interface */
    MFP_BASE->udr = (char)b;
    return 1L;
#else
    return 0L;
#endif
}
