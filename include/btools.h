/*
 * btools.h - header file for memory block copy and set routines
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
 * functions memset and memcpy.
 *
 * Nevertheless this header will only work with -mshort option.
 */

#ifndef _BTOOLS_H
#define _BTOOLS_H

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

