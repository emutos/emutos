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



// ==== Defines ==============================================================

        .equ    TPASTART, 0xE000        // default start address of tpa area



// ==== References ===========================================================

        .global meminit                 // memory initialization



// ==== startup.s - variables for memory  ====================================

        .xdef   memdone       // return to, if memory config done



// ==== tosvars.s - TOS System variables =====================================

	.xdef	bssstrt	

        .xdef   memctrl       
        .xdef   _phystop       

        .xdef   _membot       
        .xdef   _memtop       

        .xdef   memvalid      
        .xdef   memval2       
        .xdef   memval3       

        .xdef   _v_bas_ad     

	.xdef   bssstart
        .xdef   _bssend



// ===========================================================================
// ==== TEXT segment (TOS image) =============================================
// ===========================================================================

        .text



// ==== Clear RAM ============================================================
//        move.l  _membot, a0             // Set start of RAM
//clrbss:
//        clr.w   (a0)+                   // Clear actual word
//        cmp.l   _memtop, a0             // End of BSS reached?
//        bne     clrbss                  // if not, clear next word

//        pea 	msg_clrbss  // Print, what's going on
//        bsr 	_kprint
//        addq 	#4,sp



// ==== Clear BSS before calling any C function ======================
// the C part expects the bss to be cleared, so let's do this early

meminit:



	lea 	bssstart, a0
	lea 	_bssend-1, a1
	move.l 	a1, d0
	sub.l 	a0, d0
	lsr.l 	#2, d0
clearbss:
	clr.l 	(a0)+
	dbra 	d0, clearbss

//        pea msg_bss     // Print, what's going on
//        bsr _kprint
//        addq #4,sp



// ==== Check, if old memory config can be used ==============================
// return address has been save in a6

        cmp.l   #0x752019f3, memvalid   // magic in memvalid ?
        bne     memconf			// no -> config memory
        cmp.l   #0x237698aa, memval2    // magic in memval2 ?
        bne     memconf			// no -> config memory

memok:
        move.b  memctrl, 0xffff8001     // write old valid config to controller
        				// phystop should then also be ok
        bra	memdone                 // config ok -> memdone in startup.s



// ==== Find out memory size for TPA =========================================
// The memory configuration is found out by running into bus-errors, if memory
// is not present at a tested address. Therefor a special buserror-handler is
// temporarely installed. Works just for 4mb or more till now. (MAD)

memconf:
        clr.w   d6                      // start value form controller
        move.b  #0xa, 0xffff8001        // set hw to 2 banks by 2 mb

        movea.w #8, a0                  // startoffset for test
        lea     0x200008, a1            // a1 to 2. bank
        clr.w   d0                      // first test bitpattern

wrt_patt1:      
        move.w  d0, (a0)+               // write to both memory banks
        move.w  d0, (a1)+
        add.w   #0xfa54, d0             // bitpattern
        cmpa.l  #0x200, a0              // till adr. 0x200
        bne.s   wrt_patt1

        move.l  #0x200000, d1           // d1 to 2. bank
bank1:
        lsr.w   #2, d6

        movea.w #0x208, a0
        lea     after1(pc), a5          // save return address
        bra     memtest                 // memory test
after1:
        beq.s   nextbank                // ok 128k

        movea.w #0x408, a0
        lea     after2(pc), a5          // save return address
        bra     memtest                 // memory test
after2:
        beq.s   conf2mb                 // ok 512k

        movea.w #8, a0
        lea     after3(pc), a5          // save return address
        bra     memtest                 // memory test
after3:
        bne.s   nextbank

        addq.w  #4, d6
conf2mb:
        addq.w  #4, d6                  // configuration value to 2 mbyte

nextbank:
        sub.l   #0x200000, d1           // next bank
        beq.s   bank1                   // test for bank 1.
  
  
        move.b  d6, 0xffff8001          // program memorycontroller

        movea.l 8, a4                   // save bus-error vector
        lea     buserror(pc), a0        // adr. new  bus error routine

        move.l  a0, 8                   // set for test where memory bank ends
        move.w  #0xfb55, d3             // start-bitpattern
        move.l  #0x20000, d7            // startadr: 128 k
        movea.l d7, a0

teston:
        movea.l a0, a1
        move.w  d0, d2
        moveq   #0x2a, d1               // 43 worte

loopmem1:
        move.w  d2, -(a1)
        add.w   d3, d2
        dbra    d1, loopmem1
        movea.l a0, a1
        moveq   #0x2a, d1

loopmem2:
        cmp.w   -(a1), d0
        bne.s   buserror
        clr.w   (a1)
        add.w   d3, d0
        dbra    d1, loopmem2            // test next word
        adda.l  d7, a0
        bra.s   teston                  // weiter testen



// ==== Temporary bus error handler ==========================================
// Go on here, if memory vialotion has happenend. Above max values make config

buserror:
        suba.l  d7, a0
        move.l  a0, d5
      	move.l	a4, 0x00000008          // restore old bus error vector
        movea.l d5, a0
        move.l  #0x400, d4              // lowest address for clearing

        movem.l emptylong(pc), d0-d3    // empty bytes for d-register clearing
clearmem:
        movem.l d0-d3, -(a0)            // delete 16 bytes
        cmpa.l  d4, a0                  // lowest address reached?
        bne.s   clearmem
        suba.l  a5, a5
  
  
  
// ==== Set new memory configuration to sysvars ==============================

        move.b  d6, memctrl             // memctrl
        move.l  d5, _phystop            // higest address as phystop
        move.l  #0x752019f3, memvalid   // set memvalid to ok
        move.l  #0x237698aa, memval2    // set memval2 to ok

        bra	memdone                 // config done -> return to startup.s



// ===========================================================================
// ==== Subroutines ==========================================================
// ===========================================================================

// ==== Test memory ==========================================================

memtest:
        add.l   d1, a0
        clr.w   d0
        lea     0x1f8(a0), a1
memtloop:
        cmp.w   (a0)+, d0
        bne     memerr
        add.w   #0xfa54, d0
        cmp.l   a0, a1
        bne     memtloop
        
memerr:
        jmp     (a5)



// ==== Some messages ========================================================

msg_clrbss:
        .ascii "BIOS: Cleared RAM ...\n\0"

msg_bss:
        .ascii "BIOS: Cleared BSS segment ...\n\0"



// ==== Videopalette for shifter =============================================

emptylong:
        dc.l    0
        dc.l    0
        dc.l    0
        dc.l    0



// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end





// this is easier... (MAD)
//bus_error:
//      	move.l	a0, 0x00000008          // restore old bus error vector
//      	move.l	a1,sp			// and old stack pointer
//
//      	move.l	m_start,a0		// clear TPA
//      	move.l	m_length,d0		// get byte count
//      	lsr.l	#1,d0			// make word count
//      	subq.l	#1,d0			// pre-decrement for DBRA use
//      	clr.w	d1			// handy zero
//
//parity_loop:
//      	move.w	d1,(a0)+		// clear word
//      	dbra	d0,parity_loop
	




