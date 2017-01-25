/*
 *  biosmem.h - dumb bios-level memory management
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL    Laurent Vogel
 *  VRI    Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */
#define DBG_BALLOC 0

#include "config.h"
#include "portab.h"
#include "biosmem.h"
#include "kprint.h"
#include "tosvars.h"

#if DBG_BALLOC
static BOOL bmem_allowed;
#endif

/*
 * bmem_init - initialize some memory related variables
 *
 * when TOS is in ROM, the ST RAM is used thus:
 *   0        400   end                         v_bas_ad   phystop
 *   +---------+-----+------------------------------+--------+
 *   | vectors | BSS | Transient Program Area (TPA) | screen |
 *   +---------+-----+------------------------------+--------+
 * when it is in RAM, the memory map is:
 *   0        400   end       _edata            v_bas_ad   phystop
 *   +---------+-----+-----------+------------------+--------+
 *   | vectors | BSS | TEXT+DATA |       TPA        | screen |
 *   +---------+-----+-----------+------------------+--------+
 *
 * variables and symbols:
 *   BYTE end[]      set by GNU LD: end of BSS
 *   BYTE _etext[]   set by GNU LD: end of TEXT segment
 *   BYTE _edata[]   set by GNU LD: end of DATA segment
 *   LONG end_os     TOS variable in 0x4fa: start of the TPA
 *   LONG membot     TOS variable in 0x432: bottom of TPA
 *   LONG memtop     TOS variable in 0x436: top of TPA
 *   LONG phystop    TOS variable in 0x43a: top of ST RAM
 *
 * For unknown reasons, it is reported that in the first RAM TOS membot
 * and end_os id have different values. Here in EmuTOS we will always
 * have:
 *   membot = end_os = start of TPA
 *   memtop = end of TPA
 *
 */
void bmem_init(void)
{
    KDEBUG(("Memory map before balloc() adjustments:\n"));
    KDEBUG(("        _text = %p\n", _text));
    KDEBUG(("       _etext = %p\n", _etext));
    KDEBUG(("        _data = %p\n", _data));
    KDEBUG(("       _edata = %p\n", _edata));
    KDEBUG(("         _bss = %p\n", _bss));
    KDEBUG(("   _endvdibss = %p\n", _endvdibss));
#if WITH_AES
    KDEBUG(("   _endgembss = %p\n", _endgembss));
#endif
    KDEBUG(("        _ebss = %p\n", _ebss));
    KDEBUG(("_end_os_stram = %p\n", _end_os_stram));

    /* Start of available ST-RAM */
    end_os = _end_os_stram;
    membot = end_os;
    KDEBUG(("       membot = %p\n", membot));

    /* End of available ST-RAM */
    /* The screen buffer will be allocated later */
    memtop = phystop;
    KDEBUG(("       memtop = %p\n", memtop));

#if DBG_BALLOC
    bmem_allowed = TRUE;
#endif
}



/*
 * Allocate an ST-RAM buffer.
 * This is only allowed before BDOS is initialized.
 *   size: requested size of buffer, in bytes
 *   top: TRUE to get a block at top of memory, FALSE for bottom.
 */
UBYTE *balloc_stram(ULONG size, BOOL top)
{
    UBYTE *ret;

    KDEBUG(("before balloc_stram: membot=%p, memtop=%p\n", membot, memtop));
    KDEBUG(("balloc_stram(%lu, %d)\n", size, top));

#if DBG_BALLOC
    if (!bmem_allowed)
        panic("balloc_stram(%lu) at wrong time\n", size);
#endif

    /* Round the size to a multiple of 4 bytes to keep alignment.
     * Alignment on long boundaries is faster in FastRAM. */
    size = (size + 3) & ~3;

    if (memtop - membot < size)
        panic("balloc_stram(%lu): not enough memory\n", size);

    if (top)
    {
        /* Allocate the new buffer at the top of the ST-RAM */
        memtop -= size;
        ret = memtop;
    }
    else
    {
        /* Allocate the new buffer at the bottom of the ST-RAM */
        ret = membot;
        membot += size;
    }

    KDEBUG(("balloc_stram(%lu, %d) returns %p\n", size, top, ret));
    KDEBUG((" after balloc_stram: membot=%p, memtop=%p\n", membot, memtop));

    return ret;
}

extern MD themd;                /* BIOS memory descriptor */

void getmpb(MPB * mpb)
{
#if DBG_BALLOC
    bmem_allowed = FALSE; /* BIOS memory handling not allowed past this point */
#endif
    KDEBUG(("Memory map after balloc() adjustments:\n"));
    KDEBUG(("       membot = %p\n", membot));
    KDEBUG(("       memtop = %p\n", memtop));

    /* Fill out the first memory descriptor */
    themd.m_link = NULL;        /* no next memory descriptor */
    themd.m_start = membot;
    themd.m_length = memtop - themd.m_start;
    themd.m_own = NULL;         /* no owner's process descriptor */

    mpb->mp_mfl = &themd;       /* free list set to initial MD */
    mpb->mp_mal = NULL;         /* allocated list empty */
}
