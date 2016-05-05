/*
 * fsbuf.c - buffer mgmt for file system
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002-2015 The EmuTOS development team
 *
 * Authors:
 *  SCC   Steve C. Cavender
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "fs.h"
#include "gemerror.h"
#include "biosbind.h"
#include "ahdi.h"
#include "mem.h"
#include "string.h"
#include "kprint.h"

extern BCB *bufl[];     /* buffer lists - two lists:  FAT and dir/data */

#define NUMBUFS 2       /* buffers per list */

/* creates a chain of BCBs and corresponding buffers */
static void *create_chain(char *p,LONG n)
{
    BCB *bcbptr;
    WORD i;

    for (i = 0; i < NUMBUFS; i++, p += n) {
        bcbptr = (BCB *)p;
        memset(bcbptr,0x00,sizeof(BCB));
        if (i < NUMBUFS-1)                  /* chain to next */
            bcbptr->b_link = (BCB *)(p + n);
        bcbptr->b_bufdrv = -1;              /* mark as invalid */
        bcbptr->b_bufr = p + sizeof(BCB);
    }

    return p;
}

/*
 * bufl_init - BDOS buffer list initialization
 */
void bufl_init(void)
{
    char *p;
    LONG n;

    n = sizeof(BCB) + pun_ptr->max_sect_siz;
    p = (char *)xmalloc(2L*NUMBUFS*n);
    if (!p)
        panic("bufl_init(%ld): no memory\n",2L*NUMBUFS*n);

    /* set up FAT chain */
    bufl[BI_FAT] = (BCB *)p;
    p = create_chain(p,n);

    /* set up dir/data chain */
    bufl[BI_DATA] = (BCB *)p;
    create_chain(p,n);
}



/*
 * flush -
 *
 * NOTE: longjmp_rwabs() is a macro that includes a longjmp() which is
 *       executed if the BIOS returns an error, therefore flush() does
 *       not need to return any error codes.
 */

void flush(BCB *b)
{
    int n,d;
    DMD *dm;

    /* if buffer not in use or not dirty, no work to do */

    if ((b->b_bufdrv == -1) || (!b->b_dirty)) {
        b->b_bufdrv = -1;
        return;
    }

    dm = b->b_dm;               /*  media descr for buffer      */
    n = b->b_buftyp;
    d = b->b_bufdrv;
    b->b_bufdrv = -1;           /* invalidate in case of error */

    longjmp_rwabs(1, (long)b->b_bufr, 1, b->b_bufrec+dm->m_recoff[n], d);

    /* flush to both fats */

    if (n == BT_FAT) {
        longjmp_rwabs(1, (long)b->b_bufr, 1,
                      b->b_bufrec+dm->m_recoff[BT_FAT]-dm->m_fsiz, d);
    }
    b->b_bufdrv = d;                    /* re-validate */
    b->b_dirty = 0;
}



/*
 * getrec - return the ptr to the buffer containing the desired record
 */

char *getrec(RECNO recn, OFD *of, int wrtflg)
{
    DMD *dm = of->o_dmd;
    BCB *b;
    BCB *p,*mtbuf,**q,**phdr;
    int n,err;

    KDEBUG(("getrec 0x%lx, %p, 0x%x\n",recn,dm,wrtflg));

    /* put bcb management here */
    if (of->o_dmd->m_fatofd == of)  /* is this the OFD for the 'FAT file'? */
        n = BT_FAT;                 /* yes, must be FAT access             */
    else if (!of->o_dnode)          /* no - do we have a dir node?         */
        n = BT_ROOT;                /* no, must be root                    */
    else n = BT_DATA;               /* yes, must be normal dir/file        */

    KDEBUG(("n=%i, dm->m_recoff[n]=0x%lx\n",n,dm->m_recoff[n]));

    mtbuf = 0;
    phdr = &bufl[n==BT_FAT ? BI_FAT : BI_DATA];

    /*
     * See if the desired record for the desired drive is in memory.
     * If it is, we will use it.  Otherwise we will use
     *          the last invalid (available) buffer,  or
     *          the last (least recently) used buffer.
     */

    for (b = *(q = phdr); b; b = *(q = &b->b_link))
    {
        if ((b->b_bufdrv == dm->m_drvnum) && (b->b_buftyp == n) && (b->b_bufrec == recn))
            break;
        /*
         * keep track of the last invalid buffer
         */
        if (b->b_bufdrv == -1)  /*  if buffer not valid */
            mtbuf = b;          /*    then it's 'empty' */
    }

    if (!b)
    {
        /*
         * not in memory.  If there was an 'empty' buffer, use it.
         */
        if (mtbuf)
            b = mtbuf;

        /*
         * find predecessor of mtbuf, or last guy in list, which
         * is the least recently used.
         */

doio:   for (p = *(q = phdr); p->b_link; p = *(q = &p->b_link))
            if (b == p)
                break;
        b = p;

        /*
         * flush the current contents of the buffer, and read in the
         * new record.
         */

        flush(b);
        longjmp_rwabs(0, (long)b->b_bufr, 1, recn+dm->m_recoff[n], dm->m_drvnum);

        /*
         * make the new buffer current
         */

        b->b_bufrec = recn;
        b->b_dirty = 0;
        b->b_buftyp = n;
        b->b_bufdrv = dm->m_drvnum;
        b->b_dm = dm;
    }
    else
    {   /* use a buffer, but first validate media */
        err = Mediach(b->b_bufdrv);
        if (err != 0) {
            if (err == 1) {
                goto doio; /* media may be changed */
            } else if (err == 2) {
                /* media definitely changed */
                errdrv = b->b_bufdrv;
                rwerr = E_CHNG; /* media change */
                errcode = rwerr;
                longjmp(errbuf,1);
            }
        }
    }

    /*
     *  now put the current buffer at the head of the list
     */

    *q = b->b_link;
    b->b_link = *phdr;
    *phdr = b;

    /*
     *  if we are writing to the buffer, dirty it.
     */

    if (wrtflg) {
        b->b_dirty = 1;
    }

    return(b->b_bufr);
}
