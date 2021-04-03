/*
 * serport.c - handle serial port(s)
 *
 * This file exists to centralise the handling of serial port hardware.
 *
 * Copyright (C) 2013-2019 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "asm.h"
#include "chardev.h"
#include "cookie.h"
#include "delay.h"
#include "machine.h"
#include "has.h"
#include "mfp.h"
#include "scc.h"
#include "serport.h"
#include "string.h"
#include "tosvars.h"
#include "vectors.h"
#include "coldfire.h"
#include "amiga.h"
#include "ikbd.h"

/*
 * defines
 */
#define RS232_BUFSIZE   256     /* like Atari TOS */

#if CONF_WITH_SCC
#define RESET_RECOVERY_DELAY    delay_loop(reset_recovery_loops)
#define RECOVERY_DELAY          delay_loop(recovery_loops)
#endif

/*
 * function prototypes
 */
#if BCONMAP_AVAILABLE
static ULONG rsconf_dummy(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
static void init_bconmap(void);
#endif

#if CONF_WITH_SCC
static LONG bconstatA(void);
static LONG bconinA(void);
static LONG bcostatA(void);
static LONG bconoutA(WORD,WORD);
static ULONG rsconfA(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);

static LONG bconstatB(void);
static LONG bconinB(void);
static LONG bcostatB(void);
static ULONG rsconfB(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
#endif  /* CONF_WITH_SCC */

#if CONF_WITH_TT_MFP
static LONG bconstatTT(void);
static LONG bconinTT(void);
static LONG bcostatTT(void);
static LONG bconoutTT(WORD,WORD);
static ULONG rsconfTT(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
#endif  /* CONF_WITH_TT_MFP */

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
static UBYTE ibuf1[RS232_BUFSIZE], obuf1[RS232_BUFSIZE];
static const EXT_IOREC iorec_init = {
    { NULL, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    { NULL, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    B9600, FLOW_CTRL_NONE, 0x88, 0xff, 0xea };

#if BCONMAP_AVAILABLE
static MAPTAB maptable[4];

static EXT_IOREC iorec_dummy;
static const MAPTAB maptable_dummy =
    { char_dummy, char_dummy, char_dummy, charout_dummy, rsconf_dummy, &iorec_dummy };
static const MAPTAB maptable_mfp =
    { bconstat1, bconin1, bcostat1, bconout1, rsconf1, &iorec1 };
#endif  /* BCONMAP_AVAILABLE */

#if CONF_WITH_SCC
ULONG recovery_loops;
static EXT_IOREC iorecA, iorecB;
static UBYTE ibufA[RS232_BUFSIZE], obufA[RS232_BUFSIZE];
static UBYTE ibufB[RS232_BUFSIZE], obufB[RS232_BUFSIZE];
static const MAPTAB maptable_port_a =
    { bconstatA, bconinA, bcostatA, bconoutA, rsconfA, &iorecA };
static const MAPTAB maptable_port_b =
    { bconstatB, bconinB, bcostatB, bconoutB, rsconfB, &iorecB };
#endif  /* CONF_WITH_SCC */

#if CONF_WITH_TT_MFP
static EXT_IOREC iorecTT;
static UBYTE ibufTT[RS232_BUFSIZE], obufTT[RS232_BUFSIZE];
static const MAPTAB maptable_mfp_tt =
    { bconstatTT, bconinTT, bcostatTT, bconoutTT, rsconfTT, &iorecTT };
#endif  /* CONF_WITH_TT_MFP */

/*
 * MFP serial port i/o routines
 */
LONG bconstat1(void)
{
    /* Character available in the serial input buffer? */
    if (iorec1.in.head == iorec1.in.tail) {
        return 0;   /* iorec empty */
    }
    else {
        return -1;  /* not empty => input available */
    }
}

LONG bconin1(void)
{
    WORD old_sr;
    LONG value;

    /* Wait for character at the serial line */
    while(!bconstat1())
        ;

    /* Return character... */

    /* disable interrupts */
    old_sr = set_sr(0x2700);

    iorec1.in.head++;
    if (iorec1.in.head >= iorec1.in.size) {
        iorec1.in.head = 0;
    }
    value = *(UBYTE *)(iorec1.in.buf + iorec1.in.head);

    /* restore interrupts */
    set_sr(old_sr);
    return value;
}

LONG bcostat1(void)
{
#if CONF_WITH_COLDFIRE_RS232
    return coldfire_rs232_can_write() ? -1 : 0;
#elif CONF_WITH_MFP_RS232
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
    while(!bcostat1())
        ;

#if CONF_WITH_COLDFIRE_RS232
    coldfire_rs232_write_byte(b);
    return 1;
#elif CONF_WITH_MFP_RS232
    /* Output to RS232 interface */
    MFP_BASE->udr = (char)b;
    return 1L;
#else
    /* The above loop will never return */
    return 0L;
#endif
}

void push_serial_iorec(UBYTE data)
{
    WORD tail;

    tail = iorec1.in.tail + 1;
    if (tail >= iorec1.in.size) {
        tail = 0;
    }
    if (tail == iorec1.in.head) {
        /* iorec full, do nothing */
    } else {
        *((UBYTE *)(iorec1.in.buf + tail)) = data;
        iorec1.in.tail = tail;
    }
}

/*
 * MFP Rsconf() routines
 */
#if CONF_WITH_MFP_RS232
struct mfp_rs232_table {
    UBYTE control;
    UBYTE data;
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

/*
 * the following routine is called by the assembler interrupt handler.
 * it runs at interrupt level 6 & is therefore never interrupted.
 */
void mfp_rs232_rx_interrupt_handler(void)
{
    if (MFP_BASE->rsr & 0x80) {
        UBYTE data = MFP_BASE->udr;
#if CONF_SERIAL_CONSOLE && !CONF_SERIAL_CONSOLE_POLLING_MODE
        /* And append a new IOREC value into the IKBD buffer */
        push_ascii_ikbdiorec(data);
#else
        /* And append a new IOREC value into the serial buffer */
        push_serial_iorec(data);
#endif
    }

    /* clear the interrupt service bit */
    MFP_BASE->isra = 0xef;
}

#endif  /* CONF_WITH_MFP_RS232 */

#if CONF_WITH_MFP || CONF_WITH_TT_MFP
static ULONG rsconf_mfp(MFP *mfp, EXT_IOREC *iorec, WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    const struct mfp_rs232_table *init;
    ULONG old;

    if (baud == -2)     /* wants current baud rate */
        return iorec->baudrate;

    /*
     * remember old ucr/rsr/tsr; note that we don't bother with scr, despite
     * the docs, because it's not useful and TOS doesn't return it either ...
     */
    old = ((ULONG)mfp->ucr<<24) | ((ULONG)mfp->rsr<<16) | (ULONG)mfp->tsr<<8;

    if ((baud >= MIN_BAUDRATE_CODE ) && (baud <= MAX_BAUDRATE_CODE)) {
        iorec->baudrate = baud;
        init = &mfp_rs232_init[baud];
        setup_timer(mfp,3,init->control,init->data);
    }

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
        iorec->flowctrl = ctrl;
    if (ucr >= 0)
        mfp->ucr = ucr;
    if (rsr >= 0)
        mfp->rsr = rsr;
    if (tsr >= 0)
        mfp->tsr = tsr;
    if (scr >= 0)
        mfp->scr = scr;

    return old;
}
#endif

ULONG rsconf1(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if CONF_WITH_MFP_RS232
    return rsconf_mfp(MFP_BASE,&iorec1,baud,ctrl,ucr,rsr,tsr,scr);
#else
    return 0UL;
#endif  /* CONF_WITH_MFP_RS232 */
}

#if CONF_WITH_SCC
/*
 * SCC port A i/o routines
 */
static LONG bconstatA(void)
{
    SCC *scc = (SCC *)SCC_BASE;
    LONG rc;

    rc = (scc->portA.ctl & 0x01) ? -1L : 0L;
    RECOVERY_DELAY;

    return rc;
}

static LONG bconinA(void)
{
    SCC *scc = (SCC *)SCC_BASE;
    LONG data;

    while(!bconstatA())
        ;
    data = scc->portA.data & iorecA.datamask;
    RECOVERY_DELAY;

    return data;
}

static LONG bcostatA(void)
{
    SCC *scc = (SCC *)SCC_BASE;
    LONG rc;

    rc = (scc->portA.ctl & 0x04) ? -1L : 0L;
    RECOVERY_DELAY;

    return rc;
}

static LONG bconoutA(WORD dev, WORD b)
{
    SCC *scc = (SCC *)SCC_BASE;

    while(!bcostatA())
        ;
    scc->portA.data = (UBYTE)b;
    RECOVERY_DELAY;

    return 1L;
}

/*
 * SCC port B i/o routines
 */
static LONG bconstatB(void)
{
    SCC *scc = (SCC *)SCC_BASE;
    LONG rc;

    rc = (scc->portB.ctl & 0x01) ? -1L : 0L;
    RECOVERY_DELAY;

    return rc;
}

static LONG bconinB(void)
{
    SCC *scc = (SCC *)SCC_BASE;
    LONG data;

    while(!bconstatB())
        ;
    data = scc->portB.data & iorecB.datamask;
    RECOVERY_DELAY;

    return data;
}

static LONG bcostatB(void)
{
    SCC *scc = (SCC *)SCC_BASE;
    LONG rc;

    rc = (scc->portB.ctl & 0x04) ? -1L : 0L;
    RECOVERY_DELAY;

    return rc;
}

/* note that bconoutB() is global to support SCC_DEBUG_PRINT */
LONG bconoutB(WORD dev, WORD b)
{
    SCC *scc = (SCC *)SCC_BASE;

    while(!bcostatB())
        ;
    scc->portB.data = (UBYTE)b;
    RECOVERY_DELAY;

    return 1L;
}

/*
 * SCC Rsconf() routines
 */
/*
 * NOTE: the following time constants for the SCC are calculated
 * using a PCLK of 8.053976 MHz.  The maximum difference between
 * the specified & actual baud rates is approximately 0.84%.
 */
static const WORD scc_timeconst[] = {
    /* 19200 */  11,
    /*  9600 */  24,
    /*  4800 */  50,
    /*  3600 */  68,
    /*  2400 */  103,
    /*  2000 */  124,
    /*  1800 */  138,
    /*  1200 */  208,
    /*   600 */  417,
    /*   300 */  837,
    /*   200 */  1256,
    /*   150 */  1676,
    /* 134.5 */  1869,
    /*   110 */  2286,
    /*    75 */  3354,
    /*    50 */  5032
};

static void write_scc(PORT *port,UBYTE reg,UBYTE data)
{
    port->ctl = reg;
    RECOVERY_DELAY;
    port->ctl = data;
    RECOVERY_DELAY;
}

static ULONG rsconf_scc(PORT *port,EXT_IOREC *iorec,WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    ULONG old;

    if (baud == -2)     /* wants current baud rate */
        return iorec->baudrate;

    /*
     * retrieve old ucr/rsr/tsr/scr
     * according to the TT030 TOS Release notes, for non-MFP hardware,
     * we must return 0 for rsr and scr, and the only valid bit in the
     * tsr is bit 3.
     */
    old = (ULONG)(iorec->ucr) << 24;
    if (iorec->wr5 & 0x10)  /* break being sent? */
        old |= 0x0800;              /* yes, mark it in the returned pseudo-TSR */

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
        iorec->flowctrl = ctrl;

    /*
     * set baudrate from lookup table
     */
    if ((baud >= MIN_BAUDRATE_CODE ) && (baud <= MAX_BAUDRATE_CODE)) {
        WORD tc;
        iorec->baudrate = baud;
        tc = scc_timeconst[baud];
        write_scc(port,12,LOBYTE(tc));
        write_scc(port,13,HIBYTE(tc));
    }

    /*
     * handle ucr
     */
    if (ucr >= 0) {
        UBYTE bpc, mask, wr4, wr5;
        iorec->ucr = ucr;
        switch((ucr>>5)&0x03) {     /* isolate ucr bits/char code */
        case 3:     /* 5 bits */
            mask = 0x1f;
            bpc = 0x00;
            break;
        case 2:     /* 6 bits */
            mask = 0x3f;
            bpc = 0x40;
            break;
        case 1:     /* 7 bits */
            mask = 0x7f;
            bpc = 0x20;
            break;
        default:     /* 8 bits */
            mask = 0xff;
            bpc = 0x60;
            break;
        }
        iorec->datamask = mask;
        wr5 = (iorec->wr5&0x9f) | bpc;
        iorec->wr5 = wr5;       /* update tx bits/char in shadow wr5 */
        write_scc(port,5,wr5);          /* update real wr5 */
        write_scc(port,3,(bpc<<1)|0x01);/* update rx bits/char too */
        wr4 = 0x40 | ((ucr>>1)&0x0c);   /* set x16 clock & stop bits */
        if (ucr&0x02)                   /* even parity */
            wr4 |= 0x02;
        if (ucr&0x04)                   /* parity enable */
            wr4 |= 0x01;
        write_scc(port,4,wr4);
    }

    /*
     * handle tsr
     */
    if (tsr >= 0) {
        UBYTE wr5;
        wr5 = iorec->wr5;
        if (tsr & 0x08) {       /* break requested */
            if (!(wr5 & 0x10))      /* not currently breaking */
                wr5 |= 0x10;
        } else {                /* no break requested */
            if (wr5 & 0x10)         /* break in progress */
                wr5 &= ~0x10;
        }
        if (wr5 != iorec->wr5) {
            iorec->wr5 = wr5;
            write_scc(port,5,wr5);
        }
    }

    return old;
}

static ULONG rsconfA(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    SCC *scc = (SCC *)SCC_BASE;

    return rsconf_scc(&scc->portA,&iorecA,baud,ctrl,ucr,rsr,tsr,scr);
}

static ULONG rsconfB(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    SCC *scc = (SCC *)SCC_BASE;

    return rsconf_scc(&scc->portB,&iorecB,baud,ctrl,ucr,rsr,tsr,scr);
}

static const WORD SCC_init_string[] = {
    0x0444,     /* x16 clock mode, 1 stop bit, no parity */
    0x0104,     /* 'parity is special condition' */
    0x0260,     /* interrupt vector #s start at 0x60 (lowmem 0x180) */
    0x03c0,     /* Rx 8 bits/char, disabled */
    0x05e2,     /* Tx 8 bits/char, disabled, DTR, RTS */
    0x0600,     /* SDLC (n/a) */
    0x0700,     /* SDLC (n/a) */
    0x0901,     /* status low, vector includes status */
    0x0a00,     /* misc flags */
    0x0b50,     /* Rx/Tx clocks from baudrate generator output */
    0x0c18,     /* time const low = 24 | so rate = (24+2)*2/BR clock period */
    0x0d00,     /* time const hi = 0   | = 52/(8053976/16) => 9680 bps      */
    0x0e02,     /* baudrate generator source = PCLK (8MHz) */
    0x0e03,     /* ditto + enable baudrate generator */
    0x03c1,     /* Rx 8 bits/char, enabled */
    0x05ea,     /* Tx 8 bits/char, enabled, DTR, RTS */
    0x0f20,     /* CTS interrupt enable */
    0x0010,     /* reset external/status interrupts */
    0x0010,     /* reset again (necessary, see manual) */
    0x0117,     /* interrupts for Rx, Tx, special condition; parity is special */
    0x0901,     /* status low, master interrupt disable */
                /* NOTE: change above to 0x0909 to enable interrupts! */
    0xffff      /* end of table marker */
};

/*
 * initialise the SCC
 */
void scc_init(void)
{
    SCC *scc = (SCC *)SCC_BASE;
    const WORD *p;
    ULONG reset_recovery_loops;

    /* calculate delay times for SCC access: note that SCC PCLK is 8MHz */
    reset_recovery_loops = loopcount_1_msec / 1000; /* 8 cycles = 1 usec */
    recovery_loops = loopcount_1_msec / 2000;       /* 4 cycles = 0.5 usec */

    /* issue hardware reset */
    scc->portA.ctl = 0x09;
    RECOVERY_DELAY;
    scc->portA.ctl = 0xC0;
    RESET_RECOVERY_DELAY;

    /* initialise channel A */
    for (p = SCC_init_string; *p >= 0; p++)
        write_scc(&scc->portA,HIBYTE(*p),LOBYTE(*p));

    /* initialise channel B */
    for (p = SCC_init_string; *p >= 0; p++)
        write_scc(&scc->portB,HIBYTE(*p),LOBYTE(*p));

    /*
     * Enable routing of the SCC interrupt through the SCU like TOS does.
     * Even though interrupts are not used here, other programs might
     * install their own interrupt handlers and expect the interrupt
     * to be available to them.
     * Point interrupts to just_rte() in case a program enables them
     * before setting its own handler.
     */

     VEC_SCCB_TBE = just_rte;
     VEC_SCCB_EXT = just_rte;
     VEC_SCCB_RXA = just_rte;
     VEC_SCCB_SRC = just_rte;

     VEC_SCCA_TBE = just_rte;
     VEC_SCCA_EXT = just_rte;
     VEC_SCCA_RXA = just_rte;
     VEC_SCCA_SRC = just_rte;

     if (HAS_VME)
        *(volatile char *)VME_INT_MASK |= VME_INT_SCC;
}
#endif  /* CONF_WITH_SCC */

#if CONF_WITH_TT_MFP
/*
 * TT MFP i/o routines
 */
static LONG bconstatTT(void)
{
    /* Character available in the serial input buffer? */
    /* FIXME: We ought to use Iorec() for this... */
    if (TT_MFP_BASE->rsr & 0x80)
        return -1L;

    return 0L;
}

static LONG bconinTT(void)
{
    /* Wait for character at the serial line */
    while(!bconstatTT())
        ;

    /* Return character...
     * FIXME: We ought to use Iorec() for this... */
    return (LONG)TT_MFP_BASE->udr;
}

static LONG bcostatTT(void)
{
    if (TT_MFP_BASE->tsr & 0x80)
        return -1L;

    return 0L;
}

static LONG bconoutTT(WORD dev, WORD b)
{
    /* Wait for transmit buffer to become empty */
    while(!bcostatTT())
        ;

    /* Output to RS232 interface */
    TT_MFP_BASE->udr = (UBYTE)b;

    return 1L;
}

/*
 * TT Rsconf() routine
 */
static ULONG rsconfTT(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    return rsconf_mfp(TT_MFP_BASE,&iorecTT,baud,ctrl,ucr,rsr,tsr,scr);
}
#endif  /* CONF_WITH_TT_MFP */

#if BCONMAP_AVAILABLE
static ULONG rsconf_dummy(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    return 0UL;
}

/*
 * initialise the Bconmap() structures
 *
 * note: using IS_ARANYM below rather than (cookie_mch==MCH_ARANYM) avoids
 * producing unnecessary code for non-ARAnyM images
 */
static void init_bconmap(void)
{
    MAPTAB *maptabptr;
    int i;

    /* initialise with dummy entries */
    for (i = 0; i < 4; i++)
        memcpy(&maptable[i],&maptable_dummy,sizeof(MAPTAB));
    bconmap_root.maptab = maptable;
    bconmap_root.maptabsize = 1;
    bconmap_root.mapped_device = (cookie_mch==MCH_FALCON || IS_ARANYM) ? 7 : 6;

    /*
     * initialise the BCONMAP structure according to machine type first
     * and detected hardware second
     */
    memcpy(&maptable[0],&maptable_mfp,sizeof(MAPTAB));

    if ((cookie_mch == MCH_FALCON) || (cookie_mch == MCH_MSTE) || IS_ARANYM) {
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

    /* set up to use mapped device values */
    maptabptr = &maptable[bconmap_root.mapped_device-BCONMAP_START_HANDLE];
    bconstat_vec[1] = maptabptr->Bconstat;
    bconin_vec[1] = maptabptr->Bconin;
    bcostat_vec[1] = maptabptr->Bcostat;
    bconout_vec[1] = maptabptr->Bconout;
    rsconfptr = maptabptr->Rsconf;
    rs232iorecptr = maptabptr->Iorec;
}
#endif      /* BCONMAP_AVAILABLE */

/*
 * initialise the serial port(s)
 */
void init_serport(void)
{
    /* initialisation for device 1 */
    memcpy(&iorec1,&iorec_init,sizeof(EXT_IOREC));
    iorec1.in.buf = ibuf1;
    iorec1.out.buf = obuf1;

    rs232iorecptr = &iorec1;
    rsconfptr = rsconf1;

    /* initialisation for other devices if required */
#if CONF_WITH_SCC
    memcpy(&iorecA,&iorec_init,sizeof(EXT_IOREC));
    iorecA.in.buf = ibufA;
    iorecA.out.buf = obufA;
    memcpy(&iorecB,&iorec_init,sizeof(EXT_IOREC));
    iorecB.in.buf = ibufB;
    iorecB.out.buf = obufB;
#endif  /* CONF_WITH_SCC */

#if CONF_WITH_TT_MFP
    memcpy(&iorecTT,&iorec_init,sizeof(EXT_IOREC));
    iorecTT.in.buf = ibufTT;
    iorecTT.out.buf = obufTT;
    if (has_tt_mfp)
        rsconfTT(B9600, 0, 0x88, 1, 1, 0);  /* set default initial values for TT MFP */
#endif  /* CONF_WITH_TT_MFP */

#if BCONMAP_AVAILABLE
    memcpy(&iorec_dummy,&iorec_init,sizeof(EXT_IOREC));
    init_bconmap();
#endif

#ifdef MACHINE_AMIGA
    amiga_rs232_init();
#endif

#if !CONF_SERIAL_IKBD
    (*rsconfptr)(B9600, 0, 0x88, 1, 1, 0);
#endif

#if CONF_WITH_MFP_RS232
    /* Set up a handler for MFP receive buffer full interrupt */
    mfpint(MFP_RBF, (LONG) mfp_rs232_rx_interrupt);
#endif

#ifdef __mcoldfire__
    coldfire_rs232_enable_interrupt();
#endif
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
#endif  /* BCONMAP_AVAILABLE */
}
