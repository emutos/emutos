/*
 * fsio.c - read/write routines for the file system
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002-2016 The EmuTOS development team
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
#include "string.h"
#include "kprint.h"


/*
 * forward prototypes
 */

static void addit(OFD *p, long siz, int flg);
static long xrw(int wrtflg, OFD *p, long len, char *ubufr);
static void usrio(int rwflg, int num, long strt, char *ubuf, DMD *dm);


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

long    xlseek(long n, int h, int flg)
{
    OFD *f;

    f = getofd(h);
    if ( !f )
        return(EIHNDL);

    if (flg == 2)
        n += f->o_fileln;
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

    if ((n < 0) || (n > p->o_fileln))
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
        clx = p->o_strtcl;

    /*
     * note: if we're seeking to a position which is at a cluster boundary,
     * we actually point to the cluster before that.  this unobvious action
     * is because, when the read point is at the start of a cluster, xrw()
     * starts its processing by handling whole clusters.  this occurs in
     * either the middle or tail section processing, but in both cases,
     * xrw() always chains to the next cluster before doing the actual read.
     *
     * see the code in xrw() if you need to know more ...
     */
    if ((n&dm->m_clbm) == 0)    /* go one less if on cluster boundary */
        clnum--;

    for (i = 0; i < clnum; i++) {
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

long    xread(int h, long len, void *ubufr)
{
    OFD *p;
    long ret;

    p = getofd(h);
    if ( p )
        ret = ixread(p,len,ubufr);
    else
        ret = EIHNDL;

    KDEBUG(("xread(%d,%ld) => %ld\n",h,len,ret));

    return ret;
}

/*
 * ixread -
 */

long    ixread(OFD *p, long len, void *ubufr)
{
    long maxlen;

    /*
     * we used to disallow reads from a file opened as write-only,
     * but this is not compatible with Atari TOS ...
     */

    if (len > (maxlen = p->o_fileln - p->o_bytnum))
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

long    xwrite(int h, long len, void *ubufr)
{
    OFD *p;
    long ret;

    p = getofd(h);

    /*
     * we used to disallow writes to an existing file opened as read-only,
     * but this is not compatible with Atari TOS ...
     */

    if ( p ) {
        ret = ixwrite(p,len,ubufr);
    } else {
        ret = EIHNDL;
    }

    KDEBUG(("xwrite(%d,%ld) => %ld\n",h,len,ret));

    return ret;
}

/*
 *  ixwrite -
 */

long    ixwrite(OFD *p, long len, void *ubufr)
{
    return(xrw(1,p,len,ubufr));
}

/*
 * addit - update the OFD for the file
 *
 * update the OFD for the file to reflect the fact that 'siz' bytes
 * have been written to it.
 *
 * flg: update curbyt ? (yes if less than 1 cluster transferred)
 */

static void addit(OFD *p, long siz, int flg)
{
    p->o_bytnum += siz;

    if (flg)
        p->o_curbyt += siz;

    if (p->o_bytnum > p->o_fileln)
    {
        p->o_fileln = p->o_bytnum;
        p->o_flag |= O_DIRTY;
    }
}



/*
 * xrw -
 *
 * This has two (related) functions:
 * 1. transfer 'len' bytes between the file indicated by the OFD and the
 *    buffer pointed to by 'ubufr'
 * 2. if 'ubufr' is NULL, return a pointer to the current file position
 *    within an internal buffer.  this is used internally within fsdir.c
 *    and fsopnclo.c to request ixread() [which calls xrw()] to return a
 *    pointer to a directory entry.
 *
 * We wish to do the i/o in whole clusters as much as possible.
 * Therefore, we break the i/o up into 5 sections.  Data which occupies
 * part of a logical sector along with data not in the request (both at
 * the start and the end of the request) are handled separately and are
 * called header (tail) bytes.  Data which are contained complete in
 * sectors but share part of a cluster with data not in the request are
 * called header (tail) records.  These are also handled separately.  In
 * between handling of header and tail sections, we do i/o in terms of
 * whole clusters.
 *
 *  returns
 *      1. nbr of bytes read/written from/to the file, or
 *      2. pointer (see above)
 */

static long xrw(int wrtflg, OFD *p, long len, char *ubufr)
{
    DMD *dm;
    char *bufp;
    unsigned int bytn, tailrec;
    int lenxfr, lentail;
    RECNO recn, num;
    int hdrrec, lsiz;
    RECNO last, nrecs;                  /* multi-sector variables */
    int lflg;
    long nbyts;
    long rc,bytpos,lenrec,lenmid;

    /* determine where we currently are in the file */

    dm = p->o_dmd;                      /*  get drive media descriptor  */

    bytpos = p->o_bytnum;               /*  starting file position      */

    /*
     * get logical record number to start i/o with
     * (bytn will be byte offset into sector # recn)
     */

    recn = p->o_curbyt >> dm->m_rblog;
    bytn = p->o_curbyt & dm->m_rbm;

    recn += p->o_currec;

    /* determine "header" of request. */

    if (bytn) /* do header */
    {
        /* xfer len is min( #bytes req'd , */
        /* #bytes left in current record ) */

        lenxfr = min(len,dm->m_recsiz-bytn);
        bufp = getrec(recn,p,wrtflg);   /* get desired record  */
        addit(p,(long) lenxfr,1);       /* update ofd          */
        len -= lenxfr;                  /* nbr left to do      */
        recn++;                         /* starting w/ next    */

        if (!ubufr)
            return (long) (bufp+bytn);

        if (wrtflg)
            memcpy(bufp+bytn,ubufr,lenxfr);
        else memcpy(ubufr,bufp+bytn,lenxfr);

        ubufr += lenxfr;
    }

    /* "header" complete.      See if there is a "tail". */
    /* After that, see if there is anything left in the middle. */

    lentail = len & dm->m_rbm;

    lenmid = len - lentail;             /*  Is there a Middle ? */
    if ( lenmid )
    {
        hdrrec = recn & dm->m_clrm;

        if (hdrrec)
        {
            /*  if hdrrec != 0, then we do not start on a clus bndy;
             *  so determine the min of (the nbr sects
             *  remaining in the current cluster) and (the nbr
             *  of sects remaining in the file).  This will be
             *  the number of header records to read/write.
             */

            hdrrec = ( dm->m_clsiz - hdrrec ) ; /* M00.14.01 */
            if ( hdrrec > lenmid >> dm->m_rblog )       /* M00.14.01 */
                hdrrec = lenmid >> dm->m_rblog; /* M00.14.01 */

            usrio(wrtflg,hdrrec,recn,ubufr,dm);
            ubufr += (lsiz = hdrrec << dm->m_rblog);
            lenmid -= lsiz;
            addit(p,(long) lsiz,1);
        }

        /* do whole clusters */

        lenrec = lenmid >> dm->m_rblog;            /* nbr of records  */

        num = lenrec >> dm->m_clrlog;
        tailrec = lenrec & dm->m_clrm;

        last = nrecs = 0L;
        nbyts = lflg = 0;

        while (num--)           /*  for each whole cluster...   */
        {
            rc = nextcl(p,wrtflg);

            /*
             *  if eof or non-contiguous cluster, or last cluster
             *  of request, then finish pending I/O
             */

            if ((!rc) && (p->o_currec == last + nrecs))
            {
                nrecs += dm->m_clsiz;
                nbyts += dm->m_clsizb;
                if (!num) goto mulio;
            }
            else
            {
                if (!num)
                    lflg = 1;
mulio:
                if (nrecs)
                    usrio(wrtflg,nrecs,last,ubufr,dm);
                ubufr += nbyts;
                addit(p,nbyts,0);
                if (rc)
                    goto eof;
                last = p->o_currec;
                nrecs = dm->m_clsiz;
                nbyts = dm->m_clsizb;
                if ((!num) && lflg)
                {
                    lflg = 0;
                    goto mulio;
                }
            }
        }  /*  end while  */

        /* do "tail" records */

        if (tailrec)
        {
            if (nextcl(p,wrtflg))
                goto eof;
            lsiz = tailrec << dm->m_rblog;
            addit(p,(long) lsiz,1);
            usrio(wrtflg,tailrec,p->o_currec,ubufr,dm);
            ubufr += lsiz;
        }
    }

    /* do tail bytes within this cluster */

    if (lentail)
    {
        recn = p->o_curbyt >> dm->m_rblog;
        bytn = p->o_curbyt & dm->m_rbm;

        if ((!recn) || (recn == (RECNO)dm->m_clsiz))
        {
            if (nextcl(p,wrtflg))
                goto eof;
            recn = 0;
        }

        bufp = getrec((RECNO)p->o_currec+recn,p,wrtflg);
        addit(p,(long) lentail,1);

        if (!ubufr)
            return (long) bufp;

        if (wrtflg)
             memcpy(bufp,ubufr,lentail);
        else memcpy(ubufr,bufp,lentail);
    } /*  end tail bytes  */

eof:
    rc = p->o_bytnum - bytpos;

    return(rc);
}

/*
 * usrio -
 *
 * NOTE: longjmp_rwabs() is a macro that includes a longjmp() which is
 *       executed if the BIOS returns an error, therefore usrio() does
 *       not need to return any error codes.
 */

static void usrio(int rwflg, int num, long strt, char *ubuf, DMD *dm)
{
    BCB *b;

    for (b = bufl[BI_DATA]; b; b = b->b_link)
        if ((b->b_bufdrv == dm->m_drvnum) &&
            (b->b_bufrec >= strt) &&
            (b->b_bufrec < strt+num))
            flush(b);

    longjmp_rwabs(rwflg, (long)ubuf, num, strt+dm->m_recoff[BT_DATA], dm->m_drvnum);
}
