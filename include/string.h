/*
 * string.h - EmuTOS own copy of an ANSI standard header
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


/* LVL: I do not use types WORD and ULONG on purpose, to
 * avoid gcc complaining about conflicting types for built-in
 * functions (memset, memcpy, strlen, ...).
 *
 * Nevertheless this header will only work with -mshort option.
 */

#ifndef STRING_H
#define STRING_H

/* string routines */

#if !(USE_STATIC_INLINES)
char *strcpy(char *dest, const char *src);
#endif

unsigned long strlcpy(char *dest,const char *src,unsigned long count);
unsigned long int strlen(const char *s);
short strlencpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, unsigned long int n);
int strncasecmp(const char *a, const char *b, unsigned long int n);
char *strchr(const char *s, int c);
int toupper(int c);
int sprintf(char *str, const char *fmt, ...);


/* Inline string routines: */
#if USE_STATIC_INLINES
static inline char *strcpy(char *dest, const char *src)
{
    register char *tmp = dest;

    while( (*tmp++ = *src++) )
        ;
    return dest;
}
#endif

/* block memory routines */

int memcmp(const void * s1, const void * s2, unsigned long int n);

/* moves length bytes from src to dst. returns dst as passed.
 * the behaviour is undefined if the two regions overlap.
 */
void memcpy(void * dst, const void * src,
            long unsigned int length);

/* moves length bytes from src to dst, performing correctly
 * if the two regions overlap. returns dst as passed.
 */
void memmove(void * dst, const void * src,
             long unsigned int length);

/* fills with byte c, returns the given address. */
void * memset(void *address, int c, long unsigned int size);

/* clear memory */
void bzero(void *address, long unsigned int size);

#endif /* STRING_H */

