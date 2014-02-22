/*
 *  biosmem.h - dumb bios-level memory management
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * Authors:
 *  LVL    Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "biosmem.h"
#include "kprint.h"
#include "tosvars.h"


extern MD themd;            /* BIOS memory descriptor (from tosvars.S) */

static int bmem_allowed;

/*
 * bmem_init - initialize some memory related variables
 *
 * when the TOS is in ROM, the ST RAM is used thus:
 *   0        400   end                         v_bas_ad   phystop
 *   +---------+-----+------------------------------+--------+
 *   | vectors | BSS | Transiant Program Area (TPA) | screen |
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
 *   LONG os_end     header field in _sysbase+12: start of free RAM
 *                   (set at end when compiling, patched in RAM TOS)
 *   LONG membot     TOS variable in 0x432: bottom of TPA
 *   LONG memtop     TOS variable in 0x436: top of TPA
 *   LONG phystop    TOS variable in 0x43a: top of ST RAM
 *
 * For unknown reasons, it is reported that in the first RAM TOS membot
 * and end_os id have different values. Here in EmuTOS we will always
 * have:
 *   membot = end_os = os_end = start of TPA
 *   memtop = end of TPA
 *
 */
void bmem_init(void)
{
    KDEBUG(("_etext = 0x%08lx\n", (LONG)_etext));
    KDEBUG(("_edata = 0x%08lx\n", (LONG)_edata));
    KDEBUG(("end    = 0x%08lx\n", (LONG)end));

    /* initialise some memory variables */
    membot = end_os = os_end;
#if CONF_VRAM_ADDRESS
    /* Available ST-RAM ends at the physical ST-RAM end */
    memtop = (LONG) phystop;
#else
    /* Available ST-RAM ends at the screen start */
    memtop = (LONG) v_bas_ad;
#endif

    /* Fill out the first memory descriptor */
    themd.m_link = (MD*) 0;     /* no next memory descriptor */
    themd.m_start = os_end;
    themd.m_length = memtop - themd.m_start;
    themd.m_own = (PD*) 0;      /* no owner's process descriptor */

    bmem_allowed = 1;
}



/*
 * balloc - simple BIOS memory allocation
 */
void * balloc(LONG size)
{
    void * ret;

    if(!bmem_allowed) {
        panic("balloc(%ld) at wrong time\n", size);
    }

    /* Round the size to a multiple of 4 bytes to keep alignment.
     * Alignment on long boundaries matters in FastRAM. */
    size = (size + 3) & ~3;

    if(themd.m_length < size) {
        panic("balloc(%ld): no memory\n", size);
    }

    ret = (void*) themd.m_start;

    /* subtract needed memory from initial MD */
    themd.m_length -= size;
    themd.m_start += size;

    KDEBUG(("BIOS: getmpb m_start = 0x%08lx, m_length = 0x%08lx\n",
             (LONG) themd.m_start, themd.m_length));

    return ret;
}

void getmpb(MPB * mpb)
{
    bmem_allowed = 0; /* BIOS memory handling not allowed past this point */

    mpb->mp_mfl = mpb->mp_rover = &themd;   /* free list/rover set to init MD */
    mpb->mp_mal = (MD *)0;                /* allocated list set to NULL */
}
