/*
 * string.c - simple implementation of <string.h> ANSI routines
 *
 * Copyright (c) 2002 by following authors
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

#include "string.h"
#include "kprint.h"

/* doprintf implemented in doprintf.c. 
 * This is an OLD one, and does not support floating point.
 */
#include <stdarg.h>
extern int doprintf(void (*outc)(int), const char *fmt, va_list ap);


/* The following functions are either used as inlines in string.h
   or here as normal functions */
#if !(USE_STATIC_INLINES)
char *strcpy(char *dest, const char *src) 
{
    register char *tmp = dest;
 
    while( (*tmp++ = *src++) ) 
        ;
    return dest;
}
#endif


unsigned long int strlen(const char *s)
{
    int n;

    for (n = 0; *s++; n++);
    return (n);
}

char *strcat(char *dest, const char *src) 
{
    register char *tmp = dest;
    while( *tmp++ ) 
        ;
    tmp --;
    while( (*tmp++ = *src++) ) 
        ;
    return dest;
}

int strcmp(const char *a, const char *b)
{
    while(*a && *a == *b) {
        a++;
        b++;
    }
    if(*a == *b) return 0;
    if(*a < *b) return -1;
    return 1;
}

int memcmp(const void * aa, const void * bb, unsigned long int n)
{
    const unsigned char * a = aa;
    const unsigned char * b = bb;
    
    while(n && *a == *b) {
        n--;
        a++;
        b++;
    }
    if(n == 0) return 0;
    if(*a < *b) return -1;
    return 1;
}

#if NEEDED
int strncmp(const char *a, const char *b, unsigned long int n)
{
    while(n && *a && *a == *b) {
        n--;
        a++;
        b++;
    }
    if(n == 0) return 0;
    if(*a == *b) return 0;
    if(*a < *b) return -1;
    return 1;
}
#endif /* NEEDED */

int strncasecmp(const char *a, const char *b, unsigned long int n)
{
    while(n && *a && toupper(*a) == toupper(*b)) {
        n--;
        a++;
        b++;
    }
    if(n == 0) return 0;
    if(toupper(*a) == toupper(*b)) return 0;
    if(toupper(*a) < toupper(*b)) return -1;
    return 1;
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
