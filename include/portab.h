/*
 * portab.h - Definitions for writing portable C
 *
 * Copyright (C) 2001 Lineo, Inc
 *               2001-2016 The EmuTOS development team
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
 * The following include provides definitions for NULL, size_t...
 * It is provided by the compiler itself, so this is not a dependency
 * to the standard library.
 */
#include <stddef.h>

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

#define TRUE    (1)                     /*      Function TRUE  value        */
#define FALSE   (0)                     /*      Function FALSE value        */

/*
 *  Miscellaneous
 */

#define GLOBAL                                  /* Global variable         */
#define UNUSED(x)       (void)(x)               /* Unused variable         */
#define MAYBE_UNUSED(x) UNUSED(x)               /* Maybe unused variable   */

/*
 *  Types
 */

typedef char            BYTE;                   /*  Signed byte         */
typedef unsigned char   UBYTE;                  /*  Unsigned byte       */
typedef unsigned long   ULONG;                  /*  unsigned 32 bit word*/
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

#define MAKE_UWORD(hi,lo) (((UWORD)(UBYTE)(hi) << 8) | (UBYTE)(lo))
#define MAKE_ULONG(hi,lo) (((ULONG)(UWORD)(hi) << 16) | (UWORD)(lo))
#define LOWORD(x) ((UWORD)(ULONG)(x))
#define HIWORD(x) ((UWORD)((ULONG)(x) >> 16))
#define LOBYTE(x) ((UBYTE)(UWORD)(x))
#define HIBYTE(x) ((UBYTE)((UWORD)(x) >> 8))

/*
 * The following ARRAY_SIZE() macro is taken from Linux kernel sources.
 *
 * Inspired from this page:
 * http://stackoverflow.com/questions/4415530/equivalents-to-msvcs-countof-in-other-compilers
 */

/*
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type size_t), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * aren't permitted).
 *
 * Explanations there:
 * http://stackoverflow.com/questions/9229601/what-is-in-c-code
 *
 * Note that the name of this macro is misleading:
 * it actually produces a compilation bug when the parameter is *not zero*
 * However, when the parameter is zero, it evaluates as 0, hence the name.
 */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))

#if __GNUC_PREREQ(3, 3)
/* &a[0] degrades to a pointer: a different type from an array */
# define __must_be_array(a) \
    BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(__typeof__(a), __typeof__(&a[0])))
#else
# define __must_be_array(a) 0
#endif

/* Number of elements of an array */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

/* Lightweight cast to only remove const and volatile qualifiers from a pointer.
 * This is similar to the C++ const_cast<> operator.
 * It is usefull to call a function with const data while the parameter
 * is not properly marked as const (usually because the constness depends
 * on other parameters).
 * A better implementation may add safer type checking.
 */
#define CONST_CAST(type, expr) ((type)(expr))

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
typedef UWORD UWORD_ALIAS MAY_ALIAS;
typedef WORD WORD_ALIAS MAY_ALIAS;
typedef ULONG ULONG_ALIAS MAY_ALIAS;
typedef LONG LONG_ALIAS MAY_ALIAS;
typedef BYTE *BYTEPTR_ALIAS MAY_ALIAS;

#endif /* PORTAB_H */
