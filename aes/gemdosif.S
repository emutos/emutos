/*      GEMDOSIF.S      */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                 Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

.globl _cli
.globl _sti
.globl _hsti
.globl _hcli
.globl _giveerr
.globl _takeerr
.globl _retake
.globl _setdsss
.globl _justretf
.globl _givecpm
.globl _takecpm
.globl _back
.globl _far_mcha
.globl _far_bcha
.globl _drawrat

.globl _tikaddr
.globl _tiksav
.globl _CMP_TICK
.globl _NUM_TICK
.globl _drwaddr
.globl savesr
.globl _tikcod



/* disable interrupts */
_cli:
            move    SR,savesr
            ori     #0x0700,SR
            rts

/* restore interrupt pask as it was before cli() */
_sti:
            move    savesr,SR
            rts

_hcli:
            ori     #0x0700,SR
            rts

_hsti:
            andi    #0xF8FF,SR
            rts


/*
 DOS error trapping code.
*/
_retake:
L1F0C:
            move.l  #_aestrap,0x88
            move.l  #_err_tra,D0
L1F1C:
            move.l  D0,-(SP)
            move.w  #0x0101,-(SP)
            move.w  #0x05,-(SP)
            trap    #13
            addq.w  #8,SP
            rts
_giveerr:
L1F2C:
            move.l  _crit_er,D0
            bra     L1F1C
_takeerr:
L1F34:
            moveq   #-1,D0
            bsr     L1F1C
            move.l  D0,_crit_er
            move.l  #_err_tra,D0
            bra     L1F1C
_err_tra:
L1F46:
            move.w  0x04(SP),D0
            move.w  0x06(SP),D1
            movem.l D3-D7/A3-A6,-(SP)
            movea.l #LE3EE,A1
            move.w  D0,-(SP)
            bne     L1F62
            movea.w #0x00,A0
            bra     L1F70
L1F62:
            not.w   D0
            movea.w D0,A0
            cmp.w   #0x11,D0
            ble     L1F70
            movea.w #0x00,A0
L1F70:
            move.b  0x00(A0,A1.l),D0
            move.w  D1,-(SP)
            move.w  D0,-(SP)
            jsr     _eralert
            addq.l  #4,SP
            move.w  (SP)+,D1
            ext.l   D1
            cmp.w   #0x00,D0
            beq     L1F90
            move.l  #0x010000,D1
L1F90:
            move.l  D1,D0
            movem.l (SP)+,D3-D7/A3-A6
            rts



_back:
L211C:
        clr.w   -(SP)
        move.w  #0x4C,-(SP)   // Pterm
        trap    #1



// **** Trap entry: ****
_aestrap:
        cmpi.w  #0x00,D0
        beq     _back
        cmpi.w  #0xC8,D0
        beq     L2126
        cmpi.w  #0xC9,D0
        beq     L2126
        move.l  savetrap2,-(SP)
        rts

L2126:
        bsr     _cli
        move.l  usp,a0
        movem.l d1-d7/a0-a6,-(a0)
        move.l  a0,usp

        movea.l _rlr,A6
        movea.l 8(A6),A6
        move.w  #1,(A6)
        move.l  A0,0x42(A6)
        move.l  SP,0x46(A6)
        movea.l 0x3E(A6),SP
        bsr     _sti

        move.l  D1,-(SP)
        jsr     _super
        addq.l  #4,SP

supret:                             // return from gementry
        bsr     _cli

        movea.l _rlr,A0
        movea.l 8(A0),A0
        clr.w   (A0)
        move.l  SP,0x3E(A0)
        movea.l 0x46(A0),SP
        movea.l 0x42(A0),A0

        movem.l (a0)+,d1-d7/a0-a6
        move.l  a0,usp
        bsr     _sti
        rte



_givecpm:
        move.l  savetrap2,0x88
        rts
_takecpm:
        move.l  0x88,savetrap2
        move.l  #_aestrap,0x88
        rts

        

_far_bcha:
        move.l  SP,LED6E
        lea     LEDDE,SP
        movem.l D0-D2/A0-A2,-(SP)
        move.w  D0,-(SP)
        jsr     _b_click
        addq.l  #2,SP
        movem.l (SP)+,D0-D2/A0-A2
        movea.l LED6E,SP
        rts

_far_mcha:
        move.l  SP,LED6E
        lea     LEDDE,SP
        movem.l D0-D2/A0-A2,-(SP)

        move.w  D1,-(SP)
        move.w  D0,-(SP)
        move.l  #_mchange,-(SP)
        jsr     _forkq
        addq.l  #8,SP
        movem.l (SP)+,D0-D2/A0-A2
        movea.l LED6E,SP
        rts

/*
;
;       drawrat(newx, newy)
;
*/
_drawrat:
        move.w  4(SP),D0
        move.w  6(SP),D1
        movea.l _drwaddr,A0
        jsr     (A0)
        rts




_justretf:
        rts


_tikcod:
        move.l  a7,LED72
        lea     LEE3E,a7
        tst.l   _CMP_TICK
        beq     L2234
        addq.l  #1,_NUM_TICK
        subq.l  #1,_CMP_TICK
        bne     L2234

        move.l  _NUM_TICK,-(a7)
        move.l  #_tchange,-(a7)
        jsr     _forkq
        addq.l  #8,a7
L2234:
        move.w  #1,-(a7)
        jsr     _b_delay
        addq.l  #2,a7
        movea.l LED72,a7
        movea.l _tiksav,a0
        jsr     (a0)
        rts



.data

LE3EE:
        .dc.b   0x04,0x01,0x01,0x02,0x01,0x01,0x02,0x02
        .dc.b   0x04,0x02,0x02,0x02,0x00,0x03,0x04,0x02
        .dc.b   0x06,0x00


.bss

LEAC2:
        .ds.w    1

_crit_er:
        .ds.l    1
savetrap2:
        .ds.l    1

_drwaddr:
        .ds.l    1

_tikaddr:
        .ds.l    1
LED6E:
        .ds.l    1

LED72:
        .ds.l    1

_tiksav:
        .ds.l    1
_NUM_TICK:
        .ds.l    1
_CMP_TICK:
        .ds.b    0x60
LEDDE:
        .ds.b    0x60
LEE3E:
        .ds.l    1

