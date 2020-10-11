;
; dspboot.asm - source for DSP boot
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
;   asm56000.ttp -a -bdspboot.cld -ldspboot.lst -v -z dspboot.asm
;   cldlod.ttp dspboot.cld >dspboot.lod
;   lod2c.ttp dspboot.lod dspboot.c
;
; [asm5600.ttp / cldlod.ttp are from the standard Falcon development
; package; lod2c.ttp is a tool in tools/]
;

;
; this is loaded by the DSP56001 bootstrap ROM, using Dsp_ExecBoot()
; on the host side.  it has two functions:
;   1. initialise interrupt vectors for interrupts $12-$1e
;   2. read the DSPPROG program into high memory
;
        include "dsp.inc"
;
; the following MUST be kept in sync with labels from dspprog.asm
;
BLOCKMOVE equ   $7edc
LOADSUB equ     $7eef
MEMLOAD equ     $7f00

        org     P:0

        jmp     >start

        dup     19                      ; vectors $01-$13
        empty_vector
        endm

        jsr     >MEMLOAD                ; vector $14: valid until TriggerHC() is used
        jsr     >LOADSUB                ; vector $15: VECT_LOADSUB
        jsr     >BLOCKMOVE              ; vector $16: VECT_BLOCKMOVE
        jsr     >MEMLOAD                ; vector $17: valid until DSP subroutine is installed

        dup    7                        ; vectors $18-$1e: for remaining DSP subroutines
        empty_vector
        endm

        empty_vector                    ; vector $1f (illegal instruction)

;
; start of program
;
start:
        movep   #>4,X:<<HCR             ; set HCIE host command interrupt enable
        movep   #$0c00,X:<<IPR          ; set HOST COMMAND IPL to 2
        andi    #$fe,mr                 ; mask interrupt levels 0,1
        movep   #>1,X:<<PBC             ; turn on host interface
        movep   #0,X:<<BCR              ; set zero wait states for all memory
        move    #PROGLOAD,r0            ; start of receive area
        move    #<PROGSIZE,r1           ; number of DSP words to receive
        do      r1,EndRec
WaitRec
        jclr    #0,X:<<HSR,WaitRec      ; loop if data isn't yet available
        movep   X:<<HRX,P:(r0)+         ; store the data from the host
EndRec
WaitLoop
        jmp     <WaitLoop               ; busy loop when done

        end
