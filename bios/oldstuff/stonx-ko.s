
; ===========================================================================
; ==== stonx.a - Emulator specific startup module.  To be linked in first!
; ===========================================================================

; ===========================================================================
; ==== Textsegment ==========================================================
; ===========================================================================

	.org 0x00fc0000		; start of ROM-GEMDOS (TOS 1.0)
       	.text


; ==== References ===========================================================

       	.xref	_main		; OS entry point
       	.xref	_GSX_ENT
       	.xref	_trap_1		; Calling GEMDOS from C
	.xref	_sysbase


;#if	OLDCLOCK
;       	.globl	_ti
;       	.globl	_x1
;#else
;       	.xdef	_cl_init	;  vmetimer.c
;       	.xdef	_cl_intr	;  vmetimer.c
;#endif
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
       	.globl	saveret
       	.globl	_b_mdx
       	.globl	_tticks
       	.globl	_bsetvec

; ==== VME/10 Specifics =============
       	.globl	_no_device
       	.globl	_rddat
       	.globl	_wrdat
       	.globl	_supstk

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
       	.xdef	_bios_A
       	.xdef	_bios_B
       	.xdef	_bios_C
       	.xdef	_bios_D
       	.xdef	_bios_E
       	.xdef	_bios_F
       	.xdef	_bios_10

       	.xdef	_m400_isr
       	.xdef	_charvec

;#if	INTRGLITCH
;       	.xdef	_nullint
;#endif

       	.globl	_bcli
       	.globl	_bsti
       	.globl	_cmain			;file system init in main.c (cmain)
       	.globl	_criter


; ==== Defines ==============================================================

SUPSIZ		=	2048		; size of the supervisor stack (decl fs.h)

_GSX_ENT 	= 	0		; Entry to GEM (if graphically)

TPASTART 	=	$E000		; start address of tpa area
TPALENGTH 	=	$30000-TPASTART ; length of tpa area - from $30000 down to TPASTART

SAVECT		=	$138		; software abort vector


; ==== OSHEADER =============================================================

_sysbase:
	bra.s	_main		; os_entry, branch to _main in biosa.s
	.byte   1,0		; os_version, TOS version
	.long 	_main		; reseth, pointer to reset handler
	.long 	_sysbase	; os_beg, base of os = _sysbase
	.long 	0x6100		; os_end, first byte RAM not used by OS
	.long 	_main		; os_res1, reserved
	.long 	0x0		; os_magic, pointer to GEM's MUPB
	.long 	0x14062001      ; os_date, Date of system build
	.word 	0x0003    	; Flag for PAL version
	.word 	0x0c46    	; os_dosdate (wrong)


; ==== Get into supervisor mode =============================================
; This mechanism must be compatible for both, booting off of gemdos 
; (in which case we're already in supervisor mode) as well as entering Gemdos
; from CP/M (in which case we are not in supervisor mode) and it must not 
; violate the 68010 supervisor mode-only instructions.

_main:					; stunt to guarantee entry into supervisor mode
       	movem.l	d0-d7/a0-a7,regsav	; get present register values
       	move.l	#supmode, $b4		; revector trap #13 vector to point to me
       	trap	#13			; once get back to me then in supervisor mode

supmode: 				; now we're in supervisor mode & can mess w SR
	move	#$2700,sr		; disable interrupts
	reset				; Reset all hardware
	movem.l	regsav,d0-d7/a0-a7  	; restore orig regist to prev val
everloop:
	bra	everloop			; Halt for debugging

; ==== Cartridge 1 test =====================================================
	cmpi.l	#$abcdef42, $fa0000	; Is cartridge of type 1 installed?
        bne.s 	memvalid			; no -> memvalid
        lea 	memvalid, a6		; save return address
        jmp	$fa0000			; execute cartridge program

; ==== Memory configuration valid test ======================================
memvalid:

	
; ==== Reset vector =========================================================
;essetvec:
;       suba.l 	a5,a5			; Clear a5 for indirect adressing
;       cmpi.l 	#$31415926, $426(a5)	; Jump to resetvector?
;       bne.s 	initsnd			; No --> initsnd 
;       move.l 	$42a(a5), d0		; Yes: resvec to d0
;       tst.b 	$42a(a5)		; Is it valid?
;       bne.s 	initsnd			; No --> initsnd
;       btst 	#0, d0			; Address odd ?
;       bne.s 	initsnd			; Yes --> initsnd
;       movea.l d0, a0			; resvec
;       lea 	resetvec(pc), a6        ; save return address
;       jmp 	(a0)			; jump to resvec


; ==== Initialize Yamaha soundchip ==========================================
initsnd:


; ==== Initialize Video Shifter to PAL mode =================================
initpal:


; ==== Initialize Video Shifter palette and physbase ========================
initcol:


; ==== Find memory sizing ===================================================
initmem:


; ==== Test memory ==========================================================






; CDOS does this in their init code.  We'll try it too
;#if	CDOS_INIT
;	
;	and.b	#$fc,$f19f05		; mask off timer and dma interrupts
;	move.b	#$10,$f19f09		; reset the keyboard
;	clr.b	$f19f0b			; disable all VME bus interrupts
;	move.b	#$80,$f19f11		; allow SCM MPU interrupts
;#endif


; ==== Set unassigned user interrupts to BadInterrupt =======================
       	move.l	SAVECT,-(sp)		; save software abort vector

       	move.l	#$100,a0		; start with 1st user vector
       	move.l	#BadInterrupt,a1	; address of uninit inter handler
resintr: 
       	move.l	a1,(a0)+		; set new vector and bump ptr
       	cmp.l	#$400,a0		; have we reached #$400?
       	blt	resintr			; if not, get next user vector

       	move.l	(sp)+,SAVECT		; restore software abort vector


; ==== Set all cpu-interrupts to _nullint ===================================
; we currently are experiencing an unexpected cpu interrupt.  We will
; set the vector addresses of these interrupts to a known location.

	move.l	#0,a0
	move.l	_nullint,a1		;  in biosc.c
xresintr:
	move.l	a1,(a0)+                ; set vector, increase pointer
	cmp.l	#$100,a0                ; Last vector reached?
	blt	xresintr                ; if not, take next vector


; ==== Clear my bss =========================================================
       	move.l	#bssstrt,a0            	; Set start of BSS
clrmor:
       	clr.w	(a0)+                   ; Clear actual word
       	cmp.l	#$14000,a0              ; End of BSS reached?
       	bne	clrmor	                ; if not, clear next word

; ==== Initialize the hardware ==============================================

; ==== Revector the timer interrupt to me.(OLDCLOCK)
;_x1:
;       	move.l	#timerint,$130		; vector timer interrupt vector 
;       	move.w	#0,_tticks		; initialize timer
;       	ori.b	#$3,$f19f05		; enable timer interrupts
;       	move.b	$f1a09a,d0		; read and set VRT bit
;
; ==== Revector the timer interrupt to me.(VME-CLOCK)
;       	move.l	#_timerint,$130		; vector timer interrupt vector 
;       	move.w	#0,_tticks		; initialize timer
;       	jsr	_cl_init		; vmetimer.c


; ==== and the serial port interrupt 
;      	move.l	#_m400_isr,$114		; this is where INT4 goes


; ==== Memory initializations ===============================================
       	lea	_supstk+SUPSIZ,a6	; Setup Supervisor Stack
       	move.l	a6,sp


; ==== Find out memory size for TPA =========================================
; The memory configuration is found out by running into bus-errors, if memory
; is not present at a tested address. Therefor a special buserror-handler is
; temporarely installed.

       	move.l	8,a0			; save old bus error vector
       	move.l	#bus_error,$8		; and put in our temporary handler
       	move.l	sp,a1			; save the current SP

       	move.l	#TPASTART,m_start	; set up default values
       	move.l	#TPALENGTH,m_length	; for internal memory

       	move.w	#0,$180000		; do we have memory here?
       	move.l	#$180000,m_start	; yes, update MPB start
       	move.l	#$40000,m_length	; and length (for 256K initially)

       	move.w	#0,$1C0000		; memory at 2nd 256K boundary? 
	move.l	#$80000,m_length	; yes, assume 512K at this step

	move.w	#0,$200000		; memory at 3rd 256K boundary?
	move.l	#$C0000,m_length	; yes, assume 3/4M at this step

bus_error:
       	move.l	a0,$8			; restore old bus error vector
       	move.l	a1,sp			; and old stack pointer

       	move.l	m_start,a0		; clear TPA
       	move.l	m_length,d0		; get byte count
       	lsr.l	#1,d0			; make word count
       	subq.l	#1,d0			; pre-decrement for DBRA use
       	clr.w	d1			; handy zero

parity_loop:
       	move.w	d1,(a0)+		; clear word
       	dbra	d0,parity_loop
	
       	move.l	#_brkpt,$7c		; set nmi to do an illegal instruction
       	move.l	#bios,$b4		; revector bios entry = trap #13
       	move.l	#defcrit,v101		; default crit error:	     bios13, fnct 5
       	move.l	#defterm,v102		; terminate handler:	     bios13, fnct 5
       	move.l	#fix_SR,$20		; privilege violation
       	jsr	_biosinit		 

       	move	#$2000,sr
       	jsr	_cmain

_brkpt:
       	illegal
       	bra.s	_brkpt

; ==== bugger unknown trap so pick offstack pointer and pass to biosng
	.ifne 0
bugger:
	move.l	sp,-(sp)
	jsr	_biosng
hang:
	bra	hang
	.endc

; ==== This is the timer interrupt handler ==================================
; _ti: is here strictly so we have a symbol for the isr in the symtab

timerint:
_ti:
	movem.l	d0-d7/a0-a7, regsav
	move.b	$f1a099,d0	; read control register C to clear int
	bchg.b	#0,tiggle	; toggle on every other tick
	beq	ti_ret

	addq.l	#1,_tticks	; increment our counter
	move.l	_charvec+0xc,a0	; call user specified timer interrupt routine
	move.l	#31, -(sp)	; 2 x (15.625 ms clk rate rounded up to 16 ms)
	move.l	#1,-(sp)	; show packet received
	jsr	(a0)		; call user spec clock interrupt handler

ti_ret:
	movem.l regsav, d0-d7/a0-a7
	rte

; ==== _timerint - clock interrupt front end.================================
; this front end is different from the rest in that the clock isr 
; will call _timlox which will call the logical vector, if one has
; been set with b_setvec.
;_timerint:
;	addq.l	#1,_tticks	; increment our counter
;	movem.l	d0-d7/a0-a7, regsav
;	jsr	_cl_intr	;  vmetimer.c
;ti_xit:
;	movem.l regsav, d0-d7/a0-a7
;	rte


; because move from SR is privileged in 68010 we have this code
; to intercept privilege violations, look at see if it was a move
; from SR and if so change it to a move from CCR

fix_SR:
	movem.l	d0-d0/a0-a0,-(sp)	; saving d0 and a7 while we test
	movea.l	$a(sp),a0		; the offending address
	move.w	(a0),d0			; the offending instruction
	andi.w	#$ffc0,d0		; isolate move from SR opcode
	cmpi.w	#$40c0,d0		; we don't care where "To" was
	bne	not_lSR			; Not a MOVE SR,XX instruction
	ori.w	#$200,(a0)		; make it MOVE CCR,XX
	movem.l	(sp)+,d0-d0/a0-a0
	rte

; ERIC!!!! need tp gracefully handle exceptions like this

not_lSR:
	bra	not_lSR

; ==== 68010 this procedure MUST be called by the routine that is pointed at
; by an exception vector, it removes the trap frame type and leaves the stack
; looking just like a 68000

_fix_trap:
	move.w	8(sp),10(sp)	; lsb of return address on top of frame type
	move.w	6(sp),8(sp)	; msb of return address on top of lsb
	move.w	4(sp),6(sp)	; move status register  on top of msb
	move.w	2(sp),4(sp)	; fix_trap caller lsb
	move.w	0(sp),2(sp)	; fix_trap caller msb
	add.l	#2,sp		; discard the missing word to even the stack
	rts

; ==== This routine must be entered via BRA rather than executing an RTE
_fix_rte:
	move.w	#0,-(sp)	; reserve a hole on top of stack
	move.w	2(sp),0(sp)	; move SR out of the way
	move.w	4(sp),2(sp)	; msb of return address
	move.w	6(sp),4(sp)	; lsb of return address
	move.w	#0,6(sp)	; store a bogus exeception frame type
	rte

; Without considering wait states movep.l makes the faster loop. 
; However, accounting for wait states makes the 68010 loop mode faster.

; ==== read 128 bytes of data in a hurry
; C calling sequence:
;     rddat(char *where)

_rddat:
	move.l	4(sp),a0
	move.w	#127,d0
	move.l	#$f1c0d9,a1
rdlp:
	move.b	(a1),(a0)+
	dbf	d0,rdlp
	rts

; ==== writes 128 bytes of data in a hurry
; C calling sequence:
;     wrdat(char *where)

_wrdat:
	move.l	4(sp),a0		; where data to be written lives in mem
	move.w	#127,d0			; always transfers entire physical sector
	move.l	#$f1c0d9,a1		; where the disk interface lives
wrlp:
	move.b	(a0)+,(a1)		; repeatedly write the sector data bytes
	dbf	d0,wrlp			; decrement/test for complete
	rts


; ==== Determine if a memory address in fact is present
buserr	=	$8			; Bus error vector

_no_device:
	move.l	4(sp),a0	; byte memory address to test existence
	move.l	buserr,-(sp)	; save current bus error vector
	move.l	#ndber,buserr	; point at our routine
	move.b	(a0),d0		; try to read the test address
	moveq.l	#0,d0		; return false if memory exists
	move.l	(sp)+,buserr	; restore original bus error vector
	rts

; ====  where a buss error would take us

ndber:
	moveq.l #1,d0		; returns true if no device at that address
	add.l	#58,sp		; no need to restore all that stuff
	move.l	(sp)+,buserr	; but restore original occupant of vector
	rts			; return to original caller

; ==== BIOS entry points to allow individual module compilation =============

	.even
links:
	.dc.l	_bios_0
	.dc.l	_bios_1			; LONG character_input_status()
	.dc.l	_bios_2			; LONG character_input()
	.dc.l	_bios_3			; void character_output()
	.dc.l	_bios_4			; LONG read_write_sectors()
	.dc.l	_bsetvec
	.dc.l	_bios_6			; LONG get_timer_ticks()
	.dc.l	_bios_7			; get disk parameter block address
	.dc.l	_bios_8			; LONG character_output_status(h)
	.dc.l	_bios_9			; media change?
	.dc.l	_bios_A			; what drives exist?
	.dc.l	_bios_B			; get/set alt-ctrl-shift status
	.dc.l	_bios_C
	.dc.l	_bios_D
	.dc.l	_bios_E
	.dc.l	_bios_F
	.dc.l	_bios_10

entries:
	.dc.w	(entries-links)/4

defterm: rts

defcrit: rts


_criter:
	move	4(sp),d0		; error code
	move	savesr,-(sp)
	move.l	saveret,-(sp)
	move.l	savesp,-(sp)
	move	d0,-(sp)
	move.l	v101,a0
	move.l	#-1,d0			; abort(-1) is default
	jsr	(a0)
	addq.l	#2,sp
	move.l	d0,d1
	swap	d1
	cmp	#1,d1
	bne	okgo			; retry ?
	move.l	(sp)+,savesp		; yes, restore stuff
	move.l	(sp)+,saveret
	move	(sp)+,savesr
	rts
okgo:	
	move.l	(sp)+,a0
	move.l	(sp)+,-(a0)		; retadd
	move	(sp)+,-(a0)		; sr
	move.l	a0,sp
	bra	_fix_rte
        
; ==== trap_1 - trap 1 (GEMDOS) entry point =================================

_trap_1:
	move.l	(sp)+,t1regsav	; save return address
	trap	#1		; call bdos call
	move.l	t1regsav,-(sp)	; restore return address
		rts

; ==== trap_13 - trap 13 (BIOS) entry point =================================

bios:
	jsr	_fix_trap
	move.w	(sp)+,d0
	move.w	d0,savesr
	move.l	(sp)+,saveret	; save return address (within trap13)
	move.l	sp,savesp
	btst	#13,d0		; were we in user?
	beq	ret_exc		; yes, exit
	move.l	#0, a0		; clear a0
	move.w	(sp)+,a0	; remove the function number from stack
	cmp.w	entries,a0	; do we have an entry for it?
	bge	ret_exc
	add.l	a0,a0		; indirection function table is 1 LW per
	add.l	a0,a0		; so multiply function number by 4
	add.l	#links,a0	; add in the base address of lookup table
	move.l	(a0),a0		; get the procedures address
	jsr	(a0)		; go do it and then come back
ret_exc:
	move.l	savesp,sp	; restore original stack
	move.l	saveret,-(sp)
	move.w	savesr,-(sp)
	bra	_fix_rte	; return with function # in D0

; ==== Do nothing ===========================================================
_nop:	rts

; ==== Dummy routine ========================================================
medchk:	move.l	#1,d0		; media may have changed
	rts

; ==== Set userdefined exception vectors ====================================
; Function is trap13, function # 5 sets various user defined exception 
; vectors.  Currently, all vectors between 0x103 and 0x2ff are treated as 
; errors (= ignorred) which is different from the spec's definition.

_bsetvec: 
	move	4(sp),a0	; discover the exception vector to set
	adda.l	a0,a0		; multiply that number by four cause 4 bytes
	adda.l	a0,a0		;	are needed to make up one address
	cmpa.l	#$400,a0	; >= 400 if user defined extended excptn vect
	blt	norml		; branch if vector defined by 68010 hardware
	cmpa.l	#$408, a0	; is it 100h, 101h or 102h ?
	bgt	vsplt		; branch iff it is not 100h, 101h or 102h
	adda.l	#v100-0x400,a0	; determine which of the 3 it is
norml:	
	move.l	(a0),d0
	cmp.l	#-1,6(sp)
	beq	vsplt
	move.l	6(sp),(a0)
vsplt:	
	rts
_bcli:	
	move	#$2700,sr
	rts
_bsti:	
	move	#$2000,sr
	rts

BadInterrupt:
	rte



; ===========================================================================
; ==== Datasegment ==========================================================
; ===========================================================================
;	.data
;	.org 0x00fd0000		; start of ROM-GEMDOS (TOS 1.0)

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


; ===========================================================================
; ==== Bsssegment ===========================================================
; ===========================================================================
;	.bss
;	.org 0x00a000		; start of ROM-GEMDOS (TOS 1.0)

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
t1regsav:	.dc.l	1   
	

bssstrt: 	dc.w	1

;	.org 0x00fdffff		; end of ROM-GEMDOS (TOS)

; ===========================================================================
; ==== End ==================================================================
; ===========================================================================

	.end
