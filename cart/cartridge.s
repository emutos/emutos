|
| cartridge.s - STonX native function interface cartridge
|
| Copyright (c) 2001, STonX Team - http://stonx.sourceforge.net
|
| Authors:
|  NIN   Marinos Yannikos
|  MAD   Martin Doering 
|  LVL   Laurent Vogel
|
| This file is distributed under the GPL, version 2 or at your
| option any later version.  See COPYING file for details.
|

| This assembler file can be compiled with the MiNT gcc (cross-)
| compiling toolchain.


|============================================================================
| We just have the text segment here

	.text
	.org 0x000

|============================================================================
| cart_beg - header of cartridge

cart_beg:
	dc.l 0xabcdef42
	dc.l 0                   | cartridge of type 0
	dc.l init+0x01000000     | cartridge init (24 bit + 1 byte)
	dc.l start               | program to execute
	dc.w 0                   | time (unused)
	dc.w 0                   | date (unused)
	dc.l cart_end-init       | calculated program length
	.ascii "UNIXDISK.ROM"
	dc.w 0


|============================================================================
| init - Initialization of STonX native functions

init:	
	pea initmsg
	bsr print
	addq #4,sp

        		| set _bootdev, _nflops and _drvbits
	dc.w 0xa0ff     | jump native 
	dc.l 3          | disk_init (from native.c)

	dc.w 0xa0ff     | jump native
	dc.l 9	        | Initialize (from native.c)
        
| LVL: either use the three lines, or comment them out completely.
|	dc.w 0xa000     | put right value in d0 for Init_Linea().
|	dc.w 0xa0ff     | jump native 
|	dc.l 10 	| Init_Linea (from native.c)
        
| now set up some patched vectors
	move.l #disk_rw, 0x476.w	
	move.l #disk_bpb, 0x472.w
	move.l #hdv_init, 0x46a.w
|	move.l #boot, 0x47a.w	| not needed?
	move.l #mediach, 0x47e.w
	move.l #vidchng, 0x46e.w

	move #0x30, -(sp)   | get TOS version (whatfor?!?)
	trap #1             | call GEMDOS
	addq #2, sp

	move.w 0x446.w, d0  | get the default boot drive from sysvar
	move d0, -(sp)      | put it on stack
	move #0xe, -(sp)    | set boot-drive function
	trap #1             | call GEMDOS
	addq #4, sp
                    
	rts

	.even
initmsg:
	.ascii "CART: init - cartridge initialization\n"
	dc.w 0



|============================================================================
| start - dummy start of cartridge program (not used - init does the work!)

start:	
	rts


|============================================================================
| mediach - detect media change (dummy - disk images do not change!)

mediach:	
	moveq #0,d0    | 0 = media never changes in STonX
	rts

|============================================================================
| vidchng - (swv_vec) vector for video resolution change (do nothing)

vidchng:
	rts



|============================================================================
| Initialize the harddisk stuff

hdv_init:
|	pea hdvinitmsg
|	bsr print
|	addq #4,sp
	rts

hdvinitmsg:
	.ascii "CART: hdv_init\n"
	dc.w 0



|============================================================================
| boot - load sector and boot from device (not used)

boot:	
|	pea bootmsg
|	bsr print
|	addq #4,sp

	move.w 0x446.w,d0     | get default boot device

	move d0,-(sp)         | put device on stack
	move #0,-(sp)         | begin of logical sector
	move #1,-(sp)         | sector count
	move.l 0x4c6.w,-(sp)  | get pointer to disk buffer
	move #0,-(sp)	      | rwflag: read
	bsr disk_rw
	lea 12(sp),sp         | repair stack

	moveq #4,d0           | return code (why 4 ??? - 0 means ok)
	rts

bootmsg:
	.ascii "CART: hdv_boot\n"
	dc.w 0



|============================================================================
| disk_bpb - get bios parameter block with drive characteristics

disk_bpb:
|	pea bpbmsg
|	bsr print
|	addq #4,sp

	dc.w 0xa0ff     | jump native 
	dc.l 2
	rts

bpbmsg:
	.ascii "CART: hdv_bpb\n"
	dc.w 0



|============================================================================
| native disk sector read/write (emulated disk)

disk_rw:
|	pea diskrwmsg
|	bsr print
|	addq #4,sp
	dc.w 0xa0ff     | jump native 
	dc.l 1
	rts

diskrwmsg:
	.ascii "CART: disk_rw\n"
	dc.w 0



|============================================================================
| write_native - function for STonX console output (no more implemented)

print:
	dc.w 0xa0ff     | jump native 
	dc.l 0          | write_native (from native.c)
	rts



|============================================================================
| unixfs - access to unix filesystem from GEMDOS

	.org 0x400
old_gemdos:     	| old vector
	dc.l	0       | zero out

unixfs:
|	pea gemdosmsg
|	bsr print
|	addq #4,sp

	dc.w 0xa0ff
	dc.l 5
	bvs.s pexec     	| V bit = 1
	bne.s go_oldgemdos      | Z bit = 0
	rte
go_oldgemdos:
|	pea oldgemdosmsg	
|	bsr print
|	addq #4,sp

	move.l old_gemdos,a0    | fallback to TOS
	jmp (a0)
pexec:
	lea 8(sp),a0           
	btst #5,(sp)           
	bne.s s_ok             
	move.l usp,a0
	addq #2,a0
s_ok:
	tst (a0)                | is the pexec mode = 0?
	bne.s mode3             | if mode not 0 (= load + go)
        
        			
mode0:                          | load, relocate and start program
	move.l a6,-(sp)         | save a6

	move.l a0,a6            | copy pointer to pexec arguments
	bsr find_prog           | does program exist
	bsr pexec5              | create a basepage for new process
	bsr reloc               | relocate program
	clr.l 2(a6)             | clear filename
	clr.l 10(a6)            | clear environment string
	move.l d0,6(a6)         | copy command arguments pointer

	move #4,(a6)		| pexec mode 4 for exec. prepared program

	move.l (sp)+,a6         | restore a6
        			| after finding, basepage creation and
	bra go_oldgemdos        |   relocation start prepared program

mode3:                          | just load and relocate program
	cmp #3,(a0)             | is the pexec mode = 3?
	bne.s go_oldgemdos      | if not, return (should never happen)

	move.l a6,-(sp)         | save a6

	move.l a0,a6            | copy pointer to pexec arguments
	bsr find_prog           | does program exist
	bsr pexec5              | create a basepage for new process
	bsr reloc               | relocate program
gohome:
	move.l (sp)+,a6         | restore a6
	rte
        
        
        
|============================================================================
| find the program using GEMDOS functions Fgetdta() and Fsfirst(fnam, attr)
find_prog:    
	move #0x2f,-(sp)        | Fgetdta()
	trap #1
	addq #2,sp
	move.l d0,a0            | Pointer to DTA in a0
	move.l (a0)+,-(sp)      | copy DTA to stack
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)      | this needs a valid basepage setting
	move.l (a0)+,-(sp)      | in gemdos 
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l a0,-(sp)
	move #0x17,-(sp)        | get attributes
	move.l 2(a6),-(sp)      | get filename
	move #0x4e,-(sp)        | Fsfirst(fnam, attr)
	trap #1
	addq #8,sp
	move.l (sp)+,a0
	move.l (sp)+,-(a0)	| restore 44 bytes DTA buffer
	move.l (sp)+,-(a0)      | from stack
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)      
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)
	tst.l d0
	beq.s findprog_ok
	addq #4,sp
	bra.s gohome
findprog_ok:
	rts
pexec5: 			| pexec mode 5 (allocate basepage)
	move.l 10(a6),-(sp)     | get environment
	move.l 6(a6),-(sp)      | get programm arguments
	clr.l -(sp)             | no command name
	move #5,-(sp)           | pexec mode=5 (allocate basepage)
	move #0x4b,-(sp)        | call pexec
	trap #1
	lea 16(sp),sp
	tst.l d0                | return code...
	bmi.s pexecerr          | allocation unsuccessful
	rts
pexecerr:
	addq #4,sp
	bra gohome
        
reloc:                          | load and relocate the code
	movem.l a3-a5/d6-d7,-(sp)
	move.l d0,a5
	clr -(sp)
	move.l 2(a6),-(sp)      | use filename and
	move #0x3d,-(sp)        | file open 
	trap #1
	addq #8,sp
	move.l d0,d6            | copy file-handle to d6

	move.l a5,-(sp)
	add.l #228,(sp)         | buffer for file-header
	pea 0x1c.w              | get just file-header
	move d6,-(sp)           | put file-handle on stack
	move #0x3f,-(sp)        | read just file-header from file
	trap #1
	lea 12(sp),sp
| check size!!
	move.l a5,-(sp)         | save a5
	add.l #256,(sp)         |
	pea 0x7fffffff          | get whole file
	move d6,-(sp)           | put file-handle on stack
	move #0x3f,-(sp)        | file read
	trap #1
	lea 12(sp),sp
	move d6,-(sp)           | put file-handle on stack
	move #0x3e,-(sp)        | file close
	trap #1
	addq #4,sp

	lea 8(a5),a4
	move.l a5,d0
	add.l #0x100,d0
	move.l d0,(a4)+		| text start
	move.l 230(a5),d0
	move.l d0,(a4)+		| text length
	add.l 8(a5),d0		| data start
	move.l d0,(a4)+
	move.l 234(a5),(a4)+	| data length
	add.l 234(a5),d0
	move.l d0,(a4)+		| bss start
	move.l 238(a5),(a4)+	| bss length
	move.l a5,d0
	add.l #0x80,d0
	move.l d0,32(a5)
	move.l 24(a5),a4
	add.l 242(a5),a4	| symbol table length
	move.l 8(a5),a3
	move.l a3,d0
	tst.w 254(a5)
	bne.s relocdone
	move.l (a4)+,d7
	beq.s relocdone
	add.l d7,a3
	moveq #0,d7
relloop0:
	add.l d0,(a3)
relloop:
	move.b (a4)+,d7
	beq.s relocdone
	cmp.b #1,d7
	bne.s no254
	lea 254(a3),a3
	bra.s relloop
no254:
	add.w d7,a3
	bra.s relloop0
relocdone:
	move.l 28(a5),d0
	beq.s cleardone
	move.l 24(a5),a0
clear:
	clr.b (a0)+
	subq.l #1,d0
	bne.s clear
cleardone:
	move.l a5,d0
	movem.l (sp)+,a3-a5/d6-d7
	rts
gemdosmsg:
	.ascii "GEMDOS call\n"
	dc.w 0

oldgemdosmsg:
	.ascii "GEMDOS fallback to TOS routines ...\n"
	dc.w 0


|============================================================================
| new_trap2 - entry point for nativly implemented VDI

	.org 0xff8
	.ascii "XBRASTNX"
old_trap2:
	dc.l 0
new_trap2:
	cmp.l #115,d0
	bne.s trap2_no
|	dc.w 0xa0ff
|	dc.l 8
|	beq.s trap2_back
	pea hiho
	move sr,-(sp)
trap2_no:
	move.l old_trap2,a0
	jmp (a0)
hiho:
	dc.w 0xa0ff
	dc.l 6

trap2_back:
	rte



|============================================================================
| resvec - resvector routine
| needed for manual memory configuration if larger screen is used

	.org 0x1400
resvec:
	dc.w 0xa0ff
	dc.l 11           | gemdos_post (from native.c)
	clr.l 0x426.w     | 0 = do not validate reset vector (bended)
	jmp (a6)



|============================================================================
| disp - the STonX configuration menu (not implemented yet)

	.org 0x2000
	movem.l d0-a6,-(sp)
	moveq #1,d7
disp:
	move.l #conf_text,a6
	pea vt52_clear
	move #9,-(sp)
	trap #1
	addq #6,sp
	moveq #0,d6
l1:
	move.l (a6)+,d5
	beq.s do1
	addq #1,d6
	cmp d6,d7
	bne.s pr
	pea i_on
	move #9,-(sp)
	trap #1
	addq #6,sp
pr:
	move.l d5,-(sp)
	move #9,-(sp)
	trap #1
	addq #6,sp
	cmp d6,d7
	bne.s do2
	pea i_off
	move #9,-(sp)
	trap #1
	addq #6,sp
do2:
	pea crlf
	move #9,-(sp)
	trap #1
	addq #6,sp
	bra.s l1
do1:
	move #7,-(sp)
	trap #1
	addq #2,sp
	cmp.l #0x00480000,d0
	bne.s nx1
	cmp #1,d7
	beq.s do1
	subq #1,d7
	bra disp
nx1:
	cmp.l #0x00500000,d0
	bne.s nx2
	cmp d6,d7
	beq.s do1
	addq #1,d7
	jmp.s disp
nx2:
	cmp.l #0x001c000d,d0
	bne.s do1
	cmp #1,d7
	beq.s ret0
	bra do1
ret0:
	movem.l (sp)+,d0-a6
	rte
conf_text:
	dc.l t_return
	dc.l t_warm
	dc.l t_cold
	dc.l t_quit
	dc.l 0
t_warm:
	.ascii "Warm reset"
	dc.w 0
t_cold:
	.ascii "Cold reset"
	dc.w 0
t_quit:
	.ascii "Quit STonX"
	dc.w 0
t_return:
	.ascii "Return to emulator"
	dc.w 0
t_test:
	.ascii "CART: ========= TEST ============"
	dc.w 0
i_on:
	dc.l 0x1b700000
i_off:
	dc.l 0x1b710000
crlf:
	dc.l 0x0a0d0000
vt52_clear:
	dc.l 0x1b450000

|============================================================================
| cart_end - End of cartridge

cart_end:
	dc.l 0x0

	.end
