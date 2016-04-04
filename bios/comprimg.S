/*
 * comprimg.S - EmuTOS startup module for a compressed RAM image
 *
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* LVL note: this is a very minimum startup module. it just setups memory
 * and decompresses the compressed RAM image, then jumps into the RAM TOS.
 */


#include "asmdefs.h"
#include "header.h"

        .globl  _main           // OS entry point
        .globl  _os_end

        .extern __end
        .extern resvalid
        .extern resvector
        .extern _stktop
        .extern meminit
        .extern memvalid
        .extern memval2
        .extern memval3
        .extern __edata
        .extern _uncompr

/*
 * OSHEADER
 */

        .text

_os_entry:
    bra.s   _main       // os_entry, branch to _main
os_version:
    .dc.w   TOS_VERSION // os_version, TOS version
reseth:
    .dc.l   _main       // reseth, pointer to reset handler
_os_beg:
    .dc.l   _os_entry   // os_beg, base of os = _sysbase
_os_end:
    .dc.l   __end       // os_end, first byte RAM not used by OS
os_res1:
    .dc.l   _main       // os_res1, reserved
_os_magic:
    .dc.l   0           // os_magic, pointer to GEM's MUPB
_os_date:
    .dc.l   OS_DATE     // os_date, Date of system build
_os_conf:
    .dc.w   (OS_CONF << 1) + OS_PAL // Flag for PAL version + country
_os_dosdate:
    .dc.w   OS_DOSDATE  // os_date, Date of system build in GEMDOS format
os_root:
    .dc.l   0           // Pointer to the GEMDOS mem pool
os_kbshift:
    .dc.l   0           // Pointer to the keyboard shift keys states
os_run:
    .dc.l   0           // Pointer to a pointer to the actual basepage (patched by compr.c)
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

        .globl  memdone         // return to, if memory config done
        jmp     meminit         // do/test memory config and clear it (needs sp)
memdone:

        /* make memory config valid */
        move.l  #0x752019f3, memvalid   // set memvalid to ok
        move.l  #0x237698aa, memval2    // set memval2 to ok
        move.l  #0x5555aaaa, memval3    // set memval3 to ok

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
 * check_read_byte - Verify access to the memory pointed to by the ptr
 *
 * C call: int check_read_byte(long);
 * returns 0 if reading the address caused a bus error
 */

        .globl  _check_read_byte
_check_read_byte:
        move.l  sp,d1
        move.l  (8).w,a1
        lea     berr.w(pc),a0
        move.l  a0,(8).w
        moveq   #0,d0
        nop                                     // flush pipeline

        move.l  4(sp),a0
        tst.b   (a0)
        nop                                     // flush pipeline

        moveq   #0x01,d0        // passed

berr:   move.l  a1,(8).w
        move.l  d1,sp
        nop
        rts
