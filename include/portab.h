/*
 * portab.h - Definitions for writing portable C
 *
 * Copyright (C) 2001 Lineo, Inc
 *               2001-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Warning: This header is included by both EmuTOS code (compiled with
 * cross-compiler) and native tools such as "bug" (compiled with native
 * compiler). So it must *not* assume that we are compiling for 68000,
 * or that pointers are 32-bit.
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

#ifdef __GNUC__
#define PRINTF_STYLE __attribute__ ((format (printf, 1, 2)))
#else
#define PRINTF_STYLE
#endif

#ifdef __GNUC__
#define SPRINTF_STYLE __attribute__ ((format (printf, 2, 3)))
#else
#define SPRINTF_STYLE
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
#define CLOBBER_MEMORY "memory"     /* When memory is the only clobber */
#define AND_MEMORY , CLOBBER_MEMORY /* When memory is the last clobber */
#else
#define CLOBBER_MEMORY
#define AND_MEMORY
#endif

/*
 * Restricted pointer parameters are advertised to never overlap.
 * https://en.wikipedia.org/wiki/Restrict
 */
#if __GNUC_PREREQ(4, 5)
#define RESTRICT __restrict__
#else
#define RESTRICT
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
#define FORCE_READ(x)   UNUSED(x)     /* Read a volatile hardware register */

/*
 *  Types
 */

typedef signed char     SBYTE;                  /*  Signed byte         */
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

/* BDOS program entry point */
typedef void PRG_ENTRY(void) /* NORETURN */;

/*
 *  Macros
 */

#define MAKE_UWORD(hi,lo) (((UWORD)(UBYTE)(hi) << 8) | (UBYTE)(lo))
#define MAKE_ULONG(hi,lo) (((ULONG)(UWORD)(hi) << 16) | (UWORD)(lo))
#define LOWORD(x) ((UWORD)(ULONG)(x))
#define HIWORD(x) ((UWORD)((ULONG)(x) >> 16))
#define LOBYTE(x) ((UBYTE)(UWORD)(x))
#define HIBYTE(x) ((UBYTE)((UWORD)(x) >> 8))
#define IS_ODD(x) ((x) & 1)
#define IS_ODD_POINTER(x) IS_ODD((ULONG)(x))
#define IS_32BIT_POINTER(x) ((ULONG)(x) & 0xff000000)

/*
 * The following ARRAY_SIZE() macro is taken from Linux kernel sources.
 *
 * Inspired from this page:
 * https://stackoverflow.com/questions/4415530/equivalents-to-msvcs-countof-in-other-compilers
 */

/*
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type size_t), so the expression can be used
 * e.g. in a structure initializer (or wherever else comma expressions
 * aren't permitted).
 *
 * Explanations there:
 * https://stackoverflow.com/questions/9229601/what-is-in-c-code
 *
 * Note that the name of this macro is misleading:
 * it actually produces a compilation bug when the parameter is *not zero*
 * However, when the parameter is zero, it evaluates as 0, hence the name.
 *
 * Additional "int dummy" added by VRI to avoid warning "struct has no named
 * members" when compiling with -pedantic.
 */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); int dummy; }) & 0)

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
 * It is useful to call a function with const data while the parameter
 * is not properly marked as const (usually because the constness depends
 * on other parameters).
 * A better implementation may add safer type checking.
 */
#define CONST_CAST(type, expr) ((type)(expr))

/* This is grammatically equivalent to a call to a function which does nothing.
 * It is useful as placeholder in conditional macros, when there is nothing to
 * do. So such macros can safely be called in if / else / while / for contexts,
 * without any potential side effects regarding to the grammar.
 */
#define NULL_FUNCTION() do { } while (0)

/*
 * Workarounds for the GCC strict aliasing rule
 */

#if __GNUC_PREREQ(3, 3)
# define MAY_ALIAS __attribute__ ((__may_alias__))
#else
# define MAY_ALIAS
#endif

typedef UBYTE UBYTE_ALIAS MAY_ALIAS;
typedef UWORD UWORD_ALIAS MAY_ALIAS;
typedef WORD WORD_ALIAS MAY_ALIAS;
typedef ULONG ULONG_ALIAS MAY_ALIAS;
typedef LONG LONG_ALIAS MAY_ALIAS;

#define ULONG_AT(p) (*(ULONG_ALIAS *)(p)) /* ULONG pointed by p, regardless of pointer type */

/*
 * GCC 7 needs special care to avoid warning when using switch/case fallthrough:
 * warning: this statement may fall through
 * This warning is generated when -Wimplicit-fallthrough is enabled, which is
 * the case when compiling with -Wextra (a.k.a -W).
 *
 * See documentation of -Wimplicit-fallthrough
 * https://gcc.gnu.org/onlinedocs/gcc-7.4.0/gcc/Warning-Options.html#index-Wimplicit-fallthrough
 * https://developers.redhat.com/blog/2017/03/10/wimplicit-fallthrough-in-gcc-7/
 *
 * Special comments such as -fallthrough can be put just before a switch label
 * to disable the warning. But this does not work if there is a #endif between
 * both.
 * To avoid any ambiguity, we use the explicit FALLTHROUGH macro.
 */
#if __GNUC_PREREQ(7, 1)
# define FALLTHROUGH __attribute__ ((fallthrough))
#else
# define FALLTHROUGH NULL_FUNCTION()
#endif

/*
 * See:
 * - https://gcc.gnu.org/onlinedocs/gcc-4.4.0/gcc/Function-Specific-Option-Pragmas.html
 *
 * NOTE:
 * - Later GCC manual states:
 *     Optimize attribute should be used for debugging purposes only.
 *     It is not suitable in production code.
 */
#if __GNUC_PREREQ(4, 4)
/* potentially reduce function size (and perf) for -O2 (256k / 512k) builds */
# define OPTIMIZE_SMALL __attribute__ ((optimize("Os")))
/* potentially increase function perf (and size) for -Os (192k) build */
# define OPTIMIZE_O2 __attribute__ ((optimize("O2")))
# define OPTIMIZE_O3 __attribute__ ((optimize("O3")))
#else
# define OPTIMIZE_SMALL
# define OPTIMIZE_O2
# define OPTIMIZE_O3
#endif

#endif /* PORTAB_H */
