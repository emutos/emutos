/*
 * vectors.S - exception vectors
 *
 * Copyright (c) 2001-2015 by the EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#define DBG_CHECK_READ_BYTE 0   // to trace check_read_byte() calls

#include "asmdefs.h"

        .globl  _init_exc_vec
        .globl  _init_user_vec

        .globl  _gemtrap        // call VDI/AES
        .globl  _biostrap       // call BIOS from C
        .globl  _xbiostrap      // call XBIOS from C
        .globl  _bios_unimpl
        .globl  _xbios_unimpl


        .globl  _check_read_byte
        .globl  _int_vbl
#if CONF_WITH_SHIFTER
        .globl  _int_hbl
#endif
        .globl  _int_timerc
        .globl  _int_illegal
        .globl  _int_priv
        .globl  _just_rts
        .globl  _just_rte
        .globl  _bell
        .globl  _keyclick
        .globl  _trap_save_area

        .globl  _call_etv_critic
        .globl  _default_etv_critic

// ==== tosvars.s - TOS System variables =====================================

        .extern _longframe
        .extern _vbclock
        .extern _frclock
        .extern _vblqueue
        .extern _swv_vec
        .extern _hz_200
        .extern etv_timer
        .extern etv_critic
        .extern _timer_ms
        .extern _v_bas_ad

        .extern _blink          // conout.c - console output

// Note: this scheme is designed to print the exception number
// for vectors 2 to 63 even if working on a 32bit address bus.
// LVL.

        .extern _dopanic

        .extern _proc_lives
        .extern _proc_dregs
        .extern _proc_aregs
        .extern _proc_enum
        .extern _proc_usp
        .extern _proc_stk

        .extern _stktop


        .text



/* basically initialize the 62 exception vectors. */

_init_exc_vec:
        clr.l   _proc_lives
        lea     deflt_vec_table, a0
        lea     8, a1
        move.l  #61, d0
set_vec:
        move.l  a0, (a1)+
        add.l   #2, a0
#ifdef __mcoldfire__
        subq.l  #1, d0
        bpl.s   set_vec
#else
        dbra    d0, set_vec
#endif
        rts

/* initialize the 192 user vectors. */

_init_user_vec:
        lea     user_vec, a0
        lea     256, a1
        move.l  #191, d0
set_uvec:
        move.l  a0, (a1)+
#ifdef __mcoldfire__
        subq.l  #1, d0
        bpl.s   set_uvec
#else
        dbra    d0, set_uvec
#endif
        rts

/* this table holds the default exception vectors for vectors 2 to 63.
 * thanks to the return address pushed by the bsr, any_vec is able
 * to re-compute the vector number. For this reason, jumping back to
 * the default vector after having attempted to e.g. overcome an
 * illegal exception should best be made by jumping to the actual vector
 * location (i.e. illg_vec below) and not directly to any_vec, as
 * the former method will allow any_vec to display the correct vector
 * number and report a meaningful panic message.
 */

deflt_vec_table:
        bsr.s   any_vec         // vector 2
        bsr.s   any_vec
illg_vec:
        bsr.s   any_vec
        bsr.s   any_vec         // vector 5
        bsr.s   any_vec
        bsr.s   any_vec
priv_vec:
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 10
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 15
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 20
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 25
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 30
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 35
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 40
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 45
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 50
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 55
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 60
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec

user_vec:
        pea     deflt_vec_table(pc)

// at this point, stack contains:
// 0:exception vector address 4:sr 6:pc
// we copy debug data in the structure at address 0x380, then go in dopanic()
any_vec:
        move.w  #0x2700, sr
        // dump the registers, including USP
#ifdef __mcoldfire__
        move.l  a0,_proc_dregs+32
        lea     _proc_dregs,a0
        movem.l d0-d7,(a0)
        movem.l a1-a6,36(a0)
#else
        movem.l d0-d7/a0-a6,_proc_dregs
#endif
        move.l  usp,a0
        move.l  a0,_proc_usp
        // compute and dump the exception number
        move.l  (sp)+,d0
        // dump the correct value for sp
        move.l  sp,_proc_aregs+28
        lea     deflt_vec_table(pc), a0
        sub.l   a0, d0
        lsr.l   #1, d0
        add.l   #1, d0
        move.l  d0, _proc_enum
        // dump 16 words from the stack
        movem.l (sp),d0-d7
#ifdef __mcoldfire__
        lea     _proc_stk,a0
        movem.l d0-d7,(a0)
        // magic value
        lea     _proc_lives, a0
        move.l  #0x12345678, (a0)
#else
        movem.l d0-d7,_proc_stk
        // magic value
        move.l  #0x12345678, _proc_lives
#endif
        // restore a sane stack
        // this stack must be large enough for dopanic(); as of
        // may 2015, this is about 460 bytes
        lea     panic_stack,sp
        jmp     _dopanic



/*
 * _gemtrap - VDI trap dispatcher
 *
 * This functions checks for a trap-2 VDI call and jumps to the VDI dispatcher
 * then. It also provides a simple vq_gdos call and the "d0=-1" query function
 * that returns the dispatcher's address.
 *
 * Note that this function does not care for AES calls since the AES later
 * hook into the trap-2 vector.
 *
 */

_gemtrap:
        cmp.w   #0x73,d0                // VDI call?
        bne     no_vdi
        jsr     _GSX_ENTRY
        rte

no_vdi:
        cmp.w   #0xfffe,d0              // vq_gdos call?
        beq     what_gdos

        cmp.w   #0xffff,d0              // query for jsr:able address?
        beq     query

        rte


what_gdos:
        move.l  #-2, d0                 // -2  -  no GDOS installed
        rte

query:
        move.l  #_GSX_ENTRY,d0          // Return address of VDI dispatcher
        rte


#if CONF_WITH_SHIFTER

/*
 * Int 0x68 - HBL interrupt
 */

_int_hbl:
#ifdef __mcoldfire__
// Beware: On ColdFire, the first word on the stack is the Format/Vector word,
// and the user SR comes after. Furthermore, we need to backup the whole d0.l.
// As a result the user status register is at 6(sp) instead of 2(sp)
        move.l  d0, -(sp)       // save d0
        moveq   #0, d0
        move.w  6(sp), d0       // get status register from stack
        and.l   #0x0700, d0     // isolate just IPL-mask from sr
        bne     is_ipl          // if IPL is nonzero, then end
        move.w  6(sp), d0
        or.l    #0x0300, d0     // else set IPL3 in status register
        move.w  d0, 6(sp)
is_ipl:
        move.l  (sp)+, d0       // restore d0
#else
        move.w  d0, -(sp)       // save d0
        move.w  2(sp), d0       // get status register from stack
        and.w   #0x0700, d0     // isolate just IPL-mask from sr
        bne     is_ipl          // if IPL is nonzero, then end
        or.w    #0x0300, 2(sp)  // else set IPL3 in status register
is_ipl:
        move.w  (sp)+, d0       // restore d0
#endif
        rte

#endif /* CONF_WITH_SHIFTER */


/*
 * _int_vbl - Int 0x70 - VBL interrupt
 *
 * no video resolution change is done.
 */

_int_vbl:
        addq.l  #1, _frclock            // increase num of happened ints
#ifdef __mcoldfire__
        move.l  d0, -(sp)               // ColdFire specific
        moveq   #0, d0
        move.w  _vblsem, d0
        subq.l  #1, d0
        move.w  d0, _vblsem
#else
        subq.w  #1, _vblsem             // check vbl semaphore
#endif
        bmi     vbl_end                 // if VBl routine disabled -> end

#ifdef __mcoldfire__
        lea     -56(sp), sp
        movem.l d1-d7/a0-a6, (sp)       // save registers
#else
        movem.l d0-d7/a0-a6, -(sp)      // save registers
#endif
        addq.l  #1, _vbclock            // count number of VBL interrupts

        // TODO: Check for monitor switching on normal ST(E) here,
        // and jump to _swv_vec if necessary...

        jsr     _blink                  // blink cursor

#if CONF_WITH_SHIFTER
// the contents of colorptr indicate the palette processing required; since
// the source address must be on a word boundary, we use bit 0 as a flag:
//  contents                 meaning
//  --------                 -------
//     0                     do nothing
//  address                  load ST(e) palette registers from address
//  address with flag set    load 16 Falcon palette registers from address
//     0 with flag set       load 256 Falcon palette registers from
//                             _falcon_shadow_palette
        move.l  _colorptr,d0
        beq     vbl_no_palette
        // load new color palette
        clr.l   _colorptr
        lea     0xffff8240.w,a1             // ST(e) hardware palette registers
        moveq   #7,d1                       // count/2 - 1 (we move longwords)
        bclr    #0,d0                       // test & clear Falcon indicator
#if CONF_WITH_VIDEL
        beq.s   docopy                      // wasn't set, so update STe regs
        // load Falcon color palette
        lea     0xffff9800.w,a1             // Falcon palette registers
        moveq   #15,d1                      // assume 16 only
        tst.l   d0                          // source address specified?
        bne.s   docopy                      // yes, go update 16
        move.l  #_falcon_shadow_palette,d0  // else set source address
        move.l  #255,d1                     //  & prepare to update 256
docopy:
#endif
        move.l  d0,a0                       // a0 -> source
vbl_palette_loop:
        move.l  (a0)+,(a1)+
#ifdef __mcoldfire__
        subq.l  #1,d1
        bpl.s   vbl_palette_loop
#else
        dbra    d1,vbl_palette_loop
#endif
vbl_no_palette:

        // set new video address
        move.l  _screenpt,d0
        beq     vbl_no_screenpt
        move.l  d0,_v_bas_ad
#if CONF_WITH_STE_SHIFTER
        tst.w   _has_ste_shifter
        beq     st_shifter
        move.b  d0,0xffff820d.w
#endif
st_shifter:
#ifdef __mcoldfire__
        lsr.l   #8,d0
        move.b  d0,0xffff8203.w
        lsr.l   #8,d0
#else
        lsr.w   #8,d0
        move.b  d0,0xffff8203.w
        swap    d0
#endif
        move.b  d0,0xffff8201.w
vbl_no_screenpt:
#endif /* CONF_WITH_SHIFTER */

#if CONF_WITH_FDC
        // flopvbl
        jsr     _flopvbl
#endif

        // vblqueue
#ifdef __mcoldfire__
        moveq   #0,d0
#endif
        move.w  _nvbls,d0
        beq     vbl_no_queue
#ifdef __mcoldfire__
        sub.l   #1,d0
#else
        sub.w   #1,d0
#endif
        move.l  _vblqueue,a0
vbl_queue_loop:
        move.l  (a0)+,a1
        cmp.l   #0,a1
        beq     vbl_queue_next
#ifdef __mcoldfire__
        move.l  a0,-(sp)
        move.l  d0,-(sp)
#else
        movem.l d0/a0,-(sp)
#endif
        jsr     (a1)
#ifdef __mcoldfire__
        move.l  (sp)+,d0
        move.l  (sp)+,a0
#else
        movem.l (sp)+,d0/a0
#endif
vbl_queue_next:
#ifdef __mcoldfire__
        subq.l  #1,d0
        bpl.s   vbl_queue_loop
#else
        dbra    d0,vbl_queue_loop
#endif
vbl_no_queue:

        // screen hardcopy
        // (not done)

#ifdef __mcoldfire__
        movem.l (sp), d1-d7/a0-a6       // restore registers
        lea     56(sp), sp
#else
        movem.l (sp)+, d0-d7/a0-a6      // restore registers
#endif
vbl_end:
#ifdef __mcoldfire__
        moveq   #0,d0
        move.w  _vblsem,d0
        addq.l  #1,d0
        move.w  d0,_vblsem
        move.l  (sp)+,d0                // ColdFire specific
#else
        addq.w  #1,_vblsem
#endif

#if CONF_WITH_SHIFTER
        rte
#else
        rts
#endif



// ==== Timer C interrupt handler ============================================

#define REAL_TIMER_C_HANDLER (CONF_WITH_MFP && !CONF_COLDFIRE_TIMER_C)

_int_timerc:
        addq.l  #1, _hz_200             // increment 200 Hz counter

#ifdef __mcoldfire__
        // Save early ColdFire registers
        move.l  d0,-(sp)
        move.l  a0,-(sp)
#endif

#ifdef __mcoldfire__
        lea     _timer_c_sieve,a0
        move.w  (a0),d0
        swap    d0
        move.w  (a0),d0
        lsl.l   #1,d0                   // check for 4th call
        swap    d0
        move.w  d0,(a0)
        bpl     timerc_end
#else
        rol.w   _timer_c_sieve          // check for 4th call
        bpl     timerc_end
#endif

#ifdef __mcoldfire__
        // On ColdFire, d0 and a0 were saved earlier
        lea     -52(sp),sp
        movem.l d1-d7/a1-a6,(sp)        // save registers
#else
        movem.l d0-d7/a0-a6,-(sp)       // save registers
#endif

        // repeat keys
        jsr     _kb_timerc_int

#if CONF_WITH_YM2149
        // dosound support
        jsr     _sndirq
#endif

        move.w  _timer_ms, -(sp)
        move.l  _etv_timer, a0
        jsr     (a0)                    // jump to etv_timer routine
        addq.l  #2, sp                  // correct stack

#ifdef __mcoldfire__
        // On ColdFire, d0 and a0 will be restored later
        movem.l (sp),d1-d7/a1-a6
        lea     52(sp),sp
#else
        movem.l (sp)+,d0-d7/a0-a6
#endif

timerc_end:
#if REAL_TIMER_C_HANDLER
#ifdef __mcoldfire__
        lea     0xfffffa11.w, a0
        bclr    #5, (a0)                // clear interrupt service bit
#else
        bclr    #5, 0xfffffa11.w        // clear interrupt service bit
#endif
#endif /* REAL_TIMER_C_HANDLER */

#ifdef __mcoldfire__
        // Restore late ColdFire registers
        move.l  (sp)+, a0
        move.l  (sp)+, d0
#endif

#if REAL_TIMER_C_HANDLER
        rte
#else
        rts
#endif


// ==== Critical error handler functions =====================================

/*
 * (1) Call the critical error handler from C
 *     The handler is allowed to modify D0-D2/A0-A2, but GCC-compiled
 *     code expects D2/A2 to be unchanged, so we must save/restore them.
 */
_call_etv_critic:
        move.l  4(sp),d0            // d0 is error code || device
#ifdef __mcoldfire__
        move.l  a2,-(sp)            // save regs
        move.l  d2,-(sp)
#else
        movem.l d2/a2,-(sp)         // save regs
#endif
        move.l  d0,-(sp)            // error code || device to stack
        moveq.l #-1,d0              // set default return code like TOS does
        movea.l _etv_critic,a0
        jsr     (a0)                // call handler
        addq.l  #4,sp               // fixup stack
#ifdef __mcoldfire__
        move.l  (sp)+,d2            // restore regs
        move.l  (sp)+,a2
#else
        movem.l (sp)+,d2/a2         // restore regs
#endif
        rts
/*
 * (2) The default character-mode critical error handler
 *     Just returns the error code from the stack
 */
_default_etv_critic:
        move.w  4(sp),d0            // get error code
        ext.l   d0
        rts


#ifndef __mcoldfire__

/*
 * _int_illegal - illegal instruction handler
 *
 * Convert an illegal 'MOVE CCR,...' to 'MOVE SR,...'
 * (see bios/processor.h for the full story)
 */

_int_illegal:
        movem.l d0/a0,-(sp)             // saving d0 and a0 while we test
        movea.l 10(sp),a0               // the offending address
        move.w  (a0),d0                 // the offending instruction
        andi.w  #0xffc0,d0              // isolate move from SR opcode
        cmpi.w  #0x42c0,d0              // we don't care where "To" was
        bne     ii_back                 // Not a MOVE SR,XX instruction

        andi.w  #0xfdff,(a0)            // make it MOVE SR,XX
        movem.l (sp)+,d0/a0
        rte

ii_back:
        movem.l (sp)+,d0/a0
        bra     illg_vec                // go directly to our panic handler



/*
 * _int_priv - privileged instruction handler
 *
 * Convert a priviledged 'MOVE SR,...' to 'MOVE CCR,...'
 * (see bios/processor.h for the full story)
 */

_int_priv:
        movem.l d0/a0,-(sp)             // saving d0 and a0 while we test
        movea.l 10(sp),a0               // the offending address
        move.w  (a0),d0                 // the offending instruction
        andi.w  #0xffc0,d0              // isolate move from SR opcode
        cmpi.w  #0x40c0,d0              // we don't care where "To" was
        bne     pi_back                 // Not a MOVE SR,XX instruction

        ori.w   #0x200,(a0)             // make it MOVE CCR,XX

        cmpi.b  #30,_mcpu+3
        bne.s   pi_not30

        // on 68030, invalidate the i-cache line we just modified
        move.l  a0,d0                   // get full address
        andi.l  #0x000000fc,d0          //  zero out unused bits
        MOVEC_D0_CAAR                   //   & set caar -> index/longword of cache line
        MOVEC_CACR_D0                   // get current cacr
        ori.b   #0x04,d0                // set the CEI bit
        MOVEC_D0_CACR                   // invalidate the line
        bra.s   pi_next

pi_not30:
        cmp.b   #40,_mcpu+3
        bne     pi_next

        // 68040 only, invalidate the line holding the opcode only
        nop
        .dc.w   0xf488                  // cinvl   ic,(a0)
        nop

pi_next:
        movem.l (sp)+,d0/a0
        rte

pi_back:
        movem.l (sp)+,d0/a0
        bra     priv_vec                // go directly to our panic handler

#endif /* __mcoldfire__ */


// ==== Trap 13 - BIOS entry point ==========================================

_biostrap:
        move.w  _bios_ent,d1
        lea     _bios_vecs,a0
        bra.s   biosxbios

// ==== Trap 14 - XBIOS entry point =========================================

_xbiostrap:
        move.w  _xbios_ent,d1
        lea     _xbios_vecs,a0


// ==== Trap 13+14 handler ==================================================

biosxbios:
        move.l  _savptr, a1
#ifdef __mcoldfire__
        move.w  (sp)+,d0        // Format/Vector Word
        andi.l  #0x3000,d0      // Keep the filler bits
        beq.s   bx_cf_nofiller

        // Assume a 2-byte filler after the exception frame
        move.w  (sp)+,d0        // Status register -> d0
        move.w  d0,-(a1)        // and save in save_area
        move.l  (sp)+,-(a1)     // save return address
        addq.l  #2,sp           // discard the filler word
        bra.s   bx_nolongframe

bx_cf_nofiller:
        move.w  (sp)+,d0        // Status register -> d0
        move.w  d0,-(a1)        // and save in save_area
        move.l  (sp)+,-(a1)     // save return address
#else
        move.w  (sp)+,d0        // Status register -> d0
        move.w  d0,-(a1)        // and save in save_area
        move.l  (sp)+,-(a1)     // save return address

        tst.w   _longframe      // Check processor type
        beq.s   bx_nolongframe  // ...when we support CPU >=68000
        move.w  (sp)+,-(a1)
#endif

bx_nolongframe:
#ifdef __mcoldfire__
        lea     -40(a1), a1
        movem.l d3-d7/a3-a7, (a1)       // regs, including stack pointer
#else
        movem.l d3-d7/a3-a7, -(a1)      // regs, including stack pointer
#endif
        move.l  a1, _savptr

        btst    #13,d0          // were we in user mode?
        bne     bx_sp_ok        // yes, the sp already points to the arguments
        move.l  usp,sp          // no, the arguments were on the user stack
bx_sp_ok:
        moveq   #0,d0           // clear d0 (use d0 like the original TOS)
        move.w  (sp)+,d0        // remove the function number from stack
        cmp.w   d1,d0           // Higher, than highest number?
        bhs.s   bx_ret_exc
        move.l  d0,d1           // if not implemented, returns the func number
        add.l   d1,d1           // indirection function table is 1 LW per
        add.l   d1,d1           // so multiply function number by 4
        add.l   d1,a0           // add to the base address of lookup table
        move.l  (a0),a0         // get the procedures address
        jsr     (a0)            // go do it and then come back

bx_ret_exc:
        move.l  _savptr, a1
#ifdef __mcoldfire__
        movem.l (a1), d3-d7/a3-a7       // Get regs back, including sp
        lea     40(a1), a1
#else
        movem.l (a1)+, d3-d7/a3-a7      // Get regs back, including sp
#endif

#ifdef __mcoldfire__
        move.l  (a1)+,-(sp)     // return adress
        move.w  (a1)+,-(sp)     // sr
        move.w  #0x40B4,-(sp)   // store a fake Format/Vector Word
#else
        tst.w   _longframe      // Check longframe again: Is CPU >= 68000?
        beq.s   bx_nolong2
        move.w  (a1)+,-(sp)

bx_nolong2:
        move.l  (a1)+,-(sp)
        move.w  (a1)+,-(sp)
#endif
        move.l  a1, _savptr
        rte                     // return with return value in D0



/*
 * (X)BIOS unimplemented function handler.
 * These functions are called indirectly, ONLY from biosxbios.
 * The requested BIOS/XBIOS function number is in d0.
 */

_bios_unimpl:
       move.w   d0,-(sp)
       jsr      _bios_do_unimpl
       addq.l   #2,sp
       rts

_xbios_unimpl:
       move.w   d0,-(sp)
       jsr      _xbios_do_unimpl
       addq.l   #2,sp
       rts


/* ==== check_read_byte =====================================================
 *
 * check_read_byte - Verify access to the memory pointed to by the ptr
 *
 * C call: int check_read_byte(long);
 * returns 0 if reading the address caused a bus error
 */

_check_read_byte:
        move.l  sp,d1
        move.l  (8).w,a1
        lea     berr.w(pc),a0
        move.l  a0,(8).w
        clr.l   d0
        nop                                     // flush pipeline

        move.l  4(sp),a0
        tst.b   (a0)
        nop                                     // flush pipeline

        moveq   #0x01,d0        // passed

berr:   move.l  a1,(8).w
        move.l  d1,sp
        nop

#if DBG_CHECK_READ_BYTE
        move.l  d0,-(sp)        // backup return value

        move.w  d0,-(sp)
        move.l  a0,-(sp)
        pea     msgcrb(pc)
        jsr     _kprintf        // print debug message
        lea     10(sp),sp

        move.l  (sp)+,d0        // restore return value
#endif
        rts

#if DBG_CHECK_READ_BYTE
msgcrb:
        .asciz  "check_read_byte(0x%08lx) returns %d\n"
        .even
#endif



/*
 * _just_rts - just a dummy
 */

_just_rts:
        rts



/*
 * just_rte - just rte for divide by zero
 */

_just_rte:
        rte


/* ==== Glue for external vectors ===========================================
 *
 * EmuTOS uses d0-d1/a0-a1 as scratch registers. However the original TOS
 * used d0-d2/a0-a2 as scratch registers. For this reason, when jumping into
 * external user-supplied code we need to save/restore d2/a2. The routines
 * below provide the necessary assembler support.
 */

        .globl   _protect_v
        .globl   _protect_w
        .globl   _protect_ww
        .globl   _protect_wlwwwl

/*
 * LONG protect_v(LONG (*func)(void));
 */

_protect_v:
        move.l   4(sp),a0
#ifdef __mcoldfire__
        move.l   a2,-(sp)
        move.l   d2,-(sp)
#else
        movem.l  d2/a2,-(sp)
#endif
        jsr      (a0)
#ifdef __mcoldfire__
        move.l   (sp)+,d2
        move.l   (sp)+,a2
#else
        movem.l  (sp)+,d2/a2
#endif
        rts

/*
 * LONG protect_w(LONG (*func)(WORD), WORD);
 */

_protect_w:
        move.l   4(sp),a0
        move.w   8(sp),d0
#ifdef __mcoldfire__
        move.l   a2,-(sp)
        move.l   d2,-(sp)
#else
        movem.l  d2/a2,-(sp)
#endif
        move.w   d0,-(sp)
        jsr      (a0)
        addq.l   #2,sp
#ifdef __mcoldfire__
        move.l   (sp)+,d2
        move.l   (sp)+,a2
#else
        movem.l  (sp)+,d2/a2
#endif
        rts

/*
 * LONG protect_ww(LONG (*func)(WORD), WORD, WORD);
 */

_protect_ww:
        move.l   4(sp),a0
        move.l   8(sp),d0
#ifdef __mcoldfire__
        move.l   a2,-(sp)
        move.l   d2,-(sp)
#else
        movem.l  d2/a2,-(sp)
#endif
        move.l   d0,-(sp)
        jsr      (a0)
        addq.l   #4,sp
#ifdef __mcoldfire__
        move.l   (sp)+,d2
        move.l   (sp)+,a2
#else
        movem.l  (sp)+,d2/a2
#endif
        rts


/*
 * LONG protect_wlwwwl(LONG (*func)(), WORD, LONG, WORD, WORD, WORD, LONG);
 */

_protect_wlwwwl:
        movem.l  8(sp),d0-d1/a0-a1
#ifdef __mcoldfire__
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        lea      -16(sp),sp
        movem.l  d0-d1/a0-a1,(sp)
#else
        movem.l  d2/a2,-(sp)
        movem.l  d0-d1/a0-a1,-(sp)
#endif
        move.l   28(sp),a0
        jsr      (a0)
        lea      16(sp),sp
#ifdef __mcoldfire__
        move.l   (sp)+,d2
        move.l   (sp)+,a2
#else
        movem.l  (sp)+,d2/a2
#endif
        rts


/* Compendium says that the scancode is supplied in register d0. */
_keyclick:
        move.w   4(sp),d0
#ifdef __mcoldfire__
        move.l   a2,-(sp)
        move.l   d2,-(sp)
#else
        movem.l  d2/a2,-(sp)
#endif
        move.l   _kcl_hook,a0
        jsr      (a0)
#ifdef __mcoldfire__
        move.l   (sp)+,d2
        move.l   (sp)+,a2
#else
        movem.l  (sp)+,d2/a2
#endif
        rts

// ==== Variables ============================================================

        .bss
        .even

        .ds.w    256            // this stack is only used by dopanic()
panic_stack:

/* save area for trap13/14. */
save_beg:       .ds.w   24*3    // d3-d7/a3-a7, sr, pc, vo = 24 words,
                                // multiplied by 3 ("3 level deep reentrant")
_trap_save_area:                     // End of Save storage



// ===========================================================================
// ==== End ==================================================================
// ===========================================================================
