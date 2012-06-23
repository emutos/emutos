/*
 * memory.S - memory initialization
 *
 * Copyright (c) 2001-2011 by Authors:
 *
 *  MAD  Martin Doering
 *  PES  Petr Stehlik
 *  jfdn Jean-François DEL NERO
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"

#define DETECT_MMU (CONF_WITH_FALCON_MMU + CONF_WITH_ST_MMU > 1)

// ==== References ===========================================================

        .globl  meminit                 // memory initialization

// ==== startup.s - variables for memory  ====================================

        .extern memdone       // return to, if memory config done

// ==== detect.s - function for detecting CPU type and for testing memory  ===

        .extern _check_read_byte
        .extern _detect_cpu

// ==== tosvars.s - TOS System variables =====================================

        .extern memctrl
        .extern _phystop

        .extern memvalid
        .extern memval2
        .extern memval3

        .extern _ramtop
        .extern _ramvalid

// ==== Overall memory map ===================================================

        .extern bssstart      // end of documented low memory tosvars
        .extern _end          // end of bss + comm sections (ld generated)
        .extern _os_end       // first byte RAM not used by OS

// ===========================================================================
// ==== TEXT segment (TOS image) =============================================
// ===========================================================================

        .text

/*
 * meminit - Clear BSS + COMM before calling any C function
 *
 * The C part expects the bss to be cleared. Also we want to save our
 * configuration into it, so let's do this early.
 */

meminit:
        lea     bssstart, a0
        lea     _end-1, a1
        move.l  a1, d0
        sub.l   a0, d0
        lsr.l   #2, d0
clearbss:
        clr.l   (a0)+
#ifdef __mcoldfire__
        subq.l  #1, d0
        bpl.s   clearbss
#else
        dbra    d0, clearbss
#endif

/*
 * The caller (either startup.S or comprimg.S) needs to have initialized sp
 * with a temporary supervisor stack (jsr _check_read_byte/_detect_cpu)
 * The stack will be overwritten and lost during clearing of memory
 */

#if DETECT_MMU
        /* detect if this is a Falcon by reading $ff8282 register */
        pea     0xffff8282
        jsr     _check_read_byte
        addq.l  #4, sp

        tst.w   d0                      // is it Falcon MMU?
        beq     memchk                  // no -> skip 0xffff8007 test
#endif /* DETECT_MMU */

#if CONF_WITH_FALCON_MMU
        /* Initialize ant test Falcon MMU ??? */
#ifdef __mcoldfire__
        lea     0xffff8007, a0
        bset    #6, (a0)                // seen in TOS 4.04
#else
        bset    #6, 0xffff8007          // seen in TOS 4.04
#endif
        beq     memconf                 // not sure what 6th bit means
#endif /* CONF_WITH_FALCON_MMU */

/*
 * memchk - Check, if old memory config can be used
 */
 
memchk:
#ifdef __mcoldfire__
        move.l  #0x752019f3, d1
        cmp.l   memvalid, d1            // magic in memvalid ?
        bne     memconf                 // no -> config memory
        move.l  #0x237698aa, d1
        cmp.l   memval2, d1             // magic in memval2 ?
        bne     memconf                 // no -> config memory
        move.l  #0x5555aaaa, d1
        cmp.l   memval3, d1             // magic in memval3 ?
        bne     memconf                 // no -> config memory
#else
        cmp.l   #0x752019f3, memvalid   // magic in memvalid ?
        bne     memconf                 // no -> config memory
        cmp.l   #0x237698aa, memval2    // magic in memval2 ?
        bne     memconf                 // no -> config memory
        cmp.l   #0x5555aaaa, memval3    // magic in memval3 ?
        bne     memconf                 // no -> config memory
#endif

memok:
#if DETECT_MMU
        tst.w   d0                      // is it Falcon MMU?
        bne     no_memctrl              // yes -> skip memctrl init
#endif

#if CONF_WITH_ST_MMU
#ifdef __mcoldfire__
        lea     0xffff8001, a0
        move.b  memctrl, (a0)           // write old valid config to controller
#else
        move.b  memctrl, 0xffff8001     // write old valid config to controller
#endif
                                        // phystop should then also be ok
#endif /* CONF_WITH_ST_MMU */
no_memctrl:
        jmp     memdone                 // config ok -> memdone in startup.s

/*
 * memconf - Configure the memory and clear it
 *
 * We first test if EmuTOS runs on a Falcon by reading a Falcon
 * hardware register. If so, we can use the Falcon register $ff8006 for
 * memory setup. Note that we can't use $ff8006 to do the Falcon test
 * since the STE does not generate a bus error when reading from this
 * register, however it does not really have the $ff8006 register :-(
 */
memconf:
#if CONF_STRAM_SIZE

        move.l  #CONF_STRAM_SIZE,d5     // this is phystop - the end of RAM
        moveq   #0,d6                   // fake memctrl

        // Fall into stram_detected

#else /* CONF_STRAM_SIZE */

#if DETECT_MMU
        tst.w   d0                      // is it Falcon MMU?
        bne     falcmemconf             // yes -> jump to Falcon mem routine
#endif

#if CONF_WITH_ST_MMU

/*
 * memconf for ST
 *
 * At first the memory configuration is set to 4 Mb. If the memory
 * configuration is not really 2 * 2 mb, the written test values will
 * repeat in memory. The trick is to find these repeated values and
 * set the memory config accordingly. No real memory check is done, 
 * because in emulators it should be always ok - hopefully!
 *
 * 24/05/10 - jfdn
 * Note: To be reliable on the real hardware, the pattern test algorithm
 * is now the same used in the TOS 1.6 ROM.
 * Using of only 1 word pattern test is not reliable since without SIMM
 * the memory bus are floating and may present the good pattern value.
 * That's why a 200 bytes pattern test are used in the TOS 1.6 ROM to avoid this issue.
 */
        suba.l  a0,a0                   // Note:The TOS 1.6 use these opcode probably to access to the MMU register more slowly than the classic "move.b  #0xa,0xffff8001"  
        move.b  #0xa,0xffff8001(a0)        // set hw to 2 banks by 2 mb (reg 0xffff8001 - memorycontroller)

/* init the test pattern in bank 0 and 1 */
        movea.w #0x000008,a0            // bank 0
        lea     0x200008,a1             // bank 1
        moveq   #0,d0
init_pattern_loop:
        move.w  d0,(a0)+                // bank 0
        move.w  d0,(a1)+                // bank 1
#ifdef __mcoldfire__
        add.l   #0xfa54,d0              // next pattern data
#else
        add.w   #0xfa54,d0              // next pattern data
#endif
        cmpa.l  #0x200,a0               // 200 word done?
        bne.s   init_pattern_loop

/* In this part:
 * D4 = Actual bank memory size in byte
 * D5 = total memory size (later phystop)
 * D6 = value to the MMU config register (reg 0xffff8001) at the end
 * A0 = pattern test offset (0x8,0x40008,0x80008)
 * D1 = pattern test bank base (0x200000 ,0x000000)
 * A5 = pattern test subroutine return address.
 */
        move.l  #0x200000,d1            // we begin with the bank 1
        clr.l   d5                      // total memory (later phystop)
        clr.l   d6                      // MMU setting set to 0

        /* test the presence of a 256k bank config */
test_256k:
#ifdef __mcoldfire__
        lsr.l   #2,d6                   // switch bank in configuration byte
#else
        lsr.w   #2,d6                   // switch bank in configuration byte
#endif
        clr.l   d4                      // bank memory size

        movea.l #0x40008,a0             // 256k test
        lea     test_512k,a5            // return point of test pattern
        move.l  #0x40000,d4             // 256 kb bank memory size
        bra     check_test_pattern

        /* test the presence of a 512k bank config */
test_512k:
        beq.s   bank_test_end           // There are a memory loop in the pevious test ! -> D6=0x0 - 256k bank

        movea.l #0x80008,a0             // 512k test
        lea     test_2m,a5              // return point of test pattern
        move.l  #0x80000,d4             // 512 kb bank memory size
        bra     check_test_pattern

        /* test the presence of a 2M bank config */
test_2m:
        beq.s   bank_test_end_512k      // There are a memory loop in the pevious test -> D6=0x4 - 512K bank

        /* This the last test. in previous tests (256k&512k) we didn't found any memory loop.
           Here we are testing if there a 2Mb simm (full back memory size) or nothing. */

        movea.l #0x00008,a0             // 0kb/2m test
        lea     last_return,a5          // return point of test pattern
        move.l  #0x0,d4                 // 0kb/2Mb bank memory size
        bra     check_test_pattern

last_return:
        bne.s   bank_test_end           // Pattern not found!  -> D6=0x0 - NO RAM
        move.l  #0x200000,d4            // Pattern found!      -> D6=0x6 - 2m

#ifdef __mcoldfire__
        addq.l  #4,d6
#else
        addq.w  #4,d6
#endif

bank_test_end_512k:
#ifdef __mcoldfire__
        addq.l  #4,d6
#else
        addq.w  #4,d6
#endif

bank_test_end:
        add.l   d4,d5                   // compute total memory size
        sub.l   #0x200000,d1            // next bank 0
        beq.s   test_256k               // already done ?   -> no start the tests for the bank 0
                                        // in the case this test is the end.

        // Set the final MMU config
        suba.l  a0,a0                   // Note:The TOS 1.6 use these opcode probably to access to the MMU register more slowly than the classic "move.b  d6,0xffff8001"
        move.b  d6,0xffff8001(a0)       // set hw memory config (reg 0xffff8001 - memorycontroller)

        bra     detect_extra_stram

/*
 * check_test_pattern "subroutine":
 * D1= bank base   (Bank0: 0x000000 Bank1: 0x200000)
 * A0= test offset (0x8,0x40008,0x80008)
 * A5= return point
 * if Z=1 -> pattern found !
 * if Z=0 -> pattern not found !
 */
check_test_pattern:
        adda.l  d1,a0                   // compute starting address
        moveq   #0,d0                   // first pattern word
        lea     0x1f8(a0),a1            // end address
check_test_pattern_loop:
        cmp.w   (a0)+,d0                // is the data valid ?
        bne     end_check_test_pattern
#ifdef __mcoldfire__
        add.l   #0xfa54,d0              // next pattern word to test
#else
        add.w   #0xfa54,d0              // next pattern word to test
#endif
        cmpa.l  a0,a1                   // at the end of the pattern ?
        bne.s   check_test_pattern_loop
end_check_test_pattern:
        jmp (a5)                        // return

detect_extra_stram:
        /* Now detect if there is some extra ST-RAM installed */
        cmpi.l  #4*1024*1024,d5         // we should already have 4 MB
        bne.s   detect_extra_stram_end

        lea     5*1024*1024-1,a0        // start at the end of 5 MB
loop_extra_stram:
        pea     (a0)
        jsr     _check_read_byte        // test the last byte
        addq.l  #4,sp
        tst.w   d0                      // readable ?
        beq.s   detect_extra_stram_end  // no, stop here

        move.l  #1*1024*1024,d0         // one more megabyte
        add.l   d0,d5                   // to the current amount of ram

        cmp.l   #14*1024*1024,d5        // check for 14 MB maximum
        bhs.s   detect_extra_stram_end

        add.l   d0,a0                   // and to next address to check
        bra.s   loop_extra_stram
detect_extra_stram_end:

        /* Some emulators do not emulator the behaviour of memory banks
         * so we end up here with a bad value in d5 ... let's do a
         * last sanity check whether the memory is really usable: */
        move.l  #0xff0055aa,d0          // Test pattern
mem_sanity_loop:
        move.l  d5,a0
        subq.l  #4,a0
        move.l  d0,(a0)                 // Write to memory
        cmp.l   (a0),d0                 // Was memory really writable?
        beq.s   mem_sanity_done
        lsr.l   #1,d5                   // Decrease memory size
        bra.s   mem_sanity_loop
mem_sanity_done:
        bra     stram_detected

#endif /* CONF_WITH_ST_MMU */

#if CONF_WITH_FALCON_MMU

/*
 * memconf for Falcon
 */
falcmemconf:
        /* find out the memory size by checking the d0 = $ff8006 value */
#ifdef __mcoldfire__
        moveq   #0, d0
        move.w  0xffff8006.w, d0        // read the Falcon MMU register
        lsr.l   #8,d0
        lsr.l   #1,d0
        move.l  d0,d1
        and.l   #1,d1
        lsr.l   #2,d0
        and.l   #6,d0
        or.l    d0,d1
#else
        move.w  0xffff8006.w, d0        // read the Falcon MMU register
        lsr.w   #8,d0
        lsr.b   #1,d0
        move.b  d0,d1
        and.b   #1,d1
        lsr.b   #2,d0
        and.b   #6,d0
        or.b    d0,d1
#endif
        move.l  #0x00080000,d0
        asl.l   d1,d0
        cmp.b   #5,d1
        bne.s   less16
        move.l  #0x00E00000,d0
less16: 
        move.l  d0, d5                  // this is phystop - the end of RAM
        moveq   #0, d6                  // fake memctrl with Falcon MMU

        // Fall into stram_detected

#endif /* CONF_WITH_FALCON_MMU */

#ifdef MACHINE_AMIGA

/*
 * memconf for Amiga
 */
        lea     amiga_memconf_ret(pc),a6
        jmp     amiga_memconf
amiga_memconf_ret:

#endif /* MACHINE_AMIGA */

#endif /* CONF_STRAM_SIZE */

/*
 * At this point:
 * - The size of the ST-RAM has been detected
 * - The MMU has been properly initalized
 * - The ST-RAM is ready to be used
 * 
 * d5 = phystop
 * d6 = memctrl
 */
stram_detected:
        /* clear ST-RAM till end (phystop in d5) */
        lea     st_cleared(pc),a6       // remember return address
        bra     clear_st_ram            // jump to routine clearing the RAM
st_cleared:

        /* Store the ST-RAM parameters in the ST-RAM itself */
        move.b  d6, memctrl             // set copy of hw memory config
        move.l  d5, _phystop            // highest address as phystop

#if CONF_WITH_FASTRAM

/*
 * TT-RAM (FastRAM) detection (PES)
 * This alternate RAM begins at 16MB boundary ($1000000)
 */
#if CONF_FASTRAM_SIZE
        move.l  #0x1000000+CONF_FASTRAM_SIZE,a3    // fixed ramtop
#else
        /* clear MB counter */
        moveq   #0, d6

#ifndef __mcoldfire__
        /* test CPU - FastRAM is possible on 68020+ (32-bit address space) */
        jsr     _detect_cpu
        cmp.w   #20,d0                  // 68020 or better?
        blt     setramtop               // no -> skip the whole FastRAM stuff
#endif

        /* Can we access FastRAM region? */
        pea     0x1000400
        jsr     _check_read_byte
        addq.l  #4,sp
        tst.w   d0
        beq     setramtop

        /* Check wether we are really on a 32 bit machine or if we warp around: */
        lea     0x400.w,a0
        lea     0x1000400,a3
        move.l  #0x12345678,d0
        move.l  d0,(a0)
        clr.l   (a3)
        cmp.l   (a0),d0                 // Value destroyed => Warp around?
        bne     setramtop               // yes, skip fast ram check
        clr.l   (a0)

        /* No warp around detected, let's scan for FastRAM: */
        lea     0x1000000, a3           // start from 16MB boundary
        move.l  #1024*1024, d3          // test by blocks of 1MB size
loop:
        lea     -1(a3,d3.l), a0
        pea     (a0)
        jsr     _check_read_byte
        addq.l  #4,sp
        tst.w   d0
        beq.s   setramtop
        addq.l  #1, d6
        add.l   d3, a3
        cmp.l   #2048, d6                // 2048 MB of FastRAM should be enough
        blt.s   loop

setramtop:
        /* test MB counter */
        tst.l   d6
        bne.s   alter
        sub.l   a3, a3
alter:
#endif /* CONF_FASTRAM_SIZE */
        move.l  a3, _ramtop             // set ramtop to size of found memory
#ifdef __mcoldfire__
        lea     _ramvalid, a4
        move.l  #0x1357BD13, (a4)       // set ramvalid to ok
#else
        move.l  #0x1357BD13, _ramvalid  // set ramvalid to ok
#endif

        /* clear all Fast/TT-RAM - from 0x1000000 till the end */
        lea     0x1000000, a4           // start from 16MB boundary
        move.l  a3, d5                  // end at ramtop
        beq     tt_cleared              // if no FastRAM then skip clearing
        lea     tt_cleared(pc),a6       // remember return address
        bra     clear_ram               // jump to routine clearing the RAM
tt_cleared:

#endif /* CONF_WITH_FASTRAM */

        jmp     memdone                 // config done -> return to startup.s

// ===========================================================================
// ==== Below are utility functions ==========================================
// ===========================================================================

// ==== Clear the ST-RAM =====================================================
// d5 = end of the ST-RAM
// a6 = return address
// d0-d4/a0-a5 = clobbered
clear_st_ram:
#ifdef EMUTOS_RAM
        // When EmuTOS lives in the RAM, it must not be cleared!
        move.l  _os_end, d0             // end of EmuTOS in RAM
        addi.l  #0xff, d0               // round to next multiple of 256
        andi.l  #0xffffff00, d0         // for clear_ram
        // BUG: This is not strictly correct. A few bytes will not be cleared
        // at the beginning of the free ram.
        move.l  d0,a4                   // begin of RAM to clear
#else
        lea     0x0400, a4              // begin of RAM to clear
#endif
        // Fall into clear_ram

// ==== Clear RAM from a4 to d5 and return to (a6) ===========================
// Restriction: the amount of ram to clear must be a multiple of 256
/*
 * a4 = begin of RAM to clear
 * d5 = end of RAM to clear
 * a6 = return address
 * d0-d4/a0-a5 = clobbered
 */
clear_ram:
        move.l  d5, d4
        sub.l   a4, d4
        lsr.l   #8, d4
        subq.l  #1, d4
#ifdef __mcoldfire__
        lea     0x0100, a5
#else
        lea     0x0200, a5
#endif
        moveq   #0x00, d0
        move.l  d0, a0
        moveq   #0x00, d1
        move.l  d1, a1
        moveq   #0x00, d2
        move.l  d2, a2
        moveq   #0x00, d3
        move.l  d3, a3
        lea     0x0100(a4), a4
clear_loop:
#ifdef __mcoldfire__
        movem.l d0-d3/a0-a3, -32(a4)
        movem.l d0-d3/a0-a3, -64(a4)
        movem.l d0-d3/a0-a3, -96(a4)
        movem.l d0-d3/a0-a3, -128(a4)
        movem.l d0-d3/a0-a3, -160(a4)
        movem.l d0-d3/a0-a3, -192(a4)
        movem.l d0-d3/a0-a3, -224(a4)
        movem.l d0-d3/a0-a3, -256(a4)
        adda.l  a5, a4
        subq.l  #1, d4
        bpl.s   clear_loop
#else
        movem.l d0-d3/a0-a3, -(a4)
        movem.l d0-d3/a0-a3, -(a4)
        movem.l d0-d3/a0-a3, -(a4)
        movem.l d0-d3/a0-a3, -(a4)
        movem.l d0-d3/a0-a3, -(a4)
        movem.l d0-d3/a0-a3, -(a4)
        movem.l d0-d3/a0-a3, -(a4)
        movem.l d0-d3/a0-a3, -(a4)
        adda.l  a5, a4
        dbf     d4, clear_loop
#endif
        jmp     (a6)

// ===========================================================================
// ==== End ==================================================================
// ===========================================================================
