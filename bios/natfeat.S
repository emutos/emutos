/*
 * natfeat.S - EmuTOS NatFeat detection routine
 *
 * Copyright (c) 2001-2003 by the EmuTOS development team
 *
 * Authors:
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



#include "asmdefs.h"
#include "header.h"
#include "config.h"


// ==== References ===========================================================

        
        .global _detect_native_features
        .global _natfeat_cookie
        .global _xhdi_vec
        .xdef   _xhdi_handler

// ===========================================================================
// ==== TEXT segment (TOS image) =============================================
// ===========================================================================

        .text

_detect_native_features:


/* 
 * NatFeats test
 */
        clr.l   d0                   // assume no NatFeats available

#if DETECT_NATIVE_FEATURES
        .equ vec_illegal, 0x10       // illegal exception vector

        move.l  sp,a1
        move.l  vec_illegal,a0
        move.l  #fail_natfeat,vec_illegal
        pea     nf_version_name
        sub.l   #4,sp
        dc.w    0x7300              // Jump to NATFEAT_ID
        tst.l   d0
        beq.s   fail_natfeat
        moveq   #1,d0               // NatFeats detected

fail_natfeat:
        move.l  a1,sp
        move.l  a0,vec_illegal
#endif  /* DETECT_NATIVE_FEATURES */

        rts

nf_version_name:
        .ascii  "NF_VERSION\0"
        .even

// ===========================================================================
// ==== NatFeat cookie points here ===========================================
// ===========================================================================

_natfeat_cookie:
        dc.l    0x20021021              // NatFeat magic constant
        dc.l    nfID
        dc.l    nfCall

nfID:
        dc.w    0x7300
        rts

nfCall:
        dc.w    0x7301
        rts

// ===========================================================================
// ==== XHDI cookie points here ==============================================
// ===========================================================================

        dc.l    0x27011992              // XHDI magic constant
_xhdi_vec:
        jmp     _xhdi_handler

        .end

