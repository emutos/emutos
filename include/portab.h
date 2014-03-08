/*
 * portab.h - Definitions for writing portable C
 *
 * Copyright (c) 2001 Lineo, Inc
 *               2001-2013 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * I guess now this file is just badly named. it just contains
 * common macros that were so often in the code that we just have
 * to keep them here.
 */

#ifndef PORTAB_H
#define PORTAB_H



/*
 *  Compiler Definitions
 */

#ifdef __GNUC__
#define NORETURN __attribute__ ((noreturn))
#else
#define NORETURN
#endif

/* Convenience macros to test the versions of glibc and gcc.
   Use them like this:
   #if __GNUC_PREREQ (2,8)
   ... code requiring gcc 2.8 or later ...
   #endif
   Note - they won't work for gcc1 or glibc1, since the _MINOR macros
   were not defined then.  */
#undef __GNUC_PREREQ
#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
        ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define __GNUC_PREREQ(maj, min) 0
#endif

#undef __CLOBBER_RETURN
#if __GNUC_PREREQ(3, 3)
# define __CLOBBER_RETURN(a)
#else
# define __CLOBBER_RETURN(a) a,
#endif

#undef AND_MEMORY
#if __GNUC_PREREQ(2, 6)
#define AND_MEMORY , "memory"
#else
#define AND_MEMORY
#endif

/*
 *  Constants
 */

#define NULL    0                       /*      Null character value        */
#define TRUE    (1)                     /*      Function TRUE  value        */
#define FALSE   (0)                     /*      Function FALSE value        */
#define NULLPTR ((void*)0)              /*      Null pointer value          */

/*
 *  Miscellaneous
 */

#define REG             register                /* register variable       */
#define GLOBAL                                  /* Global variable         */
#define UNUSED(x)       (void)(x)               /* Unused variable         */
#define MAYBE_UNUSED(x) UNUSED(x)               /* Maybe unused variable   */

/*
 *  Types
 */

typedef char            BYTE;                   /*  Signed byte         */
typedef unsigned char   UBYTE;                  /*  Unsigned byte       */
typedef unsigned long   ULONG;                  /*  unsigned 32 bit word*/
typedef long            PTR;                    /*  32 bit pointer */
typedef int             BOOL;                   /*  boolean, TRUE or FALSE */
typedef short int       WORD;                   /*  signed 16 bit word  */
typedef unsigned short  UWORD;                  /*  unsigned 16 bit word*/
typedef long            LONG;                   /*  signed 32 bit word  */

/* pointer to function returning LONG */
typedef LONG (*PFLONG)(void);

/* pointer to function returning VOID */
typedef void (*PFVOID)(void);

/*
 *  Macros
 */

#define MAKE_UWORD(hi,lo) (((UWORD)(BYTE)(hi) << 8) | (BYTE)(lo))
#define MAKE_ULONG(hi,lo) (((ULONG)(UWORD)(hi) << 16) | (UWORD)(lo))

/*
 * Workarounds for the GCC strict aliasing rule
 */

#if __GNUC_PREREQ(3, 3)
# define MAY_ALIAS __attribute__ ((__may_alias__))
#else
# define MAY_ALIAS
#endif

typedef UBYTE UBYTE_ALIAS MAY_ALIAS;
typedef BYTE BYTE_ALIAS MAY_ALIAS;
typedef WORD WORD_ALIAS MAY_ALIAS;
typedef LONG LONG_ALIAS MAY_ALIAS;

#endif /* PORTAB_H */
