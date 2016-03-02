/*
 * dma.h - dma definitions
 *
 * Copyright (c) 2001-2015 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef DMA_H
#define DMA_H

#include "portab.h"

#define DMA     ((volatile struct dma *) 0xFFFF8600)

struct dma {
    UWORD   pad0[2];
     WORD   data;       /* sector count, data register */
     WORD   control;    /* status/control register */
    UBYTE   pad1;
    UBYTE   addr_high;
    UBYTE   pad2;
    UBYTE   addr_med;
    UBYTE   pad3;
    UBYTE   addr_low;
    UBYTE   pad4;
                    /* the "mode/control" register is only present on the Falcon */
    UBYTE   modectl;    /* R: bit 3 is tested during write/format */
                        /* W: floppy density control (DD or HD) */
};

/*
 * Control register bits
 */
/*                  0x0001             not used */
#define DMA_A0      0x0002          /* signal A0 to fdc/hdc */
#define DMA_A1      0x0004          /* signal A1 to fdc/hdc */
#define DMA_HDC     0x0008          /* must be on if accessing hdc */
#define DMA_SCREG   0x0010          /* access sector count register */
/*                  0x0020             reserved */
#define DMA_NODMA   0x0040          /* no DMA (yet) */
#define DMA_FDC     0x0080          /* must be on if accessing fdc */
#define DMA_WRBIT   0x0100          /* write to fdc/hdc via dma_data */

/*
 * Status register bits
 */
#define DMA_OK      0x0001          /* something wrong */
#define DMA_SCNOT0  0x0002          /* sector count not 0 */
#define DMA_DATREQ  0x0004          /* FDC data request signal */

/*
 * Falcon "mode/control" bits
 */
#define DMA_MCBIT3  0x08            /* must be zero before starting write */

void set_dma_addr(UBYTE *addr);

#endif /* DMA_H */
