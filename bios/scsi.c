/*
 * scsi.c - SCSI routines
 *
 * Copyright (C) 2018-2019 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * A note on arbitration
 * SCSI arbitration was optional in SCSI-1, and required in SCSI-2
 * (introduced in 1993) and later.  Any working used drive available
 * today (2018) is almost certainly SCSI-2 or later, and some will not
 * work without arbitration.  Therefore this driver always arbitrates.
 * Setting the 0x80 bit in location 16 of NVRAM, which formerly requested
 * the use of arbitration, is now just used to validate the host SCSI id
 * in bits 0-2 of that location.  If the 0x80 bit is clear, arbitration
 * is done with a hostid of DEFAULT_HOSTID.
 */

/*
 * A note on parity error checking
 * By design, this driver does not provide parity error checking.  Although
 * it might seem simple to add a check for it after DMA completes, this
 * does not work: a parity error is always detected.  It is surmised
 * that this is due to the bus state being somewhat undefined as soon
 * as DMA completes.  Proper detection of parity errors seems to require
 * the use of an interrupt handler that gets control at EOP (this is how
 * HDDRIVER does it, for example), but implementing this is a fairly big
 * task, especially for Falcon SCSI.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "scsi.h"
#include "asm.h"
#include "biosdefs.h"
#include "cookie.h"
#include "delay.h"
#include "disk.h"
#include "dma.h"
#include "gemerror.h"
#include "intmath.h"                /* for max() */
#include "machine.h"
#include "has.h"
#include "mfp.h"
#include "nvram.h"
#include "biosext.h"    /* for cache control routines */
#include "string.h"
#include "tosvars.h"
#include "vectors.h"

#if CONF_WITH_SCSI

/*
 * SCSI messages
 */
#define COMMAND_COMPLETE_MSG    0x00
#define EXTENDED_MSG            0x01        /* i.e. multibyte */
#define MESSAGE_REJECT_MSG      0x07
#define IDENTIFY_MSG            0x80


/*
 * SCSI commands
 */
#define REQUEST_SENSE   0x03
#define INQUIRY         0x12
#define READ_CAPACITY   0x25


/*
 * SCSI phase numbers (as returned by get_next_phase())
 */
#define DATA_OUT_PHASE  0
#define DATA_IN_PHASE   1
#define COMMAND_PHASE   2
#define STATUS_PHASE    3
#define MSG_OUT_PHASE   6
#define MSG_IN_PHASE    7


/*
 * other SCSI-related constants
 */
#define DEFAULT_HOSTID          7       /* if not available from NVRAM */
#define MAXSECS_PER_FSCSI_IO    16383   /* Falcon DMA chip limitation */
#define MAXSECS_PER_TTSCSI_IO   65535   /* TT limitation due to CDB used */


/*
 * TT030 SCSI DMA registers
 */
typedef struct
{
    UBYTE dummy01;
    UBYTE ptr_upper;        /* 4 bytes of DMA pointer */
    UBYTE dummy02;
    UBYTE ptr_uprmiddle;
    UBYTE dummy03;
    UBYTE ptr_lwrmiddle;
    UBYTE dummy04;
    UBYTE ptr_lower;
    UBYTE dummy05;
    UBYTE cnt_upper;        /* 4 bytes of DMA count */
    UBYTE dummy06;
    UBYTE cnt_uprmiddle;
    UBYTE dummy07;
    UBYTE cnt_lwrmiddle;
    UBYTE dummy08;
    UBYTE cnt_lower;
    ULONG residue;          /* residue bytes */
    UBYTE dummy09;
    UBYTE control;          /* control & status bits */
} SCSIDMA;
#define SCSIDMA_BASE    ((volatile SCSIDMA *)0xffff8700L)


/*
 * TT030 5380 (SCSI chip) registers
 */
typedef struct
{
    UBYTE dummy01;
    UBYTE data;             /* data register */
    UBYTE dummy02;
    UBYTE icr;              /* initiator command register */
    UBYTE dummy03;
    UBYTE mode;             /* mode register */
    UBYTE dummy04;
    UBYTE tcr;              /* target command register */
    UBYTE dummy05;
    UBYTE bus_status;       /* bus status (R) / select enable (W) */
#define select_enable   bus_status
    UBYTE dummy06;
    UBYTE dma_status;       /* dma & bus status (R) / start DMA send (W) */
#define dma_send        dma_status
    UBYTE dummy07[3];
    UBYTE reset;            /* reset (R) / start DMA initiator receive (W) */
#define dma_initrecv    reset
} SCSI;
#define SCSI_BASE   ((volatile SCSI *)0xffff8780L)


/*
 * we need the definition for TT_MFP_BASE for TT SCSI support, but the
 * definition (in mfp.h) is dependent on CONF_WITH_TT_MFP.  to avoid
 * dragging in TT MFP support, we define TT_MFP_BASE here if necessary.
 */
#ifndef TT_MFP_BASE
#define TT_MFP_BASE ((MFP *)(0xfffffa80L))
#endif


/*
 * Falcon030 mappings for SCSI register access via ACSI hardware (see dma.h)
 */
#define fscsi_data          0x88    /* data register */
#define fscsi_icr           0x89    /* initiator command register */
#define fscsi_mode          0x8a    /* mode register */
#define fscsi_tcr           0x8b    /* target command register */
#define fscsi_bus_status    0x8c    /* bus status (R) / select enable (W) */
#define fscsi_select_enable 0x8c
#define fscsi_dma_status    0x8d    /* dma & bus status (R) / start DMA send (W) */
#define fscsi_dma_send      0x018d
#define fscsi_reset         0x8f    /* reset (R) / start DMA initiator receive (W) */
#define fscsi_dma_initrecv  0x8f

#define FSCSI_BASE  DMA


/*
 * structure passed to do_scsi_io() / scsi_dispatcher()
 */
typedef struct
{
    UBYTE *cdbptr;                  /* command address */
    WORD cdblen;                    /* command length */
    UBYTE *bufptr;                  /* buffer address */
    LONG buflen;                    /* buffer length */
    ULONG xfer_time;                /* calculated, in ticks */
    UBYTE mode;                     /* see below */
#define WRITE_MODE  0x01
#define DMA_MODE    0x02
    UBYTE next_msg_out;             /* next msg to send */
    UBYTE msg_in;                   /* first msg byte received */
    UBYTE status;                   /* (last) status byte received */
} CMDINFO;


/*
 * timeouts & other longish times
 */
#define RESET_TIME      (CLOCKS_PER_SEC)    /* 1 second (SCSI spec says >250msec) */
#define BUSFREE_TIMEOUT (CLOCKS_PER_SEC)    /* 1 second (HDDRIVER has 60 seconds ...) */
#define SELECTION_TIMEOUT (CLOCKS_PER_SEC/4)/* 250msec (SCSI spec) */
#define SHORT_TIMEOUT   (CLOCKS_PER_SEC)    /* timeout for most things: 1 second */

/*
 * the following is used to check for broken hardware or, more likely, an
 * incomplete emulation of the SCSI chip (e.g. Hatari's Falcon emulation).
 * during arbitration, we wait for bus free before we set the 5380's
 * ARBITRATE bit; so the AIP bit should be set within a time equal to:
 *      Bus settle delay (400 nsec) + Bus set delay (1.8 usec)
 * i.e. 2.2 usec.  we allow 2 timer ticks due to timer resolution.
 */
#define AIP_TIMEOUT     2

/*
 * the timeout for data transfers depends on the data length: we use
 * BYTES_PER_CLOCK to calculate the timeout, which means that we assume
 * the data transfer rate is at least (CLOCKS_PER_SEC * BYTES_PER_CLOCK)
 */
#define BYTES_PER_CLOCK 1024                /* convenient for division */


/*
 * delay macros
 */
#define arbitration_delay() delay_loop(arbitration_delay_count)
#define bus_clear_delay()   delay_loop(bus_clear_count)
#define bus_settle_delay()  delay_loop(bus_settle_count)
#define deskew2_delay()     delay_loop(deskew2_count)
#define rst_hold_delay()    delay_loop(rst_hold_count)
#define toggle_delay()      delay_loop(toggle_delay_count)


/*
 * internal error codes
 */
#define PHASE_CHANGE        -1              /* not really an error, signals end of phase */
#define TIMEOUT_ERROR       -2
#define PHASE_ERROR         -3
#define BUS_ERROR           -4


/*
 * local variables
 */
static UBYTE has_scsi;                      /* 0 => no scsi, otherwise type below */
#define TT_SCSI     1
#define FALCON_SCSI 2
static UBYTE hostid_bit;
static UWORD device_exists;                 /* device bitmap: 1 => successful I/O */

static ULONG arbitration_delay_count;       /* counts used by delay macros */
static ULONG bus_clear_count;
static ULONG bus_settle_count;
static ULONG deskew2_count;
static ULONG rst_hold_count;
static ULONG toggle_delay_count;

#define REQSENSE_LENGTH 16
static UBYTE reqsense_buffer[REQSENSE_LENGTH];
#define INQUIRY_LENGTH  32
static UBYTE inquiry_buffer[INQUIRY_LENGTH];
#define CAPACITY_LENGTH 8


/*
 * function prototypes
 */
static LONG do_scsi_rw(UWORD rw, ULONG sector, UWORD count, UBYTE *buf, WORD dev);
static LONG scsi_capacity(WORD dev, ULONG *buffer);
static LONG scsi_inquiry(WORD dev, UBYTE *buffer);


/*
 * externally-visible SCSI routines
 */
void detect_scsi(void)
{
    has_scsi = 0;

#if CONF_WITH_SCSI
    /*
     * logically, we could/should check for the presence of the SCSI
     * registers themselves (e.g. at SCSI_BASE+1).  however, this causes
     * problems when running under Hatari 2.1 (at least) and earlier,
     * since its TT emulation emulates their presence, but does not
     * emulate the actual SCSI chip.  however, Hatari does not attempt
     * to emulate the presence of the SCSI DMA controller, so we use
     * that instead.
     */
    if (check_read_byte((LONG)SCSIDMA_BASE+1))
        has_scsi = TT_SCSI;
    else if (HAS_VIDEL)
        has_scsi = FALCON_SCSI;             /* must have Falcon SCSI */
#endif

    KDEBUG(("detect_scsi(): has_scsi = 0x%02x\n",has_scsi));
}

void scsi_init(void)
{
    UBYTE temp;
    MAYBE_UNUSED(temp);

    if (!has_scsi)
        return;

    /*
     * set up various SCSI delay times
     */
    arbitration_delay_count = loopcount_1_msec * 24 / 10000;    /* 2.4 usec */
    bus_clear_count = loopcount_1_msec * 8 / 10000;     /* 800 nsec */
    bus_settle_count = loopcount_1_msec * 4 / 10000;    /* 400 nsec */
    deskew2_count = loopcount_1_msec * 9 / 100000;      /* 90 nsec */
    rst_hold_count = loopcount_1_msec / 40;             /* 25 usec */
    /*
     * the following delay is used for Falcon SCSI between toggling dma
     * out.  in Atari TOS 4, the delay is provided by an instruction
     * sequence which takes about 5usec on a Falcon.
     */
    toggle_delay_count = loopcount_1_msec / 200;        /* 5 usec */

    /* initially, we don't know if there are any devices */
    device_exists = 0;

    /*
     * get system SCSI id value from NVRAM (if available) & save
     */
    hostid_bit = 1 << DEFAULT_HOSTID;
#if CONF_WITH_NVRAM
    if (nvmaccess(0, 16, 1, &temp) == 0)
        if (temp & 0x80)
            hostid_bit = 1 << (temp&0x07);
#endif

    KDEBUG(("SCSI hostid_bit = 0x%02x\n",hostid_bit));
}

/*
 * perform miscellaneous non-data-transfer functions
 */
LONG scsi_ioctl(WORD dev, UWORD ctrl, void *arg)
{
    LONG rc = ERR;
    UBYTE *name = arg;
    ULONG *info = arg;

    switch(ctrl) {
    case GET_DISKINFO:
        rc = scsi_capacity(dev, info);
        if (rc == 0L)
            info[0]++;                  /* capacity = last sector + 1 */
        break;
    case GET_DISKNAME:
        rc = scsi_inquiry(dev, inquiry_buffer);
        if (rc != 0L)
            break;
        if (inquiry_buffer[0] & 0x1f)   /* if not a disk,   */
        {
            rc = EUNDEV;                /* we didn't see it */
            break;
        }
        memcpy(name, inquiry_buffer+8, 24);
        name[24] = '\0';
        break;
    case GET_MEDIACHANGE:
        rc = MEDIANOCHANGE;
        break;
    }

    KDEBUG(("scsi_ioctl(%d, %u) returned %ld\n", dev, ctrl, rc));
    return rc;
}

/*
 * perform data transfer functions
 */
LONG scsi_rw(UWORD rw, ULONG sector, UWORD count, UBYTE *buf, WORD dev)
{
    UWORD maxsecs_per_io, numsecs = 0;
    BOOL use_tmpbuf = FALSE;
    int retry;
    LONG ret = 0L;
    UBYTE *p, *tmp_buf = NULL;

    rw &= RW_RW;    /* we just care about read or write for now */

    if (has_scsi == TT_SCSI)
        maxsecs_per_io = MAXSECS_PER_TTSCSI_IO;
    else            /* assumed to be FALCON_SCSI */
    {
        /*
         * the Falcon SCSI hardware requires that the buffer be word-aligned
         * and located in ST-RAM.  if it isn't, we use an intermediate buffer:
         * the FRB (if available) or dskbufp.
         */
        maxsecs_per_io = MAXSECS_PER_FSCSI_IO;
        if (IS_ODD_POINTER(buf) || !IS_STRAM_POINTER(buf))
        {
#if CONF_WITH_FRB
            tmp_buf = get_frb_cookie();
            if (maxsecs_per_io > FRB_SECS)
                maxsecs_per_io = FRB_SECS;
#endif
            if (!tmp_buf) {
                tmp_buf = dskbufp;
                if (maxsecs_per_io > DSKBUF_SECS)
                    maxsecs_per_io = DSKBUF_SECS;
            }
            use_tmpbuf = TRUE;
        }
    }

    while(count)
    {
        numsecs = (count>maxsecs_per_io) ? maxsecs_per_io : count;

        p = use_tmpbuf ? tmp_buf : buf;
        if (rw && use_tmpbuf)
            memcpy(p, buf, numsecs * SECTOR_SIZE);

        for (retry = 0; retry < 2; retry++)
            if ((ret=do_scsi_rw(rw, sector, numsecs, p, dev)) == 0)
                break;
        if (ret)
            break;

        if (!rw && use_tmpbuf)
            memcpy(buf, p, numsecs * SECTOR_SIZE);

        count -= numsecs;
        buf += numsecs * SECTOR_SIZE;
        sector += numsecs;
    }

    KDEBUG(("scsi_rw() (rw=%d,dev=%d,sector=%lu,numsecs=%u) returned %ld\n",
            rw, dev, sector, numsecs, ret));
    return ret;
}



/*
 * low-level routines
 */
/*
 * the following __inline__ routines simplify coding the kludgy
 * method that is required to access the Falcon SCSI controller
 */
static __inline__ void fscsi_put(UWORD reg, UWORD value)
{
    FSCSI_BASE->control = reg;
    FSCSI_BASE->data = value;
}

static __inline__ UWORD fscsi_get(UWORD reg)
{
    FSCSI_BASE->control = reg;
    return FSCSI_BASE->data;
}

static __inline__ void fscsi_and(UWORD reg, UWORD value)
{
    UWORD temp;

    FSCSI_BASE->control = reg;
    temp = FSCSI_BASE->data;
    FSCSI_BASE->control = reg;
    FSCSI_BASE->data = temp & value;
}

static __inline__ void fscsi_or(UWORD reg, UWORD value)
{
    UWORD temp;

    FSCSI_BASE->control = reg;
    temp = FSCSI_BASE->data;
    FSCSI_BASE->control = reg;
    FSCSI_BASE->data = temp | value;
}

static UBYTE get_data_reg(void)
{
    if (has_scsi == TT_SCSI)
        return SCSI_BASE->data;
    else
        return fscsi_get(fscsi_data);
}

static void put_data_reg(UBYTE b)
{
    if (has_scsi == TT_SCSI)
        SCSI_BASE->data = b;
    else
        fscsi_put(fscsi_data, b);
}

static UBYTE get_icr_reg(void)
{
   if (has_scsi == TT_SCSI)
        return SCSI_BASE->icr;
    else
        return fscsi_get(fscsi_icr);
}

static void put_icr_reg(UWORD value)
{
   if (has_scsi == TT_SCSI)
        SCSI_BASE->icr = value;
    else
        fscsi_put(fscsi_icr, value);
}

static void and_icr_reg(UWORD value)
{
    if (has_scsi == TT_SCSI)
        SCSI_BASE->icr &= value;
    else
        fscsi_and(fscsi_icr, value);
}

static void or_icr_reg(UWORD value)
{
    if (has_scsi == TT_SCSI)
        SCSI_BASE->icr |= value;
    else
        fscsi_or(fscsi_icr, value);
}

static void put_mode_reg(UWORD value)
{
   if (has_scsi == TT_SCSI)
        SCSI_BASE->mode = value;
    else
        fscsi_put(fscsi_mode, value);
}

static void and_mode_reg(UWORD value)
{
    if (has_scsi == TT_SCSI)
        SCSI_BASE->mode &= value;
    else
        fscsi_and(fscsi_mode, value);
}

static void or_mode_reg(UWORD value)
{
    if (has_scsi == TT_SCSI)
        SCSI_BASE->mode |= value;
    else
        fscsi_or(fscsi_mode, value);
}

static void put_tcr_reg(UWORD value)
{
   if (has_scsi == TT_SCSI)
        SCSI_BASE->tcr = value;
    else
        fscsi_put(fscsi_tcr, value);
}

static UBYTE get_bus_status_reg(void)
{
    if (has_scsi == TT_SCSI)
        return SCSI_BASE->bus_status;
    else
        return fscsi_get(fscsi_bus_status);
}

static UBYTE get_dma_status_reg(void)
{
    if (has_scsi == TT_SCSI)
        return SCSI_BASE->dma_status;
    else
        return fscsi_get(fscsi_dma_status);
}

static void put_select_enable_reg(UWORD value)
{
   if (has_scsi == TT_SCSI)
        SCSI_BASE->select_enable = value;
    else
        fscsi_put(fscsi_select_enable, value);
}

/*
 * return next phase encoded as values from 0-7 (MSG=4, C/D=2, I/O=1)
 */
static UBYTE get_next_phase(void)
{
    return (get_bus_status_reg()>>2) & 0x07;
}

static void set_output_phase(UBYTE phase)
{
    UBYTE dummy;
    MAYBE_UNUSED(dummy);

    if (has_scsi == TT_SCSI)
    {
        SCSI_BASE->tcr = phase;         /* assert new phase */
        SCSI_BASE->icr |= 0x01;         /* gate it on to bus */
        dummy = SCSI_BASE->reset;       /* reset parity, interrupts */
    }
    else                        /* handle Falcon SCSI */
    {
        fscsi_put(fscsi_tcr, phase);    /* assert new phase */
        fscsi_or(fscsi_icr, 0x01);      /* gate it on to bus */
        dummy = fscsi_get(fscsi_reset); /* reset parity, interrupts */
    }
}

static void set_input_phase(UBYTE phase)
{
    UBYTE dummy;
    MAYBE_UNUSED(dummy);

    if (has_scsi == TT_SCSI)
    {
        SCSI_BASE->tcr = phase;         /* assert new phase */
        SCSI_BASE->icr &= 0xfe;         /* degate bus */
        dummy = SCSI_BASE->reset;       /* reset parity, interrupts */
    }
    else                        /* handle Falcon SCSI */
    {
        fscsi_put(fscsi_tcr, phase);    /* assert new phase */
        fscsi_and(fscsi_icr, 0xfe);     /* degate bus */
        dummy = fscsi_get(fscsi_reset); /* reset parity, interrupts */
    }
}

static void setup_tt_dma(CMDINFO *info)
{
    UBYTE *p;

    p = (UBYTE *)&info->bufptr;     /* move data address to DMA pointer */
    SCSIDMA_BASE->ptr_upper = *p++;
    SCSIDMA_BASE->ptr_uprmiddle = *p++;
    SCSIDMA_BASE->ptr_lwrmiddle = *p++;
    SCSIDMA_BASE->ptr_lower = *p;

    p = (UBYTE *)&info->buflen;     /* move data length to DMA byte count */
    SCSIDMA_BASE->cnt_upper = *p++;
    SCSIDMA_BASE->cnt_uprmiddle = *p++;
    SCSIDMA_BASE->cnt_lwrmiddle = *p++;
    SCSIDMA_BASE->cnt_lower = *p;
}

static void toggle_dmaout(BOOL write)
{
    UWORD start = write ? 0x0090 : 0x0190;

    FSCSI_BASE->control = start;
    toggle_delay();
    FSCSI_BASE->control = start ^ 0x0100;
    toggle_delay();
}

static void setup_falcon_dma(CMDINFO *info)
{
    fscsi_or(fscsi_mode, 0x02);     /* set DMA mode, we are initiator */

    set_dma_addr(info->bufptr);             /* move 24-bit data address to DMA ptr */
    toggle_dmaout(info->mode&WRITE_MODE);   /* set DMA direction bit */
    FSCSI_BASE->data = (info->buflen + SECTOR_SIZE - 1) / SECTOR_SIZE;

    while(FSCSI_BASE->modectl & DMA_MCBIT3) /* Falcon TOS does this, so do we */
        ;
}

/*
 * decide if we should use DMA
 */
static BOOL use_dma(CMDINFO *info)
{
    /*
     * for TT SCSI, we always use DMA
     */
    if (has_scsi == TT_SCSI)
        return TRUE;

    /*
     * for Falcon SCSI, it depends on the data length
     */
    if ((info->buflen < SECTOR_SIZE)    /* for small I/Os or I/Os that */
     || (info->buflen & 0x0f))          /* aren't a multiple of 16,    */
        return FALSE;                   /* we'll do programmed i/o     */

    return TRUE;
}

/*
 * initialise DMA for data out processing
 */
static void init_data_out(CMDINFO *info)
{
    if (has_scsi == TT_SCSI)        /* handle TT SCSI DMA */
    {
        setup_tt_dma(info);
        SCSI_BASE->mode |= 0x02;            /* set DMA mode, we are initiator */
        SCSI_BASE->dma_send = 0x00;         /* start DMA send */
        SCSIDMA_BASE->control = 0x01;       /* set DMA chip to DMA OUT */
        SCSIDMA_BASE->control = 0x03;       /*  & enable DMA           */
    }
    else                            /* handle Falcon SCSI DMA */
    {
        setup_falcon_dma(info);
        fscsi_put(fscsi_dma_send, 0x00);    /* start DMA send */
        FSCSI_BASE->control = 0x0100;       /* & turn on DMA  */
    }
}

/*
 * initialise DMA for data in processing
 */
static void init_data_in(CMDINFO *info)
{
    if (has_scsi == TT_SCSI)        /* handle TT SCSI DMA */
    {
        setup_tt_dma(info);
        SCSIDMA_BASE->control = 0x00;       /* set DMA chip to DMA IN */
        SCSIDMA_BASE->control = 0x02;       /*  & enable DMA          */
        SCSI_BASE->mode |= 0x02;            /* set DMA mode, we are initiator */
        SCSI_BASE->dma_initrecv = 0x00;     /* start DMA initiator receive */
    }
    else                            /* handle Falcon SCSI DMA */
    {
        setup_falcon_dma(info);
        fscsi_put(fscsi_dma_initrecv, 0x00);/* start DMA initiator receive */
        FSCSI_BASE->control = 0x0000;       /*  & turn on DMA              */
    }
}

/* wait for DMA to complete */
static int wait_dma_complete(ULONG timeout)
{
    UBYTE dummy;
    MAYBE_UNUSED(dummy);

    if (has_scsi == TT_SCSI)
    {
        while(!(TT_MFP_BASE->gpip & 0x80))  /* until we get IRQ */
        {
            if (!(TT_MFP_BASE->gpip & 0x20))/* got DMAC interrupt ? */
                if (SCSIDMA_BASE->control & 0x80)
                    return BUS_ERROR;       /* exit iff bus error */
            if (hz_200 >= timeout)
                return TIMEOUT_ERROR;
        }
        dummy = SCSI_BASE->reset;           /* reset parity, interrupts */
        SCSIDMA_BASE->control = 0x00;       /* disable DMA chip */
        SCSI_BASE->mode &= 0xfd;            /* disable DMA mode */
        SCSI_BASE->icr = 0x00;              /* unassert ACK, BSY, SEL, ATN, data bus */
        return 0;
    }

    /* handle Falcon SCSI */
    while(MFP_BASE->gpip & 0x20)            /* until we get interrupt */
        if (hz_200 >= timeout)
            return TIMEOUT_ERROR;
    dummy = fscsi_get(fscsi_reset);         /* reset parity, interrupts */
    fscsi_and(fscsi_mode, 0xfd);            /* disable DMA mode */
    fscsi_put(fscsi_icr, 0x00);             /* unassert ACK, BSY, SEL, ATN, data bus */
    return 0;
}

/* wait for phase change */
static int wait_phase_change(ULONG timeout)
{
    while(get_dma_status_reg() & 0x08)      /* phase match */
        if (hz_200 >= timeout)
            return TIMEOUT_ERROR;

    return 0;
}

/* wait for pending data transfer request OR phase mismatch */
static int wait_req(ULONG timeout)
{
    while(get_dma_status_reg() & 0x08)      /* while phases match */
    {
        if (get_bus_status_reg() & 0x20)    /* got REQ ? */
            break;
        if (hz_200 >= timeout)
            return TIMEOUT_ERROR;
    }

    if ((get_dma_status_reg() & 0x08) == 0)
        return PHASE_CHANGE;                /* takes precedence over REQ */

    return 0;
}

/*
 * handle TT-style DMA cleanup: the command has completed ok but
 * we still need to get any data bytes that weren't DMA'd
 */
static void cleanup_tt_dma(CMDINFO *info)
{
    WORD bytes_remaining;
    ULONG residue;
    union
    {
        UBYTE *ptr;
        UBYTE byte[4];
    } u;
    UBYTE *p, *q;

    bytes_remaining = SCSIDMA_BASE->ptr_lower & 0x03;
    if (bytes_remaining == 0)
        return;                             /* everything was DMA'd */

    residue = SCSIDMA_BASE->residue;        /* contains remaining bytes */

    u.byte[0] = SCSIDMA_BASE->ptr_upper;    /* build target pointer */
    u.byte[1] = SCSIDMA_BASE->ptr_uprmiddle;
    u.byte[2] = SCSIDMA_BASE->ptr_lwrmiddle;
    u.byte[3] = SCSIDMA_BASE->ptr_lower & 0xfc;

    for (p = u.ptr, q = (UBYTE *)&residue; bytes_remaining; bytes_remaining--)
        *p++ = *q++;
}

/* general SCSI cleanup */
static void cleanup_scsi(void)
{
    UBYTE dummy;
    MAYBE_UNUSED(dummy);

    if (has_scsi == TT_SCSI)
    {
        dummy = SCSI_BASE->reset;
        SCSIDMA_BASE->control = 0x00;   /* disable DMA chip */
        SCSI_BASE->mode = 0x00;         /* disable DMA mode */
        SCSI_BASE->icr = 0x00;          /* unassert ACK, BSY, SEL, ATN, data bus */
    }
    else                            /* handle Falcon SCSI */
    {
        dummy = fscsi_get(fscsi_reset);
        FSCSI_BASE->control = 0x0080;   /* reset DMA chip */
        fscsi_put(fscsi_mode, 0x00);    /* disable DMA mode */
        fscsi_put(fscsi_icr, 0x00);     /* unassert ACK, BSY, SEL, ATN, data bus */
    }
}

/*
 * perform SCSI handshake (ACK/REQ)
 *
 * NOTE: There must be a delay of at least 10ns (cable skew delay) between
 *  detecting a change in REQ and making the corresponding change to ACK.
 *  For output, there must be an additional 45ns (deskew delay) between
 *  putting data on the bus and asserting ACK.
 *  These delays are implicitly provided by instruction execution times:
 *  a 100MHz system with 1 cycle instructions would take 10ns for one
 *  instruction.  It might just be possible to cause problems on a fast
 *  68060 _if_ we executed all code inline ...
 */
static int handshake(ULONG timeout)
{
    int ret = 0;

    or_icr_reg(0x11);           /* assert ACK, gate data bus */
    while(get_bus_status_reg() & 0x20)  /* wait for REQ low */
    {
        if (hz_200 >= timeout)
        {
            ret = TIMEOUT_ERROR;
            break;
        }
    }
    and_icr_reg(0xef);          /* clear ACK */

    return ret;
}

/* send a byte, with handshaking & timeout handling */
static int send_byte(UBYTE b, ULONG timeout)
{
    int ret;

    ret = wait_req(timeout);
    if (ret == 0)
    {
        put_data_reg(b);
        ret = handshake(timeout);
    }

    return ret;
}

/* receive a byte, with handshaking & timeout handling */
static int receive_byte(ULONG timeout)
{
    int ret;
    UBYTE c;

    ret = wait_req(timeout);
    if (ret == 0)
    {
        c = get_data_reg();
        ret = handshake(timeout);
        if (ret == 0)
            ret = c;
    }

    return ret;
}

static void reset_bus(BOOL write)
{
    ULONG timeout;

    if (has_scsi == TT_SCSI)
    {
        SCSI_BASE->icr = 0x80;          /* assert RST */
        rst_hold_delay();
        SCSI_BASE->icr = 0x00;          /* unassert everything */
    }
    else    /* handle Falcon SCSI */
    {
        while(FSCSI_BASE->modectl & DMA_MCBIT3) /* Falcon TOS does this, so do we */
            ;
        toggle_dmaout(write);
        fscsi_put(fscsi_icr, 0x80);     /* assert RST */
        rst_hold_delay();
        fscsi_put(fscsi_icr, 0x00);     /* unassert everything */
    }

    /* wait for devices to reset themselves */
    timeout = hz_200 + RESET_TIME;
    while(hz_200 < timeout)
        ;
}

/*
 * middle-level SCSI stuff
 */
static int scsi_arbitrate(void)
{
    ULONG timeout;
    UBYTE temp;

    /*
     * wait for BUS FREE (BSY bit not set)
     */
    timeout = hz_200 + BUSFREE_TIMEOUT;
    while (get_bus_status_reg() & 0x40)
        if (hz_200 >= timeout)
            return TIMEOUT_ERROR;

    put_tcr_reg(0x00);              /* unassert I/O, C/D, MSG: bus free phase */
    put_select_enable_reg(0x00);    /* disable interrupts */
    put_mode_reg(0x00);             /* no parity check */

    do
    {
        do
        {
            put_icr_reg(0x00);              /* unassert BSY & degate data reg */
            put_data_reg(hostid_bit);       /* our id into data reg */
            put_icr_reg(0x01);              /* gate it onto bus */
            or_mode_reg(0x01);              /* set ARBITRATE bit */
            timeout = hz_200 + AIP_TIMEOUT;
            while(!(get_icr_reg()&0x40))    /* wait for AIP bit to be set */
                if (hz_200 >= timeout)
                    return TIMEOUT_ERROR;   /* probably broken hardware */
            arbitration_delay();
        } while(get_icr_reg()&0x20);        /* retry if we lost arbitration */
        temp = get_data_reg();          /* read active bus */
        temp -= hostid_bit;             /* remove ourselves */
        /* continue if not the highest device or we lost arbitration */
    } while((temp > hostid_bit) || (get_icr_reg()&0x20));

    put_icr_reg(0x0c);              /* assert BSY, SEL */
    bus_clear_delay();              /* wait for devices to clear bus */
    bus_settle_delay();             /*  & bus to settle */
    and_mode_reg(0xfe);             /* clear ARBITRATE bit */

    return 0;
}

static int scsi_select(WORD device)
{
    UBYTE devbits = hostid_bit | (1<<device);
    ULONG timeout = hz_200 + SELECTION_TIMEOUT;

    or_icr_reg(0x02);                   /* assert ATN */
    put_data_reg(devbits);              /* device bits to data reg */
    or_icr_reg(0x01);                   /* gate it onto bus */
    deskew2_delay();
    and_icr_reg(0xf7);                  /* turn off BSY so target can take over */
    bus_settle_delay();
    while(!(get_bus_status_reg() & 0x40)) /* wait for BSY bit */
        if (hz_200 >= timeout)
            return TIMEOUT_ERROR;
    and_icr_reg(0x02);                  /* turn off everything except ATN */

    return 0;
}

static int handle_data_out(CMDINFO *info)
{
    ULONG timeout = hz_200 + info->xfer_time;
    UBYTE *p;
    int ret;

    set_output_phase(DATA_OUT_PHASE);

    if (info->mode & DMA_MODE)  /* doing DMA ? */
    {
        init_data_out(info);
        return wait_dma_complete(timeout);
    }

    /* handle programmed i/o */
    p = info->bufptr;
    while(1)
    {
        ret = send_byte(*p++, timeout);
        if (ret < 0)
            break;
    }
    if (ret == PHASE_CHANGE)
        ret = 0;
    return ret;
}

static int handle_data_in(CMDINFO *info)
{
    ULONG timeout = hz_200 + info->xfer_time;
    UBYTE *p;
    int ret;

    set_input_phase(DATA_IN_PHASE);

    if (info->mode & DMA_MODE)      /* doing DMA ? */
    {
        init_data_in(info);
        ret = wait_dma_complete(timeout);
        if ((ret == 0) && (has_scsi == TT_SCSI))
            cleanup_tt_dma(info);   /* do any DMA cleanup required */
        return ret;
    }

    /* handle programmed i/o */
    p = info->bufptr;
    while(1)
    {
        ret = receive_byte(timeout);
        if (ret < 0)
            break;
        *p++ = (UBYTE)ret;
    }
    if (ret == PHASE_CHANGE)
        ret = 0;
    return ret;
}

static int handle_command(CMDINFO *info)
{
    ULONG timeout = hz_200 + SHORT_TIMEOUT;
    UBYTE *cdb = info->cdbptr;
    int ret;

    set_output_phase(COMMAND_PHASE);

    do
    {
        ret = send_byte(*cdb++, timeout);
    } while (ret == 0);

    return ret;
}

static int handle_status(CMDINFO *info)
{
    ULONG timeout = hz_200 + SHORT_TIMEOUT;
    int ret;

    set_input_phase(STATUS_PHASE);

    ret = receive_byte(timeout);
    if (ret == PHASE_CHANGE)        /* we didn't get any status :-( */
        ret = PHASE_ERROR;
    if (ret >= 0)
    {
        info->status = (UBYTE)ret;
        ret = wait_phase_change(timeout);
    }

    return ret;
}

/*
 * handle MESSAGE OUT phase
 *
 * currently, we only send single-byte messages, either IDENTIFY or MESSAGE REJECT
 *
 * as per SCSI-2 6.2.1:
 *  Normally, the initiator negates the ATN signal while the REQ signal
 *  is true and the ACK signal is false during the last REQ/ACK handshake
 *  of the MESSAGE OUT phase.
 * as per Table 10:
 *  For the IDENTIFY message [and queue tag messages], the initiator may
 *  or may not negate ATN before the last ACK of the message.
 * as per 6.1.9.2:
 *  If the target detects one or more parity error(s) on the message byte(s)
 *  received, it may ... retry by asserting the REQ signal after detecting
 *  the ATN signal has gone false and prior to changing to any other phase
 * Therefore:
 *  . we unassert ATN before the handshake
 *  . if we detect REQ after the handshake, we retry the message up to 3 times
 */
static int handle_msg_out(CMDINFO *info)
{
    ULONG timeout = hz_200 + SHORT_TIMEOUT;
    int i, ret;

    set_output_phase(MSG_OUT_PHASE);

    ret = wait_req(timeout);        /* wait for REQ, phase change, or timeout */
    if (ret == 0)                   /* got REQ */
    {
        for (i = 0; i < 3; i++)
        {
            put_data_reg(info->next_msg_out);   /* send byte */
            and_icr_reg(0xfd);          /* unassert ATN */
            ret = handshake(timeout);
            if (ret < 0)
                break;
            ret = wait_req(timeout);    /* wait for REQ, phase change, or timeout */
            if (ret < 0)
                break;
        }
        if (ret >= 0)                   /* stuck on REQ */
            ret = TIMEOUT_ERROR;
    }

    return ret;
}

/*
 * handle MESSAGE IN phase
 *
 * we accept COMMAND COMPLETE, otherwise we force a subsequent MESSAGE OUT
 */
static int handle_msg_in(CMDINFO *info)
{
    ULONG timeout = hz_200 + SHORT_TIMEOUT;
    int i, msglen, ret;
    UBYTE msgbyte;

    set_input_phase(MSG_IN_PHASE);

    /*
     * get first message byte so we know how to proceed
     */
    ret = wait_req(timeout);
    if (ret < 0)
        return ret;

    info->msg_in = msgbyte = get_data_reg();
    if ((msgbyte != COMMAND_COMPLETE_MSG)
     && (msgbyte != MESSAGE_REJECT_MSG))
    {
        or_icr_reg(0x02);       /* assert ATN to request MESSAGE OUT phase */
        info->next_msg_out = MESSAGE_REJECT_MSG;
    }
    ret = handshake(timeout);   /* handshake the first message byte */
    if (ret < 0)
        return ret;

    /*
     * determine length of message
     */
    if (msgbyte == EXTENDED_MSG)
    {
        msglen = receive_byte(timeout);
        if (msglen < 0)         /* error */
            return msglen;
    }
    else if ((msgbyte >= 0x20) && (msgbyte <= 0x2f))
        msglen = 2;
    else msglen = 1;

    /*
     * receive (& ignore) remaining bytes of message (if any)
     */
    for (i = 1; i < msglen; i++)
    {
        ret = receive_byte(timeout);
        if (ret < 0)
            return ret;
    }

    return wait_phase_change(timeout);
}

static int scsi_dispatcher(CMDINFO *info)
{
    WORD error = 0;
    WORD current_phase;

    while(!error)
    {
        switch(current_phase=get_next_phase()) {
        case DATA_OUT_PHASE:
            error = handle_data_out(info);
            break;
        case DATA_IN_PHASE:
            error = handle_data_in(info);
            break;
        case COMMAND_PHASE:
            error = handle_command(info);
            break;
        case STATUS_PHASE:
            error = handle_status(info);
            break;
        case MSG_OUT_PHASE:
            error = handle_msg_out(info);
            break;
        case MSG_IN_PHASE:
            error = handle_msg_in(info);
            if (!error && (info->msg_in == COMMAND_COMPLETE_MSG))
                return 0;
            break;
        default:
            error = PHASE_ERROR;
        }
        if (error == PHASE_CHANGE)  /* expected for all except MSG IN */
            error = 0;
    }

    KDEBUG(("scsi_dispatcher(): error %d during phase %d\n",error,current_phase));
    return error;
}


/*
 * high-level SCSI support
 */

static LONG do_scsi_io(WORD dev, CMDINFO *info)
{
    ULONG timeout;
    LONG ret;
    UWORD dev_bit = 1 << dev;

    if (!has_scsi)
        return -1;

    /*
     * if we should use DMA, set the appropriate flag
     * and flush the cache if writing a non-zero amount
     */
    if (use_dma(info))
    {
        info->mode |= DMA_MODE;
        if ((info->mode&WRITE_MODE) && info->buflen)
            flush_data_cache(info->bufptr, info->buflen);
    }

    /* calculate conservative transfer time, for use as timeout */
    info->xfer_time = max(SHORT_TIMEOUT, (info->buflen/BYTES_PER_CLOCK));

    info->next_msg_out = IDENTIFY_MSG;  /* set for first msg out phase */

    if (has_scsi == FALCON_SCSI)
        flock = -1;                     /* don't let floppy interfere */

    if ((ret=scsi_arbitrate()) == 0)
    {
        if ((ret=scsi_select(dev)) == 0)
        {
            /* set starting phase to match bus before target gets going */
            set_input_phase(DATA_OUT_PHASE);
            timeout = hz_200 + SHORT_TIMEOUT;
            if ((ret=wait_phase_change(timeout)) == 0)
            {
                if ((ret=scsi_dispatcher(info)) == 0)
                {
                    ret = MAKE_UWORD(info->msg_in, info->status);
                }
            }
        }
    }

    /*
     * if we got an error and the device exists, we reset the bus to try
     * to fix things.  if the device isn't known to exist, the error might
     * be valid (e.g. a selection timeout), so we avoid the reset.
     */
    if (ret >= 0)
        device_exists |= dev_bit;
    else if (device_exists & dev_bit)
        reset_bus(info->mode&WRITE_MODE);

    cleanup_scsi();

    /*
     * invalidate cache if necessary
     */
    if (info->mode & DMA_MODE)
        if (!(info->mode & WRITE_MODE) && info->buflen)
            invalidate_data_cache(info->bufptr, info->buflen);

    if (has_scsi == FALCON_SCSI)
        flock = 0;                      /* allow floppy i/o again */

    return ret;
}

static LONG decode_scsi_status(WORD dev, LONG ret)
{
    UBYTE cdb[6];
    CMDINFO info;

    if (ret == 0)
        return E_OK;

    if (ret < 0L)
        return EUNDEV;

    if (ret & 0x08)                 /* busy */
        return EDRVNR;

    if ((ret & 0x02) == 0)
        return ret;

    /*
     * handle check condition: do a request sense & check result
     */
    bzero(cdb, 6);
    cdb[0] = REQUEST_SENSE;
    cdb[4] = REQSENSE_LENGTH;

    bzero(&info, sizeof(CMDINFO));
    info.cdbptr = cdb;
    info.cdblen = 6;
    info.bufptr = reqsense_buffer;
    info.buflen = REQSENSE_LENGTH;
    ret = do_scsi_io(dev, &info);

    if (ret < 0)                                /* errors on request sense are bad */
        return EDRVNR;

    if ((reqsense_buffer[0] & 0x7e) != 0x70)    /* if extended sense not present, */
        return E_CHNG;                          /* say media change               */

    /* check sense key */
    switch(reqsense_buffer[2]&0x0f) {
    case 0:     /* no sense */
    case 6:     /* unit attention */
        return E_CHNG;
    case 1:     /* recovered error */
        return E_OK;
    case 2:     /* not ready */
        return EDRVNR;
    case 5:     /* illegal request */
        return EBADRQ;
    case 7:     /* data protect */
        return EWRPRO;
    }

    return EGENRL;
}

static LONG do_scsi_rw(UWORD rw, ULONG sector, UWORD count, UBYTE *buf, WORD dev)
{
    UBYTE cdb[10];  /* allow for 10-byte read/write commands */
    CMDINFO info;
    LONG ret;

    bzero(&info, sizeof(CMDINFO));
    info.cdbptr = cdb;
    info.cdblen = build_rw_command(cdb, rw, sector, count);
    info.bufptr = buf;
    info.buflen = count * SECTOR_SIZE;
    info.mode = rw ? WRITE_MODE : 0;

    /* execute command */
    ret = do_scsi_io(dev, &info);

    return decode_scsi_status(dev, ret);
}

static LONG scsi_capacity(WORD dev, ULONG *buffer)
{
    UBYTE cdb[10];
    CMDINFO info;
    LONG ret;

    bzero(cdb, 10);         /* build READ CAPACITY command */
    cdb[0] = READ_CAPACITY;

    bzero(&info, sizeof(CMDINFO));
    info.cdbptr = cdb;
    info.cdblen = 10;
    info.bufptr = (void *)buffer;
    info.buflen = CAPACITY_LENGTH;
    ret = do_scsi_io(dev, &info);

    return decode_scsi_status(dev, ret);
}

static LONG scsi_inquiry(WORD dev, UBYTE *buffer)
{
    UBYTE cdb[6];
    CMDINFO info;
    LONG ret;

    bzero(cdb, 6);          /* build INQUIRY command */
    cdb[0] = INQUIRY;
    cdb[4] = INQUIRY_LENGTH;

    bzero(&info, sizeof(CMDINFO));
    info.cdbptr = cdb;
    info.cdblen = 6;
    info.bufptr = buffer;
    info.buflen = INQUIRY_LENGTH;
    ret = do_scsi_io(dev, &info);

    return decode_scsi_status(dev, ret);
}
#endif

#if (CONF_WITH_ACSI || CONF_WITH_SCSI)
/*
 * SCSI commands used by build_rw_command()
 */
#define READ6           0x08
#define WRITE6          0x0a
#define READ10          0x28
#define WRITE10         0x2a

/*
 * build ACSI/SCSI read/write command
 * returns length of command built
 */
int build_rw_command(UBYTE *cdb, UWORD rw, ULONG sector, UWORD count)
{
    /*
     * this function builds commands suitable for use with both ACSI and
     * SCSI interfaces.
     *
     * if the command is being built for the ACSI interface, the sector
     * count will be <= 255, and the operation code selected will depend
     * on the sector number.  if it is greater than 0x1fffffL (the disk
     * is >1GB), then the 10-byte commands will be used.  these are not
     * ACSI-compatible, but that isn't a problem since pure ACSI disks
     * cannot be that large, so we must be talking to a SCSI disk via a
     * converter.
     *
     * if the command is being built for the SCSI interface, the sector
     * count can be up to 16383 (on the Falcon) or 65535 (on the TT).
     * the operation code selected will depend on both the starting
     * sector number and the sector count.
     */
    if ((sector <= 0x1fffffL) && (count <= 255))    /* we can use 6-byte CDBs */
    {
        cdb[0] = (rw==RW_WRITE) ? WRITE6 : READ6;
        cdb[1] = (sector >> 16) & 0x1f;
        cdb[2] = sector >> 8;
        cdb[3] = sector;
        cdb[4] = count;
        cdb[5] = 0x00;
        return 6;
    }

    cdb[0] = (rw==RW_WRITE) ? WRITE10 : READ10;
    cdb[1] = 0x00;
    cdb[2] = sector >> 24;
    cdb[3] = sector >> 16;
    cdb[4] = sector >> 8;
    cdb[5] = sector;
    cdb[6] = 0x00;
    cdb[7] = count >> 8;
    cdb[8] = count;
    cdb[9] = 0x00;
    return 10;
}
#endif
