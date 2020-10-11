;
; dspstart.asm - source for code to restart DSPPROG
;
; Copyright (C) 2020 The EmuTOS Development Team
;
; Authors:
;         Thorsten Otto
;   RFB   Roger Burrows
;
; This file is distributed under the GPL, version 2 or at your
; option any later version.  See doc/license.txt for details.
;
        page    255,255                 ;avoid line- & page-wrap in listing
;
; how to build the binary, formatted as C array initialisation code:
;   asm56000.ttp -a -bdspstart.cld -ldspstart.lst -v -z dspstart.asm
;   cldlod.ttp dspstart.cld >dspstart.lod
;   lod2c.ttp dspstart.lod dspstart.c
;
; [asm5600.ttp / cldlod.ttp are from the standard Falcon development
; package; lod2c.ttp is a tool in tools/]
;

;
; this program restarts the in-memory copy of DSPPROG so that the latter
; may load a program into DSP memory.
;
        include "dsp.inc"

        org     P:0

        jmp     >start

        dup     31                      ;vectors $01-$1f
        empty_vector
        endm

;
; Start of Program
;
start
        movep   #>4,X:<<HCR             ; set HCIE host command interrupt enable
        movep   #$0c00,X:<<IPR          ; set HOST COMMAND IPL to 3
        andi    #$fe,mr                 ; mask interrupt levels 0,1

        jmp     PROGLOAD
