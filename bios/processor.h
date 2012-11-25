/*
 * processor.h - declarations for processor type check
 *
 * Copyright (c) 2001-2012 EmuTOS development team.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Processor-related handling in EmuTOS
 * ------------------------------------
 * 
 * EmuTOS does not use or support advanced 680x0 features, except for long 
 * stack frames starting from the 68010. In fact one might imagine that the 
 * processor for which EmuTOS is developed is a kind of 68000 with 32-bit 
 * addresses.
 *
 * Exception:
 * If a 68030 or 68040 is detected, the instruction cache is enabled.  See
 * below for more details.
 *
 * Details
 * -------
 * - all addresses are considered to be 32 bit wide -- there is no 'clever'
 *   reusing of the upper 8 bits in e.g. exception vectors.
 * - move-from-sr is a privileged instruction except on a plain 68000. 
 *   the privilege exception handler in vectors.S catches any user-mode 
 *   call to move-from-sr and replaces it with a move-from-ccr instead.
 * - move-from-ccr is an illegal instruction on a plain 68000. the 
 *   illegal instruction handler in vectors.S catches any illegal call to
 *   move-from-ccr and replaces it with a move-from-sr instead.
 * - stack frames: variable longframe indicates whether the exception stack
 *   frames are like on a plain 68000 or with an additional format word 
 *   telling the length of the frame.
 * - floating point support: not handled.
 * - data cache: disabled.
 * - instruction cache: on a 68030 or 68040, the instruction cache is enabled.
 *   the instruction cache is invalidated in two circumstances: after pexec()
 *   has loaded code in memory, and after exception handlers have changed
 *   code (move from SR/CCR).
 * - there is no particular handling of any other processor-specific details.
 *
 *   To the best of our knowledge any behaviour different from that of a plain
 *   68000 is disabled on power up or after reset on a 680x0, or is not likely
 *   to provoke any unwanted behaviour, so there is no issue here.
 *
 * The processor speed is undefined. Any code counting a particular number
 * of processor cycles for delaying should be rewritten to use a general-
 * purpose delay routine (based on processor cycle calibration ?)
 */

#ifndef PROCESSOR_H
#define PROCESSOR_H
 
long detect_cpu(void);
long detect_fpu(void);

extern void processor_init(void);
extern void invalidate_icache(void *start, long size);
extern LONG mcpu;
extern LONG fputype;
extern WORD longframe;

#endif /* PROCESSOR_H */
