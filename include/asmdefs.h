/*
 * asmdefs.h - definitions to include in top of all assembler files
 *
 * Copyright (c) 2001-2013 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* this symbol will be defined in all ASM source.
 * use it to filter out definitions that would generate syntax
 * errors in asm.
 */
#define ASM_SOURCE

/* general-purpose configuration file */
#include "config.h"

/* generate constants for opcodes that are 68010 and above */
#define MOVEC_CACR_D0       dc.l 0x4e7a0002         /* 68020-68060 */

#define MOVEC_D0_CACR       dc.l 0x4e7b0002         /* 68020-68060 */
#define MOVEC_D0_VBR        dc.l 0x4e7b0801         /* 68010-68060 */
#define MOVEC_D0_CAAR       dc.l 0x4e7b0802         /* 68020-68030 */

#define MOVEC_D0_TC         dc.l 0x4e7b0003         /* 68040-68060 (except 68ec040) */
#define MOVEC_D0_ITT0       dc.l 0x4e7b0004         /* 68040-68060 */
#define MOVEC_D0_ITT1       dc.l 0x4e7b0005         /* 68040-68060 */
#define MOVEC_D0_DTT0       dc.l 0x4e7b0006         /* 68040-68060 */
#define MOVEC_D0_DTT1       dc.l 0x4e7b0007         /* 68040-68060 */

#define PMOVE_FROM_TC(addr) dc.l 0xf0394200,addr    /* 68030 (except 68ec030) */

#define PMOVE_TO_TC(addr)   dc.l 0xf0394000,addr    /* 68030 (except 68ec030) */
#define PMOVE_TO_CRP(addr)  dc.l 0xf0394c00,addr    /* 68030 (except 68ec030) */
#define PMOVE_TO_TTR0(addr) dc.l 0xf0390800,addr    /* 68030 */
#define PMOVE_TO_TTR1(addr) dc.l 0xf0390c00,addr    /* 68030 */

/* ELF toolchain support */
#ifdef ELF_TOOLCHAIN
#define DEFINE_FUNCTION_ALIAS(alias, impl) .globl alias; alias:; jmp impl
#define ELF_LIB_REF(f) DEFINE_FUNCTION_ALIAS(_##f, f)
#endif

/* Read-only data section */
#ifdef ELF_TOOLCHAIN
/* ELF objects have real support for .rodata */
#define SECTION_RODATA .section .rodata
#else
/* a.out objects have no .rodata section, default to .text */
#define SECTION_RODATA .text
#endif
