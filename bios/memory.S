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

memok:
        move.b  memctrl, 0xffff8001     // write old valid config to controller
        				// phystop should then also be ok
        bra	memdone                 // config ok -> memdone in startup.s



/*
 * memconf - Configure the memory and clear it
 *
 * At first the memory configuratio is set to 4 Mb.
 * The real memory is found out by running into bus-errors, if memory 
 * is not present at a tested address. Therefor a special buserror-handler
 * is temporarely installed. The real memory may be bigger than the 
 * setting for the memory controller, because some emulators patch it in.
 */
 
memconf:

        /* set memory config to 4 Mb */
        move.b  #0x0a, d1               // set 2 banks by 2 mb
        move.b  d1, 0xffff8001          // set config to hw

        /* save old buserror handler for later restore */
       	move.l	0x00000008, a1 		// save old bus error vector
       	move.l	#buserr, 0x00000008     // and put in our temporary handler

        /* clear memory till running into buserror or reaching top */
	move.l 	#0xd00000, d0           // maximum top of RAM
        move.l  _end, a0                // Set start of RAM
clrmem:
        clr.w   (a0)+                   // Clear actual word, then increase
        cmp.l   d0, a0                  // End of MEM reached?
        blt     clrmem                  // if not, clear next word




/*
 * buserr - temporary buserror handler
 */
 
buserr:
        /* Set config */
        move.b  d1, memctrl             // set copy of hw memory config
        move.l  a0, _phystop            // higest address as phystop

        pea msg_bss     // Print, what's going on
        bsr _kprintf
        addq #4,sp

        /* make memory config valid */
        move.l  #0x752019f3, memvalid   // set memvalid to ok
        move.l  #0x237698aa, memval2    // set memval2 to ok

        /* restore old buserror handler */
       	move.l	a1, 0x00000008          // and put in our temporary handler

        bra	memdone                 // config done -> return to startup.s

// ==== Some messages ========================================================

msg_bss:
        .ascii "BIOS: Cleared RAM ...\n\0"


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end

