;
; dspvect.asm - source for DSP vector table
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
;   asm56000.ttp -a -bdspvect.cld -ldspvect.lst -v -z dspvect.asm
;   cldlod.ttp dspvect.cld >dspvect.lod
;   lod2c.ttp dspvect.lod dspvect.c
;
; [asm5600.ttp / cldlod.ttp are from the standard Falcon development
; package; lod2c.ttp is a tool in tools/]
;

;
; this is the skeleton for the vector table loaded by Dsp_LoadProg() at
; the end of loading a DSP user program.  it contains vectors $15->$1e.
;
        include "dsp.inc"

dummy_vector macro
        jsr     >0
        endm

;
; the following MUST be kept in sync with labels from dspprog.asm
;
BLOCKMOVE equ   $7edc
LOADSUB equ     $7eef
MEMLOAD equ     $7f00

        org     P:$2a

        jsr     >LOADSUB                ; vector $15: VECT_LOADSUB
        jsr     >BLOCKMOVE              ; vector $16: VECT_BLOCKMOVE
        jsr     >MEMLOAD                ; vector $17: valid until DSP subroutine is installed

        dup    7                        ; vectors $18-$1e: for remaining DSP subroutines
        dummy_vector
        endm

        end
