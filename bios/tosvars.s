| ===========================================================================
| ==== tosvars.s - TOS System variables
| ===========================================================================


	.global _vec_118

	.global bssstrt
	.global	proc_lives
	.global	proc_dregs	
	.global	proc_aregs	
	.global	proc_enum	
	.global	proc_usp	
	.global	proc_stk	
                
	.global	etv_timer     
	.global	etv_critic    
	.global	etv_term      
	.global	etv_xtra      
	.global	memvalid      
	.global	memctrl       
	.global	resvalid      
	.global	resvector     
	.global	phystop       
	.global	_membot       
	.global	_memtop       
	.global	memval2       
	.global	flock	       
	.global	seekrate      
	.global	_timer_ms     
	.global	_fverify      
	.global	_bootdev      
	.global	palmode       
	.global	defshiftmod   
	.global	sshiftmod      
	.global	_v_bas_ad     
	.global	vblsem        
	.global	nvbls	       
	.global	_vblqueue     
	.global	colorptr      
	.global	screenpt      
	.global	_vbclock      
	.global	_frclock      
	.global	hdv_init      
	.global	swv_vec       
	.global	hdv_bpb        
	.global	hdv_rw        
	.global	hdv_boot      
	.global	hdv_mediach   
	.global	_cmdload      
	.global	conterm       
	.global	themd	       
	.global	____md        
	.global	savptr        
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
	.global	end_os        
	.global	exec_os       
	.global	dump_vec      
	.global	prt_stat      
	.global	prt_vec       
	.global	aux_stat      
	.global	aux_vec       
	.global	memval3       
	.global	bconstat_vec  
	.global	bconin_vec    
	.global	bcostat_vec   
	.global	bconout_vec  

	.global	diskbuf	
	.global	_supstk 	

	.global	midivec
	.global	vkbderr
	.global	vmiderr
	.global	statvec
	.global	mousevec
	.global	clockvec
	.global	joyvec	
	.global	midisys
	.global	ikbdsys
        
| ===========================================================================
| ==== BSS segment ==========================================================
| ===========================================================================


| ==== Beginning of RAM (used by OS) ========================================
	.bss
	.org 0x00000000 	| start of RAM
bssstrt:

| ==== Start of Exception related variables =================================
	.org	0x380
proc_lives:	ds.l	1       | Validates system crash page, if 0x12345678
proc_dregs:	ds.l    8	| Saved registers d0-d7
proc_aregs:	ds.l	8	| Saved registers a0-a7
proc_enum:	ds.l	1	| Vector number of crash exception
proc_usp:	ds.l	1	| Saved user stackpointer
proc_stk: 	ds.w	16	| 16 words from exception stack

| ==== Start of System variables ============================================
	.org 0x400		|
etv_timer:      ds.l    1       |
etv_critic:     ds.l    1
etv_term:       ds.l    1
etv_xtra:       ds.l    5
memvalid:       ds.l    1
	.org 0x424
memctrl:        ds.b    1
	.org 0x426
resvalid:       ds.l    1
resvector:      ds.l    1
phystop:        ds.l    1
_membot:        ds.l    1
_memtop:        ds.l    1
memval2:        ds.l    1
flock:	        ds.w    1
seekrate:       ds.w    1
_timer_ms:      ds.w    1
_fverify:       ds.w    1
_bootdev:       ds.w    1
palmode:        ds.w    1
defshiftmod:    ds.w    1
sshiftmod:     	ds.b    1
	.org 0x44e
_v_bas_ad:      ds.l    1	| screen base address  
vblsem:         ds.w    1
nvbls:	        ds.w    1
_vblqueue:      ds.l    1
colorptr:       ds.l    1
screenpt:       ds.l    1
_vbclock:       ds.l    1
_frclock:       ds.l    1
hdv_init:       ds.l    1
swv_vec:        ds.l    1
hdv_bpb:        ds.l    1
hdv_rw:         ds.l    1
hdv_boot:       ds.l    1
hdv_mediach:    ds.l    1
_cmdload:       ds.w    1
conterm:        ds.b    1
	.org 0x48e
themd:	        ds.l    4
____md:         ds.w    2
savptr:         ds.l    1
_nflops:        ds.w    1
con_state:      ds.l    1
save_row:       ds.w    1
sav_context:    ds.l    1
_bufl:	        ds.l    2
_hz_200:        ds.l    1
the_env:        ds.b    4
	.org 0x4c2
_drvbits:       ds.l    1
_dskbufp:      ds.l    1
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
		ds.w    1     | dummy so that _sysbase == 0x4F2
_sysbase:       ds.l    1
_shell_p:       ds.l    1
end_os:         ds.l    1
exec_os:        ds.l    1
dump_vec:       ds.l    1
prt_stat:       ds.l    1
prt_vec:        ds.l    1
aux_stat:       ds.l    1
aux_vec:        ds.l    1
memval3:        ds.l    1
bconstat_vec:   ds.l    8
bconin_vec:     ds.l    8
bcostat_vec:    ds.l    8
bconout_vec:    ds.l    8

| ==== KBDVBASE =============================================================
| Table of routines for managing midi and keyboard data
| in packets from IKBD (shown by A0 und 4(sp)) 
	.org 0xdcc
midivec:	ds.l	1	| MIDI input
vkbderr:	ds.l	1	| keyboard error
vmiderr:	ds.l	1	| MIDI-Error
statvec:	ds.l	1	| IKBD-Status
mousevec:	ds.l	1	| mouse-routine
clockvec:	ds.l	1	| time-routine
joyvec:		ds.l	1	| joystick-routinee
midisys:	ds.l	1	| MIDI-systemvector
ikbdsys:	ds.l	1	| IKBD-systemvector



| ===========================================================================
| ==== End ==================================================================
| ===========================================================================

	.end
