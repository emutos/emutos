/*
 * string.h - EmuTOS's own version of the ANSI standard header
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

/* string routines */

#if !(USE_STATIC_INLINES)
char *strcpy(char *RESTRICT dest, const char *RESTRICT src);
char *strcat(char *RESTRICT dest, const char *RESTRICT src);
#endif

size_t strlcpy(char *RESTRICT dest,const char *RESTRICT src,size_t count);
size_t strlen(const char *s);
short strlencpy(char *RESTRICT dest, const char *RESTRICT src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
int strncasecmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
int toupper(int c);
int sprintf(char *RESTRICT str, const char *RESTRICT fmt, ...) SPRINTF_STYLE;


/* Inline string routines: */
#if USE_STATIC_INLINES
static __inline__ __attribute__((always_inline)) void inline_strcpy(char *RESTRICT dest, const char *RESTRICT src)
{
    /* Until gcc is able to generate such compact code... we do this manually. */
    /* NB: For simplification, this block returns void. */
    __asm__ volatile
    (
        "1:\n\t"
        "move.b  (%1)+,(%0)+\n\t"
        "jne     1b"
    : "+a"(dest), "+a"(src) /* outputs */
    : /* inputs */
    : CLOBBER_MEMORY /* clobbered */
    );
}

#define strcpy(dest, src) inline_strcpy(dest, src)

static __inline__ __attribute__((always_inline)) void inline_strcat(char *RESTRICT dest, const char *RESTRICT src)
{
    /* Until gcc is able to generate such compact code... we do this manually. */
    /* NB: For simplification, this block returns void. */
    __asm__ volatile
    (
        "1:\n\t"
        "tst.b   (%0)+\n\t"
        "jne     1b\n\t"
        "subq.l  #1,%0\n\t"
        "2:\n\t"
        "move.b  (%1)+,(%0)+\n\t"
        "jne     2b"
    : "+a"(dest), "+a"(src) /* outputs */
    : /* inputs */
    : CLOBBER_MEMORY /* clobbered */
    );
}

#define strcat(dest, src) inline_strcat(dest, src)

#endif

/* block memory routines */

int memcmp(const void * s1, const void * s2, size_t n);

/* moves length bytes from src to dst. returns dst as passed.
 * the behaviour is undefined if the two regions overlap.
 */
void * memcpy(void * RESTRICT dst, const void * RESTRICT src,
              size_t length);

/* moves length bytes from src to dst, performing correctly
 * if the two regions overlap. returns dst as passed.
 */
void * memmove(void * dst, const void * src,
               size_t length);

/* fills with byte c, returns the given address. */
void * memset(void *address, int c, size_t size);

/* clear memory
 * we use our own name to circumvent GCC converting bzero to memset
 */
#define bzero bzero_nobuiltin
void bzero(void *address, size_t size);

#endif /* STRING_H */
