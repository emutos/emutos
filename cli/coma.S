/*
 * coma.S - GEMDOS command interpreter assembly language interface
 *
 * Copyright (c) 1999 by Caldera Inc.
 * Copyright (c) 1985, 2001 by Authors:
 *
 *  JSL  Jason S. Loveman
 *  SCC  Steven C. Cavender
 *  MAD  Martin Doering
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



        .text
        .globl  _in_term
        .globl  _rm_term
        .globl  _cmain
        .globl  _div10
        .globl  _mod10
        .globl  _jmp_gemdos
        .globl  _jmp_bios
        .globl  _jmp_xbios
        .globl  _exeflg
        .globl  _jb
        .globl  _devector       // SCC  22 Mar 85
        .globl  _coma_start

// When compiling without AES, the CLI is our system shell:
#if WITH_AES == 0
        .globl  _ui_start
        .globl  _ui_mupb

_ui_start:
#endif

_coma_start:
        move.l  4(sp),a5
        move.l  #mystak,sp
        move.l  a5,-(sp)        // Save pointer to basepage on stack

// Shrink the TPA to a usefull size:
        move.l  0xc(a5),d0
        add.l   0x14(a5),d0
        add.l   0x1c(a5),d0
        add.l   #0x100,d0
        move.l  d0,-(sp)
        move.l  a5,-(sp)
        move.w  #0,-(sp)
        move.w  #0x4a,-(sp)
        trap    #1
        lea     12(sp),sp

#ifndef NO_ROM
// if running in ROM, go in supervisor mode to be able to 
// read our data and our code (switch tables)
        clr.l   -(sp)
        move.w  #0x20,-(sp)
        trap    #1
        //addq.w   #6,sp
        move.l  d0,save_ssp
#endif /* ! NO_ROM */
        
        move.l  #mycrit,-(sp)
        move    #0x101,-(sp)                                   
        move    #5,-(sp)
        trap    #13             // set/get critical error
        add.l   #8,sp
        move.l  d0,ocrit

        bsr     _in_term                                       

        jsr     _cmain          // leave basepage on stack
        addq.l  #4,sp

        bsr     _devector

#ifndef NO_ROM
// if running in ROM, switch back to user mode
        move.l  save_ssp,-(sp)
        move.w  #0x20,-(sp)
        trap    #1
        addq.w  #6,sp
#endif /* ! NO_ROM */

        // Pterm0()
        clr.l   -(sp)
        trap    #1




_devector:                                                     
        move.l  ocrit,-(sp)     // restore crit err vector      
        move.w  #0x101,-(sp)                                   
        move.w  #5,-(sp)                                       
        trap    #13                                            
        add.l   #8,sp                                          

        jsr     _rm_term                                       
        rts                                                    


_in_term:
        move.l  #myterm,-(sp)                                  
        move.w  #0x102,-(sp)                                   
        move.w  #5,-(sp)                                       
        trap    #13             // set/get terminate vector     
        add.l   #8,sp                                          
        move.l  d0,oterm                                       
        rts                                                    


_rm_term:                                                      
        move.l  oterm,-(sp)     // restore terminate vector     
        move.w  #0x102,-(sp)                                   
        move.w  #5,-(sp)                                       
        trap    #13                                            
        add.l   #8,sp                                          
        rts


myterm: cmp     #0,_exeflg      // is this my child's term
        beq     itsme

        rts

// I never terminate (need to distinguish 2nd level command.com (ie. batch)

itsme:
        andi.w  #0x5fff,sr      // return to user mode

        move    #1,-(sp)
        move.l  #_jb,-(sp)
        jsr     _longjmp

mycrit: move.l  #aris,a0
        jsr     bprt
        move    #2,-(sp)
        move    #2,-(sp)        // conin
        trap    #13
        addq.l  #4,sp
        and     #0x5f,d0        // upcase
        cmp.b   #'A',d0
        beq     acrit

        cmp.b   #'R',d0
        beq     rcrit

        cmp.b   #'I',d0
        bne     mycrit

// ignore the failure, continue processing
icrit:  clr.l   d0
        rts


// abort the offending process
acrit:  move.w  4(sp),d0
        ext.l   d0
        rts


// retry the operation
rcrit:  move    #1,d0
        swap    d0
        rts



bprt:   clr.l   d0
        move.b  (a0)+,d0
        cmp.b   #0,d0
        beq     nomoch
        move.l  a0,-(sp)
        move    d0,-(sp)
        move    #2,-(sp)        // device handle
        move    #3,-(sp)        // conout function
        trap    #13
        addq.l  #6,sp
        move.l  (sp)+,a0
        jmp     bprt
nomoch: rts


_jmp_bios:
        move.l  (sp)+,bios_ret
        trap    #13
        move.l  bios_ret,-(sp)
        rts


_jmp_xbios:
//        movem.l d0-d2/a0-a2, -(sp)      // save registers
        move.l  (sp)+,xbios_ret
        trap    #14
        move.l  xbios_ret,-(sp)
//        movem.l (sp)+, d0-d2/a0-a2      // save registers
        rts


_div10: link    a6,#0
        move.l  8(a6),d0
        divu    #10,d0
        swap    d0
        clr     d0
        swap    d0
        unlk    a6
        rts


_mod10: link    a6,#0
        move.l  8(a6),d0
        divu    #10,d0
        clr     d0
        swap    d0
        unlk    a6
        rts

// call dosjr from within itself (or from linked-in shell)
_jmp_gemdos:
        move.l  (sp)+,gemdos_ret
        trap    #1
        move.l  gemdos_ret,-(sp)
        rts


        .data
aris:
        .dc.b   13,10
        .ascii "(A)bort, (R)etry, or (I)gnore ?"
        .dc.b   0
        .even


// The CLI's MUPB structure - only used when compiling without the AES:
#if WITH_AES == 0
_ui_mupb:
        .dc.l   0x87654321      // Magic value
        .dc.l   coma_bss_end    // End of CLI's BSS (actually a dummy value)
        .dc.l   _coma_start     // Start of CLI code
#endif



        .bss

save_ssp:       .ds.l   1                                         

ocrit:          .ds.l   1
oterm:          .ds.l   1

/* Saved return addresses for jumps to BIOS, XBIOS, GEMDOS */
bios_ret:       .ds.l   1
xbios_ret:      .ds.l   1
gemdos_ret:     .ds.l   1

                .ds.l   800             // Stack space, growing backwards                         
mystak:         .ds.l   1               // Stack for command.prg


coma_bss_end:

        .end


