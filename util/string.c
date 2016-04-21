/*
 * string.c - simple implementation of <string.h> ANSI routines
 *
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Note: compiling EmuTOS with option -lc does not work. Because of
 * this, replacements for common string routines are provided here.
 */

#include "config.h"
#include <stdarg.h>
#include "doprintf.h"
#include "string.h"
#include "kprint.h"


/* The following functions are either used as inlines in string.h
   or here as normal functions */
#if !(USE_STATIC_INLINES)
char *strcpy(char *dest, const char *src)
{
    char *tmp = dest;

    while( (*tmp++ = *src++) )
        ;
    return dest;
}
#endif

/*
 * strlcpy: a better strcnpy()
 *
 * strlcpy() was introduced in OpenBSD in the 90s as a
 * replacement for strncpy().  it has three improvements
 * compared to strncpy():
 *    . it always nul-terminates the destination (as long
 *      as the length is > 0)
 *    . it does not nul-fill the destination (a performance
 *      benefit when the source is short and the destination
 *      is large)
 *    . the return value allows a simple test for truncation
 *
 * this implementation of strlcpy() was written from scratch
 * by roger burrows.  it is based on the description of the
 * function in the paper "strlcpy and strlcat - consistent,
 * safe string copy and concatenation", by todd c. miller
 * and theo de raadt.
 *
 * description: strlcpy() copies bytes from src to dest,
 * stopping when the last (pre-NUL) byte of src is reached
 * OR count-1 bytes have been copied, whichever comes first.
 * as long as count is not zero, dest is then NUL-terminated.
 *
 * strlcpy() returns the length of src (excluding the
 * terminating NUL).  this allows a simple test for string
 * truncation: if the return value is greater than or equal
 * to the specified length, then truncation has occurred.
 */
size_t strlcpy(char *dest,const char *src,size_t count)
{
char *d = dest;
const char *s = src;
size_t n;

    if (count > 0) {
        for (n = count-1; *s && n; n--)
            *d++ = *s++;
        *d = '\0';
    }

    while (*s++)
        ;

    return s-src-1;
}

size_t strlen(const char *s)
{
    size_t n;

    for (n = 0; *s++; n++);
    return (n);
}

char *strcat(char *dest, const char *src)
{
    char *tmp = dest;
    while( *tmp++ )
        ;
    tmp --;
    while( (*tmp++ = *src++) )
        ;
    return dest;
}

int strcmp(const char *a, const char *b)
{
    unsigned char s1, s2;

    while(1) {
        s1 = (unsigned char)*a++;
        s2 = (unsigned char)*b++;
        if (s1 != s2)
            return s1 - s2;
        if (s1 == '\0')
            break;
    }

    return 0;
}

int memcmp(const void * aa, const void * bb, size_t n)
{
    const unsigned char * a = aa;
    const unsigned char * b = bb;
    unsigned char s1, s2;

    while(n-- > 0) {
        s1 = *a++;
        s2 = *b++;
        if (s1 != s2)
            return s1 - s2;
    }

    return 0;
}

int strncmp(const char *a, const char *b, size_t n)
{
    unsigned char s1, s2;

    while(n-- > 0) {
        s1 = (unsigned char)*a++;
        s2 = (unsigned char)*b++;
        if (s1 != s2)
            return s1 - s2;
        if (s1 == '\0')
            break;
    }

    return 0;
}

int strncasecmp(const char *a, const char *b, size_t n)
{
    unsigned char s1, s2;

    while(n-- > 0) {
        s1 = toupper((unsigned char)*a++);
        s2 = toupper((unsigned char)*b++);
        if (s1 != s2)
            return s1 - s2;
        if (s1 == '\0')
            break;
    }

    return 0;
}

int toupper(int c)
{
    if(c>='a' && c<='z')
        return(c-'a'+'A');
    else
        return(c);
}


/*
 * Implementation of sprintf():
 * It uses doprintf to print into a string.
 * Note: It is currently not reentrant.
 */

static char *sprintf_str;
static int sprintf_flag;

static void sprintf_outc(int c)     /* Output one character from doprintf */
{
    *sprintf_str++ = c;
}

int sprintf(char *str, const char *fmt, ...)
{
    int n;
    va_list ap;

    /*if (sprintf_flag)  panic("sprintf is not reentrant!\n");*/

    sprintf_flag += 1;
    sprintf_str = str;

    va_start(ap, fmt);
    n = doprintf(sprintf_outc, fmt, ap);
    va_end(ap);

    str[n] = 0;                     /* Terminate string with a 0 */
    sprintf_flag -= 1;

    return n;
}
