/*
 * startup.S - EmuTOS startup module
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *  LVL  Laurent Vogel
 *  THH  Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



#include "asmdefs.h"
#include "header.h"
#include "config.h"


// ==== Defines ==============================================================

        .equ    SUPSIZ, 8192            // size of supervisor stack in words


// ==== References ===========================================================

        
        .global _os_entry
        .global _cartscan

// ==== tosvars.s - TOS System variables =====================================
                
        .xdef   resvalid      
        .xdef   resvector     
        .xdef   _kprintf

        
// ==== memory.s - variables for memory  =====================================

        .xdef   meminit       // initialize the memory and it's controller

// ==== Overall memory map ===================================================

        .xdef   _end          // end of bss + comm sections (ld generated)
        .xdef   _etext        // end of text section
        .xdef   _edata        // end of data section
        

// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================
        .bss

_stkbot:        ds.w    SUPSIZ  // Supervisor stack
_stktop:                        // filled from top to bottom
        
aexecsave:      ds.l    1       // save zone for autoexec
        
        
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
        .global _shifty
        .global _run

_os_entry:
    bra.s   _main       // os_entry, branch to _main
os_version:
    dc.w    TOS_VERSION // os_version, TOS version
reseth:
    dc.l    _main       // reseth, pointer to reset handler
_os_beg:
    dc.l    _os_entry   // os_beg, base of os = _sysbase
_os_end: 
    dc.l    _end        // os_end, first byte RAM not used by OS
os_res1:        
    dc.l    _main       // os_res1, reserved
_os_magic:
    dc.l    0x0         // os_magic, pointer to GEM's MUPB
_os_date:
    dc.l    OS_DATE     // os_date, Date of system build
_os_pal:        
    dc.w    OS_PAL      // Flag for PAL version + country
_os_dosdate:
    dc.w    OS_DOSDATE  // os_date, Date of system build in GEMDOS format
os_root:
    dc.l    0x0         // Pointer to the GEMDOS mem pool - not yet supported
os_kbshift:
    dc.l    _shifty     // Pointer to the keyboard shift keys states
os_run:
    dc.l    _run        // Pointer to a pointer to the actual basepage
os_dummy:
    dc.l    0           // ??? _main should start at 0xfc0030, shouldn't it?




/*
 * Get in supervisor mode and reset all Hardware
 */
 
_main:                         // stunt to guarantee entry into supervisor mode
        move    #0x2700,sr     // disable interrupts
        reset                  // reset all hardware



/*
 * Check for diagnostic cartridge
 */

        .equ    cart_base,      0x00fa0000

        cmp.l   #0xfa52235f, cart_base  // magic - is cartridge present?
        bne     nodiag                  // no -> go on
        lea     nodiag(pc), a6          // save return address
        jmp     cart_base+4             // execute diagnostig cartridge
nodiag:


/*
 * Set up a supervisor stack 
 */

        lea     _stktop, sp             // Setup Supervisor Stack



/*
 * Check, if we should jump to a reset vector after warmboot
 */

resetvec:
       cmpi.l   #0x31415926, resvalid   // Jump to resetvector?
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
 * LVL - I put back the mem configuration early, to be able to use
 * the stack, so that the rest of boot sequence can be made more
 * modular using CDECL routines.
 */

        .global memdone         // return to, if memory config done
        bra     meminit         // do/test memory config (needs sp)
memdone:

/* 
 * From now, the stack is allowed, and C code can be used.
 * The startup goes on in bios.c
 */

        jmp     _startup



/* Different subroutines */



/*
 * vidchng - change video resolution 
 *
 * Do nothing for now
 */

_vidchng:
        rts



/*
 * _ara_XHDI
 */
        .global _ara_XHDI
_ara_XHDI:
        dc.w 0x7137
        rts


/*
 * _cartscan - Use cartridge, if present
 *
 * get cartridge-type in d0, else use C glue.
 */

msg_cart:
        .ascii "BIOS: Cartridge has been initialized ...\n\0"
        .even

_cartscan:      
        move.w  4(sp),d0        
cartscan:
        lea     cart_base, a0
        cmp.l   #0xABCDEF42, (a0)+      // is cartridge present?
        bne     cartover                // no -> cartover
testtype:
        btst    d0, 4(a0)               // What type?
        beq     nextapp

//        pea     msg_cart
//        bsr     _kprintf
//        addq #4,sp

        movem.l d0-d7/a0-a6, -(sp)      // save registers
        move.l  4(a0), a0
        jsr     (a0)                    // execute app in cartridge
        movem.l (sp)+, d0-d7/a0-a6      // restore registers
nextapp:        
        tst.l   (a0)                    // another application?
        move.l  (a0), a0
        bne     testtype
cartover:
        rts



        .end




