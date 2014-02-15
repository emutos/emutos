/*
 * spi.h - header for SPI functions used by SD/MMC driver
 *
 * Copyright (c) 2013-2014 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef _SPI_H
#define _SPI_H

#ifdef __mcoldfire__

#include <portab.h>

void spi_clock_ident(void);
void spi_clock_mmc(void);
void spi_clock_sd(void);
void spi_cs_assert(void);
void spi_cs_unassert(void);
void spi_initialise(void);
UBYTE spi_recv_byte(void);
void spi_send_byte(UBYTE input);

#endif /* __mcoldfire__ */

#endif /* _SPI_H */
