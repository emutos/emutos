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


extern void cputs( char *s );
extern void cstatus(long status);
extern void kprint( char *s );
extern void kputb( BYTE *l );
extern void kputw( WORD *l );
extern void kputl( LONG *l );
extern void kputp( VOID *p );

