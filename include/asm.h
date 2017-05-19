/*
 * asm.h - Assembler help routines
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * This file contains utility routines (macros) to
 * perform functions not directly available from C.
 *
 * available macros:
 *
 * WORD set_sr(WORD new);
 *   sets sr to the new value, and return the old sr value
 * WORD get_sr(void);
 *   returns the current value of sr. the CCR bits are not meaningful.
 * void regsafe_call(void *addr);
 *   Do a subroutine call with saving/restoring the CPU registers
 * void delay_loop(ULONG loopcount);
 *   Loops for the specified loopcount.
 *
 * For clarity, please add such two lines above when adding
 * new macros below.
 */

#ifndef ASM_H
#define ASM_H

/*
 * values of 'mode' for Pexec()
 *
 * these were moved here because of the definition of trap1_pexec() below
 */
#define PE_LOADGO     0
#define PE_LOAD       3
#define PE_GO         4
#define PE_BASEPAGE   5
#define PE_GOTHENFREE 6
#define PE_BASEPAGEFLAGS 7
#define PE_RELOCATE   50    /* required for NatFeats support only, not in Atari TOS */

/* OS entry points implemented in util/miscasm.S */
extern long trap1(int, ...);
extern long trap1_pexec(short mode, const char * path,
  const void * tail, const char * env);

/* Wrapper around the STOP instruction. This preserves SR. */
extern void stop_until_interrupt(void);

/*
 * Push/Pop registers from stack, with ColdFire support.
 * This is intended to be used inside inline assembly.
 * Borrowed from MiNTLib:
 * https://github.com/freemint/mintlib/blob/master/include/compiler.h
 */
#ifdef __mcoldfire__

#define PUSH_SP(regs,size)              \
    "lea     -" #size "(sp),sp\n\t"     \
    "movem.l " regs ",(sp)\n\t"

#define POP_SP(regs,size)               \
    "movem.l (sp)," regs "\n\t"         \
    "lea     " #size "(sp),sp\n\t"

#else

#define PUSH_SP(regs,size)              \
    "movem.l " regs ",-(sp)\n\t"

#define POP_SP(regs,size)               \
    "movem.l (sp)+," regs "\n\t"

#endif

/*
 * WORD swpw(WORD val);
 *   swap endianess of val, 16 bits only.
 */

#ifdef __mcoldfire__
#define swpw(a)                           \
  __extension__                           \
  ({long _tmp;                            \
    __asm__ volatile                      \
    ("move.w  %0,%1\n\t"                  \
     "lsl.l   #8,%0\n\t"                  \
     "lsr.l   #8,%1\n\t"                  \
     "move.b  %1,%0"                      \
    : "=d"(a), "=d"(_tmp) /* outputs */   \
    : "0"(a)     /* inputs  */            \
    : "cc"       /* clobbered */          \
    );                                    \
  })
#else
#define swpw(a)                           \
  __asm__ volatile                        \
  ("ror   #8,%0"                          \
  : "=d"(a)          /* outputs */        \
  : "0"(a)           /* inputs  */        \
  : "cc"             /* clobbered */      \
  )
#endif

/* Copy and swap an UWORD from *src to *dest */
static __inline__ void swpcopyw(const UWORD* src, UWORD* dest)
{
    const UBYTE_ALIAS* s = (const UBYTE_ALIAS*)src;
    UBYTE_ALIAS* d = (UBYTE_ALIAS*)dest;

    d[0] = s[1];
    d[1] = s[0];
}

/*
 * WORD swpl(LONG val);
 *   swap endianess of val, 32 bits only.
 *   e.g. ABCD => DCBA
 */

#ifdef __mcoldfire__
#define swpl(a)                           \
  __extension__                           \
  ({long _tmp;                            \
    __asm__ volatile                      \
    ("move.b  (%1),%0\n\t"                \
     "move.b  3(%1),(%1)\n\t"             \
     "move.b  %0,3(%1)\n\t"               \
     "move.b  1(%1),%0\n\t"               \
     "move.b  2(%1),1(%1)\n\t"            \
     "move.b  %0,2(%1)"                   \
    : "=d"(_tmp)      /* outputs */       \
    : "a"(&a)        /* inputs  */        \
    : "cc", "memory" /* clobbered */      \
    );                                    \
  })
#else
#define swpl(a)                           \
  __asm__ volatile                        \
  ("ror   #8,%0\n\t"                      \
   "swap  %0\n\t"                         \
   "ror   #8,%0"                          \
  : "=d"(a)          /* outputs */        \
  : "0"(a)           /* inputs  */        \
  : "cc"             /* clobbered */      \
  )
#endif


/*
 * WORD swpw2(ULONG val);
 *   swap endianness of val, treated as two 16-bit words.
 *   e.g. ABCD => BADC
 */

#ifdef __mcoldfire__
#define swpw2(a)                          \
  __extension__                           \
  ({unsigned long _tmp;                   \
    __asm__ volatile                      \
    ("move.b  (%1),%0\n\t"                \
     "move.b  1(%1),(%1)\n\t"             \
     "move.b  %0,1(%1)\n\t"               \
     "move.b  2(%1),%0\n\t"               \
     "move.b  3(%1),2(%1)\n\t"            \
     "move.b  %0,3(%1)"                   \
    : "=d"(_tmp)     /* outputs */        \
    : "a"(&a)        /* inputs  */        \
    : "cc", "memory" /* clobbered */      \
    );                                    \
  })
#else
#define swpw2(a)                          \
  __asm__ volatile                        \
  ("ror   #8,%0\n\t"                      \
   "swap  %0\n\t"                         \
   "ror   #8,%0\n\t"                      \
   "swap  %0"                             \
  : "=d"(a)          /* outputs */        \
  : "0"(a)           /* inputs  */        \
  : "cc"             /* clobbered */      \
  )
#endif


/*
 * Warning: The following macros use "memory" in the clobber list,
 * even if the memory is not modified. On ColdFire, this is necessary
 * to prevent these instructions being reordered by the compiler.
 *
 * Apparently, this is standard GCC behaviour (RFB 2012).
 */


/*
 * WORD set_sr(WORD new);
 *   sets sr to the new value, and return the old sr value
 */

#define set_sr(a)                         \
__extension__                             \
({short _r, _a = (a);                     \
  __asm__ volatile                        \
  ("move.w sr,%0\n\t"                     \
   "move.w %1,sr"                         \
  : "=&d"(_r)       /* outputs */        \
  : "nd"(_a)        /* inputs  */        \
  : "cc", "memory"   /* clobbered */      \
  );                                      \
  _r;                                     \
})


/*
 * WORD get_sr(void);
 *   returns the current value of sr.
 */

#define get_sr()                          \
__extension__                             \
({short _r;                               \
  __asm__ volatile                        \
  ("move.w sr,%0"                         \
  : "=dm"(_r)        /* outputs */        \
  :                  /* inputs  */        \
  : "cc", "memory"   /* clobbered */      \
  );                                      \
  _r;                                     \
})



/*
 * void regsafe_call(void *addr)
 *   Saves all registers to the stack, calls the function
 *   that addr points to, and restores the registers afterwards.
 */
#ifdef __mcoldfire__
#define regsafe_call(addr)                         \
__extension__                                      \
({__asm__ volatile ("lea     -60(sp),sp\n\t"       \
                    "movem.l d0-d7/a0-a6,(sp)");   \
  ((void (*)(void))addr)();                        \
  __asm__ volatile ("movem.l (sp),d0-d7/a0-a6\n\t" \
                    "lea     60(sp),sp");          \
})
#else
#define regsafe_call(addr)                         \
__extension__                                      \
({__asm__ volatile ("movem.l d0-d7/a0-a6,-(sp)");  \
  ((void (*)(void))addr)();                        \
  __asm__ volatile ("movem.l (sp)+,d0-d7/a0-a6");  \
})
#endif



/*
 * Loops for the specified count; for a 1 millisecond delay on the
 * current system, use the value in the global 'loopcount_1_msec'.
 */
#define delay_loop(count)                   \
  __extension__                             \
  ({ULONG _count = (count);                 \
    __asm__ volatile                        \
    ("0:\n\t"                               \
     "subq.l #1,%0\n\t"                     \
     "jpl    0b"                            \
    :                   /* outputs */       \
    : "d"(_count)       /* inputs  */       \
    : "cc", "memory"    /* clobbered */     \
    );                                      \
  })

#endif /* ASM_H */
