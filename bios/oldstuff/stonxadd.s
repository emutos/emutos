
| ==== Clear GEM-System variables ===========================================
	suba.l a5,a5
	movea.w #$93a,a0		; from end of system variables
	movea.l #$10000,a1		; up to 0x10000
	moveq #0,d0
	move.w d0,(a0)+ 		; clear
	cmpa.l a0,a1
	bne.s $fc01d0

| ==== Put video memory at end of physical memory ===========================

| ==== Test, if GEM is in memory (never will with my os) ====================
	lea $fc0008(pc),a0		; sonst Systemadressen für reines TOS verwenden
	move.l 4(a0),$4fa		; end_os
	move.l 8(a0),$4fe		; exec_os

| ==== STonX different vectors ==============================================
	move.l #prt_stat, 0x506(a5)	;
	move.l #prt_vec, 0x50a(a5)	;
	move.l #aux_stat,0x50e(a5)	;
	move.l #aux_vec,0x512(a5)	;
	move.l #dump_vec,0x502(a5)	;
	move.l 0x44e(a5),0x436(a5)	; _v_bs_ad to _memtop
	move.l 0x4fa(a5),0x432(a5)	; end_os to _membot

	lea    0x4db8, a7		; Setup system stackpointer

	move.w #0x8, 0x454(a5)		  ; nvbls
	st     0x444(a5)		; _fverify
	move.w #0x3, 0x440(a5)		  ; floppy seekrate = 3 ms
	move.l #0x167a, 0x4c6<a5)	; _dskbufp
	move.w #-1, 0x4ee(a5)		  ; clear _dumpflg
	move.l #_sysbase, 0x4f2(a5)	; Set _sysbase to ROM-start
	move.l #0x93a, 4a2(a5)		; savptr for BIOS


| ==== Set Exceptions to known handler ======================================
	lea $fc0a1a(pc),a1		; Show bomb routine
	adda.l #$2000000,a1		; #2 -#63
	lea 8,a0			; Pointer to exeptions
	move.w #$3d,d0			; d0 = loop counter
loop_ex:
	move.l a1, (a0)+		; Set exeption routine
	adda.l #$1000000, a1		; Increase Vectorno.
	dbra d0, #loop_ex
	move.l a3,$14			; Division durch Null to rte

| ==== STonX different vectors ==============================================
	move.l #$fc0634,$70(a5) 	; Vbl-interr
	move.l #$fc061e,$68(a5) 	; Hbl-interr
	move.l a3,$88(a5)		; Trap #2  (->RTE)
	move.l #$fc074e,$b4(a5) 	; Trap #13
	move.l #$fc0748,$b8(a5) 	; trap #14
	move.l #$fc9ca2,$28(a5) 	; Line-A
	move.l a4,$400(a5)		; etv_timer (->RTS)
	move.l #$fc0744,$404(a5)	; etv_critic
	move.l a4,$408(a5)		; etv_term  (->RTS)


	 lea $4ce(a5),a0	   ; Adr. VBLqueue
	 move.l a0,$456(a5)	   ; nach _vblqueue
	 move.w #7,d0		   ; Standart-
	 clr.l (a0)+		   ; VBL-QUEUE
	 dbra d0,$fc033e	   ; l”schen

	 bsr $fc21b4		   ; MFP initialisieren
	 moveq #2,d0		   ; INIT BIT
	 bsr $fc0596		   ; @Ca_Test(2)

----- Vedeoaufl”sung anpassen nach Monitortyp
	 move.b -$7da0,d0	   ; Vedeoaufl”sung
	 and.b #3,d0
	 cmp.b #3,d0		   ; 3=ungültig?
	 bne.s $fc0360		   ; nein -->
	 moveq #2,d0		   ; ja, dann highres setzen
	 move.b d0,$44c 	   ; in sysvar setzen
	 move.b -$5ff,d0	   ; MFP GIPIP, monomon
	 bmi.s $fc0386		   ; Kein Monochrom-Monitor ? dann -->
	 lea $fc0376(pc),a6
	 bra $fc0ce4
	 move.b #2,-$7da0	   ; highres setzen
	 move.b #2,$44c 	   ;	  "
	 jsr $fca7c4		   ; Bildschirmausgabe initialisieren
	 cmpi.b #1,$44c 	   ; mittlere Aufl. ?
	 bne.s $fc03a0		   ; nein, -->
	 move.w -$7da2,-$7dba	   ; Farbe 16->4 kopieren

	 move.l #$fc0020,$46e(a5)  ; Swv_vec auf Reset setzen
	 move.w #1,$452 	   ; vblsem: VBL freigeben

	 clr.w d0		   ; initbit
	 bsr $fc0596		   ; BSR Ca_Test(0)

	 move.w #$2300,sr	   ; Interrupts freigeben

	 moveq #1,d0		   ; initbit
	 bsr $fc0596		   ; BSR Ca_Test(1)

	 bsr $fc4b5a		   ; GEMDOS initialisieren

	 move.w $fc001e,-(a7)	   ; Erstellungsdatum im DOS-Format
	 move.w #$2b,-(a7)	   ; set_data
	 trap #1
	 addq.w #4,a7

	 bsr $fc048c		   ; Ca_Test(3) + Booten von Floppy
	 bsr $fc04a8		   ; vom DMA-BUS booten
	 bsr $fc0d20		   ; Reset-resistente PRGs ausführen
	 tst.w $482		   ; soll command.prg geladen werden ?
	 beq.s $fc03fe		   ; Nein -->
---- COMMAND.PRG laden
	 bsr $fc457c		   ; Ja, -> Cursor einschalten
	 bsr $fc0b14		   ; autoexec : Prgs im AUTO-Ordner ausführen
	 pea $fc0489(pc)	   ; Environmentstring ('')
	 pea $fc0489(pc)	   ; Kommandozeile ('')
	 pea $fc0476(pc)	   ; ('COMMAND.PRG')
	 clr.w -(a7)		   ; laden +starten
	 bra.s $fc045a		   ; goto EXEC




