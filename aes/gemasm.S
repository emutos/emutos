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
 * in a standard RTS stack frame, "in preparation for an RTS that
 * will start this process executing".
 * That RTS will occur at the end of switchto()
 */

_psetup:
#ifdef __mcoldfire__
        move.w  sr,d0
        move.w  d0,savesr2
        ori.l   #0x0700,d0
        move.w  d0,sr
#else
        move    sr,savesr2
        ori     #0x0700,sr
#endif
        movea.l 4(sp),a0        // PD *
        move.l  8(sp),d0        // New process entry point
        movea.l PD_UDA(a0),a0
        movea.l UDA_SPSUPER(a0),a1
        move.l  d0,-(a1)        // Push entry point on the process stack
        move.l  a1,UDA_SPSUPER(a0)
#ifdef __mcoldfire__
        move.w  savesr2,d0
        move.w  d0,sr
#else
        move    savesr2,sr
#endif
        rts


/*
 * gotopgm(void)
 * launches the top of rlr list, as if called from within function
 * back( basepage * )
 * This the entry point for accessories processes.
 * It is called in supervisor mode, on the private AES stack of the new
 * accessory process. Its responsability is to go to the start of the
 * accessory TEXT segment in user mode. The register a0 must contain the
 * address of the basepage. The value of the other registers is not
 * important.
 */

_gotopgm:
        // get the basepage for the top of rlr
        movea.l _rlr,a0
        movea.l PD_UDA(a0),a2
        movea.l PD_LDADDR(a0),a0        // The basepage must stay in a0

        // set the user stack to the top of BSS for this process
        movea.l P_BBASE(a0),a1
        adda.l  P_BLEN(a0),a1
        // ??? adda.l  0x20(a0),a1  why add P_XDTA also ???

        // push basepage and return address
        // FIXME: Are we clobbering the end of the BSS segment ?
        move.l  a0,-(a1)
        move.l  #_back,-(a1)
        move.l  a1,usp

        // *** WARNING ***
        // We are currently on the top of the private stack for the
        // new accessory process. On the next trap #2 for AES, the CPU
        // will use this stack when going to supervisor mode, and this
        // will be perfectly correct.
        // Unfortunately, the AES trap prologue always switch to
        // rlr->p_uda->u_spsuper even if it is already on that stack.
        // Thus, if we don't take care, the stack pointer may go up in
        // the next trap #2 and cause data corruption.
        // We must fix rlr->p_uda->u_spsuper here to ensure this will
        // not happen. We have to estimate the amount of data stored
        // on this stack when the stack switch occurs in the trap.
        // Currently, there is only the trap stack frame, which is CPU
        // dependent and may be 6 or 8 bytes. Thus 8 bytes of additional
        // room will be enough in all cases.
        // You must keep an eye on the trap #2 prologue for AES.
        // It is located in gemdosif.S, label "trapaes".
        // If some additional data is pushed on the stack before
        // the stack switch, you must add the equivalent room here.

        move.l  sp,d0   // Current stack
        subq.l  #8,d0   // Room for the trap stack frame or more
        move.l  d0,UDA_SPSUPER(a2)      // Will be the stack on next trap

#ifdef __mcoldfire__
        move.w  sr,d0
        bclr    #13,d0                  // Switch to user mode
        move.w  d0,sr
#else
        andi.w  #0xdfff,sr              // Switch to user mode
#endif
        move.l  P_TBASE(a0),a1          // Start of the TEXT segment
        jmp     (a1)


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
#ifdef __mcoldfire__
        move.b  _indisp,d0
        addq.l  #1,d0
        move.b  d0,_indisp

        move.w  sr,d0
        move.w  d0,savesr0
        ori.l   #0x0700,d0
        move.w  d0,sr
#else
        addq.b  #1,_indisp

        move    sr,savesr0
        ori     #0x0700,sr
#endif

        move.l  a6,-(sp)        // Temporary backup
        movea.l _rlr,a6         // Get pointer to rlr PD structure
        movea.l PD_UDA(a6),a6   // Get pointer to UDA structure
        move.l  (sp)+,UDA_REG_A6(a6)    // Save old a6 in UDA struct
        movem.l d0-a5,UDA_REGS(a6)      // Save registers d0-a5 in UDA struct
        move.l  usp,a5
        move.l  a5,UDA_SPUSER(a6)       // Save old usp in UDA struct
        move.l  sp,UDA_SPSUPER(a6)      // Save old ssp in UDA struct

        lea     gemasm_stack,sp
#ifdef __mcoldfire__
        move.w  savesr0,d0
        move.w  d0,sr
#else
        move    savesr0,sr
#endif
        jmp     _disp


_switchto:
#ifdef __mcoldfire__
        move.w  sr,d0
        move.w  d0,savesr0
        ori.l   #0x0700,d0
        move.w  d0,sr
#else
        move    sr,savesr0
        ori     #0x0700,sr
#endif

        movea.l 4(sp),sp        // Use SP as pointer to UDA struct
        movem.l UDA_REGS(sp),d0-a5
        movea.l UDA_SPUSER(sp),a6
        move.l  a6,usp
        movea.l UDA_REG_A6(sp),a6
        movea.l UDA_SPSUPER(sp),sp

        clr.b   _indisp
#ifdef __mcoldfire__
        move.w  savesr0,d0
        move.w  d0,sr
#else
        move.w  savesr0,sr
#endif
        rts                     // Switch to the next process now !


        .bss

savesr0:
        .ds.w    1
savesr2:
        .ds.w    1

        .ds.b    0x0280
gemasm_stack:
        .ds.b    4
