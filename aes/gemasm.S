/* GEMASM.S */
/*
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
*/                                                                       


.globl _psetup
.globl _gotopgm
.globl _dsptch
.globl _switchto

.xdef _rlr

.xdef savesr

.text

/*
 * defines
 */

/* AES PD struct */
#define PD_UDA 0x08
#define PD_LDADDR 0x18

/* BDOS PD struct */
#define P_TBASE 0x08    // address of TEXT segment
#define P_BBASE 0x18    // address of BSS segment
#define P_BLEN 0x1C     // length of BSS
#define P_XDTA 0x20     // pointer to DTA buffer ???
        
/*
 * psetup(PD *p, void *codevalue)
 * arranges for codevalue to be pushed to the stack for process p,
 * in a 68000-only RTE stack frame, "in preparation for an RTE that
 * will start this process executing".
 * note: I suppose that the 68000-only RTE stack frame is converted
 * to a proper RTE stack frame in switchto before actually doing the RTE.
 */

_psetup:
            move    SR,savesr2
            ori     #0x0700,SR
            movea.l 4(SP),A0
            move.l  8(SP),D0
            movea.l PD_UDA(A0),A0
            movea.l 0x3E(A0),A1
            move.l  D0,-(A1)
            move.w  #0x2000,-(A1)
            move.l  A1,0x3E(A0)
            move    savesr2,SR
            rts



/*
 * gotopgm(void)
 * launches the top of rlr list, as if called from within function
 * back( basepage * ) 
 *
 */

_gotopgm:
        move    SR,savesr
        ori     #0x0700,SR

        // get the basepage for the top of rlr
        movea.l _rlr,a0
        movea.l PD_UDA(a0),a2
        movea.l PD_LDADDR(a0),a0

        // set the user stack to the top of BSS for this process
        movea.l P_BBASE(a0),a1
        adda.l  P_BLEN(a0),a1
        // ??? adda.l  0x20(A0),A1  why add P_XDTA also ???

        // push basepage and return address
        move.l  a0,-(a1)
        move.l  #_back,-(a1)
        move    a1,USP
            
        // jump to the start of TEXT segment, in user mode.
        tst.w   _longframe              // Is CPU>=68010 ?
        beq.s   gpgm_nolong
        subq.l  #2,0x3E(a2)             // Adjust stored stack pointer
        move.w  #0,-(SP)                // Additional word on stack for rte
gpgm_nolong:
        move.l  P_TBASE(a0),-(SP)
        move.w  savesr,-(SP)
        eori.w  #0x2000,(SP)
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

            move    SR,-(SP)           // We'll later return with a RTE

            move    SR,savesr0
            ori     #0x0700,SR

            move.l  A6,-(SP)

            movea.l _rlr,A6            // Get pointer to rlr PD structure
            movea.l 8(a6),A6           // Get pointer to UDA structure

            lea     0x3A(A6),A6
            movem.l D0-A5,-(A6)        // Save registers D0-A5 in UDA struct
            move.l  (SP)+,0x38(A6)     // Save old A6 in UDA struct
            move.l  SP,0x3C(A6)        // Save old SSP in UDA struct
            move.l  USP,A5
            move.l  A5,0x40(A6)        // Save old USP in UDA struct

            lea     gemasm_stack,SP
            move    savesr0,SR
            jmp     _disp


_switchto:
            move    SR,savesr0
            ori     #0x0700,SR
            addq.l  #4,SP

            movea.l (SP),SP             // Restore register from UDA struct
            addq.l  #2,SP               // by abusing the SP as a index reg
            movem.l (SP)+,D0-A5
            movea.l 8(SP),A6
            move    A6,USP
            movea.l (SP)+,A6
            movea.l (SP),SP

            tst.w   _longframe
            beq     swto_68000
            move.w  (SP),-(SP)
            move.w  4(SP),2(SP)
            move.w  6(SP),4(SP)
            move.w  #0,6(SP)
swto_68000:
            clr.b   _indisp
            move.w  savesr0,(SP)

            rte


.bss


savesr0:
            ds.w 1
savesr2:
            ds.w 1

            DS.B 0x0280
gemasm_stack:
            DS.B 4
