/*
 * processor.S - Detect and set CPU and FPU type
 *
 * Copyright (c) 1999, 2002 by Authors:
 *
 *      Jörg Westheide <joerg_westheide@su.maus.de>
 *      Draco  
 * LVL  Laurent Vogel
 * MAD  Martin Doering
 *      Norman Feske
 *  
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */


#include "asmdefs.h"



/* References */

        .globl  _processor_init
        .globl  _invalidate_instruction_cache
        .globl  _flush_data_cache
        .globl  _invalidate_data_cache
        .globl  _mcpu
        .globl  _mcpu_subtype
        .globl  _fputype
        .globl  _detect_cpu
        .globl  _detect_fpu

        .extern _longframe              // If not 0, use long stack frames
        .extern _setup_68030_pmmu
        .extern _setup_68040_pmmu



        .text

#ifndef __mcoldfire__

/*
 * _detect_cpu - CPU detection
 *
 * Returns a long word indicating the type of CPU.  The low-order word
 * contains the generic type, and the high-order word indicates the
 * subtype.  Current returned values are as follows:
 *   low-order    high-order    CPU type
 *       0             0        68000
 *      10             0        68010
 *      20             0        68020 
 *      30             0        68030
 *      30             1        68ec030
 *      40             0        68040
 *      60             0        68060
 */

_detect_cpu:
        movem.l  d2/a2-a3,-(sp)

// intercept possible exceptions

        move.l  (0x10).w,a1     // save exception vectors: illegal instruction
        move.l  (0x2c).w,a2     //   line F
        move.l  (0xf4).w,a3     //     unimplemented instruction
        lea     exit.w(PC),a0
        move.l  a0,(0x10).w     // replace vectors
        move.l  a0,(0x2c).w
        move.l  a0,(0xf4).w
        move.l  sp,a0           // save the ssp
        nop                     // eventually flush pipelines

// on 68000 we try out a `move from CCR'.

        clr.l   d0              // assume 68000
        dc.w    0x42c0          // move.w ccr,d0 legal on 68010+
        moveq   #10,d0

// CACR is present only in 68020+

        dc.l    0x4e7a1002      // movec cacr,d1  get cache control register
        move.l  d1,d2           // hold a copy for later
        ori.w   #0x8100,d1      // enable '030 data and '040 instr. caches
        dc.l    0x4e7b1002      // movec d1,cacr  set new cache controls
        dc.l    0x4e7a1002      // movec cacr,d1  read it back to check 
        dc.l    0x4e7b2002      // movec d2,cacr  restore original CACR 
        tst.w   d1              // if 15th bit was set, this is a 68040+
        bmi.s   x040
        moveq   #20,d0          // assume 68020
        btst    #8,d1           // check if 68030 data cache was enabled
        beq.s   exit            // a zero here means no data cache, i.e. 68020
        move.l  #0x0001001e,d0  // data cache enabled means 68030 - assume 68ec030
        PMOVE_FROM_TC(temp)     // try to access the TC register
        moveq   #30,d0          // no fault -> this is a 68030
        bra.s   exit

// 68040 or 68060

x040:   moveq   #40,d0          // assume 68040
        dc.l    0x4e7a1808      // movec pcr,d1
        moveq   #60,d0          // no fault -> this is 68060
exit:  
        move.l  a3,(0xf4).w     // restore stuff and exit
        move.l  a2,(0x2c).w
        move.l  a1,(0x10).w
        move.l  a0,sp
        nop                     // flush pipelines
        movem.l  (sp)+,d2/a2-a3
        rts



/*
 * _detect_fpu - FPU type detection, experimental (draco@atari.org).
 *
 * This can only detect the hardware FPU, any software emulation
 * will be ignored.
 *
 * NOTICE: the _FPU cookie value for 68060 is not defined by Atari!
 *         *** How does it look like on a Hades060? ***
 *
 * Return value is cookie value for _FPU slot or a zero if no FPU
 * is present:
 *
 * 0x00000000, no FPU
 * 0x00020000, 68881 or 68882
 * 0x00040000, 68881 for sure
 * 0x00060000, 68882 for sure
 * 0x00080000, 68040 internal FPU
 * 0x00100000, 68060 internal FPU
 *
 * The detection algorithm goes as follows:
 *
 * - FNOP is executed. If a Line-F exception follows, then there's no FPU
 *   in coprocessor mode. If the CPU is >= 68020, no FPU is assumed.
 * - if FNOP doesn't take an exception, an FPU attached in coprocessor mode
 *   is present. Then if 68040 or 68060 CPU was detected previously, an 
 *   appropriate FPU type is assumed. Otherwise the stackframe is checked 
 *   for magic value indicating 68882 and if it is different, a 68881 is 
 *   assumed.
 *
 * I am very interested if this will really work on everything =)
 * On a 68030/68882 tandem it does.
 */

_detect_fpu:
        move.l  a2,-(sp)
        move.l  sp,a0           // save the ssp
        clr.l   d0              // assume no FPU
        move.l  (0x2c).w,a1     // save the Line-F vector
        move.l  (0x08).w,a2
        move.l  #fexit,d1
        move.l  d1,(0x2c).w     // install temporary Line-F
        move.l  d1,(0x08).w
        nop                     // flush pipelines

        move.l  _mcpu,d1        // check if 68000 or 68010
        cmpi.w  #20,d1
#if CONF_WITH_SFP004
        bmi.s   sfp
#else
        bmi.s   fexit
#endif
        
        cmpi.w  #60,d1          // enable FPU on 68060 before the check
        bmi.s   no60

        dc.l    0x4e7a0808      // movec pcr,d0
        swap    d0
        cmp.w   #0x0431,d0      // "broken" 68LC/EC060
        beq.s   no60
        swap    d0
        bclr    #0x01,d0
        dc.l    0x4e7b0808      // movec d0,pcr

no60:   dc.l    0xf2800000      // fnop
        clr.l   -(sp)           // push NULL frame
        clr.l   -(sp)           // extra longs for 68060
        clr.l   -(sp)
        dc.w    0xf35f          // frestore (sp)+  reset FPU into NULL state
        dc.l    0xf2800000      // fnop  force it into IDLE state
        dc.w    0xf327          // fsave -(sp)  save the IDLE frame

        moveq   #0x10,d0        // assume 68060 FPU (cookie 0x00100000)
        cmpi.w  #60,d1          // d1 is loaded above the FPU code
        beq.s   fexit
        moveq   #0x08,d0        // if not 060, maybe 040 (cookie 0x00080000)
        cmpi.w  #40,d1
        beq.s   fexit
        moveq   #0x06,d0        // if neither, maybe a 68882 (0x00060000)
        move.b  (sp)+,d1                
        cmpi.b  #0x1f,d1
        beq.s   fexit
        moveq   #0x04,d0        // must be 68881

#if CONF_WITH_SFP004
        bra.s   fexit
sfp:    tst.w   0xfffffa40.w    // CIR
        moveq   #0x01,d0        // memory mapped FPU
#endif

fexit:  move.l  a1,(0x2c).w     // restore Line-F
        move.l  a2,(0x08).w
        move.l  a0,sp
        nop                     // flush pipelines
        swap    d0
        move.l  (sp)+,a2
        rts

#endif /* __mcoldfire__ */



/*
 * void processor_init(void) - sets mcpu and fputype.
 */

_processor_init:
#ifdef __mcoldfire__
        // TODO enable the caches
#else
        jsr     _detect_cpu
        swap	d0
        move.w	d0,_mcpu_subtype    // save processor subtype
        clr.w	d0
        swap    d0
        move.l  d0,_mcpu            // & type
        beq     m68000


        move.w  #1,_longframe   // this is a 68010 or later
m68000:
        jsr     _detect_fpu
        move.l  d0,_fputype

        cmpi.b  #30,_mcpu+3
        bne.s   pi_chk040

// We're running on a 68030 or a 68ec030

#if CONF_WITH_68030_PMMU

// User would like a PMMU tree, so see if it's possible

        tst.w   _mcpu_subtype   // check for full 68030
        beq.s   m68030

// On a 68ec030, we enable just the instruction cache, since we can't
// set up a proper PMMU tree to avoid caching the I/O address area

        move.l  #0x00000011,d0  // set IBE and EI only
        bra.s   init_cacr

// On a full 68030, if a PMMU tree is wanted, we must set it up before
// enabling the data cache.  The PMMU tree will ensure that caching is
// inhibited for access to i/o addresses.

m68030:
        jsr     _setup_68030_pmmu
        PMOVE_TO_CRP(root_pointer_descriptor)   // tell system where tree is
        PMOVE_TO_TC(init_tc)        // enable PMMU
        PMOVE_TO_TTR0(init_ttr0)    // override PMMU tree for accesses to
        PMOVE_TO_TTR1(init_ttr1)    //   0x01000000-0xfeffffff (see below for details)
        move.l  #0x00003111,d0  // we set WA, DBE, ED, IBE and EI
#else

// A PMMU tree isn't wanted, so we avoid caching problems by only
// enabling the instruction cache.

        move.l  #0x00000011,d0  // we set IBE and EI only
#endif

init_cacr:
        MOVEC_D0_CACR
        bra.s   pi_done

pi_chk040:
        cmp.b   #40,_mcpu+3
        bne.s   pi_done
        
#if CONF_WITH_68040_PMMU
        jsr     setup_68040_pmmu
#else
// On a 68040 only, clear the instruction cache then activate it;
// the data cache remains disabled.
        nop
        dc.w    0xf498          // cinva   ic
        nop
        move.l  #0x8000,d0
        dc.l    0x4e7b0002      // movec   d0,cacr
        
        // Instruction Transparent Translation Registers (ITTRs):
        // all addresses (bits 23-16), enabled (bit 15), both user and
        // supervisor (bits 14-13), cachable+write-through (bits 6-5)
        move.l  #0x00ffe000,d0
        dc.l    0x4e7b0004      // movec   d0,ittr0
        moveq.l #0,d0
        dc.l    0x4e7b0005      // movec   d0,ittr1
        
        // The Transparent Translation Registers are setup (even if the cache
        // and the MMU are disabled) to ensure serialized access ("Without 
        // serialization, the IU pipeline allows read accesses to occur 
        // before completion of a write-back for a previous instruction.")
        move.l  #0x00ffe040,d0
        dc.l    0x4e7b0006      // movec   d0,dttr0
        moveq.l #0,d0
        dc.l    0x4e7b0007      // movec   d0,dttr1
#endif /* CONF_WITH_68040_PMMU */

#endif /* __mcoldfire__ */

pi_done:
        rts

/*
 * void invalidate_instruction_cache(void *start, long length)
 * First, the data cache is flushed to push changes into the RAM.
 * Then the instruction cache is invalidated for the specified zone.
 *
 * We're lazy here and invalidate all the cache. A real implementation
 * would invalidate only the needed pages using several cinvp ic,(a0). 
 * It is not worth the trouble for EmuTOS right now.
 */

_invalidate_instruction_cache:
#ifdef __mcoldfire__
        lea     cpushl_bc,a1    // flush/invalidate both caches
        bra.s   cpushl_loop
#else
        cmpi.b  #30,_mcpu+3
        bne.s   ii_not30
        MOVEC_CACR_D0                   // get current cacr
        ori.b   #0x08,d0                // set the CI bit
        MOVEC_D0_CACR                   // clear the whole i-cache
        bra.s   ii_done

ii_not30:
        cmp.b   #40,_mcpu+3
        bne     ii_done
        nop
        dc.w    0xf498          // cinva   ic
        nop
ii_done:
        rts
#endif

/*
 * void flush_data_cache(void *start, long length)
 *
 * flush data cache before writing data with DMA
 *
 * the actions required depend on the mode of data cache:
 *   write-through:
 *     no action is necessary
 *   copyback:
 *     we must push the data cache (the backing memory may be stale)
 */

_flush_data_cache:
#ifdef __mcoldfire__
        lea     cpushl_dc,a1    // flush/invalidate data cache
        bra.s   cpushl_loop
#else
//
// 68030 data caches are always write-through, so no action is necessary
//
// 68040 data caches are either write-through or copyback, depending on
// how the system is set up.  at this time, the data TTRs are set up so
// that the cache would be write-through if enabled; but in any case it
// is disabled.  so no action is necessary.
//
        rts
#endif


/*
 * void invalidate_data_cache(void *start, long length)
 *
 * invalidate data cache after data has been read with DMA
 *
 * for both modes of data cache (write_through and copyback),
 * the cache needs to be invalidated
 */
_invalidate_data_cache:
#ifdef __mcoldfire__
        lea     cpushl_dc,a1    // flush/invalidate data cache
        // drop into cpushl_loop
#else
        cmpi.b  #30,_mcpu+3
        bne.s   ic_not30
//
// for a 68030, we invalidate both instruction and data caches, just
// like TOS4.  this allows backward compatibility in case some foolish
// person reads code from an i/o device and branches to it directly;
// this would have been legal on STs and STes.  since the caches are
// relatively small on a 68030, this should not have a large impact on
// performance.
//
        MOVEC_CACR_D0                   // get current cacr
        ori.w   #0x0808,d0              // set CD & ID bits
        MOVEC_D0_CACR                   // clear both caches
        //bra.s   ic_done
ic_not30:
        //cmpi.b  #40,_mcpu+3
        //bne.s   ic_done
//
// at this time, the data cache is disabled on 68040, so we have
// nothing to do.  if the data cache is enabled in the future, we
// should issue a "cinva dc" instead.  for performance reasons, we
// do not intend to invalidate the instruction cache in this case.
//

ic_done:
        rts
#endif

#ifdef __mcoldfire__
cpushl_loop:
        // This helper routine is a loop around cpushl ?c,(a0)
        // a1 must point to the actual instruction called in the loop
        
        // This code comes from the MCF547x Reference Manual
        // Section 7.11 Cache Management
        //
        // The ColdFire has no cinva instruction.
        // Instead, cpushl writes the modified cache data to the RAM
        // then invalidates the caches (data + instruction) except if
        // the DDPI and IDPI bits have been set in the CACR.
        //
        // The ColdFire V4e core has a 32 kB instruction cache
        // and a 32 kB data cache. Both caches have the same structure.
        // The data is stored in "Line" elements of 16 bytes.
        // The Lines are stored in a 2D array of 4 Ways * 512 Sets.
        //
        // The following changes have been made to the original code:
        // - call jsr (a1) instead of "cpushl dc"
        // - flush the 512 Sets (original code forgot the last one)
        
        nop                     // synchronize/flush store buffer
        moveq.l #0,d0           // initialize way counter
        moveq.l #0,d1           // initialize set counter
        move.l  d0,a0           // initialize cpushl pointer

setloop:
        jsr     (a1)            // call appropriate cpushl instruction
        lea     0x0010(a0),a0   // increment set index by 1
        addq.l  #1,d1           // increment set counter
        cmpi.l  #512,d1         // are sets for this way done?
        bne.s   setloop

        moveq.l #0,d1           // set counter to zero again
        addq.l  #1,d0           // increment to next way
        move.l  d0,a0           // set = 0, way = d0
        cmpi.l  #4,d0           // flushed all the ways?
        bne.s   setloop

        rts

// Helper routines for cpushl_loop
        
cpushl_ic:
        cpushl  ic,(a0)         // instruction cache
        rts
        
cpushl_dc:
        cpushl  dc,(a0)         // data cache
        rts
        
cpushl_bc:
        cpushl  bc,(a0)         // both caches
        rts
        
#endif

        SECTION_RODATA

#if CONF_WITH_68030_PMMU
root_pointer_descriptor:
        dc.l    0x80000002,_pmmutree // lower limit 0, short format, table at '_pmmutree'
init_tc:
        dc.l    0x80F04445          // enable, 32K pages, table indexes=4/4/4/5
init_ttr0:                          // transparent translation is enabled for all FCs
        dc.l    0x017E8107          //   for 0x01000000-0x7fffffff, caching allowed
init_ttr1:                          // transparent translation is enabled for all FCs
        dc.l    0x807E8507          //   for 0x80000000-0xfeffffff, *caching inhibited*
#endif

// ==== Variables ============================================================
 
        .bss
        .even

#ifndef __mcoldfire__
_mcpu:          ds.l    1
_mcpu_subtype:  ds.w    1
_fputype:       ds.l    1
temp:           ds.l    1
#endif


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end




