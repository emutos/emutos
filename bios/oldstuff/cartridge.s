	dc.l $abcdef42
	dc.l 0
	dc.l $fa0000+$08000000+38
	dc.l $fa0000+38; +start-init
	dc.w 0
	dc.w 0
	dc.l 1024
	dc.b "UNIXDISK.ROM"
	dc.w 0

init:	
;	pea initmsg+$fa0000
;	bsr print
;	addq #4,sp
	bsr dinit
	; set _bootdev, _nflops and _drvbits
	dc.w $a0ff
	dc.l 9	;Initialize
	dc.w $a000
	dc.w $a0ff
	dc.l 10 ; Init_Linea
	move.l #disk_rw+$fa0000,$476.w	
	move.l #disk_bpb+$fa0000,$472.w
	move.l #hdv_init+$fa0000,$46a.w
;	move.l #boot+$fa0000,$47a.w
	move.l #mediach+$fa0000,$47e.w
	move.l #switch_ok+$fa0000,$46e.w
	move #$30,-(sp)
	trap #1
	addq #2,sp
	move.w $446.w,d0
	move d0,-(sp)
	move #$e,-(sp)
	trap #1
	addq #4,sp
start:	rts
mediach:	
	moveq #0,d0
	rts

switch_ok:
	rts

hdv_init:
;	pea hdvinitmsg+$fa0000
;	bsr print
;	addq #4,sp
	rts

boot:	
;	pea bootmsg+$fa0000
;	bsr print
;	addq #4,sp
	move.w $446.w,d0
	move d0,-(sp)
	move #0,-(sp)
	move #1,-(sp)
	move.l $4c6.w,-(sp)
	move #0,-(sp)
	bsr disk_rw
	lea 12(sp),sp
	moveq #4,d0
	rts

disk_bpb:
;	pea bpbmsg+$fa0000
;	bsr print
;	addq #4,sp
	dc.w $a0ff
	dc.l 2
	rts

dummy:	rts
disk_rw:
;	pea diskrwmsg+$fa0000
;	bsr print
;	addq #4,sp
	dc.w $a0ff
	dc.l 1
	rts

print:
	dc.w $a0ff
	dc.l 0
	rts

dinit:
	dc.w $a0ff
	dc.l 3
	dc.w $a000
	rts

move_native:
	dc.w $a0ff
	dc.l 4
	rts

initmsg:
	dc.b "cartridge_init\n"
	dc.w 0
hdvinitmsg:
	dc.b "hdv_init\n"
	dc.w 0
bootmsg:
	dc.b "hdv_boot\n"
	dc.w 0
diskrwmsg:
	dc.b "disk_rw\n"
	dc.w 0
bpbmsg:
	dc.b "hdv_bpb\n"
	dc.w 0

	org 0x400
old_gemdos:
dc.l	0
unixfs_gemdos:
;	pea gemdosmsg
;	bsr print
;	addq #4,sp
	dc.w $a0ff
	dc.l 5
	bvs.s pexec
	bne.s go_oldgemdos
	rte
go_oldgemdos:
	move.l old_gemdos,a0
	jmp (a0)
pexec:
	lea 8(sp),a0
	btst #5,(sp)
	bne.s s_ok
	move.l usp,a0
	addq #2,a0
s_ok:
	tst (a0)
	bne.s no_0
	move.l a6,-(sp)
	move.l a0,a6
	bsr find_prog
	bsr pexec5
	bsr reloc
	clr.l 2(a6)
	clr.l 10(a6)
	move.l d0,6(a6)
	move #6,(a6)
	move.l (sp)+,a6
	bra go_oldgemdos
no_0:
	cmp #3,(a0)
	bne.s go_oldgemdos
	move.l a6,-(sp)
	move.l a0,a6
	bsr find_prog
	bsr pexec5
	bsr reloc
gohome:
	move.l (sp)+,a6
	rte
find_prog:
	move #$2f,-(sp)
	trap #1
	addq #2,sp
	move.l d0,a0
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l (a0)+,-(sp)
	move.l a0,-(sp)
	move #$17,-(sp)
	move.l 2(a6),-(sp)
	move #$4e,-(sp)
	trap #1
	addq #8,sp
	move.l (sp)+,a0
	move.l (sp)+,-(a0)
	move.l (sp)+,-(a0)
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
pexec5:
	move.l 10(a6),-(sp)
	move.l 6(a6),-(sp)
	clr.l -(sp)
	move #5,-(sp)
	move #$4b,-(sp)
	trap #1
	lea 16(sp),sp
	tst.l d0
	bmi.s pexecerr
	rts
pexecerr:
	addq #4,sp
	bra.s gohome
reloc:
	movem.l a3-a5/d6-d7,-(sp)
	move.l d0,a5
	clr -(sp)
	move.l 2(a6),-(sp)
	move #$3d,-(sp)
	trap #1
	addq #8,sp
	move.l d0,d6
	move.l a5,-(sp)
	add.l #228,(sp)
	pea $1c.w
	move d6,-(sp)
	move #$3f,-(sp)
	trap #1
	lea 12(sp),sp
; check size!!
	move.l a5,-(sp)
	add.l #256,(sp)
	pea $7fffffff
	move d6,-(sp)
	move #$3f,-(sp)
	trap #1
	lea 12(sp),sp
	move d6,-(sp)
	move #$3e,-(sp)
	trap #1
	addq #4,sp
	lea 8(a5),a4
	move.l a5,d0
	add.l #$100,d0
	move.l d0,(a4)+		; text start
	move.l 230(a5),d0
	move.l d0,(a4)+		; text length
	add.l 8(a5),d0		; data start
	move.l d0,(a4)+
	move.l 234(a5),(a4)+	; data length
	add.l 234(a5),d0
	move.l d0,(a4)+		; bss start
	move.l 238(a5),(a4)+	; bss length
	move.l a5,d0
	add.l #$80,d0
	move.l d0,32(a5)
	move.l 24(a5),a4
	add.l 242(a5),a4	; symbol table length
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
	dc.b "GEMDOS call\n"
	dc.w 0

	org 0xff8
	dc.b "XBRASTNX"
old_trap2:
	dc.l 0
new_trap2:
	cmp.l #115,d0
	bne.s trap2_no
;	dc.w $a0ff
;	dc.l 8
;	beq.s trap2_back
	pea hiho+$fa0000
	move sr,-(sp)
trap2_no:
	move.l old_trap2+$fa0000,a0
	jmp (a0)
hiho:
	dc.w $a0ff
	dc.l 6
;	cmp.b #2,$44c
;	bne.s trap2_back
;	movem.l d0/a1-a6,-(sp)
;	move.l d1,a6
;	move.l (a6),a5
;	cmp #1,(a5)
;	bne.s trap2_back0
;	move.l 12(a6),a6
;	dc.w $a000
;	move.w (a6),d0
;	addq #1,d0
;	move d0,-12(a0)
;	lsr.w #3,d0
;	move.w d0,2(a0)
;	move.w d0,-40(a0)
;	subq #1,d0
;	move.w d0,-44(a0)
;	move.w 2(a6),d0
;	addq #1,d0
;	move d0,-4(a0)
;	lsr.w #4,d0
;	subq #1,d0
;	move.w d0,-42(a0)
trap2_back0:
;	movem.l (sp)+,d0/a1-a6
trap2_back:
	rte

	org 0x1200
old_linea:	dc.l 0
new_linea:
;	cmp.b #2,$ff8260
;	bne.s linea_back0
	move.l $28.w,-(sp)
	pea hiho1+$fa0000
	move sr,-(sp)
	move.l #linea_back+$fa0000,$28.w
linea_back0:
	move.l old_linea+$fa0000,a0
	jmp (a0)
hiho1:
	dc.w $a000
	move.l (sp)+,$28.w
	dc.w $a0ff
	dc.l 7
	addq.l #2,2(sp)
linea_back:
	rte

	org 0x1400
; resvector routine, needed for manual memory configuration if larger screen
; is used
resvec:
	dc.w $a0ff
	dc.l 11
	clr.l $426.w
	jmp (a6)

; here comes the STonX configuration menu...
	org 0x2000
	movem.l d0-a6,-(sp)
	moveq #1,d7
disp:
	move.l #conf_text+$fa0000,a6
	pea vt52_clear+$fa0000
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
	pea i_on+$fa0000
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
	pea i_off+$fa0000
	move #9,-(sp)
	trap #1
	addq #6,sp
do2:
	pea crlf+$fa0000
	move #9,-(sp)
	trap #1
	addq #6,sp
	bra.s l1
do1:
	move #7,-(sp)
	trap #1
	addq #2,sp
	cmp.l #$00480000,d0
	bne.s nx1
	cmp #1,d7
	beq.s do1
	subq #1,d7
	bra disp
nx1:
	cmp.l #$00500000,d0
	bne.s nx2
	cmp d6,d7
	beq.s do1
	addq #1,d7
	bra.s disp
nx2:
	cmp.l #$001c000d,d0
	bne.s do1
	cmp #1,d7
	beq.s ret0
	bra do1
ret0:
	movem.l (sp)+,d0-a6
	rte
conf_text:
	dc.l $fa0000+t_return
	dc.l $fa0000+t_warm
	dc.l $fa0000+t_cold
	dc.l $fa0000+t_quit
	dc.l 0
t_warm:
	dc.b "Warm reset"
	dc.w 0
t_cold:
	dc.b "Cold reset"
	dc.w 0
t_quit:
	dc.b "Quit STonX"
	dc.w 0
t_return:
	dc.b "Return to emulator"
	dc.w 0
i_on:
	dc.l $1b700000
i_off:
	dc.l $1b710000
crlf:
	dc.l $0a0d0000
vt52_clear:
	dc.l $1b450000
