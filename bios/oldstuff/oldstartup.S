/*
 * oldstartup.S - old assembler stuff, taken out after new C implementation
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




	
/*
 *  below, the dummy drv_* calls, now replaced by flop_hdv_* 
 *
 */ 
	

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

/*
 *  unused bios functions
 *
 */

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


//        dc.l   _bios_c
//        dc.l   _bios_d
//        dc.l   _bios_e
//        dc.l   _bios_f
//        dc.l   _bios_10

/*
 *  unused messages
 *
 */

	
msg_start:
        .ascii "BIOS: Starting up EmuTOS Ver. 0.0 ...\n\0"
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
        
gemdosmsg:
        .ascii "GEMDOS call\n\0"
        .even

/*
 *  unused data
 *
 */

// ==== Videopalette for shifter =============================================
colorpal:
        dc.w    0x0777, 0x0707, 0x0070, 0x0770
        dc.w    0x0007, 0x0707, 0x0077, 0x0555
        dc.w    0x0333, 0x0733, 0x0373, 0x0773
        dc.w    0x0337, 0x0737, 0x0377, 0x0000

int_test:
        dc.l    0x88888888
        dc.w    0

emptylong:
        dc.l    0
        dc.l    0
        dc.l    0
        dc.l    0

tiggle:
        .dc.b   0

