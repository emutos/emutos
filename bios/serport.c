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
#include "chardev.h"
#include "cookie.h"
#include "machine.h"
#include "mfp.h"
#include "serport.h"
#include "string.h"
#include "tosvars.h"

/*
 * defines
 */
#if CONF_WITH_SERIAL_INTERRUPTS /* TODO */
#define RS232_BUFSIZE 256
#else
#define RS232_BUFSIZE 4         /* save space if buffers unused */
#endif

/*
 * global variables
 */
ULONG (*rsconfptr)(WORD,WORD,WORD,WORD,WORD,WORD);
EXT_IOREC *rs232iorecptr;

#if BCONMAP_AVAILABLE
BCONMAP bconmap_root;
#endif

/*
 * local variables
 */
static EXT_IOREC iorec1;
static char ibuf1[RS232_BUFSIZE], obuf1[RS232_BUFSIZE];
static const EXT_IOREC iorec_init = {
    { NULL, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    { NULL, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    B9600, FLOW_CTRL_NONE };

#if BCONMAP_AVAILABLE
static EXT_IOREC iorecA, iorecB, iorecTT, iorec_dummy;
static char ibufA[RS232_BUFSIZE], obufA[RS232_BUFSIZE];
static char ibufB[RS232_BUFSIZE], obufB[RS232_BUFSIZE];
static char ibufTT[RS232_BUFSIZE], obufTT[RS232_BUFSIZE];

static MAPTAB maptable[4];

static const MAPTAB maptable_dummy =
    { char_dummy, char_dummy, char_dummy, charout_dummy, rsconf_dummy, &iorec_dummy };
static const MAPTAB maptable_mfp =
    { bconstat1, bconin1, bcostat1, bconout1, rsconf1, &iorec1 };
static const MAPTAB maptable_port_a =
    { bconstatA, bconinA, bcostatA, bconoutA, rsconfA, &iorecA };
static const MAPTAB maptable_port_b =
    { bconstatB, bconinB, bcostatB, bconoutB, rsconfB, &iorecB };
static const MAPTAB maptable_mfp_tt =
    { bconstatTT, bconinTT, bcostatTT, bconoutTT, rsconfTT, &iorecTT };
#endif



/*
 * MFP serial port i/o routines
 */
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

#if CONF_WITH_SCC
/*
 * SCC port A i/o routines
 */
LONG bconstatA(void)
{
    return 0L;
}

LONG bconinA(void)
{
    return 0L;
}

LONG bcostatA(void)
{
    return -1L;
}

LONG bconoutA(WORD b)
{
    return 0L;
}

/*
 * SCC port B i/o routines
 */
LONG bconstatB(void)
{
    return 0L;
}

LONG bconinB(void)
{
    return 0L;
}

LONG bcostatB(void)
{
    return -1L;
}

LONG bconoutB(WORD b)
{
    return 0L;
}
#endif

#if CONF_WITH_TT_MFP
/*
 * TT MFP i/o routines
 */
LONG bconstatTT(void)
{
    return 0L;
}

LONG bconinTT(void)
{
    return 0L;
}

LONG bcostatTT(void)
{
    return -1L;
}

LONG bconoutTT(WORD b)
{
    return 0L;
}
#endif


/*
 * Rsconf() routines
 */
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

ULONG rsconf1(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if CONF_WITH_MFP_RS232
    MFP *mfp;
    const struct mfp_rs232_table *init;
    ULONG old;

    if (baud == -2)     /* wants current baud rate */
        return rs232iorecptr->baudrate;

    mfp = MFP_BASE;     /* set base address of MFP */

    /*
     * remember old ucr/rsr/tsr; note that we don't bother with scr, despite
     * the docs, because it's not useful and TOS doesn't return it either ...
     */
    old = ((ULONG)mfp->ucr<<24) | ((ULONG)mfp->rsr<<16) | (ULONG)mfp->tsr<<8;

    if ((baud >= MIN_BAUDRATE_CODE ) && (baud <= MAX_BAUDRATE_CODE)) {
        rs232iorecptr->baudrate = baud;
        init = &mfp_rs232_init[baud];
        setup_timer(3,init->control,init->data);
    }

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
        rs232iorecptr->flowctrl = ctrl;
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

#if BCONMAP_AVAILABLE
ULONG rsconfA(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if CONF_WITH_SCC
    ULONG old = 0UL;

    if (baud == -2)     /* wants current baud rate */
        return rs232iorecptr->baudrate;

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
        rs232iorecptr->flowctrl = ctrl;

    return old;
#else
    return 0UL;
#endif
}

ULONG rsconfB(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if CONF_WITH_SCC
    ULONG old = 0UL;

    if (baud == -2)     /* wants current baud rate */
        return rs232iorecptr->baudrate;

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
        rs232iorecptr->flowctrl = ctrl;

    return old;
#else
    return 0UL;
#endif
}

ULONG rsconfTT(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if CONF_WITH_MFP_TT
    ULONG old = 0UL;

    if (baud == -2)     /* wants current baud rate */
        return rs232iorecptr->baudrate;

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
        rs232iorecptr->flowctrl = ctrl;

    return old;
#else
    return 0UL;
#endif
}

ULONG rsconf_dummy(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    return 0UL;
}
#endif

/*
 * initialise the Bconmap() structures
 */
void init_bconmap(void)
{
#if BCONMAP_AVAILABLE
    MAPTAB *maptabptr;
    int i;

    /* initialise with dummy entries */
    for (i = 0; i < 4; i++)
        memcpy(&maptable[i],&maptable_dummy,sizeof(MAPTAB));
    bconmap_root.maptab = maptable;
    bconmap_root.maptabsize = 1;
    bconmap_root.mapped_device = (cookie_mch==MCH_FALCON) ? 7 : 6;

    /*
     * initialise the BCONMAP structure according to machine type first
     * and detected hardware second
     */
#if CONF_WITH_MFP_RS232
    memcpy(&maptable[0],&maptable_mfp,sizeof(MAPTAB));
#endif

    if ((cookie_mch == MCH_FALCON) || (cookie_mch == MCH_MSTE)) {
#if CONF_WITH_SCC
        if (has_scc) {
            memcpy(&maptable[1],&maptable_port_b,sizeof(MAPTAB));
            memcpy(&maptable[2],&maptable_port_a,sizeof(MAPTAB));
        }
#endif
        bconmap_root.maptabsize = 3;
    } else if (cookie_mch == MCH_TT) {
#if CONF_WITH_SCC
        if (has_scc) {
            memcpy(&maptable[1],&maptable_port_b,sizeof(MAPTAB));
            memcpy(&maptable[3],&maptable_port_a,sizeof(MAPTAB));
        }
#endif
#if CONF_WITH_TT_MFP
        if (has_tt_mfp)
            memcpy(&maptable[2],&maptable_mfp_tt,sizeof(MAPTAB));
#endif
        bconmap_root.maptabsize = 4;
    }
    /*
     * FIXME: the following is a temporary kludge and should be
     * removed when EmuTOS SCC support is available
     */
    memcpy(&maptable[1],&maptable_mfp,sizeof(MAPTAB));

    /* set up to use mapped device values */
    maptabptr = &maptable[bconmap_root.mapped_device-BCONMAP_START_HANDLE];
    bconstat_vec[1] = maptabptr->Bconstat;
    bconin_vec[1] = maptabptr->Bconin;
    bcostat_vec[1] = maptabptr->Bcostat;
    bconout_vec[1] = maptabptr->Bconout;
    rsconfptr = maptabptr->Rsconf;
    rs232iorecptr = maptabptr->Iorec;
#endif
}

/*
 * initialise the serial port(s)
 */
void init_serport(void)
{
    /* initialise the IORECs */
    memcpy(&iorec1,&iorec_init,sizeof(EXT_IOREC));
    iorec1.in.buf = ibuf1;
    iorec1.out.buf = obuf1;
#if BCONMAP_AVAILABLE
    memcpy(&iorecA,&iorec_init,sizeof(EXT_IOREC));
    iorecA.in.buf = ibufA;
    iorecA.out.buf = obufA;
    memcpy(&iorecB,&iorec_init,sizeof(EXT_IOREC));
    iorecB.in.buf = ibufB;
    iorecB.out.buf = obufB;
    memcpy(&iorecTT,&iorec_init,sizeof(EXT_IOREC));
    iorecTT.in.buf = ibufTT;
    iorecTT.out.buf = obufTT;
    memcpy(&iorec_dummy,&iorec_init,sizeof(EXT_IOREC));
#endif

    rs232iorecptr = &iorec1;
    rsconfptr = rsconf1;

    init_bconmap();

    (*rsconfptr)(B9600, 0, 0x88, 1, 1, 0);
}

LONG bconmap(WORD dev)
{
#if BCONMAP_AVAILABLE
    MAPTAB *maptabptr;
    WORD old_dev = bconmap_root.mapped_device;
    WORD map_index;

    if (dev == -1)      /* return currently-mapped device number */
        return old_dev;

    if (dev == -2)      /* return pointer */
        return (LONG) &bconmap_root;

    map_index = dev - BCONMAP_START_HANDLE;
    if ((map_index < 0) || (map_index >= bconmap_root.maptabsize))
        return 0L;      /* invalid device number */

    /*
     * we first save the values of the current low-memory vectors
     * in the 'old_dev' slot of the mapping table.  this preserves
     * any changes that may have been made to them.
     */
    maptabptr = &maptable[old_dev-BCONMAP_START_HANDLE];
    maptabptr->Bconstat = bconstat_vec[1];
    maptabptr->Bconin = bconin_vec[1];
    maptabptr->Bcostat = bcostat_vec[1];
    maptabptr->Bconout = bconout_vec[1];
    maptabptr->Rsconf = rsconfptr;
    maptabptr->Iorec = rs232iorecptr;

    /* now we update the low-memory vectors */
    maptabptr = &maptable[map_index];
    bconstat_vec[1] = maptabptr->Bconstat;
    bconin_vec[1] = maptabptr->Bconin;
    bcostat_vec[1] = maptabptr->Bcostat;
    bconout_vec[1] = maptabptr->Bconout;
    rsconfptr = maptabptr->Rsconf;
    rs232iorecptr = maptabptr->Iorec;

    bconmap_root.mapped_device = dev;   /* update current dev in mapping table */
    return old_dev;
#else
    return 0x2c;    /* return the function opcode */
#endif
}
