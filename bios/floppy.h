/*
 * floppy.h - floppy routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _FLOPPY_H
#define _FLOPPY_H

#include "portab.h"
 
/* structures */

struct bpb /* bios parameter block */
{
	WORD	recsiz; 	/* sector size in bytes */
	WORD	clsiz;		/* cluster size in sectors */
	WORD	clsizb; 	/* cluster size in bytes */
	WORD	rdlen;		/* root directory length in records */
	WORD	fsiz;		  /* fat size in records */
	WORD	fatrec; 	/* first fat record (of last fat) */
	WORD	datrec; 	/* first data record */
	WORD	numcl;		/* number of data clusters available */
	WORD	b_flags;
};

typedef struct bpb BPB;

extern BPB flop_bpb[2];

/* bios functions */

#define RW_READ             0
#define RW_WRITE            1
#define RW_READ_NO_MEDIACH  2
#define RW_WRITE_NO_MEDIACH 3
#define RW_RW_MASK          1


/* Functions hdv_boot, hdv_init, rwabd, getbpb and mediach are 
 * called using variable pointers. The five flop_* functions
 * are the implementation of these provided by floppy.c.
 */

extern LONG flop_hdv_boot(VOID);
extern VOID flop_hdv_init(VOID);
extern LONG flop_rwabs(WORD rw, LONG buf, WORD cnt, WORD recnr, WORD dev);
extern LONG flop_getbpb(WORD dev);
extern LONG flop_mediach(WORD dev);

/* xbios functions */

extern WORD floprd(LONG buf, LONG filler, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 
extern WORD flopwr(LONG buf, LONG filler, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 
extern WORD flopfmt(LONG buf, LONG filler, WORD dev, WORD spt,
                    WORD track, WORD side, WORD interleave, 
                    ULONG magic, WORD virgin); 
extern VOID protobt(LONG buf, LONG serial, WORD type, WORD exec);
extern WORD flopver(LONG buf, LONG filler, WORD dev, 
                    WORD sect, WORD track, WORD side, WORD count); 
extern WORD floprate(WORD dev, WORD rate);

/* internal functions */

extern UWORD compute_cksum(LONG buf);

/* call hdv_boot() and execute bootsector */
extern VOID do_hdv_boot(VOID);  

/* initialise hdv_* vectors and floppy parameters */
extern VOID floppy_init(VOID);

#endif /* _FLOPPY_H */
