/*
 * string.h - EmuTOS own copy of an ANSI standard header
 *
 * Copyright (c) 2001-2014 The EmuTOS development team
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
char *strcpy(char *dest, const char *src);
#endif

size_t strlcpy(char *dest,const char *src,size_t count);
size_t strlen(const char *s);
short strlencpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
int strncasecmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
int toupper(int c);
int sprintf(char *str, const char *fmt, ...);


/* Inline string routines: */
#if USE_STATIC_INLINES
static __inline__ char *strcpy(char *dest, const char *src)
{
    register char *tmp = dest;

    while( (*tmp++ = *src++) )
        ;
    return dest;
}
#endif

/* block memory routines */

int memcmp(const void * s1, const void * s2, size_t n);

/* moves length bytes from src to dst. returns dst as passed.
 * the behaviour is undefined if the two regions overlap.
 */
void * memcpy(void * dst, const void * src,
              size_t length);

/* moves length bytes from src to dst, performing correctly
 * if the two regions overlap. returns dst as passed.
 */
void * memmove(void * dst, const void * src,
               size_t length);

/* fills with byte c, returns the given address. */
void * memset(void *address, int c, size_t size);

/* clear memory */
void bzero(void *address, size_t size);

#endif /* STRING_H */
