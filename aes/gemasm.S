/* GEMASM.S */
/*
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
*/                                                                       


.globl _psetup
.globl _gotopgm
.globl _dsptch
.globl _savestat
.globl _switchto


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
            movea.l 0x04(SP),A0
            move.l  0x08(SP),D0
            movea.l 0x08(A0),A1
            movea.l 0x3E(A1),A2
            move.l  D0,-(A2)
            move.w  #0x2000,-(A2)
            move.l  A2,0x3E(A1)
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
            tst.w   is68010
            beq     L1DEC
            move.w  0x00,-(SP)
L1DEC:
            move.l  0x08(A0),-(SP)
            move.w  LEAC2,-(SP)
            eori.w  #0x2000,(SP)
            rte

_dsptch:
L1D80:
            tst.b   _indisp
            beq     L1D8A
            rts
L1D8A:
            move    SR,-(SP)
            move.l  A0,-(SP)
            jmp     _disp



_savestat:
L1DFC:
            move    SR,LEAC0
            ori     #0x0700,SR
            tst.b   _indisp
            beq     L1E14
            unlk    A6
            movea.l (SP)+,A0
            rte
L1E14:
            link    A6,#0x00
            addq.b  #1,_indisp
            movea.l 0x08(A6),A6
            lea     0x3A(A6),A6
            movem.l D0-A5,-(A6)
            movea.l (SP),A5
            move.l  (A5),0x38(A6)
            move.l  0x04(A5),0x20(A6)
            lea     0x08(A5),A5
            move.l  A5,0x3C(A6)
            move    USP,A5
            move.l  A5,0x40(A6)
            movea.l SP,A6
            unlk    A6
            movea.l (SP),A0
            lea     LED46,SP
            move    LEAC0,SR
            jmp     (A0)


_switchto:
L1E58:
            move    SR,LEAC0
            ori     #0x0700,SR
            addq.l  #4,SP
            movea.l (SP),SP
            addq.l  #2,SP
            movem.l (SP)+,D0-A5
            movea.l 0x08(SP),A6
            move    A6,USP
            movea.l (SP)+,A6
            movea.l (SP),SP
            tst.w   is68010
            beq     L1E92
            move.w  (SP),-(SP)
            move.w  0x04(SP),0x02(SP)
            move.w  0x06(SP),0x04(SP)
            move.w  #0x00,0x06(SP)
L1E92:
            clr.b   _indisp
            move.w  LEAC0,(SP)
            rte



.data



.bss

is68010:
LEA30:
            DS.B 2

LEAC0:
            DS.B 2
LEAC4:
            DS.B 2

            DS.B 0x0280
LED46:
            DS.B 4
