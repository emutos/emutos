/*
 * detect.S - detect CPU and FPU type
 *
 * Copyright (c) 1999, 2001 by Authors:
 *
 *  ???  Jörg Westheide <joerg_westheide@su.maus.de>
 *  ???  Draco  
 *  LVL  Laurent Vogel
 *  
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

/*
 * LVL - This file was taken from FreeMiNT sys/arch/detect.S
 * the RCS id was: 1.2 2001/06/13 20:21:35 fna Exp 
 *
 * Changed:
 * - removed Detection of Harun Scheutzows ST-ESCC
 * - have it assemble using option -m68000
 * - address detection (nops for flushing the pipeline) (joy)
 * - 78 columns wide comments only
 *
 * below is the file. Must we keep the initial notice ?
 */
 
/* 
 * This file belongs to FreeMiNT. It's not in the original MiNT 1.12
 * distribution. See the file CHANGES for a detailed log of changes.
 * 
 * 
 * Author: Jörg Westheide <joerg_westheide@su.maus.de>
 * Started: 1999-06-11
 * 
 * please send suggestions or bug reports to me or
 * the MiNT mailing list
 * 
 * 
 * Hardware detection for MiNT.
 * This code _must_ be compiled as -m68030
 * 
 * Detection of Harun Scheutzows ST-ESCC (Joerg)
 * Detection of CPU and FPU model (Draco)
 * 
 */

	.global	_check_read_byte
	.global	_detect_cpu
	.global	_detect_fpu
	.xdef	_mcpu

	.text

// Verify access to the memory pointed to by the ptr
// int check_read_byte(long);
// returns 0 if reading the address caused a bus error
_check_read_byte:
	move.l	sp,a0
	move.l	(8).w,a1
	lea	berr.w(pc),a2
	move.l	a2,(8).w
	clr.l	d0
	nop					// flush pipeline

	move.l	4(sp),a2
	tst.b	(a2)
	nop					// flush pipeline

	moveq	#0x01,d0	// passed

berr:	move.l	a1,(8).w
	move.l	a0,sp
	nop
	rts


//
// CPU detection, experimental (draco@atari.org).
//
// Return value:
//
//  0 - 68000
// 10 - 68010
// 20 - 68020
// 30 - 68030
// 40 - 68040
// 60 - 68060
//


_detect_cpu:
	move.l	(0x10).w,a1	// save the illegal instr. exc. vector
	move.l	(0xf4).w,a2	// save the unimplemented instr. exc. vector
	lea	exit.w(PC),a0
	move.l	a0,(0x10).w	// set either vector	
	move.l	a0,(0xf4).w
	move.l	sp,a0		// save the ssp
	nop			// eventually flush pipelines

// on 68000 we try out a `move from CCR'.

	clr.l	d0              // assume 68000
	dc.w	0x42c0		// move.w ccr,d0 legal on 68010+
	moveq	#10,d0

// CACR is present only in 68020+

	dc.l	0x4e7a1002	// movec cacr,d1  get cache control register
	move.l	d1,d2		// hold a copy for later
	ori.w	#0x8100,d1	// enable '030 data and '040 instr. caches
	dc.l	0x4e7b1002	// movec d1,cacr  set new cache controls
	dc.l	0x4e7a1002	// movec cacr,d1  read it back to check 
	dc.l	0x4e7b2002	// movec d2,cacr  restore original CACR 
	tst.w	d1		// if 15th bit was set, this is a 68040+
	bmi.s	x040
	moveq	#20,d0		// assume 68020
	btst	#8,d1		// check if 68030 data cache was enabled
	beq.s	exit		// a zero here means no data cache, i.e. 68020
	moveq	#30,d0		// data cache enabled means 68030
	bra.s	exit

// 68040 or 68060. Quad word mulu test.

x040:	moveq	#60,d0		// assume 68060
	dc.l	0x4c102401	// mulu.l (a0),d1-d2
	moveq	#40,d0		// no fault -> this is 68040
exit:	move.l	a2,(0xf4).w	// restore stuff and exit
	move.l	a1,(0x10).w
	move.l	a0,sp
	nop			// flush pipelines
	rts

// FPU type detection, experimental (draco@atari.org).
//
// This can only detect the hardware FPU, any software emulation
// will be ignored.
//
// NOTICE: the _FPU cookie value for 68060 is not defined by Atari!
//         *** How does it look like on a Hades060? ***
//
// Return value is cookie value for _FPU slot or a zero if no FPU
// is present:
//
// 0x00000000, no FPU
// 0x00020000, 68881 or 68882
// 0x00040000, 68881 for sure
// 0x00060000, 68882 for sure
// 0x00080000, 68040 internal FPU
// 0x00100000, 68060 internal FPU
//
// The detection algorithm goes as follows:
//
// - FNOP is executed. If a Line-F exception follows, then there's no FPU
//   in coprocessor mode. If the CPU is >= 68020, no FPU is assumed.
// - if FNOP doesn't take an exception, an FPU attached in coprocessor mode
//   is present. Then if 68040 or 68060 CPU was detected previously, an 
//   appropriate FPU type is assumed. Otherwise the stackframe is checked 
//   for magic value indicating 68882 and if it is different, a 68881 is 
//   assumed.
//
// I am very interested if this will really work on everything =)
// On a 68030/68882 tandem it does.


_detect_fpu:
	move.l	sp,a0		// save the ssp
	clr.l	d0		// assume no FPU
	move.l	(0x2c).w,a1	// save the Line-F vector
	move.l	(0x08).w,a2
	move.l	#fexit,d1
	move.l	d1,(0x2c).w	// install temporary Line-F
	move.l	d1,(0x08).w
	nop			// flush pipelines

	move.l	_mcpu,d1	// check if 68000 or 68010
	cmpi.w	#20,d1
	bmi.s	sfp
	
	cmpi.w	#60,d1		// enable FPU on 68060 before the check
	bmi.s	no60

	dc.l	0x4e7a0808	// movec pcr,d0
	swap	d0
	cmp.w	#0x0431,d0	// "broken" 68LC/EC060
	beq.s	no60
	swap	d0
	bclr	#0x01,d0
	dc.l	0x4e7b0808	// movec d0,pcr

no60:	dc.l	0xf2800000	// fnop
	clr.l	-(sp)		// push NULL frame
	clr.l	-(sp)		// extra longs for 68060
	clr.l	-(sp)
	dc.w	0xf35f		// frestore (sp)+  reset FPU into NULL state
	dc.l	0xf2800000	// fnop  force it into IDLE state
	dc.w	0xf327		// fsave -(sp)  save the IDLE frame

	moveq	#0x10,d0	// assume 68060 FPU (cookie 0x00100000)
	cmpi.w	#60,d1		// d1 is loaded above the FPU code
	beq.s	fexit
	moveq	#0x08,d0	// if not 060, maybe 040 (cookie 0x00080000)
	cmpi.w	#40,d1
	beq.s	fexit
	moveq	#0x06,d0	// if neither, maybe a 68882 (0x00060000)
	move.b	(sp)+,d1		
	cmpi.b	#0x1f,d1
	beq.s	fexit
	moveq	#0x04,d0	// must be 68881
	bra.s	fexit
sfp:	tst.w	(0xfa40).w	// CIR
	moveq	#0x01,d0	// memory mapped FPU
fexit:	move.l	a1,(0x2c).w	// restore Line-F
	move.l	a2,(0x08).w
	move.l	a0,sp
	nop			// flush pipelines
	swap	d0
	rts


