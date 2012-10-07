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
#include "iorec.h"
#include "mfp.h"
#include "serport.h"
#include "string.h"

#define RS232_BUFSIZE 256

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
static EXT_IOREC iorec_init = {
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

struct rsconf_struct {
    BYTE control;
    BYTE data;
};

static const struct rsconf_struct rsconf_data[] = {
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

void rsconf(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if CONF_WITH_MFP_RS232
    MFP *mfp=MFP_BASE;   /* set base address of MFP */

    if (baud >= 0 && baud <= 15) {
        rs232iorec.baudrate = baud;
        setup_timer(3, rsconf_data[baud].control, rsconf_data[baud].data);
    }

    if (ctrl >= 0)
        rs232iorec.flowctrl = ctrl;
    if (ucr >= 0)
        mfp->ucr = ucr;
    if (rsr >= 0)
        mfp->rsr = rsr;
    if (tsr >= 0)
        mfp->tsr = tsr;
    if (scr >= 0)
        mfp->scr = scr;
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
    /* Wait for character at the serial line */
    while(!bconstat1()) ;

#if CONF_WITH_MFP_RS232
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

LONG bconout1(WORD dev, WORD b)
{
    /* Wait for transmit buffer to become empty */
    while(!bcostat1()) ;

#if CONF_WITH_MFP_RS232
    /* Output to RS232 interface */
    MFP_BASE->udr = (char)b;
    return 1L;
#else
    return 0L;
#endif
}
