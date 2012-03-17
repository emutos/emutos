/*
 * gemdosif.S - Assembler part of the GEMDOS interface
 *
 * Copyright 2002-2012, The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"
#include "asmstruct.h"

.globl _cli
.globl _sti
.globl _giveerr
.globl _takeerr
.globl _retake
.globl _setdsss
.globl _justretf
.globl _givecpm
.globl _takecpm
.globl _back
.globl _far_mcha
.globl _aes_wheel
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
#ifdef __mcoldfire__
        move.l  d0,-(sp)
        move.w  sr,d0
        move.w  d0,savesr
        ori.l   #0x0700,d0
        move.w  d0,sr
        move.l  (sp)+,d0
#else
        move    sr,savesr
        ori     #0x0700,sr
#endif
        rts

/* restore interrupt mask as it was before cli() */
_sti:
#ifdef __mcoldfire__
        move.l  d0,-(sp)
        move.w  savesr,d0
        move.w  d0,sr
        move.l  (sp)+,d0
#else
        move    savesr,sr
#endif
        rts


/*
 DOS error trapping code.
*/
_retake:
#ifdef __mcoldfire__
        lea     0x88,a0
        move.l  #_aestrap,(a0)
#else
        move.l  #_aestrap,0x88
#endif
        move.l  #_err_tra,d0
call_err:
        move.l  d0,-(sp)
        move.w  #0x0101,-(sp)
        move.w  #0x05,-(sp)
        trap    #13
        addq.l  #8,sp
        rts
        
_giveerr:
        move.l  _crit_er,d0
        bra     call_err
        
_takeerr:
        moveq   #-1,d0
        bsr     call_err
        move.l  d0,_crit_er
        move.l  #_err_tra,d0
        bra     call_err
        
_err_tra:
        move.w  0x04(sp),d0
        move.w  0x06(sp),d1
#ifdef __mcoldfire__
        lea     -36(sp),sp
        movem.l d3-d7/a3-a6,(sp)
#else
        movem.l d3-d7/a3-a6,-(sp)
#endif
        movea.l #err_tbl,a1
        move.w  d0,-(sp)
        bne     L1F62
        movea.w #0x00,a0
        bra     L1F70
        
L1F62:
#ifdef __mcoldfire__
        not.l   d0
#else
        not.w   d0
#endif
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
#ifdef __mcoldfire__
        movem.l (sp),d3-d7/a3-a6
        lea     36(sp),sp
#else
        movem.l (sp)+,d3-d7/a3-a6
#endif
        rts




// **** Trap entry: ****
_aestrap:
        tst.w   d0
        beq.s   _back
        cmpi.w  #0xC8,d0
        beq.s   trapaes
        cmpi.w  #0xC9,d0
        beq.s   trapaes
        move.l  savetrap2,-(sp)
        rts

_back:
        clr.w   -(sp)
        move.w  #0x4C,-(sp)             // Pterm
        trap    #1


trapaes:
        bsr     _cli                    // mask out interrupts
        move.l  usp,a0                  
#ifdef __mcoldfire__
        lea     -56(a0),a0
        movem.l d1-d7/a0-a6,(a0)        // put registers to user stack
#else
        movem.l d1-d7/a0-a6,-(a0)       // put registers to user stack
#endif
        move.l  a0,usp                  

        movea.l _rlr,a6
        movea.l PD_UDA(a6),a6
        move.w  #1,UDA_INSUPER(a6)
        move.l  a0,UDA_SPUSER(a6)
        move.l  sp,UDA_OLDSPSUPER(a6)
        movea.l UDA_SPSUPER(a6),sp
        bsr     _sti                    // restore interrupt mask

        move.l  d1,-(sp)                // addr of parameter
        move.w  d0,-(sp)                // 200 or 201
        jsr     _super                  // jump to supervisor entry point
        addq.l  #6,sp                   // restore stack pointer

supret:                                 // return from gementry
        bsr     _cli                    // mask out interrupts

        movea.l _rlr,a0
        movea.l PD_UDA(a0),a0
        clr.w   UDA_INSUPER(a0)
        move.l  sp,UDA_SPSUPER(a0)
        movea.l UDA_OLDSPSUPER(a0),sp
        movea.l UDA_SPUSER(a0),a0

#ifdef __mcoldfire__
        movem.l (a0),d1-d7/a0-a6
        lea     56(a0),a0
#else
        movem.l (a0)+,d1-d7/a0-a6
#endif
        move.l  a0,usp                  
        bsr     _sti                    // restore interrupt mask
        rte



_givecpm:
#ifdef __mcoldfire__
        lea     0x88,a0
        move.l  savetrap2,(a0)
#else
        move.l  savetrap2,0x88
#endif
        rts
_takecpm:
#ifdef __mcoldfire__
        lea     0x88,a0
        move.l  (a0),savetrap2
        move.l  #_aestrap,(a0)
#else
        move.l  0x88,savetrap2
        move.l  #_aestrap,0x88
#endif
        rts

        

_far_bcha:
        move.l  sp,gstksave
        lea     gstack,sp
#ifdef __mcoldfire__
        lea     -24(sp),sp
        movem.l d0-d2/a0-a2,(sp)
#else
        movem.l d0-d2/a0-a2,-(sp)
#endif
        move.w  d0,-(sp)
        jsr     _b_click
        addq.l  #2,sp
#ifdef __mcoldfire__
        movem.l (sp),d0-d2/a0-a2
        lea     24(sp),sp
#else
        movem.l (sp)+,d0-d2/a0-a2
#endif
        movea.l gstksave,sp
        rts

_far_mcha:
        move.l  sp,gstksave
        lea     gstack,sp
#ifdef __mcoldfire__
        lea     -24(sp),sp
        movem.l d0-d2/a0-a2,(sp)
#else
        movem.l d0-d2/a0-a2,-(sp)
#endif

        move.w  d1,-(sp)
        move.w  d0,-(sp)
        move.l  #_mchange,-(sp)
        jsr     _forkq
        addq.l  #8,sp
#ifdef __mcoldfire__
        movem.l (sp),d0-d2/a0-a2
        lea     24(sp),sp
#else
        movem.l (sp)+,d0-d2/a0-a2
#endif
        movea.l gstksave,sp
        rts

/* AES mouse wheel handler called by the VDI */
_aes_wheel:
        move.l  sp,gstksave
        lea     gstack,sp
#ifdef __mcoldfire__
        lea     -24(sp),sp
        movem.l d0-d2/a0-a2,(sp)
#else
        movem.l d0-d2/a0-a2,-(sp)
#endif

        move.w  d1,-(sp)
        move.w  d0,-(sp)
        move.l  #_wheel_change,-(sp)
        jsr     _forkq
        addq.l  #8,sp
#ifdef __mcoldfire__
        movem.l (sp),d0-d2/a0-a2
        lea     24(sp),sp
#else
        movem.l (sp)+,d0-d2/a0-a2
#endif
        movea.l gstksave,sp
        rts


/*
;
;       drawrat(newx, newy)
;
*/
_drawrat:
        move.w  4(sp),d0
        move.w  6(sp),d1
        move.l  _drwaddr,-(sp)
        rts                     // Jump to vector stored in _drwaddr


_justretf:
        rts


_tikcod:
        move.l  sp,tstksave
        lea     tstack,sp
        tst.l   _CMP_TICK
        beq     L2234
        addq.l  #1,_NUM_TICK
        subq.l  #1,_CMP_TICK
        bne     L2234

        move.l  _NUM_TICK,-(sp)
        move.l  #_tchange,-(sp)
        jsr     _forkq
        addq.l  #8,sp
L2234:
        move.w  #1,-(sp)
        jsr     _b_delay
        addq.l  #2,sp
        movea.l tstksave,sp
        move.l  _tiksav,-(sp)
        rts                     // Jump to vector stored in _tiksav



.text // .rodata

err_tbl:
        .dc.b   4,1,1,2,1,1,2,2
        .dc.b   4,2,2,2,0,3,4,2
        .dc.b   6,0


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
gstksave:
        .ds.l    1

tstksave:
        .ds.l    1

_tiksav:
        .ds.l    1
_NUM_TICK:
        .ds.l    1
_CMP_TICK:
        .ds.l    1

        .ds.b    0x80
gstack:                         // gsx stack for mouse

        .ds.b    0x80
tstack:                         // tick stack
        .ds.l    1

