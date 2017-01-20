/*
 *  biosmem.h - dumb bios-level memory management
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL    Laurent Vogel
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


extern MD themd;            /* BIOS memory descriptor */

#if DBG_BALLOC
static int bmem_allowed;
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
    KDEBUG(("_etext     = 0x%08lx\n", (LONG)_etext));
    KDEBUG(("_edata     = 0x%08lx\n", (LONG)_edata));
    KDEBUG(("_endvdibss = 0x%08lx\n", (LONG)_endvdibss));
#if WITH_AES
    KDEBUG(("_endgembss = 0x%08lx\n", (LONG)_endgembss));
#endif
    KDEBUG(("_end       = 0x%08lx\n", (LONG)_end));

    /* Start of available ST-RAM */
#if EMUTOS_LIVES_IN_RAM
    /*
     * When EmuTOS is run from the RAM, the BSS starts at address 0 as usual,
     * but the TEXT and DATA segments are just after the BSS.
     * Thus the first unused RAM address is the end of the DATA segment.
     */
    end_os = _edata;
#else
    /*
     * When EmuTOS is run from the ROM, the TEXT and DATA segments stays in the
     * ROM, but the BSS starts at address 0.
     * Thus the first unused RAM address is the end of the BSS segment.
     */
    end_os = _end;
#endif
    membot = end_os;
    KDEBUG(("membot     = 0x%08lx\n", (LONG)membot));

    /* End of available ST-RAM */
#if CONF_VRAM_ADDRESS
    /* Available ST-RAM ends at the physical ST-RAM end */
    memtop = phystop;
#else
    /* Available ST-RAM ends at the screen start */
    memtop = v_bas_ad;
#endif
    KDEBUG(("memtop     = 0x%08lx\n", (LONG)memtop));

    /* Fill out the first memory descriptor */
    themd.m_link = (MD*) 0;     /* no next memory descriptor */
    themd.m_start = membot;
    themd.m_length = memtop - themd.m_start;
    themd.m_own = (PD*) 0;      /* no owner's process descriptor */

#if DBG_BALLOC
    bmem_allowed = 1;
#endif
}



/*
 * balloc - simple BIOS memory allocation
 */
UBYTE *balloc(LONG size)
{
    UBYTE *ret;

#if DBG_BALLOC
    if(!bmem_allowed) {
        panic("balloc(%ld) at wrong time\n", size);
    }
#endif

    /* Round the size to a multiple of 4 bytes to keep alignment.
     * Alignment on long boundaries is faster in FastRAM. */
    size = (size + 3) & ~3;

    if(themd.m_length < size) {
        panic("balloc(%ld): no memory\n", size);
    }

    ret = themd.m_start;

    /* subtract needed memory from initial MD */
    themd.m_length -= size;
    themd.m_start += size;

    KDEBUG(("BIOS: getmpb m_start = 0x%08lx, m_length = 0x%08lx\n",
             (ULONG) themd.m_start, themd.m_length));

    return ret;
}

void getmpb(MPB * mpb)
{
#if DBG_BALLOC
    bmem_allowed = 0; /* BIOS memory handling not allowed past this point */
#endif

    mpb->mp_mfl = &themd;       /* free list set to initial MD */
    mpb->mp_mal = NULL;         /* allocated list empty */
}
