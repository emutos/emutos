/*
 *  ps.h - common header file for ps files
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#define	GLOBAL	.xref
#define	EXTERN	.xdef

#define	PUSH(m,x)	move.m	x,-(sp)
#define	POP(m,x)	move.m	(sp)+,x
#define	PUSHA		movem.l	d1-d7/a0-a6,-(sp)
#define	POPA		movem.l	(sp)+,d1-d7/a0-a6

