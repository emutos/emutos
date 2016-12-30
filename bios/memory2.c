/*
 * memory2.c - Memory functions
 *
 * Copyright (C) 2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "memory.h"
#include "tosvars.h"
#include "kprint.h"
#include "machine.h"
#include "cookie.h"
#include "vectors.h"

extern long xmaddalt(UBYTE *start, long size); /* found in bdos/mem.h */

#if CONF_WITH_FASTRAM

/* Detect hardware FastRAM size */
static ULONG detect_fastram_size(void)
{
#if CONF_FASTRAM_SIZE
    /* Fixed-size FastRAM */
    return CONF_FASTRAM_SIZE;
#else
    UBYTE *addr;

    /* By design, TT-like FastRAM is always in 32-bit address space */
    if (!IS_BUS32)
        return 0;

#if CONF_WITH_TT_MMU
    if (cookie_mch == MCH_TT)
    {
        /* On TT, FastRAM requires special refresh rate initialization */
        init_tt_fastram();
    }
#endif /* CONF_WITH_TT_MMU */

    /* Detect FastRAM size. Assume that the size is always a multiple of 1 MB.
     * Try to read the last byte of each FastRAM megabyte. If it does not cause
     * a Bus Error, assume that that megabyte is valid FastRAM.
     * We artificially limit FastRAM detection to positive addresses to avoid
     * shocking BDOS. */
    for (addr = FASTRAM_START + 1024UL*1024 - 1;
        (long)addr > 0; addr += 1024UL*1024)
    {
        if (!check_read_byte((long)addr))
            break;
    }

    /* Valid FastRAM stops at the beginning of current megabyte */
    return ((ULONG)addr & 0xfff00000) - (ULONG)FASTRAM_START;
#endif /* !CONF_FASTRAM_SIZE */
}

#endif /* CONF_WITH_FASTRAM */

/* TT-like FastRAM: Detect, initialize and add to BDOS */
void fastram_init(void)
{
#if CONF_WITH_FASTRAM
    if (ramvalid == RAMVALID_MAGIC)
    {
        /* Previous FastRAM settings were valid. Just trust them blindly. */
    }
    else
    {
        /* Previous FastRAM settings were invalid, detect them now. */
        ULONG fastram_size = detect_fastram_size();

        if (fastram_size)
            ramtop = FASTRAM_START + fastram_size;
        else
            ramtop = NULL;

        /* Always validate ramvalid even if ramtop == 0, like TOS 1.6+ */
        ramvalid = RAMVALID_MAGIC;
    }
#else
    /* Force no FastRAM */
    ramtop = NULL;
    ramvalid = RAMVALID_MAGIC;
#endif

    KDEBUG(("fastram_init(): ramtop=%p\n", ramtop));

#if CONF_WITH_FASTRAM
    /* Add eventual FastRAM to BDOS pool */
    if (ramtop != NULL)
        xmaddalt(FASTRAM_START, ramtop - FASTRAM_START);
#endif
}
