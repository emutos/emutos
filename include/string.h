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

#ifndef _STRING_H
#define _STRING_H

/* string routines */

unsigned long int strlen(const char *s);
char *strcat(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, unsigned long int n);


/* block memory routines */

int memcmp(const void * s1, const void * s2, unsigned long int n);

/* moves length bytes from src to dst. returns dst as passed.
 * the behaviour is undefined if the two regions overlap.
 */
void * memcpy(void * dst, 
              const void * src, 
              long unsigned int length);
       
/* moves length bytes from src to dst, performing correctly 
 * if the two regions overlap. returns dst as passed.
 */
void * memmove(void * dst,
               const void * src, 
               long unsigned int length);

/* same as memmove save for the order of arguments. */
void * bcopy(void * src, void * dst, long unsigned int size);

/* fills with byte c, returns the given address. */
void * memset(void *address, int c, long unsigned int size);

/* clear memory */
void bzero(void *address, long unsigned int size);

#endif /* _BTOOLS_H */

