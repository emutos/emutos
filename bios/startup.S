/*
 * startup.S - Emulator specific startup module.  To be linked in first!
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *  LVL  Laurent Vogel
 *  THO  Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"

// ==== Defines ==============================================================

        .equ    SUPSIZ, 4096            // size of supervisor stack in words
        .equ    _GSX_ENT, 0             // Entry to GEM (if graphically)

        .equ    TPASTART, 0xE000        // default start address of tpa area

        .equ    vec_divnull, 0x14       // division by zero interrupt vector
        .equ    vec_linea, 0x28         // LineA interrupt vector
        .equ    vec_hbl, 0x68           // HBL interrupt vector
        .equ    vec_vbl, 0x70           // VBL interrupt vector
        .equ    vec_mfp, 0x78           // MFP interrupt vector
        .equ    vec_aes, 0x88           // AES interrupt vector

        .equ    vec_bios, 0xb4          // BIOS interrupt vector
        .equ    vec_xbios, 0xb8         // XBIOS interrupt vector
        .equ    vec_acia, 0x118         // keyboard/Midi interrupt vector



// ==== References ===========================================================

        .global _os_dosdate             // OS entry point
        .global _main           // OS entry point
        .global _GSX_ENT
        .global _trap_1         // Calling GEMDOS from C
        .global _b_mdx          // used in bios.c

        .global _printout

        .global _run

        .global _criter
	.global _criter1

        .global memdone         // return to, if memory config done

        .global _cartscan
        .global _os_end
        .global _os_beg
	.global _os_magic
        .global _int_vbl
        .global _int_hbl
        .global _bios
        .global _xbios
        .global _dummyaes
        .global _os_entry
        .global _just_rts
        .global _just_rte
        .global _xbios
        .global _diskbuf
        .global _brkpt
        
        .global _print_stat
        .global _print_vec
        .global _serial_stat
        .global _serial_vec
        .global _dump_scr
        
        .global _m_start
        .global _m_length

	.global _save_area
	.global _int_timera
	.global _int_timerb
	.global _int_timerc
	.global _int_timerd

// ==== From bios.c ==========================================================

        .xdef   _bios_0
        .xdef   _bios_1
        .xdef   _bios_2
        .xdef   _bios_3
        .xdef   _bios_4
        .xdef   _bios_6
        .xdef   _bios_7
        .xdef   _bios_8
        .xdef   _bios_9
        .xdef   _bios_a
        .xdef   _bios_b
        .xdef   _bios_c
        .xdef   _bios_d
        .xdef   _bios_e
        .xdef   _bios_f
        .xdef   _bios_10


// ==== tosvars.s - TOS System variables =====================================
                
        .xdef   etv_timer     
        .xdef   etv_critic    
        .xdef   resvalid      
        .xdef   resvector     
        .xdef   _timer_ms     

        .xdef   palmode       
        .xdef   defshiftmod   
        .xdef   sshiftmod      
        .xdef   _v_bas_ad     
        .xdef   vblsem        
        .xdef   nvbls          
        .xdef   _vblqueue     
        .xdef   colorptr      
        .xdef   screenpt      
        .xdef   _vbclock      
        .xdef   _frclock      

        .xdef   savptr        

        .xdef   _hz_200       

        .xdef   _kprint
	.xdef   _kprintf

        .xdef   _longframe
        
// ==== conout.s - Graphics output ===========================================

	.xdef	_blink
		
// ==== memory.s - variables for memory  =====================================

	.xdef	meminit       // initialize the memory and it's controller

// ==== Overall memory map ===================================================

	.xdef	bssstrt	      // address 0
	.xdef   bssstart      // end of documented low memory tosvars
        .xdef   _end          // end of bss + comm sections (ld generated)

	.xdef	_os_entry     // start of text section
	.xdef   _etext        // end of text section
	.xdef   _edata        // end of data section
	

// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================
        .bss


// save area for trap13/14.
save_beg:       ds.w    24*3    // d3-d7/a3-a7, sr, pc, vo = 24 words, 
				// multiplied by 3 ("3 level deep reentrant")
_save_area:                     // End of Save storage


// LVL - this .org directive can only be used in tosvars.S	
//        .org    0x167a
_diskbuf:       ds.b    1024    // 1 cluster disk buffer
//        .org    0x45b8
_stkbot:        ds.w    SUPSIZ  // Supervisor stack
_stktop:                        // filled from top to bottom
	
t1regsav:       dc.l    1   

_b_mdx:                         // initial memory descriptor
                dc.l    1
_m_start:       dc.l    1       // start address of TPA
_m_length:      dc.l    1       // length of TPA in byte
                dc.l    1

// ===========================================================================
// ==== TEXT segment (TOS image) =============================================
// ===========================================================================

        .text
        .org    0x000000




// ==== OSHEADER =============================================================

.global _shifty
.global _run

_os_entry:
    bra.s   _main       // os_entry, branch to _main
os_version:
    dc.w    0x0102      // os_version, TOS version
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
os_date:
    dc.l    0x14062001  // os_date, Date of system build
os_pal:        
    dc.w    0x0003      // Flag for PAL version (Only a word!)
_os_dosdate:
    dc.w    0x0c46      // _os_dosdate (wrong)
os_root:
    dc.l    0x0         // Pointer to the GEMDOS mem pool - not yet supported
os_kbshift:
    dc.l    _shifty     // Pointer to the keyboard shift keys states
os_run:
    dc.l    _run        // Pointer to a pointer to the actual basepage
os_dummy:
    dc.l    0           // ??? _main should start at 0xfc0030, shouldn't it?




// ==== Get into supervisor mode ==============================================
_main:                         // stunt to guarantee entry into supervisor mode
        move    #0x2700,sr     // disable interrupts

// ==== Reset all Hardware ====================================================

        reset                           // Reset all hardware

// ==== Check for diagnostic cartridge =======================================

        .equ    cart_base,      0xfffa0000

        cmp.l   #0xfa52235f, cart_base  // magic - is cartridge present?
        bne     nodiag                  // no -> go on
        lea     nodiag(pc), a6          // save return address
        jmp     cart_base+4             // execute diagnostig cartridge
nodiag:


// ==== Set up a supervisor stack ============================================

        lea     _stktop, sp             // Setup Supervisor Stack



// ==== Reset vector =========================================================
resetvec:
       cmpi.l 	#0x31415926, resvalid	// Jump to resetvector?
       bne.s 	noreset    		// No --> noreset
       move.l 	resvector, d0		// Yes: old resvec to d0
       tst.b 	resvector		// Is it valid?
       bne.s 	noreset                	// No --> noreset
       btst 	#0, d0			// Address odd ?
       bne.s 	noreset    		// Yes --> noreset
       movea.l  d0, a0			// resvec
       lea 	resetvec(pc), a6        // save return address
       jmp 	(a0)			// jump to resvec
noreset:

// ==== Check, if old memory config can be used ==============================

/* 
 * LVL - I put back the mem configuration early, to be able to use
 * the stack, so that the rest of boot sequence can be made more
 * modular using CDECL routines.
 */
	
        bra     meminit                 // do/test memory config (no sp used)
memdone:

/* 
 * From now, the stack is allowed, and C code can be used.
 *
 */

// ==== Rest of boot in bios.c ===============================================

        jmp     _startup


// ===========================================================================
// ==== dummy aes ============================================================
// ===========================================================================

//
// implements only appl_init(), returns 0, to tell Mint that GEM
// is not running.
//

_dummyaes:
        cmp.l #0xc8,d0
        bne failgem
        move.l d1,a0
        move.l (a0),a1
        cmp.w #0x0a,(a1)
        bne failgem
        move.l 4(a0),a1
        clr.l (a1)
        rte

failgemmsg:
        .ascii "unimplemented gem call.\n\0"
        .even
failgem:
        pea failgemmsg
        jsr _kprint
        addq #4,sp

everloop:	
        bra everloop            // Loop forever, if GEM call not implemented





// ===========================================================================
// ==== Subroutines ==========================================================
// ===========================================================================

// ==== Dummy functions ======================================================

_print_stat:
_print_vec:
_serial_stat:
_serial_vec:
_dump_scr:

_just_rts:       
        rts             // Just a dummy

// ==== just rte for divide by zero ==========================================
_just_rte:
        rte		



/* 
 * Int 0x68 - HBL interrupt 
 *
 */
 
_int_hbl:
        move.w  d0, -(sp)       // save d0
        move.w  2(sp), d0       // get status register from stack
        and.w   #0x0700, d0     // isolate just PIL-mask from sr
        bne     is_ipl0         // if IPL0, then end
        or.w    #0x0300, 2(sp)  // else set IPL3 in status register
        move.w  (sp)+, d0       // restore d0
is_ipl0:
        rte



/*
 * vidchng - change video resolution 
 *
 * Do nothing for now
 */

_vidchng:
        rts



// ==== Int 0x70 - VBL interrupt =============================================
// no video resolution change is done.

_int_vbl:
        addq.l  #1, _frclock            // increase num of happened ints
        subq.l  #1, _vblsem             // check vblsem
        bmi     vbl_end                 // if VBl routine disabled -> end
        
        movem.l d0-d7/a0-a6, -(sp)      // save registers
        addq.l  #1, _vbclock            // count number of VBL interrupts

	// detect rez change
	// (not done)

	bsr _blink                       // blink cursor

	// load new color palette
	move.l	_colorptr,d0
	beq	vbl_no_palette
	move.l	d0,a0
	lea	0xffff8240,a1
	move.w	#15,d0
vbl_palette_loop:
	move.w	(a0)+,(a1)+
	dbra	d0,vbl_palette_loop
vbl_no_palette:	

	// set new video address
	move.l	_screenpt,d0
	beq	vbl_no_screenpt
	move.l	d0,_v_bas_ad
	// move.b d0,0xffff820d
	lsr.w	#8,d0
	move.b	d0,0xffff8203
	swap	d0
	move.b	d0,0xffff8201
vbl_no_screenpt:	

	// flopvbl
	// bsr    _flopvbl                 // flopvbl routine not needed

	// vblqueue
	move.w	_nvbls,d0
	beq	vbl_no_queue
	sub.w	#1,d0
	move.l	_vblqueue,a0
vbl_queue_loop:	
	move.l	(a0)+,a1
	cmp.l	#0,a1
	beq	vbl_queue_next
	movem.l	d0/a0,-(sp)
	jsr	(a1)
	movem.l	(sp)+,d0/a0
vbl_queue_next:	
	dbra	d0,vbl_queue_loop
vbl_no_queue:
	
	// screen hardcopy
	// (not done) 

        movem.l (sp)+, d0-d7/a0-a6      // restore registers
vbl_end:
        addq.l  #1, _vblsem      //
        rte


// ==== breakpoint for illegal instruction ===================================
_brkpt:
        illegal
        bra.s   _brkpt  // never return

// ==== Timer A interrupt handler ============================================
_int_timera:
        rte             // Just a dummy

// ==== Timer B interrupt handler ============================================
_int_timerb:
        rte             // Just a dummy

// ==== Timer C interrupt handler ============================================
_int_timerc:
	
        addq.l  #1, _hz_200             // increment 200 Hz counter

	rol.w   _timer_c_sieve          // check for 4th call
	bpl	timerc_end

        movem.l d0-d7/a0-a6,-(sp)       // save registers	
        // TODO, repeat keys ?

	// dosound support
        jsr     _sndirq
	
        move.w  _timer_ms, -(sp)
        move.l  _etv_timer, a0
//        jsr     (a0)                    // jump to etc_timer routine
        addq.w  #2, sp                  // correct stack
        
        movem.l (sp)+,d0-d7/a0-a6
timerc_end:			
        bclr    #5, 0xfffffa11          // clear interrupt service bit
        rte
        

// ==== Timer D interrupt handler ============================================
_int_timerd:
        rte             // Just a dummy

// ==== txerror ==============================================================
_txerror:
        rte             // Just a dummy

// ==== txerror ==============================================================
_txrint:
        rte             // Just a dummy

// ==== txerror ==============================================================
_rxerror:
        rte             // Just a dummy

// ==== txerror ==============================================================
_rcvint:
        rte             // Just a dummy



// ==== Critical error handler ===============================================
// Just sets D0 (return code) to -1, end ends the subroutine

_criter:
        move.l  _criter, -(sp)  // etv_critic on stack
_criter1:
        moveq.l #-1, d0         // Default error
        rts                     // jump back to routine
        

// ==== STonX - Native print routine for debugging ===========================
_print:
_printout:
#if STONX_NATIVE_PRINT
        dc.w 0xa0ff     // Jump to native execution
        dc.l 0          // Printing subroutine
#endif
        rts



// ==== Use cartridge, if present ============================================
// get cartridge-type in d0, else use C glue.

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

        pea     msg_cart
        bsr     _kprint
        addq #4,sp

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
        
        


// ==== trap_1 - trap 1 (GEMDOS) entry point =================================

_trap_1:
        move.l  (sp)+,t1regsav  // save return address
        trap    #1              // call bdos call
        move.l  t1regsav,-(sp)  // restore return address
        rts

// ==== Trap 13 - BIOS entry point ==========================================

_bios:
        move.w  bios_ent,d1
        lea     bios_vecs,a0
        bra.s   biosxbios

// ==== Trap 14 - XBIOS entry point =========================================

_xbios:
        move.w  xbios_ent,d1
        lea     xbios_vecs,a0


// ==== Trap 13+14 handler ==================================================

biosxbios:
	move.l  _savptr, a1     
        move.w  (sp)+,d0        // Status register -> d0
        move.w  d0,-(a1)        // and save in save_area
        move.l  (sp)+,-(a1)     // save return address
//       tst.w   _longframe      // Check _longframe sysvariable one day...
//       beq.s   bx_nolongframe  // ...when we support CPU >=68000
//       move.w  (sp)+,-(a1)
bx_nolongframe:
        movem.l d3-d7/a3-a7, -(a1)      // regs, including stack pointer
        move.l  a1, _savptr
        
        btst    #13,d0          // were we in user mode?
        bne     bx_sp_ok        // yes, the sp already points to the arguments
        move.l  usp,sp          // no, the arguments were on the user stack
bx_sp_ok:
        move.l  #0,d0           // clear d0 (use d0 like the original TOS)
        move.w  (sp)+,d0        // remove the function number from stack
        cmp.w   d1,d0           // Higher, than highest number?
        bge     bx_ret_exc
        add.l   d0,d0           // indirection function table is 1 LW per
        add.l   d0,d0           // so multiply function number by 4
        add.l   d0,a0           // add to the base address of lookup table
        move.l  (a0),a0         // get the procedures address
        jsr     (a0)            // go do it and then come back

bx_ret_exc:
        move.l  _savptr, a1
        movem.l (a1)+, d3-d7/a3-a7      // Get regs back, including sp
//       tst.w   _longframe      // Check longframe again: Is CPU >= 68000?
//       beq.s   bx_nolong2
//       move.w  (a1)+,-(sp)
bx_nolong2:
        move.l  (a1)+,-(sp)
        move.w  (a1)+,-(sp)
        move.l  a1, _savptr
        rte                     // return with return value in D0

forever:
        jmp     forever




// ===========================================================================
// ==== DATA in text segment =================================================
// ===========================================================================

// ==== BIOS entry points to allow individual module compilation =============


bios_vecs:
        dc.l   _bios_0
        dc.l   _bios_1                 // LONG character_input_status()
        dc.l   _bios_2                 // LONG character_input()
        dc.l   _bios_3                 // void character_output()
        dc.l   _bios_4                 // LONG read_write_sectors()
        dc.l   _bios_5                 // set vector
        dc.l   _bios_6                 // LONG get_timer_ticks()
        dc.l   _bios_7                 // get disk parameter block address
        dc.l   _bios_8                 // LONG character_output_status(h)
        dc.l   _bios_9                 // media change?
        dc.l   _bios_a                 // what drives exist?
        dc.l   _bios_b                 // get/set alt-ctrl-shift status

bios_ent:               // Max. number of BIOS entries
        dc.w   (bios_ent-bios_vecs)/4


// ==== XBIOS entry points to allow individual module compilation ============

        .even
xbios_vecs:
        .dc.l   _xbios_0
        .dc.l   _xbios_1 
        .dc.l   _xbios_2 
        .dc.l   _xbios_3 
        .dc.l   _xbios_4 
        .dc.l   _xbios_5 
        .dc.l   _xbios_6 
        .dc.l   _xbios_7 
        .dc.l   _xbios_8 
        .dc.l   _xbios_9 
        .dc.l   _xbios_a 
        .dc.l   _xbios_b 
        .dc.l   _xbios_c
        .dc.l   _xbios_d
        .dc.l   _xbios_e
        .dc.l   _xbios_f
        .dc.l   _xbios_10
        .dc.l   _xbios_11
        .dc.l   _xbios_12
        .dc.l   _xbios_13
        .dc.l   _xbios_14
        .dc.l   _xbios_15
        .dc.l   _xbios_16
        .dc.l   _xbios_17
        .dc.l   _xbios_18
        .dc.l   _xbios_19
        .dc.l   _xbios_1a
        .dc.l   _xbios_1b
        .dc.l   _xbios_1c
        .dc.l   _xbios_1d
        .dc.l   _xbios_1e
        .dc.l   _xbios_1f
        .dc.l   _xbios_20
        .dc.l   _xbios_21
        .dc.l   _xbios_22
        .dc.l   _xbios_23
        .dc.l   _xbios_24
        .dc.l   _xbios_25
        .dc.l   _xbios_26
        .dc.l   _xbios_27

xbios_ent:              // Max. number of XBIOS entries
        .dc.w   (xbios_ent-xbios_vecs)/4


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end




