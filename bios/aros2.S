/*
 * aros.c - Amiga assembler functions from AROS
 *
 * Copyright (c) 1995-2013, The AROS Development Team. All rights reserved.
 *
 * The functions in this file were borrowed from the AROS sources.
 * Glued to EmuTOS by:
 *  VRI   Vincent Rivière
 * 
 * This file is covered by the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include "asmdefs.h"

#ifdef MACHINE_AMIGA

/******************************************************************************/
/* Memory Test                                                                */
/******************************************************************************/

/* From arch/m68k-amiga/boot/rom_init.S ***************************************/

    .globl _MemoryTest
_MemoryTest:
    // In EmuTOS, we get the parameters from the stack
    move.l  %sp@(4),%a0     // address
    move.l  %sp@(8),%a1     // endAddress
    move.l  %sp@(12),%d0    // blockSize

    movem.l %d2-%d5/%a2-%a3/%a5,%sp@-
    move.w  0xdff01c,%sp@-  /* save INTENA */
    subq.l  #8,%sp
    move.l  %sp,%a5
    bsr.s   .L_memtest
    addq.l  #8,%sp
    move.w  %sp@+,%d1
    or.w    #0x8000,%d1
    move.w  %d1,0xdff09a    /* restore INTENA */
    movem.l %sp@+,%d2-%d5/%a2-%a3/%a5
    rts

    /******** Memory tester *******/
     * IN: A0 - Address, A1 - Max end address, A5 - outtable
     * IN: D0 = block size
     * OUT: D0 - Detected size
     * TRASH: D1, D2, D3, D4, D5, A1, A2, A3
     */
.L_memtest:
    move.l  %d0,%d5
    move.l  %a0,%a5@            /* write start address */
    move.l  %a1,%d0
    sub.l   %a0,%d0             /* max size */

    move.l  %a0,%d1
    and.l   #0xff000000,%d1
    beq.s   .L_memtest_24bitaddr
    /* test if 32bit address mirrors address zero */
    move.l  %d1,%a1
    move.l  0.w,%d2             /* save old */
    move.l  0x100.w,%d3
    move.l  #0xfecaf00d,%d1
    move.l  %d1,0.w
    nop
    not.w   %d1
    move.l  %d1,0x100.w         /* write something else, some bus types "remember" old value */
    not.w   %d1
    nop                         /* force 68040/060 bus cycle to finish */
    cmp.l   %a1@,%d1
    bne.s   .L_memtest_32bitok  /* different? no mirror */
    move.l  #0xcafed00d,%d1
    move.l  %d1,0.w
    nop
    not.w   %d1
    move.l  %d1,0x100.w
    not.w   %d1
    nop
    cmp.l   %a1@,%d1
    bne.s   .L_memtest_32bitok  /* check again, maybe 0 already had our test value */
    move.l  %d2,0.w             /* restore saved value */
    move.l  %d3,0x100.w
    moveq   #-1,%d1
    bra     .L_memtest_none     /* 24-bit CPU, do not test this range */
.L_memtest_32bitok:
    move.l  %d2,0.w             /* restore saved value */
    move.l  %d3,0x100.w
.L_memtest_24bitaddr:

    /* a0 = tested address, d0 = max size, d1 = current size */

    clr.l   %d1
.L_memtest_loop:
    cmp.l   %d0,%d1
    bge     .L_memtest_done

    move.l  %a0,%d2
    and.l   #0xff000000,%d2
    bne.s   .L_memtest_chipcheck_done   /* no chiptest if 32bit address */
    move.w  #0x7fff,0xdff09a
    nop
    tst.w   %a0@(0x1c,%d1)              /* If non-zero, this is not INTENAR */
    bne.s   .L_memtest_chipcheck_done
    /* It was zero ... */
    move.w  #0xc000,0xdff09a            /* Try the master enable */
    nop
    tst.w   %a0@(0x1c,%d1)              /* If still zero, not INTENAR */
    bne     .L_memtest_done             /* It was a custom chip. */
.L_memtest_chipcheck_done:

    move.l  %a0,%a2
    add.l   %d1,%a2
    cmp.l   #.L_memtest,%a2     /* Make sure we don't modify our own test code */
    bcs.s   .L_memtest_nottestcode
    cmp.l   #.L_memtest_end,%a2
    bcs.s   .L_memtest_next
.L_memtest_nottestcode:

    move.l  %a0@(%d1),%d3       /* read old value */
    move.l  %a0@,%a2            /* save mirror test contents */
    move.l  #0xfecaf00d,%a0@    /* write mirror test value */
    nop
    move.l  #0xcafed00d,%d2
    move.l  %d2,%a0@(%d1)       /* write test pattern */
    nop
    tst.l   %d1                 /* first test addrress? */
    beq.s   .L_memtest_nomirror
    cmp.l   %a0@,%d2            /* no, check mirrorirng */
    bne.s   .L_memtest_nomirror
    move.l  %a2,%a0@            /* restore mirror test contents */
    bra.s   .L_memtest_done
.L_memtest_nomirror:

    not.l   %d2
    move.l  %a0@(4,%d1),%a3     /* read temp address */
    move.l  %d2,%a0@(4,%d1)     /* fill bus with something else */
    not.l   %d2
    nop
    move.l  %a0@(%d1),%d4       /* read test pattern */
    move.l  %a3,%a0@(4,%d1)     /* restore */

    cmp.l   %d4,%d2             /* pattern match? */
    bne.s   .L_memtest_done
    neg.l   %d2                 /* test pattern 2 */

    move.l  %d2,%a0@(%d1)       /* write test pattern */
    nop
    not.l   %d2
    move.l  %a0@(4,%d1),%a3     /* read temp address */
    move.l  %d2,%a0@(4,%d1)     /* fill bus with something else */
    not.l   %d2
    nop
    move.l  %a0@(%d1),%d4       /* read test pattern */
    move.l  %a3,%a0@(4,%d1)     /* restore */

    cmp.l   %d4,%d2
    bne.s   .L_memtest_done
    not.l   %d2
    move.l  %d3,%a0@(%d1)       /* write old value back */

    move.l  %a2,%a0@            /* restore mirror test contents */
.L_memtest_next:
    add.l   %d5,%d1             /* next block */
    bra     .L_memtest_loop

.L_memtest_done:
    tst.l   %d1                 /* nothing found? */
    beq.s   .L_memtest_none
    addq.l  #4,%a5
    move.l  %d1,%a5@+
.L_memtest_none:
    move.l  %d1,%d0
    rts
.L_memtest_end:

/******************************************************************************/
/* Gayle detection                                                            */
/******************************************************************************/

/* From arch/m68k-amiga/exec/readgayle.S **************************************/

        .globl  _ReadGayle
_ReadGayle:
    move.l  #0xde1000,%a0       /* Gayle ID register */
    move.l  #0xdff000,%a1
    /* NOTE: We must check for custom chip mirroring.
     */
    /* Can we turn off interrupts? */
    move.w  0x1c(%a1),%d0       /* INTENAR */
    move.w  #0x7fff,0x9a(%a0)   /* INTENA */
    tst.w   0x1c(%a1)
    bne.s   0f                  /* If non-zero, not INTENAR */
    move.w  #0x8001,0x9a(%a0)
    tst.w   0x1c(%a1)
    beq.s   0f                  /* If still zero, not INTENAR */
    /* Restore interrupts. */
    move.w  #0x7fff,0x9a(%a1)   /* INTENA */
    or.w    #0x8000,%d0
    move.w  %d0,0x9a(%a1)       /* INTENA */
    /* It was a custom chip mirror. Can't be Gayle*/
    clr.l   %d0
    bra.s   1f
0:

    tst.b   %a0@(-0x1000)       /* clear possible Fat Gary Timeout register
                                 * to prevent bogus 0x80 Gayle ID on an A3000 */

    /* Now, finally, we can check for a Gayle */
    clr.l   %d0                 /* ID we will return */
    moveq   #8-1,%d1            /* Gayle bit index */
    move.b  #0,%a0@             /* Reset ID register */
0:  move.b  %a0@,%d0            /* Put reg into lower byte of %d0 */
    lsl.w   #1,%d0              /* Shift lower 16 bits of %d0 left by 1 */
    dbf     %d1,0b
    lsr.w   #8,%d0              /* Move ID to lower 8 bits of %d0 */

    cmp.b   #0xff,%d0
    bne.s   1f
    clr.b   %d0
1:
    rts

#endif /* MACHINE_AMIGA */
