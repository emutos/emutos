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
# define ROM_LENGTH 256K
#elif defined (TARGET_CART)
# define ROM_ORIGIN 0x00fa0000
# define ROM_LENGTH 128K
#elif defined (TARGET_192)
# define ROM_ORIGIN 0x00fc0000
# define ROM_LENGTH 192K
#else
# define ROM_ORIGIN 0x00e00000
# define ROM_LENGTH 1M /* Maximum size before I/O area */
#endif

/* Memory regions of a typical Atari computer.
 * Only start addresses matter. Lengths just need to be large enough.
 */
MEMORY
{
  rom : ORIGIN = ROM_ORIGIN, LENGTH = ROM_LENGTH
  sysvars : ORIGIN = 0x00000000, LENGTH = sysvars_end
  stram : ORIGIN = LENGTH(sysvars), LENGTH = 512K - LENGTH(sysvars)
  ttram : ORIGIN = 0x01000000, LENGTH = 1M
}

/* Region used as read/write memory */
#define REGION_RAM stram

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
  } >stram

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

  /* Discard the following ELF sections.
   * Some of them may be present in ELF libgcc.a.
   */
  /DISCARD/ :
  {
    *(.comment)
    *(.debug*)
  }
}
