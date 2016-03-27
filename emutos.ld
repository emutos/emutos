/*
 * emutos.ld - Custom linker script for EmuTOS ROM
 *
 * Copyright (c) 2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* Create a plain binary ROM, without any header */
OUTPUT_FORMAT(binary)

SECTIONS
{
  /* The TEXT segment is mapped at the beginning of the ROM.
   * The starting address will be specified on the linker command-line
   * with -Ttext=...
   */
  .text :
  {
    CREATE_OBJECT_SYMBOLS
    *(.text)
    *(.rodata .rodata.*)
    __etext = .;
  }

  /* FIXME: Our DATA segment is currently read-only.
   * It currently lives in the ROM, just after the TEXT segment.
   * This means that initialized global variables can't be modified.
   * Since a read-only DATA segment is useless, we try to keep it empty by
   * making all initialized variables const, so they go to the TEXT segment.
   */
  .data :
  {
    *(.data)
    __edata = .;
  }

  /* The BSS segment starts at the beginning of the RAM.
   * It includes the processor Exception Vector Table,
   * system variables, and private variables.
   */
  .bss 0x00000000:
  {
    *(.bss COMMON)
    __end = .;
  }

  /* Discard the following ELF sections.
   * Some of them may be present in ELF libgcc.a.
   */
  /DISCARD/ :
  {
    *(.comment)
    *(.debug*)
  }
}
