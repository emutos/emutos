/*
 * tosvars.S - TOS System variables
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



	.global _vec_118

	.global bssstrt
	.global bssstart
	.global	_proc_lives
	.global	_proc_dregs	
	.global	_proc_aregs	
	.global	_proc_enum	
	.global	_proc_usp	
	.global	_proc_stk	
                
	.global	_etv_timer     
	.global	_etv_critic    
	.global	_etv_term      
	.global	_etv_xtra      
	.global	memvalid      
	.global	memctrl       
	.global	resvalid      
	.global	resvector     
	.global	_phystop       
	.global	_membot       
	.global	_memtop       
	.global	memval2       
	.global	_flock	       
	.global	_seekrate      
	.global	_timer_ms     
	.global	_fverify      
	.global	_bootdev      
	.global	_palmode       
	.global	_defshiftmod   
	.global	_sshiftmod      
	.global	_v_bas_ad     
	.global	_vblsem        
	.global	_nvbls	       
	.global	_vblqueue     
	.global	_colorptr      
	.global	_screenpt      
	.global	_vbclock      
	.global	_frclock      
	.global	_hdv_init      
	.global	_swv_vec       
	.global	_hdv_bpb        
	.global	_hdv_rw        
	.global	_hdv_boot      
	.global	_hdv_mediach   
	.global	_cmdload      
	.global	_conterm       
	.global	_themd	       
	.global	____md        
	.global	_savptr        
	.global	_nflops       
	.global	con_state     
	.global	save_row      
	.global	sav_context   
	.global	_bufl	       
	.global	_hz_200       
	.global	the_env       
	.global	_drvbits      
	.global	_dskbufp     
	.global	_autopath     
	.global	_vbl_list     
	.global	_dumpflg      
	.global	_sysbase      
	.global	_shell_p      
	.global	_end_os        
	.global	_exec_os       
	.global	_dump_vec      
	.global	_prt_stat      
	.global	_prt_vec       
	.global	_aux_stat      
	.global	_aux_vec       
	.global	memval3       
	.global	_bconstat_vec  
	.global	_bconin_vec    
	.global	_bcostat_vec   
	.global	_bconout_vec  
	.global	_longframe

	.global	_diskbuf	
	.global	_supstk 	

	.global rs232ibufbuf
	.global rs232obufbuf
	.global ikbdibufbuf
	.global midiibufbuf
	.global _rs232iorec
	.global rs232ibuf
	.global rs232ibufsz
	.global rs232ibufhd
	.global rs232ibuftl
	.global rs232ibuflo
	.global rs232ibufhi
	.global rs232obuf
	.global rs232obufsz
	.global rs232obufhd
	.global rs232obuftl
	.global rs232obuflo
	.global rs232obufhi
	.global _ikbdiorec
	.global ikbdibuf
	.global ikbdibufsz
	.global ikbdibufhd
	.global ikbdibuftl
	.global ikbdibuflo
	.global ikbdibufhi
	.global _midiiorec
	.global midiibuf
	.global midiibufsz
	.global midiibufhd
	.global midiibuftl
	.global midiibuflo
	.global midiibufhi

	.global _kbdvecs
	.global	midivec
	.global	vkbderr
	.global	vmiderr
	.global	statvec
	.global	mousevec
	.global	clockvec
	.global	joyvec	
	.global	midisys
	.global	ikbdsys

	.global	kbdlength
	.global kbdindex
	.global	kbdbuf
	.global joybuf

// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================


// ==== Beginning of RAM (used by OS) ========================================
	.bss
	.org 0x00000000 	// start of RAM
bssstrt:

// ==== Start of Exception related variables =================================
	.org	0x380
_proc_lives:	ds.l	1       // Validates system crash page, if 0x12345678
_proc_dregs:	ds.l    8	// Saved registers d0-d7
_proc_aregs:	ds.l	8	// Saved registers a0-a7
_proc_enum:	ds.l	1	// Vector number of crash exception
_proc_usp:	ds.l	1	// Saved user stackpointer
_proc_stk: 	ds.w	16	// 16 words from exception stack

// ==== Start of System variables ============================================
	.org 0x400		//
_etv_timer:      ds.l    1       //
_etv_critic:     ds.l    1
_etv_term:       ds.l    1
_etv_xtra:       ds.l    5
memvalid:       ds.l    1
	.org 0x424
memctrl:        ds.b    1
	.org 0x426
resvalid:       ds.l    1
resvector:      ds.l    1
_phystop:       ds.l    1
_membot:        ds.l    1
_memtop:        ds.l    1
memval2:        ds.l    1
_flock:	        ds.w    1
_seekrate:      ds.w    1
_timer_ms:      ds.w    1
_fverify:       ds.w    1
_bootdev:       ds.w    1
_palmode:       ds.w    1
_defshiftmod:   ds.w    1
_sshiftmod:     ds.b    1
	.org 0x44e
_v_bas_ad:      ds.l    1	// screen base address  
_vblsem:         ds.w    1
_nvbls:	        ds.w    1
_vblqueue:      ds.l    1
_colorptr:      ds.l    1
_screenpt:      ds.l    1
_vbclock:       ds.l    1
_frclock:       ds.l    1
_hdv_init:      ds.l    1
_swv_vec:        ds.l    1
_hdv_bpb:       ds.l    1
_hdv_rw:        ds.l    1
_hdv_boot:      ds.l    1
_hdv_mediach:   ds.l    1
_cmdload:       ds.w    1
_conterm:       ds.b    1
	.org 0x48e
_themd:	        ds.l    4
____md:         ds.w    2
_savptr:        ds.l    1
_nflops:        ds.w    1
con_state:      ds.l    1
save_row:       ds.w    1
sav_context:    ds.l    1
_bufl:	        ds.l    2
_hz_200:        ds.l    1
the_env:        ds.b    4
	.org 0x4c2
_drvbits:       ds.l    1
_dskbufp:       ds.l    1
_autopath:      ds.l    1
_vbl_list:      ds.l    1
		ds.l    1
		ds.l    1
		ds.l    1
		ds.l    1
		ds.l    1
		ds.l    1
		ds.l    1
_dumpflg:       ds.w    1
		ds.w    1     // dummy so that _sysbase == 0x4F2
_sysbase:       ds.l    1
_shell_p:       ds.l    1
_end_os:        ds.l    1
_exec_os:       ds.l    1

	.org 0x502
_dump_vec:      ds.l    1       //Pointer to screen dump routine
_prt_stat:      ds.l    1       //prv_lsto
_prt_vec:       ds.l    1       //prv_lst
_aux_stat:      ds.l    1       //prv_auxo
_aux_vec:       ds.l    1       //prv_aux
pun_ptr:        ds.l    1       // If AHDI, pointer to pun_info  
memval3:        ds.l    1       //If $5555AAAA, reset
_bconstat_vec:  ds.l    8       //8 Pointers to input-status routines
_bconin_vec:    ds.l    8       //8 Pointers to input routines
_bcostat_vec:   ds.l    8       //8 Pointers to output-status routines
_bconout_vec:   ds.l    8       //8 Pointers to output routines
_longframe:     ds.w    1       //If not 0, then not 68000 - use long stack frames
_p_cookies:     ds.l    1       //Pointer to cookie jar
ramtop:         ds.l    1       //Pointer to end of FastRam
ramvalid:       ds.l    1       //Validates ramtop if $1357BD13
bell_hook:      ds.l    1       //Pointer to routine for system bell
kcl_hook:       ds.l    1       //Pointer to routine for system keyclick
	.org 0x5b4


// ==== IOREC BUFFERS ======================================================
// Table of input-output buffersfor rs232 in, rs232 out, kdb in, midi in
	.org	0xa0e
rs232ibufbuf:	
	.org	0xb0e
rs232obufbuf:
	.org	0xc0e
ikbdibufbuf:
	.org	0xd0e
midiibufbuf:	
	.org	0xd8e

// ==== IORECS =============================================================
// Table of input-output records for rs232 in, rs232 out, kdb in, midi in
	.org	0xd8e
_rs232iorec:
rs232ibuf:	ds.l	1
rs232ibufsz:	ds.w	1
rs232ibufhd:	ds.w	1
rs232ibuftl:	ds.w	1
rs232ibuflo:	ds.w	1
rs232ibufhi:	ds.w	1
rs232obuf:	ds.l	1
rs232obufsz:	ds.w	1
rs232obufhd:	ds.w	1
rs232obuftl:	ds.w	1
rs232obuflo:	ds.w	1
rs232obufhi:	ds.w	1
_ikbdiorec:
ikbdibuf:	ds.l	1
ikbdibufsz:	ds.w	1
ikbdibufhd:	ds.w	1
ikbdibuftl:	ds.w	1
ikbdibuflo:	ds.w	1
ikbdibufhi:	ds.w	1
_midiiorec:
midiibuf:	ds.l	1
midiibufsz:	ds.w	1
midiibufhd:	ds.w	1
midiibuftl:	ds.w	1
midiibuflo:	ds.w	1
midiibufhi:	ds.w	1

	
// ==== KBDVBASE =============================================================
// Table of routines for managing midi and keyboard data
// in packets from IKBD (shown by A0 und 4(sp))
// (see aciavecs.S)
	.org 0xdcc
_kbdvecs:	
midivec:	ds.l	1	// MIDI input
vkbderr:	ds.l	1	// keyboard error
vmiderr:	ds.l	1	// MIDI-Error
statvec:	ds.l	1	// IKBD-Status
mousevec:	ds.l	1	// mouse-routine
clockvec:	ds.l	1	// time-routine
joyvec:		ds.l	1	// joystick-routinee
midisys:	ds.l	1	// MIDI-systemvector
ikbdsys:	ds.l	1	// IKBD-systemvector
kbdlength:	ds.b	1	// kbd length of packet
kbdindex:	ds.b	1	// kbd index of next byte of packet 
kbdbuf:		ds.b	8	// kbd buffer for packet
joybuf:         ds.b    3       // kbd buffer for joystick
	.even

	
// memory past this address will be cleared on startup or on reset.
bssstart:

// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

	.end
