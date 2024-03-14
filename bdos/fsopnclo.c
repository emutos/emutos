/*
 * fsopnclo.c - open/close/create/delete routines for file system
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2024 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
** NOTE:
**      mods with "SCC.XX.NN" are mods which try to merge fixes to a special
**      post 1.0 / pre 1.1 version.  The notation refers to a DRI internal
**      document (see SCC), which is a change log.  SCC refers to the
**      originator of the fix.  The XX refers to the module in which the
**      fix was originally made, fs.c (FS), sup.c (SUP), etc.  The NN is
**      the fix number to that module as indicated on the change log.  For
**      the most part, these numbers are meaningless, and serve only to
**      correspond code to particular problems.
**
**  mods
**     date     who mod                 fix/change/note
**  ----------- --  ------------------  -------------------------------
**  06 May 1986 ktb M01.01.SCC.FS.02    ixcreat(): rescan.
**  21 Jul 1986 ktb M01.01.0721.01      ixcreat(): check for bad chars
**  30 Jul 1986 ktb M01.01.0730.01      deleting entries from the sft had
**                                      problems if there were dup'd entries
**                                      pointing to the same OFD
**  15 Sep 1986 scc M01.01.0915.01      ixcreat(): disallow creation of subdir
**                                      if file by same name exists
**  22 Oct 1986 scc M01.01.1022.01      xclose(): range check the handle coming in
**  23 Oct 1986 scc M01.01.1023.01      xclose(): check for closing a standard handle
**                                      that was already closed
**  23 Oct 1986 scc M01.01.1023.03      sftdel() and sftosrch() erroneously used NULL
**                                      rather than NULLPTR.
**
**  12 Dec 1986 scc M01.01.1212.01      modified ixcreat(), ixopen(), and xunlink() to
**                                      check for a negative error return from findit().
**
**  14 Dec 1986 scc M01.01.1214.01      Further modification of M01.01.1212.01 to check
**                                      for 0 return (indicating BDOS level error).
*/

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "asm.h"
#include "fs.h"
#include "gemerror.h"
#include "string.h"
#include "mem.h"
#include "time.h"
#include "console.h"
#include "bdosstub.h"
#include "tosvars.h"

/* the following characters are disallowed in the name when creating
 * or renaming files or folders.  this is *mostly* the same list as
 * for MS-DOS, except that Atari allows the '+' character.
 */
#define ILLEGAL_FNAME_CHARACTERS " *,:;<=>?[]|"


/*
 * forward prototypes
 */
static long ixopen(char *name, int mod);
static long opnfil(FCB *f, DND *dn, int mod);


/*
 *  xcreat - create file with specified name, attributes
 *
 *  Function 0x3C   Fcreate
 *
 *  Error returns   EPTHNF, EACCDN, ENHNDL
 */
long xcreat(char *name, UBYTE attr)
{
    long rc;

    rc = ixcreat(name, attr & ~FA_SUBDIR);

    KDEBUG(("xcreat(%s,0x%02x): rc=%ld\n",name,attr,rc));

    return rc;
}


/*
**  ixcreat - internal routine for creating files
*/
/*  name: path name of file
 *  attr: attributes
 */
long ixcreat(char *name, UBYTE attr)
{
    DND *dn;
    OFD *fd;
    FCB *fcb;
    const char *s;
    char n[2], a[FNAMELEN];                 /*  M01.01.03   */
    int i, f2;                              /*  M01.01.03   */
    long pos, rc;

    n[0] = ERASE_MARKER; n[1] = 0;

    /* first find path */

    dn = findit(name,&s,0);
    if (!dn)                                /* M01.01.1214.01 */
        return EPTHNF;

    if (!*s || (*s == '.'))         /*  no file name || '.' || '..' */
        return EPTHNF;

    /*  M01.01.0721.01  */
    if (contains_illegal_characters(s))
        return EACCDN;

    /*
     * if the volume label attribute is set, no others are allowed
     */
    if ((attr&FA_VOL) && (attr != FA_VOL))
        return EACCDN;

    /*
     * volume labels may only be created in the root
     */
    if ((attr == FA_VOL) && dn->d_parent)
        return EACCDN;

    if (!(fd = dn->d_ofd))
        fd = makofd(dn);            /* makofd() also updates dn->d_ofd */

    /*
     * if a matching file already exists, we delete it first.  note
     * that the definition of matching, and the action taken, differs
     * depending on whether the file is a volume label or not:
     *  . for a volume label, *any* existing volume label matches
     *    and will be deleted (reference: Rainbow TOS Release Notes)
     *  . for other files, the name must match, and the existing
     *    file will be deleted unless (a) it's read-only or a folder
     *    or (b) the file being created is a folder.
     */
    pos = 0;
    if (attr == FA_VOL)
        fcb = scan(dn,"*.*",FA_VOL,&pos);
    else fcb = scan(dn,s,FA_NORM|FA_SUBDIR,&pos);

    if (fcb)                /* found matching file / label */
    {
        if (attr != FA_VOL) /* for normal files, need to check more stuff */
        {
                                        /* M01.01.0730.01   */
            if ((fcb->f_attrib & (FA_SUBDIR | FA_RO)) || (attr == FA_SUBDIR))
                return EACCDN;          /*  subdir or read only  */
        }
        pos -= sizeof(FCB);
        if (ixdel(dn,fcb,pos) < 0)  /* file currently open by another process? */
            return EACCDN;
    }
    else
        pos = 0;

    /* now scan for empty space */

    /*  M01.01.SCC.FS.02  */
    while( !( fcb = scan(dn,n,0xff,&pos) ) )
    {
        /*  not in current dir, need to grow  */
        if (!fd->o_dnode)           /*  but can't grow root  */
            return EACCDN;

        if ( nextcl(fd,1) )
            return EACCDN;

        dirinit(dn);
        pos = 0;
    }

    builds(s,a);
    pos -= sizeof(FCB);
    fcb->f_attrib = attr;
    for (i = 0; i < 10; i++)
        fcb->f_fill[i] = 0;
    fcb->f_td.time = current_time;
    swpw(fcb->f_td.time);
    fcb->f_td.date = current_date;
    swpw(fcb->f_td.date);
    fcb->f_clust = 0;
    fcb->f_fileln = 0;
    ixlseek(fd,pos);
    ixwrite(fd,FNAMELEN,a);         /* write name, set dirty flag */
    ixclose(fd,CL_DIR);             /* partial close to flush */
    ixlseek(fd,pos);
    s = (char *)ixgetfcb(fd);
    f2 = rc = opnfil((FCB*)s,dn,(fcb->f_attrib&FA_RO)?RO_MODE:RW_MODE);

    if (rc < 0)
        return rc;

    getofd(f2)->o_dfd->o_flag |= O_DIRTY;

    return f2;
}


/*
 *  xopen - open a file (path name)
 *
 *  Function 0x3D   Fopen
 *
 *  Error returns   EFILNF, opnfil()
 *
 *  +ve return      file handle
 */
long xopen(char *name, int mod)
{
    long rc;

    rc = ixopen(name, mod&VALID_FOPEN_BITS);

    KDEBUG(("xopen(%s,%d): rc=%ld\n",name,mod,rc));

    return rc;
}

/*
**  ixopen - open a file
**
**  returns
**      <0 = error
**      >0 = file handle
*/
static long ixopen(char *name, int mod)
{
    FCB *fcb;
    DND *dn;
    const char *s;
    long pos;

    /* first find path */
    dn = findit(name,&s,0);
    if (!dn)                                        /* M01.01.1214.01 */
        return EFILNF;

    /*
     **  now scan the directory file for a matching filename
     */

    pos = 0;
    if (!(fcb = scan(dn,s,FA_NORM,&pos)))
        return EFILNF;

    /* Check to see if the file is read only */
    if ((fcb->f_attrib & FA_RO) && (mod != 0))
        return EACCDN;

    return opnfil(fcb, dn, mod);
}


/*
**  makopn - make an open file for sft handle h
**
*/
static long makopn(FCB *f, DND *dn, int h, int mod)
{
    OFD *p;
    OFD *p2;
    DFD *dfd;
    DMD *dm;                        /*  M01.01.03   */

    dm = dn->d_drv;

    p = MGET(OFD);                  /* MGET(OFD) only returns if it succeeds */

    p->o_mod = mod;                 /*  set mode                    */
    p->o_dmd = dm;                  /*  link OFD to media           */
    sft[h-NUMSTD].f_ofd = p;
    /* no need to zero o_curcl & o_curbyt, since MGET zeroes the OFD */
    p->o_dnode = dn;                /*  link to directory           */
    p->o_dirfil = dn->d_ofd;        /*  link to dir's OFD           */
    p->o_dirbyt = dn->d_ofd->o_bytnum - sizeof(FCB);    /* offset of fcb in dir */

    for (p2 = dn->d_files; p2; p2 = p2->o_link)
        if (p2->o_dirbyt == p->o_dirbyt)
            break;              /* same dir, same dcnt */

    p->o_link = dn->d_files;
    dn->d_files = p;

    /*
     * if this file is already open, we copy the DFD pointer; this
     * ensures that that all OFDs for the same file use the same DFD.
     * otherwise, we use the DFD in the current OFD.
     */
    if (p2)
    {
        dfd = p2->o_dfd;
        dfd->o_usecnt++;                /* more than one user of DFD! */
        /* not used yet... TBA *********/
        p2->o_thread = p;
    }
    else
    {
        dfd = &p->o_disk;
        dfd->o_usecnt = 1;              /* only OFD using this DFD */
        dfd->o_td.date = f->f_td.date;  /* note: OFD time/date are  */
        dfd->o_td.time = f->f_td.time;  /*  actually little-endian! */
        dfd->o_strtcl = f->f_clust;     /* 1st cluster of file */
        swpw(dfd->o_strtcl);
        dfd->o_fileln = f->f_fileln;    /* init length of file */
        swpl(dfd->o_fileln);
    }

    p->o_dfd = dfd;                     /* for future reference ... */

    return h;
}


/*
**  opnfil - does the real work in opening a file
**
**  Error returns   ENHNDL
**
**  NOTES:
**          make a pointer to the ith entry of sft
*/
static long opnfil(FCB *f, DND *dn, int mod)
{
    int i;
    int h;

    /* find free sft handle */
    for (i = 0; i < OPNFILES; i++)
        if( !sft[i].f_own )
            break;

    if (i == OPNFILES)
        return ENHNDL;

    sft[i].f_own = run;
    sft[i].f_use = 1;
    h = i + NUMSTD;

    return makopn(f, dn, h, mod);
}


/*
**  sftofdsrch - search the sft for an entry with the specified OFD
**  returns:
**      ptr to the matching sft, or
**      NULL
*/
static FTAB *sftofdsrch(OFD *ofd)
{
    FTAB *sftp;     /* scan ptr for sft */
    int i;

    for (i = 0, sftp = sft; i < OPNFILES; i++, sftp++)
        if (sftp->f_ofd == ofd)
            return sftp;

    return NULL;
}


/*
**  sftdel - delete an entry from the sft
**      delete the entry from the sft.  If no other entries in the sft
**      have the same OFD, free up the OFD, also.
*/
static void sftdel(FTAB *sftp)
{
    OFD *ofd;
    DFD *d;

    ofd = sftp->f_ofd;

    /*  clear out the entry  */
    sftp->f_ofd = NULL;
    sftp->f_own = NULL;
    sftp->f_use = 0;

    /*
     * if there are no other sft entries with same OFD, delete the OFD
     * (subject to the complication of multiple OFDs pointing to the same file)
     */
    if (sftofdsrch(ofd) == NULL)
    {
        d = ofd->o_dfd;
        if (d->o_usecnt > 0)        /* paranoia */
            d->o_usecnt--;

        if (d != &ofd->o_disk)      /* not the 'base OFD', */
            xmfreblk(ofd);          /*  so OK to delete it */

        if (d->o_usecnt == 0)       /* no more users of this file */
        {
            ofd = (OFD *)((char *)d - offsetof(OFD, o_disk));
            xmfreblk(ofd);          /* delete the 'base OFD' */
        }
    }
}


/*
 *  xclose - Close a file.
 *
 *  Function 0x3E   Fclose
 *
 *  Error returns   EIHNDL, ixclose()
 *
 *  SCC:    I have added 'rc' to allow return of status from ixclose().  I
 *          do not yet know whether it is appropriate to perform the
 *          operations inside the 'if' statement following the invocation
 *          of ixclose(), but I am leaving the flow of control intact.
 */
long xclose(int h)
{
    FTAB *ftab;
    OFD *fd;
    long rc;

    if (h < 0)
        return E_OK;    /* always a good close on a character device */

    if (h >= NUMHANDLES)            /* M01.01.1022.01 */
        return EIHNDL;

    /*
     * for a standard handle:
     *  . revert it to the default character device
     *  . if the original was mapped to a character device, we're done
     *  . otherwise it was Fforce'd: as long as it wasn't mapped to
     *    another standard handle (which would be illegal), we continue
     *    on to close the non-standard handle
     *
     * for a non-standard handle:
     *  . if it's mapped to a character device, decrement the usage, then we're done
     *  . otherwise we continue on to close the non-standard handle
     */
    if (h < NUMSTD)
    {
        int h0 = run->p_uft[h];     /* remember old mapping */
        run->p_uft[h] = get_default_handle(h);  /* revert to default */
        h = h0;
        if (h < 0)                  /* M01.01.1023.01 */
            return E_OK;
        if (h < NUMSTD)             /* "can't happen" (bug in Fforce()?) */
            return EIHNDL;
    }
    else
    {
        ftab = &sft[h-NUMSTD];

        if ((long)ftab->f_ofd < 0L)
        {
            if (--ftab->f_use == 0)
            {
                ftab->f_ofd = 0;
                ftab->f_own = 0;
            }
            return E_OK;
        }
    }

    /*
     * we have a non-standard handle:
     *  . check that it's valid
     *  . if so, do a real close, updating the OFD, directory etc and
     *    flushing buffers
     */
    if (!(fd = getofd(h)))
        return EIHNDL;

    rc = ixclose(fd,0);

    /*
     * finally, decrement the usage and, if it is now zero, zero out the
     * sft[] entry and, if there are no other entries with a pointer to
     * the OFD for this handle, free up the OFD
     */
    ftab = &sft[h-NUMSTD];
    if (--ftab->f_use == 0)
        sftdel(ftab);

    return rc;
}


/*
**  ixclose -
**
**  Error returns   EINTRN
**
**  Last modified   SCC     10 Apr 85
**
**  NOTE:   I'm not sure that returning immediately upon an error from
**          ixlseek() is the right thing to do.  Some data structures may
**          not be updated correctly.  Watch out for this!
**          Also, I'm not sure that the EINTRN return is ok.
*/
long ixclose(OFD *fd, int part)
{                                   /*  M01.01.03                   */
    OFD *p, **q;
    int i;                          /*  M01.01.03                   */
    BCB *b;
    DFD *dfd = fd->o_dfd;

    /*
     * if the file or folder has been modified, we need to make sure
     * that the date/time, starting cluster, and file length in the
     * directory entry are updated.  In addition, for files, we must
     * set the archive flag.
     *
     * The following code avoids multiple ixlseek()/ixlread()/ixlwrite()
     * sequences by just getting a pointer to a buffer containing the
     * FCB, and updating it directly.  We must do an ixwrite() at the
     * end so that the buffer is marked as dirty and is subsequently
     * written.
     */
    if (dfd->o_flag & O_DIRTY)
    {
        FCB *fcb;
        UBYTE attr;

        ixlseek(fd->o_dirfil,fd->o_dirbyt); /* start of dir entry */
        fcb = ixgetfcb(fd->o_dirfil);
        attr = fcb->f_attrib;               /* get attributes */
        memcpy(&fcb->f_td,&dfd->o_td,10);   /* copy date/time, start, length */
        swpw(fcb->f_clust);                 /*  & fixup byte order */
        swpl(fcb->f_fileln);

        if (part & CL_DIR)
            fcb->f_fileln = 0L;             /* dir lengths on disk are zero */
        else
            attr |= FA_ARCHIVE;             /* set the archive flag for files */

        ixlseek(fd->o_dirfil,fd->o_dirbyt+FNAMELEN);/* seek to attrib byte */
        ixwrite(fd->o_dirfil,1,&attr);          /*  & rewrite it       */
        dfd->o_flag &= ~O_DIRTY;            /* not dirty any more */
    }

    if ((!part) || (part & CL_FULL))
    {
        q = &fd->o_dnode->d_files;

        for (p = *q; p ; p = *(q = &p->o_link))
            if (p == fd)
                break;

        /* someone else has this file open **** TBA */

        if (p)
            *q = p->o_link;
        else
            return EINTRN;  /* some kind of internal error */
    }

    /*
     * flush all drives
     *
     * this could in theory be improved by flushing all sectors for one
     * drive before moving on to the next, reducing arm movement on
     * partitioned hard disks.  however this would cost code space and,
     * in practice, flushing usually takes place to one drive only.
     */
    for (i = BI_FAT; i <= BI_DATA; i++)
        for (b = bufl[i]; b; b = b->b_link)
            if ((b->b_bufdrv != -1) && b->b_dirty)
                flush(b);

    return E_OK;
}


/*
 *  ixunlink - do the work for xunlink
 */
static long ixunlink(char *name)
{
    DND *dn;
    FCB *fcb;
    const char *s;
    long pos;

    /* first find path */

    dn = findit(name,&s,0);
    if (!dn)                                                /* M01.01.1214.01 */
        return EFILNF;

    /* now scan for filename */

    pos = 0;
    if (!(fcb = scan(dn,s,FA_NORM,&pos)))
        return EFILNF;

    if (fcb->f_attrib & FA_RO)
        return EACCDN;

    pos -= sizeof(FCB);

    return ixdel(dn,fcb,pos);
}


/*
 *  xunlink - unlink (delete) a file
 *
 *  Function 0x41   Fdelete
 *
 *  returns     EFILNF, EACCDN
 */
long xunlink(char *name)
{
    long rc;

    rc = ixunlink(name);

    KDEBUG(("xunlink(%s): rc=%ld\n",name,rc));

    return rc;
}


/*
**  ixdel - internal delete file.
**
**  Traverse the list of files open for this directory node.
**  If a file is found that has the same position in the directory as the one
**  we are to delete, then scan the system file table to see if this process is
**  then owner.  If so, then close it, otherwise abort.
**
**  NOTE:       both 'for' loops scan for the entire length of their
**              respective data structures, and do not drop out of the loop on
**              the first occurrence of a match.
**      Used by
**              ixcreat()
**              xunlink()
**              xrmdir()
**
*/
long ixdel(DND *dn, FCB *f, long pos)
{
    OFD *fd;
    DMD *dm;
    int n2;
    int n;
    char c;

    for (fd = dn->d_files; fd; fd = fd->o_link)
        if (fd->o_dirbyt == pos)
            for (n = 0; n < OPNFILES; n++)
                if (sft[n].f_ofd == fd)
                {
                    if (sft[n].f_own == run)
                        ixclose(fd,0);
                    else
                        return EACCDN;
                }

    /*
     * Traverse this file's chain of allocated clusters, freeing them.
     */
    dm = dn->d_drv;
    n = f->f_clust;
    swpw(n);

    while (n && !endofchain(n))
    {
        n2 = getrealcl(n,dm);
        clfix(n,FREECLUSTER,dm);
        n = n2;
    }

    /*
     * Mark the directory entry as erased.
     */
    fd = dn->d_ofd;
    ixlseek(fd,pos);
    c = ERASE_MARKER;
    ixwrite(fd,1L,&c);
    ixclose(fd,CL_DIR);

    /*
     * NOTE that the preceding routines that do physical disk operations
     * will 'longjmp' on failure at the BIOS level, thereby allowing us to
     * simply return with E_OK.
     */
    return E_OK;
}


/*
**  contains_illegal_characters - check for illegal filename chars in specified string
**
**  returns TRUE if found
*/
BOOL contains_illegal_characters(const char *test)
{
    const char *ref = ILLEGAL_FNAME_CHARACTERS;
    const char *t;

    while(*ref)
    {
        for (t = test; *t; t++)
            if (*t == *ref)
                return TRUE;
        ref++;
    }

    return FALSE;
}
