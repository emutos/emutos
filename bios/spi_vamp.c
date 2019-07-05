/*
 * spi_vamp.c - SPI interface for SD/MMC card driver on SAGA core
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * Authors:
 *  Christian Zietz
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "spi.h"

#if CONF_WITH_VAMPIRE_SPI

#define SAGA_SDCARD_DATA (*(volatile UBYTE*)(0xDE0000UL))
#define SAGA_SDCARD_CTL  (*(volatile UWORD*)(0xDE0004UL))
#define SAGA_SDCARD_STAT (*(volatile UWORD*)(0xDE0006UL))
#define SAGA_SDCARD_CLK  (*(volatile UWORD*)(0xDE000CUL))

#define SAGA_SDCARD_CLKDIV_IDENT ((UWORD)0xffU) /* 195 kHz */
#define SAGA_SDCARD_CLKDIV_MMC   ((UWORD)0x02U) /* 16.7 MHz */
#define SAGA_SDCARD_CLKDIV_SD    ((UWORD)0x01U) /* 25 MHz */

/*
 *  initialise spi for memory card
 */
void spi_initialise(void)
{
    spi_cs_unassert();
}

void spi_clock_sd(void)
{
    SAGA_SDCARD_CLK = SAGA_SDCARD_CLKDIV_SD;
}

void spi_clock_mmc(void)
{
    SAGA_SDCARD_CLK = SAGA_SDCARD_CLKDIV_MMC;
}

void spi_clock_ident(void)
{
    SAGA_SDCARD_CLK = SAGA_SDCARD_CLKDIV_IDENT;
}

/* when we assert or unassert, we send a dummy byte to
 * force a write to the register
 */
void spi_cs_assert(void)
{
    SAGA_SDCARD_CTL = 0;
    spi_send_byte(0xff);
}

void spi_cs_unassert(void)
{
    SAGA_SDCARD_CTL = 1;
    spi_send_byte(0xff);
}

void spi_send_byte(UBYTE c)
{
    SAGA_SDCARD_DATA = c;
    /* reading will stall until transmission is complete */
    FORCE_READ(SAGA_SDCARD_DATA);
}

UBYTE spi_recv_byte(void)
{
    /* send a byte to get one */
    SAGA_SDCARD_DATA = 0xFF;
    /* reading will stall until transmission is complete */
    return SAGA_SDCARD_DATA;
}
#endif /* CONF_WITH_VAMPIRE_SPI */
