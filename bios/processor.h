/*
 * processor.h - declarations for processor type check
 *
 * Copyright (c) 2001 EmuTOS development team.
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
 * processor for which EmuTOS is developped, is a kind of 68000 with 32-bit 
 * addresses.
 *
 * Details are:
 * - all addresses are considered to be 32 bit wide -- there is no 'clever'
 *   reusing of the upper 8 bits in e.g. exception vectors.
 * - the caches, especially the instruction cache are not handled. They are 
 *   disabled by default on power up or after reset on a 680x0, so there is 
 *   no trouble here -- However if we choose to enable them, then a function 
 *   invalidate_cache() must be implemented and called from within Pexec().
 * - move-from-sr is a priviledged instruction except on a plain 68000. 
 *   the priviledge exception handler in ???.S catches any user-mode call 
 *   to move-from-sr and executes a move-from-ccr instead.
 *   => potential issue: check if this behaviour might provoke undefined 
 *      behaviour if the instruction cache is enabled.
 * - stack frames: variable longframe indicates whether the exception stack
 *   frames are like on a plain 68000 or with an additional format word 
 *   telling the length of the frame.
 *   => potential issue: size of supervisor stack in higher processors
 * - floating point support: not handled.
 * - there is no particular handling of any other processor-specific details.
 *   To the best of our knowledge any beghaviour different from that of a plain
 *   68000 is disabled on power up or after reset on a 680x0, or are not likely
 *   to provoke any unwanted behaviour, so there is no issue here.
 *
 * The processor speed is undefined. Any code counting a particular number
 * of processor cycles for delaying should be rewritten to use a general-
 * purpose delay routine (based on processor cycle callibration ?)
 */

#ifndef H_PROCESSOR_
#define H_PROCESSOR_
 
long detect_cpu(void);
long detect_fpu(void);

extern void processor_init(void);
extern LONG mcpu;
extern LONG fputype;
extern WORD longframe;

#endif /* PROCESSOR_H */
  
