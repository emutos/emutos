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
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

extern long xmaddalt(UBYTE *start, long size); /* found in bdos/mem.h */

#if CONF_WITH_TTRAM

/* Detect hardware TT-RAM size */
static ULONG detect_ttram_size(void)
{
#if CONF_TTRAM_SIZE
    /* Fixed-size TT-RAM */
    return CONF_TTRAM_SIZE;
#else
    UBYTE *addr;

    /* By design, TT-RAM is always in 32-bit address space */
    if (!IS_BUS32)
        return 0;

#if CONF_WITH_TT_MMU
    if (cookie_mch == MCH_TT)
    {
        /* On TT, TT-RAM requires special refresh rate initialization */
        init_ttram();
    }
#endif /* CONF_WITH_TT_MMU */

    /* Detect TT-RAM size. Assume that the size is always a multiple of 1 MB.
     * Try to read the last byte of each TT-RAM megabyte. If it does not cause
     * a Bus Error, assume that that megabyte is valid TT-RAM.
     * We artificially limit TT-RAM detection to positive addresses to avoid
     * shocking BDOS. */
    for (addr = TTRAM_START + 1024UL*1024 - 1;
        (long)addr > 0; addr += 1024UL*1024)
    {
        if (!check_read_byte((long)addr))
            break;
    }

    /* Valid TT-RAM stops at the beginning of current megabyte */
    return ((ULONG)addr & 0xfff00000) - (ULONG)TTRAM_START;
#endif /* !CONF_TTRAM_SIZE */
}

#endif /* CONF_WITH_TTRAM */

/* TT-RAM: Detect, initialize and add to BDOS */
static void ttram_init(void)
{
#if CONF_WITH_TTRAM
    if (ramvalid == RAMVALID_MAGIC)
    {
        /* Previous TT-RAM settings were valid. Just trust them blindly. */
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

    KDEBUG(("ttram_init(): ramtop=%p\n", ramtop));

#if CONF_WITH_TTRAM
    /* Add eventual TT-RAM to BDOS pool */
    if (ramtop != NULL)
        xmaddalt(TTRAM_START, ramtop - TTRAM_START);
#endif
}

/* Detect and initialize all Alt-RAM */
void altram_init(void)
{
    /* Always intialize the TT-RAM system variables */
    KDEBUG(("ttram_init()\n"));
    ttram_init();

#if CONF_WITH_ALT_RAM

#if CONF_WITH_MONSTER
    /* Add MonSTer alt-RAM detected in machine.c */
    if (has_monster)
    {
        /* Dummy read from MonSTer register to initiate write sequence. */
        unsigned short monster_reg = *(volatile unsigned short *)MONSTER_REG;

        /* Only enable 6Mb when on a Mega STE due to address conflict with
           VME bus. Todo: This should be made configurable. */
        if (has_vme)
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

#ifdef MACHINE_AMIGA
    KDEBUG(("amiga_add_alt_ram()\n"));
    amiga_add_alt_ram();
#endif

#endif /* CONF_WITH_ALT_RAM */
}