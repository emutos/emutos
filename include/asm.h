/*
 * newkbd.c - Intelligent keyboard routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * This file contains utility routines (macros) to 
 * manipulate special registers from C language.
 * 
 * available macros:
 *
 * WORD set_sr(WORD new); 
 *   sets sr to the new value, and return the old sr value 
 * 
 * For clarity, please add such two lines above when adding 
 * new macros below.
 */

/*
 * WORD set_sr(WORD new); 
 *   sets sr to the new value, and return the old sr value 
 */

#define set_sr(a)                           \
__extension__                               \
({register short retvalue __asm__("d0");    \
  short _a = (short)(a);                    \
  __asm__ volatile                          \
  ("                                        \
    move.w sr,d0;                           \
    move.w %1,sr "                          \
  : "=r"(retvalue)			/* outputs */	      \
	: "r"(_a)     /* inputs  */		            \
	: "d0"  /* clobbered regs */	            \
	);								                        \
	retvalue;							                    \
})

