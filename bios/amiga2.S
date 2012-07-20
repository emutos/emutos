/*
 * amiga2.S - Amiga specific assembler functions
 *
 * Copyright (c) 2012 EmuTOS development team
 * Some parts Copyright (c) 1995-2007, The AROS Development Team.
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#ifdef MACHINE_AMIGA

/* Startup */

        .globl  amiga_startup
amiga_startup:
        move.l  #0x020000,D0
delay:  subq.l  #1,D0                   // Delay loop.
        jbgt    delay

        // Set up port A on the first CIA (8520-A).

        move.b  #3,0xbfe201             // Set low two bits for output.
        move.b  #2,0xbfe001             // Set boot ROM off, power light dim.

        // Disable interrupts and DMA.

        lea     0xdff000,a0             // Base address of custom chip area.
        move.w  #0x7fff,d0
        move.w  d0,0x9a(a0)             // Disable all interrupts.
        move.w  d0,0x9c(a0)             // Clear all pending interrupts.
        move.w  d0,0x96(a0)             // Disable all DMA.

        jmp     (a6)                    // Return

/* memconf */

        .globl amiga_memconf
amiga_memconf:
        lea     0,a0                    // Start looking at location 0.
        lea     0x800000,a1             // Don't look past 8 megabytes.

checkmem:
        // On entry, A0 is the lower bound of the area to check, and A1 is
        // the high bound.  Memory is checked in 4K blocks.

        moveq   #0,d1
        move.l  d1,(a0)                 // Write a zero to the first location.
        move.l  a0,a2                   // Save the first location.
        move.l  #0xf2d4b698,d0          // Use this as a signature value.

        // Main loop:  We enter here to check each 4K block.

checkmore:
        lea     0x1000(a0),a0           // Increment current location by 4K.
        cmp.l   a0,a1                   // See if upper bound reached.
        jbls   exitcheck                // If so, exit from the loop.
        move.l  d0,(a0)                 // Write the signature into memory.

        // Longword 0 of the block being checked was initially cleared to
        // zero.  If it is now no longer zero, we have "wrapped around",
        // i.e. due to incomplete address decoding, we have written the
        // signature value at the beginning of the block.  When this
        // occurs, we have reached the end of memory, even though the
        // signature value would read back correctly.

        tst.l   (a2)                    // Check location 0.
        jbne    exitcheck               // Exit if signature appears there.
        cmp.l   (a0),d0                 // See if signature can be read back.
        jbeq    checkmore               // If successful, go check more memory.

exitcheck:
        move.l  a0,d5                   // this is phystop - the end of RAM
        //move.l  #512*1024,d5            // this is phystop - the end of RAM
        moveq   #0,d6                   // fake memctrl

        jmp     (a6)                    // Return

/* Keyboard and 200 Hz timer interrupt intialization */

        .globl _amiga_init_keyboard_interrupt
_amiga_init_keyboard_interrupt:
        move.l  #_amiga_int_2,0x68.w

        // Disable CIAA
        move.b  #0,0xbfee01             // CIAA control register A (serial)
        move.b  #0,0xbfef01             // CIAA control register B (Timer 200 Hz)

        // CIAA Timer B: 200 Hz system timer, value = 3547
        move.b  #0x0d,0xbfe701
        move.b  #0xdb,0xbfe601

        // CIAA control register
        move.b  #0b00000001,0xbfef01    // CIAA control register B (Timer 200 Hz)

        // CIAA interrupt control register
        move.b  #0b01111111,0xbfed01    // CIAA interrupt control register: Clear all 8520 interrupts
        move.b  #0b10001010,0xbfed01    // Enable serial port interrupt + Timer B
        move.w  #0xc008,0xdff09a        // INTENA CIAA

        rts

/* Interrupt 2 */

        .globl _amiga_int_2
_amiga_int_2:
        //move.w  #0x0f00,0xdff180        // Background color = red
        movem.l d0-d2/a0-a1,-(sp)

        move.w  0xdff01e,d0             // intreq
        btst    #3,d0                   // interrupt CIA ?
        jeq     amiga_int_2_end

        move.b  0xbfed01,d2             // CIA A ? interrupt control register (DATA)
        jpl     amiga_ciaa_end

        jbsr    amiga_int_ciaa

amiga_ciaa_end:
        move.w  #0x0008,0xdff09c        // Intreq = interrupt processed.
        move.w  #0x0008,0xdff09c        // twice for compatibility

amiga_int_2_end:
        movem.l (sp)+,d0-d2/a0-a1
        rte

/* CIA A interrupt */

amiga_int_ciaa:
        btst    #3,d2
        jbeq    tst_ciaa_timerb

        jbsr    _amiga_int_ciaa_serial

tst_ciaa_timerb:
        btst    #1,d2
        jbeq    amiga_int_ciaa_end

        jbsr    _amiga_int_ciaa_timer_b

amiga_int_ciaa_end:
        rts

/* CIA A Serial interrupt (from keyboard) */

_amiga_int_ciaa_serial:
        moveq   #0,d0
        move.b  0xbfec01,d0             // Keypress + acknowledge
        not.b   d0
        ror.b   #1,d0                   // d0 now contains the raw key

        // Begin acknowledge on CNT pin
        ori.b   #0b01000000,0xbfee01    // SPMODE = 1

        // Start the CIAA Timer A in one shot mode
        move.b  #0b00001000,0xbfee01    // CIAA control register A (Timer A)

        // The timer will start after writing the high byte
        // CIAA Timer A: keyboard acknowledge timer, value = 63 (85 ms)
        move.b  #0x3d,0xbfe401          // Low byte
        move.b  #0x00,0xbfe501          // High byte

        // Wait for timer expired
wait_kbd_ack:
        btst.b  #0,0xbfed01             // Wait for CIAA A Timer A interrupt flag
        jbeq    wait_kbd_ack

        // End acknowledge on CNT pin
        andi.b  #~0b01000000,0xbfee01   // SPMODE = 0

        move.b  d0,d1
        andi.b  #0x80,d0
        andi.w  #0x007f,d1
        lea     _scancode_atari_from_amiga,a0
        or.b    0(a0,d1.w),d0

        move.l  _kbdvecs-4,a0           // kbdvec
        jsr     (a0)
        rts

/* CIA A Timer B interrupt (for 200 Hz system timer) */

_amiga_int_ciaa_timer_b:
        .extern _int_timerc
        jbsr    _int_timerc             // Call the standard Timer C handler
        rts

/* VBL interrupt */

        .extern _copper_list

        .globl  _amiga_vbl
_amiga_vbl:
        //move.w  #0x00f0,0xdff180        // Background color = green
        movem.l d0-d1/a0-a1,-(sp)

        move.w  0xdff01e,d0             // intreq
        btst    #5,d0                   // interrupt vbl ?
        jbeq    amivblend

        move.l  _v_bas_ad,d0            // Video Base address
        tst.w   0xdff004                // Test bit 15 = LOF bit
        jpl     vl1                     // LOF = 0, even lines, no change

        // Odd field: start at the second line
        add.l   #80,d0

vl1:
        move.w  d0,_copper_list+6       // BPL1PTL
        swap    d0
        move.w  d0,_copper_list+2       // BPL1PTH

        jbsr    _int_vbl                // Call the standard VBL handler

        .extern _amiga_mouse_vbl
        jbsr    _amiga_mouse_vbl        // Poll the mouse

        move.w  #0x0020,0xdff09c        // Intreq = interrupt processed.
        move.w  #0x0020,0xdff09c        // Twice for compatibility

amivblend:
        movem.l (sp)+,d0-d1/a0-a1
        rte

/* Call mousevec from C */

        .globl  _call_mousevec
_call_mousevec:
        move.l  4(sp),a0
        move.l  _kbdvecs+16,a1          // mousevec
        movem.l d2-d7/a2-a6,-(sp)
        jsr     (a1)
        movem.l (sp)+,d2-d7/a2-a6
        rts

/******************************************************************************/
/* Memory Test stuff                                                          */
/******************************************************************************/

// The following code comes from AROS sources.
// It is covered by the AROS PUBLIC LICENSE (APL) Version 1.1

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
