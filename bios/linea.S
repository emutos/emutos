/*
 * linea.S - linea graphics stuff
 *
 * Copyright (C) 2002-2020 by Authors:
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

/* #define ENABLE_KDEBUG */

#include "asmdefs.h"

// ==== References ===========================================================

        .extern line_a_vars
        .extern _sysfonts
        .extern _kprintf

        .extern _linea_show_mouse
        .extern _linea_hide_mouse
        .extern _linea_transform_mouse

        .extern _get_pix
        .extern _put_pix

        .extern _linea_line
        .extern _linea_rect
        .extern _linea_hline
        .extern _linea_polygon

        .extern _linea_blit
        .extern _text_blt

        .extern _cur_replace
        .extern _cur_display
        .extern _linea_raster
        .extern _linea_fill

// ==== Definitions ==========================================================

        .globl  _int_linea              // entry for linea exception

// ==== Line-A handler ===============================
_int_linea:
        /* save d0/d1/a0/a2 as they are needed for sprite undraw/draw */
#ifdef __mcoldfire__
        lea     linea_save,a1
        movem.l d0/d1/a0/a2,(a1)
#else
        movem.l d0/d1/a0/a2,linea_save
#endif
#ifdef __mcoldfire__
        move.l  4(sp),a0        /* Get the opcode address */
#else
        move.l  2(sp),a0        /* Get the opcode address */
#endif
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
        jhs     wrong_linea

#ifdef ENABLE_KDEBUG
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
#endif /* ENABLE_KDEBUG */

        lea     linea_vecs,a1
#ifdef __mcoldfire__
        lsl.l   #2,d0
#else
        lsl.w   #2,d0
#endif
        move.l  0(a1,d0),a1
        /* restore d0,d1,a0,a2 in case we're calling sprite undraw/draw */
#ifdef __mcoldfire__
        move.l  a1,-(sp)
        lea     linea_save,a1
        movem.l (a1),d0/d1/a0/a2
        move.l  (sp)+,a1
#else
        movem.l linea_save,d0/d1/a0/a2
#endif
        jsr     (a1)
linea_dispatch_pc:
        rte

wrong_linea:
#ifdef ENABLE_KDEBUG
        move.w  d0,-(sp)
        subq.l  #2,a0
        move.l  a0,-(sp)
        pea     wrong_linea_msg
        jsr     _kprintf
        lea     10(sp),sp
#endif /* ENABLE_KDEBUG */
        rte

        .bss
        .even
linea_save:
        .ds.l   4       /* d0, d1, a0, a2 */

        SECTION_RODATA

#ifdef ENABLE_KDEBUG

wrong_linea_msg:
        .ascii  "pc=0x%08lx: Line-A call number 0x%03x out of bounds\n\0"
        .even

linea_msg:
        .ascii  "Line-A #%x\n\0"
        .even

#endif  /* ENABLE_KDEBUG */

        .text

_linea_0:
        lea     line_a_vars,a0  // get base address for line a variables
        move.l  a0,d0

        lea     _sysfonts,a1   // get pointer to the three system font headers

        lea     linea_vecs,a2   // get pointer to table of the Line-A routines
        rts

undraw_sprite:
        move.l  a2,-(sp)        /* save area ptr */
        jsr     _cur_replace
        addq.l  #4,sp
        rts

draw_sprite:
        move.w  d1,-(sp)        /* y coord of hotspot */
        move.w  d0,-(sp)        /* x coord */
        move.l  a2,-(sp)        /* save area ptr */
        move.l  a0,-(sp)        /* new sprite */
        jsr     _cur_display
        lea     12(sp),sp
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
 * with VDI.  They are linea_line, linea_hline, linea_rect, linea_polygon,
 * linea_raster, linea_fill.  Their arguments are in global variables/arrays.
 *
 * Bitblt (linea_blit) has both a small C-wrapper and an ASM-wrapper, as
 * the pointer to its argument array is given in a register.
 *
 * The assembly "text_blt" routine can be called directly because
 * it uses line-A variables which VDI just sets up before
 * calling it.
 *
 * The undraw_sprite and draw_sprite routines are passed their arguments
 * in registers.  Because lineA routines may be called directly (not via
 * the lineA interface), we must preserve those registers until the actual
 * routine is entered.  These routines are ASM wrappers for VDI utility
 * functions.
 *
 * Neochrome Master and old GFA programs use all of the lineA routines,
 * and many (even new) freeware games still use bitblt.
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
        .dc.l   _linea_show_mouse      /* $9 - show mouse */
        .dc.l   _linea_hide_mouse      /* $A - hide cursor */
        .dc.l   _linea_transform_mouse /* $B - transform mouse */
        .dc.l   undraw_sprite   /* $C - undraw sprite */
        .dc.l   draw_sprite     /* $D - draw sprite */
        .dc.l   _linea_raster   /* $E - copy raster form */
        .dc.l   _linea_fill     /* $F - flood fill */
linea_ents:
        /* Number of implemented Line A routines */
        .equ    nb_linea,(linea_ents-linea_vecs)/4
