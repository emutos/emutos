



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
