/*
 * spi.c - SPI interface for SD/MMC card driver
 *
 * Copyright (c) 2013-2014 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef __mcoldfire__
#error This file must only be compiled on ColdFire targets
#endif

#include "config.h"
#include "portab.h"
#include "coldpriv.h"
#include "spi.h"

/*
 * DSPI configuration register initialisation value
 */
#define DMCR_INIT   MCF_DSPI_DMCR_MSTR |    /* we're the DSPI master */ \
                    MCF_DSPI_DMCR_CSIS5 |   /* CS5 inactive state high */ \
                    MCF_DSPI_DMCR_CSIS3 |   /* CS3 inactive state high */ \
                    MCF_DSPI_DMCR_CSIS2 |   /* CS2 inactive state high */ \
                    MCF_DSPI_DMCR_CSIS0 |   /* CS0 inactive state high */ \
                    MCF_DSPI_DMCR_DTXF |    /* disable transmit FIFO */ \
                    MCF_DSPI_DMCR_DRXF |    /* disable receive FIFO */ \
                    MCF_DSPI_DMCR_CTXF |    /* clear transmit FIFO */ \
                    MCF_DSPI_DMCR_CRXF      /* clear receive FIFO */

/*
 *  according to the MC547x Reference Manual Rev.5 p.27-7, the DCTARn
 *  registers must not be written to while the DSPI is in the running
 *  state.  we need three different clock speeds, but we can avoid stopping
 *  and starting the DSPI by using three different DCTARn registers, and
 *  selecting between them when writing to the DTFR.  the following clock
 *  speeds and registers are used:
 *      <= 25MHz    for SD operation        DCTAR0
 *      <= 20MHz    for MMC operation       DCTAR1
 *      <= 400kHz   identification mode     DCTAR2
 *  
 *  the actual clock speeds available are limited by the Coldfire bus clock
 *  and the baud rate prescaler/scaler combination.  we also need to set
 *  other timings correspondingly.
 */

/*
 *  Firebee timings are as follows:
 *  SD mode:
 *      Tcsc = (PCSSCK * CSSCK / Fsys) = (3 * 4 / 132000000) = 91 nsec
 *      Tasc = (PASC * ASC / Fsys) = (3 * 4 / 132000000) = 91 nsec
 *      Tdt = (PDT * DT / Fsys) = (3 * 8 / 132000000) = 182 nsec
 *      baud rate = (Fsys / (PBR * BR)) = (132000000 / (3 * 2)) = 22MHz
 *  MMC mode:
 *      Tcsc = (PCSSCK * CSSCK / Fsys) = (1 * 16 / 132000000) = 121 nsec
 *      Tasc = (PASC * ASC / Fsys) = (1 * 16 / 132000000) = 121 nsec
 *      Tdt = (PDT * DT / Fsys) = (1 * 32 / 132000000) = 242 nsec
 *      baud rate = (Fsys / (PBR * BR)) = (132000000 / (2 * 4)) = 16.5MHz
 *  identification mode:
 *      Tcsc = (PCSSCK * CSSCK / Fsys) = (3 * 256 / 132000000) = 5.8 usec
 *      Tasc = (PASC * ASC / Fsys) = (3 * 256 / 132000000) = 5.8 usec
 *      Tdt = (PDT * DT / Fsys) = (3 * 512 / 132000000) = 11.6 usec
 *      baud rate = (Fsys / (PBR * BR)) = (132000000 / (3 * 128)) = 343.75kHz
 */
#define SD_MODE     MCF_DSPI_DCTAR_TRSZ(7L) |       /* transfer size = 8 bit */ \
                    MCF_DSPI_DCTAR_PCSSCK_3CLK |    /* 3 clock DSPICS to DSPISCK delay prescaler */ \
                    MCF_DSPI_DCTAR_PASC_3CLK |      /* 3 clock DSPISCK to DSPICS negation prescaler */ \
                    MCF_DSPI_DCTAR_PDT_3CLK |       /* 3 clock delay between DSPICS assertions prescaler */ \
                    MCF_DSPI_DCTAR_PBR_3CLK |       /* 3 clock baudrate prescaler */ \
                    MCF_DSPI_DCTAR_CSSCK(1L) |      /* CS to SCK delay scaler = 4 */\
                    MCF_DSPI_DCTAR_ASC(1L) |        /* after SCK delay scaler = 4 */ \
                    MCF_DSPI_DCTAR_DT(2L) |         /* delay after transfer scaler = 8 */ \
                    MCF_DSPI_DCTAR_BR(0L)           /* baudrate scaler = 2 */

#define MMC_MODE    MCF_DSPI_DCTAR_TRSZ(7L) |       /* transfer size = 8 bit */ \
                    MCF_DSPI_DCTAR_PCSSCK_1CLK |    /* 1 clock DSPICS to DSPISCK delay prescaler */ \
                    MCF_DSPI_DCTAR_PASC_1CLK |      /* 1 clock DSPISCK to DSPICS negation prescaler */ \
                    MCF_DSPI_DCTAR_PDT_1CLK |       /* 1 clock delay between DSPICS assertions prescaler */ \
                    MCF_DSPI_DCTAR_PBR_2CLK |       /* 2 clock baudrate prescaler */ \
                    MCF_DSPI_DCTAR_CSSCK(3L) |      /* CS to SCK delay scaler = 16 */\
                    MCF_DSPI_DCTAR_ASC(3L) |        /* after SCK delay scaler = 16 */ \
                    MCF_DSPI_DCTAR_DT(4L) |         /* delay after transfer scaler = 32 */ \
                    MCF_DSPI_DCTAR_BR(1L)           /* baudrate scaler = 4 */

#define IDENT_MODE  MCF_DSPI_DCTAR_TRSZ(7L) |       /* transfer size = 8 bit */ \
                    MCF_DSPI_DCTAR_PCSSCK_3CLK |    /* 3 clock DSPICS to DSPISCK delay prescaler */ \
                    MCF_DSPI_DCTAR_PASC_3CLK |      /* 3 clock DSPISCK to DSPICS negation prescaler */ \
                    MCF_DSPI_DCTAR_PDT_3CLK |       /* 3 clock delay between DSPICS assertions prescaler */ \
                    MCF_DSPI_DCTAR_PBR_3CLK |       /* 3 clock baudrate prescaler */ \
                    MCF_DSPI_DCTAR_CSSCK(7L) |      /* CS to SCK delay scaler = 256 */\
                    MCF_DSPI_DCTAR_ASC(7L) |        /* after SCK delay scaler = 256 */ \
                    MCF_DSPI_DCTAR_DT(8L) |         /* delay after transfer scaler = 512 */ \
                    MCF_DSPI_DCTAR_BR(7L)           /* baudrate scaler = 128 */


/* the following maintains the status of chip select and DCTARn select */
static ULONG fifo_out;


/*
 *  initialise spi for memory card
 */
void spi_initialise(void)
{
    /*
     *  first, halt the DSPI so we can change the registers
     */
    MCF_DSPI_DMCR |= MCF_DSPI_DMCR_HALT;

    /*
     *  initialise the DSPI clock and transfer attributes registers
     */
    MCF_DSPI_DCTAR0 = SD_MODE;
    MCF_DSPI_DCTAR1 = MMC_MODE;
    MCF_DSPI_DCTAR2 = IDENT_MODE;

    /*
     * initialize DSPI configuration register and start
     */
    MCF_DSPI_DMCR = DMCR_INIT;

    fifo_out = 0UL;
}

void spi_clock_sd(void)
{
    fifo_out &= ~MCF_DSPI_DTFR_CTAS(7L);    /* remove old DCTARn selection */
    fifo_out |= MCF_DSPI_DTFR_CTAS(0L);     /* use DCTAR0 */
}

void spi_clock_mmc(void)
{
    fifo_out &= ~MCF_DSPI_DTFR_CTAS(7L);    /* remove old DCTARn selection */
    fifo_out |= MCF_DSPI_DTFR_CTAS(1L);     /* use DCTAR1 */
}

void spi_clock_ident(void)
{
    fifo_out &= ~MCF_DSPI_DTFR_CTAS(7L);    /* remove old DCTARn selection */
    fifo_out |= MCF_DSPI_DTFR_CTAS(2L);     /* use DCTAR2 */
}

/* when we assert or unassert, we send a dummy byte to
 * force a write to the register
 */
void spi_cs_assert(void)
{
    fifo_out |= MCF_DSPI_DTFR_CS5;
    spi_send_byte(0xff);
}

void spi_cs_unassert(void)
{
    fifo_out &= ~MCF_DSPI_DTFR_CS5;
    spi_send_byte(0xff);
}

void spi_send_byte(UBYTE c)
{
ULONG temp;

    UNUSED(temp);

    MCF_DSPI_DTFR = fifo_out | c;
    while(!(MCF_DSPI_DSR & MCF_DSPI_DSR_TCF))   /* wait for transfer complete */
        ;
    temp = MCF_DSPI_DRFR;                       /* need to do this! */

    MCF_DSPI_DSR = 0xffffffffL;                 /* clear status register */
}

UBYTE spi_recv_byte(void)
{
ULONG temp;

    MCF_DSPI_DTFR = fifo_out | 0xff;            /* send a byte to get one */
    while(!(MCF_DSPI_DSR & MCF_DSPI_DSR_TCF))   /* wait for transfer complete */
        ;
    temp = MCF_DSPI_DRFR;                       /* retrieve this before clearing DSR */

    MCF_DSPI_DSR = 0xffffffffL;                 /* clear status register */

    return (UBYTE)(temp & 0xff);
}
