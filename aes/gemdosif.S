/*
 * gemdosif.S - Assembler part of the GEMDOS interface
 *
 * Copyright 2002-2015, The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"
#include "asmstruct.h"

        .globl  _disable_interrupts
        .globl  _enable_interrupts
        .globl  _giveerr
        .globl  _takeerr
        .globl  _retake
        .globl  _justretf
        .globl  _unset_aestrap
        .globl  _set_aestrap
        .globl  _back
        .globl  _far_mcha
        .globl  _aes_wheel
        .globl  _far_bcha
        .globl  _drawrat

        .globl  _tikaddr
        .globl  _tiksav
        .globl  _CMP_TICK
        .globl  _NUM_TICK
        .globl  _drwaddr
        .globl  savesr
        .globl  _tikcod

        .extern _eralert
        .extern _rlr
        .extern _super
        .extern _b_click
        .extern _mchange
        .extern _forkq
        .extern _wheel_change
        .extern _tchange
        .extern _b_delay

/* disable interrupts */
_disable_interrupts:
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
_enable_interrupts:
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
 * DOS error trapping code: restores aestrap & critical error vector
 *
 * called when entering graphics mode to run a graphics application
 * after a character mode application (which may have stepped on the
 * aes trap and/or the critical error vector)
 */
_retake:
#ifdef __mcoldfire__
        lea     0x88,a0
        move.l  #_aestrap,(a0)
#else
        move.l  #_aestrap,0x88
#endif
        move.l  #criterr_handler,d0
call_err:
        move.l  d0,-(sp)
        move.w  #0x0101,-(sp)       // critical error vector
        move.w  #5,-(sp)            // Setexc
        trap    #13
        addq.l  #8,sp
        rts

/*
 * restore the previous critical error handler
 *
 * called when leaving graphics mode, either to run a character-based
 * application, or during desktop shutdown
 */
_giveerr:
        move.l  save_etv_critic,d0
        bra     call_err

/*
 * install the GEM critical error handler
 *
 * called during desktop startup, just before entering graphics mode
 */
_takeerr:
        moveq   #-1,d0
        bsr     call_err            // get current error vector
        move.l  d0,save_etv_critic  //  & save it locally
        move.l  #criterr_handler,d0 // set new vector
        bra     call_err

/*
 * default critical error handler in graphics mode
 *
 * NOTE: we call form_alert() from eralert(), without going through
 * the AES trap.  form_alert() may do a context switch, during which
 * the current stack pointer is saved in UDA_SPSUPER.  Although this
 * is restored later when the context is switched back to us, it will
 * remain pointing to the wrong stack (AES routines expect that it
 * always points within the AES stack for the process).  If we are not
 * careful, the next call to an AES routine will crash the system.  So
 * we do the following:
 *  . save UDA_SPSUPER before calling eralert(), and restore it after
 *  . save the current stack pointer, and restore it after
 *  . use our own private stack during critical error processing
 */
criterr_handler:
        move.w  4(sp),d0            // d0 = error code
        move.w  6(sp),d1            // d1 = drive number
        movea.l _rlr,a0
        movea.l PD_UDA(a0),a0       // a0 -> UDA
#ifdef __mcoldfire__
        lea     UDA_SPSUPER(a0),a0
        move.l  (a0),save_spsuper
#else
        move.l  UDA_SPSUPER(a0),save_spsuper
#endif
        jsr     _disable_interrupts // stack switch must be atomic
        move.l  sp,save_spcriterr
        lea     criterr_stack,sp
        jsr     _enable_interrupts
#ifdef __mcoldfire__
        lea     -36(sp),sp
        movem.l d3-d7/a3-a6,(sp)    // save regs on temp stack
#else
        movem.l d3-d7/a3-a6,-(sp)   // save regs on temp stack
#endif
        movea.l #err_tbl,a1
        suba.l  a0,a0               // assume zeroth entry (error -1)
        move.w  d0,-(sp)            // save original error code on stack
        beq     alert_lookup        // zero, go do lookup
#ifdef __mcoldfire__
        not.l   d0                  // ones complement error code
#else
        not.w   d0                  // ones complement error code
#endif
        cmpi.w  #err_tbl_last,d0    // outside table?
        bgt     alert_lookup        // yes, assume zeroth entry
        movea.w d0,a0               // else a0 = lookup table index
alert_lookup:
        move.b  0(a0,a1.l),d0       // convert error code to alert number
        move.w  d1,-(sp)            // put drive on stack
        move.w  d0,-(sp)            // and alert number
        jsr     _eralert            // issue alert
        addq.l  #4,sp
        move.w  (sp)+,d1            // recover original error code
        ext.l   d1
        tst.w   d0                  // did user say Cancel?
        beq     no_retry            // yes, branch
        move.l  #0x00010000,d1      // else set special retry request value
no_retry:
        move.l  d1,d0               // set return code
#ifdef __mcoldfire__
        movem.l (sp),d3-d7/a3-a6    // restore regs from temp stack
        lea     36(sp),sp
#else
        movem.l (sp)+,d3-d7/a3-a6   // restore regs from temp stack
#endif
        movea.l save_spcriterr,sp   // recover old stack pointer
        movea.l _rlr,a0
        movea.l PD_UDA(a0),a0       // a0 -> UDA
#ifdef __mcoldfire__
        lea     UDA_SPSUPER(a0),a0
        move.l  save_spsuper,(a0)
#else
        move.l  save_spsuper,UDA_SPSUPER(a0)
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
#if CONF_DEBUG_AES_STACK
        lea     -16(sp),sp
        movem.l d0-d1/a0-a1,(sp)
        jsr     _trapaes_debug_enter
        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
#endif

        bsr     _disable_interrupts     // mask out interrupts
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
        bsr     _enable_interrupts      // restore interrupt mask

        move.l  d1,-(sp)                // addr of parameter
        move.w  d0,-(sp)                // 200 or 201
        jsr     _super                  // jump to supervisor entry point
        addq.l  #6,sp                   // restore stack pointer

supret:                                 // return from gementry
        bsr     _disable_interrupts     // mask out interrupts

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
        bsr     _enable_interrupts      // restore interrupt mask

#if CONF_DEBUG_AES_STACK
        lea     -16(sp),sp
        movem.l d0-d1/a0-a1,(sp)
        jsr     _trapaes_debug_exit
        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
#endif
        rte



_unset_aestrap:
#ifdef __mcoldfire__
        lea     0x88,a0
        move.l  savetrap2,(a0)
#else
        move.l  savetrap2,0x88
#endif
        rts
_set_aestrap:
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



SECTION_RODATA

/*
 * This table converts an error number to an (internal-only) alert
 * number: entry[n-1] contains the alert number to use for error -n.
 *
 * The alert number is used by eralert() in gemfmlib.c to index into
 * arrays in that module.  Thus this table must be synchronized with
 * gemfmlib.c.
 */
err_tbl:
        .dc.b   4,1,1,2,1,1,2,2     // errors -1 to -8
        .dc.b   4,2,2,2,0,3,4,2     // errors -9 to -16
        .dc.b   5                   // error -17 (EOTHER, currently not implemented)
err_tbl_end:
        .dc.b   0                   // dummy, not needed

        .equ    err_tbl_last,(err_tbl_end-err_tbl)

.bss

savesr:
        .ds.w    1

save_etv_critic:
        .ds.l    1      // save area for character-mode critical error vector

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

/*
 *  data areas used by the critical error handler
 */
save_spsuper:
        .ds.l    1              // save area for contents of UDA_SPSUPER
save_spcriterr:
        .ds.l    1              // save area for stack ptr on entry

        .ds.w   512             // private stack, has about 50 words spare
criterr_stack:          

/*
 * miscellaneous stacks
 */
        .ds.b    0x80
gstack:                         // gsx stack for mouse

        .ds.b    0x80
tstack:                         // tick stack
        .ds.l    1
