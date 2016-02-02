/*
 * startup.S - EmuTOS startup module
 *
 * Copyright (c) 2001-2015 by the EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



#include "asmdefs.h"
#include "header.h"


// ==== Defines ==============================================================

        .equ    SUPSIZ, 1024    // size of supervisor stack in words

        .equ    cart_base, 0x00fa0000 // Base of ROM port cartridge

// ==== References ===========================================================


        .globl  _os_entry
#if CONF_WITH_CARTRIDGE
        .globl  _run_cartridge_applications
#endif
        .globl  _stktop

// ==== tosvars.s - TOS System variables =====================================

        .extern resvalid
        .extern resvector
//        .extern _kprintf

// ==== memory.s - variables for memory  =====================================

        .extern meminit       // initialize the memory and it's controller

// ==== Overall memory map ===================================================

        .extern _end          // end of bss + comm sections (ld generated)
        .extern _etext        // end of text section
        .extern _edata        // end of data section


// ===========================================================================
// ==== TEXT segment (TOS image) =============================================
// ===========================================================================

        .text


/*
 * OSHEADER
 */

        .globl  _main           // OS entry point
        .globl  _os_end
        .globl  _os_beg
        .globl  _os_magic
        .globl  _os_date
        .globl  _os_conf
        .globl  _os_dosdate
        .globl  _root
        .globl  _shifty
        .globl  _run

#if DIAGNOSTIC_CARTRIDGE
    .dc.l   0xfa52235f  // diagnostic cartridge magic number
#endif

#if defined(MACHINE_AMIGA) && !EMUTOS_LIVES_IN_RAM
    .dc.w   0x1111      // 256 kB ROM
    .dc.w   0x4ef9      // jmp
    .dc.l   amiga_main  // pc at power on or after physical reset

    /* Some softkick programs (ex: BlizKick) requires the entry point to be here */
    .org    0xd0
    reset
amiga_main:
    // drop into _os_entry
#endif

_os_entry:
    bra.s   _main       // os_entry, branch to _main
os_version:
    .dc.w   TOS_VERSION // os_version, TOS version
reseth:
    .dc.l   _main       // reseth, pointer to reset handler
_os_beg:
    .dc.l   _os_entry   // os_beg, base of os = _sysbase
_os_end:
#if EMUTOS_LIVES_IN_RAM
    // When EmuTOS is run from the RAM, the BSS starts at address 0 as usual,
    // but the TEXT and DATA segments are just after the BSS.
    // Thus the first unused RAM address is the end of the DATA segment.
    .dc.l   _edata
#else
    // When EmuTOS is run from the ROM, the TEXT and DATA segments stays in the
    // ROM, but the BSS starts at address 0.
    // Thus the first unused RAM address is the end of the BSS segment.
    .dc.l   _end        // os_end, first byte RAM not used by OS
#endif
os_res1:
    .dc.l   _main       // os_res1, reserved
_os_magic:
#if WITH_AES
    .dc.l   _ui_mupb    // os_magic, pointer to GEM's MUPB
#else
    .dc.l   0           // os_magic, pointer to GEM's MUPB
#endif
_os_date:
    .dc.l   OS_DATE     // os_date, Date of system build
_os_conf:
    .dc.w   (OS_CONF << 1) + OS_PAL // Flag for PAL version + country
_os_dosdate:
    .dc.w   OS_DOSDATE  // os_date, Date of system build in GEMDOS format
os_root:
    .dc.l   _root       // Pointer to the GEMDOS mem pool - not yet supported
os_kbshift:
    .dc.l   _shifty     // Pointer to the keyboard shift keys states
os_run:
    .dc.l   _run        // Pointer to a pointer to the actual basepage
os_dummy:
    .ascii  "ETOS"      // _main should start at offset 0x30, shouldn't it?




/*
 * Get in supervisor mode and reset all Hardware
 *
 * The following considerations have been taken into account:
 * 1. for unknown reasons, real Falcons need an access to location $ffff8006
 *    before and after the reset instruction
 * 2. in order to run the same code on systems without a register at $ffff8006,
 *    the bus error vector must be intercepted prior to #1
 * 3. since non-68000 systems use the VBR to point to the start of the exception
 *    vectors, the VBR must be zeroed prior to #2 via MOVEC
 * 4. in order to run the same code on 68000 systems (which don't have MOVEC),
 *    the illegal instruction vector must be intercepted prior to #3.  for this,
 *    it doesn't matter if the VBR is non-zero because, if it is, the MOVEC
 *    instruction must be legal so the trap won't get taken ...
 */

_main:
        // disable the interrupts
        // this is useless on cold boot,
        // but it matters for software reset
        move    #0x2700,sr

        lea     _stktop,sp     // required for exception

#ifndef __mcoldfire__
        // first, reset the VBR
        // it may have been modified before emutos-ram or soft reset
        move.l  #vbr_ok,16     // must be 68000 if we get illegal instrn error
        moveq   #0,d0
        MOVEC_D0_VBR           // initialise VBR to zero
vbr_ok:
#endif

#if CONF_WITH_RESET && !defined(MACHINE_AMIGA)
        // now, reset the external hardware

        // we don't need to call reset on Amiga
        // because either this is a cold boot
        // or reset has been called just before amiga_main

#if CONF_ATARI_HARDWARE
        // on Falcon, we need special care around the reset instruction
        move.l  #not_falcon,8  // can't be Falcon if we get bus error

        // special Falcon reset
        tst.w   0xffff8006.w   // magic: must access this before & after the reset
        reset                  // reset all hardware
        tst.w   0xffff8006.w   // remaining magic
        bra.b   main_after_reset_instruction
#endif /* CONF_ATARI_HARDWARE */

not_falcon:
        // we are not on Falcon, a simple reset is enough
        reset
main_after_reset_instruction:
#endif /* CONF_WITH_RESET && !defined(MACHINE_AMIGA) */

#ifdef __mcoldfire__
        lea     coldfire_startup_ret,a6
        jmp     coldfire_startup
coldfire_startup_ret:
#endif

#if CONF_WITH_CARTRIDGE
/*
 * Check for diagnostic cartridge
 */

#ifdef __mcoldfire__
        move.l  cart_base, d0
        cmp.l   #0xfa52235f, d0         // magic - is cartridge present?
#else
        cmp.l   #0xfa52235f, cart_base  // magic - is cartridge present?
#endif
        bne     nodiag                  // no -> go on
        lea     nodiag(pc), a6          // save return address
        jmp     cart_base+4             // execute diagnostig cartridge
nodiag:
#endif /* CONF_WITH_CARTRIDGE */

#ifdef MACHINE_AMIGA
        lea     amiga_startup_ret,a6
        jmp     amiga_startup
amiga_startup_ret:
#endif

/*
 * disable the instruction/data caches
 */
#ifndef __mcoldfire__
        move.l  #cache_done,0x10   // must be 68000 if we get illegal instrn error
        move.l  #0x00000808,d0     // clear & disable instruction/data caches on
        MOVEC_D0_CACR              //  68020-60 (even though bit usage differs!)
cache_done:
#endif

/*
 * disable the MMU
 */
#ifndef __mcoldfire__
        move.l  #not_68030,0x2c    // PMOVE is lineF on non-68030 systems
        PMOVE_TO_TTR0(zero)        // first we initialise the TTRs (ACRs on a 68ec030)
        PMOVE_TO_TTR1(zero)
        move.l  #mmu_done,0x2c     // since PMOVE_TO_TC doesn't exist on a 68ec030,
        PMOVE_TO_TC(zero)          //  we're done if we get a lineF exception ...
        bra.b   mmu_done
not_68030:
        move.l  #mmu_done,0x10     // if a MOVEC causes an exception, we're done
        moveq   #0,d0
        MOVEC_D0_ITT0              // first we initialise the TTRs (ACRs on a 68ec040)
        MOVEC_D0_ITT1
        MOVEC_D0_DTT0
        MOVEC_D0_DTT1
        MOVEC_D0_TC                // disable translation on 68040-60 (will error out
                                   //  on a 68ec040)
mmu_done:
#endif

/*
 * Check, if we should jump to a reset vector after warmboot
 */

resetvec:
#ifdef __mcoldfire__
       move.l   resvalid, d0
       cmpi.l   #0x31415926, d0         // Jump to resetvector?
#else
       cmpi.l   #0x31415926, resvalid   // Jump to resetvector?
#endif
       bne.s    noreset                 // No --> noreset
       move.l   resvector, d0           // Yes: old resvec to d0
       beq.s    noreset                 // Address null? Yes --> noreset
       btst     #0, d0                  // Address odd ?
       bne.s    noreset                 // Yes --> noreset
       movea.l  d0, a0                  // resvec
       lea      resetvec(pc), a6        // save return address
       jmp      (a0)                    // jump to resvec
noreset:


/*
 * Set up a supervisor stack
 * It is required by meminit, but may be destroyed.
 */

        lea     _stktop, sp             // Setup Supervisor Stack


/*
 * memory configuration
 */

        .globl  memdone         // return to, if memory config done
        jmp     meminit         // do/test memory config and clear it (needs sp)
memdone:


/*
 * initialize kprintf (for debug prints)
 */

        jsr     kprintf_init


/*
 * From now, the stack is allowed, and C code can be used.
 * The BIOS startup goes on in bios.c
 */

        jmp     _biosmain


#if CONF_WITH_CARTRIDGE
/*
 * void run_cartridge_applications(WORD typebit);
 *
 * Run all cartridge applications of the specified type.
 *
 * typebit: application type bit number which must be set to 1 to match
 */
_run_cartridge_applications:
        move.w  4(sp),d0                // application bit number
        lea     cart_base, a0
#ifdef __mcoldfire__
        move.l  #0xABCDEF42, d1
        cmp.l   (a0)+, d1               // is cartridge present?
#else
        cmp.l   #0xABCDEF42, (a0)+      // is cartridge present?
#endif
        bne     cartover                // no -> cartover
testtype:
        btst    d0, 4(a0)               // What type?
        beq     nextapp

#ifdef __mcoldfire__
        lea     -60(sp), sp
        movem.l d0-d7/a0-a6, (sp)       // save registers
#else
        movem.l d0-d7/a0-a6, -(sp)      // save registers
#endif
        move.l  4(a0),d0                // d0 = address of cartridge init
        andi.l  #0x00ffffff,d0          // make it 24-bit clean
        movea.l d0,a0
        jsr     (a0)                    // execute app in cartridge
#ifdef __mcoldfire__
        movem.l (sp), d0-d7/a0-a6       // restore registers
        lea     60(sp), sp
#else
        movem.l (sp)+, d0-d7/a0-a6      // restore registers
#endif
nextapp:
        tst.l   (a0)                    // another application?
        move.l  (a0), a0
        bne     testtype
cartover:
        rts
#endif /* CONF_WITH_CARTRIDGE */

/* ELF toolchain support */
#ifdef ELF_TOOLCHAIN
/* Our code is compiled with -fleading-underscore, so the references to
 * external libraries will include a leading underscore.
 * However, libgcc was not compiled with that option, so its symbols do not
 * start with a leading underscore.
 * So we need to define leading-underscore aliases
 * to the no-leading-underscore functions in libgcc.
 */
        ELF_LIB_REF(__mulsi3)
        ELF_LIB_REF(__divsi3)
        ELF_LIB_REF(__udivsi3)
        ELF_LIB_REF(__modsi3)
        ELF_LIB_REF(__umodsi3)
#endif

// ===========================================================================
// ==== Read-only data =======================================================
// ===========================================================================
        .text

zero:   .dc.l   0


// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================
        .bss

_stkbot:        .ds.w   SUPSIZ  // Supervisor stack
_stktop:                        // filled from top to bottom
