/*
 * emutos.ld - Custom linker script for EmuTOS ROM
 *
 * Copyright (C) 2016-2024 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* Note: this linker script is preprocessed,
 * to allow #include, #define, #if, etc. */
#include "include/config.h"

/* Create a plain binary file, without any header */
OUTPUT_FORMAT(binary)

#if !EMUTOS_LIVES_IN_RAM

/* The ROM address depends on the target */
#if defined(MACHINE_AMIGA)
# define ROM_ORIGIN 0x00fc0000
#elif defined(TARGET_CART)
# define ROM_ORIGIN 0x00fa0000
#elif defined(TARGET_192)
# define ROM_ORIGIN 0x00fc0000
#else
# define ROM_ORIGIN 0x00e00000
#endif

/* Always assume there is a lot of ROM space, so the link will always succeed.
 * Then our padding tool will eventually report an overflow, with a better
 * diagnostic message than the linker.
 */
#define ROM_LENGTH 1M /* Maximum size before I/O area */

#endif /* !EMUTOS_LIVES_IN_RAM */

/* Memory regions of a typical Atari computer.
 * Only start addresses matter. Lengths just need to be large enough.
 */
MEMORY
{
    stram : ORIGIN = 0x00000000, LENGTH = 14M
#if !EMUTOS_LIVES_IN_RAM
    rom : ORIGIN = ROM_ORIGIN, LENGTH = ROM_LENGTH
#endif
#if CONF_WITH_STATIC_ALT_RAM
    altram : ORIGIN = STATIC_ALT_RAM_ADDRESS, LENGTH = 1M
#endif
}

/* Region used as read/write memory */
#if CONF_WITH_STATIC_ALT_RAM
REGION_ALIAS("REGION_RAM", altram)
#else
REGION_ALIAS("REGION_RAM", stram)
#endif

/* Regions used by EmuTOS sections */
#if EMUTOS_LIVES_IN_RAM
REGION_ALIAS("REGION_READ_ONLY", REGION_RAM)
#else
REGION_ALIAS("REGION_READ_ONLY", rom)
#endif
REGION_ALIAS("REGION_READ_WRITE", REGION_RAM)

SECTIONS
{
    /* First section in ST-RAM */
    .first_stram :
    {
        /* System variables are located at fixed addresses */
#include "tosvars.ld"
        . = 0x800; /* Extend sysvars area upto end of supervisor-only RAM */
        sysvars_end = .;
    } >stram

#if CONF_WITH_STATIC_ALT_RAM
    /* First section in static Alt-RAM */
    .first_altram :
    {
        __static_altram_start = .;
    } >altram
#endif

    /* This section is the internal BIOS stack.
     * It will *not* be cleared on startup or reset.
     */
    .stack :
    {
        _stkbot = .;
        . += 2K;
        _stktop = .;
    } >REGION_READ_WRITE

    /* This section is located as low as possible in ST-RAM,
     * but after eventual BIOS stack.
     * Variables requiring very low addresses, while being accessible
     * from user mode, can be put here.
     */
    .low_stram : SUBALIGN(2)
    {
        __low_stram_start = .;
        obj/lowstram.o(.bss COMMON)
        obj/lineavars.o(.bss COMMON)
        __low_stram_end = .;
        ASSERT(ABSOLUTE(_shifty) < 0x8000, "error: bad _shifty address");
        ASSERT((ABSOLUTE(_dskbuf) & 3) == 0, "error: bad _dskbuf alignment");
        ASSERT(ABSOLUTE(__low_stram_end) <= 0x8000, "error: .low_stram section extends beyond 32k");
    } >stram

    /* The TEXT segment contains EmuTOS executable code and read-only data.
     * It is always read-only, so it can safely live in ROM.
     */
    .text : SUBALIGN(2)
    {
        CREATE_OBJECT_SYMBOLS
        __text = .;
        *(.text .rodata .rodata.*)
        /* Note: .rodata sections are only present in ELF objects.
         * We mix .text and .rodata to keep critical parts (such as BIOS
         * and fonts) at the beginning of the ROM.
         */
        __etext = .;
    } >REGION_READ_ONLY =0x4afc

    /* FIXME: Our DATA segment is currently read-only.
     * It currently lives in the ROM, just after the TEXT segment.
     * This means that initialized global variables can't be modified.
     * Since a read-only DATA segment is useless, we try to keep it empty by
     * making all initialized variables const, so they go to the TEXT segment.
     */
    .data : SUBALIGN(2)
    {
        __data = .;
        *(.data)
        __edata = .;
    } >REGION_READ_ONLY

    /* The BSS segment contains uninitialized global variables.
     * It will be cleared by the startup code.
     */
    .bss : SUBALIGN(2)
    {
        __bss = .;
        *(.bss COMMON)
        __ebss = .;
    } >REGION_READ_WRITE

    /* This section is always the last one stored in ST-RAM.
     * It is usually empty, just used to calculate the last address
     * of ST-RAM statically used by EmuTOS. If needed, the address
     * is rounded up to the next multiple of four.
     */
    .laststram : ALIGN(4)
    {
        __end_os_stram = .;
    } >stram

#if CONF_WITH_STATIC_ALT_RAM
    /* Last section in static Alt-RAM */
    .last_altram :
    {
        __static_altram_end = .;
    } >altram
#endif

    /* Discard the following ELF sections.
     * Some of them may be present in ELF libgcc.a.
     */
    /DISCARD/ :
    {
        *(.comment)
        *(.debug*)
    }
}

#ifdef ELF_TOOLCHAIN
/* Fix leading-underscore issues */
#include "tools/elfalias.ld"
#endif
