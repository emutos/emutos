/*
 * vectors.S - default exception vectors
 *
 * Copyright (c) 2001 by Authors:
 *
 *  LVL  Laurent Vogel
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
        .global _brkpt
        .global _just_rts
        .global _just_rte
        .global _save_area

                
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

        .xdef   _blink          // conout.s - Graphics output

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
        


        .text
        

//
// initialize the 62 exception vectors.
//
_init_exc_vec:
        clr.l   _proc_lives
        lea     deflt_vec_table, a0
        lea     8, a1
        move.l  #61, d0
set_vec:
        move.l  a0, (a1)+
        add.l   #2, a0
        dbra    d0, set_vec
        rts
//
// initialize the 192 user vectors.
//
_init_user_vec: 
        lea     user_vec, a0
        lea     256, a1
        move.l  #191, d0
set_uvec:
        move.l  a0, (a1)+
        dbra    d0, set_uvec
        rts

deflt_vec_table:
        bsr.s   any_vec         // vector 2
        bsr.s   any_vec
        bsr.s   any_vec
        bsr.s   any_vec         // vector 5
        bsr.s   any_vec
        bsr.s   any_vec
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

// at this point, stack contains
// 0:exception vector address 4:sr 6:pc
any_vec:
        move.w  #0x2700, sr
        movem.l d0-d7, _proc_dregs
        move.l  (sp)+, d0
        movem.l a0-a7, _proc_aregs
        lea     deflt_vec_table(pc), a0
        sub.l   a0, d0
        lsr.l   #1, d0
        add.l   #1, d0
        move.l  d0, _proc_enum
        move    usp, a0
        move.l  a0, _proc_usp
        lea     _proc_stk, a0
        move.l  a7, a1
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  (a1)+, (a0)+
        move.l  #0x12345678, _proc_lives
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
 *
 */
 
_int_hbl:
        move.w  d0, -(sp)       // save d0
        move.w  2(sp), d0       // get status register from stack
        and.w   #0x0700, d0     // isolate just PIL-mask from sr
        bne     is_ipl0         // if IPL0, then end
        or.w    #0x0300, 2(sp)  // else set IPL3 in status register
        move.w  (sp)+, d0       // restore d0
is_ipl0:
        rte



// ==== Int 0x70 - VBL interrupt =============================================
// no video resolution change is done.

_int_vbl:
        addq.l  #1, _frclock            // increase num of happened ints
        subq.l  #1, _vblsem             // check vblsem
        bmi     vbl_end                 // if VBl routine disabled -> end
        
        movem.l d0-d7/a0-a6, -(sp)      // save registers
        addq.l  #1, _vbclock            // count number of VBL interrupts

#if 0   // MAD: does not work with Aranym for now - disabled
        // detect emulator friendly rez change (for now just ST modes)
        move.b  0xffff8260,d0           // get shifter mode
        and.b   #3, d0                  // isolate first 2 bits
        move.w  _defshiftmod, d1        // get new mode
        and.b   #3, d1                  // isolate first 2 bits
        cmp.b   d0, d1                  // has mode changed?
        beq     vbl_nochg               // no...
        
        move.b  d1, _sshiftmod          // set new mode to tosvars
        move.b  d1, 0xffff8260          // and to HW
        move.l  _swv_vec, a0            // video mode change vector
        jmp (a0)                        // is normally reset
 
vbl_nochg:
#endif

        bsr _blink                       // blink cursor

        // load new color palette
        move.l  _colorptr,d0
        beq     vbl_no_palette
        move.l  d0,a0
        lea     0xffff8240,a1
        move.w  #15,d0
vbl_palette_loop:
        move.w  (a0)+,(a1)+
        dbra    d0,vbl_palette_loop
vbl_no_palette: 

        // set new video address
        move.l  _screenpt,d0
        beq     vbl_no_screenpt
        move.l  d0,_v_bas_ad
        tst.w   _has_ste_shifter
        beq     st_shifter
        move.b  d0,0xffff820d
st_shifter:
        lsr.w   #8,d0
        move.b  d0,0xffff8203
        swap    d0
        move.b  d0,0xffff8201
vbl_no_screenpt:        

        // flopvbl
        jsr     _flopvbl

        // vblqueue
        move.w  _nvbls,d0
        beq     vbl_no_queue
        sub.w   #1,d0
        move.l  _vblqueue,a0
vbl_queue_loop: 
        move.l  (a0)+,a1
        cmp.l   #0,a1
        beq     vbl_queue_next
        movem.l d0/a0,-(sp)
        jsr     (a1)
        movem.l (sp)+,d0/a0
vbl_queue_next: 
        dbra    d0,vbl_queue_loop
vbl_no_queue:
        
        // screen hardcopy
        // (not done) 

        movem.l (sp)+, d0-d7/a0-a6      // restore registers
vbl_end:
        addq.l  #1, _vblsem      //
        rte


// ==== breakpoint for illegal instruction ===================================
_brkpt:
        illegal
        bra.s   _brkpt  // never return

// ==== Timer A interrupt handler ============================================
        .global _int_timera
_int_timera:
        rte             // Just a dummy

// ==== Timer B interrupt handler ============================================
        .global _int_timerb
_int_timerb:
        rte             // Just a dummy

// ==== Timer C interrupt handler ============================================
        .global _int_timerc
_int_timerc:
        
        addq.l  #1, _hz_200             // increment 200 Hz counter

        rol.w   _timer_c_sieve          // check for 4th call
        bpl     timerc_end

        movem.l d0-d7/a0-a6,-(sp)       // save registers       
        // TODO, repeat keys ?

        // dosound support
        jsr     _sndirq
        
        move.w  _timer_ms, -(sp)
        move.l  _etv_timer, a0
        jsr     (a0)                    // jump to etv_timer routine
        addq.w  #2, sp                  // correct stack
        
        movem.l (sp)+,d0-d7/a0-a6
timerc_end:                     
        bclr    #5, 0xfffffa11          // clear interrupt service bit
        rte
        

// ==== Timer D interrupt handler ============================================
        .global _int_timerd
_int_timerd:
        rte             // Just a dummy



// ==== txerror ==============================================================
_txerror:
        rte             // Just a dummy

// ==== txerror ==============================================================
_txrint:
        rte             // Just a dummy

// ==== txerror ==============================================================
_rxerror:
        rte             // Just a dummy

// ==== txerror ==============================================================
_rcvint:
        rte             // Just a dummy



// ==== Critical error handler ===============================================
// Just sets D0 (return code) to -1, end ends the subroutine
        .global _criter
        .global _criter1
_criter:
        move.l  _criter, -(sp)  // etv_critic on stack
_criter1:
        moveq.l #-1, d0         // Default error
        rts                     // jump back to routine
        



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
        move.w  (sp)+,d0        // Status register -> d0
        move.w  d0,-(a1)        // and save in save_area
        move.l  (sp)+,-(a1)     // save return address

        tst.w   _longframe      // Check processor type
        beq.s   bx_nolongframe  // ...when we support CPU >=68000
        move.w  (sp)+,-(a1)

bx_nolongframe:
        movem.l d3-d7/a3-a7, -(a1)      // regs, including stack pointer
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
        movem.l (a1)+, d3-d7/a3-a7      // Get regs back, including sp

        tst.w   _longframe      // Check longframe again: Is CPU >= 68000?
        beq.s   bx_nolong2
        move.w  (a1)+,-(sp)

bx_nolong2:
        move.l  (a1)+,-(sp)
        move.w  (a1)+,-(sp)
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


/*
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



// ===========================================================================
// ==== DATA in text segment =================================================
// ===========================================================================

// ==== BIOS entry points to allow individual module compilation =============

#if 0  /* moved in bios.c */
_bios_vecs:
        dc.l   _bios_0
        dc.l   _bios_1                 // LONG character_input_status()
        dc.l   _bios_2                 // LONG character_input()
        dc.l   _bios_3                 // void character_output()
        dc.l   _bios_4                 // LONG read_write_sectors()
        dc.l   _bios_5                 // set vector
        dc.l   _bios_6                 // LONG get_timer_ticks()
        dc.l   _bios_7                 // get disk parameter block address
        dc.l   _bios_8                 // LONG character_output_status(h)
        dc.l   _bios_9                 // media change?
        dc.l   _bios_a                 // what drives exist?
        dc.l   _bios_b                 // get/set alt-ctrl-shift status
        dc.l   _just_rts        // unused bios_c
        dc.l   _just_rts        // unused bios_d
        dc.l   _just_rts        // unused bios_e
        dc.l   _just_rts        // unused bios_f
        dc.l   _just_rts        // unused bios_10
        dc.l   _bios_11                 // get/set date/time (used by BDOS)
        
_bios_ent:               // Max. number of BIOS entries
        dc.w    (_bios_ent-_bios_vecs)/4
#endif

// ==== XBIOS entry points to allow individual module compilation ============

        .even
#if 0  /* moved in xbios.c */
_xbios_vecs:
        dc.l    _xbios_0
        dc.l    _xbios_1 
        dc.l    _xbios_2 
        dc.l    _xbios_3 
        dc.l    _xbios_4 
        dc.l    _xbios_5 
        dc.l    _xbios_6 
        dc.l    _xbios_7 
        dc.l    _xbios_8 
        dc.l    _xbios_9 
        dc.l    _xbios_a 
        dc.l    _xbios_b 
        dc.l    _xbios_c
        dc.l    _xbios_d
        dc.l    _xbios_e
        dc.l    _xbios_f
        dc.l    _xbios_10
        dc.l    _xbios_11
        dc.l    _xbios_12
        dc.l    _xbios_13
        dc.l    _xbios_14
        dc.l    _xbios_15
        dc.l    _xbios_16
        dc.l    _xbios_17
        dc.l    _xbios_18
        dc.l    _xbios_19
        dc.l    _xbios_1a
        dc.l    _xbios_1b
        dc.l    _xbios_1c
        dc.l    _xbios_1d
        dc.l    _xbios_1e
        dc.l    _xbios_1f
        dc.l    _xbios_20
        dc.l    _xbios_21
        dc.l    _xbios_22
        dc.l    _xbios_23
        dc.l    _xbios_24
        dc.l    _xbios_25
        dc.l    _xbios_26
        dc.l    _xbios_27
        dc.l    _just_rts       // 28
        dc.l    _xbios_29
        dc.l    _xbios_2a
        dc.l    _xbios_2b
        dc.l    _xbios_2c
        dc.l    _just_rts       // 2d
        dc.l    _xbios_2e
        dc.l    _just_rts       // 2f
        dc.l    _just_rts       // 30
        dc.l    _just_rts       // 31
        dc.l    _just_rts       // 32
        dc.l    _just_rts       // 33
        dc.l    _just_rts       // 34
        dc.l    _just_rts       // 35
        dc.l    _just_rts       // 36
        dc.l    _just_rts       // 37
        dc.l    _just_rts       // 39
        dc.l    _just_rts       // 3a
        dc.l    _just_rts       // 3b
        dc.l    _just_rts       // 3c
        dc.l    _just_rts       // 3d
        dc.l    _just_rts       // 3e
        dc.l    _just_rts       // 3f
        dc.l    _xbios_40

_xbios_ent:              // Max. number of XBIOS entries
        .dc.w   (_xbios_ent-_xbios_vecs)/4
#endif

        .bss
/* save area for trap13/14. */
save_beg:       ds.w    24*3    // d3-d7/a3-a7, sr, pc, vo = 24 words, 
                                // multiplied by 3 ("3 level deep reentrant")
_save_area:                     // End of Save storage



        .end
