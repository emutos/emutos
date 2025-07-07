     .syntax unified
     .cpu arm1176jzf-s
     .section .text
     .global _start
     .extern _rpiboot
_start:
     /* set up the stack pointer */
     ldr   sp, =_stack_top

     /* zero out .bss */
     ldr   r0, =__bss_start__
     ldr   r1, =__bss_end__
     mov   r2, #0
1:   cmp   r0, r1
     it    lt
     strlt r2, [r0], #4
     blt   1b

     /* jump into the C entrypoint */
     bl    _rpiboot

     /* if rpiboot() ever returns, spin */
     b     .