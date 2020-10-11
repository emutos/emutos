;
; dspprog.asm - source for DSP program loader
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
;   asm56000.ttp -a -bdspprog.cld -ldspprog.lst -v -z dspprog.asm
;   cldlod.ttp dspprog.cld >dspprog.lod
;   lod2c.ttp dspprog.lod dspprog.c
;
; [asm5600.ttp / cldlod.ttp are from the standard Falcon development
; package; lod2c.ttp is a tool in tools/]
;

;
; this program acts as the interface to the host for loading programs
; and subroutines.  it is loaded during DSP initialisation by DSPBOOT,
; using Dsp_DoBlock() on the host side.  it remains in DSP memory
; between system resets, and provides subroutine load functions via
; host command $15.  however, to load a program, this must be restarted
; via the DSPSTART program.
;
; note: this source code has been adjusted to compile into exactly the
; same binary that Atari TOS uses, when compiled with the version of
; asm56000.ttp shipped with the Falcon Development Kit (v4.1.1).
;
        include "dsp.inc"

        org     P:PROGLOAD

        movep   #>1,X:<<PBC             ; turn on host interface
        movep   #0,X:<<BCR              ; set zero wait states for all memory
;
; each data section to load is preceded by 3 DSP words:
; memory type / start address / number of DSP words to transfer
;
WaitMemType
        jclr    #0,X:<<HSR,WaitMemType  ; wait for data
        clr     a
        movep   X:<<HRX,a1              ; A1 = memory type
        move    #>3,x1                  ; memtype 3 => end of loading
        cmp     x1,a
        jeq     <$0000                  ; end of loaded code: go execute it
WaitOrg
        jclr    #0,X:<<HSR,WaitOrg
        movep   X:<<HRX,r0              ; R0 = start address
WaitCount
        jclr    #0,X:<<HSR,WaitCount
        movep   X:<<HRX,r1              ; R1 = #words to transfer
        move    #>1,x1                  ; now decode memory type
        cmp     x1,a
        jeq     StoreX                  ; 1 => X
        move    #>2,x1
        cmp     x1,a
        jeq     StoreY                  ; 2 => Y
        jmp     StoreP                  ; otherwise (expect 0) => P
;
; store data into P memory
;
StoreP
        do      r1,EndStoreP
WaitP
        jclr    #0,X:<<HSR,WaitP
        movep   X:<<HRX,P:(r0)+
EndStoreP
        jmp     WaitMemType
;
; store data into X memory
;
StoreX
        do      r1,EndStoreX
WaitX
        jclr    #0,X:<<HSR,WaitX
        movep   X:<<HRX,X:(r0)+
EndStoreX
        jmp     WaitMemType
;
; store data into Y memory
;
StoreY
        do      r1,EndStoreY
WaitY
        jclr    #0,X:<<HSR,WaitY
        movep   X:<<HRX,Y:(r0)+
EndStoreY
        jmp     WaitMemType

;
; Block Mover
; This function is executed by triggering interrupt #$16 (VECT_BLOCKMOVE)
;
BLOCKMOVE:
        movep   #>1,X:<<PBC             ; turn on host interface
        movep   #>0,X:<<BCR             ; set zero wait states for all memory
WaitSrc
        jclr    #0,X:<<HSR,WaitSrc
        movep   X:<<HRX,r0              ; R0 = source address
WaitDst
        jclr    #0,X:<<HSR,WaitDst
        movep   X:<<HRX,r1              ; R1 = destination address
WaitBlockCount
        jclr    #0,X:<<HSR,WaitBlockCount
        movep   X:<<HRX,r2              ; R2 = length
        do      r2,EndCopy
        movem   P:(r0)-,a1              ; do the move
        movem   a1,P:(r1)-
EndCopy
        nop
        rti

;
; Subroutine Loader
; This function is executed by triggering interrupt #$15 (VECT_LOADSUB)
;
LOADSUB
        movep   #>1,X:<<PBC             ; turn on host interface
        movep   #>0,X:<<BCR             ; set zero wait states for all memory
WaitSubOrg
        jclr    #0,X:<<HSR,WaitSubOrg
        movep   X:<<HRX,r0              ; R0 = start address
WaitSubSize
        jclr    #0,X:<<HSR,WaitSubSize
        movep   X:<<HRX,r1              ; R1 = code length
        do      r1,EndLoadSub
WaitSubCode
        jclr    #0,X:<<HSR,WaitSubCode
        movep   X:<<HRX,P:(r0)+         ; transfer from host to P memory
EndLoadSub
        nop
        rti

;
; Debug - Memory downloader
; Note: this is a temporary routine that resides in subroutine BSS,
; i.e it is expected to start at $7f00.  it will be usable until the
; first subroutine has been installed; after that, probably not ...
;
MEMLOAD
        if      (MEMLOAD-$7f00)
        fail    'MEMLOAD must be at $7f00'
        endif
        movep   #>1,X:<<PBC             ; turn on host interface
        movep   #>0,X:<<BCR             ; set zero wait states for all memory
MemWait1
        jclr    #0,X:<<HSR,MemWait1
        clr     a
        movep   X:<<HRX,a1              ; A1 = memory type
        move    #>1,x1
        cmp     x1,a
        jeq     MemLoadX                ; 1 => download X memory
        move    #>2,x1
        cmp     x1,a
        jeq     MemLoadY                ; 2 => download Y memory
        jmp     MemLoadP                ; otherwise (expect 0) => download P memory
;
; download P memory
;
MemLoadP
        jclr    #0,X:<<HSR,MemLoadP
        movep   X:<<HRX,r0              ; R0 = start address
MemLoadPSize
        jclr    #0,X:<<HSR,MemLoadPSize
        movep   X:<<HRX,r1              ; R1 = count
        do      r1,EndMemLoadP
        movem   P:(r0)+,a1              ; transfer from P memory to A1
WaitLoadP
        jclr    #1,X:<<HSR,WaitLoadP
        move    a1,X:HTX                ; transfer from A1 to host
EndMemLoadP
        nop
        rti
;
; download X memory
;
MemLoadX
        jclr    #0,X:<<HSR,MemLoadX
        movep   X:<<HRX,r0              ; R0 = start address
MemLoadXSize
        jclr    #0,X:<<HSR,MemLoadXSize
        movep   X:<<HRX,r1              ; R1 = count
        do      r1,EndMemLoadX
        move    X:(r0)+,a1              ; transfer from X memory to A1
WaitLoadX
        jclr    #1,X:<<HSR,WaitLoadX
        move    a1,X:HTX                ; transfer from A1 to host
EndMemLoadX
        nop
        rti
;
; download Y memory
;
MemLoadY
        jclr    #0,X:<<HSR,MemLoadY
        movep   X:<<HRX,r0              ; R0 = start address
MemLoadYSize
        jclr    #0,X:<<HSR,MemLoadYSize
        movep   X:<<HRX,r1              ; R1 = count
        do      r1,EndMemLoadY
        move    Y:(r0)+,a1              ; transfer from Y memory to A1
WaitLoadY
        jclr    #1,X:<<HSR,WaitLoadY
        move    a1,X:HTX                ; transfer from A1 to host
EndMemLoadY
        nop
        rti

        if      (PROGEND-*)
        fail    'PROGEND has an incorrect value'
        endif

        END
