/*
 * dma.c - DMA disk routines
 *
 * Copyright (C) 2011-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "dma.h"
#include "tosvars.h"
#include "cookie.h"
#include "kprint.h"

void set_dma_addr(UBYTE *addr)
{
    UBYTE_ALIAS* b = (UBYTE_ALIAS*)&addr;

    DMA->addr_low = b[3];
    DMA->addr_med = b[2];
    DMA->addr_high = b[1];
}

#if CONF_WITH_FRB

/*
 * Get a buffer suitable for disk DMA (floppy and ACSI).
 * - If the provided buffer is in ST-RAM, it is just returned
 * - If the provided buffer is in Alt-RAM, then the _FRB buffer is returned
 */
UBYTE *get_stram_disk_buffer(UBYTE *userbuf)
{
    UBYTE *iobuf;

    /* If the user buffer is inside the ST-RAM, that's fine. */
    if (userbuf < phystop)
        return userbuf;

    /* The user buffer is in Alt-RAM: we need to use the _FRB */
    iobuf = get_frb_cookie();
    if (!iobuf)
        panic("Missing _FRB for user buffer %p\n", userbuf);

    return iobuf;
}

#endif
