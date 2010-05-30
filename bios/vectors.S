/*
 * vectors.S - exception vectors
 *
 * Copyright (c) 2001-2002 by the EmuTOS Development Team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */



        .global _init_exc_vec
        .global _init_user_vec

        .global _trap_1         // call GEMDOS from C
        .global _gemtrap        // call VDI/AES
        .global _biostrap       // call BIOS from C
        .global _xbiostrap      // call XBIOS from C
        .global _bios_unimpl
        .global _xbios_unimpl
        

        .global _check_read_byte
        .global _int_vbl
        .global _int_hbl
        .global _int_timerc
        .global _int_illegal
        .global _int_priv
        .global _just_rts
        .global _just_rte
        .global _bell
        .global _keyclick
        .global _trap_save_area

                
// ==== tosvars.s - TOS System variables =====================================

        .xdef   _longframe
        .xdef   _vbclock
        .xdef   _frclock
        .xdef   vblsem
        .xdef   nvbls
        .xdef   _vblqueue
        .xdef   _swv_vec
        .xdef   _hz_200
        .xdef   savptr
        .xdef   etv_timer
        .xdef   etv_critic
        .xdef   _timer_ms
        .xdef   _v_bas_ad
        .xdef   colorptr
        .xdef   screenpt

        .xdef   _blink          // conout.c - console output

// Note: this scheme is designed to print the exception number
// for vectors 2 to 63 even if working on a 32bit address bus. 
// LVL.

        .xdef   _dopanic
        
        .xdef   _proc_lives
        .xdef   _proc_dregs     
        .xdef   _proc_aregs     
        .xdef   _proc_enum      
        .xdef   _proc_usp       
        .xdef   _proc_stk       
        
        .xdef   _stktop


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
        lea     _stktop,sp
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



/* 
 * Int 0x68 - HBL interrupt 
 */
 
_int_hbl:
#ifdef __mcoldfire__
// Beware: In the ColdFire version, d0.l is pushed on the stack
// so the user status register is in 4(sp)
        move.l  d0, -(sp)       // save d0
        moveq   #0, d0
        move.w  4(sp), d0       // get status register from stack
        and.l   #0x0700, d0     // isolate just IPL-mask from sr
        bne     is_ipl          // if IPL is nonzero, then end
        move.w  4(sp), d0
        or.l    #0x0300, d0     // else set IPL3 in status register
        move.w  d0, 4(sp)
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

        // load new color palette
        move.l  _colorptr,d0
        beq     vbl_no_palette
        clr.l   _colorptr
        move.l  d0,a0
        lea     0xffff8240,a1
        moveq   #15,d0
vbl_palette_loop:
        move.w  (a0)+,(a1)+
#ifdef __mcoldfire__
        subq.l  #1,d0
        bpl.s   vbl_palette_loop
#else
        dbra    d0,vbl_palette_loop
#endif
vbl_no_palette: 

        // set new video address
        move.l  _screenpt,d0
        beq     vbl_no_screenpt
        move.l  d0,_v_bas_ad
        tst.w   _has_ste_shifter
        beq     st_shifter
        move.b  d0,0xffff820d
st_shifter:
#ifdef __mcoldfire__
        lsr.l   #8,d0
        move.b  d0,0xffff8203
        lsr.l   #8,d0
#else
        lsr.w   #8,d0
        move.b  d0,0xffff8203
        swap    d0
#endif
        move.b  d0,0xffff8201
vbl_no_screenpt:        

        // flopvbl
        jsr     _flopvbl

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
        rte



// ==== Timer C interrupt handler ============================================
_int_timerc:
        
        addq.l  #1, _hz_200             // increment 200 Hz counter

#ifdef __mcoldfire__
        move.l  d0,-(sp)
        move.l  a0,-(sp)
        move.w  _timer_c_sieve,d0
        swap    d0
        move.w  _timer_c_sieve,d0
        lsl.l   #1,d0                   // check for 4th call
        swap    d0
        move.w  d0,_timer_c_sieve
        bpl     timerc_end

        lea     -52(sp),sp
        movem.l d1-d7/a1-a6,(sp)        // save registers       
#else
        rol.w   _timer_c_sieve          // check for 4th call
        bpl     timerc_end

        movem.l d0-d7/a0-a6,-(sp)       // save registers       
#endif

        // repeat keys 
        jsr     _kb_timerc_int

        // dosound support
        jsr     _sndirq
        
        move.w  _timer_ms, -(sp)
        move.l  _etv_timer, a0
        jsr     (a0)                    // jump to etv_timer routine
        addq.l  #2, sp                  // correct stack
        
#ifdef __mcoldfire__
        movem.l (sp),d1-d7/a1-a6
        lea     52(sp),sp
timerc_end:                     
        lea     0xfffffa11, a0
        bclr    #5, (a0)                // clear interrupt service bit
        move.l  (sp)+, a0
        move.l  (sp)+, d0
#else
        movem.l (sp)+,d0-d7/a0-a6
timerc_end:                     
        bclr    #5, 0xfffffa11          // clear interrupt service bit
#endif
        rte
        


// ==== Critical error handler ===============================================
// Just sets D0 (return code) to -1, end ends the subroutine
        .global _criter
        .global _criter1
_criter:
        move.l  _criter, -(sp)  // etv_critic on stack
_criter1:
        moveq.l #-1, d0         // Default error
        rts                     // jump back to routine


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
        
        cmp.b   #40,_mcpu+3
        bne     pi_next
        
        // 68040 only, invalidate the line holding the opcode only
        nop
        dc.w    0xf488                  // cinvl   ic,(a0)
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
        move.l  #0,d0           // clear d0 (use d0 like the original TOS)
        move.w  (sp)+,d0        // remove the function number from stack
        cmp.w   d1,d0           // Higher, than highest number?
        bge     bx_ret_exc
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
 */

_bios_unimpl:
       move.w   d1,-(sp)
       jsr      _bios_do_unimpl
       addq.l   #2,sp
       rts

_xbios_unimpl:
       move.w   d1,-(sp)
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
        rts



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

        .global  _protect_v
        .global  _protect_w
        .global  _protect_ww
        .global  _protect_wlwwwl
        
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

/* save area for trap13/14. */
save_beg:       ds.w    24*3    // d3-d7/a3-a7, sr, pc, vo = 24 words, 
                                // multiplied by 3 ("3 level deep reentrant")
_trap_save_area:                     // End of Save storage



// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end
