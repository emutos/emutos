/*
 *  kprint.h - header file for keyboard/console routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _KPRINT_H
#define _KPRINT_H

// #include "portab.h"

#ifdef __GNUC__
#define PRINTF_STYLE __attribute__ ((format (printf, 1, 2)))
#else
#define PRINTF_STYLE
#endif

/* console output */
extern int cprintf(const char *fmt, ...) PRINTF_STYLE;
extern void cputs( char *s );
//extern void cstatus(long status);

/* native debugging output */
extern int kprintf(const char *fmt, ...) PRINTF_STYLE;
extern void kprint( char *s );
extern void kputb( BYTE *l );
extern void kputw( WORD *l );
extern void kputl( LONG *l );
extern void kputp( VOID *p );

#endif /* _KPRINT_H */

