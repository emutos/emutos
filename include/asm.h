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
 * WORD get_sr(void);
 *   returns the current value of sr. the CCR bits are not meaningful.
 * void stop2300(void);
 * void stop2500(void);
 *   the STOP immediate instruction
 *
 * For clarity, please add such two lines above when adding 
 * new macros below.
 */

#ifndef _ASM_H
#define _ASM_H

/*
 * WORD set_sr(WORD new); 
 *   sets sr to the new value, and return the old sr value 
 */

#define set_sr(a)                           \
__extension__                               \
({register short retvalue __asm__("d0");    \
  short _a = (short)(a);                    \
  __asm__ volatile                          \
  ("move.w sr,d0;                           \
    move.w %1,sr "                          \
  : "=r"(retvalue)   /* outputs */          \
  : "d"(_a)          /* inputs  */          \
  : "d0"             /* clobbered regs */   \
  );                                        \
  retvalue;                                 \
})

/*
 * WORD get_sr(void); 
 *   returns the current value of sr. 
 */

#define get_sr()                            \
__extension__                               \
({register short retvalue __asm__("d0");    \
  __asm__ volatile                          \
  ("move.w sr,d0 "                          \
  : "=r"(retvalue)   /* outputs */          \
  :                  /* inputs  */          \
  : "d0"             /* clobbered regs */   \
  );                                        \
  retvalue;                                 \
})

/*
 * void stop2x00(void)
 *   the m68k STOP immediate instruction
 */

#define stop2300()                              \
__extension__                                   \
({__asm__ volatile                              \
  ("stop #0x2300 ");                            \
})

#define stop2500()                              \
__extension__                                   \
({__asm__ volatile                              \
  ("stop #0x2500 ");                            \
})

#endif /* _ASM_H */
