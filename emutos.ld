/*
 * emutos.ld - Custom linker script for EmuTOS ROM
 *
 * Copyright (C) 2016 The EmuTOS development team
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

/*
 * The system variables have fixed addresses,
 * they live in the 0x380-0x800 region.
 */
#include "tosvars.ld"

/* The ROM address depends on the target */
#if defined (MACHINE_AMIGA)
# define ROM_ORIGIN 0x00fc0000
#elif defined (TARGET_CART)
# define ROM_ORIGIN 0x00fa0000
#elif defined (TARGET_192)
# define ROM_ORIGIN 0x00fc0000
#else
# define ROM_ORIGIN 0x00e00000
#endif

/* Always assume there is a lot of ROM space, so the link will always succeed.
 * Then our padding tool will eventually report an overflow, with a better
 * diagnostic message than the linker.
 */
# define ROM_LENGTH 1M /* Maximum size before I/O area */

/* Memory regions of a typical Atari computer.
 * Only start addresses matter. Lengths just need to be large enough.
 */
MEMORY
{
    rom : ORIGIN = ROM_ORIGIN, LENGTH = ROM_LENGTH
    sysvars : ORIGIN = 0x00000000, LENGTH = 0x800
    stram : ORIGIN = LENGTH(sysvars), LENGTH = 1M - LENGTH(sysvars)
#ifdef STATIC_ALT_RAM_ADDRESS
    altram : ORIGIN = STATIC_ALT_RAM_ADDRESS, LENGTH = 1M
#endif
}

/* Global symbols */
sysvars_end = ORIGIN(sysvars) + LENGTH(sysvars);

/* Region used as read/write memory */
#if CONF_WITH_STATIC_ALT_RAM
# define REGION_RAM altram
#else
# define REGION_RAM stram
#endif

/* Regions used by EmuTOS sections */
#if EMUTOS_LIVES_IN_RAM
# define REGION_READ_ONLY   REGION_RAM
# define REGION_READ_WRITE  REGION_RAM
#else
# define REGION_READ_ONLY   rom
# define REGION_READ_WRITE  REGION_RAM
#endif

SECTIONS
{
    /* This section is located as low as possible in ST-RAM.
     * Variables requiring very low addresses, while being accessible
     * from user mode, can be put here.
     * Warning: this area is not cleared at startup.
     */
    .lowstram :
    {
        __low_stram_start = .;

        /* Workaround for bug in GFA Basic 3.51 startup.
         * Theoretically, we are allowed to put the shifty variable anywhere,
         * then programs may access it through the pointer present in
         * OSHEADER. However, by mistake the GFA only reads the low word of
         * that pointer, and sign-extends it to get the whole address.
         * Consequently, this can only be correct if shifty is located in the
         * first 32K of memory.
         * TOS puts this variable in a region accessible in user mode, so we
         * do.
         */
        _shifty = .; /* reflects the status up/down of mode keys */
        . += 1; /* UBYTE */
        ASSERT(ABSOLUTE(_shifty) < 0x8000, "error: bad _shifty address");

        /* Be sure to align this section size */
        . = ALIGN(4);

        __low_stram_end = .;
    } >stram

#if CONF_WITH_STATIC_ALT_RAM
    /* First section in static Alt-RAM */
    .first_altram :
    {
        __static_altram_start = .;
    } >altram
#endif

    /* The TEXT segment contains EmuTOS executable code and read-only data.
     * It is always read-only, so it can safely live in ROM.
     */
    .text :
    {
        CREATE_OBJECT_SYMBOLS
        __text = .;
        *(.text)
        *(.rodata .rodata.*) /* Only present in ELF objects */
        __etext = .;
    } >REGION_READ_ONLY

    /* FIXME: Our DATA segment is currently read-only.
     * It currently lives in the ROM, just after the TEXT segment.
     * This means that initialized global variables can't be modified.
     * Since a read-only DATA segment is useless, we try to keep it empty by
     * making all initialized variables const, so they go to the TEXT segment.
     */
    .data :
    {
        __data = .;
        *(.data)
        __edata = .;
    } >REGION_READ_ONLY

    /* This section is the internal BIOS stack.
     * It will *not* be cleared on startup or reset.
     */
    .stack :
    {
        _stkbot = .;
        . += 2K;
        _stktop = .;
    } >REGION_READ_WRITE

    /* The BSS segment contains uninitialized global variables.
     * It will be cleared by the startup code.
     */
    .bss :
    {
        __bss = .;
        *(.bss COMMON)
        __ebss = .;
    } >REGION_READ_WRITE

    /* This section is always the last one stored in ST-RAM.
     * It is usually empty, just used to calculate the last address
     * of ST-RAM statically used by EmuTOS.
     */
    .laststram :
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
/* Our code is compiled with -fleading-underscore, so the references to external
 * libraries will include a leading underscore. However, libgcc was not compiled
 * with that option, so its symbols do not start with a leading underscore. So
 * we need to define leading-underscore aliases to the no-leading-underscore
 * functions in libgcc.
 */
#define DEFINE_SYMBOL_ALIAS(alias, impl) EXTERN(impl) PROVIDE(alias = impl);
#define ELF_LIB_REF(symbol) DEFINE_SYMBOL_ALIAS(_##symbol, symbol)
ELF_LIB_REF(__mulsi3)
ELF_LIB_REF(__divsi3)
ELF_LIB_REF(__modsi3)
ELF_LIB_REF(__udivsi3)
ELF_LIB_REF(__umodsi3)
#endif
