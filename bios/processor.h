/*
 * processor.h - declarations for processor type check
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Processor-related handling in EmuTOS
 * ====================================
 * EmuTOS may be compiled for ColdFire or non-ColdFire processors.
 *
 *
 * ColdFire processors
 * ===================
 * If EmuTOS is compiled for ColdFire, it assumes a V4e core.
 *
 * Details
 * -------
 * 1. Data cache is enabled (by the preboot).  It is flushed/invalidated
 *    in the following situations:
 *      . before DMA writes to ensure that RAM isn't stale
 *      . after DMA reads to avoid stale cache data
 *      . in Pexec() when a program is loaded.
 * 2. Instruction cache is enabled (by the preboot).  It is flushed/
 *    invalidated in Pexec() after a program is loaded.
 * 3. The processor speed must be defined in SDCLK_FREQUENCY_MHZ.  This
 *    is used to derive a value that is used for timing short delays via
 *    a small instruction-looping routine.  See delay.c.
 *
 *
 * Non-ColdFire processors
 * =======================
 * If EmuTOS is not compiled for ColdFire, it detects the following
 * CPU types:
 *  . Motorola 68000, 68010, 68020, 68030, 68ec030, 68040, 68060
 * and the following FPU types:
 *  . Motorola 68881, 68882, internal 68040, internal 68060
 *
 * If a 68030 is detected, it is configured by default with a PMMU
 * table that is TT- and Falcon-compatible.  In this case, both data
 * and instruction caches are enabled.  For 68ec030s (or if a PMMU
 * table is not configured), just the instruction cache is enabled.

 * If a 68040 is detected, it is configured by default _without_ a
 * PMMU table, and just the instruction cache is enabled.  A PMMU
 * table may optionally be configured, in which case the data cache
 * is also enabled (write-through for ST-RAM, copyback for TT-RAM).
 *
 * For other 680x0 processors, no advanced features are used.
 *
 * Details
 * -------
 * 1. All addresses are considered to be 32 bits wide: there is no
 *    'clever' reuse of the upper 8 bits in e.g. exception vectors.
 * 2. move-from-sr is a privileged instruction except on a plain 68000.
 *    The privilege exception handler in vectors.S catches any user-mode
 *    call to move-from-sr and replaces it with a move-from-ccr instead.
 *    On a 68030/68040, the relevant instruction cache line is
 *    invalidated.
 * 3. move-from-ccr is an illegal instruction on a plain 68000. The
 *    illegal instruction handler in vectors.S catches any illegal call
 *    to move-from-ccr and replaces it with a move-from-sr instead.
 * 4. Stack frames: as with TOS3/TOS4, the system variable 'longframe'
 *    indicates whether the exception stack frames are like those on a
 *    plain 68000, or have an additional format word.
 * 5. Data cache: if enabled, it is invalidated after DMA to avoid stale
 *    data.
 * 6. Instruction cache: if enabled, it is invalidated in Pexec() after
 *    a program is loaded. For TOS compatibility, it is also invalidated
 *    after Rwabs() or DMAread().
 * 7. The processor speed is undefined. The routine calibrate_delay() is
 *    used to derive a value that is used for timing short delays via a
 *    small instruction-looping routine.  See delay.c.
 *
 * WARNING: if you specify CONF_WITH_68040_PMMU, and you run EmuTOS on
 * a machine with a real 68040, and you do DMA into TT-RAM (e.g. SCSI),
 * there will be major problems, since TT-RAM is set up in copyback mode,
 * but flush_data_cache() does not do a flush in this case.
 */

#ifndef PROCESSOR_H
#define PROCESSOR_H

extern void processor_init(void);
/* invalidate_instruction_cache() is declared in include/biosext.h */
extern void instruction_cache_kludge(void *start,long size);
extern void flush_data_cache(void *start, long size);
extern void invalidate_data_cache(void *start, long size);
extern LONG mcpu;
extern LONG fputype;
extern WORD longframe;

#if CONF_WITH_APOLLO_68080
extern BOOL is_apollo_68080;
#endif

#endif /* PROCESSOR_H */
