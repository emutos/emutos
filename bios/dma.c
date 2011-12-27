/*
 * dma.c - DMA disk routines
 *
 * Copyright (c) 2011 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "dma.h"
#include "mfp.h"
#include "tosvars.h"

void set_dma_addr(ULONG addr)
{
    DMA->addr_low = addr;
    DMA->addr_med = addr>>8;
    DMA->addr_high = addr>>16;
}

#if CONF_WITH_MFP

/* returns 1 if the timeout (milliseconds) elapsed before gpip went low */
int timeout_gpip(LONG delay)
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

#endif /* CONF_WITH_MFP */
