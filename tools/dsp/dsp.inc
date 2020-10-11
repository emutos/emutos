;
; dsp.inc - include file for EmuTOS DSP code
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

;
; hardware equates
; source: DSP56000/56001 User's Manual, Figure 10-5
;
PBC     equ     $ffe0        ; Port B Bus Control Register
HCR     equ     $ffe8        ; Host Control Register (r/w)
HSR     equ     $ffe9        ; Host Status Register (r/w)
HRX     equ     $ffeb        ; Host Receive Data Register (r)
HTX     equ     $ffeb        ; Host Transmit Data register (w)
BCR     equ     $fffe        ; Port A Bus Control Register
IPR     equ     $ffff        ; Interrupt Priority Register

;
; the following must be synchronised with the assembly of DSPPROG.ASM
;
PROGLOAD equ    $7ea9
PROGEND equ     $7f41
PROGSIZE equ    (PROGEND-PROGLOAD)

empty_vector macro
        nop
        nop
        endm
