/*
 * fsio.c - read/write routines for the file system
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2024 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "fs.h"
#include "gemerror.h"
#include "biosbind.h"
#include "string.h"
#include "tosvars.h"
#include "intmath.h"


#define CNTMAX  0x7FFFul  /* 16-bit MAXINT */


/*
 * addit - update the OFD for the file
 *
 * update the OFD for the file to reflect the fact that 'siz' bytes
 * have been written to it.
 *
 * update o_curbyt iff less than 1 cluster is being transferred
 */
static void addit(OFD *p, long siz)
{
    DFD *dfd = p->o_dfd;

    if (siz < p->o_dmd->m_clsizb)
        p->o_curbyt += siz;

    p->o_bytnum += siz;

    if (p->o_bytnum > dfd->o_fileln)
    {
        dfd->o_fileln = p->o_bytnum;
        dfd->o_flag |= O_DIRTY;
    }
}


/*
 * usrio - interface to rwabs
 *
 * NOTE: longjmp_rwabs() is a macro that includes a longjmp() which is
 *       executed if the BIOS returns an error, therefore usrio() does
 *       not need to return any error codes.
 */
static void usrio(int rwflg, int num, long strt, char *ubuf, DMD *dm)
{
    BCB *b;

    for (b = bufl[BI_DATA]; b; b = b->b_link)
    {
        if ((b->b_bufdrv == dm->m_drvnum) &&
            (b->b_bufrec >= strt) &&
            (b->b_bufrec < strt+num))
        {
            if (b->b_dirty)
                flush(b);
            b->b_bufdrv = -1;
        }
    }

    longjmp_rwabs(rwflg, (long)ubuf, num, strt+dm->m_recoff[BT_DATA], dm->m_drvnum);
}


/*
 * read/write records on behalf of xrw()
 *
 * returns
 *      NULL if end of cluster chain was reached
 *      otherwise, updated buffer ptr
 */
static char *xrw_recs(WORD wrtflg, OFD *p, RECNO startrec, RECNO numrecs, char *ubufr)
{
    DMD *dm;
    RECNO hdrrec, tailrec, last, nrecs;
    CLNO numclus;
    LONG nbytes;
    WORD rc;
    BOOL first_time;

    dm = p->o_dmd;

    /*
     * do 'header' records
     */
    hdrrec = startrec & dm->m_clrm;
    if (hdrrec)     /* not on a cluster boundary */
    {
        /*
         * the number of records to write is the minimum of:
         *  .the number of records remaining in the current cluster, and
         *  .the number of records remaining in the file
         */
        hdrrec = dm->m_clsiz - hdrrec;      /* M00.14.01 */
        if (hdrrec > numrecs)               /* M00.14.01 */
            hdrrec = numrecs;               /* M00.14.01 */

        KDEBUG(("xrw(%c %d): xfer head recs %ld->%ld\n",
                wrtflg?'W':'R',dm->m_drvnum,startrec,startrec+hdrrec-1));
        usrio(wrtflg,hdrrec,startrec,ubufr,dm);
        nbytes = hdrrec << dm->m_rblog;
        ubufr += nbytes;
        addit(p,nbytes);
        numrecs -= hdrrec;
    }

    /* now we can calculate the number of 'tail' records */
    tailrec = numrecs & dm->m_clrm;

    /*
     * do whole (middle) clusters
     */
    numclus = numrecs >> dm->m_clrlog;
    last = nrecs = 0L;
    rc = 0;
    first_time = TRUE;

    while(TRUE)
    {
        if (numclus)
            rc = nextcl(p,wrtflg);

        if (first_time)
        {
            last = p->o_currec;
            first_time = FALSE;
        }

        /* if necessary, complete pending data transfer */

        if ((rc != 0)                       /* end of cluster chain */
         || (numclus == 0)                  /* end of request */
         || (p->o_currec != last + nrecs)   /* clusters aren't contiguous */
         || (nrecs + dm->m_clsiz > CNTMAX)) /* request is too large for one i/o */
        {
            if (nrecs)
            {
                KDEBUG(("xrw(%c %d): xfer main recs %ld->%ld\n",
                        wrtflg?'W':'R',dm->m_drvnum,last,last+nrecs-1));
                usrio(wrtflg,nrecs,last,ubufr,dm);
                nbytes = nrecs << dm->m_rblog;
                addit(p,nbytes);
                ubufr += nbytes;
                last = p->o_currec;
                nrecs = 0;
            }
        }

        if (rc != 0)
            return NULL;

        if (numclus == 0)
            break;

        nrecs += dm->m_clsiz;
        numclus--;
    }

    /*
     * do 'tail' records
     */
    if (tailrec)
    {
        if (nextcl(p,wrtflg))
            return NULL;
        KDEBUG(("xrw(%c %d): xfer tail recs %ld->%ld\n",
                wrtflg?'W':'R',dm->m_drvnum,p->o_currec,p->o_currec+tailrec-1));
        usrio(wrtflg,tailrec,p->o_currec,ubufr,dm);
        nbytes = tailrec << dm->m_rblog;
        addit(p,nbytes);
        ubufr += nbytes;
    }

    return ubufr;
}


/*
 * xrw - read/write for BDOS functions
 *
 * This transfers 'len' bytes between the file indicated by the OFD and
 * the buffer pointed to by 'ubufr'
 *
 * We wish to do the i/o in whole records (logical sectors) as much as
 * possible.  Therefore, we break the i/o up into 3 sections:
 *  . data at the start of the request which occupies part of a record
 *    along with data not in the request
 *  . data which is contained completely within records - this is handled
 *    separately by xrw_recs()
 *  . data at the end of the request which occupies part of a record
 *    along with data not in the request
 *
 *  returns
 *      nbr of bytes read/written from/to the file
 */
static long xrw(int wrtflg, OFD *p, long len, char *ubufr)
{
    DMD *dm;
    UBYTE *bufp;
    WORD bytn, lenxfr, lentail;
    RECNO recn, numrecs;
    LONG rc, bytpos;

    dm = p->o_dmd;                      /*  get drive media descriptor  */
    bytpos = p->o_bytnum;               /*  starting file position      */

    /*
     * get logical record number to start i/o with
     * (bytn will be byte offset into sector # recn)
     */
    recn = p->o_curbyt >> dm->m_rblog;
    bytn = p->o_curbyt & dm->m_rbm;

    recn += p->o_currec;

    /* do header bytes */
    if (bytn)
    {
        /* xfer len is min( #bytes req'd , */
        /* #bytes left in current record ) */

        lenxfr = min(len,dm->m_recsiz-bytn);
        bufp = getrec(recn,p,wrtflg);   /* get desired record  */
        addit(p,lenxfr);                /* update OFD          */
        len -= lenxfr;                  /* nbr left to do      */
        recn++;                         /* starting w/ next    */

        if (wrtflg)
            memcpy(bufp+bytn,ubufr,lenxfr);
        else memcpy(ubufr,bufp+bytn,lenxfr);

        ubufr += lenxfr;
    }

    /* "header" complete.      See if there is a "tail". */
    /* After that, see if there is anything left in the middle. */

    lentail = len & dm->m_rbm;              /* in bytes */
    numrecs = (len-lentail) >> dm->m_rblog; /* length of middle in records */

    if (numrecs)
    {
        ubufr = xrw_recs(wrtflg,p,recn,numrecs,ubufr);
        if (!ubufr)                         /* end of cluster chain reached */
            goto eof;
    }

    /* do tail bytes */
    if (lentail)
    {
        recn = p->o_curbyt >> dm->m_rblog;

        if ((!recn) || (recn == (RECNO)dm->m_clsiz))
        {
            if (nextcl(p,wrtflg))
                goto eof;
            recn = 0;
        }

        bufp = getrec((RECNO)p->o_currec+recn,p,wrtflg);
        addit(p,lentail);

        if (wrtflg)
             memcpy(bufp,ubufr,lentail);
        else memcpy(ubufr,bufp,lentail);
    }

eof:
    rc = p->o_bytnum - bytpos;

    return(rc);
}


/*
 * eof - check for end of file
 *
 * returns  1   eof
 *          0   not eof
 *          <0  error
 */
long eof(int h)
{
    OFD *f;

    f = getofd(h);
    if (!f)
        return EIHNDL;

    if (f->o_bytnum >= f->o_dfd->o_fileln)
        return 1;

    return 0;
}


/*
 * xlseek - seek to byte position n on file with handle h
 *
 * Function 0x42        f_seek
 *
 * Error returns
 *   EIHNDL
 *
 *   EINVFN
 *      ixlseek()
 */

long xlseek(long n, int h, int flg)
{
    OFD *f;

    f = getofd(h);
    if (!f)
        return(EIHNDL);

    if (flg == 2)
        n += f->o_dfd->o_fileln;
    else if (flg == 1)
        n += f->o_bytnum;
    else if (flg)
        return(EINVFN);

    return(ixlseek(f,n));
}

/*
 * ixlseek - file position seek
 *
 * Error returns
 *   ERANGE
 *   EINTRN
 *
 * NOTE: This function returns ERANGE and EINTRN errors, which are new
 *       error numbers I just made up (that is, they were not defined
 *       by the BIOS or by PC DOS).
 *
 * p: file descriptor for file in use
 * n: number of bytes to seek
 */

long ixlseek(OFD *p,long n)
{
    CLNO clnum, clx, curnum, i;
    DMD *dm = p->o_dmd;
    DFD *dfd = p->o_dfd;

    if ((n < 0) || (n > dfd->o_fileln))
        return ERANGE;

    if (n == 0)
    {
        p->o_curcl = p->o_currec = p->o_bytnum = p->o_curbyt = 0;
        return 0;
    }

    /*
     * calculate the desired position in units of 1 cluster
     */
    clnum = n >> dm->m_clblog;

    /*
     * if that's beyond where we are, we can chain forward;
     * otherwise, we need to start from the beginning
     */
    if (p->o_curcl && (n >= p->o_bytnum))   /* OK, we can chain forward */
    {
        /*
         * calculate the current position in units of 1 cluster
         */
        curnum = p->o_bytnum >> dm->m_clblog;

        /*
         * if we're currently at the end of a cluster, we haven't yet read
         * in the cluster that really corresponds to our position, so we
         * need to allow for that.  See the comments further below for why
         * we also do this when we're at the beginning of a cluster ...
         */
        if (((p->o_curbyt == 0) || (p->o_curbyt == dm->m_clsizb)) && p->o_bytnum)
            curnum--;

        clnum -= curnum;
        clx = p->o_curcl;
    }
    else            /* we have to start at the beginning */
        clx = dfd->o_strtcl;

    /*
     * note: if we're seeking to a position which is at a cluster boundary,
     * we actually point to the cluster before that.  this nonobvious action
     * is because, when the read point is at the start of a cluster, xrw()
     * starts its processing by handling whole clusters.  this occurs in
     * either the middle or tail section processing, but in both cases,
     * xrw() always chains to the next cluster before doing the actual read.
     *
     * see the code in xrw() if you need to know more ...
     */
    if ((n&dm->m_clbm) == 0)    /* go one less if on cluster boundary */
        clnum--;

    for (i = 0; i < clnum; i++)
    {
        clx = getclnum(clx,p);
        if (endofchain(clx))
            return EINTRN;      /* FAT chain is shorter than filesize says ... */
    }

    p->o_curcl = clx;
    p->o_currec = cl2rec(clx,dm);
    p->o_bytnum = n;
    p->o_curbyt = n & dm->m_clbm;

    return n;
}



/*
 * xread - read 'len' bytes  from handle 'h'
 *
 * Function 0x3F        f_read
 *
 * Error returns
 *   EIHNDL
 *   bios()
 */

long xread(int h, long len, void *ubufr)
{
    OFD *p;
    long ret;

    p = getofd(h);
    if (p)
        ret = ixread(p,len,ubufr);
    else
        ret = EIHNDL;

    KDEBUG(("xread(%d,%ld): rc=%ld\n",h,len,ret));

    return ret;
}


/*
 * ixgetfcb - get ptr to FCB in directory buffer
 *
 * returns NULL at end of buffer
 */
FCB *ixgetfcb(OFD *p)
{
    DMD *dm;
    UBYTE *buf;
    RECNO recnum;
    UWORD offset;

    /* get logical record number & byte offset within it */
    dm = p->o_dmd;
    recnum = p->o_curbyt >> dm->m_rblog;
    offset = p->o_curbyt & dm->m_rbm;

    /* handle start-of-record case */
    if (offset == 0)
    {
        if ((recnum == 0) || (recnum == (RECNO)dm->m_clsiz))
        {
            if (nextcl(p,0))    /* end of directory? */
                return NULL;
            recnum = 0;
        }
    }

    recnum += p->o_currec;

    buf = getrec(recnum,p,0);   /* get desired record  */
    addit(p,sizeof(FCB));       /* update OFD          */

    return (FCB *)(buf+offset);
}


/*
 * ixread -
 */

long ixread(OFD *p, long len, void *ubufr)
{
    long maxlen;

    /*
     * we used to disallow reads from a file opened as write-only,
     * but this is not compatible with Atari TOS ...
     */

    if (len > (maxlen = p->o_dfd->o_fileln - p->o_bytnum))
        len = maxlen;

    if (len > 0)
        return(xrw(0,p,len,ubufr));

    return(0L); /* zero bytes read for zero requested */
}



/*
 * xwrite - write 'len' bytes to handle 'h'.
 *
 * Function 0x40  f_write
 *
 * Error returns
 *   EIHNDL
 *   bios()
 */

long xwrite(int h, long len, void *ubufr)
{
    OFD *p;
    long ret;

    p = getofd(h);

    /*
     * we used to disallow writes to an existing file opened as read-only,
     * but this is not compatible with Atari TOS ...
     */

    if (p)
        ret = ixwrite(p,len,ubufr);
    else
        ret = EIHNDL;

    KDEBUG(("xwrite(%d,%ld): rc=%ld\n",h,len,ret));

    return ret;
}

/*
 *  ixwrite -
 */

long ixwrite(OFD *p, long len, void *ubufr)
{
    return(xrw(1,p,len,ubufr));
}
