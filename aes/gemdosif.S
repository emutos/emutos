/*
 * gemdosif.S - Assembler part of the GEMDOS interface
 *
 * Copyright 2002, The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
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
	move    sr,savesr
        ori     #0x0700,sr
        rts

/* restore interrupt mask as it was before cli() */
_sti:
        move    savesr,sr
        rts

_hcli:
        ori     #0x0700,sr
        rts

_hsti:
        andi    #0xF8FF,sr
        rts


/*
 DOS error trapping code.
*/
_retake:
L1F0C:
        move.l  #_aestrap,0x88
        move.l  #_err_tra,d0
L1F1C:
        move.l  d0,-(sp)
        move.w  #0x0101,-(sp)
        move.w  #0x05,-(sp)
        trap    #13
        addq.w  #8,sp
        rts
        
_giveerr:
L1F2C:
        move.l  _crit_er,d0
        bra     L1F1C
        
_takeerr:
L1F34:
        moveq   #-1,d0
        bsr     L1F1C
        move.l  d0,_crit_er
        move.l  #_err_tra,d0
        bra     L1F1C
        
_err_tra:
L1F46:
        move.w  0x04(sp),d0
        move.w  0x06(sp),d1
        movem.l d3-d7/a3-a6,-(sp)
        movea.l #LE3EE,a1
        move.w  d0,-(sp)
        bne     L1F62
        movea.w #0x00,a0
        bra     L1F70
        
L1F62:
        not.w   d0
        movea.w d0,a0
        cmp.w   #0x11,d0
        ble     L1F70
        movea.w #0x00,a0
L1F70:
        move.b  0x00(a0,a1.l),d0
        move.w  d1,-(sp)
        move.w  d0,-(sp)
        jsr     _eralert
        addq.l  #4,sp
        move.w  (sp)+,d1
        ext.l   d1
        cmp.w   #0x00,d0
        beq     L1F90
        move.l  #0x010000,d1
L1F90:
        move.l  d1,d0
        movem.l (sp)+,d3-d7/a3-a6
        rts




// **** Trap entry: ****
_aestrap:
        cmpi.w  #0x00,d0
        beq     _back
        cmpi.w  #0xC8,d0
        beq     L2126
        cmpi.w  #0xC9,d0
        beq     L2126
        move.l  savetrap2,-(sp)
        rts

_back:
        clr.w   -(sp)
        move.w  #0x4C,-(sp)   		// Pterm
        trap    #1


L2126:
        bsr     _cli                    // mask out interrupts
        move.l  usp,a0                  
        movem.l d1-d7/a0-a6,-(a0)       // put registers to user stack
        move.l  a0,usp          	

        movea.l _rlr,a6
        movea.l 8(a6),a6
        move.w  #1,(a6)
        move.l  a0,0x42(a6)
        move.l  sp,0x46(a6)
        movea.l 0x3E(a6),sp
        bsr     _sti                    // restore interrupt mask

        move.l  d1,-(sp)                // addr of parameter
        jsr     _super                  // jump to supervisor mode
        addq.l  #4,sp                   // restore stack pointer

supret:                                 // return from gementry
        bsr     _cli                    // mask out interrupts

        movea.l _rlr,a0
        movea.l 8(a0),a0
        clr.w   (a0)
        move.l  sp,0x3E(a0)
        movea.l 0x46(a0),sp
        movea.l 0x42(a0),a0

        movem.l (a0)+,d1-d7/a0-a6
        move.l  a0,usp                  
        bsr     _sti                    // restore interrupt mask
        rte



_givecpm:
        move.l  savetrap2,0x88
        rts
_takecpm:
        move.l  0x88,savetrap2
        move.l  #_aestrap,0x88
        rts

        

_far_bcha:
        move.l  sp,LED6E
        lea     LEDDE,sp
        movem.l d0-d2/a0-a2,-(sp)
        move.w  d0,-(sp)
        jsr     _b_click
        addq.l  #2,sp
        movem.l (sp)+,d0-d2/a0-a2
        movea.l LED6E,sp
        rts

_far_mcha:
        move.l  sp,LED6E
        lea     LEDDE,sp
        movem.l d0-d2/a0-a2,-(sp)

        move.w  d1,-(sp)
        move.w  d0,-(sp)
        move.l  #_mchange,-(sp)
        jsr     _forkq
        addq.l  #8,sp
        movem.l (sp)+,d0-d2/a0-a2
        movea.l LED6E,sp
        rts

/*
;
;       drawrat(newx, newy)
;
*/
_drawrat:
        move.w  4(sp),d0
        move.w  6(sp),d1
        movea.l _drwaddr,a0
        jsr     (a0)
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

savesr:
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
        .ds.b    0x80
LEDDE:
        .ds.b    0x80
LEE3E:
        .ds.l    1

