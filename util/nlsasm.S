/*
 * nlsasm.S - fast routines for the nls hash tables
 *
 * Copyright (c) 2001 Laurent Vogel
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "i18nconf.h"
#include "config.h"

#if CONF_NO_NLS

        .global _gettext

_gettext:
        move.l  4(sp),d0
        rts

#else /* ! CONF_NO_NLS */
        
// set this to one to first hash addresses in RAM before
// computing actual string hashes. (experimental)
 
#define USE_RAM_HASH 1
 
        .global _gettext
        .global _gettext_init
                
        .xdef   _nls_hash       // current hash
        .xdef   _memset
        
        .text

_gettext_init:

#if USE_RAM_HASH
// clear the RAM hash 
        move.l  #(1024+10)*8,-(sp)
        clr.w   -(sp)
        pea     ram_hash
        jsr     _memset
        add.w   #10,sp
#endif  /* USE_RAM_HASH */
        rts
                
_gettext:
        tst.l   _nls_hash
        bne     empty
        move.l  4(sp),d0
        rts
empty:
/* check for empty string - often used in RSC - must return original address */
        move.l  4(sp),a0
        tst.b   (a0)
        bne     next
        move.l  a0,d0
        rts
next:
#if USE_RAM_HASH
        move.l  4(sp),d0
        move.l  d0,a1
        lsl.l   #3,d0
        move.l  d0,d1
        lsl.l   #6,d1
        swap    d1
        add.w   d1,d0
        and.w   #0x1ff8,d0
        lea     ram_hash,a0
        add.w   d0,a0
        move.l  #10,d0
rh_try:
        move.l  (a0)+,d1
        beq     rh_not_found
        cmp.l   d1,a1
        beq     rh_found
        add.w   #4,a0
        dbra    d0,rh_try
        bra     rh_not_found
rh_found:
        move.l  (a0)+,d0
        rts
rh_not_found:
#endif  /* USE_RAM_HASH */

        move.l  4(sp),a0
        move.l  #0,d0
        move.w  d0,d1
        bra     hash_next
hash_loop:
        rol.w   #1,d0
        add.w   d1,d0
hash_next:
        move.b  (a0)+,d1
        bne     hash_loop
        move.w  d0,d1
        rol.w   #6,d1
        and.w   #0x003F,d1
        eor.w   d1,d0
        and.w   #0x03FF,d0
        lsl.w   #2,d0
        move.l  _nls_hash,a0
        move.l  (a0,d0.w),a0
        cmp     #0,a0
        bne     search
// not in hash, return original string
not_found:
        move.l  4(sp),d0
#if USE_RAM_HASH
        jmp     store_ram_hash
#else
        rts
#endif
        
search: 
        move.l  (a0)+,d1
        beq     not_found
        exg     a0,d1
        move.l  4(sp),a1
strcmp:
        move.b  (a0)+,d0
        beq     strend
        cmp.b   (a1)+,d0
        beq     strcmp
// the strings differ, next
strneq:
        move.l  d1,a0
        add.w   #4,a0
        bra     search
strend: 
        tst.b   (a1)
        bne     strneq
// strings are equal, return next string
        move.l  d1,a0
        move.l  (a0),d0

#if USE_RAM_HASH
store_ram_hash:
        move.l  d0,a1
        lsl.l   #3,d0
        move.l  d0,d1
        lsl.l   #6,d1
        swap    d1
        add.w   d1,d0
        and.w   #0x1ff8,d0
        lea     ram_hash,a0
        add.w   d0,a0
        move.l  #10,d0
srh_try:
        tst.l   (a0)
        beq     srh_free_slot
        add.w   #8,a0
        dbra    d0,srh_try
// no room in ram hash
        bra     srh_end
srh_free_slot:
        move.l  4(sp),(a0)+
        move.l  a1,(a0)
srh_end:
        move.l  a1,d0
#endif /* USE_RAM_HASH */
        rts
        

        .bss
#if USE_RAM_HASH
ram_hash:
        ds.l    2048    // 1024 entries
        ds.l    20      // 10 additional slots.
#endif /* USE_RAM_HASH */
        
#endif /* ! CONF_NO_NLS */
 
