/*
 * comprimg.S - EmuTOS startup module for a compressed RAM image
 *
 * Copyright (c) 02-03 by the EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* LVL note: this is a very minimum startup module. it just setups memory
 * and decompresses the compressed RAM image, then jumps into the RAM TOS.
 */


#include "asmdefs.h"
#include "header.h"

/* define this to 1 to activate minimum debug support. This will print 
 * data using the native feature interface. 
 */
#define COMPRDBG 0

        .extern _end
        .extern resvalid
        .extern resvector
        .extern meminit
        .extern __edata
        .extern _uncompr

/* To be sure not to have any trouble when loading the actual image
 * in RAM, just set the stack below the top of the exception vector 
 * zone.  We won't need that much of a stack anyway.
 */
_stktop = 0x378

/* NF_STDERR id */
stderr = 0x37C

/*
 * OSHEADER
 */
 
        .text

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
    dc.l    0           // os_magic, pointer to GEM's MUPB
_os_date:
    dc.l    OS_DATE     // os_date, Date of system build
_os_pal:        
    dc.w    OS_PAL      // Flag for PAL version + country
_os_dosdate:
    dc.w    OS_DOSDATE  // os_date, Date of system build in GEMDOS format
os_root:
    dc.l    0           // Pointer to the GEMDOS mem pool - not yet supported
os_kbshift:
    dc.l    0           // Pointer to the keyboard shift keys states
os_run:
    dc.l    0           // Pointer to a pointer to the actual basepage
os_dummy:
    .ascii  "ETOS"      // _main should start at offset 0x30, shouldn't it?




/*
 * Get in supervisor mode and reset all Hardware
 */
 
_main:                         // stunt to guarantee entry into supervisor mode
        move    #0x2700,sr     // disable interrupts
#ifndef __mcoldfire__
        reset                  // reset all hardware
#endif


/*
 * Check for diagnostic cartridge
 */

        .equ    cart_base,      0x00fa0000

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


/*
 * Set up a supervisor stack 
 */

        lea     _stktop, sp             // Setup Supervisor Stack
        
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
       bne      noreset                 // No --> noreset
       move.l   resvector, d0           // Yes: old resvec to d0
       beq      noreset                 // Address null? Yes --> noreset
       btst     #0, d0                  // Address odd ?
       bne      noreset                 // Yes --> noreset
       movea.l  d0, a0                  // resvec
       lea      resetvec(pc), a6        // save return address
       jmp      (a0)                    // jump to resvec
noreset:

        .global memdone         // return to, if memory config done
        jmp     meminit         // do/test memory config and clear it (needs sp)
memdone:

/*
 * from there, check the starting address of the compressed data by
 * looking for 'CMPR' after the end of the compiled binary
 */
        lea     __edata,a0
        move.l  cmpr,d0
        bra     find
cmpr:
        .ascii  "CMPR"
findloop:
        addq.l  #2,a0
find:
        move.l  (a0),d1
        cmp.l   d0,d1
        bne     findloop
        addq.l  #4,a0

/* okay, now a0 is just after 'CMPR', that's the start of compressed data.
 * It points to a header containing the address to where we must 
 * decompress, and the length of uncompressed data. 
 * Let's just pass all this to uncompress()
 */
        move.l  a0,-(sp)
        
        jsr     _uncompr

        /* jump at start of RAMTOS. The address of the header is still
         * on the stack.
         */
        move.l  (sp)+,a0
        move.l  (a0),a0
        jmp     (a0)
        

/*
 * stubs - these are called during memory initialisation
 */

        .global kprintf_init
        .global _kprintf
kprintf_init:
_kprintf:
        moveq.l #0,d0
        rts


/*
 * minimum natfeat init - only initializes the stderr id for debug below
 */
        .global _natfeat_init

#if DETECT_NATIVE_FEATURES || COMPRDBG

        .equ vec_illegal, 0x10       // illegal exception vector

_natfeat_init:
        clr.l   stderr
        move.l  sp,a1
        move.l  #fail_natfeat,vec_illegal
        pea     nf_stderr_name
        sub.l   #4,sp
        dc.w    0x7300              // Jump to NATFEAT_ID
        move.l  d0,stderr
fail_natfeat:
        move.l  a1,sp
        rts
        
nf_stderr_name:
        .ascii  "NF_STDERR\0"
        .even

#else /* no native features */
        
_natfeat_init:
        clr.l   stderr
        rts       

#endif
        
/*
 * debug - activate to use minimal debug support using natfeat NF_STDERR.
 * usage:
 *    move.l d0,-(sp)   
 *    jsr p   
 * prints d0 in hexadecimal
 *    jsr h    
 * prints "H %08lx" the current pc
 */

#if COMPRDBG
        .global p
p:
        movem.l d0-d2,-(sp)
        move.l  16(sp),d0
        /* stack:  d0,d1,d2,rts,val */
        move.w  #7,d1
nibble: 
        rol.l   #4,d0
        move.b  d0,d2
        and.b   #0xF,d2
        add.b   #48,d2
        cmpi.b  #58,d2
        blt     digit
        add.b   #7,d2
digit:
        move.b  d2,-(sp)
        bsr     outch
        addq.l  #2,sp
        dbra    d1,nibble
        move.b  #10,-(sp)
        bsr     outch
        addq.l  #2,sp
        movem.l (sp)+,d0-d2
        move.l  (sp),4(sp)
        addq.l  #4,sp
        rts

        .global h
h:
        move.b  #72,-(sp)
        bsr     outch
        move.b  #32,-(sp)
        bsr     outch
        move.l  4(sp),(sp)
        bsr     p
        move.b  #10,-(sp)
        bsr     outch
        addq.l  #2,sp
        rts
        
outch:
        subq.l  #4,sp
        clr.b   9(sp)
        move.l  sp,(sp)
        add.l   #8,(sp)
        move.l  stderr,-(sp)
        beq     no_natfeat
        move.l  d0,-(sp)
        dc.w    0x7301
        move.l  (sp)+,d0
no_natfeat:
        add.l   #12,sp
        rts
#endif


/*
 * check_read_byte - Verify access to the memory pointed to by the ptr
 *
 * C call: int check_read_byte(long);
 * returns 0 if reading the address caused a bus error
 */

        .global _check_read_byte
_check_read_byte:
        move.l  sp,d1
        move.l  (8).w,a1
        lea     berr.w(pc),a0
        move.l  a0,(8).w
        clr.l   d0
        nop                                     // flush pipeline

        move.l  4(sp),a0
        tst.b   (a0)
        nop                                     // flush pipeline

        moveq   #0x01,d0        // passed

berr:   move.l  a1,(8).w
        move.l  d1,sp
        nop
        rts
