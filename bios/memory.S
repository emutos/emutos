/*
 * memory.S - memory initialization
 *
 * Copyright (c) 2001-2013 by Authors:
 *
 * Authors:
 *  MAD  Martin Doering
 *  PES  Petr Stehlik
 *  jfdn Jean-François DEL NERO
 *  VRI  Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

// ==== Definitions ==========================================================

        .globl  meminit                 // memory initialization

// ==== References ===========================================================

        .extern memdone       // return to, after memory config is done
        .extern _warm_magic

        // functions for detecting hardware and CPU type
        .extern _check_read_byte
        .extern _detect_cpu
        .extern _bzero

        // ST-RAM settings
        .extern memctrl
        .extern _phystop
        .extern memvalid
        .extern memval2
        .extern memval3

        // FastRAM settings
        .extern _ramtop
        .extern _ramvalid

        // Overall memory map
        .extern sysvars_start // start of system variables
        .extern sysvars_end   // end of system variables
        .extern bssstart      // end of documented low memory tosvars
        .extern __end         // end of BSS segment
        .extern _os_end       // pointer to first byte RAM not used by OS

// ===========================================================================
// ==== meminit - Reuse or detect RAM settings, then initialize RAM areas ====
// ===========================================================================
meminit:

/*
 * The caller (either startup.S or comprimg.S) needs to have initialized sp
 * with a temporary supervisor stack (jsr _check_read_byte/_detect_cpu)
 * The stack will be overwritten and lost during clearing of memory
 */

#if CONF_WITH_FALCON_MMU
        /* detect if this is a Falcon by reading $ff8282 register */
        pea     0xffff8282.w
        jsr     _check_read_byte
        addq.l  #4, sp

        tst.w   d0                      // is it Falcon MMU?
        beq     memchk                  // no -> skip 0xffff8007 test

        /* Initialize and test Falcon MMU ??? */
#ifdef __mcoldfire__
        lea     0xffff8007.w, a0
        bset    #6, (a0)                // seen in TOS 4.04
#else
        bset    #6, 0xffff8007.w        // seen in TOS 4.04
#endif
        beq     memconf                 // not sure what 6th bit means
#endif /* CONF_WITH_FALCON_MMU */

// ===========================================================================
// ==== memchk - Check if old memory config can be used ======================
// ===========================================================================
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

// ===========================================================================
// ==== The ST-RAM configuration is valid ====================================
// ===========================================================================
// Inputs:
//   d0.w: 1 = Falcon MMU, 0 = ST/TT MMU
// Outputs:
//   d6.b: old memctrl
//   d5.l: old _phystop
//   d3.l: old _ramvalid (#if CONF_WITH_FASTRAM)
//   a3.l: old _ramtop (#if CONF_WITH_FASTRAM)
//   d7.b: _first_boot (autodetected)

        // Load old RAM settings into registers.
        // They will stay in safe place in case we clear the sysvars.
        move.b  memctrl, d6
        move.l  _phystop, d5
#if CONF_WITH_FASTRAM
        move.l  _ramvalid, d3
        move.l  _ramtop, a3
#endif

#if CONF_WITH_FALCON_MMU
        tst.w   d0                      // is it Falcon MMU?
        bne.s   memory_reused           // yes -> skip memctrl init
#endif

#if CONF_WITH_ST_MMU
        move.b  d6, 0xffff8001.w        // write old valid config to controller
#endif

memory_reused:
        // If the memory is reused, this is not a first boot
        moveq   #0, d7                  // _first_boot = FALSE

#if CONF_DETECT_FIRST_BOOT_WITHOUT_MEMCONF
        // If _warm_magic == WARM_MAGIC, this is explicitly not a first boot
        lea     _warm_magic, a0
        move.l  #0x5741524D, d1         // WARM_MAGIC
        cmp.l   (a0), d1
        beq.s   .warm_magic_done        // Indeed, this is not a first boot

        // _warm_magic != WARM_MAGIC: this is a first boot
        moveq   #1, d7                  // _first_boot = TRUE
.warm_magic_done:
#endif
        bra     stram_detected

// ===========================================================================
// ==== memconf - Detect RAM and configure the memory controller =============
// ===========================================================================
// Inputs:
//   d0.w: 1 = Falcon MMU, 0 = ST/TT MMU
// Outputs:
//   d6.b: new value of 0xffff8001 (maybe fake), to put into memctrl.
//   d5.l: detected end of the ST-RAM, to put into _phystop.
//   d7.b: _first_boot (always 1 after memconf)
// Must be preserved:
//   d3.l: old _ramvalid (#if CONF_WITH_FASTRAM)
//   a3.l: old _ramtop (#if CONF_WITH_FASTRAM)
memconf:
#if CONF_STRAM_SIZE
        // The size of the ST-RAM is hardcoded: no real detection
        move.l  #CONF_STRAM_SIZE,d5     // this is phystop - the end of RAM
        moveq   #0,d6                   // fake memctrl
        // Fall into memconf_done
#elif defined (MACHINE_AMIGA)
        lea     amiga_memconf_ret(pc),a6
        jmp     amiga_memconf
amiga_memconf_ret:
        // Fall into memconf_done
#else /* ST/TT/Falcon memconf */

#if CONF_WITH_FALCON_MMU
        tst.w   d0                      // is it Falcon MMU?
        bne     falcmemconf             // yes -> jump to Falcon mem routine
#endif

#if CONF_WITH_ST_MMU

// ===========================================================================
// ==== memconf for ST =======================================================
// ===========================================================================
/*
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
#ifdef __mcoldfire__
        lea     0xffff8001.w,a0
        move.b  #0xa,(a0)               // set hw to 2 banks by 2 mb (reg 0xffff8001 - memorycontroller)
#else
        move.b  #0xa,0xffff8001.w       // set hw to 2 banks by 2 mb (reg 0xffff8001 - memorycontroller)
#endif

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
        moveq   #0,d5                   // total memory (later phystop)
        moveq   #0,d6                   // MMU setting set to 0

        /* test the presence of a 256k bank config */
test_256k:
#ifdef __mcoldfire__
        lsr.l   #2,d6                   // switch bank in configuration byte
#else
        lsr.w   #2,d6                   // switch bank in configuration byte
#endif
        moveq   #0,d4                   // bank memory size

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
        move.b  d6,0xffff8001.w         // set hw memory config (reg 0xffff8001 - memorycontroller)

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
        bra     memconf_done

#endif /* CONF_WITH_ST_MMU */

#if CONF_WITH_FALCON_MMU

// ===========================================================================
// ==== memconf for Falcon ===================================================
// ===========================================================================
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

        // Fall into memconf_done

#endif /* CONF_WITH_FALCON_MMU */

#endif /* ST/TT/Falcon memconf */

memconf_done:
        // Since the memory was not valid, this is a first boot
        moveq   #1, d7                  // _first_boot = TRUE
#if CONF_WITH_FASTRAM
        // Since the ST-RAM was invalid, FastRAM settings were also invalid
        moveq   #0, d3                  // _ramvalid
#endif

// ===========================================================================
// ==== End of ST-RAM detection ==============================================
// ===========================================================================
/*
 * At this point:
 * - The size of the ST-RAM has been detected
 * - The MMU has been properly initalized
 * - The ST-RAM is ready to be used
 * - The stack is valid between sysvars and BSS
 *
 * d7.b = _first_boot
 * d5.l = new phystop
 * d6.b = new memctrl
 * d3.l = old _ramvalid
 * a3.l = old _ramtop (if old _ramvalid was valid)
 */
stram_detected:
        tst.b   d7                      // first boot ?
        beq     any_boot                // no: skip first boot initialization

// ===========================================================================
// ==== This part is executed only on first boot =============================
// ===========================================================================

        /* Clear the sysvars */
        move.l  #sysvars_start, a0
        move.l  #sysvars_end, a1
        bsr     call_bzero_a0_a1

        /* Store the ST-RAM parameters in the ST-RAM itself */
        move.b  d6, memctrl             // set copy of hw memory config
        move.l  d5, _phystop            // highest address as phystop

        /* Make ST-RAM configuration valid */
#ifdef __mcoldfire__
        lea     memvalid, a0
        move.l  #0x752019f3, (a0)       // set memvalid to ok
        lea     memval2, a0
        move.l  #0x237698aa, (a0)       // set memval2 to ok
        lea     memval3, a0
        move.l  #0x5555aaaa, (a0)       // set memval3 to ok
#else
        move.l  #0x752019f3, memvalid   // set memvalid to ok
        move.l  #0x237698aa, memval2    // set memval2 to ok
        move.l  #0x5555aaaa, memval3    // set memval3 to ok
#endif

#if CONF_DETECT_FIRST_BOOT_WITHOUT_MEMCONF
        lea     _warm_magic, a0
        move.l  #0x5741524D, d1         // WARM_MAGIC
        move.l  d1, (a0)                // Next boot will not be a first boot
#endif

// ===========================================================================
// ==== TT-RAM (FastRAM) detection (PES) =====================================
// ===========================================================================
/*
 * This alternate RAM begins at 16MB boundary ($1000000)
 *
 * Inputs (#if CONF_WITH_FASTRAM):
 *   d3.l: old _ramvalid
 *   a3.l: old _ramtop (if old _ramvalid was valid)
 */
        move.l  #0x1357BD13,d1          // _ramvalid magic

#if !CONF_WITH_FASTRAM
        sub.l   a3, a3                  // _ramtop = 0
#elif CONF_FASTRAM_SIZE
        // The size of the FastRAM is hardcoded: no real detection
        move.l  #0x1000000+CONF_FASTRAM_SIZE,a3    // fixed ramtop
#else
        cmp.l   d1,d3                   // old _ramvalid valid ?
        beq     fastram_detected        // yes: skip detection

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

        /* no FastRAM found */
        sub.l   a3, a3                  // _ramtop = 0
alter:
#endif /* CONF_FASTRAM_SIZE */

/*
 * At this point:
 *   a3.l = new _ramtop
 *   d1.l = _ramvalid magic
 */
fastram_detected:
        // Write FastRAM settings into system variables (in ST-RAM)
        // Note that, like TOS 1.06 or TOS 2.06, we always set _ramvalid
        // as valid even if _ramtop is 0.
        move.l  a3, _ramtop             // set ramtop to size of found memory
        move.l  d1, _ramvalid

// ===========================================================================
// ==== This part is executed on any boot (first boot or reset) ==============
// ===========================================================================
any_boot:
        // Clear the BSS
        move.l  #bssstart, a0
        move.l  #__end, a1
        bsr     call_bzero_a0_a1

        // Remember if this is a first boot or not
        move.b  d7, _first_boot

        jmp     memdone                 // config done -> return to startup.s

// ===========================================================================
// ==== Below are utility functions ==========================================
// ===========================================================================

// Clear memory from a0 to a1
call_bzero_a0_a1:
        sub.l   a0,a1
        move.l  a1,-(sp)    // size
        move.l  a0,-(sp)    // start
        jsr     _bzero
        addq.l  #8,sp
        rts
