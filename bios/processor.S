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

        .global _processor_init
        .global _invalidate_icache
        .global _mcpu
        .global _fputype
        .global _detect_cpu
        .global _detect_fpu

        .xdef   _longframe              // If not 0, use long stack frames
        .xdef   _mcpu



        .text

/*
 * _detect_cpu - CPU detection
 *
 * Return values:
 *
 *  0 - 68000
 *  5 - coldfire
 * 10 - 68010
 * 20 - 68020
 * 30 - 68030
 * 40 - 68040
 * 60 - 68060
 */

_detect_cpu:

//
// The instruction "move.l %macsr,d1" is Coldfire-specific
// and raises a Line-A exception on a plain m68k. We just
// execute this instruction and check if such an exception
// was raised.
//
	move.l #0x28.w,a0         // keep old Line-A exception vector
	move.l #tmp_line_a,d0
	move.l %d0,0x28.w          // install custom Line-A handler
	moveq  #5,d0              // set default return value (Coldfire present)
	dc.w   0xa981              // move.l %macsr,%d1
	move.l a0,0x28.w          // restore original Line-A handler
	rts

tmp_line_a:
	addq.l #2,2(sp)           // skip Line-A instruction
	move.l a0,0x28.w          // restore original Line-A handler
        bsr    check68k         // no coldfire, gon on with 68k check
	rte


check68k:
        move.l  a2,-(sp)  
        move.l  (0x10).w,a1     // save the illegal instr. exc. vector
        move.l  (0xf4).w,a2     // save the unimplemented instr. exc. vector
//	move.l  (0x28).w,a3     // save old Line-A exception vector
        lea     exit.w(PC),a0
        move.l  a0,(0x10).w     // set either vector    
        move.l  a0,(0xf4).w
//        move.l  a0,(0x28).w     // linea-vector for coldfire test
        move.l  sp,a0           // save the ssp
        nop                     // eventually flush pipelines

// on coldfire we try out a 'move.l %macsr,d1', exception on 68k

//	moveq  #100,d0          // set return value (Coldfire present)
//	dc.w   0xa981           // move.l %macsr,%d1

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
        moveq   #30,d0          // data cache enabled means 68030
        bra.s   exit

// 68040 or 68060. Quad word mulu test.

x040:   moveq   #60,d0          // assume 68060
        dc.l    0x4c102401      // mulu.l (a0),d1-d2
        moveq   #40,d0          // no fault -> this is 68040
exit:  
        move.l  a2,(0xf4).w     // restore stuff and exit
        move.l  a1,(0x10).w
//	move.l  a3,(0x28).w     // restore original Line-A handler
        move.l  a0,sp
        nop                     // flush pipelines
        move.l  (sp)+,a2
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
        bmi.s   sfp
        
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
        bra.s   fexit
sfp:    tst.w   (0xfa40).w      // CIR
        moveq   #0x01,d0        // memory mapped FPU
fexit:  move.l  a1,(0x2c).w     // restore Line-F
        move.l  a2,(0x08).w
        move.l  a0,sp
        nop                     // flush pipelines
        swap    d0
        move.l  (sp)+,a2
        rts



/*
 * void processor_init(void) - sets mcpu and fputype.
 */

_processor_init:
        jsr     _detect_cpu
        move.l  d0,_mcpu
        beq     m68000


        move.w  #1,_longframe   // this is a 68010 or later
m68000:
        jsr     _detect_fpu
        move.l  d0,_fputype

        cmp.b   #40,_mcpu+3
        bne     done
        
// On a 68040 only, clear the instruction cache then activate it;
// the data cache remains disabled.
        nop
        dc.w    0xf498          // cinva   ic
        nop
        move.l  #0x8000,d0
        dc.l    0x4e7b0002      // movec   d0,cacr
done:
        rts

/*
 * void invalidate_icache(void *start, long length)
 * invalidate the instruction cache for addresses in the specified zone.
 *
 * We're lazy here and invalidate all the cache. A real implementation
 * would invalidate only the needed pages using several cinvp ic,(a0). 
 * It is not worth the trouble for EmuTOS right now.
 */

_invalidate_icache:
        cmp.b   #40,_mcpu+3
        bne     done
        nop
        dc.w    0xf498          // cinva   ic
        nop
        rts



// ==== Variables ============================================================
 
        .bss
        .even

_mcpu:          ds.l    1
_fputype:       ds.l    1


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end




