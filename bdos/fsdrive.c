/*
 * fsdrive.c - physical drive routines for file system
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 **       date      who     comment
 **     ---------   ---     -------
 **     21 Mar 86   ktb     M01.01.20 - ckdrv() returns EINTRN if no slots
 **                         avail in dirtbl.
 **
 **     15 Sep 86   scc     M01.01.0915.02  ckdrv() now checks for negative error
 **                                         return from BIOS
 **
 **      7 Oct 86   scc     M01.01.1007.01  cast several pointers to longs when
 **                         they are compared to 0.
 **
 **     31 Oct 86   scc     M01.01.1031.01  removed definition of drvmap.  It was used on
 **                         the assumption that the drive map would not change after boot
 **                         time, which is not the case in BNR land.  Also removed
 **                         ValidDrv(), which checked it.  Corresponding change made in
 **                         xgetdir() in FSDIR.C
 **
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "fs.h"
#include "biosdefs.h"
#include "mem.h"
#include "gemerror.h"
#include "biosbind.h"
#include "kprint.h"


/*
 **     globals
 */

/*
 **     dirtbl - default directories.
 **         Each entry points to the DND for someone's default directory.
 **         They are linked to each process by the p_curdir entry in the PD.
 **         The first entry (dirtbl[0]) is not used, as p_curdir[i]==0
 **         means 'none set yet'.
 */

DIRTBL_ENTRY dirtbl[NCURDIR];

/*
 **     drvsel - mask of drives selected since power up
 **     drvrem - mask of drives with removable media
 */

LONG    drvsel ;
LONG    drvrem;


/*
 *  ckdrv - check the drive, see if it needs to be logged in.
 *
 *  Arguments:
 *    d - has this drive been accessed, or had a media change
 *
 *
 *      returns:
 *          ERR     if getbpb() failed
 *          ENSMEM  if log() failed
 *          EINTRN  if no room in dirtbl
 *          drive nbr if success.
 */

long    ckdrv(int d)
{
    int curdir;
    LONG mask;
    BPB *b;

    KDEBUG(("ckdrv(%i)\n",d));

    mask = 1L << d;

    if (!(mask & drvsel))
    {       /*  drive has not been selected yet  */
        b = (BPB *) Getbpb(d);

        if ( !(long)b )             /* M01.01.1007.01 */
            return EDRIVE;

        if ( (long)b < 0 ) /* M01.01.0915.02 */ /* M01.01.1007.01 */
            return( (long)b );

        if (log_media(b,d))
            return (ENSMEM);

        drvsel |= mask;
    }
    else if (mask & drvrem)     /* handle removable media */
    {
        if (Mediach(d))
        {
            errdrv = d;
            rwerr = E_CHNG;         /* signal media change */
            errcode = rwerr;
            longjmp(errbuf,1);
        }
    }

    /*
     * ensure that current process has a valid default directory
     * on this drive
     */
    curdir = run->p_curdir[d];
    if (curdir >= NCURDIR)      /* validate */
        curdir = 0;             /* if invalid, say none */
    if (!curdir                 /* no current dir for this drive */
     || !dirtbl[curdir].dnd)    /* or current dir is invalid  */
    {
        curdir = incr_curdir_usage(drvtbl[d]->m_dtl);   /* add root DND */
        if (curdir < 0)
            return ENSMEM;
        run->p_curdir[d] = curdir;  /* link to process  */
    }

    return(d);
}



/*
**      getdmd - allocate storage for and initialize a DMD
*/

static DMD *getdmd(int drv)
{
        DMD *dm;

        KDEBUG(("getdmd(%i)\n",drv));

        if (!(drvtbl[drv] = dm = MGET(DMD)))
                return ( (DMD *) 0 );

        /*
         * no need to check the following, since MGET(DND)
         * and MGET(OFD) only return if they succeed
         */
        dm->m_dtl = MGET(DND);
        dm->m_dtl->d_ofd = MGET(OFD);
        dm->m_fatofd = MGET(OFD);

        return(dm);
}


/*
 * log2ul - return log base 2 of n
 */

static int log2ul(unsigned long n)
{
    int i;

    for (i = 0; n ; i++) {
        n >>= 1;
    }

    return(i-1);
}


/*
**      log_media -
**          log in media 'b' on drive 'drv'.
**
*/

/* b: bios parm block for drive
 * drv: drive number
 */

long    log_media(BPB *b, int drv)
{
        OFD *fo,*f;                         /*  M01.01.03   */
        DND *d;
        DMD *dm;
        unsigned long rsiz,cs,n,fs;

        rsiz = b->recsiz;
        cs = b->clsiz;
        n = b->rdlen;
        fs = b->fsiz;

        KDEBUG(("log_media(%p,%i) rsiz=0x%lx, cs=0x%lx, n=0x%lx, fs=0x%lx\n",
                b,drv,rsiz,cs,n,fs));

        if (fs == 0) {
            KDEBUG(("Warning: Trying to access a FAT32 partition?\n"));
            return EDRIVE;
        }

        if (!(dm = getdmd(drv)))
                return (ENSMEM);

        d = dm->m_dtl;              /*  root DND for drive          */
        dm->m_fsiz = fs;            /*  fat size                    */
        f = d->d_ofd;               /*  root dir file               */
        dm->m_drvnum = drv;         /*  drv nbr into media descr    */
        f->o_dmd = dm;              /*  link to OFD for rt dir file */

        d->d_drv = dm;              /*  link root DND with DMD      */
        d->d_name[0] = 0;           /*  null out name of root       */

        dm->m_16 = b->b_flags & B_16;       /*  set 12 or 16 bit fat flag   */
        dm->m_clsiz = cs;                   /*  set cluster size in sectors */
        dm->m_clsizb = b->clsizb;           /*    and in bytes              */
        dm->m_recsiz = rsiz;                /*  set record (sector) size    */
        dm->m_numcl = b->numcl;             /*  set cluster size in records */
        dm->m_clrlog = log2ul(cs);          /*    and log of it             */
        dm->m_clrm = (1L<<dm->m_clrlog)-1;  /*      and mask of it          */
        dm->m_rblog = log2ul(rsiz);         /*  set log of bytes/record     */
        dm->m_rbm = (1L<<dm->m_rblog)-1;    /*    and mask of it            */
        dm->m_clblog = log2ul(dm->m_clsizb);/*  log of bytes/clus           */
        dm->m_clbm = (1L<<dm->m_clblog)-1;  /*    and mask of it            */

        f->o_fileln = n * rsiz;             /*  size of file (root dir)     */
        d->d_strtcl = f->o_strtcl = 2;      /*  root start pseudo-cluster   */

        fo = dm->m_fatofd;                  /*  OFD for 'fat file'          */
        fo->o_strtcl = 2;                   /*  FAT start pseudo-cluster    */
        fo->o_dmd = dm;                     /*  link with DMD               */

        dm->m_recoff[BT_FAT] = (RECNO)b->fatrec;
        dm->m_recoff[BT_ROOT] = (RECNO)b->fatrec + fs;
        dm->m_recoff[BT_DATA] = (RECNO)b->datrec;

        KDEBUG(("log_media(%i) dm->m_recoff[0-2] = 0x%lx/0x%lx/0x%lx\n",
                drv, dm->m_recoff[0],dm->m_recoff[1],dm->m_recoff[2]));

        fo->o_fileln = fs * rsiz;

        return (0L);
}
