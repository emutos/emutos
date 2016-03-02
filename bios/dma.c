/*
 * dma.c - DMA disk routines
 *
 * Copyright (c) 2011-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "dma.h"

void set_dma_addr(UBYTE *addr)
{
    ULONG value = (ULONG)addr;

    DMA->addr_low = value;
    DMA->addr_med = value>>8;
    DMA->addr_high = value>>16;
}
