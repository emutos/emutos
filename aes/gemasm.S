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


.text

/*
;
;       Push flags and code pointer onto a processes stack in preparation
;       for an IRET that will start this process executing
;
;       --->    ip
;               cs
;               flags
;              =======
;
;               VOID
;       psetup(p, codevalue)
;               PD              *p;
;               CODE            *codevalue;
;
*/
_psetup:
            move    SR,LEAC4
            ori     #0x0700,SR
            movea.l 4(SP),A0
            move.l  8(SP),D0
            movea.l 8(A0),A0
            movea.l 0x3E(A0),A1
            move.l  D0,-(A1)
            move.w  #0x2000,-(A1)
            move.l  A1,0x3E(A0)
            move    LEAC4,SR
            rts





_gotopgm:
L1DB4:
            move    SR,LEAC2
            ori     #0x0700,SR
            movea.l _rlr,A0
            movea.l 0x18(A0),A0
            movea.l 0x18(A0),A1
            adda.l  0x1C(A0),A1
            adda.l  0x20(A0),A1
            move.l  A0,-(A1)
            move.l  #_back,-(A1)
            move    A1,USP
            tst.w   _longframe
            beq     L1DEC
            move.w  #0,-(SP)        //??? Was:   move.w 0,-(sp)
L1DEC:
            move.l  8(A0),-(SP)
            move.w  LEAC2,-(SP)
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

            move    SR,LEAC0
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

            lea     LED46,SP
            move    LEAC0,SR
            jmp     _disp


_switchto:
L1E58:
            move    SR,LEAC0
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
            beq     L1E92
            move.w  (SP),-(SP)
            move.w  4(SP),2(SP)
            move.w  6(SP),4(SP)
            move.w  #0,6(SP)
L1E92:
            clr.b   _indisp
            move.w  LEAC0,(SP)

            rte


.bss


LEAC0:
            DS.B 2
LEAC4:
            DS.B 2

            DS.B 0x0280
LED46:
            DS.B 4
