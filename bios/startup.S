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
        .global _sysbase
        .global _b_mdx          // used in bios.c

        .global _printout
        .global _output

        .global _s68
        .global _s68l
        .global _setjmp
        .global _longjmp
        .global _gouser
        .global _osif
        .global _xterm
        .global _oscall
        .global _run
        .global _tikfrk
        .global _fix_trap
        .global _fix_rte
        .global save_rt
        .global _tticks
        .global _bsetvec

        .global _bcli
        .global _bsti
        .global _cmain                  //file system init in main.c (cmain)
        .global _criter
	.global _criter1

        .global _drv_init
        .global _drv_boot
        .global _drv_rw
        .global _drv_bpb
        .global _drv_mediach

        .global   memdone       	// return to, if memory config done

        .global _cartscan
        .global _os_end
        .global _os_beg
        .global _int_vbl
        .global _int_hbl
        .global _bios
        .global _xbios
        .global _init_trap
        .global _dummyaes
        .global _os_entry
        .global _just_rts
        .global _just_rte
        .global _xbios
        .global _diskbuf
        .global _brkpt
        
        // useless ?? C code ??
        .global _print_stat
        .global _print_vec
        .global _serial_stat
        .global _serial_vec
        .global _prt_stat
        .global _prt_vec
        .global _aux_stat
        .global _aux_vec
        .global _dump_scr
        .global _dump_vec
        
        // TODO: move these into C code.
        .global _m_start
        .global _m_length

	.global _save_area
	.global _int_timerc

// ==== From bios.c ==========================================================

        .xdef   _biosinit
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

        .xdef   _m400_isr
        .xdef   _charvec
        .xdef   _osinit


// ==== tosvars.s - TOS System variables =====================================

	.xdef	bssstrt	
	.xdef   bssstart
	.xdef	_proc_lives	
	.xdef	_proc_dregs	
	.xdef	_proc_aregs	
	.xdef	_proc_enum	
	.xdef	_proc_usp	
	.xdef	_proc_stk	
                
        .xdef   etv_timer     
        .xdef   etv_critic    
        .xdef   etv_term      
        .xdef   etv_xtra      
        .xdef   memvalid      
        .xdef   memctrl       
        .xdef   resvalid      
        .xdef   resvector     
        .xdef   phystop       
        .xdef   _membot       
        .xdef   _memtop       
        .xdef   memval2       
        .xdef   flock          
        .xdef   seekrate      
        .xdef   _timer_ms     
        .xdef   _fverify      
        .xdef   _bootdev      
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
        .xdef   hdv_init      
        .xdef   swv_vec       
        .xdef   hdv_bpb        
        .xdef   hdv_rw        
        .xdef   hdv_boot      
        .xdef   hdv_mediach   
        .xdef   _cmdload      
        .xdef   conterm       
        .xdef   _themd          
        .xdef   ____md        
        .xdef   savptr        
        .xdef   _nflops       
        .xdef   con_state     
        .xdef   save_row      
        .xdef   sav_context   
        .xdef   _bufl          
        .xdef   _hz_200       
        .xdef   the_env       
        .xdef   _drvbits      
        .xdef   _dskbufp     
        .xdef   _autopath     
        .xdef   _vbl_list     
        .xdef   _dumpflg      
        .xdef   _sysbase      
        .xdef   _shell_p      
        .xdef   end_os        
        .xdef   exec_os       
        .xdef   dump_vec      
        .xdef   prt_stat      
        .xdef   prt_vec       
        .xdef   aux_stat      
        .xdef   aux_vec       
        .xdef   memval3       

        .xdef   _kprint

        .xdef   midivec
        .xdef   vkbderr
        .xdef   vmiderr
        .xdef   statvec
        .xdef   mousevec
        .xdef   clockvec
        .xdef   joyvec  
        .xdef   midisys
        .xdef   ikbdsys

        .xdef   _init_mfp
        .xdef   _init_timer
        .xdef   _init_usart
        .xdef   _bssend
        .xdef   _linea_init

        .xdef   _longframe
        
// ==== conout.s - Graphics output ===========================================

	.xdef	_blink
	
// ==== vectors.s - Default exception vectors ================================

	.xdef	_init_exc_vec
	.xdef	_init_user_vec
	
// ==== memory.s - variables for memory  =====================================

	.xdef	meminit         // initialize the memory and it's controller


// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================
        .bss


// save area for trap13/14.
save_beg:       ds.w    24*3    // d3-d7/a3-a7, sr, pc, vo = 24 words, 
				// multiplied by 3 ("3 level deep reentrant")
_save_area:                      // End of Save storage


        .org    0x167a
_diskbuf:       ds.b    1024    // 1 cluster disk buffer
        .org    0x45b8
_stkbot:        ds.w    SUPSIZ  // Supervisor stack
_stktop:                        // filled from top to bottom

_output:        ds.b    1024
t1regsav:       dc.l    1   

_b_mdx:                         // initial memory descriptor
                dc.l    1
_m_start:       dc.l    1       // start address of TPA
_m_length:      dc.l    1       // length of TPA in byte
                dc.l    1

_tticks:        dc.l    1

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
    dc.l    _bssend     // os_end, first byte RAM not used by OS
os_res1:        
    dc.l    _main       // os_res1, reserved
os_magic:
    dc.l    0x0         // os_magic, pointer to GEM's MUPB
os_date:
    dc.l    0x14062001  // os_date, Date of system build
os_pal:        
    dc.w    0x0003      // Flag for PAL version (Only a word!)
_os_dosdate:
    dc.w    0x0c46      // _os_dosdate (wrong)
os_root:
    dc.l    0x0         // Pointer to the GEMDOS memory pool - not yet supported
os_kbshift:
    dc.l    _shifty     // Pointer to the keyboard shift keys states
os_run:
    dc.l    _run        // Pointer to a pointer to the actual basepage
os_dummy:
    dc.l    0           // ??? _main should start at $fc030, shouldn't it?




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

// ==== Rest of boot in bios.c (experimental) ================================

#if 1
        jmp     _startup
#endif
	
// ==== Reset Soundchip, deselect floppies  ==================================

#if 0	/* old asm sound & flop init */
        lea     0xffff8800, a0  // base address of PSG Soundchip
        move.b  #7, (a0)        // port A and B          
        move.b  #0xC0, 2(a0)    // set to output           
        move.b  #0xE, (a0)      // port A                

        move.b  #7, 2(a0)       // deselect floppy
#else
	jsr     _snd_init
#endif

/*
 * this pre-screen initialisation before memory config is not needed
 * anymore. Do the full screen init in screen.c
 */	

#if 0   /* old asm screen pre-init */
	
// ==== Set shifter to pal ===================================================

        move.b #2, 0xff820a     // sync-mode to 50 hz pal, internal sync



// ==== Set color palette ====================================================

        lea    0xffff8240, a1   // video-shifter 
        move.w #0xf, d0         // loop for 16 colors
        lea    colorpal, a0     // color palette to a0
loadcol:
        move.w  (a0)+,(a1)+     // set color value         
        dbra   d0, loadcol      // next value   



// ==== Set temporary screenmem address 0x10000 to videoshifter ==============
        
        move.b  #0x1, 0xffff8201 // set hw video base high word
        move.b  #0x0, 0xffff8203 // set hw video base low word

#endif  /* old asm screen pre-init */

// ==== Check, if old memory config can be used ==============================

// moved above.	
//      bra     meminit                 // do/test memory config (no sp used)
// memdone:

//        pea msg_mem     // Print, what's going on
//        bsr _kprint
//        addq #4,sp



// ==== Set videoshifter address to screenmem ================================

#if 0	        
        move.l  _phystop, a0     // get memory top
        sub.l   #0x8000, a0     // minus screen mem length
        move.l  a0, _v_bas_ad   // set screen base

        move.b  _v_bas_ad+1, 0xffff8201 // set hw video base high word
        move.b  _v_bas_ad+2, 0xffff8203 // set hw video base low word


        pea msg_shift   // Print, what's going on
        bsr _kprint
        addq #4,sp
#else
	jsr _screen_init
#endif
	
// ==== Set memory width to sysvars ==========================================

        move.l  _os_end, _end_os        // end_os
        move.l  _end_os, _membot        // end_os to _membot
        move.l  _os_beg, _exec_os       // exec_os
        move.l  _v_bas_ad, _memtop      // _v_bas_ad to _memtop



// ==== Set memory config to initial memory descriptor =======================

	move.l  #TPASTART, _m_start     // set up default values

        move.l  _memtop, d0             // calculate length of TPA
        sub.l   #TPASTART, d0
        move.l  d0, _m_length           // set length of TPA in byte

        move.l  #_b_mdx, _themd         // write addr of MDB to sysvar themd
                                        // defined in bios.c


// ==== Set all cpu-interrupts to dummy handler ==============================
// We currently are experiencing an unexpected cpu interrupt.  We will
// set the vector addresses of these interrupts to a known location.

	jsr     _init_exc_vec           // LVL: moved in vectors.s



// ==== Set unassigned user interrupts to dummy handler ======================
//      move.l	SAVECT,-(sp)		// save software abort vector

	jsr     _init_user_vec          // LVL: moved in vectors.s

//      move.l	(sp)+,SAVECT		// restore software abort vector

//	pea msg_main	// Print, what's going on
//	bsr _kprint
//	addq #4,sp



// ==== Clear RAM ============================================================
//        move.l  _membot, a0             // Set start of RAM
//clrbss:
//        clr.w   (a0)+                   // Clear actual word
//        cmp.l   _memtop, a0             // End of BSS reached?
//        bne     clrbss                  // if not, clear next word
//
//        pea msg_clrbss  // Print, what's going on
//        bsr _kprint
//        addq #4,sp

// ==== vector setup =========================================================

        move.l #_int_vbl, vec_vbl       // Vbl-interr
        move.l #_int_hbl, vec_hbl       // Hbl-interr
        move.l #_dummyaes, vec_aes      // Trap #2  (AES, almost dummy)
        move.l #_bios, vec_bios         // Trap #13 (BIOS)
        move.l #_xbios, vec_xbios       // trap #14 (XBIOS)
        move.l #_int_linea, vec_linea   // Line-A
	jsr	_init_acia_vecs	        // LVL: moved to aciavecs.s
        move.l #_just_rte, vec_divnull  // Division by zero to rte

// ==== disk related vectors =================================================

#if 0	
        move.l  #_drv_init, hdv_init    // Initialize Harddrive
        move.l  #_drv_rw, hdv_rw        // Read/write sectors
        move.l  #_drv_bpb, hdv_bpb      // Get BIOS parameter Block
        move.l  #_drv_mediach, hdv_mediach      // Dummy mediach (STonX)
        move.l  #_drv_boot, hdv_boot    // Get boot device
        st      _fverify                // _fverify
        move.w  #0x3, seekrate          // floppy seekrate = 3 ms
        move.l  #_diskbuf, _dskbufp     // _dskbufp
#else
	jsr     _floppy_init
#endif
		
// ==== Some other vectors ===================================================

        move.l  #_print_stat, _prt_stat   //
        move.l  #_print_vec, _prt_vec     //
        move.l  #_serial_stat, _aux_stat  //
        move.l  #_serial_vec, _aux_vec    //
        move.l  #_dump_scr, _dump_vec     //

        move.w  #0x8, _nvbls            // nvbls
        move.w  #-1, _dumpflg           // clear _dumpflg
        move.l  #_os_entry, _sysbase    // Set _sysbase to ROM-start
        move.l  #_save_area, _savptr     // savptr for Trap dispatcher
	move.l	#_vidchng, _swv_vec	// vector for video mode change

        move.l  #_just_rts, _etv_timer   // etv_timer (->RTS)
        move.l  #_criter1, _etv_critic   // etv_critic
        move.l  #_just_rts, _etv_term    // etv_term  (->RTS)

        move.l  #_brkpt, 0x7c           // set nmi to do an illegal instruction
        move.l  #_bios, 0xb4            // revector bios entry = trap #13



// ==== Clear VBL queue list =================================================
        lea _vbl_list, a0               // Get addr. of VBL-routine
        move.l a0, _vblqueue            // to _vblqueue
        move.w #7, d0                   // Loop counter
clrvbl:
        clr.l (a0)+                     // Clear VBL-QUEUE
        dbra d0, clrvbl                 // Loop

        move.w #1, _vblsem              // do not execute vbl-routine


// ==== Test, if Cartridge of type 2 =========================================

        moveq.l #2, d0          // after interrupts are enabled
        bsr     cartscan

// ==== Detect and set graphics resolution ===================================

#if 0   /* done in screen_init */	
        move.b 0xffff8260, d0   // Get video resolution from pseudo Hw
        and.b #3,d0             // Isolate bits 0 and 1
        cmp.b #3,d0             // mode invalid?
        bne.s setscrnres        // yes, jump over
        moveq #2,d0             // invalid, set valid mode highres

setscrnres:
        move.b d0, sshiftmod    // Set in sysvar
        move.b d0, 0xffff8260     // Hardware set to highres

      	move.b  0xfffffa01, d0  // detect b/w-monitor pin via MFP, line I7
      	bmi     low_rez         // if bit set, then it is a color monitor 

        move.b #2, sshiftmod    // Set in sysvar
      	move.b #2, 0xffff8260   // and to shifter register
low_rez:

        cmp.b #1, sshiftmod             // middle resolution?
        bne.s initmidres                // nein, -->
        move.w 0xffff825e, 0xffff8246   // Copy Color 16->4 kopieren

initmidres:
#endif

        bsr _linea_init         // init linea variables

        move.l  #_main, _swv_vec // Set Swv_vec (vector res change) to Reset
        move.w  #1, _vblsem      // vblsem: enable VBL 

        pea msg_linea  // Print, what's going on
        bsr _kprint
        addq #4,sp



// ==== Now initialize the BIOS =============================================

        jsr     _biosinit                



        clr.l	d0                      // Cartridge of type 0
        bsr     cartscan

        move    #0x2300,sr              // enable Interrupts

        
// ==== Now initialize the BDOS =============================================

        jsr     _osinit                 // Initialize the GEMDOS

        moveq.l #1, d0              	// Cartridge of type 1
        bsr     cartscan



// ==== Test, if Cartridge of type 3 ========================================

        moveq.l #3,d0              // just before GEMDOS starts!
        bsr     cartscan



// ==== Now really start the BDOS ===========================================

        jsr     _biosmain       // go and start our shell


// ==== Get lost forever... =================================================
everloop:
        bra     everloop                        // Halt for debugging

        .even

msg_cart:
        .ascii "BIOS: Cartridge has been initialized ...\n\0"



// ===========================================================================
// ==== dummy aes ==========================================================
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
	// bsr    flopvbl                 // flopvbl routine not needed

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




// ==== Int 0x118 - exception for keyboard interrupt =========================
// LVL: moved to aciavec.s





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
        
        


// ==== Read/write sectors ===================================================
_drv_rw:
        pea drv_rwmsg
        bsr _kprint
        addq #4,sp
        rts             // Just a dummy

drv_rwmsg:
        .ascii "BIOS: hdv_rw - Native Disk read/write\n\0"
        .even



// ==== Get the BIOS parameter block =========================================
_drv_bpb:
        pea drv_bpbmsg  // Print, what's going on
        bsr _kprint
        addq #4,sp
        rts             // Just a dummy

drv_bpbmsg:
        .ascii "BIOS: hdv_bpb - Got native Bios Parameter Block for drive\n\0"
        .even



// ==== Init the Harddrive ===================================================
_drv_init:
        pea drv_initmsg
        bsr _kprint
        addq #4,sp
        rts             // Just a dummy

drv_initmsg:
        .ascii "BIOS: Do dummy drv_init - Init the Harddrive (fake)\n\0"
        .even



// ==== Did the media (Floppy) change? =======================================
_drv_mediach:   
        moveq #0,d0     // just a dummy 
        rts             // STonX can not change floppies (till now)



// ==== Boot from floppy/Disk ================================================
flopboot:
        move.l  _hdv_boot, a0   // Get floppy boot vector
        jsr     (a0)            // Load boot sector
        tst.w   d0              // Executable?
        bne     rtnflop         // no -> that's it!
        lea     _diskbuf, a0    // Get disk buffer
        jsr     (a0)            // Execute boot sector
rtnflop:        
        rts



// ==== Boot the Harddrive ===================================================
_drv_boot:      
        pea hdv_bootmsg
        bsr _kprint
        addq #4,sp

        move.w _bootdev, d0     // get boot device
        move d0,-(sp)
        move #0,-(sp)
        move #1,-(sp)
        move.l _dskbufp, -(sp)  // get pointer to 1k buffer for io
        move #0,-(sp)
        lea     _hdv_rw, a0      // Get routines address
        jsr (a0)
        lea 12(sp),sp
        moveq #4,d0
        rts

hdv_bootmsg:
        .ascii "BIOS: Do hdv_boot - Boot from specific drive\n\0"
        .even



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
// bx_nolongframe:
        movem.l d3-d7/a3-a7, -(a1)      // regs, including stack pointer
        move.l  a1, _savptr
        
        btst    #13,d0          // were we in user mode?
        bne     bx_sp_ok        // yes, the sp already points to the arguments
        move.l  usp,sp          // no, the arguments were on the user stack
bx_sp_ok:
        move.l  #0,d0           // clear d0 (use d0 like the original TOS does!)
        move.w  (sp)+,d0        // remove the function number from stack
        cmp.w   d1,d0           // Higher, than highest number?
        bge     bx_ret_exc
        add.l   d0,d0           // indirection function table is 1 LW per
        add.l   d0,d0           // so multiply function number by 4
        add.l   d0,a0           // add the offset to the base address of lookup table
        move.l  (a0),a0         // get the procedures address
        jsr     (a0)            // go do it and then come back

bx_ret_exc:
        move.l  _savptr, a1
        movem.l (a1)+, d3-d7/a3-a7      // Get regs back, including sp
//       tst.w   _longframe      // Check longframe again: Is CPU >= 68000?
//       beq.s   bx_nolong2
//       move.w  (a1)+,-(sp)
// bx_nolong2:
        move.l  (a1)+,-(sp)
        move.w  (a1)+,-(sp)
        move.l  a1, _savptr
        rte                     // return with return value in D0


// ==== Set userdefined exception vectors ====================================
// Function is trap13, function # 5 sets user defined exception vector

_bsetvec: 
        move    4(sp),a0        // discover the exception vector to set
        adda.l  a0,a0           // multiply that number by four cause 4 bytes
        adda.l  a0,a0           //       are needed to make up one address

        move.l  (a0),d0         // old vector to d0
        cmp.l   #-1,6(sp)       // Is argument = -1?
        beq     vsplt           // If yes, just return
        move.l  6(sp),(a0)      // set new vector
vsplt:  
        rts                     // return with d0 = old vector


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
        dc.l   _bios_c
        dc.l   _bios_d
        dc.l   _bios_e
        dc.l   _bios_f
        dc.l   _bios_10

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

        .even

// ==== Some messages ========================================================

msg_start:
        .ascii "BIOS: Starting up EmuTOS Ver. 0.0 ...\n"
        dc.w 0
msg_main:
        .ascii "BIOS: Entered supervisor mode...\n\0"
msg_mem:
        .ascii "BIOS: Initialized RAM ...\n\0"
msg_shift:
        .ascii "BIOS: Initialized shifter ...\n\0"
msg_linea:
        .ascii "BIOS: Linea set up and cleared screen ...\n\0"
msg_drvinit:
        .ascii "BIOS: Drives initialized ...\n\0"
msg_sound:
        .ascii "BIOS: Soundchip initialized ...\n\0"
msg_floppy:
        .ascii "BIOS: Floppy deselected ...\n\0"
msg_mfp:
        .ascii "BIOS: MFP initialized ...\n\0"
msg_gemdos:
        .ascii "BIOS: GEMDOS (not yet) initialized ...\n\0"
msg_shell:
        .ascii "BIOS: COMMAND.PRG loaded ...\n\0"
msg_halt:
        .ascii "BIOS: HALT - should never be reached!\n\0"
msg_vbl:
        .ascii "BIOS: VBL interrupt happened ...\n\0"

msg_test:
        .ascii "BIOS: Last test point reached ...\n\0"

msg_key:
        .ascii "BIOS: Key pressed or released ...\n\0"
        .even
        


int_test:
        dc.l    0x88888888
        dc.w    0
gemdosmsg:
        .ascii "GEMDOS call\n\0"
        .even

// ==== Videopalette for shifter =============================================
colorpal:
        dc.w    0x0777, 0x0707, 0x0070, 0x0770
        dc.w    0x0007, 0x0707, 0x0077, 0x0555
        dc.w    0x0333, 0x0733, 0x0373, 0x0773
        dc.w    0x0337, 0x0737, 0x0377, 0x0000

emptylong:
        dc.l    0
        dc.l    0
        dc.l    0
        dc.l    0

tiggle:
        .dc.b   0


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end




