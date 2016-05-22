/*
 * fdc.h - Western Digital 1772 Floppy Disk Controller
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FDC_H
#define FDC_H

#include "portab.h"
#include "dma.h"

/*
 * Accessing the FDC registers is indirect through ST-specific
 * DMA circuitry. See also dma.h.
 */
#define FDC_CS  (DMA_FLOPPY              )  /* command/status  */
#define FDC_TR  (DMA_FLOPPY|       DMA_A0)  /* track register  */
#define FDC_SR  (DMA_FLOPPY|DMA_A1       )  /* sector register */
#define FDC_DR  (DMA_FLOPPY|DMA_A1|DMA_A0)  /* data register   */

/*
 * commands (relevant bits/fields indicated)
 */
#define FDC_RESTORE 0x00    /* ( HVRR) seek to track 0 */
#define FDC_SEEK    0x10    /* ( HVRR) seek to track */
#define FDC_STEP    0x20    /* (UHVRR) step in same direction */
#define FDC_STEPI   0x40    /* (UHVRR) step in */
#define FDC_STEPO   0x60    /* (UHVRR) step out */
#define FDC_READ    0x80    /* (MHE00) read sector */
#define FDC_WRITE   0xA0    /* (MHEPA) write sector */
#define FDC_READID  0xC0    /* ( HE00) read sector ID */
#define FDC_READTR  0xE0    /* ( HE00) read track */
#define FDC_WRITETR 0xF0    /* ( HEP0) write track */
#define FDC_IRUPT   0xD0    /* ( IIII) force interrupt */

/*
 * other bits/fields in command register
 */
#define FDC_RATE6   0x00    /* not 2, but  6 msec steprate */
#define FDC_RATE12  0x01    /* not 3, but 12 msec steprate */
#define FDC_RATE2   0x02    /* not 5, but  2 msec steprate */
#define FDC_RATE3   0x03    /* not 6, but  3 msec steprate */
#define FDC_VBIT    0x04    /* verify sector ID */
#define FDC_HBIT    0x08    /* suppress motor on sequence */
#define FDC_UBIT    0x10    /* update track register */
#define FDC_EBIT    0x04    /* wait 30 msec to settle */
#define FDC_MBIT    0x10    /* multi-sector */
#define FDC_PBIT    0x02    /* write precompensate */
#define FDC_A0BIT   0x01    /* suppress (?) data address mark */
#define FDC_IINDEX  0x04    /* interrupt on each index pulse */
#define FDC_IFORCE  0x08    /* force interrupt */

/*
 * status register
 */
#define FDC_BUSY    0x01    /* set if command under execution */
#define FDC_DRQ     0x02    /* Data Register status (pin c1) */
#define FDC_LOSTDAT 0x04    /* lost data */
#define FDC_TRACK0  0x04    /* track 0 */
#define FDC_CRCERR  0x08    /* CRC error */
#define FDC_RNF     0x10    /* Record Not Found */
#define FDC_RT_SU   0x20    /* Record Type; Spin Up completed */
#define FDC_WRI_PRO 0x40    /* Write Protected */
#define FDC_MOTORON 0x80    /* Motor On */


#endif /* FDC_H */
