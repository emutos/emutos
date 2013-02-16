/*
 * linea.S - linea graphics stuff
 *
 * Copyright (c) 2002 by Authors:
 *
 *  THH  Thomas Huth
 *  MAD  Martin Doering
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"

// ==== External Declarations ================================================

        .extern _put_pix
        .extern _get_pix
        .extern _abline
        .extern _habline
        .extern _clc_flit         // from vdi/gsxasm1.S
        .extern _RECTFILL
        .extern _text_blt
        .extern _dis_cur
        .extern _hide_cur
        .extern _xfm_crfm


// ==== Global Declarations ================================================

        .globl  _int_linea              // entry for linea exception


// ==== Line-A handler ===============================
_int_linea:
#ifdef __mcoldfire__
        move.l  4(sp),a0        /* Get the opcode address */
#else
        move.l  2(sp),a0        /* Get the opcode address */
#endif
        move.l  a0,a1
        clr.l   d0
        move.w  (a0)+,d0        /* Get the opcode value */
#ifdef __mcoldfire__
        move.l  a0,4(sp)        /* Return after the Line A opcode */
#else
        move.l  a0,2(sp)        /* Return after the Line A opcode */
#endif
#ifdef __mcoldfire__
        /* On ColdFire, all the standard Line A opcodes conflict with
         * valid MAC instructions. Therefore they can't be used.
         * Fortunately, the 0xA92x opcodes are still illegal and trigger
         * the Line A exception. The programs can use 0xA92x instead of
         * 0xA00x to call the Line A. Thus we must keep only the last
         * digit as function number. */
        and.l   #0x00F,d0       /* Keep only the function number */
#else
        and.w   #0xFFF,d0       /* Keep only the function number */
#endif
        tst.w   d0
        bmi     wrong_linea
        cmp.w   linea_ents,d0
        bpl     wrong_linea

#if 0
#ifdef __mcoldfire__
        lea     -16(sp),sp
        movem.l d0-d1/a0-a1,(sp)
#else
        movem.l d0-d1/a0-a1,-(sp)
#endif
        move.w  d0,-(sp)
        pea     linea_msg
        jsr     _kprintf              /* Print debugging information */
        addq.l  #6,sp
#ifdef __mcoldfire__
        movem.l (sp),d0-d1/a0-a1
        lea     16(sp),sp
#else
        movem.l (sp)+,d0-d1/a0-a1
#endif
#endif

        lea     linea_vecs,a0
#ifdef __mcoldfire__
        lsl.l   #2,d0
#else
        lsl.w   #2,d0
#endif
        move.l  0(a0,d0),a0
        jsr     (a0)
linea_dispatch_pc:
        rte

wrong_linea:
        move.w  d0,-(sp)
        subq.l  #2,a0
        move.l  a0,-(sp)
        pea     wrong_linea_msg
        jsr     _kprintf
        lea     10(sp),sp
        rte

        SECTION_RODATA

wrong_linea_msg:
        .ascii  "pc=0x%08lx: Line-A call number 0x%03x out of bounds\n\0"
        .even

linea_msg:
        .ascii  "Line-A #%x\n\0"
        .even

        .text

_linea_0:
        lea     line_a_vars,a0  // get base address for line a variables
        move.l  a0,d0

        lea     _sysfonts,a1   // get pointer to the three system font headers

        lea     linea_vecs,a2   // get pointer to table of the Line-A routines
        rts

//
// These are stubs for linea :
// the stub will print the pc of the caller, whether the function
// was called using the line a opcode, or directly via its address.
//


_linea_1:
        move.w  #1,d0
        bra.s   linea_stub


_linea_2:
        move.w  #2,d0
        bra.s   linea_stub


_linea_3:
        move.w  #3,d0
        bra.s   linea_stub


_linea_4:
        move.w  #4,d0
        bra.s   linea_stub


_linea_5:
        move.w  #5,d0
        bra.s   linea_stub


_linea_6:
        move.w  #6,d0
        bra.s   linea_stub


_linea_7:
        move.w  #7,d0
        bra.s   linea_stub


_linea_8:
        move.w  #8,d0
        bra.s   linea_stub

/*
_linea_9:
        move.w  #9,d0
        bra.s   linea_stub
*/
/*
_linea_a:
        move.w  #0xa,d0
        bra.s   linea_stub
*/

_linea_b:
        move.w  #0xb,d0
        bra.s   linea_stub


_linea_c:
        move.w  #0xc,d0
        bra.s   linea_stub


_linea_d:
        move.w  #0xd,d0
        bra.s   linea_stub


_linea_e:
        move.w  #0xe,d0
        bra.s   linea_stub


_linea_f:
        move.w  #0xf,d0
        // Fall into linea_stub



linea_stub:
        move.l  (sp),d1
        sub.l   #linea_dispatch_pc,d1
        and.l   #0xFFFFFF,d1
        bne     1f
        move.l  a1,a0
        bra     2f
1:      move.l  (sp),a0
2:      move.w  d0,-(sp)
        move.l  a0,-(sp)
        pea     linea_stub_msg
        jsr     _kprintf
        lea     10(sp),sp
        rts

        SECTION_RODATA

linea_stub_msg:
        .ascii  "pc=0x%08lx: unimplemented Line-A call number 0x%03x\n\0"
        .even



/* These routines are now different, and no longer usable from assembler */
linea_vecs:
        dc.l    _linea_0
        dc.l    _linea_1        /* _put_pix */
        dc.l    _linea_2        /* _get_pix */
        dc.l    _linea_3        /* _abline  */
        dc.l    _linea_4        /* _habline */
        dc.l    _linea_5        /* _rectfill */
        dc.l    _linea_6        /* _clc_flit */
        dc.l    _linea_7        /* _bit_blt */
        dc.l    _linea_8        /* _text_blt */
        dc.l    __v_show_c       /* $9 - _dis_cur */
        dc.l    __v_hide_c       /* $A - _hide_cur */
        dc.l    _linea_b        /* _xfm_crfm */
        dc.l    _linea_c        /* cur_replace??? */
        dc.l    _linea_d        /* cur_display??? */
        dc.l    _linea_e        /* _COPY_RFM */
        dc.l    _linea_f        /* _d_contourfill?? */
linea_ents:
        dc.w    (linea_ents-linea_vecs)/4



// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end
