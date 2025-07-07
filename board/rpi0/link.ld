ENTRY(_start)
SECTIONS {
  . = 0x00008000;        /* QEMU’s default load address for –kernel */

  .text : { *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }

  .bss (NOLOAD) : {
    __bss_start__ = .;
    *(.bss*)
    __bss_end__   = .;
  }

  /* carve out top of RAM for the stack */
  . = 0x00100000;
  PROVIDE(_stack_top = .);
}