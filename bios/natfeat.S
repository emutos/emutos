/*
 * natfeat.S - EmuTOS NatFeat detection routine
 *
 * Copyright (c) 2001-2003 by the EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



#include "asmdefs.h"


// ==== References ===========================================================


        .globl  _detect_native_features
        .globl  _natfeat_cookie
        .globl  _xhdi_vec
        .extern _xhdi_handler

        .equ vec_illegal, 0x10       // illegal exception vector

// ===========================================================================
// ==== TEXT segment (TOS image) =============================================
// ===========================================================================

        .text

#if DETECT_NATIVE_FEATURES

/*
 * NatFeats test
 */
_detect_native_features:
        clr.l   d0                   // assume no NatFeats available
        move.l  sp,a1
        move.l  vec_illegal,a0
        move.l  #fail_natfeat,vec_illegal
        pea     nf_version_name
        sub.l   #4,sp
#ifdef __mcoldfire__
#error Conflict with instruction mvs.b d0,d1
#else
        dc.w    0x7300              // Jump to NATFEAT_ID
#endif
        tst.l   d0
        beq.s   fail_natfeat
        moveq   #1,d0               // NatFeats detected

fail_natfeat:
        move.l  a1,sp
        move.l  a0,vec_illegal

        rts

        SECTION_RODATA

nf_version_name:
        .ascii  "NF_VERSION\0"
        .even

        .text

// ===========================================================================
// ==== NatFeat cookie points here ===========================================
// ===========================================================================

_natfeat_cookie:
        dc.l    0x20021021              // NatFeat magic constant
        dc.l    nfID
        dc.l    nfCall

nfID:
        dc.w    0x7300  // Conflict with ColdFire instruction mvs.b d0,d1
        rts

nfCall:
        dc.w    0x7301  // Conflict with ColdFire instruction mvs.b d1,d1
        rts

#endif  /* DETECT_NATIVE_FEATURES */


#if CONF_WITH_XHDI

// ===========================================================================
// ==== XHDI cookie points here ==============================================
// ===========================================================================

        dc.l    0x27011992              // XHDI magic constant
_xhdi_vec:
#ifdef __mcoldfire__
        lea     -12(sp),sp
        movem.l d1/a0-a1,(sp)
#else
        movem.l d1/a0-a1,-(sp)
#endif
        pea     16(sp)
        jsr     _xhdi_handler
        addq.l  #4,sp
#ifdef __mcoldfire__
        movem.l (sp),d1/a0-a1
        lea     12(sp),sp
#else
        movem.l (sp)+,d1/a0-a1
#endif
        rts

#endif  /* CONF_WITH_XHDI */

        .end
