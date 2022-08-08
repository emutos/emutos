/*
 * memory2.c - Memory functions
 *
 * Copyright (C) 2016-2022 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "biosmem.h"
#include "asm.h"
#include "memory.h"
#include "tosvars.h"
#include "machine.h"
#include "has.h"
#include "cookie.h"
#include "biosext.h"    /* for cache control routines */
#include "bios.h"
#include "vectors.h"
#include "../bdos/bdosstub.h"
#include "amiga.h"
#include "string.h"

#define ZONECOUNT   32      /* for memory test */

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


/* List of information about available physical memory */
static struct memory_block_t *memory_info = NULL;

/* Returns the information about the memory detected by the BIOS */
struct memory_block_t *bget_memory_info(void)
{
    return memory_info;
}

/* Registers detected memory to the BIOS */
void bmem_register(const void *start, ULONG size)
{
    struct memory_block_t *new;

    new = (void*)balloc_stram((ULONG)sizeof(struct memory_block_t), FALSE);
    if (new == NULL)
        KDEBUG(("Failed to register memory at %p, size:%lu\n", start, size));
    new->start = (void*)start;
    new->size = size;
    new->next = memory_info;
    memory_info = new;
    KDEBUG(("Registered memory: at %p, size:%lu\n", name, type, start, size));
}

#if CONF_WITH_ALT_RAM

void altram_init(void)
{
#if CONF_WITH_STATIC_ALT_RAM && defined(STATIC_ALT_RAM_SIZE)
    bmem_register((void*)STATIC_ALT_RAM_ADDRESS, STATIC_ALT_RAM_SIZE);
#endif

#if CONF_WITH_TTRAM
    /* Add eventual TT-RAM to BDOS pool */
    if (ramtop != NULL)
        bmem_register(TTRAM_START, ramtop - TTRAM_START);
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
        bmem_register((void *)0x400000L, monster_reg*0x100000L);
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

        bmem_register((void *)0x400000L, magnum_ram*0x100000L);
    }
#endif  /* CONF_WITH_MAGNUM */

#ifdef MACHINE_AMIGA
    KDEBUG(("amiga_add_alt_ram()\n"));
    amiga_add_alt_ram();
#endif
}

#endif /* CONF_WITH_ALT_RAM */

#if CONF_WITH_MEMORY_TEST

/*
 * test one memory 'zone' (contiguous memory area)
 *
 * returns TRUE if ok, FALSE if error
 */
static BOOL testzone(UBYTE *start, LONG size)
{
    ULONG *p;
    ULONG *end = (ULONG *)(start + size);
    ULONG n;

    /* set all bits on & verify */
    memset(start, 0xff, size);
    if (!memtest_verify((ULONG *)start, 0xffffffffUL, size))
        return FALSE;

    /* rotate bit & verify */
    /* note: converting the setup part to assembler gives only a slight speedup */
    for (p = (ULONG *)start, n = 1; p < end; )
    {
        roll(n,1);
        *p++ = n;
    }
    if (!memtest_rotate_verify((ULONG *)start, size))
        return FALSE;

    /* set all bits off & verify */
    memset(start, 0x00, size);
    if (!memtest_verify((ULONG *)start, 0UL, size))
        return FALSE;

    return TRUE;
}

static void init_line(BOOL is_ttram)
{
    /* disable line wrap, display title, switch to inverse video */
    cprintf("\x1bw%cT RAM \x1bp", is_ttram ? 'T' : 'S');
    /* save cursor posn, display 32 spaces, restore cursor posn */
    cprintf("\x1bj%32s\x1bk", " ");
}

static void end_line(LONG memsize)
{
    char type;

    /*
     * we display the size as KB when under 4MB, to allow for
     * sizes like 512K or 2.5MB
     */
    if (memsize < 4*1024*1024L)
    {
        memsize >>= 10;     /* convert to KB */
        type = 'K';
    }
    else
    {
        memsize >>= 20;     /* convert to MB */
        type = 'M';
    }

    /* backspace, display memory size, disable inverse video */
    cprintf("\b\b\b\b\b\b\b\b%5ld %cB\n\x1bq",memsize,type);
}

/*
 * test one type of RAM (ST RAM or TT RAM)
 */
static BOOL testtype(BOOL is_ttram, LONG memsize)
{
    UBYTE *testaddr, *startaddr;
    LONG zonesize;
    WORD i;
    BOOL ok;

    init_line(is_ttram);
    startaddr = is_ttram ? TTRAM_START : (UBYTE *)0L;
    zonesize = (memsize / ZONECOUNT);
    for (i = 0, testaddr = startaddr; i < ZONECOUNT; i++, testaddr += zonesize)
    {
        ok = TRUE;
        /* we skip testing areas in use by the system! */
        if (is_ttram
         || ((testaddr >= membot) && (testaddr+zonesize <= memtop)))
            ok = testzone(testaddr, zonesize);
        cprintf(ok?"-":"X");
        if (bconstat(2))    /* abort */
        {
            bconin(2);
            cprintf("\n\x1bq"); /* new line, disable inverse video */
            return FALSE;
        }
    }
    end_line(memsize);      /* display memory size */

    return TRUE;
}

/*
 * perform a simple memory test with visual feedback and the option to
 * abort.  we test ST RAM, followed by TT RAM.  within each type, we
 * test a 'zone' of memory at a time, where a zone is 1/32 of the total
 * amount and is assumed to be an even length, aligned on an even boundary.
 *
 * returns TRUE if OK, FALSE if aborted
 */
BOOL memory_test(void)
{
    /* handle ST RAM */
    if (!testtype(FALSE, (LONG)phystop))
        return FALSE;

#if CONF_WITH_TTRAM
    /* handle TT RAM */
    if (ramtop)         /* TT RAM detected */
        if (!testtype(TRUE, ramtop-TTRAM_START))
            return FALSE;
#endif

    return TRUE;
}

#endif
