/*
 * mem.h - header file for memory and process management routines  
 *
 * Copyright (c) 2001 Lineo, Inc. and 
 *
 * Authors:
 *  xxx <xxx@xxx>
 *  LVL Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _MEM_H
#define _MEM_H


/*
 *  conditional compile switches
 */

#define OSMPANIC	FALSE
#define OSMLIST 	FALSE


/*
 *  externals
 */

extern	FTAB	sft[] ;
extern	long	errbuf[3] ;			/*  sup.c  */
extern	MPB	pmd ;
extern	int	osmem[] ;
extern	int	osmlen ;
extern	int	*root[20];
extern	int	osmptr;

/* 
 * in osmem.c
 */

/*  xmgetblk - get a block of memory from the o/s pool. */
void *xmgetblk(int i);

/*  MGET - wrapper around xmgetblk */
#define MGET(x) ((x *) xmgetblk((sizeof(x) + 15)>>4))

/*  xmfreblk - free up memory allocated through mgetblk */
void xmfreblk(void *m);

/*
 * in umem.c 
 */

/* allocate memory */
long xmalloc(long amount);
/* mfree */
long xmfree(long addr);
/* mshrink */
long xsetblk(int n, void *blk, long len);

/*
 * in iumem.c
 */ 

/* find first fit for requested memory in ospool */
MD *ffit(long amount, MPB *mp);
/* Free up a memory descriptor */
void freeit(MD *m, MPB *mp);


#endif /* _MEM_H */
