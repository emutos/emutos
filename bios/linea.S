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


// ==== External Declarations ================================================

        .xdef _put_pix
        .xdef _get_pix
        .xdef _abline
        .xdef _habline
        .xdef _clc_flit           // from vdi/gsxasm1.S
        .xdef _RECTFILL
//        .xdef bit_blt
        .xdef _text_blt
        .xdef _dis_cur
        .xdef _hide_cur
        .xdef _xfm_crfm
//        .xdef _COPY_RFM


// ==== Global Declarations ================================================

        .global _int_linea              // entry for linea exception


// ==== Line-A handler ===============================
_int_linea:
        move.l  2(sp),a0
        move.l  a0,a1
        clr.l   d0
        move.w  (a0)+,d0
        move.l  a0,2(sp)
        and.w   #0xFFF,d0
        tst.w   d0
        bmi     wrong_linea
        cmp.w   linea_ents,d0
        bpl     wrong_linea

#if 0
        movem.l d0-d2/a0-a2,-(sp)
        move.w  d0,-(sp)
        pea     linea_msg
        jsr     _kprintf              /* Print debugging information */
        addq.l  #6,sp
        movem.l (sp)+,d0-d2/a0-a2
#endif

        lea     linea_vecs,a0
        lsl.w   #2,d0
        move.l  0(a0,d0),a0
        jsr     (a0)
linea_dispatch_pc:      
        rte

wrong_linea:
        move.w  d0,-(sp)
        sub.w   #2,a0
        move.l  a0,-(sp)
        pea     wrong_linea_msg
        jsr     _kprintf
        add.w   #10,sp
        rte

wrong_linea_msg:
        .ascii  "pc=0x%08lx: Line-A call number 0x%03x out of bounds\n\0"
        .even

linea_msg:
        .ascii  "Line-A #%x\n\0"
        .even


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
        bra     linea_stub


_linea_2:
        move.w  #2,d0
        bra     linea_stub


_linea_3:
        move.w  #3,d0
        bra     linea_stub


_linea_4:
        move.w  #4,d0
        bra     linea_stub


_linea_5:
        move.w  #5,d0
        bra     linea_stub


_linea_6:
        move.w  #6,d0
        bra     linea_stub


_linea_7:
        move.w  #7,d0
        bra     linea_stub


_linea_8:
        move.w  #8,d0
        bra     linea_stub


_linea_9:
        move.w  #9,d0
        bra     linea_stub


_linea_a:
        move.w  #0xa,d0
        bra     linea_stub


_linea_b:
        move.w  #0xb,d0
        bra     linea_stub


_linea_c:
        move.w  #0xc,d0
        bra     linea_stub


_linea_d:
        move.w  #0xd,d0
        bra     linea_stub


_linea_e:
        move.w  #0xe,d0
        bra     linea_stub


_linea_f:
        move.w  #0xf,d0
        bra     linea_stub



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
        add.w   #10,sp
        rts
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
        dc.l    _linea_9        /* _dis_cur */
        dc.l    _linea_a        /* _hide_cur */
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
