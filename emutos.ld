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

/* Create a plain binary ROM, without any header */
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
 * Only start addresses matter.
 * Lengths just need to be large enough.
 */
MEMORY
{
  rom : ORIGIN = ROM_ORIGIN, LENGTH = ROM_LENGTH
  sysvars : ORIGIN = 0x00000000, LENGTH = sysvars_end
  stram : ORIGIN = LENGTH(sysvars), LENGTH = 512K - LENGTH(sysvars)
  fastram : ORIGIN = 0x01000000, LENGTH = 1M
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
  /* The TEXT segment is mapped at the beginning of the ROM.
   * The starting address will be specified on the linker command-line
   * with -Ttext=...
   */
  .text :
  {
    CREATE_OBJECT_SYMBOLS
    __text = .;
    *(.text)
    *(.rodata .rodata.*)
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

  /* By default, the BSS segment starts just after the system variables,
   * at the bottom of the ST-RAM.
   */
  .bss :
  {
    __bss = .;
    *(.bss COMMON)
    __ebss = .;
  } >REGION_READ_WRITE

  /* Discard the following ELF sections.
   * Some of them may be present in ELF libgcc.a.
   */
  /DISCARD/ :
  {
    *(.comment)
    *(.debug*)
  }

  /* Determine the last ST-RAM address used by the OS TEXT and BSS.
   * We deliberately avoid to assign the same symbol several times here
   * to avoid multiple matches with FUNCTION_SHELL_GET_SYMBOL_ADDRESS.
   * We take the maximum of sysvars_end, __edata and __ebss,
   * while staying in ST-RAM.
   */
  __end_os_stram_candidate_1 = sysvars_end;
  __end_os_stram_candidate_2 = (__edata > __end_os_stram_candidate_1 && __edata < 0x00e00000) ? __edata : __end_os_stram_candidate_1;
  __end_os_stram = (__ebss > __end_os_stram_candidate_2 && __ebss < 0x00e00000) ? __ebss : __end_os_stram_candidate_2;
}
