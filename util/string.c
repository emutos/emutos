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

char *strcpy(char *dest, const char *src) 
{
    register char *tmp = dest;
 
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

