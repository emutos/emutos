/*
 * acsi.c - Atari Simple Computer Interface (ACSI) support
 *
 * Copyright (c) 2002 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "acsi.h"
#include "dma.h"
#include "kprint.h"
#include "string.h"
#include "mfp.h"
#include "machine.h"
#include "tosvars.h"
#include "gemerror.h"
#include "config.h"

#if CONF_WITH_ACSI==0

LONG acsi_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev)
{
    return EUNDEV;
}

#else /* CONF_WITH_ACSI */

/*
 * private prototypes
 */

static void hdc_start_dma_read(int count);
static void hdc_start_dma_write(int count);
static int timeout_gpip(LONG delay);
static void dma_send_byte(BYTE data, BYTE control);
static int do_acsi_rw(WORD rw, LONG sect, WORD cnt, LONG buf, WORD dev);

/*
 * defines
 */

#define SMALL_TIMEOUT 100   /* ms between cmd bytes */
#define LARGE_TIMEOUT 1000  /* ms for the command itself */
 
/*
 * High-level ACSI stuff.
 */

LONG acsi_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev)
{
    int cnt;
    int need_frb = 0;
    int retry;
    int err = 0;
    LONG tmp_buf;
    
#if CONF_WITH_ACSI==0
    return EUNDEV;
#endif

    /* read by chunks of at most 0x80 sectors. 
     * (0x80 * 512 bytes will fit in the 64 kB buffer _FRB, and cnt
     * must fit in a byte anyway.)
     */
    while(count > 0) {
        cnt = 0x80;
        if(cnt > count) 
            cnt = count;
            
        if (buf > 0x1000000L) {
            if (cookie_frb <= 0) {
                kprintf("missing FRB buffer");
                return -1L;
            } else {
                /* proper FRB lock (TODO) */
                need_frb = 1;
                tmp_buf = cookie_frb;
            }
        } else {
            tmp_buf = buf;
        }
        
        if(rw && need_frb) {
            memcpy((void *)tmp_buf, (void *)buf, cnt * 512L);
        }
        for(retry = 0; retry < 2 ; retry++) {
            err = do_acsi_rw(rw, sector, cnt, tmp_buf, dev);
            if(err == 0) break;
        }
        if((!rw) && need_frb) {
            memcpy((void *)buf, (void *)tmp_buf, cnt * 512L);
        }
        if(need_frb) {
            /* proper FRB unlock (TODO) */
        }
        if(err) {
            kprintf("ACSI -> %d\n", err);
            return err;
        }
        
        count -= cnt;
        buf += cnt * 512L;
        sector += cnt;
    }
    return 0;
}

/*
 * Internal implementation - 
 * cnt <= 0xFF, no retry done, returns -1 if timeout, or the DMA status.
 */

static int do_acsi_rw(WORD rw, LONG sector, WORD cnt, LONG buf, WORD dev)
{
    /* TODO - is dev the device number, the controller number,
     * or a combination of both ?
     */
#if 0
    int ctrlnum = 0;
    int devnum = dev << 5;
#else
    int ctrlnum = dev << 5;
    int devnum = 0;
#endif
    int opcode;
    int status;
    
    if(rw) {
        opcode = 0x0a | ctrlnum;  /* write */
    } else {
        opcode = 0x08 | ctrlnum;  /* read */
    }
    
    /* set flock */
    flock = -1;
    
    /* load DMA base address */
    DMA->addr_high = buf>>16;
    DMA->addr_med = buf>>8;
    DMA->addr_low = buf;
    
    if(rw) {
        hdc_start_dma_write(cnt);
    } else {
        hdc_start_dma_read(cnt);
    }
        
    /* emit command */
    dma_send_byte( opcode, DMA_FDC | DMA_HDC);
    if(timeout_gpip(SMALL_TIMEOUT)) goto timeout;
    dma_send_byte( devnum | ((sector >> 16) & 0x1F), 
                   DMA_FDC | DMA_HDC | DMA_A0);
    if(timeout_gpip(SMALL_TIMEOUT)) goto timeout;
    dma_send_byte( sector >> 8, DMA_FDC | DMA_HDC | DMA_A0);
    if(timeout_gpip(SMALL_TIMEOUT)) goto timeout;
    dma_send_byte( sector,  DMA_FDC | DMA_HDC | DMA_A0);
    if(timeout_gpip(SMALL_TIMEOUT)) goto timeout;
    dma_send_byte( cnt, DMA_FDC | DMA_HDC | DMA_A0);
    if(timeout_gpip(SMALL_TIMEOUT)) goto timeout;
    dma_send_byte( 0, DMA_HDC | DMA_A0);
    if(timeout_gpip(LARGE_TIMEOUT)) goto timeout;
    
    /* read status */
    DMA->control = DMA_FDC | DMA_HDC | DMA_A0;
    status = DMA->data;
    
    /* put back to floppy and free flock */
    DMA->control = DMA_FDC;
    flock = 0;
    return status;

timeout:
    DMA->control = DMA_FDC;
    flock = 0;
    return -1;
}

static void dma_send_byte(BYTE data, BYTE control)
{
    DMA->control = control;
    DMA->data = data;
}

/* returns 1 if the timeout (milliseconds) elapsed before gpip went low */
static int timeout_gpip(LONG delay)
{
    MFP *mfp = MFP_BASE;
    LONG next = hz_200 + delay/5;

    while(hz_200 < next) {
        if((mfp->gpip & 0x20) == 0) {
            return 0;
        }
    }
    return 1;
}



/* the hdc_start_dma_*() functions toggle the DMA write bit, to
 * signal the DMA to clear its internal buffers (16 bytes in input, 
 * 32 bytes in output). This is done just before issuing the 
 * command to the DMA.
 */

static void hdc_start_dma_read(int count)
{
    DMA->control = DMA_SCREG | DMA_FDC | DMA_HDC;
    DMA->control = DMA_SCREG | DMA_FDC | DMA_HDC | DMA_WRBIT;
    DMA->control = DMA_SCREG | DMA_FDC | DMA_HDC;
    DMA->data = count;
}

static void hdc_start_dma_write(int count)
{
    DMA->control = DMA_SCREG | DMA_FDC | DMA_HDC | DMA_WRBIT;
    DMA->control = DMA_SCREG | DMA_FDC | DMA_HDC;
    DMA->control = DMA_SCREG | DMA_FDC | DMA_HDC | DMA_WRBIT;
    DMA->data = count;
}

#endif /* CONF_WITH_ACSI */
