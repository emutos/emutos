/*
 * memory.S - memory initialization
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



// ==== References ===========================================================

        .global meminit                 // memory initialization



// ==== startup.s - variables for memory  ====================================

        .xdef   memdone       // return to, if memory config done



// ==== tosvars.s - TOS System variables =====================================


        .xdef   memctrl       
        .xdef   _phystop       

        .xdef   memvalid      
        .xdef   memval2       
        .xdef   memval3       

        .xdef   ramtop
        .xdef   ramvalid

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

	lea 	bssstart, a0
	lea 	_end-1, a1
	move.l 	a1, d0
	sub.l 	a0, d0
	lsr.l 	#2, d0
clearbss:
	clr.l 	(a0)+
	dbra 	d0, clearbss



/*
 * memchk - Check, if old memory config can be used
 */
 
memchk:
        cmp.l   #0x752019f3, memvalid   // magic in memvalid ?
        bne     memconf			// no -> config memory
        cmp.l   #0x237698aa, memval2    // magic in memval2 ?
        bne     memconf			// no -> config memory
        cmp.l   #0x5555aaaa, memval3    // magic in memval3 ?
        bne     memconf			// no -> config memory

memok:
        move.b  memctrl, 0xffff8001     // write old valid config to controller
        				// phystop should then also be ok
        bra	memdone                 // config ok -> memdone in startup.s



/*
 * memconf - Configure the memory (and clear it)
 */
 
memconf:
        /* detect if this is a Falcon by reading $ff8006 register (joy) */

        /* The test fires a bus error on non-Falcon machines.
         * The bus error fills a couple of words on the temporary stack.
         * If we wanted to avoid stack usage we would have to abandon
         * this run-time check and go with #ifdef FALCON...
         */
        lea     8.w, a0
        move.l  (a0), a1
        move.l  #fbuserr, (a0)          // set up a new bus error handler
        move.w  sr, d1                  // remember SR
        move.l  sp, a0                  // remember SP
        clr.l   d0
        nop
        move.w  0xffff8006.w, d0        // read the Falcon MMU register
        nop
fbuserr:
        move.l  a0, sp                  // restore SP
        move.w  d1, sr                  // restore SR
        move.l  a1, 8.w                 // restore original bus error handler
        tst.w   d0
        bne     falcmemconf             // non-zero value on real Falcon

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

        bra.s   setmem

/*
 * memconf for Falcon
 */

falcmemconf:
        /* find out the memory size by checking the d0 = $ff8006 value */
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
less16: move.l  d0,d5            /* d5 = phystop */


/*
 * setmem - Set the new values to hardware and config
 */
  
setmem:
        move.b  d6, 0xffff8001          // program memorycontroller
        move.b  d6, memctrl             // set copy of hw memory config
        move.l  d5, _phystop            // highest address as phystop

        /* make memory config valid */
        move.l  #0x752019f3, memvalid   // set memvalid to ok
        move.l  #0x237698aa, memval2    // set memval2 to ok
        move.l  #0x5555aaaa, memval3    // set memval3 to ok

        /* TT-RAM (FastRAM) */
        clr.l   ramtop                  // set ramtop to 0 (= no FastRAM)
        move.l  #0x1357BD13, ramvalid   // set ramvalid to ok

        pea msg_bss     // Print, what's going on
        bsr _kprintf
        addq #4,sp

        bra	memdone                 // config done -> return to startup.s



#if 0
        /* clear memory till running into buserror or reaching top */
        move.l 	#0xe00000, d0           // maximum top of RAM
        move.l  _end, a0                // Set start of RAM
clrmem:
        clr.w   (a0)+                   // Clear actual word, then increase
        cmp.l   d0, a0                  // End of MEM reached?
        blt.s   clrmem                  // if not, clear next word
#endif



// ==== Some messages ========================================================

msg_bss:
        .ascii "BIOS: Cleared RAM ...\n\0"


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end

