/*
 * startup.S - EmuTOS startup module
 *
 * Copyright (c) 2001-2005 by the EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



#include "asmdefs.h"
#include "header.h"


// ==== Defines ==============================================================

        .equ    SUPSIZ, 8500    // size of supervisor stack in words

        // Note: 1024 words should normally be enough for the stack, but
        // there were some strange problems with FreeMiNT and GFA-Basic
        // when it was so small, so I changed it back to > 8000 words...

        .equ    cart_base, 0x00fa0000 // Base of ROM port cartridge

// ==== References ===========================================================

        
        .global _os_entry
#if !DIAGNOSTIC_CARTRIDGE
        .global _run_cartridge_applications
#endif
        .global _stktop

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
// ==== BSS segment ==========================================================
// ===========================================================================
        .bss

_stkbot:        ds.w    SUPSIZ  // Supervisor stack
_stktop:                        // filled from top to bottom



// ===========================================================================
// ==== TEXT segment (TOS image) =============================================
// ===========================================================================

        .text


/*
 * OSHEADER
 */

        .global _main           // OS entry point
        .global _os_end
        .global _os_beg
        .global _os_magic
        .global _os_date
        .global _os_pal
        .global _os_dosdate
        .global _root
        .global _shifty
        .global _run

#if DIAGNOSTIC_CARTRIDGE
    dc.l    0xfa52235f  // diagnostic cartridge magic number
#endif
_os_entry:
    bra.s   _main       // os_entry, branch to _main
os_version:
    dc.w    TOS_VERSION // os_version, TOS version
reseth:
    dc.l    _main       // reseth, pointer to reset handler
_os_beg:
    dc.l    _os_entry   // os_beg, base of os = _sysbase
_os_end: 
#ifdef EMUTOS_RAM
    // When EmuTOS is run from the RAM, the BSS starts at address 0 as usual,
    // but the TEXT and DATA segments are just after the BSS.
    // Thus the first unused RAM address is the end of the DATA segment.
    dc.l    _edata
#else
    // When EmuTOS is run from the ROM, the TEXT and DATA segments stays in the
    // ROM, but the BSS starts at address 0.
    // Thus the first unused RAM address is the end of the BSS segment.
    dc.l    _end        // os_end, first byte RAM not used by OS
#endif
os_res1:        
    dc.l    _main       // os_res1, reserved
_os_magic:
    dc.l    _ui_mupb    // os_magic, pointer to GEM's MUPB
_os_date:
    dc.l    OS_DATE     // os_date, Date of system build
_os_pal:        
    dc.w    OS_PAL      // Flag for PAL version + country
_os_dosdate:
    dc.w    OS_DOSDATE  // os_date, Date of system build in GEMDOS format
os_root:
    dc.l    _root       // Pointer to the GEMDOS mem pool - not yet supported
os_kbshift:
    dc.l    _shifty     // Pointer to the keyboard shift keys states
os_run:
    dc.l    _run        // Pointer to a pointer to the actual basepage
os_dummy:
    .ascii  "ETOS"      // _main should start at offset 0x30, shouldn't it?




/*
 * Get in supervisor mode and reset all Hardware
 */
 
_main:
#if !DIAGNOSTIC_CARTRIDGE
        move    #0x2700,sr     // disable interrupts
#ifndef __mcoldfire__
        reset                  // reset all hardware
#endif



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
#endif /* !DIAGNOSTIC_CARTRIDGE */


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

        .global memdone         // return to, if memory config done
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


#if !DIAGNOSTIC_CARTRIDGE
/*
 * void run_cartridge_applications(WORD typebit);
 *
 * Run all cartridge applications of the speficied type.
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
        move.l  4(a0), a0
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
#endif /* !DIAGNOSTIC_CARTRIDGE */

        .end

