/*
 * gemasm.S - assembler part of GEM
 *
 * Copyright 2002, The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



        .globl  _psetup
        .globl  _gotopgm
        .globl  _dsptch
        .globl  _switchto

        .extern _rlr
        .extern savesr
        .extern _back
        .extern _longframe
        .extern _indisp
        .extern _disp

        .text

/*
 * defines
 */

/* AES PD struct */
#define PD_UDA 0x08
#define PD_LDADDR 0x18

/* BDOS PD struct */
#define P_TBASE 0x08            // address of TEXT segment
#define P_BBASE 0x18            // address of BSS segment
#define P_BLEN 0x1C             // length of BSS
#define P_XDTA 0x20             // pointer to DTA buffer ???

/*
 * psetup(PD *p, void *codevalue)
 * arranges for codevalue to be pushed to the stack for process p,
 * in a 68000-only RTE stack frame, "in preparation for an RTE that
 * will start this process executing".
 * note: I suppose that the 68000-only RTE stack frame is converted
 * to a proper RTE stack frame in switchto before actually doing the RTE.
 */

_psetup:
        move    sr,savesr2
        ori     #0x0700,sr
        movea.l 4(sp),a0
        move.l  8(sp),d0
        movea.l PD_UDA(a0),a0
        movea.l 0x3E(a0),a1
        move.l  d0,-(a1)
        move.w  #0x2000,-(a1)
        move.l  a1,0x3E(a0)
        move    savesr2,sr
        rts



/*
 * gotopgm(void)
 * launches the top of rlr list, as if called from within function
 * back( basepage * )
 */

_gotopgm:
        move    sr,savesr
        ori     #0x0700,sr

        // get the basepage for the top of rlr
        movea.l _rlr,a0
        movea.l PD_UDA(a0),a2
        movea.l PD_LDADDR(a0),a0

        // set the user stack to the top of BSS for this process
        movea.l P_BBASE(a0),a1
        adda.l  P_BLEN(a0),a1
        // ??? adda.l  0x20(a0),a1  why add P_XDTA also ???

        // push basepage and return address
        move.l  a0,-(a1)
        move.l  #_back,-(a1)
        move    a1,usp

        // jump to the start of TEXT segment, in user mode.
        tst.w   _longframe      // Is CPU>=68010 ?
        beq.s   gpgm_nolong
        subq.l  #2,0x3E(a2)     // Adjust stored stack pointer
        clr.w   -(sp)           // Additional word on stack for rte
gpgm_nolong:
        move.l  P_TBASE(a0),-(sp)
        move.w  savesr,-(sp)
        eori.w  #0x2000,(sp)
        rte


/*
 * dsptch() is a machine (& compiler) dependent routine which:
 *      1) saves any flags that will be trashed
 *      2) if (indisp) restore flags, return to dsptch caller
 *      3) otherwise
 *              save machine state,
 *              jump to disp
 */


_dsptch:
        tst.b   _indisp
        beq     savestate
        rts

savestate:
        addq.b  #1,_indisp

        move    sr,-(sp)        // We'll later return with a RTE

        move    sr,savesr0
        ori     #0x0700,sr

        move.l  a6,-(sp)

        movea.l _rlr,a6         // Get pointer to rlr PD structure
        movea.l 8(a6),a6        // Get pointer to UDA structure

        lea     0x3A(a6),a6
        movem.l d0-a5,-(a6)     // Save registers d0-a5 in UDA struct
        move.l  (sp)+,0x38(a6)  // Save old a6 in UDA struct
        move.l  sp,0x3C(a6)     // Save old SSP in UDA struct
        move.l  usp,a5
        move.l  a5,0x40(a6)     // Save old usp in UDA struct

        lea     gemasm_stack,sp
        move    savesr0,sr
        jmp     _disp


_switchto:
        move    sr,savesr0
        ori     #0x0700,sr
        addq.l  #4,sp

        movea.l (sp),sp         // Restore register from UDA struct
        addq.l  #2,sp           // by abusing the sp as a index reg
        movem.l (sp)+,d0-a5
        movea.l 8(sp),a6
        move    a6,usp
        movea.l (sp)+,a6
        movea.l (sp),sp

        tst.w   _longframe
        beq     swto_68000
        move.w  (sp),-(sp)
        move.w  4(sp),2(sp)
        move.w  6(sp),4(sp)
        move.w  #0,6(sp)
swto_68000:
        clr.b   _indisp
        move.w  savesr0,(sp)

        rte


        .bss


savesr0:
        .ds.w    1
savesr2:
        .ds.w    1

        .ds.b    0x0280
gemasm_stack:
        .ds.b    4
