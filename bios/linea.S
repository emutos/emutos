/*
 * linea.S - linea graphics stuff
 *
 * Copyright (c) 2002-2015 by Authors:
 *
 * Authors:
 *  THH  Thomas Huth
 *  MAD  Martin Doering
 *  LVL  Laurent Vogel
 *  EET  Eero Tamminen
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_LINEA 0

#include "asmdefs.h"

// ==== References ===========================================================

        .extern line_a_vars
        .extern _sysfonts
        .extern _kprintf

        .extern _vdi_v_show_c
        .extern _vdi_v_hide_c
        .extern _vdi_vsc_form

        .extern _get_pix
        .extern _set_pix

        .extern _linea_line
        .extern _linea_rect
        .extern _linea_hline
        .extern _linea_polygon

        .extern _linea_blit

        .extern _sprite_x
        .extern _sprite_y
        .extern _sprite_def
        .extern _sprite_sav
        .extern _draw_sprite
        .extern _undraw_sprite


// ==== Definitions ==========================================================

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
         * the Line A exception. So programs can use 0xA92x instead of
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

linea_blit:
        move.l  a6,-(sp)                // blit frame structure addr
        jsr     _linea_blit
        addq.l  #4,sp
        rts

        SECTION_RODATA


/* Some VDI C-functions can be used directly as-is because:
 * - they don't use the passed VDI workstation pointer
 * - parameters for line-A go in the same Intin/Ptsin/Control arrays
 * These functions are show, hide and transform (mouse) cursor.
 *
 * A couple of functions have line-A specific C-implementations.
 * They are put_pix, get_pix.
 *
 * Several functions need a C-wrapper around utility functions shared
 * with VDI.  They are draw_sprite, undraw_sprite, linea_line,
 * linea_hline, linea_rect, linea_polygon, linea_raster, linea_fill.
 * Their arguments are in global variables/arrays.
 *
 * Bitblt has both a small C-wrapper and an ASM-wrapper, as the
 * pointer to its argument array is given in a register.
 *
 * The assembly "text_blt" routine can be called directly because
 * it uses line-A variables which VDI just sets up before
 * calling it.
 *
 * Neochrome Master and old GFA programs can use all of them,
 * and many (even new) freeware games can still use bitblt.
 *
 * It's probably only safe to run line-A-using programs in (1-4 plane)
 * ST/e modes.  Such programs can e.g. leave too little space for
 * sprite backup buffer in higher resolutions.
 */
        .even   // Mandatory in ELF section .rodata
linea_vecs:
        .dc.l   _linea_0
        .dc.l   _put_pix        /* $1 - put pixel */
        .dc.l   _get_pix        /* $2 - get pixel */
        .dc.l   _linea_line     /* $3 - draw line */
        .dc.l   _linea_hline    /* $4 - horizontal line */
        .dc.l   _linea_rect     /* $5 - filled rectangle */
        .dc.l   _linea_polygon  /* $6 - draw 1 line of polygon */
        .dc.l   linea_blit      /* $7 - area blit */
        .dc.l   _text_blt       /* $8 - text blitting */
        .dc.l   _vdi_v_show_c   /* $9 - show mouse */
        .dc.l   _vdi_v_hide_c   /* $A - hide cursor */
        .dc.l   _vdi_vsc_form   /* $B - transform mouse */
        .dc.l   _undraw_sprite  /* $C - undraw sprite */
        .dc.l   _draw_sprite    /* $D - draw sprite */
        .dc.l   _linea_raster   /* $E - copy raster form */
        .dc.l   _linea_fill     /* $F - flood fill */
linea_ents:
        /* Number of implemented Line A routines */
        .equ    nb_linea,(linea_ents-linea_vecs)/4

// ===========================================================================
// ==== End ==================================================================
// ===========================================================================
