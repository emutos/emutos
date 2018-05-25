/*
 * fsfat.c - fat mgmt routines for file system
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "asm.h"
#include "fs.h"
#include "gemerror.h"
#include "kprint.h"

/*
**  cl2rec -
**      M01.0.1.03
*/
RECNO cl2rec(CLNO cl, DMD *dm)
{
    return((RECNO)(cl-2) << dm->m_clrlog);
}


/*
**  clfix -
**      replace the contents of the fat entry indexed by 'cl' with the value
**      'link', which is the index of the next cluster in the chain.
**
**      M01.01.03
*/
void clfix(CLNO cl, CLNO link, DMD *dm)
{
    int spans;
    CLNO f, mask;
    LONG offset, recnum;
    char *buf;

    offset = dm->m_16 ? (LONG)cl << 1 : ((LONG)cl + (cl >> 1));
    recnum = offset >> dm->m_rblog;
    offset &= dm->m_rbm;

    /*
     * handle 16-bit FAT
     * easier because content is word-aligned and cannot span FAT sectors
     */
    if (dm->m_16)
    {
        buf = getrec(recnum,dm->m_fatofd,1);
        swpw(link);
        *(CLNO *)(buf+offset) = link;
        return;
    }

    /*
     * handle 12-bit FAT
     */
    if (IS_ODD(cl))
    {
        link = link << 4;
        mask = 0x000f;
    }
    else
    {
        link = link & 0x0fff;
        mask = 0xf000;
    }

    spans = (dm->m_recsiz-offset == 1); /* content spans FAT sectors ... */

    /* get current contents */
    buf = getrec(recnum,dm->m_fatofd,0) + offset;
    f = *(UBYTE *)buf++ << 8;
    if (spans)
        buf = getrec(recnum+1,dm->m_fatofd,0);
    f |= *(UBYTE *)buf;

    /* update */
    swpw(f);
    f = (f & mask) | link;
    swpw(f);

    /* write back */
    buf = getrec(recnum,dm->m_fatofd,1) + offset;
    *(UBYTE *)buf++ = f >> 8;
    if (spans)
        buf = getrec(recnum+1,dm->m_fatofd,1);
    *(UBYTE *)buf = LOBYTE(f);
}


/*
**  getrealcl -
**      get the contents of the fat entry indexed by 'cl'.
**
**  returns 16-bit value
**      for FAT12: 0xffff if entry contains the end of file marker
**                 otherwise, the contents of the entry
**      for FAT16: the contents of the entry
**
**      M01.0.1.03
*/
CLNO getrealcl(CLNO cl, DMD *dm)
{
    CLNO f;
    LONG offset, recnum;
    char *buf;

    offset = dm->m_16 ? (LONG)cl << 1 : ((LONG)cl + (cl >> 1));
    recnum = offset >> dm->m_rblog;
    offset &= dm->m_rbm;
    buf = getrec(recnum,dm->m_fatofd,0) + offset;

    /*
     * handle 16-bit FAT
     * easier because content is word-aligned and cannot span FAT sectors
     */
    if (dm->m_16)
    {
        f = *(CLNO *)buf;
        swpw(f);
        return f;
    }

    /*
     * handle 12-bit FATs
     */
    f = *(UBYTE *)buf++ << 8;
    if (dm->m_recsiz-offset == 1) /* content spans FAT sectors ... */
        buf = getrec(recnum+1,dm->m_fatofd,0);
    f |= *(UBYTE *)buf;

    swpw(f);

    if (IS_ODD(cl))
        cl = f >> 4;
    else
        cl = f & 0x0fff;

    if ((cl&0x0ff8) == 0x0ff8)  /* handle end of chain */
        cl = ENDOFCHAIN;

    return cl;
}


/*
**  getclnum -
**      get the next cluster number (including fake cluster number for FAT/root).
**
**  returns
**      for FAT/root sectors, we just add 1 to the input number.
**      otherwise we return the value returned by getrealcl().
**
*/
CLNO getclnum(CLNO cl, OFD *of)
{
    if (!of->o_dnode)           /* if no dir node, must be FAT/root */
        return cl+1;

    return getrealcl(cl,of->o_dmd);
}


/*
 * findfree16 - fast scan of FAT16 filesystem to find first free cluster
 *
 * returns cluster number, or 0 if no free clusters
 */
static CLNO findfree16(DMD *dm)
{
    int recnum, offset;
    CLNO clnum;
    char *buf;

    for (clnum = 2; clnum < dm->m_numcl+2; )
    {
        /*
         * get the next FAT record
         */
        recnum = (clnum * sizeof(CLNO)) >> dm->m_rblog;
        offset = (clnum * sizeof(CLNO)) & dm->m_rbm;
        buf = getrec(recnum, dm->m_fatofd, 0);

        /*
         * scan the FAT record, looking for a free slot
         */
        for ( ; (offset < dm->m_recsiz) && (clnum < (dm->m_numcl+2)); offset += sizeof(CLNO), clnum++)
        {
            if (*(CLNO *)(buf+offset) == 0)
                return clnum;
        }
    }

    return 0;
}


/*
 * findfree - scan filesystem to find next free cluster
 *
 * returns cluster number, or 0 if no free clusters
 */
static CLNO findfree(CLNO cl, DMD *dm)
{
    CLNO i;

    /*
     * fast scan for first free cluster on FAT16 filesystem
     */
    if ((cl == 0) && dm->m_16)
        return findfree16(dm);

    /*
     * handle FAT12 filesystems, or subsequent scans on FAT16
     *
     * the following code carefully avoids allowing overflow in CLNO variables
     */
    if (cl < 2)
        cl = 2;     /* start search at first or current cluster */
    for (i = 0; i < dm->m_numcl; i++, cl++) /* look at every cluster once */
    {
        if (!getrealcl(cl,dm))      /* check for empty cluster */
            return cl;
        if (cl == dm->m_numcl+1)    /* wrap at max cluster num */
            cl = 1;
    }

    return 0;
}


/*
**  nextcl -
**      get the cluster number which follows the cluster indicated in the curcl
**      field of the OFD, and place it in the OFD.
**
**  returns
**      E_OK    if success,
**      -1      if error
**
*/
int nextcl(OFD *p, int wrtflg)
{
    DMD     *dm;
    CLNO    cl, cl2;                                /*  M01.01.03   */

    cl = p->o_curcl;
    dm = p->o_dmd;

    if (cl == 0)                /* initial value */
    {
        cl2 = (p->o_strtcl ? p->o_strtcl : ENDOFCHAIN );
    }
    else if (!p->o_dnode)       /* if no dir node, must be FAT/root */
    {
        cl2 = cl + 1;
    }
    else
    {
        cl2 = getrealcl(cl,dm);
    }

    if (wrtflg && endofchain(cl2))  /* end of file, allocate new clusters */
    {
        cl2 = findfree(cl,dm);
        if (cl2 == 0)
            return -1;

        clfix(cl2,ENDOFCHAIN,dm);
        if (cl)
            clfix(cl,cl2,dm);
        else
        {
            p->o_strtcl = cl2;
            p->o_flag |= O_DIRTY;
        }
    }

    if (endofchain(cl2))
        return -1;

    p->o_curcl = cl2;
    p->o_currec = cl2rec(cl2,dm);
    p->o_curbyt = 0;

    return E_OK;
}


/*
 * countfree16 - fast scan of FAT16 filesystem to count free clusters
 */
static CLNO countfree16(DMD *dm)
{
    int recnum, offset;
    CLNO free, clnum;
    char *buf;

    for (clnum = 2, free = 0; clnum < dm->m_numcl+2; )
    {
        /*
         * get the next FAT record
         */
        recnum = (clnum * sizeof(CLNO)) >> dm->m_rblog;
        offset = (clnum * sizeof(CLNO)) & dm->m_rbm;
        buf = getrec(recnum, dm->m_fatofd, 0);

        /*
         * scan the FAT record, counting free slots
         */
        for ( ; (offset < dm->m_recsiz) && (clnum < (dm->m_numcl+2)); offset += sizeof(CLNO), clnum++)
        {
            if (*(CLNO *)(buf+offset) == 0)
                free++;
        }
    }

    return free;
}


/*      Function 0x36   d_free
                get disk free space data into buffer *
        Error returns
                ERR

        The code is optimised for 16-bit FATs.  The 12-bit case is more
        complex, since the entry for a cluster can span logical records,
        and therefore we do it the old, slow way.
*/
long xgetfree(long *buf, int drv)
{
    CLNO i, free;
    long n;
    DMD *dm;

    drv = (drv ? drv-1 : run->p_curdrv);

    if ((n = ckdrv(drv, TRUE)) < 0)
        return ERR;

    dm = drvtbl[n];
    if (dm->m_16)
    {
        free = countfree16(dm);
    }
    else
    {
        free = 0;
        for (i = 0; i < dm->m_numcl; i++)
            if (!getrealcl(i+2,dm))     /* cluster numbers start at 2 */
                free++;
    }

    *buf++ = (long)(free);
    *buf++ = (long)(dm->m_numcl);
    *buf++ = (long)(dm->m_recsiz);
    *buf = (long)(dm->m_clsiz);

    return E_OK;
}
