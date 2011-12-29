/*
 * fsdrive.c - physical drive routines for file system 
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002 - 2010 The EmuTOS development team
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

#define DBGFSDRIVE 0

#include "config.h"
#include "portab.h"
#include "fs.h"
#include "bpb.h"
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

DND     *dirtbl[NCURDIR] ;

/*
 **     diruse - use count
 **     drvsel - mask of drives selected since power up
 */

char    diruse[NCURDIR] ;
LONG    drvsel ;


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
    int i;
    LONG mask;
    BPB *b;

#if DBGFSDRIVE
    kprintf("ckdrv(%i)\n", d);
#endif

    mask = 1L << d;

    if (!(mask & drvsel))
    {       /*  drive has not been selected yet  */
        b = (BPB *) Getbpb(d);

        if ( !(long)b )             /* M01.01.1007.01 */
            return(ERR);

        if ( (long)b < 0 ) /* M01.01.0915.02 */ /* M01.01.1007.01 */
            return( (long)b );

        if (log_media(b,d))
            return (ENSMEM);

        drvsel |= mask;
    }

    if ((!run->p_curdir[d]) || (!dirtbl[(int)(run->p_curdir[d])]))
    {       /* need to allocate current dir on this drv */

        for (i = 1; i < NCURDIR; i++)   /*      find unused slot    */
            if (!diruse[i])
                break;

        if (i == NCURDIR)                   /*  no slot available   */
            return( EINTRN ) ;      /*  M01.01.20           */

        diruse[i]++;                /*  mark as used        */
        dirtbl[i] = drvtbl[d]->m_dtl;   /*      link to DND         */
        run->p_curdir[d] = i;       /*  link to process     */
    }

    return(d);
}



/*
**      getdmd - allocate storage for and initialize a DMD
*/

DMD     *getdmd(int drv)
{
        DMD *dm;

#if DBGFSDRIVE
        kprintf("getdmd(%i)\n", drv);
#endif

        if (!(drvtbl[drv] = dm = MGET(DMD)))
                return ( (DMD *) 0 );

        if (!(dm->m_dtl = MGET(DND)))
                goto fredm;

        if (!(dm->m_dtl->d_ofd = MGET(OFD)))
                goto fredtl;

        if (!(dm->m_fatofd = MGET(OFD)))
                goto freofd;

        return(dm);

freofd: xmfreblk (dm->m_dtl->d_ofd);
fredtl: xmfreblk (dm->m_dtl);
fredm:  xmfreblk (dm);

        return ( (DMD *) 0 );
}


/*
 * log2 - return log base 2 of n
 */

static int log2(unsigned long n)
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

#if DBGFSDRIVE
        kprintf("log_media(%p, %i)\n", b, drv);
        kprintf("rsiz = 0x%lx, cs = 0x%lx, n = 0x%lx, fs = 0x%lx\n",
                rsiz, cs, n, fs);
#endif

        if (!(dm = getdmd(drv)))
                return (ENSMEM);

        d = dm->m_dtl;              /*  root DND for drive          */
        dm->m_fsiz = fs;                    /*  fat size                    */
        f = d->d_ofd;               /*  root dir file               */
        dm->m_drvnum = drv;         /*  drv nbr into media descr    */
        f->o_dmd = dm;              /*  link to OFD for rt dir file */

        d->d_drv = dm;              /*  link root DND with DMD      */
        d->d_name[0] = 0;                   /*  null out name of root       */

        dm->m_16 = b->b_flags & B_16;   /*      set 12 or 16 bit fat flag   */
        dm->m_clsiz = cs;                   /*  set cluster size in sectors */
        dm->m_clsizb = b->clsizb;           /*    and in bytes              */
        // kprintf("dm->m_clsizb = 0x%lx\n", (long)dm->m_clsizb);
        dm->m_recsiz = rsiz;        /*  set record (sector) size    */
        dm->m_numcl = b->numcl;     /*  set cluster size in records */
        dm->m_clrlog = log2(cs);            /*    and log of it             */
        dm->m_clrm = (1L<<dm->m_clrlog)-1;          /*  and mask of it      */
        dm->m_rblog = log2(rsiz);           /*  set log of bytes/record     */
        dm->m_rbm = (1L<<dm->m_rblog)-1;            /*  and mask of it      */
        dm->m_clblog = log2(dm->m_clsizb);          /*  log of bytes/clus   */

        f->o_fileln = n * rsiz;     /*  size of file (root dir)     */
        d->d_strtcl = f->o_strtcl = 2;      /*  root start pseudo-cluster   */

        fo = dm->m_fatofd;                  /*  OFD for 'fat file'          */
        fo->o_strtcl = 2;                   /*  FAT start pseudo-cluster    */
        fo->o_dmd = dm;             /*  link with DMD               */

        // kprintf("b->fatrec = 0x%x, fo->o_strtcl = 0x%x\n", b->fatrec, fo->o_strtcl);
        // kprintf("d->d_strtcl = 0x%x, b->datrec = 0x%x\n", d->d_strtcl, b->datrec);

        dm->m_recoff[BT_FAT] = (RECNO)b->fatrec;
        // kprintf("dm->m_recoff[0] = 0x%lx\n", dm->m_recoff[0]);
        dm->m_recoff[BT_ROOT] = (RECNO)b->fatrec + fs;
        // kprintf("dm->m_recoff[1] = 0x%lx\n", dm->m_recoff[1]);
        dm->m_recoff[BT_DATA] = (RECNO)b->datrec;
        // kprintf("dm->m_recoff[2] = 0x%lx\n", dm->m_recoff[2]);

        fo->o_bytnum = 3;
        fo->o_curbyt = 3;
        fo->o_fileln = fs * rsiz;

        if (b->fsiz == 0) {
#if DBGFSDRIVE
            kprintf("Warning: Trying to access a FAT32 partition?\n");
#endif
            return EDRIVE;
        }

        return (0L);
}
