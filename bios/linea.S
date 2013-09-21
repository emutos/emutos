/*
 * linea.S - linea graphics stuff
 *
 * Copyright (c) 2002-2013 by Authors:
 *
 * Authors:
 *  THH  Thomas Huth
 *  MAD  Martin Doering
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#define DBG_LINEA 0

#include "asmdefs.h"

// ==== External Declarations ================================================

        .extern line_a_vars
        .extern _sysfonts
        .extern _kprintf
        .extern __v_show_c
        .extern __v_hide_c

        .extern _sprite_x
        .extern _sprite_y
        .extern _sprite_def
        .extern _sprite_sav


// ==== Global Declarations ================================================

        .globl  _int_linea              // entry for linea exception


// ==== Line-A handler ===============================
_int_linea:
       /* save regs as they are needed if this is sprite un/draw */
       move.w d0,_sprite_x
       move.w d1,_sprite_y
       move.l a0,_sprite_def
       move.l a2,_sprite_sav
#ifdef __mcoldfire__
        move.l  4(sp),a0        /* Get the opcode address */
#else
        move.l  2(sp),a0        /* Get the opcode address */
#endif
        move.l  a0,a1
        moveq   #0,d0
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
        cmp.w   #nb_linea,d0
        bhs.s   wrong_linea

#if DBG_LINEA
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
#endif /* DBG_LINEA */

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
#if DBG_LINEA
        move.w  d0,-(sp)
        subq.l  #2,a0
        move.l  a0,-(sp)
        pea     wrong_linea_msg
        jsr     _kprintf
        lea     10(sp),sp
#endif /* DBG_LINEA */
        rte

        SECTION_RODATA

#if DBG_LINEA

wrong_linea_msg:
        .ascii  "pc=0x%08lx: Line-A call number 0x%03x out of bounds\n\0"
        .even

linea_msg:
        .ascii  "Line-A #%x\n\0"
        .even

#endif  /* DBG_LINEA */

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
#if HAS_KPRINTF
        moveq   #1,d0
        bra.s   linea_stub
#endif

_linea_2:
#if HAS_KPRINTF
        moveq   #2,d0
        bra.s   linea_stub
#endif

_linea_3:
#if HAS_KPRINTF
        moveq   #3,d0
        bra.s   linea_stub
#endif

_linea_4:
#if HAS_KPRINTF
        moveq   #4,d0
        bra.s   linea_stub
#endif

_linea_5:
#if HAS_KPRINTF
        moveq   #5,d0
        bra.s   linea_stub
#endif

_linea_6:
#if HAS_KPRINTF
        moveq   #6,d0
        bra.s   linea_stub
#endif

_linea_7:
#if HAS_KPRINTF
        moveq   #7,d0
        bra.s   linea_stub
#endif

_linea_8:
#if HAS_KPRINTF
        moveq   #8,d0
        bra.s   linea_stub
#endif

_linea_b:
#if HAS_KPRINTF
        moveq   #0xb,d0
        bra.s   linea_stub
#endif

_linea_c:
#if HAS_KPRINTF
        moveq   #0xc,d0
        bra.s   linea_stub
#endif

_linea_d:
#if HAS_KPRINTF
        moveq   #0xd,d0
        bra.s   linea_stub
#endif

_linea_e:
#if HAS_KPRINTF
        moveq   #0xe,d0
        bra.s   linea_stub
#endif

_linea_f:
#if HAS_KPRINTF
        moveq   #0xf,d0
#endif
        // Fall into linea_stub

/* We may arrive here either from our own Line A exception dispatcher,
 * or from direct function call.
 */
linea_stub:
#if HAS_KPRINTF
        move.l  (sp),d1
        cmp.l   #linea_dispatch_pc,d1   // Our own exception dispatcher?
        bne.s   1f                      // No
        move.l  a1,a0                   // Get the exception opcode address
        bra.s   2f
1:      move.l  d1,a0                   // Get the caller return address
2:      move.w  d0,-(sp)                // Opcode
        move.l  a0,-(sp)                // Opcode/Return address
        pea     linea_stub_msg
        jsr     _kprintf
        lea     10(sp),sp
#endif
        rts

        SECTION_RODATA

#if HAS_KPRINTF
linea_stub_msg:
        .ascii  "pc=0x%08lx: unimplemented Line-A call number 0x%03x\n\0"
        .even
#endif



/* Some of these routines are now different, and no longer usable from
 * assembler.  Only functions used by games are enabled.
 *
 * Some VDI C-functions can still be used directly as:
 * - they don't use the passed VDI workstation pointer
 * - parameters for line-A go in same Intin/Ptsin/Control arrays
 * These functions are show, hide and transform (mouse) cursor.
 *
 * Couple of functions have line-A specific C-implementations,
 * such are put_pix, get_pix, draw_sprite, undraw_sprite.
 * Their arguments are in global variables/arrays.
 *
 * Some functions can call directly the (line-A derived) ASM
 * iplementation shared by VDI, which use line-A variables
 * (VDI function sets line-A variables before calling asm).
 * "text_blt" is such function.
 *
 * Only interesting functions are:
 * - show/hide cursor
 * - draw/undraw sprite
 * - put/get pixel
 * - line/hline draw
 * - filled rectangle
 * - bitblit
 * First are used by GFA, others by games, rest can be left unimplemented.
 *
 * It's safe to run line-A using programs only in (1-4 plane)
 * ST/e modes.  Such programs can e.g. leave too little space
 * for sprite backup buffer in higher resolutions).
 */
linea_vecs:
        .dc.l   _linea_0
        .dc.l   _put_pix        /* $1 - put pixel */
        .dc.l   _get_pix        /* $2 - get_pixel */
        .dc.l   _linea_3        /* _abline  */
        .dc.l   _linea_4        /* _habline */
        .dc.l   _linea_5        /* _rectfill */
        .dc.l   _linea_6        /* _clc_flit */
        .dc.l   _linea_7        /* _bit_blt */
        .dc.l   _linea_8        /* _text_blt */
        .dc.l   __v_show_c      /* $9 - show mouse */
        .dc.l   __v_hide_c      /* $A - hide cursor */
        .dc.l   _xfm_crfm       /* $B - transform mouse */
        .dc.l   _undraw_sprite  /* $C - undraw sprite */
        .dc.l   _draw_sprite    /* $D - draw sprite */
        .dc.l   _linea_e        /* _vr_cpyfm? */
        .dc.l   _linea_f        /* _d_contourfill?? */
linea_ents:
        /* Number of implemented Line A routines */
        .equ    nb_linea,(linea_ents-linea_vecs)/4

// ===========================================================================
// ==== End ==================================================================
// ===========================================================================
