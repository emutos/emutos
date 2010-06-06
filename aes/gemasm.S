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

/* UDA struct */
#define UDA_REGS 0x02           // registers d0-a6
#define UDA_REG_A6 0x3A         // a6
#define UDA_SPSUPER 0x3E        // ssp
#define UDA_SPUSER 0x42         // usp

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
        movea.l UDA_SPSUPER(a0),a1
        move.l  d0,-(a1)
        move.w  #0x2000,-(a1)
        move.l  a1,UDA_SPSUPER(a0)
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
        // FIXME: Are we clobbering the end of the BSS segment ?
        move.l  a0,-(a1)
        move.l  #_back,-(a1)
        move    a1,usp

        // jump to the start of TEXT segment, in user mode.
        tst.w   _longframe      // Is CPU>=68010 ?
        beq.s   gpgm_nolong

	// The prologue of the AES trap will use this value as SP. It must
	// be equal to the value of SP just before the RTE instruction below.
	// Its initial value is correct for a 68000 CPU,
	// but it must be adjusted if the CPU has long stack frames.
        subq.l  #2,UDA_SPSUPER(a2)

        clr.w   -(sp)           // Format word
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

        move.l  a6,-(sp)        // Temporary backup
        movea.l _rlr,a6         // Get pointer to rlr PD structure
        movea.l PD_UDA(a6),a6   // Get pointer to UDA structure
        move.l  (sp)+,UDA_REG_A6(a6)    // Save old a6 in UDA struct
        movem.l d0-a5,UDA_REGS(a6)      // Save registers d0-a5 in UDA struct
        move.l  usp,a5
        move.l  a5,UDA_SPUSER(a6)       // Save old usp in UDA struct
        move.l  sp,UDA_SPSUPER(a6)      // Save old ssp in UDA struct

        lea     gemasm_stack,sp
        move    savesr0,sr
        jmp     _disp


_switchto:
        move    sr,savesr0
        ori     #0x0700,sr

        movea.l 4(sp),sp        // Use SP as pointer to UDA struct
        movem.l UDA_REGS(sp),d0-a5
        movea.l UDA_SPUSER(sp),a6
        move.l  a6,usp
        movea.l UDA_REG_A6(sp),a6
        movea.l UDA_SPSUPER(sp),sp

        tst.w   _longframe
        beq     swto_68000
        move.w  (sp),-(sp)      // SR
        move.w  4(sp),2(sp)     // High word of return address
        move.w  6(sp),4(sp)     // Low word of return address
        clr.w   6(sp)           // Format word
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
