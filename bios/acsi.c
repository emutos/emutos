/*
 * acsi.c - Atari Computer System Interface (ACSI) support
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "acsi.h"
#include "scsi.h"
#include "disk.h"
#include "dma.h"
#include "string.h"
#include "mfp.h"
#include "machine.h"
#include "tosvars.h"
#include "gemerror.h"
#include "blkdev.h"
#include "biosext.h"    /* for cache control routines */
#include "asm.h"
#include "cookie.h"
#include "delay.h"
#include "biosdefs.h"

#if CONF_WITH_ACSI

/*
 * private prototypes
 */
static void acsi_begin(void);
static void acsi_end(void);
static void hdc_start_dma(UWORD control);
static void dma_send_byte(UBYTE data, UWORD control);
static int send_command(UBYTE *cdb,WORD cdblen,WORD rw,WORD dev,WORD cnt,UWORD repeat);
static int do_acsi_rw(WORD rw, LONG sect, WORD cnt, UBYTE *buf, WORD dev);
static LONG acsi_capacity(WORD dev, ULONG *info);
static LONG acsi_testunit(WORD dev);
static LONG acsi_inquiry(WORD dev, UBYTE *buf);

/* the following exists to allow the data and control registers to
 * be written together as well as separately.  this avoids the
 * "DMA chip anomaly" (see "Atari ACSI/DMA Integration Guide" p.14).
 */
#define ACSIDMA ((union acsidma *) 0xFFFF8604)

union acsidma {
    volatile ULONG datacontrol;
    struct {
        volatile UWORD data;
        volatile UWORD control;
    } s;
};


/*
 * defines
 */
#define MAXSECS_PER_ACSI_IO     255
#define SMALL_TIMEOUT (CLOCKS_PER_SEC/10)   /* 100ms between cmd bytes */

/*
 * although the following large timeout should not be required for 'real'
 * ACSI devices, it's used by Atari's hard disk routines.  also, at least
 * one third-party add-on (Satandisk, which converts from ACSI to SD/MMC)
 * can take more than a second to complete a write command.
 */
#define LARGE_TIMEOUT (3*CLOCKS_PER_SEC)    /* 3 seconds for the data xfer itself */

/*
 * there should be a minimum of 5msec between ACSI I/Os.  because
 * of the granularity of the system clock, we need 2 ticks ...
 */
#define INTER_IO_TIME 2     /* ticks between I/Os */

/* delay for dma out toggle */
#define delay() delay_loop(loopcount_delay)

/* Bytes to request for an INQUIRY command */
#define INQUIRY_BYTES 36

/*
 * local variables
 */
static ULONG loopcount_delay;   /* used by delay() macro */
static ULONG next_acsi_time;    /* earliest time we can start the next i/o */


/*
 * High-level ACSI stuff.
 */
void acsi_init(void)
{
    /* the following delay is used between toggling dma out.  in Atari
     * TOS, the delay is provided by an instruction sequence which
     * takes about 15usec on an ST, 5usec on a TT or Falcon.  we always
     * use 15usec.
     */
    loopcount_delay = 15 * loopcount_1_msec / 1000;

    next_acsi_time = hz_200;    /* immediate :-) */
}

LONG acsi_rw(WORD rw, LONG sector, WORD count, UBYTE *buf, WORD dev)
{
    WORD maxsecs_per_io = MAXSECS_PER_ACSI_IO;
    BOOL use_tmpbuf = FALSE;
    int retry;
    int err = 0;
    UBYTE *p, *tmp_buf = NULL;

    rw &= RW_RW;    /* we just care about read or write for now */

    /*
     * the ACSI hardware requires that the buffer be word-aligned and
     * located in ST-RAM.  if it isn't, we use an intermediate buffer:
     * the FRB (if available) or dskbufp.
     */
    if (IS_ODD_POINTER(buf) || !IS_STRAM_POINTER(buf)) {
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

    while(count > 0) {
        WORD numsecs;

        numsecs = (count>maxsecs_per_io) ? maxsecs_per_io : count;

        p = use_tmpbuf ? tmp_buf : buf;
        if (rw && use_tmpbuf)
            memcpy(p, buf, numsecs * SECTOR_SIZE);

        for (retry = 0; retry < 2; retry++) {
            err = do_acsi_rw(rw, sector, numsecs, p, dev);
            if (err == 0)
                break;
        }

        if (err) {
            KDEBUG(("acsi.c: %s error %d\n",rw?"write":"read",err));
            KDEBUG(("        dev=%d,sector=%ld,numsecs=%d\n",dev,sector,numsecs));
            return err;
        }

        if (!rw && use_tmpbuf)
            memcpy(buf, p, numsecs * SECTOR_SIZE);

        count -= numsecs;
        buf += numsecs * SECTOR_SIZE;
        sector += numsecs;
    }
    return 0;
}

/*
 *  perform miscellaneous non-data-transfer functions
 */
LONG acsi_ioctl(UWORD dev, UWORD ctrl, void *arg)
{
    LONG rc = ERR;
    ULONG *info = arg;

    switch(ctrl) {
    case GET_DISKINFO:
        rc = acsi_capacity(dev,info);
        break;
    case GET_DISKNAME:
        /* according to the "Atari ACSI/DMA Integration Guide" p.32,
         * all ACSI devices must support Test Unit Ready.  so if we
         * get a timeout, we assume the device does not exist.
         */
        rc = acsi_testunit(dev);
        if (rc < 0)
            return EUNDEV;
        rc = acsi_inquiry(dev,dskbufp);
        /* ACSI devices are not required to support INQUIRY.
           Return generic name in case command failed.
         */
        if (rc == 0) {
            /* Only accept direct-access devices (e.g. HDDs). */
            if ((dskbufp[0] & 0x1F) != 0)
                return EUNDEV;
            /* Zero terminate vendor & product ID. */
            dskbufp[32] = 0;
            strcpy(arg, (char *)&dskbufp[8]);
        } else {
            strcpy(arg, "ACSI Disk");
            rc = 0; /* Don't return an error. */
        }
        break;
    case GET_MEDIACHANGE:
        rc = MEDIANOCHANGE;
        break;
    }

    return rc;
}

static LONG acsi_capacity(WORD dev, ULONG *info)
{
    UBYTE cdb[10];
    int status;

    acsi_begin();

    /* load DMA base address -> internal disk buffer */
    set_dma_addr(dskbufp);

    cdb[0] = 0x25;          /* set up Read Capacity cdb */
    bzero(cdb+1,9);
    status = send_command(cdb,10,RW_READ,dev,1,1);

    acsi_end();

    invalidate_data_cache(dskbufp,sizeof(ULONG)*4);

    if (status == 0) {
        const ULONG *data = (const ULONG *)dskbufp;
        info[0] = data[0] + 1;  /* data[0] is number of last sector */
        info[1] = data[1];
    }

    return status;
}

static LONG acsi_testunit(WORD dev)
{
    UBYTE cdb[6];
    int status;

    acsi_begin();

    bzero(cdb,6);           /* set up Test Unit Ready cdb */
    status = send_command(cdb,6,RW_READ,dev,0,0);

    acsi_end();

    return status;
}

static LONG acsi_inquiry(WORD dev, UBYTE *buf)
{
    UBYTE cdb[6];
    int status;

    acsi_begin();

    /* load DMA base address */
    set_dma_addr(buf);

    cdb[0] = 0x12;          /* set up Inquiry cdb */
    cdb[1] = cdb[2] = cdb[3] = cdb[5] = 0;
    cdb[4] = INQUIRY_BYTES; /* retrieve 36 bytes at maximum. */
    status = send_command(cdb,6,RW_READ,dev,1,1);

    acsi_end();

    invalidate_data_cache(buf,INQUIRY_BYTES);

    return status;
}


/* must call this before manipulating any ACSI-related hardware */
static void acsi_begin(void)
{
    while(hz_200 < next_acsi_time) {    /* wait until safe */
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        stop_until_interrupt();
#endif
    }

    flock = -1;     /* don't let floppy interfere */
}

/* must call this when finished with ACSI-related hardware */
static void acsi_end(void)
{
    /* put DMA back to floppy and allow floppy i/o */
    ACSIDMA->s.control = DMA_FLOPPY;
    flock = 0;

    next_acsi_time = hz_200 + INTER_IO_TIME;    /* next safe time */
}

/*
 * Internal implementation -
 * cnt <= 0xFF, no retry done, returns -1 if timeout, or the DMA status.
 */

static int do_acsi_rw(WORD rw, LONG sector, WORD cnt, UBYTE *buf, WORD dev)
{
    UBYTE cdb[10];  /* allow for 10-byte read/write commands */
    int status, cdblen;
    LONG buflen = cnt * SECTOR_SIZE;

    /* flush data cache here so that memory is current */
    if (rw == RW_WRITE)
        flush_data_cache(buf,buflen);

    acsi_begin();

    /* load DMA base address */
    set_dma_addr(buf);

    /* emit command */
    cdblen = build_rw_command(cdb,rw,sector,cnt);
    status = send_command(cdb,cdblen,rw,dev,cnt,0);

    acsi_end();

    /* invalidate data cache if we've read into memory */
    if (rw == RW_READ)
        invalidate_data_cache(buf,buflen);

    return status;
}

/*
 * send an ACSI command; return -1 if timeout
 *
 * note: we actually send the command repeat+1 times.  this is to handle
 * situations where the length of data returned from a single command is
 * not a multiple of 16.  without this trick, some returned data would be
 * stuck in the DMA FIFO.
 *
 * the following values for 'repeat' are suggested:
 *   1. length returned is a multiple of 16: set repeat = 0; otherwise
 *   2. length returned is greater than 16: set 'repeat' to 1; otherwise
 *   3. set 'repeat' to ceil(16/length returned).
 */
static int send_command(UBYTE *inputcdb,WORD cdblen,WORD rw,WORD dev,WORD cnt,UWORD repeat)
{
    UWORD control;
    UBYTE cdb[13];      /* allow for 12-byte input commands */
    UBYTE *cdbptr, *p;
    int j, status;

    /*
     * see if we need to use ICD trickery
     */
    if (*inputcdb > 0x1e) {
        cdbptr = cdb;
        *cdbptr = 0x1f;     /* ICD extended command method */
        memcpy(cdbptr+1,inputcdb,cdblen);
        cdblen++;
    } else cdbptr = inputcdb;

    *cdbptr |= (dev << 5);  /* insert device number */

    if (rw == RW_WRITE) {
        control = DMA_WRBIT | DMA_DRQ_FLOPPY;
    } else {
        control = DMA_DRQ_FLOPPY;
    }
    hdc_start_dma(control); /* select sector count register */
    ACSIDMA->s.data = cnt;

    /*
     * handle 'repeat' function
     */
    control |= DMA_CS_ACSI;
    do {
        while(hz_200 < next_acsi_time) {    /* wait until safe */
#if USE_STOP_INSN_TO_FREE_HOST_CPU
            stop_until_interrupt();
#endif
        }

        ACSIDMA->s.control = control;   /* assert command signal */
        control |= DMA_NOT_NEWCDB;      /* set up for remaining cmd bytes */

        for (j = 0, p = cdbptr; j < cdblen-1; j++) {
            dma_send_byte(*p++,control);
            if (timeout_gpip(SMALL_TIMEOUT))
                return -1;
        }

        /* send the last byte & wait for completion of DMA */
        dma_send_byte(*p,control&0xff00);
        status = timeout_gpip(LARGE_TIMEOUT);
        next_acsi_time = hz_200 + INTER_IO_TIME;    /* next safe time */
        if (status)
            return -1;

        /* read status & return it */
        ACSIDMA->s.control = control & ~DMA_WRBIT;
        status = ACSIDMA->s.data & 0x00ff;
        if (status)
            break;
        control &= ~DMA_NOT_NEWCDB; /* set new CDB signal for next time */
    } while(repeat--);

    return status;
}

/*
 * send current byte plus the control for the _next_ byte
 */
static void dma_send_byte(UBYTE data, UWORD control)
{
    ACSIDMA->datacontrol = MAKE_ULONG(data, control);
}


/* the hdc_start_dma() function sets 'control' to access the sector
 * count register and then toggles the DMA write bit.  this signals
 * the DMA to clear its internal buffers.
 */
static void hdc_start_dma(UWORD control)
{
    control |= DMA_SCREG;
    ACSIDMA->s.control = control ^ DMA_WRBIT;
    delay();
    ACSIDMA->s.control = control;
    delay();
}

#endif /* CONF_WITH_ACSI */
