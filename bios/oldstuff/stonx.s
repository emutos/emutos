
| ===========================================================================
| ==== stonx.a - Emulator specific startup module.  To be linked in first!
| ===========================================================================

| ==== Defines ==============================================================

	.equ	SUPSIZ, 2048		| size of the supervisor stack (also in bios.c)
	.equ	_GSX_ENT, 0		| Entry to GEM (if graphically)

	.equ	TPASTART, 0xE000	| start address of tpa area
	.equ	TPALENGTH, 0x30000-TPASTART | length of tpa area - from 0x30000 down to TPASTART

	.equ	SAVECT, 0x138		| software abort vector


| ==== References ===========================================================

       	.xref	_main		| OS entry point
       	.xref	_GSX_ENT
       	.xref	_trap_1		| Calling GEMDOS from C
	.xref	_sysbase
	.global	_printout
        .global	_output

|#if	OLDCLOCK
|       	.globl	_ti
|       	.globl	_x1
|#else
|       	.xdef	_cl_init	|  vmetimer.c
|       	.xdef	_cl_intr	|  vmetimer.c
|#endif
       	.globl	_s68
       	.globl	_s68l
       	.globl	_setjmp
       	.globl	_longjmp
       	.globl	_gouser
       	.globl	_osif
       	.globl	_xterm
       	.globl	_oscall
       	.globl	_run
       	.globl	_tikfrk
       	.globl	_fix_trap
       	.globl	_fix_rte
       	.globl	save_rt
       	.globl	_b_mdx
       	.globl	_tticks
       	.globl	_bsetvec

       	.globl	_bcli
       	.globl	_bsti
       	.globl	_cmain			|file system init in main.c (cmain)
       	.globl	_criter


| ==== VME/10 Specifics =============
       	.globl	_no_device
       	.globl	_rddat
       	.globl	_wrdat
       	.globl	_supstk

| ==== From bios.c ==========================================================

       	.xdef	_biosinit
       	.xdef	_bios_0
       	.xdef	_bios_1
       	.xdef	_bios_2
       	.xdef	_bios_3
       	.xdef	_bios_4
       	.xdef	_bios_6
       	.xdef	_bios_7
       	.xdef	_bios_8
       	.xdef	_bios_9
       	.xdef	_bios_a
       	.xdef	_bios_b
       	.xdef	_bios_c
       	.xdef	_bios_d
       	.xdef	_bios_e
       	.xdef	_bios_f
       	.xdef	_bios_10

       	.xdef	_m400_isr
       	.xdef	_charvec

|#if	INTRGLITCH
|       	.xdef	_nullint
|#endif

| ==== tosvars.s - TOS System variables =====================================

	.xdef	bssstrt	
	.xdef	proc_lives	
	.xdef	proc_dregs	
	.xdef	proc_aregs	
	.xdef	proc_enum	
	.xdef	proc_usp	
	.xdef	proc_stk	
                
	.xdef	etv_timer     
	.xdef	etv_critic    
	.xdef	etv_term      
	.xdef	etv_xtra      
	.xdef	memvalid      
	.xdef	memctrl       
	.xdef	resvalid      
	.xdef	resvector     
	.xdef	phystop       
	.xdef	_membot       
	.xdef	_memtop       
	.xdef	memval2       
	.xdef	flock	       
	.xdef	seekrate      
	.xdef	_timer_ms     
	.xdef	_fverify      
	.xdef	_bootdev      
	.xdef	palmode       
	.xdef	defshiftmod   
	.xdef	sshiftmod      
	.xdef	_v_bas_ad     
	.xdef	vblsem        
	.xdef	nvbls	       
	.xdef	_vblqueue     
	.xdef	colorptr      
	.xdef	screenpt      
	.xdef	_vbclock      
	.xdef	_frclock      
	.xdef	hdv_init      
	.xdef	swv_vec       
	.xdef	hdv_bpb        
	.xdef	hdv_rw        
	.xdef	hdv_boot      
	.xdef	hdv_mediach   
	.xdef	_cmdload      
	.xdef	conterm       
	.xdef	themd	       
	.xdef	____md        
	.xdef	savptr        
	.xdef	_nflops       
	.xdef	con_state     
	.xdef	save_row      
	.xdef	sav_context   
	.xdef	_bufl	       
	.xdef	_hz_200       
	.xdef	the_env       
	.xdef	_drvbits      
	.xdef	_dskbufp     
	.xdef	_autopath     
	.xdef	_vbl_list     
	.xdef	_dumpflg      
	.xdef	_sysbase      
	.xdef	_shell_p      
	.xdef	end_os        
	.xdef	exec_os       
	.xdef	dump_vec      
	.xdef	prt_stat      
	.xdef	prt_vec       
	.xdef	aux_stat      
	.xdef	aux_vec       
	.xdef	memval3       
	.xdef	bconstat_vec  
	.xdef	bconin_vec    
	.xdef	bcostat_vec   
	.xdef	bconout_vec  

	.xdef	_kprint

	.xdef	midivec
	.xdef	vkbderr
	.xdef	vmiderr
	.xdef	statvec
	.xdef	mousevec
	.xdef	clockvec
	.xdef	joyvec	
	.xdef	midisys
	.xdef	ikbdsys


| ==== gsxvars.s - Graphics subsystem variables =============================
	.xdef	_v_bas_ad      

| ===========================================================================
| ==== BSS segment ==========================================================
| ===========================================================================
	.bss


save_beg:	ds.l	23	| Save storage for trap dispatcher        
save_area:		  	| End of Save storage

save_sp:        ds.l	1       | save stack pointer (within trap13)
save_rt:	ds.l	1	| save return address (within trap13)
save_sr:        ds.w	1       | save status register (within trap13)
        

        .org	0x167a
diskbuf:	ds.b	1024	| 1 cluster disk buffer
        .org	0x45b8
_stkbot: 	ds.b	SUPSIZ	| Supervisor stack
_stktop:                        | filled from top to bottom

_output:	ds.b	1024
t1regsav:	  dc.l	  1   

_b_mdx:
		.dc.l	1
m_start:	.dc.l	1
m_length:	.dc.l	1
		.dc.l	1

_tticks:	.dc.l	1

sysvarend:

| ===========================================================================
| ==== TEXT segment (TOS image) =============================================
| ===========================================================================

       	.text
	.org	0x000000




| ==== OSHEADER =============================================================

os_entry:
	bra.s	_main		| os_entry, branch to _main
os_version:
	dc.b   1,0		| os_version, TOS version
reseth:
	dc.l 	_main		| reseth, pointer to reset handler
os_beg:
	dc.l 	os_entry	| os_beg, base of os = _sysbase
os_end:	
	dc.l 	sysvarend	| os_end, first byte RAM not used by OS
os_res1:        
	dc.l 	_main		| os_res1, reserved
os_magic:
	dc.l 	0x0		| os_magic, pointer to GEM's MUPB
os_date:
	dc.l 	0x14062001      | os_date, Date of system build
os_pal:        
	dc.l 	0x0003    	| Flag for PAL version
os_dosdate:
	dc.w 	0x0c46    	| os_dosdate (wrong)


| ==== Get into supervisor mode ==============================================
_main:					| stunt to guarantee entry into supervisor mode
	move	#0x2700,sr		| disable interrupts
	reset				| Reset all hardware

| ==== Set up a supervisor stack ============================================
       	lea	_stktop+SUPSIZ, sp	| Setup Supervisor Stack

	pea msg_start	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== Reset peripherals =====================================================
	reset

| ==== Set all cpu-interrupts to dummy handler ==============================
| We currently are experiencing an unexpected cpu interrupt.  We will
| set the vector addresses of these interrupts to a known location.

	move.l	#0, a0
	move.l	#just_rte, a1		| 
xresintr:
	move.l	a1, (a0)+                | set vector, increase pointer
	cmp.l	#0x100, a0               | Last vector reached?
	blt	xresintr                | if not, take next vector



| ==== Set unassigned user interrupts to dummy handler ======================
|       	move.l	SAVECT,-(sp)		| save software abort vector

       	move.l	#0x100, a0		| start with 1st user vector
       	move.l	#just_rte, a1		| address of dummy interupt handler
resintr: 
       	move.l	a1, (a0)+		| set new vector and bump ptr
       	cmp.l	#0x400, a0		| have we reached #0x400?
       	blt	resintr			| if not, get next user vector

|       	move.l	(sp)+,SAVECT		| restore software abort vector

	pea msg_main	| Print, what's going on
	bsr _kprint
	addq #4,sp


| ==== Set memory (hard for now) ============================================

	clr.l  	d6
	move.b 	#0x0f, d6		| set hw to 2 banks by 2 mb
	move.b 	d6, 0xffff8001		| set hw to 2 banks by 2 mb
        move.b 	d6, memctrl		| set copy of hw memory config
        move.l 	#0x400000, phystop      | set memory to 4 mb
        move.l 	#0x752019f3, memvalid   | set memvalid to ok
        move.l 	#0x237698aa, memval2	| set memval2 to ok


	pea msg_mem	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== Set videoshifter to PAL ==============================================
	move.b #2, 0xff820a   | sync-mode to 50 hz pal, internal sync
        
|        lea 	0xff8240, a1  | video-shifter 
|        move.w 	#0xf, d0        | set 16 colors
|        lea 	colorpal, a0    | color palette to a0
|loadcol:
|	move.w 	(a0)+,(a1)+     | set color value         
|        dbra 	d0, loadcol     | next value   


| ==== Detect and set graphics resolution ===================================

        move.b 0xff8260, d0	| Get video resolution from pseudo Hw
        and.b #3,d0		| Isolate bits 0 and 1
        cmp.b #3,d0		| Bits 0,1 set = invalid
        bne.s setscrnres	| no -->
        moveq #2,d0		| yes, set highres, make valid
setscrnres:
        move.b d0, sshiftmod    | Set in sysvar
|        lea 0xfc0376,a6	| save return address
|        bra 0xfc0ce4           | Jump to MFP init

        move.b #2,0xff8260	| Hardware set to highres
        move.b #2, sshiftmod    | Set in sysvar
|        jsr 0xfca7c4		| Init screen (video driver???)

|        cmp.b #1, sshiftmod     	| middle resolution?
|        bne.s initmidres	   	| nein, -->
|        move.w 0xff825e, 0xff8246  	| Copy Color 16->4 kopieren

initmidres:
	move.l 	#_main, swv_vec  | Set Swv_vec (vector res change) to Reset
        move.w 	#1, vblsem 	   | vblsem: VBL freigeben


|	pea 0xfffffa00	| Print, what's going on
|	bsr _kputb
|	addq #4,sp
|
|	btst 	#7,0xfffffa01	| detect b/w-monitor pin
|	beq 	low_rez         | if bit set, color monitor 
|
|	move.l 	#2,d0           | monochrome mode
|	bra 	both_rez
|low_rez:
|	move.l #0,d0
|both_rez:
|	move.b d0, sshiftmod    | set mode sysvar
|	move.w d0, 0xFFFF8260   | and to shifter register

| ==== Set videoshifter address to screenmem ================================
	
        move.l	phystop, a0	| get memory top
	sub.l	#0x8000, a0	| minus screen mem length
        move.l	a0, _v_bas_ad	| set screen base

        move.b 	_v_bas_ad+1, 0xffff8201	| set hw video base high word
        move.b 	_v_bas_ad+2, 0xffff8203 | set hw video base low word

	pea msg_shift	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== STonX linea initialisation ===========================================
|	dc.w 0xa000	|Init own linea functions

	dc.w 0xa0ff
	dc.l 10 	|Init_Linea (does nothing)

	pea msg_linea	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== Set memory width to sysvars ==========================================
        move.l	os_end, end_os		| end_os
        move.l	os_beg, exec_os		| exec_os
	move.l 	_v_bas_ad, _memtop	| _v_bas_ad to _memtop
	move.l 	end_os, _membot		| end_os to _membot


| ==== Clear RAM ============================================================
       	move.l	_membot, a0            	| Set start of RAM
clrbss:
       	clr.w	(a0)+                   | Clear actual word
       	cmp.l	_memtop, a0             | End of BSS reached?
       	bne	clrbss	                | if not, clear next word

	pea msg_clrbss	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== Clear screen =========================================================
       	move.l	_memtop, a0            	| Set start of RAM
clrscn:
       	clr.w	(a0)+                   | Clear actual word
       	cmp.l	phystop, a0             | End of BSS reached?
       	bne	clrscn	                | if not, clear next word

	pea msg_clrscn	| Print, what's going on
	bsr _kprint
	addq #4,sp

|	pea phystop	| Print, what's going on
|	bsr _kputl
|	addq #4,sp




| ==== STonX basic initialisation ===========================================
| Simply saves the original (now dummy) BIOS disk related vectors for later 
| use in  native routines - pc is later set to it

	move.l 	#just_rts, hdv_init	| dummy Initialize Harddrive
	move.l 	#just_rts, hdv_rw	| dummy Read/write sectors
	move.l 	#just_rts, hdv_bpb	| dummy Get BIOS parameter Block
	move.l 	#just_rts, hdv_mediach	| dummy Dummy mediach (STonX)
	move.l 	#just_rts, hdv_boot	| dummy Get boot device

	dc.w 0xa0ff 	| Jump to native execution
	dc.l 9		| Basic Initialization - grep these dummys


| ==== STonX disk related vectors ===========================================
	move.l 	#drv_init, hdv_init		| Initialize Harddrive
	move.l 	#drv_rw, hdv_rw			| Read/write sectors
	move.l 	#drv_bpb, hdv_bpb		| Get BIOS parameter Block
	move.l 	#drv_mediach, hdv_mediach	| Dummy mediach (STonX)
	move.l 	#drv_boot, hdv_boot		| Get boot device


| ==== Disk Initialization ==================================================
dinit:
|	dc.w 0xa0ff 	| Jump to native execution
|	dc.l 3
|	dc.w 0xa000

	pea msg_drvinit	| Print, what's going on
	bsr _kprint
	addq #4,sp



| ==== Reset vector =========================================================
resetvec:
       cmpi.l 	#0x31415926, resvalid	| Jump to resetvector?
       bne.s 	initsnd			| No --> initsnd 
       move.l 	resvector, d0		| Yes: resvec to d0
       tst.b 	resvector		| Is it valid?
       bne.s 	initsnd			| No --> initsnd
       btst 	#0, d0			| Address odd ?
       bne.s 	initsnd			| Yes --> initsnd
       movea.l d0, a0			| resvec
       lea 	resetvec(pc), a6        | save return address
       jmp 	(a0)			| jump to resvec



| ==== Reset Soundchip =======================================================
initsnd:
	lea 	0xffff8800, a0	| base address of PSG Soundchip
	move.b 	#7, (a0)        | port A and B          
	move.b 	#0xC0, 2(a0)    | set to output           
	move.b 	#0xE, (a0)      | port A                

	pea msg_sound	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== Reset Floppy =========================================================
	move.b 	#7, 2(a0)       | deselect floppy

	pea msg_floppy	| Print, what's going on
	bsr _kprint
	addq #4,sp


| ==== Reset MFP ============================================================
| zero almost all MFP registers.

init_mfp:
        lea 	0xfffa01, a0      | base address of MFP
        move.l 	#0, d0
        movep.l	d0, 0(a0)	| clear gpip, aer, ddr, iera
        movep.l	d0, 8(a0)	| clear ierb, ipra, iprb, isra
        movep.l	d0, 0x10(a0)	| clear isrb, imra, imrb, vr
        move.b 	#0x48, 0x16(a0)	| vr
        movep.l d0, 0x18(a0)	| clear tacr, tbcr, tcdcr, tadr
        movep.l d0, 0x20(a0)	| clear tbdr, tcdr, tddr, scr

  fc21ce     move.w #0x1111, 0xe42	| set timer C int to every 4th IRQ
  fc21d4     move.w #0x14, _timer_ms	| set timer to 20 ms 
  fc21da     moveq #2, d0		| select timer C
  fc21dc     moveq #0x50, d1		| divide by 64 for 200 hz
  fc21de     move.w #0xc0, d2		| 192 
  fc21e2     bsr init_timer

  fc21e6     lea int_timerc, a2	| 
  fc21ec     moveq #5, d0                        
  fc21ee     bsr mfpint1
  fc21f2     moveq #3, d0                        
  fc21f4     moveq #1, d1                        
  fc21f6     moveq #2, d2                        
  fc21f8     bsr init_timer		| init timer vectors
  fc21fc     move.l #0x980101, d0                 
  fc2202     movep.l d0, 0x26(a0)                 
|  fc2206     bsr 0xfc2d8c		| set DTR to on
|  fc220a     bsr 0xfc2d84               | set RTS to on          

|  fc220e     lea 0xd8e(a5), a0		| pointer to rs232 iorec
|  fc2212     lea 0xfc2334, a1		| start data for iorec
|  fc2218     moveq #0x21, d0            | counter
|  fc221a     bsr loop_mfp2		| copy iorec to RAM

|  fc221e     lea 0xdbe(a5), a0          | pointer to midi iorec          
|  fc2222     lea 0xfc2326, a1           | start data for iorec
|  fc2228     moveq #0xd, d0		| counter
|  fc222a     bsr loop_mfp2
  
  fc222e     move.l #just_rts, d0	| Midi + keyboard error vector
  fc2234     move.l d0, 0xdd0(a5)	| set keyboard error vector
  fc2238     move.l d0, 0xdd4(a5)       | set midi error vector          

  fc223c     move.l #0xfc2ce2, midivec
  fc2244     move.l #0xfc284a, midisys
  fc224c     move.l #0xfc285a, ikbdsys
  fc2254     move.b #3, -0x3fc                    
  fc225c     move.b #-0x6b, -0x3fc                 
  fc2264     move.b #7, conterm
  fc226a     move.l #0xfc1d12, clockvec

  fc2272     move.l #just_rts, d0                 
  fc2278     move.l d0, statvec
  fc227c     move.l d0, mousevec
  fc2280     move.l d0, joyvec

  fc2284     moveq #0, d0                        
  fc2286     move.l d0, 0xe44(a5)                 
  fc228a     move.b d0, 0xe48(a5)                 
  fc228e     move.b d0, 0xe49(a5)                 
  fc2292     move.l d0, 0xe3e(a5)                 
  fc2296     bsr 0xfc1f08		| set strobe to high
  fc229a     move.b #0xf, 0xe3c(a5)                
  fc22a0     move.b #2, 0xe3d(a5)                 

  fc22a6     lea 0xdb0(a5), a0          | pointer to keyboard iorec
  fc22aa     lea 0xfc2318, a1           | start data for iore
  fc22b0     moveq #0xd, d0             | counter
  fc22b2     bsr.s loop_mfp2		
  fc22b4     bsr 0xfc2f0e               | pointer to keyboard scancodes

  fc22b8     move.b #3, 0xfffc00     	| ACIA - master reset
  fc22c0     move.b #0x96, 0xfffc00     | ACIA - /64, 8N1

  fc22c8     movea.l #0xfc2356, a3
  fc22ce     moveq #3, d1                        
loop_mfp3:
  fc22d0     move.l d1, d2                       
  fc22d2     move.l d1, d0                       
  fc22d4     addi.b #9, d0                       
  fc22d8     asl.l #2, d2                        
  fc22da     movea.l 0(a3,d2.w), a2              
  fc22de     bsr mfpint1
  fc22e2     dbra d1, loop_mfp3
  fc22e6     lea 0xfc281c, a2                     
  fc22ec     moveq #6, d0                        
  fc22ee     bsr mfpint1

  fc22f2     lea 0xfc26b2, a2                     
  fc22f8     moveq #2, d0                        
  fc22fa     bsr mfpint1

  fc22fe     movea.l #0xfc2314, a2                
  fc2304     moveq #3, d3                        
  fc2306     bsr 0xfc1fc8                        
  fc230a     rts                                

----- cop                                       
loop_mfp2:
	move.b (a1)+,(a0)+                 
	dbra d0, loop_mfp2
	rts                                


| ==== Setup timer D ========================================================

        lea 	0xfffa01, a0      | base address of MFP
        move.b 	#2, 0x24(a0)	| set it to 9600 baud.
        move.b 	0x1C(a0), d0	| tcdcr
        and.b 	#0x70, d0	| keep the timer C stuff intact,
        or.b 	#0x01, d0	| timer D on /4 division mode
        move.b 	d0, 0x1C(a0)

	pea msg_mfp	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== Setup usart transmitter ==============================================
init_usart:
        move.b 	#1, 0x2c(a0)	| tsr
        move.b 	#0x0A, 0x28(a0)	| ucr : 8bits, 1 start, 1 stop, no parity

| ==== Setup MFP timer ======================================================
init_timer:
	movem.l	d0-d4/a0-a3,-(sp)	| save registers
        lea 	0xfffa01, a0		| base address of MFP
  fc2370   movea.l #mfp_reg4, a3  
  fc2376   movea.l #mfp_msk1, a2  
  fc237c   bsr.s msk_reg
  fc237e   movea.l #mfp_reg1, a3  
  fc2384   movea.l #mfp_msk1, a2  
  fc238a   bsr.s msk_reg
  fc238c   movea.l #mfp_reg2, a3  
  fc2392   movea.l #mfp_msk1, a2  
  fc2398   bsr.s msk_reg
  fc239a   movea.l #mfp_reg3, a3  
  fc23a0   movea.l #mfp_msk1,a2   
  fc23a6   bsr.s msk_reg

  fc23a8   movea.l #mfp_msk2,a3   
  fc23ae   movea.l #mfp_msk3,a2   
  fc23b4   bsr.s msk_reg

  fc23b6   exg.l a3, a1           
  fc23b8   lea mfp_msk4, a3       
  fc23be   moveq #0,d3            
  fc23c0   move.b 0(a3,d0.w),d3   
loop_mfp1:
  fc23c4   move.b d2, 0(a0,d3.w)  
  fc23c8   cmp.b 0(a0,d3.w), d2   
  fc23cc   bne.s loop_mfp1

  fc23ce   exg.l a3, a1            
  fc23d0   or.b d1,(a3)             
  fc23d2   movem.l (a7)+,d0-d4/a0-a3
  fc23d6   rts                      

msk_reg:
  fc23d8         bsr.s get_msk
  fc23da         move.b (a2),d3
  fc23dc         and.b d3,(a3)
  fc23de         rts

get_msk:
  fc23e0      moveq #0,d3
  fc23e2      adda.w d0,a3
  fc23e4      move.b (a3),d3
  fc23e6      add.l a0,d3
  fc23e8      movea.l d3,a3
  fc23ea      adda.w d0,a2
  fc23ec      rts

|----- mfp register numbers
fc23ee
mfp_reg1:
	dc.b 	06, 06, 08, 08	| iera, iera, ierb, ierb
mfp_reg2:
        dc.b	0a, 0a, 0c, 0c	| ipra, ipra, iprb, iprb
mfp_reg3:
        dc.b 	0e, 0e, 10, 10	| isra, isra, isrb, isrb
mfp_reg4:
        dc.b	12, 12, 14, 14	| imra, imra, imrb, imrb

|----- masks for mfp-register
  fc23fe
mfp_msk1:
	dc.b 	df, fe, df, ef
mfp_msk2:
        dc.b	18, 1a, 1c, 1c
mfp_msk3:
        dc.b	00, 00, 8f, f8
mfp_msk4:
	dc.b	1e, 20, 22, 24

|------------- Interruptvectors for MFP -------------
mfpvecs:
fc2356 	dc.l	00fc2718	| #9  - txerror
	dc.l	00fc2666        | #10 - txrint
        dc.l	00fc26fa        | #11 - rxerror
        dc.l	00fc2596        | #12 - rcvint


| ==== mfpint - (XBIOS) =====================================================
| 
mfpint:
  fc240e       move.w 4(sp),d0		| get int number from stack
  fc2412       movea.l 6(sp),a2		| get int vector from stack
  fc2416       andi.l #0xf,d0		| as long word index
mfpint1:
  fc241c    movem.l d0-d2/a0-a2,-(a7)
  fc2420    bsr.s jdisint1
  fc2422    move.l d0,d2
  fc2424    asl.w #2,d2
  fc2426    addi.l #0x100,d2
  fc242c    movea.l d2,a1
  fc242e    move.l a2,(a1)
  fc2430    bsr.s 0xfc247c
  fc2432    movem.l (a7)+,d0-d2/a0-a2
  fc2436    rts



| ==== jdisint - (XBIOS) ====================================================
| disable an MFP interrupt
jdisint:
  fc2438     move.w 4(sp),d0	| get int number from stack
  fc243c     andi.l #0xf,d0	| as long word index
jdisint1:
  fc2442     movem.l d0-d1/a0-a1,-(a7)
  fc2446     lea -0x5ff,a0
  fc244c     lea 0x12(a0),a1
  fc2450     bsr.s 0xfc249c
  fc2452     bclr d1,(a1)
  fc2454     lea 6(a0),a1
  fc2458     bsr.s 0xfc249c
  fc245a     bclr d1,(a1)
  fc245c     lea 0xa(a0),a1
  fc2460     bsr.s 0xfc249c
  fc2462     bclr d1,(a1)
  fc2464     lea 0xe(a0),a1
  fc2468     bsr.s 0xfc249c
  fc246a     bclr d1,(a1)
  fc246c     movem.l (a7)+,d0-d1/a0-a1
  fc2470     rts



| ==== jenabint - (XBIOS) ===================================================
jenabint:
  fc2472    move.w 4(a7),d0	| get int number from stack
  fc2476    andi.l #0xf,d0	| as long word index
jenabint1:
  fc247c    movem.l d0-d1/a0-a1,-(a7)
  fc2480    lea 0xfffa01,a0	| base address of MFP
  fc2486    lea 6(a0),a1
  fc248a    bsr.s bselect
  fc248c    bset d1,(a1)
  fc248e    lea 0x12(a0),a1
  fc2492    bsr.s beselect
  fc2494    bset d1,(a1)
  fc2496    movem.l (a7)+,d0-d1/a0-a1
  fc249a    rts



| ==== bselect ==============================================================
| find bit and register numbers
bselect:
  fc249c     move.b d0,d1
  fc249e     cmpi.b #8,d0
  fc24a2     blt.s go_here1
  fc24a4     subq.w #8,d1
go_here1:
  fc24a6     cmpi.b #8,d0
  fc24aa     bge.s bsel_end
  fc24ac     addq.w #2,a1
bsel_end:
  fc24ae     rts



| ==== Some other vectors ===================================================
	move.l 	#print_stat, prt_stat	|
	move.l 	#print_vec, prt_vec	|
	move.l 	#serial_stat, aux_stat  |
	move.l 	#serial_vec, aux_vec	|
	move.l 	#dump_scr, dump_vec	|


	move.w #0x8, nvbls		| nvbls
	st     _fverify			| _fverify
	move.w #0x3, seekrate		| floppy seekrate = 3 ms
	move.l #diskbuf, _dskbufp	| _dskbufp
	move.w #-1, _dumpflg		| clear _dumpflg
	move.l #os_entry, _sysbase	| Set _sysbase to ROM-start
	move.l #save_area, savptr       | savptr for Trap dispatcher


| ==== Set Exceptions to known handler ======================================
|	lea excepth, a1			| Exception handler + bomb routine
|	adda.l #0x2000000,a1		| #2 -#63
|	lea 8,a0			| Pointer to exeptions
|	move.w #0x3d,d0			| d0 = loop counter
|loop_ex:
|	move.l a1, (a0)+		| Set exeption routine
|	adda.l #0x1000000, a1		| Increase Vectorno.
|	dbra d0, loop_ex
|
|	move.l #just_rte, 0x14		| Division durch Null to rte

| ==== STonX different vectors ==============================================
	move.l #vbl_int, 0x70 		| Vbl-interr
	move.l #hbl_int, 0x68 		| Hbl-interr
	move.l #just_rte, 0x88		| Trap #2  (AES, now dummy)
	move.l #bios, 0xb4	      	| Trap #13 (BIOS)
	move.l #xbios, 0xb8 		| trap #14 (XBIOS)
|	move.l #0xfc9ca2, 0x28	 	| Line-A
	move.l #exc_acia, 0x118 		| keyboard interrupt vector (ACIAs)

	move.l #just_rts, etv_timer      | etv_timer (->RTS)
|	move.l #_criter1, etv_critic	| etv_critic
	move.l #just_rts, etv_term	| etv_term  (->RTS)


| ==== Clear VBL queue list =================================================
        lea _vbl_list, a0	   	| Get addr. of VBL-routine
        move.l a0, _vblqueue	   	| nach _vblqueue
        move.w #7, d0		  	| Loop counter
clrvbl:
	clr.l (a0)+		   	| Clear VBL-QUEUE
        dbra d0, clrvbl	   		| Loop

|        bsr 0xfc21b4		   	| MFP initialisieren
	move.w #1, vblsem 	   	| don not execute vbl-routine


| ==== Test, if Cartridge of type 2 (not implemented) =======================



| ==== Test, if Cartridge of type 0 (not implemented) ======================

        move.w #0x2300,sr	   | Interrupts freigeben

| ==== Test, if Cartridge of type 1 (not implemented) ======================

|       bsr 0xfc4b5a		   | GEMDOS initialisieren

	pea msg_gemdos	| Print, what's going on
	bsr _kprint
	addq #4,sp

| ==== Set Date ============================================================
|        move.w os_dosdate, -(sp)   | Erstellungsdatum im DOS-Format
|        move.w #0x2b, -(sp)	   | Set Date (set_data)
|        trap #1
|        addq.w #4,a7

| ==== Test, if Cartridge of type 3 (not implemented) ======================
|        bsr flopboot       	   | Boot from Floppy?
|        bsr 0xfc04a8		   | vom DMA-BUS booten
|        bsr 0xfc0d20		   | Execute Reset-resistent PRGs
|        tst.w 0x482		   | soll command.prg geladen werden ?
|        beq.s 0xfc03fe		   | Nein -->

| ==== Load command.prg =====================================================
|        bsr 0xfc457c		   | Ja, -> Cursor einschalten
|        bsr 0xfc0b14		   | autoexec : Prgs im AUTO-Ordner ausführen
|        pea 0xfc0489	   | Environmentstring ('')
|        pea 0xfc0489	   | Kommandozeile ('')
|        pea 0xfc0476	   | ('COMMAND.PRG')
|        clr.w -(a7)		   | laden +starten
|        bra.s 0xfc045a		   | goto EXEC
        
	pea msg_shell	| Print, what's going on
	bsr _kprint
	addq #4,sp


	pea msg_halt	| Print, what's going on
	bsr _kprint
	addq #4,sp
everloop:
	bra	everloop			| Halt for debugging











| ==== STonX reserved vector usage ==========================================
|	move.l #switch_ok,0x46e(a5)

	move #0x30,-(sp)
	trap #1		| Call GEMDOS
	addq #2,sp

	move.w 0x446(a5),d0 	| Get _bootdev default boot device
	move d0,-(sp)
	move #0xe,-(sp)
	trap #1		| Call GEMDOS
	addq #4,sp





| ===========================================================================
| ==== Subroutines ==========================================================
| ===========================================================================

| ==== Dummy functions ======================================================

print_stat:
print_vec:
serial_stat:
serial_vec:
dump_scr:

just_rts:	
	rts		| Just a dummy

vbl_int:
hbl_int:
timerc_int:

just_rte:	
	rte		| Just a dummy

| ==== Timer A interrupt handler ============================================
timera_int:
	rte		| Just a dummy

| ==== Timer B interrupt handler ============================================
timerb_int:
	rte		| Just a dummy

| ==== Timer C interrupt handler ============================================
timerc_int:
	rte		| Just a dummy

| ==== Timer D interrupt handler ============================================
timerd_int:
	rte		| Just a dummy

| ==== txerror ==============================================================
txerror:
	rte		| Just a dummy

| ==== txerror ==============================================================
txrint:
	rte		| Just a dummy

| ==== txerror ==============================================================
rxerror:
	rte		| Just a dummy

| ==== txerror ==============================================================
rcvint:
	rte		| Just a dummy



| ==== Critical error handler ===============================================
| Just sets D0 (return code) to -1, end ends the subroutine

_criter:
	move.l	_criter, -(sp)	| etv_critic on stack
_criter1:
	moveq.l	#-1, d0		| Default error
        rts			| jump back to routine
        

| ==== STonX - Native print routine for debugging ===========================
_print:
_printout:
	dc.w 0xa0ff 	| Jump to native execution
	dc.l 0          | Printing subroutine
	rts

|_kprintf:
|	move.w sr, -(sp)	| Save status register
|        move.w #0x2700, sr      | make routine uninteruptable 
|        jsr _doprint            | do the real work of printing
|        move.w (sp)+, sr	| restore status register
|	rts



| ==== Boot the Harddrive ===================================================
drv_rw:
	pea drv_rwmsg
	bsr _kprint
	addq #4,sp
	dc.w 0xa0ff 	| Jump to native execution
	dc.l 1
	rts

drv_rwmsg:
	.ascii "BIOS: Do hdv_rw - native Disk read/write\n"
	dc.w 0

| ==== STonX - Get the BIOS parameter block =================================
drv_bpb:
	pea drv_bpbmsg	| Print, what's going on
	bsr _kprint
	addq #4,sp
	dc.w 0xa0ff 	| Jump to native execution
	dc.l 2
	rts

drv_bpbmsg:
	.ascii "BIOS: Do hdv_bpb - Got Bios Parameter Block for drive\n"
	dc.w 0

| ==== STonX - Move native ==================================================
move_native:
	dc.w 0xa0ff 	| Jump to native execution
	dc.l 4          | Now an unused function
	rts

| ==== STonX - Use native GEMDOS ============================================
bdos:
	pea gemdosmsg	| Print message
	bsr _kprint
	addq #4,sp

	dc.w 0xa0ff 		| Jump to native execution
	dc.l 5          	| Call native GEMDOS
|	bne.s go_oldgemdos
	rte

| ==== STonX - Did the media (Floppy) change? ===============================
drv_mediach:	
	moveq #0,d0	| just a dummy 
	rts		| STonX can not change floppies (till now)

| ==== STonX - Init the Harddrive ===========================================
drv_init:
	pea drv_initmsg
	bsr _kprint
	addq #4,sp

	rts           	| Just a dummy

drv_initmsg:
	.ascii "BIOS: Do drv_init - Init the Harddrive (fake)\n"
	dc.w 0

| ==== Boot from floppy/Disk ================================================
flopboot:
	move.l	hdv_boot, a0	| Get floppy boot vector
        jsr	(a0)		| Load boot sector
        tst.w	d0		| Executable?
        bne	rtnflop		| no -> that's it!
        lea	diskbuf, a0	| Get disk buffer
        jsr	(a0)		| Execute boot sector
rtnflop:        
	rts

| ==== STonX - Boot the Harddrive ===========================================
drv_boot:	
	pea hdv_bootmsg
	bsr _kprint
	addq #4,sp

	move.w _bootdev, d0	| get boot device
	move d0,-(sp)
	move #0,-(sp)
	move #1,-(sp)
	move.l _dskbufp, -(sp)  | get pointer to 1k buffer for io
	move #0,-(sp)
	lea	hdv_rw, a0      | Get routines address
	jsr (a0)
	lea 12(sp),sp
	moveq #4,d0
	rts

hdv_bootmsg:
	.ascii "BIOS: Do hdv_boot - Boot from specific drive\n"
	dc.w 0

| ==== 0x118 - exception for keyboard interrupt =============================
exc_acia:
	movem.l	d0-d3/a0-a3/a5, -(sp)	| save registers
	pea msg_key	| Print, what's going on
	bsr _kprint
	addq #4,sp
	movem.l	(sp)+, d0-d3/a0-a3/a5	| restore registers
	rte	

| ==== STonX - Reset vector =================================================
| resvector routine, needed for manual memory configuration if larger screen
| is used

resvec:
	dc.w 0xa0ff 	| Jump to native execution
	dc.l 11		| GEMDOS post
	clr.l 0x426.w    | Clear magic number for new memory initialization
	jmp (a6)


| ==== STonX - Linea-vector =================================================
new_linea:
||       cmp.b #2,0xff8260	| Check Video Mode
||       bne.s linea_back0       | if not b/w, 
|
|	move.l 0x28.w,-(sp)
|	pea hiho1
|	move sr,-(sp)
|	move.l #linea_back,0x28.w
|linea_back0:
|	move.l old_linea,a0
|	jmp (a0)
|hiho1:
|	dc.w 0xa000
|	move.l (sp)+,0x28.w
|	dc.w 0xa0ff
|	dc.l 7
|	addq.l #2,2(sp)
|linea_back:
	rte

        
| ==== trap_1 - trap 1 (GEMDOS) entry point =================================

_trap_1:
	move.l	(sp)+,t1regsav	| save return address
	trap	#1		| call bdos call
	move.l	t1regsav,-(sp)	| restore return address
		rts

| ==== Trap 13 - BIOS entry point ==========================================

bios:
	move.w	(sp)+,d0	| Status register -> d0
	move.w	d0,save_sr       | and save in savesr
	move.l	(sp)+,save_rt	| save return address
	move.l	sp,save_sp       | save stack pointer
        
        move.l  savptr, a1
        movem.l	d3-d7/a3-a7, -(a1)	| Safety for routines
        move.l  a1, savptr
        
	btst	#13,d0		| were we in user mode?
	beq	bret_exc	| yes, exit
	move.l	#0, a0		| clear higher bits in a0
	move.w	(sp)+,a0	| remove the function number from stack
	cmp.w	bios_ent,a0	| Higher, than highest number?
	bge	bret_exc
	add.l	a0,a0		| indirection function table is 1 LW per
	add.l	a0,a0		| so multiply function number by 4
	add.l	#bios_vecs,a0	| add in the base address of lookup table
	move.l	(a0),a0		| get the procedures address
	jsr	(a0)		| go do it and then come back
bret_exc:
        move.l  savptr, a1
        movem.l	(a1)+, d3-d7/a3-a7	| Get all registers back
        move.l  a1, savptr

	move.l	save_sp,sp	| restore original stack
	move.l	save_rt,-(sp)
	move.w	save_sr,-(sp)
	rte                     | return with function # in D0

| ==== Trap 14 - XBIOS entry point =========================================

xbios:
	move.w	(sp)+,d0	| Status register -> d0
	move.w	d0,save_sr       | and save in savesr
	move.l	(sp)+,save_rt	| save return address (within trap14)
	move.l	sp,save_sp

        move.l  savptr, a1
        movem.l	d3-d7/a3-a7, -(a1)	| Safety for routines
        move.l  a1, savptr
        
	btst	#13,d0		| were we in user?
	beq	xret_exc		| yes, exit
	move.l	#0, a0		| clear a0
	move.w	(sp)+,a0	| remove the function number from stack
	cmp.w	xbios_ent,a0	| Higher, than highest number?
	bge	xret_exc
	add.l	a0,a0		| indirection function table is 1 LW per
	add.l	a0,a0		| so multiply function number by 4
	add.l	#xbios_vecs,a0	| add in the base address of lookup table
	move.l	(a0),a0		| get the procedures address
	jsr	(a0)		| go do it and then come back
xret_exc:
        move.l  savptr, a1
        movem.l	(a1)+, d3-d7/a3-a7	| Get all registers back
        move.l  a1, savptr
        
	move.l	save_sp,sp	| restore original stack
	move.l	save_rt,-(sp)
	move.w	save_sr,-(sp)
	rte                     | return with function # in D0


| ==== Set userdefined exception vectors ====================================
| Function is trap13, function # 5 sets user defined exception vector

_bsetvec: 
	move	4(sp),a0	| discover the exception vector to set
	adda.l	a0,a0		| multiply that number by four cause 4 bytes
	adda.l	a0,a0		|	are needed to make up one address

	move.l	(a0),d0		| old vector to d0
	cmp.l	#-1,6(sp)       | Is argument = -1?
	beq	vsplt           | If yes, just return
	move.l	6(sp),(a0)      | set new vector
vsplt:	
	rts           		| return with d0 = old vector

| ==== Exception handler ====================================================
excepth:
	bsr	doit
        nop
doit:
        move.l (a7)+,0x3c4  	| Exeptionnr. to 0x3c4
        movem.l d0-a7,0x384 	| Register nach 0x384 retten
        move.l usp,a0
        move.l a0,0x3c8     	| Addresse of USP to 0x3c8
        moveq #0xf,d0       	| 15 Durchlufe Schleife
        lea 0x3cc,a0        	|
        movea.l a7,a1      	|
loop2:
	move.w (a1)+,(a0)+ 	| Inhalte der SSP-Stacks nach 0x3cc
        dbra d0, loop2    	| Schleife
        move.l #0x12345678,0x380 	| quittieren
        moveq #0,d1
        move.b 0x3c4,d1
        subq.w #1,d1
        bsr.s bombs 		| Bomben auf Bildschirm
        move.l #0x93a,0x4a2
        move.w #1,-(a7)
        clr.l -(a7)      	| GEMDOS: TERM
        trap #1           
        bra _main      	| RESET, falls Rckkehr

| ==== Show bombs, if exception =============================================
bombs:
        move.b -0x7da0,d7	|Auflsung nach d7
        and.w #3,d7       	|maskieren
        add.w d7,d7       	|mal 2
        clr.l d0
        move.b -0x7dff,d0  	|Screen address
        lsl.w #8,d0       	|
        move.b -0x7dfd,d0  
        lsl.l #8,d0       
        movea.l d0,a0     	|und nach a0
        adda.w scrnmidd(pc,D7.W),A0
        lea bombdat,a1    	| Adresse Bombendaten -> a1
        move.w #0xf,d6     	| 15 Zeilen fr Bomben
loop5:  move.w d1,d2
        movea.l a0,a2
loop4:  move.w planecnt(pc,D7.W),D5
loop3:  move.w (a1),(a0)+ 	| Zeile Bomben ->Bildschirm
        dbra d5,loop3    
        dbra d2,loop4
        addq.w #2,a1
        adda.w llenght(pc,D7.W),A2
        movea.l a2,a0
        dbra d6,loop5    	| Next bomb line
        rts

| ==== Offset for middle of screen ==========================================
scrnmidd:
	dc.w	100*160		| Low resolution
        dc.w	100*160         | Middle
        dc.w	200*80		| High resolution

| ==== Count of bitplanes ===================================================
planecnt:
	dc.w	3
        dc.w	1
        dc.w	0
        dc.w	0

| ==== Line lenght in Bytes =================================================
llenght:
	dc.w	160
        dc.w	160
        dc.w	80
        dc.w	80

| ===========================================================================
| ==== DATA in text segment =================================================
| ===========================================================================

| ==== BIOS entry points to allow individual module compilation =============

	.even

bios_vecs:
	.dc.l	_bios_0
	.dc.l	_bios_1			| LONG character_input_status()
	.dc.l	_bios_2			| LONG character_input()
	.dc.l	_bios_3			| void character_output()
	.dc.l	_bios_4			| LONG read_write_sectors()
	.dc.l	_bsetvec                | 
	.dc.l	_bios_6			| LONG get_timer_ticks()
	.dc.l	_bios_7			| get disk parameter block address
	.dc.l	_bios_8			| LONG character_output_status(h)
	.dc.l	_bios_9			| media change?
	.dc.l	_bios_a			| what drives exist?
	.dc.l	_bios_b			| get/set alt-ctrl-shift status
	.dc.l	_bios_c
	.dc.l	_bios_d
	.dc.l	_bios_e
	.dc.l	_bios_f
	.dc.l	_bios_10

bios_ent:		| Max. number of BIOS entries
	.dc.w	(bios_ent-bios_vecs)/4


| ==== XBIOS entry points to allow individual module compilation ============

	.even
xbios_vecs:
	.dc.l	_xbios_0
	.dc.l	_xbios_1 
	.dc.l	_xbios_2 
	.dc.l	_xbios_3 
	.dc.l	_xbios_4 
	.dc.l	_xbios_5 
	.dc.l	_xbios_6 
	.dc.l	_xbios_7 
	.dc.l	_xbios_8 
	.dc.l	_xbios_9 
	.dc.l	_xbios_a 
	.dc.l	_xbios_b 
	.dc.l	_xbios_c
	.dc.l	_xbios_d
	.dc.l	_xbios_e
	.dc.l	_xbios_f
	.dc.l	_xbios_10
	.dc.l	_xbios_11
	.dc.l	_xbios_12
	.dc.l	_xbios_13
	.dc.l	_xbios_14
	.dc.l	_xbios_15
	.dc.l	_xbios_16
	.dc.l	_xbios_17
	.dc.l	_xbios_18
	.dc.l	_xbios_19
	.dc.l	_xbios_1a
	.dc.l	_xbios_1b
	.dc.l	_xbios_1c
	.dc.l	_xbios_1d
	.dc.l	_xbios_1e
	.dc.l	_xbios_1f
	.dc.l	_xbios_20
	.dc.l	_xbios_21
	.dc.l	_xbios_22
	.dc.l	_xbios_23
	.dc.l	_xbios_24
	.dc.l	_xbios_25
	.dc.l	_xbios_26
	.dc.l	_xbios_27

xbios_ent:		| Max. number of XBIOS entries
	.dc.w	(xbios_ent-xbios_vecs)/4

	.even

| ==== Some messages ========================================================
msg_start:
	.ascii "BIOS: Starting up EmuTOS Ver. 0.0 ...\n"
        dc.w 0
msg_main:
	.ascii "BIOS: Entered supervisor mode...\n"
        dc.w 0
msg_mem:
	.ascii "BIOS: Configured Memory ...\n"
	dc.w 0
msg_shift:
	.ascii "BIOS: Initialized Shifter ...\n"
	dc.w 0
msg_linea:
	.ascii "BIOS: LineA initialized ...\n"
	dc.w 0
msg_clrbss:
	.ascii "BIOS: Cleared RAM ...\n"
	dc.w 0
msg_clrscn:
	.ascii "BIOS: Cleared Screen ...\n"
	dc.w 0
msg_drvinit:
	.ascii "BIOS: Drives initialized ...\n"
	dc.w 0
msg_sound:
	.ascii "BIOS: Soundchip initialized ...\n"
	dc.w 0
msg_floppy:
	.ascii "BIOS: Floppy deselected ...\n"
	dc.w 0
msg_mfp:
	.ascii "BIOS: MFP initialized ...\n"
	dc.w 0
msg_gemdos:
	.ascii "BIOS: GEMDOS (not yet) initialized ...\n"
	dc.w 0
msg_shell:
	.ascii "BIOS: COMMAND.PRG loaded ...\n"
	dc.w 0
msg_halt:
	.ascii "BIOS: HALT - should never be reached!\n"
	dc.w 0
msg_test:
	.ascii "BIOS: Last test point reached ...\n"
	dc.w 0

msg_key:
	.ascii "BIOS: Key pressed or released ...\n"
	dc.w 0

int_test:
	dc.l	0x88888888
        dc.w	0
gemdosmsg:
	.ascii "GEMDOS call\n"
	dc.w 0

| ==== Videopalette for shifter =============================================
colorpal:
	dc.w	0x0777, 0x0707, 0x0070, 0x0770
        dc.w	0x0007, 0x0707, 0x0077, 0x0555
        dc.w	0x0333, 0x0733, 0x0373, 0x0773
        dc.w	0x0337, 0x0737, 0x0377, 0x0000

| ==== Image of Boms to show, if fatal error comes up =======================
bombdat:
	dc.w 	0x0600	| %0000011000000000
	dc.w 	0x2900	| %0010100100000000
	dc.w 	0x0080	| %0000000010000000
	dc.w 	0x4840	| %0100100001000000
	dc.w 	0x11f0	| %0001000111110000
	dc.w 	0x01f0	| %0000000111110000
	dc.w 	0x07fc	| %0000011111111100
	dc.w 	0x0ffe	| %0000111111111110
	dc.w 	0x0ffe	| %0000111111111110
	dc.w 	0x1fff	| %0001111111111111
	dc.w 	0x1fef	| %0001111111101111
	dc.w 	0x0fee	| %0000111111101110
	dc.w 	0x0fde	| %0000111111011110
	dc.w 	0x07fc	| %0000011111111100
	dc.w 	0x03f8	| %0000001111111000
	dc.w 	0x00e0	| %0000000011100000


tiggle:
	.dc.b	0

| ==== Scancode table unshifted =============================================
scan_unshift:	|0xfc2034
	dc.b	0x00, 0x1B,  1,  2,  3,  4,  5,  6
        dc.b	7,  8,  9,  0,  ž,  ', 0x08, 0x09
        dc.b	q,  w,  e,  r,  t,  z,  u,  i
        dc.b	o,  p,  ,  +, 0D, 00,  a,  s
        dc.b	d,  f,  g,  h,  j,  k,  l,  ”
        dc.b	„  # 00  ~  y  x  c  v
	dc.b	b,  n,  m,  ,,  .,  -, 0x00, 0x00
        dc.b	0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        dc.b	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        dc.b	0x00, 0x00, 0x2D, 0x00, 0x00, 0x00, 0x2B, 0x00
        dc.b	0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00
        dc.b	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        dc.b	0x3C, 0x00, 0x00, 0x28, 0x29, 0x2F, 0x2A, 0x37
        dc.b	0x38, 0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33
        dc.b	0x30, 0x2E, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00
        dc.b	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
| ==== Scancode table shifted ===============================================
scan_shift:	|0xfc20b4
	dc.b	00 1B  !  "  Ý  $  %  &
        dc.b	/  (  )  =  ?  ` 0x08 0x09
        dc.b	 Q  W  E  R  T  Z  U  I
        dc.b	 O  P  š  * 0D 00  A  S
        dc.b	 D  F  G  H  J  K  L  ™
        dc.b	 Ž  ^  00 >  Y  X  C  V
	dc.b	 B  N  M  ;  :  _  0x00 0x00
        dc.b	 0x00 0x20 0x00 0x00 0x00 0x00 0x00 0x00
        dc.b	 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x37
        dc.b	 0x38 0x00 0x2D 0x34 0x00 0x36 0x2B 0x00
        dc.b	 0x32 0x00 0x30 0x7F 0x00 0x00 0x00 0x00
        dc.b	 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
        dc.b	 0x3E 0x00 0x00 0x28 0x29 0x2F 0x2A 0x37
        dc.b	 0x38 0x39 0x34 0x35 0x36 0x31 0x32 0x33
        dc.b	 0x30 0x2E 0x0D 0x00 0x00 0x00 0x00 0x00
        dc.b	 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
| ==== Scancode table with caps lock ========================================
scan_caps:	| 0xfc2134
	dc.b	0x00 0x1B  1  2  3  4  5  6
        dc.b	7  8  9  0  ž  ' 0x08 0x09
        dc.b	 Q  W  E  R  T  Z  U  I
        dc.b	 O  P  š  + 0x0D 0x00  A  S
        dc.b	 D  F  G  H  J  K  L  ™
        dc.b	 Ž  # 0x00  <  Y  X  C  V
	dc.b	 B  N  M  ,  .  - 0x00 0x00
        dc.b	 0x00 0x20 0x00 0x00 0x00 0x00 0x00 0x00
        dc.b	 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
        dc.b	 0x00 0x00 0x2D 0x00 0x00 0x00 0x2B 0x00
        dc.b	 0x00 0x00 0x00 0x7F 0x00 0x00 0x00 0x00
        dc.b	 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
        dc.b	 0x3C 0x00 0x00 0x28 0x29 0x2F 0x2A 0x37
        dc.b	 0x38 0x39 0x34 0x35 0x36 0x31 0x32 0x33
        dc.b	 0x30 0x2E 0x0D 0x00 0x00 0x00 0x00 0x00
        dc.b	 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00


	.end

















| ==== Initialize the hardware ==============================================

| ==== Revector the timer interrupt to me.(OLDCLOCK)
|_x1:
|       	move.l	#timerint,0x130		| vector timer interrupt vector 
|       	move.w	#0,_tticks		| initialize timer
|       	ori.b	#0x3,0xf19f05		| enable timer interrupts
|       	move.b	0xf1a09a,d0		| read and set VRT bit
|
| ==== Revector the timer interrupt to me.(VME-CLOCK)
|       	move.l	#_timerint,0x130		| vector timer interrupt vector 
|       	move.w	#0,_tticks		| initialize timer
|       	jsr	_cl_init		| vmetimer.c


| ==== and the serial port interrupt 
|      	move.l	#_m400_isr,0x114		| this is where INT4 goes


| ==== Find out memory size for TPA =========================================
| The memory configuration is found out by running into bus-errors, if memory
| is not present at a tested address. Therefor a special buserror-handler is
| temporarely installed.

       	move.l	8,a0			| save old bus error vector
       	move.l	#bus_error,0x8		| and put in our temporary handler
       	move.l	sp,a1			| save the current SP

       	move.l	#TPASTART,m_start	| set up default values
       	move.l	#TPALENGTH,m_length	| for internal memory

       	move.w	#0,0x180000		| do we have memory here?
       	move.l	#0x180000,m_start	| yes, update MPB start
       	move.l	#0x40000,m_length	| and length (for 256K initially)

       	move.w	#0,0x1C0000		| memory at 2nd 256K boundary? 
	move.l	#0x80000,m_length	| yes, assume 512K at this step

	move.w	#0,0x200000		| memory at 3rd 256K boundary?
	move.l	#0xC0000,m_length	| yes, assume 3/4M at this step

bus_error:
       	move.l	a0,0x8			| restore old bus error vector
       	move.l	a1,sp			| and old stack pointer

       	move.l	m_start,a0		| clear TPA
       	move.l	m_length,d0		| get byte count
       	lsr.l	#1,d0			| make word count
       	subq.l	#1,d0			| pre-decrement for DBRA use
       	clr.w	d1			| handy zero

parity_loop:
       	move.w	d1,(a0)+		| clear word
       	dbra	d0,parity_loop
	
       	move.l	#_brkpt,0x7c		| set nmi to do an illegal instruction
       	move.l	#bios,0xb4		| revector bios entry = trap #13
       	move.l	#defcrit,v101		| default crit error:	     bios13, fnct 5
       	move.l	#defterm,v102		| terminate handler:	     bios13, fnct 5
       	move.l	#fix_SR,0x20		| privilege violation
       	jsr	_biosinit		 

       	move	#0x2000,sr
       	jsr	_cmain

_brkpt:
       	illegal
       	bra.s	_brkpt

| ==== bugger unknown trap so pick offstack pointer and pass to biosng
	.ifne 0
bugger:
	move.l	sp,-(sp)
	jsr	_biosng
hang:
	bra	hang
	.endc

| ==== This is the timer interrupt handler ==================================
| _ti: is here strictly so we have a symbol for the isr in the symtab

timerint:
_ti:
	movem.l	d0-d7/a0-a7, regsav
	move.b	0xf1a099,d0	| read control register C to clear int
	bchg.b	#0,tiggle	| toggle on every other tick
	beq	ti_ret

	addq.l	#1,_tticks	| increment our counter
	move.l	_charvec+0xc,a0	| call user specified timer interrupt routine
	move.l	#31, -(sp)	| 2 x (15.625 ms clk rate rounded up to 16 ms)
	move.l	#1,-(sp)	| show packet received
	jsr	(a0)		| call user spec clock interrupt handler

ti_ret:
	movem.l regsav, d0-d7/a0-a7
	rte

| ==== _timerint - clock interrupt front end.================================
| this front end is different from the rest in that the clock isr 
| will call _timlox which will call the logical vector, if one has
| been set with b_setvec.
|_timerint:
|	addq.l	#1,_tticks	| increment our counter
|	movem.l	d0-d7/a0-a7, regsav
|	jsr	_cl_intr	|  vmetimer.c
|ti_xit:
|	movem.l regsav, d0-d7/a0-a7
|	rte


| because move from SR is privileged in 68010 we have this code
| to intercept privilege violations, look at see if it was a move
| from SR and if so change it to a move from CCR

fix_SR:
	movem.l	d0-d0/a0-a0,-(sp)	| saving d0 and a7 while we test
	movea.l	0xa(sp),a0		| the offending address
	move.w	(a0),d0			| the offending instruction
	andi.w	#0xffc0,d0		| isolate move from SR opcode
	cmpi.w	#0x40c0,d0		| we don't care where "To" was
	bne	not_lSR			| Not a MOVE SR,XX instruction
	ori.w	#0x200,(a0)		| make it MOVE CCR,XX
	movem.l	(sp)+,d0-d0/a0-a0
	rte

| ERIC!!!! need tp gracefully handle exceptions like this

not_lSR:
	bra	not_lSR

| ==== 68010 this procedure MUST be called by the routine that is pointed at
| by an exception vector, it removes the trap frame type and leaves the stack
| looking just like a 68000
|
|_fix_trap:
|	move.w	8(sp),10(sp)	| lsb of return address on top of frame type
|	move.w	6(sp),8(sp)	| msb of return address on top of lsb
|	move.w	4(sp),6(sp)	| move status register  on top of msb
|	move.w	2(sp),4(sp)	| fix_trap caller lsb
|	move.w	0(sp),2(sp)	| fix_trap caller msb
|	add.l	#2,sp		| discard the missing word to even the stack
|	rts

| ==== This routine must be entered via BRA rather than executing an RTE
|_fix_rte:
|	move.w	#0,-(sp)	| reserve a hole on top of stack
|	move.w	2(sp),0(sp)	| move SR out of the way
|	move.w	4(sp),2(sp)	| msb of return address
|	move.w	6(sp),4(sp)	| lsb of return address
|	move.w	#0,6(sp)	| store a bogus exeception frame type
|	rte

| Without considering wait states movep.l makes the faster loop. 
| However, accounting for wait states makes the 68010 loop mode faster.

| ==== read 128 bytes of data in a hurry
| C calling sequence:
|     rddat(char *where)

_rddat:
	move.l	4(sp),a0
	move.w	#127,d0
	move.l	#0xf1c0d9,a1
rdlp:
	move.b	(a1),(a0)+
	dbf	d0,rdlp
	rts

| ==== writes 128 bytes of data in a hurry
| C calling sequence:
|     wrdat(char *where)

_wrdat:
	move.l	4(sp),a0		| where data to be written lives in mem
	move.w	#127,d0			| always transfers entire physical sector
	move.l	#0xf1c0d9,a1		| where the disk interface lives
wrlp:
	move.b	(a0)+,(a1)		| repeatedly write the sector data bytes
	dbf	d0,wrlp			| decrement/test for complete
	rts


| ==== Determine if a memory address in fact is present
buserr	=	0x8			| Bus error vector

_no_device:
	move.l	4(sp),a0	| byte memory address to test existence
	move.l	buserr,-(sp)	| save current bus error vector
	move.l	#ndber,buserr	| point at our routine
	move.b	(a0),d0		| try to read the test address
	moveq.l	#0,d0		| return false if memory exists
	move.l	(sp)+,buserr	| restore original bus error vector
	rts

| ====  where a buss error would take us

ndber:
	moveq.l #1,d0		| returns true if no device at that address
	add.l	#58,sp		| no need to restore all that stuff
	move.l	(sp)+,buserr	| but restore original occupant of vector
	rts			| return to original caller


entries:
	.dc.w	(entries-links)/4

defterm: rts

defcrit: rts


_criter:
	move	4(sp),d0		| error code
	move	savesr,-(sp)
	move.l	saveret,-(sp)
	move.l	savesp,-(sp)
	move	d0,-(sp)
	move.l	v101,a0
	move.l	#-1,d0			| abort(-1) is default
	jsr	(a0)
	addq.l	#2,sp
	move.l	d0,d1
	swap	d1
	cmp	#1,d1
	bne	okgo			| retry ?
	move.l	(sp)+,savesp		| yes, restore stuff
	move.l	(sp)+,saveret
	move	(sp)+,savesr
	rts
okgo:	
	move.l	(sp)+,a0
	move.l	(sp)+,-(a0)		| retadd
	move	(sp)+,-(a0)		| sr
	move.l	a0,sp
	bra	_fix_rte
        
| ==== Do nothing ===========================================================
_nop:	rts

| ==== Dummy routine ========================================================
medchk:	move.l	#1,d0		| media may have changed
	rts



| ===========================================================================
| ==== Datasegment ==========================================================
| ===========================================================================
|	.data

tiggle:
	.dc.b	0

welcome:
	.ascii "Nullbios: a bootsector loader acting as a TOS in ROM\n(C) 2000, Laurent VOGEL.\n\000"

no_good_drive_message:
	.ascii "no good drive\n\000"
	
digits:
	.ascii "0123456789ABCDEF"

exception_msg:
	.ascii "exception number "

failure_msg:
	.ascii "floppy error\n\0"
	
bad_sector_msg:
	.ascii "bad sector number\n\0"

boot_error_msg:	
	.ascii "the boot sector failed.\n\0"

booting_msg:
	.ascii "booting...\n\0"

not_bootable_msg:	
	.ascii "not bootable\n\0"
	.align 2


| ===========================================================================
| ==== Bsssegment ===========================================================
| ===========================================================================
|	.bss

	.even

_b_mdx:
		.dc.l	1
m_start:	.dc.l	1
m_length:	.dc.l	1
		.dc.l	1

_tticks:	.dc.l	1

v100:		.dc.l	1
v101:		.dc.l	1
v102:		.dc.l	1

savesp:		.dc.l	1
savesr:		.dc.l	1

regsav:		.dc.l	16

saveret: 	.dc.l	1
	

bssstrt: 	dc.w	1


| ===========================================================================
| ==== End ==================================================================
| ===========================================================================

	.end




init:	



start:	rts

| ==== Subroutines ==========================================================







| ===========================================================================
| ==== Datasegment ==========================================================
| ===========================================================================
	.data
