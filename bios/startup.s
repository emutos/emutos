| ===========================================================================
| ==== startup.s - Emulator specific startup module.  To be linked in first!
| ===========================================================================
|
| Copyright (c) 2001 Martin Doering.
|
| Authors:
|  MAD  Martin Doering
|  LVL  Laurent Vogel
|  THO  Thomas Huth
|
| This file is distributed under the GPL, version 2 or at your
| option any later version.  See doc/license.txt for details.
|



| ==== Defines ==============================================================

        .equ    SUPSIZ, 4096            | size of the supervisor stack in words
        .equ    _GSX_ENT, 0             | Entry to GEM (if graphically)

        .equ    TPASTART, 0xE000        | default start address of tpa area
	.equ	TPALENGTH, 0x30000-TPASTART | length of tpa area - from 0x30000 down to TPASTART

        .equ    vec_divnull, 0x14       | division by zero interrupt vector
        .equ    vec_linea, 0x28         | LineA interrupt vector
        .equ    vec_hbl, 0x68           | HBL interrupt vector
        .equ    vec_vbl, 0x70           | VBL interrupt vector
        .equ    vec_mfp, 0x78           | MFP interrupt vector
        .equ    vec_aes, 0x88           | AES interrupt vector

        .equ    vec_bios, 0xb4          | BIOS interrupt vector
        .equ    vec_xbios, 0xb8         | XBIOS interrupt vector
        .equ    vec_acia, 0x118         | keyboard/Midi interrupt vector



| ==== References ===========================================================

        .global _os_dosdate             | OS entry point
        .global _main           | OS entry point
        .global _GSX_ENT
        .global _trap_1         | Calling GEMDOS from C
        .global _sysbase
        .global _b_mdx          | used in bios.c

        .global _printout
        .global _output

|#if    OLDCLOCK
|               .globl  _ti
|               .globl  _x1
|#else
|               .xdef   _cl_init        |  vmetimer.c
|               .xdef   _cl_intr        |  vmetimer.c
|#endif
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
        .global _cmain                  |file system init in main.c (cmain)
        .global _criter

        .global _drv_init
        .global _drv_boot
        .global _drv_rw
        .global _drv_bpb
        .global _drv_mediach


| ==== VME/10 Specifics =============
        .globl  _no_device
        .globl  _rddat
        .globl  _wrdat
        .globl  _supstk

| ==== From bios.c ==========================================================

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


| ==== tosvars.s - TOS System variables =====================================

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
        .xdef   themd          
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
        .xdef   bconstat_vec  
        .xdef   bconin_vec    
        .xdef   bcostat_vec   
        .xdef   bconout_vec  

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
        
| ==== lineavars.s - Graphics subsystem variables =============================
	.xdef	font_ring
		
| ==== vectors.s - Default exception vectors =============================
	.xdef	init_exc_vec
	.xdef	init_user_vec
	

| ===========================================================================
| ==== BSS segment ==========================================================
| ===========================================================================
        .bss


| save area for trap13/14.
save_beg:       ds.w    24*3    | d3-d7/a3-a7, sr, pc, vo = 24 words, 
				| multiplied by 3 ("3 level deep reentrant")
save_area:                      | End of Save storage


        .org    0x167a
diskbuf:        ds.b    1024    | 1 cluster disk buffer
        .org    0x45b8
_stkbot:        ds.w    SUPSIZ  | Supervisor stack
_stktop:                        | filled from top to bottom

_output:        ds.b    1024
t1regsav:         dc.l    1   

_b_mdx:                         | initial memory descriptor
                .dc.l   1
m_start:        .dc.l   1       | start address of TPA
m_length:       .dc.l   1       | length of TPA in byte
                .dc.l   1

_tticks:        .dc.l   1

| ===========================================================================
| ==== TEXT segment (TOS image) =============================================
| ===========================================================================

        .text
        .org    0x000000




| ==== OSHEADER =============================================================

.global _shifty
.global _run

os_entry:
    bra.s   _main       | os_entry, branch to _main
os_version:
    dc.w    0x0102      | os_version, TOS version
reseth:
    dc.l    _main       | reseth, pointer to reset handler
os_beg:
    dc.l    os_entry    | os_beg, base of os = _sysbase
os_end: 
    dc.l    _bssend     | os_end, first byte RAM not used by OS
os_res1:        
    dc.l    _main       | os_res1, reserved
os_magic:
    dc.l    0x0         | os_magic, pointer to GEM's MUPB
os_date:
    dc.l    0x14062001  | os_date, Date of system build
os_pal:        
    dc.w    0x0003      | Flag for PAL version (Only a word!)
_os_dosdate:
    dc.w    0x0c46      | _os_dosdate (wrong)
os_root:
    dc.l    0x0         | Pointer to the GEMDOS memory pool - not yet supported
os_kbshift:
    dc.l    _shifty     | Pointer to the keyboard shift keys states
os_run:
    dc.l    _run        | Pointer to a pointer to the actual basepage
os_dummy:
    dc.l    0           | ??? _main should start at $fc030, shouldn't it?




| ==== Get into supervisor mode ==============================================
_main:                                  | stunt to guarantee entry into supervisor mode
        move    #0x2700,sr              | disable interrupts

| ==== Reset all Hardware ====================================================

        reset                           | Reset all hardware

| ==== Check for diagnostic cartridge =======================================

        .equ    cart_base,      0xfffa0000

        cmp.l   #0xfa52235f, cart_base  | magic - is cartridge present?
        bne     memconf                 | no -> memconf
        lea     memconf(pc), a6         | load return address
        jmp     cart_base+4              | execute diagnostig cartridge

| ==== Clear BSS before calling any C function ======================
|| the C part expects the bss to be cleared, so let's do this early
| Better not, because some values may be needed after a reset!!! (MAD)

	lea bssstart,a0
	lea _bssend-1,a1
	move.l a1,d0
	sub.l a0,d0
	lsr.l #2,d0
clearbss:
	clr.l (a0)+
	dbra d0,clearbss



| ==== Set up a supervisor stack ============================================

| LVL   lea     _stkbot+SUPSIZ, sp      | Setup Supervisor Stack
        lea     _stktop, sp             | Setup Supervisor Stack

|        pea msg_start   | Print, what's going on
|        bsr _kprint
|        addq #4,sp

| ==== Check, if old memory config can be used ==============================

memconf:
        lea     memgoon(pc), a6         | load return address
        bra     memval                  | memory config valid?
memgoon:
        bne.s   resetvec                | no -> skip next command
        move.b  memctrl, 0xffff8001     | old config to controller

| ==== Reset vector =========================================================
resetvec:
       cmpi.l 	#0x31415926, resvalid	| Jump to resetvector?
       bne.s 	noreset    		| No --> noreset
       move.l 	resvector, d0		| Yes: old resvec to d0
       tst.b 	resvector		| Is it valid?
       bne.s 	noreset                	| No --> noreset
       btst 	#0, d0			| Address odd ?
       bne.s 	noreset    		| Yes --> noreset
       movea.l  d0, a0			| resvec
       lea 	resetvec(pc), a6        | save return address
       jmp 	(a0)			| jump to resvec
noreset:

| ==== Reset Soundchip =======================================================
initsnd:
        lea     0xffff8800, a0  | base address of PSG Soundchip
        move.b  #7, (a0)        | port A and B          
        move.b  #0xC0, 2(a0)    | set to output           
        move.b  #0xE, (a0)      | port A                

|        pea msg_sound   | Print, what's going on
|        bsr _kprint
|        addq #4,sp

| ==== Reset Floppy =========================================================
        move.b  #7, 2(a0)       | deselect floppy

|        pea msg_floppy  | Print, what's going on
|        bsr _kprint
|        addq #4,sp

| ==== Set shifter to pal ===================================================

        move.b #2, 0xff820a     | sync-mode to 50 hz pal, internal sync
        
| ==== Set color palette ====================================================

        lea    0xffff8240, a1     | video-shifter 
        move.w #0xf, d0         | loop for 16 colors
        lea    colorpal, a0     | color palette to a0
loadcol:
        move.w  (a0)+,(a1)+     | set color value         
        dbra   d0, loadcol      | next value   


| ==== Set temporary screenmem address 0x10000 to videoshifter ==============
        
        move.b  #0x1, 0xffff8201 | set hw video base high word
        move.b  #0x0, 0xffff8203 | set hw video base low word

| ==== Check, if memory configuration is valid ==============================

        move.b  memctrl, d6
        move.l  phystop, d5
        lea     memok, a6
        bra     memval
memok:
        beq     aftermem

| ==== Find out memory size for TPA =========================================
| The memory configuration is found out by running into bus-errors, if memory
| is not present at a tested address. Therefor a special buserror-handler is
| temporarely installed. Works just for 4mb or more till now. (MAD)

        clr.w   d6                      | start value form controller
        move.b  #0xa, 0xffff8001        | set hw to 2 banks by 2 mb

        movea.w #8, a0                  | startoffset for test
        lea     0x200008, a1            | a1 to 2. bank
        clr.w   d0                      | first test bitpattern

wrt_patt1:      
        move.w  d0, (a0)+               | write to both memory banks
        move.w  d0, (a1)+
        add.w   #0xfa54, d0             | bitpattern
        cmpa.l  #0x200, a0              | till adr. 0x200
        bne.s   wrt_patt1

        move.l  #0x200000, d1           | d1 to 2. bank
bank1:
        lsr.w   #2, d6

        movea.w #0x208, a0
        lea     after1(pc), a5          | save return address
        bra     memtest                 | memory test
after1:
        beq.s   nextbank                | ok 128k

        movea.w #0x408, a0
        lea     after2(pc), a5          | save return address
        bra     memtest                 | memory test
after2:
        beq.s   conf2mb                 | ok 512k

        movea.w #8, a0
        lea     after3(pc), a5          | save return address
        bra     memtest                 | memory test
after3:
        bne.s   nextbank

        addq.w  #4, d6
conf2mb:
        addq.w  #4, d6                  | configuration value to 2 mbyte

nextbank:
        sub.l   #0x200000, d1           | next bank
        beq.s   bank1                   | test for bank 1.
  
  
        move.b  d6, 0xffff8001          | program memorycontroller

        movea.l 8, a4                   | save bus-error vector
        lea     buserror(pc), a0        | adr. new  bus error routine

        move.l  a0, 8                   | set for test where memory bank ends
        move.w  #0xfb55, d3             | start-bitpattern
        move.l  #0x20000, d7            | startadr: 128 k
        movea.l d7, a0

teston:
        movea.l a0, a1
        move.w  d0, d2
        moveq   #0x2a, d1               | 43 worte

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
        dbra    d1, loopmem2            | test next word
        adda.l  d7, a0
        bra.s   teston                  | weiter testen


buserror:
        suba.l  d7, a0
        move.l  a0, d5
      	move.l	a4, 0x00000008          | restore old bus error vector
        movea.l d5, a0
        move.l  #0x400, d4              | lowest address for clearing

        movem.l emptylong(pc), d0-d3    | empty bytes for d-register clearing
clearmem:
        movem.l d0-d3, -(a0)            | delete 16 bytes
        cmpa.l  d4, a0                  | lowest address reached?
        bne.s   clearmem
        suba.l  a5, a5
  
| ==== Set memory configuration to sysvars ==================================

        move.b  d6, memctrl             | memctrl
        move.l  d5, phystop             | higest address as phystop
        move.l  #0x752019f3, memvalid   | set memvalid to ok
        move.l  #0x237698aa, memval2    | set memval2 to ok

aftermem:



| this is easier... (MAD)
|bus_error:
|      	move.l	a0, 0x00000008          | restore old bus error vector
|      	move.l	a1,sp			| and old stack pointer
|
|      	move.l	m_start,a0		| clear TPA
|      	move.l	m_length,d0		| get byte count
|      	lsr.l	#1,d0			| make word count
|      	subq.l	#1,d0			| pre-decrement for DBRA use
|      	clr.w	d1			| handy zero
|
|parity_loop:
|      	move.w	d1,(a0)+		| clear word
|      	dbra	d0,parity_loop
	

|        pea msg_mem     | Print, what's going on
|        bsr _kprint
|        addq #4,sp

| ==== Detect and set graphics resolution ===================================

        move.b 0xffff8260, d0     | Get video resolution from pseudo Hw
        and.b #3,d0             | Isolate bits 0 and 1
        cmp.b #3,d0             | Bits 0 and 1 set = invalid
        bne.s setscrnres        | no -->
        moveq #2,d0             | yes, set highres, make valid
setscrnres:
        move.b d0, sshiftmod    | Set in sysvar

        move.b #2, 0xffff8260     | Hardware set to highres
        move.b #2, sshiftmod    | Set in sysvar

|       jsr 0xfca7c4            | Init screen (video driver???)

        cmp.b #1, sshiftmod             | middle resolution?
        bne.s initmidres                | nein, -->
        move.w 0xffff825e, 0xffff8246   | Copy Color 16->4 kopieren

initmidres:
        move.l  #_main, swv_vec | Set Swv_vec (vector res change) to Reset
        move.w  #1, vblsem      | vblsem: VBL freigeben


|       pea 0xfffffa00  | Print, what's going on
|       bsr _kputb
|       addq #4,sp
|
|       btst    #7,0xfffffa01   | detect b/w-monitor pin
|       beq     low_rez         | if bit set, color monitor 
|
|       move.l  #2,d0           | monochrome mode
|       bra     both_rez
|low_rez:
|       move.l #0,d0
|both_rez:
|       move.b d0, sshiftmod    | set mode sysvar
|       move.w d0, 0xFFFF8260   | and to shifter register

| ==== Set videoshifter address to screenmem ================================
        
        move.l  phystop, a0     | get memory top
        sub.l   #0x8000, a0     | minus screen mem length
        move.l  a0, _v_bas_ad   | set screen base

        move.b  _v_bas_ad+1, 0xffff8201 | set hw video base high word
        move.b  _v_bas_ad+2, 0xffff8203 | set hw video base low word


        pea msg_shift   | Print, what's going on
        bsr _kprint
        addq #4,sp

| ==== Set memory width to sysvars ==========================================

        move.l  os_end, end_os          | end_os
        move.l  end_os, _membot         | end_os to _membot
        move.l  os_beg, exec_os         | exec_os
        move.l  _v_bas_ad, _memtop      | _v_bas_ad to _memtop

| ==== Set memory config to initial memory descriptor =======================

        move.l  #TPASTART, m_start      | set up default values

        move.l  _memtop, d0             | calculate length of TPA
        sub.l   #TPASTART, d0
        move.l  d0, m_length            | set length of TPA in byte

        move.l  #_b_mdx, themd          | write addr of MDB to sysvar themd
                                        | defined in bios.c

| ==== Set all cpu-interrupts to dummy handler ==============================
| We currently are experiencing an unexpected cpu interrupt.  We will
| set the vector addresses of these interrupts to a known location.

	jsr init_exc_vec		| LVL: moved in vectors.s



| ==== Set unassigned user interrupts to dummy handler ======================
|      	move.l	SAVECT,-(sp)		| save software abort vector

	jsr init_user_vec		| LVL: moved in vectors.s

|      	move.l	(sp)+,SAVECT		| restore software abort vector

|	pea msg_main	| Print, what's going on
|	bsr _kprint
|	addq #4,sp



| ==== Clear RAM ============================================================
|        move.l  _membot, a0             | Set start of RAM
|clrbss:
|        clr.w   (a0)+                   | Clear actual word
|        cmp.l   _memtop, a0             | End of BSS reached?
|        bne     clrbss                  | if not, clear next word
|
|        pea msg_clrbss  | Print, what's going on
|        bsr _kprint
|        addq #4,sp

| ==== Setting up Line-a variables and clear screen =========================

        bsr _linea_init

        pea msg_linea  | Print, what's going on
        bsr _kprint
        addq #4,sp

| ==== vector setup =========================================================

        move.l #int_vbl, vec_vbl        | Vbl-interr
        move.l #int_hbl, vec_hbl        | Hbl-interr
        move.l #dummyaes, vec_aes       | Trap #2  (AES, almost dummy)
        move.l #bios, vec_bios          | Trap #13 (BIOS)
        move.l #xbios, vec_xbios        | trap #14 (XBIOS)
        move.l #int_linea, vec_linea    | Line-A
	jsr	init_acia_vecs		| LVL: moved to aciavecs.s
        move.l #just_rte, vec_divnull   | Division by zero to rte

| ==== disk related vectors =================================================

        move.l  #_drv_init, hdv_init    | Initialize Harddrive
        move.l  #_drv_rw, hdv_rw        | Read/write sectors
        move.l  #_drv_bpb, hdv_bpb      | Get BIOS parameter Block
        move.l  #_drv_mediach, hdv_mediach      | Dummy mediach (STonX)
        move.l  #_drv_boot, hdv_boot    | Get boot device

| ==== Some other vectors ===================================================

        move.l  #print_stat, prt_stat   |
        move.l  #print_vec, prt_vec     |
        move.l  #serial_stat, aux_stat  |
        move.l  #serial_vec, aux_vec    |
        move.l  #dump_scr, dump_vec     |

        move.w #0x8, nvbls              | nvbls
        st     _fverify                 | _fverify
        move.w #0x3, seekrate           | floppy seekrate = 3 ms
        move.l #diskbuf, _dskbufp       | _dskbufp
        move.w #-1, _dumpflg            | clear _dumpflg
        move.l #os_entry, _sysbase      | Set _sysbase to ROM-start
        move.l #save_area, savptr       | savptr for Trap dispatcher

        move.l #just_rts, etv_timer     | etv_timer (->RTS)
|       move.l #_criter1, etv_critic    | etv_critic
        move.l #just_rts, etv_term      | etv_term  (->RTS)


| ==== Clear VBL queue list =================================================
        lea _vbl_list, a0               | Get addr. of VBL-routine
        move.l a0, _vblqueue            | nach _vblqueue
        move.w #7, d0                   | Loop counter
clrvbl:
        clr.l (a0)+                     | Clear VBL-QUEUE
        dbra d0, clrvbl                 | Loop

|        bsr 0xfc21b4                   | MFP initialisieren
        move.w #1, vblsem               | don not execute vbl-routine


| ==== Test, if Cartridge of type 2 =========================================

|       moveq.l #2, d0          | after interrupts are enabled
|       bsr     cartscan

| ==== Now initialize the BIOS =============================================

        jsr     _biosinit                

| ==== Now initialize the BDOS =============================================

        jsr     _osinit

| ==== Now initialize the BDOS =============================================

        move    #0x2300,sr              | enable Interrupts 
        
        move.l  #_brkpt, 0x7c           | set nmi to do an illegal instruction
        move.l  #bios, 0xb4             | revector bios entry = trap #13

|               move.l  #defcrit, v101  | default crit error:        bios13, fnct 5
|               move.l  #defterm, v102  | terminate handler:         bios13, fnct 5
|               move.l  #fix_SR, 0x20   | privilege violation

        pea msg_test
        bsr _kprint
        addq #4,sp

| ==== Test, if Cartridge of type 3 ========================================

        moveq.l #3,d0              | just before GEMDOS starts!
        bsr     cartscan


| ==== Do some informational output ========================================

        move.l  phystop, -(sp)  | Show physical memory
        move.l  phystop, -(sp)  | Show physical memory
        pea     msg_physmem
        bsr     _cprintf
        addq    #8, sp

        move.l  _memtop, -(sp)   | Show start address of user memory
        move.l  os_end, -(sp) | Show start address of user memory
        pea     msg_usermem
        bsr     _cprintf
        add     #12, sp

        move.l  _v_bas_ad, -(sp)| Show start address of screen
        pea     msg_scradr
        bsr     _cprintf
        addq    #8,sp


| ==== Now really start the BDOS ===========================================

        jsr     _biosmain          | go and start our shell


| ==== Get lost forever... =================================================
everloop:
        bra     everloop                        | Halt for debugging


msg_cart:
        .ascii "BIOS: Cartridge has been initialized ...\n\0"
        .even







| ==== STonX reserved vector usage ==========================================
|       move.l #switch_ok,0x46e(a5)

        move #0x30,-(sp)
        trap #1         | Call GEMDOS
        addq #2,sp

        move.w 0x446(a5),d0     | Get _bootdev default boot device
        move d0,-(sp)
        move #0xe,-(sp)
        trap #1         | Call GEMDOS
        addq #4,sp




| ===========================================================================
| ==== dummy aes ==========================================================
| ===========================================================================

|
| implements only appl_init(), returns 0, to tell Mint that GEM
| is not running.
|

dummyaes:
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

        bra everloop            | Loop forever, if GEM call not implemented





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
        rts             | Just a dummy

| ==== just rte for divide by zero ==========================================
just_rte:
        rte		



| ==== Test memory ==========================================================

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



| ==== Check if memory configuration valid ==================================
memval:
        sub.l   a5, a5                  | clear a5
        cmp.l   #0x752019f3, memvalid   | magic in memvalid ?
        bne     memback
        cmp.l   #0x237698aa, memvalid   | magic in memval2 ?
memback:
        jmp (a6)                        | jump to saved return address



| ==== Int 0x68 - HBL interrupt =============================================
int_hbl:
        move.w  d0, -(sp)       | save d0
        move.w  2(sp), d0       | get status register from stack
        and.w   #0x0700, d0     | isolate just PIL-mask from sr
        bne     is_ipl0         | if IPL0, then end
        or.w    #0x0300, 2(sp)  | else set IPL3 in status register
        move.w  (sp)+, d0       | restore d0
is_ipl0:
        rte



| ==== Int 0x70 - VBL interrupt =============================================
| no video resolution change is done, nor pallette change

int_vbl:
|       pea msg_vbl
|       bsr _kprint
|       addq #4,sp

        addq.l  #1, _frclock            | increase num of happened ints
        subq.l  #1, vblsem              | check vblsem
        bmi     vbl_end                 | if VBl routine disabled -> end
        
        movem.l d0-d7/a0-a6, -(sp)      | save registers
        addq.l  #1, _vbclock            | count number of VBL interrupts
        
|        bsr    flopvbl                 | flopvbl routine not needed
        
        movem.l (sp)+, d0-d7/a0-a6      | save registers
vbl_end:
        addq.l  #1, vblsem      |
        rte




| ==== Int 0x118 - exception for keyboard interrupt =========================
| LVL: moved to aciavec.s





| ==== breakpoint for illegal instruction ===================================
_brkpt:
        illegal
        bra.s   _brkpt  | never return

| ==== Timer A interrupt handler ============================================
int_timera:
        rte             | Just a dummy

| ==== Timer B interrupt handler ============================================
int_timerb:
        rte             | Just a dummy

| ==== Timer C interrupt handler ============================================
int_timerc:
        addq.l  #1, _hz_200             | increment 200 Hz counter
                                        | check for 4th call
        movem.l d0-d7/a0-a6,-(sp)
        move.w  _timer_ms, -(sp)
        move.l  etv_timer, a0
        jsr     (a0)                    | jump to etc_timer routine
        addq.w  #2, sp                  | correct stack
        
        movem.l (sp)+,d0-d7/a0-a6
        bclr    #5, 0xfffffa11          | clear interrupt service bit
        rte
        

| ==== Timer D interrupt handler ============================================
int_timerd:
        rte             | Just a dummy

| ==== txerror ==============================================================
txerror:
        rte             | Just a dummy

| ==== txerror ==============================================================
txrint:
        rte             | Just a dummy

| ==== txerror ==============================================================
rxerror:
        rte             | Just a dummy

| ==== txerror ==============================================================
rcvint:
        rte             | Just a dummy



| ==== Critical error handler ===============================================
| Just sets D0 (return code) to -1, end ends the subroutine

_criter:
        move.l  _criter, -(sp)  | etv_critic on stack
_criter1:
        moveq.l #-1, d0         | Default error
        rts                     | jump back to routine
        

| ==== STonX - Native print routine for debugging ===========================
_print:
_printout:
        dc.w 0xa0ff     | Jump to native execution
        dc.l 0          | Printing subroutine
        rts



| ==== Use cartridge, if present ============================================
| get cartridge-type in d0

cartscan:
        lea     cart_base, a0
        cmp.l   #0xABCDEF42, (a0)+      | is cartridge present?
        bne     cartover                | no -> cartover
testtype:
        btst    d0, 4(a0)               | What type?
        beq     nextapp

        pea     msg_cart
        bsr     _kprint
        addq #4,sp

        movem.l d0-d7/a0-a6, -(sp)      | save registers
        move.l  4(a0), a0
        jsr     (a0)                    | execute app in cartridge
        movem.l (sp)+, d0-d7/a0-a6      | restore registers
nextapp:        
        tst.l   (a0)                    | another application?
        move.l  (a0), a0
        bne     testtype
cartover:
        rts
        
        


| ==== Read/write sectors ===================================================
_drv_rw:
        pea drv_rwmsg
        bsr _kprint
        addq #4,sp
        rts             | Just a dummy

drv_rwmsg:
        .ascii "BIOS: hdv_rw - Native Disk read/write\n\0"
        .even



| ==== Get the BIOS parameter block =========================================
_drv_bpb:
        pea drv_bpbmsg  | Print, what's going on
        bsr _kprint
        addq #4,sp
        rts             | Just a dummy

drv_bpbmsg:
        .ascii "BIOS: hdv_bpb - Got native Bios Parameter Block for drive\n\0"
        .even



| ==== Init the Harddrive ===================================================
_drv_init:
        pea drv_initmsg
        bsr _kprint
        addq #4,sp
        rts             | Just a dummy

drv_initmsg:
        .ascii "BIOS: Do dummy drv_init - Init the Harddrive (fake)\n\0"
        .even



| ==== Did the media (Floppy) change? =======================================
_drv_mediach:   
        moveq #0,d0     | just a dummy 
        rts             | STonX can not change floppies (till now)



| ==== Boot from floppy/Disk ================================================
flopboot:
        move.l  hdv_boot, a0    | Get floppy boot vector
        jsr     (a0)            | Load boot sector
        tst.w   d0              | Executable?
        bne     rtnflop         | no -> that's it!
        lea     diskbuf, a0     | Get disk buffer
        jsr     (a0)            | Execute boot sector
rtnflop:        
        rts



| ==== Boot the Harddrive ===================================================
_drv_boot:      
        pea hdv_bootmsg
        bsr _kprint
        addq #4,sp

        move.w _bootdev, d0     | get boot device
        move d0,-(sp)
        move #0,-(sp)
        move #1,-(sp)
        move.l _dskbufp, -(sp)  | get pointer to 1k buffer for io
        move #0,-(sp)
        lea     hdv_rw, a0      | Get routines address
        jsr (a0)
        lea 12(sp),sp
        moveq #4,d0
        rts

hdv_bootmsg:
        .ascii "BIOS: Do hdv_boot - Boot from specific drive\n\0"
        .even



| ==== trap_1 - trap 1 (GEMDOS) entry point =================================

_trap_1:
        move.l  (sp)+,t1regsav  | save return address
        trap    #1              | call bdos call
        move.l  t1regsav,-(sp)  | restore return address
        rts

| ==== Trap 13 - BIOS entry point ==========================================

bios:
        move.w  bios_ent,d1
        lea     bios_vecs,a0
        bra.s   biosxbios

| ==== Trap 14 - XBIOS entry point =========================================

xbios:
        move.w  xbios_ent,d1
        lea     xbios_vecs,a0


| ==== Trap 13+14 handler ==================================================

biosxbios:
	move.l  savptr, a1     
        move.w  (sp)+,d0        | Status register -> d0
        move.w  d0,-(a1)        | and save in save_area
        move.l  (sp)+,-(a1)     | save return address
|       tst.w   _longframe      | Check _longframe sysvariable one day...
|       beq.s   bx_nolongframe  | ...when we support CPU >=68000
|       move.w  (sp)+,-(a1)
| bx_nolongframe:
        movem.l d3-d7/a3-a7, -(a1)      | regs, including stack pointer
        move.l  a1, savptr
        
        btst    #13,d0          | were we in user mode?
        bne     bx_sp_ok        | yes, the sp already points to the arguments
        move.l  usp,sp          | no, the arguments were on the user stack
bx_sp_ok:
        move.l  #0,d0           | clear d0 (use d0 like the original TOS does!)
        move.w  (sp)+,d0        | remove the function number from stack
        cmp.w   d1,d0           | Higher, than highest number?
        bge     bx_ret_exc
        add.l   d0,d0           | indirection function table is 1 LW per
        add.l   d0,d0           | so multiply function number by 4
        add.l   d0,a0           | add the offset to the base address of lookup table
        move.l  (a0),a0         | get the procedures address
        jsr     (a0)            | go do it and then come back

bx_ret_exc:
        move.l  savptr, a1
        movem.l (a1)+, d3-d7/a3-a7      | Get regs back, including sp
|       tst.w   _longframe      | Check longframe again: Is CPU >= 68000?
|       beq.s   bx_nolong2
|       move.w  (a1)+,-(sp)
| bx_nolong2:
        move.l  (a1)+,-(sp)
        move.w  (a1)+,-(sp)
        move.l  a1, savptr
        rte                     | return with return value in D0


| ==== Set userdefined exception vectors ====================================
| Function is trap13, function # 5 sets user defined exception vector

_bsetvec: 
        move    4(sp),a0        | discover the exception vector to set
        adda.l  a0,a0           | multiply that number by four cause 4 bytes
        adda.l  a0,a0           |       are needed to make up one address

        move.l  (a0),d0         | old vector to d0
        cmp.l   #-1,6(sp)       | Is argument = -1?
        beq     vsplt           | If yes, just return
        move.l  6(sp),(a0)      | set new vector
vsplt:  
        rts                     | return with d0 = old vector


forever:
        jmp     forever




| ===========================================================================
| ==== DATA in text segment =================================================
| ===========================================================================

| ==== BIOS entry points to allow individual module compilation =============


bios_vecs:
        .dc.l   _bios_0
        .dc.l   _bios_1                 | LONG character_input_status()
        .dc.l   _bios_2                 | LONG character_input()
        .dc.l   _bios_3                 | void character_output()
        .dc.l   _bios_4                 | LONG read_write_sectors()
        .dc.l   _bios_5                 | set vector
| LVL        .dc.l   _bsetvec                | set vector
        .dc.l   _bios_6                 | LONG get_timer_ticks()
        .dc.l   _drv_bpb                | get disk parameter block address
|       .dc.l   _bios_7                 | get disk parameter block address
        .dc.l   _bios_8                 | LONG character_output_status(h)
        .dc.l   _bios_9                 | media change?
        .dc.l   _bios_a                 | what drives exist?
        .dc.l   _bios_b                 | get/set alt-ctrl-shift status
        .dc.l   _bios_c
        .dc.l   _bios_d
        .dc.l   _bios_e
        .dc.l   _bios_f
        .dc.l   _bios_10

bios_ent:               | Max. number of BIOS entries
        .dc.w   (bios_ent-bios_vecs)/4


| ==== XBIOS entry points to allow individual module compilation ============

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

xbios_ent:              | Max. number of XBIOS entries
        .dc.w   (xbios_ent-xbios_vecs)/4

        .even

| ==== Some messages ========================================================
cr:
        .ascii "\n\0"

msg_start:
        .ascii "BIOS: Starting up EmuTOS Ver. 0.0 ...\n"
        dc.w 0
msg_main:
        .ascii "BIOS: Entered supervisor mode...\n\0"
msg_shift:
        .ascii "BIOS: Initialized shifter ...\n\0"
msg_clrbss:
        .ascii "BIOS: Cleared RAM ...\n\0"
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
        

msg_physmem:
        .ascii "[ OK ] Found %ld (0x%lx) byte of physical memory ...\r\n\0"
msg_usermem:
        .ascii "[ OK ] User memory from 0x%lx to 0x%lx ...\r\n\0"
msg_scradr:
        .ascii "[ OK ] Screen starts at address 0x%lx ...\r\n\0"


int_test:
        dc.l    0x88888888
        dc.w    0
gemdosmsg:
        .ascii "GEMDOS call\n\0"
        .even

| ==== Videopalette for shifter =============================================
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


| ===========================================================================
| ==== End ==================================================================
| ===========================================================================

        .end




