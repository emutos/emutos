/*
 * memory.S - memory initialization
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *  joy  Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



// ==== References ===========================================================

        .global meminit                 // memory initialization


// ==== Misc. functions ======================================================

        .xdef   kprintf_init


// ==== startup.s - variables for memory  ====================================

        .xdef   memdone       // return to, if memory config done


// ==== detect.s - function for detecting CPU type and for testing memory  ===

        .xdef   _check_read_byte
        .xdef   _detect_cpu


// ==== tosvars.s - TOS System variables =====================================


        .xdef   memctrl       
        .xdef   _phystop       

        .xdef   memvalid      
        .xdef   memval2       
        .xdef   memval3       

        .xdef   _ramtop
        .xdef   _ramvalid

// ==== Overall memory map ===================================================

        .xdef   bssstart      // end of documented low memory tosvars
        .xdef   _end          // end of bss + comm sections (ld generated)


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
        dbra    d0, clearbss

        /* At this point, better initialize kprintf before we forget */
        jsr     kprintf_init
        /* This is it. C code and kprintf allowed past this point. */


/*
 * memchk - Check, if old memory config can be used
 */
 
memchk:
        cmp.l   #0x752019f3, memvalid   // magic in memvalid ?
        bne     memconf                 // no -> config memory
        cmp.l   #0x237698aa, memval2    // magic in memval2 ?
        bne     memconf                 // no -> config memory
        cmp.l   #0x5555aaaa, memval3    // magic in memval3 ?
        bne     memconf                 // no -> config memory

memok:
        move.b  memctrl, 0xffff8001     // write old valid config to controller
                                        // phystop should then also be ok
        bra     memdone                 // config ok -> memdone in startup.s



/*
 * memconf - Configure the memory (and clear it)
 */
memconf:
#if 1   // FIXME: set to 0 for Steem
        /* detect if this is a Falcon by reading $ff8006 register (joy) */
        pea     0xff8006
        bsr     _check_read_byte
        tst.w   d0
        bne     falcmemconf
#endif
/*
 * memconf for ST
 *
 * At first the memory configuration is set to 4 Mb. If the memory
 * configuration is not really 2 * 2 mb, the written test values will
 * repeat in memory. The trick is to find these repeated values and
 * set the memory config accordingly. No real memory check is done, 
 * because in emulators it should be always ok - hopefully!
 */
        move.b  #0xa, 0xffff8001        // set hw to 2 banks by 2 mb

        lea     0x000008, a0            // a0 points to 1. bank
        lea     0x200008, a1            // a1 points to 2. bank

        move.w  #0xabcd, d0             // my very special bitpattern
        move.w  d0, (a0)                // set bitpattern to 1st bank start
        move.w  d0, (a1)                // set bitpattern to 2nd bank start


        /* set start values */
set128kb:
        clr.l   d5                      // total memory (later phystop)
        clr.l   d6                      // configuration value to 128 kb
        move.l  #0x200000, d1           // d1 to 2. bank


        /* find config for both  banks (written for to be understood) */
bankloop:
        lsr.w   #2, d6                  // switch bank in configuration byte

        add.l   #0x20000, d5            // add default 128k to total mem
        movea.l d1, a0                  // set actual bank and
        adda    #0x208, a0              // add offset
        cmp.w   (a0)+, d0               // found repeated test value?
        beq.s   nextbank                // 128k ok in actual bank!

        add.l   #0x60000, d5            // add (512k-128k) to total mem
        movea.l d1, a0                  // set actual bank and
        adda    #0x408, a0              // add offset
        cmp.w   (a0)+, d0               // found repeated test value?
        beq.s   set2048kb               // 512k ok in actual bank!

        add.l   #0x160000, d5           // add (2048-512-128k) to total mem
        movea.l d1, a0                  // set actual bank and
        adda    #0x008, a0              // add offset
        cmp.w   (a0)+, d0               // found repeated test value?
        bne.s   nextbank                // 2048k ok in actual bank!

set512kb:
        addq    #4, d6                  // configuration value to 512 kb

set2048kb:
        addq    #4, d6                  // configuration value to 2 mb

nextbank:
        sub.l   #0x200000, d1           // first (other) bank
        beq.s   bankloop                // 

        move.b  d6, 0xffff8001          // program ST's memorycontroller
        move.b  d6, memctrl             // set copy of hw memory config
        move.l  d5, _phystop            // highest address as phystop
        bra     setmemok



/*
 * memconf for Falcon
 */

falcmemconf:
        /* find out the memory size by checking the d0 = $ff8006 value */
        move.w  0xffff8006.w, d0        // read the Falcon MMU register
        lsr.w   #8,d0
        lsr.b   #1,d0
        move.b  d0,d1
        and.b   #1,d1
        lsr.b   #2,d0
        and.b   #6,d0
        or.b    d0,d1
        move.l  #0x00080000,d0
        asl.l   d1,d0
        cmp.b   #5,d1
        bne.s   less16
        move.l  #0x00E00000,d0
less16: 
	move.l  d0, _phystop            // highest address as phystop



/*
 * TT-RAM (FastRAM) detection (joy)
 * This alternate RAM begins at 16MB boundary ($1000000)
 */
        /* clear MB counter */
        moveq   #0, d6

        /* test CPU - FastRAM is possible on 68020+ (32-bit address space) */
        bsr     _detect_cpu
        cmp.w   #20,d0
        blt     setramtop

        lea     0x1000000, a3           // start from 16MB boundary
        move.l  #1024*1024, d3          // test by blocks of 1MB size
loop:
        lea     -1(a3,d3.l), a0
        pea     (a0)
        bsr     _check_read_byte
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
        move.l  a3, _ramtop             // set ramtop to size of found memory
        move.l  #0x1357BD13, _ramvalid  // set ramvalid to ok

        move.l  #20,d1
        move.l  _ramtop, d0
        beq.s   zerofastram
        asr.l   d1, d0                  // ramtop divided by 2^20 = in MB
        sub.l   #16, d0                 // minus the first 16 MB of ST-RAM
zerofastram:
        move.l  d0, -(sp)               // is the resulting FastRAM size
        move.l  _phystop, d0
        asr.l   d1, d0
        move.l  d0, -(sp)               // phystop divided by 2^20 = in MB
        pea     msg_bss                 // Print, what's going on
        bsr     _kprintf                // this also requires stack!
        add.l   #12,sp



/*
 * setmemok - Set the new values to hardware and config
 */
  
setmemok:
        /* make memory config valid */
        move.l  #0x752019f3, memvalid   // set memvalid to ok
        move.l  #0x237698aa, memval2    // set memval2 to ok
        move.l  #0x5555aaaa, memval3    // set memval3 to ok

        bra     memdone                 // config done -> return to startup.s


// ==== Some messages ========================================================

msg_bss:
        .ascii "BIOS: Detected memory: ST-RAM = %ld MB, FastRAM = %ld MB\n\0"
msg_bss2:
        .ascii "BIOS: Cleared RAM ...\n\0"


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end

