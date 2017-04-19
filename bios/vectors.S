/*
 * vectors.S - exception vectors
 *
 * Copyright (C) 2001-2017 by the EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_CHECK_READ_BYTE 0   // to trace check_read_byte() calls

#include "asmdefs.h"

// ==== Definitions ==========================================================

        .globl  _init_exc_vec
        .globl  _init_user_vec

        .globl  _gemtrap        // call VDI/AES
        .globl  _biostrap       // call BIOS from C
        .globl  _xbiostrap      // call XBIOS from C
        .globl  _bios_unimpl
        .globl  _xbios_unimpl

        .globl  _check_read_byte
        .globl  _int_vbl
#if CONF_WITH_ATARI_VIDEO
        .globl  _int_hbl
#endif
        .globl  _int_timerc
        .globl  _int_illegal
        .globl  _int_priv
#if CONF_WITH_ADVANCED_CPU
        .globl  _int_unimpint
#endif
        .globl  _just_rts
        .globl  _just_rte
        .globl  _bell
        .globl  _keyclick
        .globl  _trap_save_area

        .globl  _call_etv_critic
        .globl  _default_etv_critic

// ==== References ===========================================================

        // TOS System variables
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
buserr_vec:
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
unimp_vec:
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
 * Note that this function does not care about AES calls since the AES later
 * hooks into the trap-2 vector itself.
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


#if CONF_WITH_ATARI_VIDEO

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

#endif /* CONF_WITH_ATARI_VIDEO */


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

#if CONF_WITH_ATARI_VIDEO
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
#if CONF_WITH_VIDEL
        tst.w   _has_videl
        bne.s   set_video_low
#endif
#if CONF_WITH_TT_SHIFTER
        tst.w   _has_tt_shifter
        bne.s   set_video_low
#endif
#if CONF_WITH_STE_SHIFTER
        tst.w   _has_ste_shifter
        bne.s   set_video_low
#endif
#if CONF_WITH_VIDEL || CONF_WITH_TT_SHIFTER || CONF_WITH_STE_SHIFTER
        bra.s   st_shifter
set_video_low:
        move.b  d0,0xffff820d.w
st_shifter:
#endif
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
#endif /* CONF_WITH_ATARI_VIDEO */

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
        move.l  a0,-(sp)
        move.l  d0,-(sp)
        jsr     (a1)
        move.l  (sp)+,d0
        move.l  (sp)+,a0
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

#if CONF_WITH_ATARI_VIDEO
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

        // Note for non-Atari hardware:
        // It is important to return with RTE here, because FreeMiNT may have
        // hooked this routine. Every fourth interrupt, it pushes an additional
        // exception stack frame pointing to a pseudo-VBL routine. In that case,
        // the RTE below jumps to that pseudo-VBL. Then that one will return to
        // our caller with RTE.
        rte


// ==== Critical error handler functions =====================================

/*
 * (1) Call the critical error handler from C
 *     The handler is allowed to modify D0-D2/A0-A2, but GCC-compiled
 *     code expects D2/A2 to be unchanged, so we must save/restore them.
 */
_call_etv_critic:
        move.l  4(sp),d0            // d0 is error code || device
        move.l  a2,-(sp)            // save regs
        move.l  d2,-(sp)
        move.l  d0,-(sp)            // error code || device to stack
        moveq.l #-1,d0              // set default return code like TOS does
        movea.l _etv_critic,a0
        jsr     (a0)                // call handler
        addq.l  #4,sp               // fixup stack
        move.l  (sp)+,d2            // restore regs
        move.l  (sp)+,a2
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
        andi.w  #0xffc0,d0              // isolate move from CCR opcode
        cmpi.w  #0x42c0,d0              // we don't care where "To" was
        bne     ii_back                 // Not a MOVE CCR,XX instruction

        andi.w  #0xfdff,(a0)            // make it MOVE SR,XX
        movem.l (sp)+,d0/a0
        rte

ii_back:
        movem.l (sp)+,d0/a0
        bra     illg_vec                // go directly to our panic handler



/*
 * _int_priv - privileged instruction handler
 *
 * Convert a privileged 'MOVE SR,...' to 'MOVE CCR,...'
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

#if CONF_WITH_ADVANCED_CPU
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
        cmpi.b  #40,_mcpu+3
        beq.s   pi_inval
        cmpi.b  #60,_mcpu+3
        bne.s   pi_next

        // on 68040 and 68060, invalidate the line holding the opcode only
pi_inval:
        nop
        CINVL_IC_A0                     // cinvl   ic,(a0)
        nop

pi_next:
#endif
        movem.l (sp)+,d0/a0
        rte

pi_back:
        movem.l (sp)+,d0/a0
        bra     priv_vec                // go directly to our panic handler

#endif /* __mcoldfire__ */



#if CONF_WITH_ADVANCED_CPU

// ==== Trap 61 - unimplemented integer instruction entry point =============

/*
 * _int_unimpint - unimplemented integer instruction handler
 *
 * Only relevant for 68060s: certain instructions that were implemented
 * in earlier processors are no longer implemented, but raise this exception
 * instead.
 *
 * The only such instruction generally relevant for Atari systems is movep,
 * and the following code emulates that instruction.  An interrupt for any
 * of the other instructions in this category just raises a panic.
 */

/* defines for local stack usage */
#define SIZE_VARS   72
#define UI_VARS     -SIZE_VARS

#define UI_OPWORD   UI_VARS+0       /* opcode */
#define UI_EXTWORD  UI_VARS+2       /* extension word */
                                    /* we have 4 bytes available here */
#define UI_DREGS    UI_VARS+8       /* must be contiguous with UI_AREGS */
#define UI_AREGS    UI_VARS+40      /* must end at the end of the local frame */

#define UI_A6       UI_AREGS+(6*4)
#define UI_A7       UI_AREGS+(7*4)

#define UI_ISR      4               /* offsets to input exception frame after link */
#define UI_IPC      6
#define UI_IVOFF    10
#define UI_FRMEND   12
#define UI_F4_SR    -4              /* offsets to fields in faked fmt4 stack frame */
#define UI_F4_PC    -2              /* (these overlap the above, so be careful!) */
#define UI_F4_VOFF  2
#define UI_F4_ADDR  4
#define UI_F4_FSLW  8

/*
 * If the movep accesses an invalid memory address, we create a fake fmt4 exception
 * frame so we report the error as an access error.  The fmt4 frame includes a Fault
 * Status Long Word (FSLW).  The following values are returned by us (modified to
 * 'Supervisor code' as appropriate).
 */
#define FSLW_WRITE  0x00A10001  /* Write, Byte access, Normal, User code, Software emulation */
#define FSLW_READ   0x01210001  /* Read, Byte access, Normal, User code, Software emulation */

_int_unimpint:
        move.l  a0,-(sp)                    // save a0 temporarily
        move.l  6(sp),a0                    // a0 -> instruction causing interrupt
        btst.b  #0,(a0)                     // quick test for movep
        movea.l (sp)+,a0                    // restore a0, sp (does not change cc)
        beq     unimp_vec                   // not movep, go to panic handler
        link    a6,#-SIZE_VARS              // reserve stack space
        movem.l d0-d7/a0-a5,UI_DREGS(a6)    // save d0-d7, a0-a5
        move.l  (a6),UI_A6(a6)              // save a6

        /* get value of a7 that interrupted program saw */
        btst    #5,UI_ISR(a6)               // supervisor mode?
        bne.s   supmode                     // yes, branch
        move    usp,a0
        bra.s   save_a7

supmode:
        lea     UI_FRMEND(a6),a0

save_a7:
        move.l  a0,UI_A7(a6)                // save a7
        movea.l UI_IPC(a6),a0               // a0 -> pc
        move.l  (a0),d0                     // get instruction
        move.l  d0,UI_OPWORD(a6)            // store instruction on stack
        addq.l  #4,UI_IPC(a6)               // update pc to point to next instruction

        bsr     emulate_movep

        movem.l UI_DREGS(a6),d0-d7/a0-a5    // restore regs from stack
                                            // note we can't have changed sp/usp
        move.l  UI_A6(a6),(a6)
        unlk    a6
        rte

/*
 *  emulate_movep: emulate movep instruction
 *
 *  there are 4 cases:
 *      movep.w Dx,(d,Ay)   register-to-memory word
 *      movep.l Dx,(d,Ay)   register-to-memory long
 *      movep.w (d,Ay),Dx   memory-to-register word
 *      movep.l (d,Ay),Dx   memory-to-register long
 */
emulate_movep:
        move.w  UI_OPWORD(a6),d1            // get opcode
        move.b  d1,d0
        andi.w  #0x0007,d0                  // isolate Ay index
        lsl.w   #2,d0                       // convert to offset
        movea.l UI_AREGS(a6,d0.w),a0        // retrieve Ay into a0
        adda.w  UI_EXTWORD(a6),a0           // add displacement

        btst    #7,d1                       // check for memory-to-register
        beq     mem2reg

        /* handle register-to-memory: fetch Dx, write to memory */

        move.w  d1,d0
        rol.w   #7,d0
        andi.w  #0x0007,d0                  // isolate Dx index
        lsl.w   #2,d0                       // convert to offset
        move.l  UI_DREGS(a6,d0.w),d0        // retrieve Dx into d0
        btst    #6,d1                       // word or long ?
        beq.s   r2m_word                    // branch if word

        /* handle register-to-memory long */

        move.l  d0,d3                       // use d3 as data source
        movea.l a0,a3                       // use a3 as address ptr
        rol.l   #8,d3                       // write byte 0
        move.l  d3,d0
        bsr     movep_write_byte
        tst.l   d1
        bne     movep_write_err

        addq.w  #2,a3                       // write byte 1
        movea.l a3,a0
        rol.l   #8,d3
        move.l  d3,d0
        bsr     movep_write_byte
        tst.l   d1
        bne.s   movep_write_err

        addq.w  #2,a3                       // write byte 2
        movea.l a3,a0
        rol.l   #8,d3
        move.l  d3,d0
        bsr     movep_write_byte
        tst.l   d1
        bne.s   movep_write_err

        addq.w  #2,a3                       // write byte 3
        movea.l a3,a0
        rol.l   #8,d3
        move.l  d3,d0
        bsr     movep_write_byte
        tst.l   d1
        bne.s   movep_write_err
        rts

        /* handle register-to-memory word */
r2m_word:
        move.l  d0,d3                       // use d3 as data source
        movea.l a0,a3                       // use a3 as address ptr
        lsr.w   #8,d0                       // write byte 0
        bsr     movep_write_byte
        tst.l   d1
        bne.s   movep_write_err

        addq.w  #2,a3                       // write byte 1
        movea.l a3,a0
        move.l  d3,d0
        bsr     movep_write_byte
        tst.l   d1
        bne.s   movep_write_err
        rts

/*
 * write byte in d0 to memory pointed to by a0
 *
 * if accessible, d1 = 0
 * if inaccessible, d1 = 1
 *
 * clobbers a1, a2, d1, d2
 */
movep_write_byte:
        btst    #5,UI_ISR(a6)               // supervisor mode?
        bne.s   mwb_sup                     // yes, branch
        bsr     movep_check_prot            // check for protected areas in user mode
        tst.l   d1
        bne     mwb_exit
mwb_sup:
        move.l  sp,d2                       // save sp in d2
        move.l  (8).w,a2                    // old bus error vector in a2
        lea     mwb_berr.w(pc),a1
        move.l  a1,(8).w                    // update bus error vector
        moveq   #1,d1                       // set error flag

        nop                                 // flush pipeline
        move.b   d0,(a0)                    // write byte
        nop                                 // flush pipeline

        moveq   #0,d1                       // clear error flag
mwb_berr:
        move.l  a2,(8).w                    // restore saved values
        move.l  d2,sp
mwb_exit:
        rts

movep_write_err:
        move.l  a3,a0
        move.l  #FSLW_WRITE,d0
        bra     movep_err_exit

/*
 * handle memory-to-register: read memory, accumulate in Dx */
 */
mem2reg:
        btst    #6,d1                       // word or long?
        beq.s   m2r_word                    // branch if word

        /* handle memory-to-register long */

        movea.l a0,a3                       // use a3 as address ptr
        bsr     movep_read_byte             // read byte 0
        tst.l   d1
        bne     movep_read_err
        move.l  d0,d3                       // use d3 to accumulate register

        addq.w  #2,a3                       // read byte 1
        movea.l a3,a0
        bsr     movep_read_byte
        tst.l   d1
        bne     movep_read_err
        lsl.w   #8,d3
        move.b  d0,d3

        addq.w  #2,a3                       // read byte 2
        movea.l a3,a0
        bsr     movep_read_byte
        tst.l   d1
        bne     movep_read_err
        lsl.l   #8,d3
        move.b  d0,d3

        addq.w  #2,a3                       // read byte 3
        movea.l a3,a0
        bsr     movep_read_byte
        tst.l   d1
        bne.s   movep_read_err
        lsl.l   #8,d3
        move.b  d0,d3

        move.b  UI_OPWORD(a6),d0
        lsr.b   #1,d0
        andi.w  #0x0007,d0                  // isolate Dx index
        lsl.w   #2,d0                       // convert to offset
        move.l  d3,UI_DREGS(a6,d0.w)        // and update Dx
        rts

        /* handle memory-to-register word */
m2r_word:
        movea.l a0,a3                       // use a3 as address ptr
        bsr     movep_read_byte             // read byte 0
        tst.l   d1
        bne.s   movep_read_err
        move.l  d0,d3                       // use d3 to accumulate register

        addq.w  #2,a3                       // read byte 1
        movea.l a3,a0
        bsr     movep_read_byte
        tst.l   d1
        bne.s   movep_read_err
        lsl.w   #8,d3
        move.b  d0,d3

        move.b  UI_OPWORD(a6),d0
        lsr.b   #1,d0
        andi.w  #0x0007,d0                  // isolate Dx index
        lsl.w   #2,d0                       // convert to offset
        move.w  d3,UI_DREGS+2(a6,d0.w)      // and update Dx.w
        rts

/*
 * read byte pointed to by a0
 *
 * if accessible, d1 = 0, value in d0
 * if inaccessible, d1 = 1
 *
 * clobbers a1, a2, d1, d2
 */
movep_read_byte:
        btst    #5,UI_ISR(a6)               // supervisor mode?
        bne.s   mrb_sup                     // yes, branch
        bsr     movep_check_prot            // check for protected areas in user mode
        tst.l   d1
        bne.s   mrb_exit
mrb_sup:
        move.l  sp,d2                       // save sp in d2
        move.l  (8).w,a2                    // old bus error vector in a2
        lea     mrb_berr.w(pc),a1
        move.l  a1,(8).w                    // update bus error vector
        moveq   #1,d1                       // set error flag

        nop                                 // flush pipeline
        move.b   (a0),d0                    // read byte
        nop                                 // flush pipeline

        moveq   #0,d1                       // clear error flag
mrb_berr:
        move.l  a2,(8).w                    // restore saved values
        move.l  d2,sp
mrb_exit:
        rts

/* handle read error */
movep_read_err:
        move.l  a3,a0
        move.l  #FSLW_READ,d0
                                            // drop through
/*
 * exit for read/write errors
 *
 * a0 = fault address
 * d0 = FSLW
 *
 * we move fields around so that we end up with a format 4 exception frame
 * on the stack, starting 8 bytes lower than the original format 0 frame
 */
movep_err_exit:
        /* convert the fmt0 exception frame to a fmt4 */
        move.w  UI_ISR(a6),UI_F4_SR(a6)
        move.l  UI_IPC(a6),UI_F4_PC(a6)
        move.w  #0x4008,UI_F4_VOFF(a6)
        move.l  a0,UI_F4_ADDR(a6)
        move.l  d0,UI_F4_FSLW(a6)
        btst.b  #5,UI_F4_SR(a6)             // supervisor mode?
        beq.s   mee_restore                 // no, branch
        bset.b  #2,UI_F4_FSLW+1(a6)         // set TM2 bit (Supervisor code) in FSLW
mee_restore:
        lea     UI_DREGS(a6),sp
        movem.l (sp)+,d0-d7/a0-a6           // restore d0-d7, a0-a6
                                            // (end up with sp correct)
        bra     buserr_vec                  // & pretend we got a standard bus error

/*
 * check if trying to access a protected area in user mode
 *
 * a0 -> area to access
 *
 * if ok, returns d1 = 0
 * if not allowed, returns d1 = 1
 */
movep_check_prot:
        cmpa.l  #0x800,a0                   // $0-$7ff is protected
        bcs.b   mcp_err
        cmpa.l  #0x00f00000,a0              // $800-$efffff is not protected
        bcs.b   mcp_ok
        cmpa.l  #0x01000000,a0              // $f00000-$ffffff is protected
        bcs.b   mcp_err
        cmpa.l  #0xff000000,a0              // $1000000-$feffffff is not protected
        bcs.b   mcp_ok
                                            // $ff000000-$ffffffff is protected
mcp_err:
        moveq.l #1,d1                       // set error flag
        rts
mcp_ok:
        moveq.l #0,d1                       // clear error flag
        rts

#endif  /* CONF_WITH_ADVANCED_CPU */


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
        cmp.w   d1,d0           // Higher than highest number?
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


#if CONF_WITH_BUS_ERROR

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
        moveq   #0,d0
        nop                                     // flush pipeline

        move.l  4(sp),a0
        jbsr    buggy_jit_save_registers_before_bus_error
        tst.b   (a0)            // this may cause a Bus Error
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

#endif /* CONF_WITH_BUS_ERROR */


/*
 * _just_rts - just a dummy
 */

buggy_jit_save_registers_before_bus_error:
_just_rts:
        rts



/*
 * just_rte - just rte for e.g. divide by zero on 680x0 systems
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
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        jsr      (a0)
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts

/*
 * LONG protect_w(LONG (*func)(WORD), WORD);
 */

_protect_w:
        move.l   4(sp),a0
        move.w   8(sp),d0
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        move.w   d0,-(sp)
        jsr      (a0)
        addq.l   #2,sp
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts

/*
 * LONG protect_ww(LONG (*func)(WORD), WORD, WORD);
 */

_protect_ww:
        move.l   4(sp),a0
        move.l   8(sp),d0
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        move.l   d0,-(sp)
        jsr      (a0)
        addq.l   #4,sp
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts


/*
 * LONG protect_wlwwwl(LONG (*func)(), WORD, LONG, WORD, WORD, WORD, LONG);
 */

_protect_wlwwwl:
        movem.l  8(sp),d0-d1/a0-a1
        move.l   a2,-(sp)
        move.l   d2,-(sp)
#ifdef __mcoldfire__
        lea      -16(sp),sp
        movem.l  d0-d1/a0-a1,(sp)
#else
        movem.l  d0-d1/a0-a1,-(sp)
#endif
        move.l   28(sp),a0
        jsr      (a0)
        lea      16(sp),sp
        move.l   (sp)+,d2
        move.l   (sp)+,a2
        rts


/* Compendium says that the scancode is supplied in register d0. */
_keyclick:
        move.w   4(sp),d0
        move.l   a2,-(sp)
        move.l   d2,-(sp)
        move.l   _kcl_hook,a0
        jsr      (a0)
        move.l   (sp)+,d2
        move.l   (sp)+,a2
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
