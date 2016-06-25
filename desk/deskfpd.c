/*      DESKFPD.C       06/11/84 - 03/29/85             Lee Lorenzen    */
/*      source merge    5/19/87  - 5/28/87              mdf             */
/*      for 3.0         11/4/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
        -------------------------------------------------------------
*       GEM Desktop                                       Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "obdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "dos.h"
#include "deskbind.h"

#include "../aes/gemdos.h"
#include "../aes/optimopt.h"
#include "deskrsrc.h"
#include "deskglob.h"

#include "string.h"
#include "kprint.h"


/*
 *  Initialize the list of pnodes
 */
static void pn_init(void)
{
    WORD i;
    PNODE *pn;

    for (i = 0, pn = G.g_plist; i < NUM_PNODES-1; i++, pn++)
        pn->p_next = pn + 1;
    pn->p_next = NULL;

    G.g_pavail = &G.g_plist[0];
    G.g_phead = (PNODE *) NULL;
}


/*
 *  Start up by initializing global variables
 */
void fpd_start(void)
{
    pn_init();
}


/*
 *  Build a filespec out of drive letter, a pointer to a path, a pointer
 *  to a filename, and a pointer to an extension
 */
static WORD fpd_bldspec(WORD drive, BYTE *ppath, BYTE *pname, BYTE *pext, BYTE *pspec)
{
    int len = 0;

    if (*pname)
    {
        len = strlen(pname) + 1;        /* allow for "\" */
        if (*pext)
            len += strlen(pext) + 1;    /* allow for "." */
    }
    if ((strlen(ppath) + len) >= (LEN_ZPATH-3))
        return FALSE;

    *pspec++ = drive;
    *pspec++ = ':';
    *pspec++ = '\\';
    if (*ppath)
    {
        while(*ppath)
            *pspec++ = *ppath++;
        if (*pname)
            *pspec++ = '\\';
    }

    if (*pname)
    {
        while(*pname)
            *pspec++ = *pname++;
        if (*pext)
        {
            *pspec++ = '.';
            while(*pext)
                *pspec++ = *pext++;
        }
    }
    *pspec++ = '\0';

    return TRUE;
}


/*
 *  Parse a filespec into its drive, path, name, and extension parts
 */
void fpd_parse(BYTE *pspec, WORD *pdrv, BYTE *ppath, BYTE *pname, BYTE *pext)
{
    BYTE *pstart, *p1st, *plast, *pperiod;

    pstart = pspec;

    /* get the drive */
    while(*pspec && (*pspec != ':'))
        pspec++;
    if (*pspec == ':')
    {
        pspec--;
        *pdrv = (WORD) *pspec;
        pspec++;
        pspec++;
        if (*pspec == '\\')
            pspec++;
    }
    else
    {
        *pdrv = (WORD) (dos_gdrv() + 'A');
        pspec = pstart;
    }

    /* scan for key bytes */
    p1st = pspec;
    plast = pspec;
    pperiod = NULL;
    while(*pspec)
    {
        if (*pspec == '\\')
            plast = pspec;
        if (*pspec == '.')
            pperiod = pspec;
        pspec++;
    }
    if (pperiod == NULL)
        pperiod = pspec;

    /* get the path */
    while(p1st != plast)
        *ppath++ = *p1st++;
    *ppath = '\0';
    if (*plast == '\\')
        plast++;

    /* get the name */
    while(plast != pperiod)
        *pname++ = *plast++;
    *pname = '\0';

    /* get the ext  */
    if (*pperiod)
    {
        pperiod++;
        while(pperiod != pspec)
            *pext++ = *pperiod++;
    }
    *pext = '\0';
}


/*
 *  Find the file node that matches a particular object id
 */
FNODE *fpd_ofind(FNODE *pf, WORD obj)
{
    if (obj < 0)    /* if object doesn't exist, */
        return NULL;/* neither does file node.  */

    while(pf)
    {
        if (pf->f_obid == obj)
            return pf;
        pf = pf->f_next;
    }

    return NULL;
}


/*
 *  Free the file nodes for a specified pathnode
 */
static void fl_free(PNODE *pn)
{
    if (pn->p_fbase)
        dos_free((LONG)pn->p_fbase);

    pn->p_fbase = pn->p_flist = NULL;
    pn->p_count = 0;
    pn->p_size = 0L;
}


/*
 *  Allocate a path node
 */
static PNODE *pn_alloc(void)
{
    PNODE *thepath;

    if (G.g_pavail)
    {
        /* get us off the avail list */
        thepath = G.g_pavail;
        G.g_pavail = G.g_pavail->p_next;

        /* put us on the active list */
        thepath->p_next = G.g_phead;
        G.g_phead = thepath;

        /* init. and return */
        thepath->p_flist = (FNODE *) NULL;
        return thepath;
    }

    return NULL;
}


/*
 *  Free a path node
 */
static void pn_free(PNODE *thepath)
{
    PNODE *pp;

    /* free our file list */
    fl_free(thepath);

    /* if first in list, unlink by changing phead
     * else by finding and changing our previous guy
     */
    pp = (PNODE *) &G.g_phead;
    while(pp->p_next != thepath)
        pp = pp->p_next;
    pp->p_next = thepath->p_next;

    /* put us on the avail list */
    thepath->p_next = G.g_pavail;
    G.g_pavail = thepath;
}


/*
 *  Close a particular path
 */
void pn_close(PNODE *thepath)
{
    pn_free(thepath);
}


/*
 *  Open a particular path
 */
PNODE *pn_open(WORD  drive, BYTE *path, BYTE *name, BYTE *ext, WORD attr)
{
    PNODE *thepath;

    thepath = pn_alloc();
    if (thepath)
    {
        if (fpd_bldspec(drive, path, name, ext, &thepath->p_spec[0]))
        {
            thepath->p_attr = attr;
            return thepath;
        }
        pn_close(thepath);  /* failed, free it up */
    }

    return NULL;
}


/*
 *  Compare file nodes pf1 & pf2, using:
 *      (1) a field determined by 'which'
 *      (2) the full name, if (1) compares equal
 *
 *  Returns -ve if pf1<pf2, 0 if pf1==pf2, +ve if pf1>pf2
 */
static LONG pn_fcomp(FNODE *pf1, FNODE *pf2, WORD which)
{
    LONG chk = 0L;
    BYTE *ps1, *ps2;

    ps1 = pf1->f_name;
    ps2 = pf2->f_name;

    switch(which)
    {
    case S_DATE:
        chk = (LONG)pf2->f_date - (LONG)pf1->f_date;
        if (chk == 0L)
            chk = (LONG)pf2->f_time - (LONG)pf1->f_time;
        break;
    case S_SIZE:
        chk = (LONG)pf2->f_size - (LONG)pf1->f_size;
        break;
    case S_TYPE:
        chk = strcmp(scasb(ps1,'.'),scasb(ps2,'.'));
        break;
    case S_NSRT:
        chk = (LONG)pf1->f_seq - (LONG)pf2->f_seq;  /* low seq #s sort first */
        break;
    }
    if (chk)
        return chk;

    return strcmp(ps1,ps2); /* always the last test (the only test if S_NAME) */
}


/*
 *  Routine to compare two fnodes to see which one is greater.
 *  Sort sequence is based on the G.g_isort parameter; folders
 *  always sort out first (unless 'unsorted' is specified).
 *
 *  Returns -ve if pf1 < pf2, 0 if pf1 == pf2, and +ve if pf1 > pf2
 */
static LONG pn_comp(FNODE *pf1, FNODE *pf2)
{
    if (G.g_isort != S_NSRT)
    {
        if ((pf1->f_attr ^ pf2->f_attr) & F_SUBDIR)
            return (pf1->f_attr & F_SUBDIR) ? -1L : 1L;
    }

    return pn_fcomp(pf1,pf2,G.g_isort);
}


/*
 *  Sort the fnodes in the list chained from the specified pathnode
 *
 */
FNODE *pn_sort(PNODE *pn)
{
    FNODE *pf, *pftemp;
    FNODE *newlist;
    FNODE **ml_pfndx;
    WORD  count, gap, i, j;

    if (pn->p_count < 2)        /* the list is already sorted */
        return pn->p_flist;

    /*
     * malloc & build index array
     */
    ml_pfndx = dos_alloc(pn->p_count*sizeof(FNODE *));
    if (!ml_pfndx)              /* no space, can't sort */
        return pn->p_flist;

    for (count = 0, pf = pn->p_flist; pf; pf = pf->f_next)
        ml_pfndx[count++] = pf;

    /* sort files using shell sort on page 108 of  K&R C Prog. Lang. */
    for (gap = count/2; gap > 0; gap /= 2)
    {
        for (i = gap; i < count; i++)
        {
            for (j = i-gap; j >= 0; j -= gap)
            {
                if (pn_comp(ml_pfndx[j], ml_pfndx[j+gap]) <= 0L)
                    break;
                pftemp = ml_pfndx[j];
                ml_pfndx[j] = ml_pfndx[j+gap];
                ml_pfndx[j+gap] = pftemp;
            }
        }
    }

    /* link up the list in order */
    newlist = ml_pfndx[0];
    pf = ml_pfndx[0];
    for (i = 1; i < count; i++)
    {
        pf->f_next = ml_pfndx[i];
        pf = ml_pfndx[i];
    }
    pf->f_next = (FNODE *) NULL;

    dos_free((LONG)ml_pfndx);

    return newlist;
}


/*
 *  Build the filenode list for the specified pathnode
 */
WORD pn_active(PNODE *pn)
{
    FNODE *fn, *prev;
    LONG maxmem, maxcount, size = 0L;
    WORD count, ret;

    fl_free(pn);                    /* free any existing filenodes */

    maxmem = dos_avail();           /* allocate max possible memory */
    if (maxmem < sizeof(FNODE))
        return E_NOMEMORY;

    pn->p_fbase = dos_alloc(maxmem);
    maxcount = maxmem / sizeof(FNODE);

    fn = pn->p_fbase;
    prev = (FNODE *)&pn->p_flist;   /* assumes fnode link is at start of fnode */

    dos_sdta(&G.g_wdta);

    for (ret = dos_sfirst(pn->p_spec,pn->p_attr), count = 0; (ret == 0) && (count < maxcount); ret = dos_snext())
    {
        if (G.g_wdta.d_fname[0] == '.') /* skip "." & ".." entries */
            continue;
        memcpy(&fn->f_junk, &G.g_wdta.d_reserved[20], 23);
        fn->f_seq = count++;
        size += fn->f_size;
        prev->f_next = fn;      /* link fnodes */
        prev = fn++;
    }
    prev->f_next = NULL;        /* terminate chain */
    pn->p_count = count;        /* & update pathnode */
    pn->p_size = size;

    if (count == 0)
    {
        dos_free((LONG)pn->p_fbase);
        pn->p_fbase = NULL;
        return 0;
    }
    dos_shrink(pn->p_fbase,count*sizeof(FNODE));

    pn->p_flist = pn_sort(pn);

    return 0;   /* TODO: return error if error occurred? */
}
