/*
 * memory2.c - Memory functions
 *
 * Copyright (C) 2016-2020 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "memory.h"
#include "tosvars.h"
#include "machine.h"
#include "has.h"
#include "cookie.h"
#include "biosext.h"    /* for cache control routines */
#include "vectors.h"
#include "../bdos/bdosstub.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

UBYTE meminit_flags;

#if CONF_WITH_TTRAM

/* Detect hardware TT-RAM size */
static ULONG detect_ttram_size(void)
{
#if CONF_TTRAM_SIZE
    /* Fixed-size TT-RAM */
    return CONF_TTRAM_SIZE;
#else
    volatile UBYTE *addr;

    /* By design, TT-RAM is always in 32-bit address space */
    if (!IS_BUS32)
        return 0;

    /* Special note for TT: we can safely use the TT-RAM here, as the refresh
     * rate has already been set in memconf(). */

    /* Detect TT-RAM size. Assume that the size is always a multiple of 1 MB.
     * Try to read the last byte of each TT-RAM megabyte. If it does not cause
     * a Bus Error, assume that that megabyte is valid TT-RAM.
     * We artificially limit TT-RAM detection to positive addresses to avoid
     * shocking BDOS. */
    for (addr = TTRAM_START + 1024UL*1024 - 1;
        (long)addr > 0; addr += 1024UL*1024)
    {
        /* First check for bus error, then try writing to memory and
         * reading back. */
        if (!check_read_byte((long)addr))
            break;

        *addr     = 0x12;
        *(addr-1) = 0x34;
        invalidate_data_cache((UBYTE *)(addr-1), 2);
        if ((*addr != 0x12) || (*(addr-1) != 0x34))
            break;
    }

    /* Valid TT-RAM stops at the beginning of current megabyte */
    return ((ULONG)addr & 0xfff00000) - (ULONG)TTRAM_START;
#endif /* !CONF_TTRAM_SIZE */
}

#endif /* CONF_WITH_TTRAM */

/* Detect TT-RAM and set ramtop/ramvalid */
void ttram_detect(void)
{
#if CONF_WITH_TTRAM
    if (ramvalid == RAMVALID_MAGIC)
    {
        /* Previous TT-RAM settings were valid. */
        if (ramtop != NULL)
        {
            /* There was some TT-RAM. Be sure it is still valid.
             * TT-RAM may disappear on CT60, after reset into 68030 mode. */
            if (ramtop < (TTRAM_START + 1)
                || !IS_BUS32
#if CONF_WITH_BUS_ERROR
                || !check_read_byte((long)TTRAM_START) /* First byte */
                || !check_read_byte((long)(ramtop - 1)) /* Last byte */
#endif
            ) {
                /* Previous TT-RAM settings aren't valid any more */
                ramtop = NULL;
            }
        }
    }
    else
    {
        /* Previous TT-RAM settings were invalid, detect them now. */
        ULONG ttram_size = detect_ttram_size();

        if (ttram_size)
            ramtop = TTRAM_START + ttram_size;
        else
            ramtop = NULL;

        /* Always validate ramvalid even if ramtop == 0, like TOS 1.6+ */
        ramvalid = RAMVALID_MAGIC;
    }
#else
    /* Force no TT-RAM */
    ramtop = NULL;
    ramvalid = RAMVALID_MAGIC;
#endif

    KDEBUG(("ttram_detect(): ramtop=%p\n", ramtop));
}

#if CONF_WITH_ALT_RAM

/* Initialize all Alt-RAM */
void altram_init(void)
{
#if CONF_WITH_STATIC_ALT_RAM && defined(STATIC_ALT_RAM_SIZE)
    KDEBUG(("xmaddalt() static adr=%p size=%ld\n",
        (UBYTE *)STATIC_ALT_RAM_ADDRESS, STATIC_ALT_RAM_SIZE));
    xmaddalt((UBYTE *)STATIC_ALT_RAM_ADDRESS, STATIC_ALT_RAM_SIZE);
    return;
#endif

#if CONF_WITH_TTRAM
    /* Add eventual TT-RAM to BDOS pool */
    if (ramtop != NULL)
        xmaddalt(TTRAM_START, ramtop - TTRAM_START);
#endif

#if CONF_WITH_MONSTER
    /* Add MonSTer Alt-RAM detected in machine.c */
    if (has_monster)
    {
        /* Dummy read from MonSTer register to initiate write sequence. */
        unsigned short monster_reg = *(volatile unsigned short *)MONSTER_REG;

        /* Only enable 6Mb when on a Mega STE due to address conflict with
           VME bus. Todo: This should be made configurable. */
        if (cookie_mch == MCH_MSTE)
            monster_reg = 6;
        else
            monster_reg = 8;

        /* Register write sequence: read - write - write */
        *(volatile unsigned short *)MONSTER_REG = monster_reg;
        *(volatile unsigned short *)MONSTER_REG = monster_reg;
        KDEBUG(("xmaddalt()\n"));
        xmaddalt((UBYTE *)0x400000L, monster_reg*0x100000L);
    }
#endif

#if CONF_WITH_MAGNUM
    /* Size and add Magnum Alt-RAM. The Magnum is detected in machine.c. */
    if (has_magnum)
    {
        unsigned short magnum_ram;
        const unsigned short magnum_check1 = 0x55AA;
        const unsigned short magnum_check2 = 0xAA55;

        if (!HAS_NOVA && check_read_byte(0xC00000))
        {
            /*
             * If a 16 MB SIMM is installed, only 10 MB can be used.
             * If Nova/Vofa is installed that also uses address 0xC00000
             * in the ST/STE, at most 8 MB of Magnum Alt-RAM can be used.
             */
            magnum_ram = 10;
        }
        else if (check_read_byte(0x800000))
        {
            magnum_ram = 8;
        }
        else
        {
            /* detect_magnum() has already checked at address 0x400000. */
            magnum_ram = 4;
        }

        /* Only enable 6 MB on a Mega STE due to address conflict with
           VME bus. */
        if ((cookie_mch == MCH_MSTE) && (magnum_ram > 6))
            magnum_ram = 6;

        /* Due to its HW design, when no SIMM is inserted at all, the
           Magnum will report 8 MB of RAM. Thus, do a sanity check. */
        *((volatile short *)0x400000L) = magnum_check1;
        *((volatile short *)0x400002L) = magnum_check2;
        if (*((volatile short *)0x400000L) != magnum_check1)
            magnum_ram = 0;

        KDEBUG(("xmaddalt()\n"));
        xmaddalt((UBYTE *)0x400000L, magnum_ram*0x100000L);
    }
#endif  /* CONF_WITH_MAGNUM */

#ifdef MACHINE_AMIGA
    KDEBUG(("amiga_add_alt_ram()\n"));
    amiga_add_alt_ram();
#endif
}

#endif /* CONF_WITH_ALT_RAM */
