/*      DESKFPD.C       06/11/84 - 03/29/85             Lee Lorenzen    */
/*      source merge    5/19/87  - 5/28/87              mdf             */
/*      for 3.0         11/4/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
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

#include "emutos.h"
#include "obdefs.h"
#include "gemdos.h"
#include "optimize.h"
#include "optimopt.h"
#include "gemerror.h"

#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskdir.h"
#include "deskfpd.h"
#include "deskins.h"
#include "deskwin.h"
#include "deskrsrc.h"
#include "desksupp.h"

#include "string.h"


/*
 *  Free the file nodes for a specified pathnode
 */
static void fl_free(PNODE *pn)
{
    if (pn->p_fbase)
        dos_free(pn->p_fbase);

    pn->p_fbase = pn->p_flist = NULL;
    pn->p_count = 0;
    pn->p_size = 0L;
}


/*
 *  Close a particular path
 */
void pn_close(PNODE *thepath)
{
    /* free our file list */
    fl_free(thepath);
}


/*
 *  Open a particular path
 */
PNODE *pn_open(char *pathname, WNODE *pw)
{
    PNODE *thepath;

    if (strlen(pathname) >= MAXPATHLEN)
        return NULL;

    /*
     * if not associated with a specific window, use the PNODE in the
     * desktop pseudo-window.  this happens for e.g. disk->disk copy
     */
    if (!pw)
        pw = &G.g_wdesktop;

    thepath = &pw->w_pnode;
    thepath->p_flist = NULL;    /* file list starts empty */
    strcpy(thepath->p_spec,pathname);
    thepath->p_attr = DISPATTR;

    return thepath;
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
    char *ps1, *ps2;

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
        if ((pf1->f_attr ^ pf2->f_attr) & FA_SUBDIR)
            return (pf1->f_attr & FA_SUBDIR) ? -1L : 1L;
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
    ml_pfndx = dos_alloc_anyram(pn->p_count*sizeof(FNODE *));
    if (!ml_pfndx)              /* no space, can't sort */
    {
        malloc_fail_alert();
        return pn->p_flist;
    }

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

    dos_free(ml_pfndx);

    return newlist;
}


/*
 *  Build the filenode list for the specified pathnode
 *
 *  if 'include_folders' is TRUE, the filenode list will include all
 *  folders in the pathnode: this is required for correct display of
 *  directory contents when the filemask is other than *.*
 *
 *  if 'include_folders' is FALSE, the filenode list will only include
 *  files/folders matching the filespec in the pathnode: this is used by
 *  fun_file2any() when dragging a desktop icon representing a file/folder
 *
 *  returns 0   0 or more files found without error
 *              NOTE: if insufficient memory is available, some files in
 *              the specified pathnode will be silently excluded from the
 *              filenode list.  our excuse is that Atari TOS does this too ...
 *          <0  error (other than EFILNF/ENMFIL) returned by dos_sfirst()/dos_snext()
 *              (e.g. when attempting to open a floppy drive with no disk)
 */
WORD pn_active(PNODE *pn, BOOL include_folders)
{
    DTA *dtasave;
    FNODE *fn, *prev;
    LONG maxmem, maxcount, size = 0L;
    WORD count, ret;
#if CONF_WITH_FILEMASK
    char search[MAXPATHLEN];
    char *match;
#endif

    fl_free(pn);                    /* free any existing filenodes */

    maxmem = dos_avail_anyram();    /* allocate max possible memory */
    maxcount = maxmem / sizeof(FNODE);
    if (maxcount)
        pn->p_fbase = dos_alloc_anyram(maxmem);

    fn = pn->p_fbase;
    prev = (FNODE *)&pn->p_flist;   /* assumes fnode link is at start of fnode */

    dtasave = dos_gdta();           /* so we can preserve it */
    dos_sdta(&G.g_wdta);

#if CONF_WITH_FILEMASK
    strcpy(search, pn->p_spec);
    /*
     * do we want to include folders in the match list?
     */
    if (include_folders)                /* match all folders? */
        del_fname(search);              /* yes - change search filespec to *.* */
    match = filename_start(pn->p_spec); /* the match filespec is always unaltered */
    for (ret = dos_sfirst(search, pn->p_attr), count = 0; (ret == 0) && (count < maxcount); ret = dos_snext())
    {
        if (G.g_wdta.d_attrib != FA_SUBDIR) /* skip *files* that don't match */
            if (!wildcmp(match, G.g_wdta.d_fname))
                continue;
#else
    for (ret = dos_sfirst(pn->p_spec,pn->p_attr), count = 0; (ret == 0) && (count < maxcount); ret = dos_snext())
    {
#endif
        if (G.g_wdta.d_fname[0] == '.') /* skip "." & ".." entries */
            continue;
        fn->f_selected = FALSE;
        memcpy(&fn->f_attr, &G.g_wdta.d_attrib, 23);
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
        fl_free(pn);            /* free any existing filenodes */
    }
    else
    {
        dos_shrink(pn->p_fbase,count*sizeof(FNODE));
        pn->p_flist = pn_sort(pn);
    }

    /* check if enough FNODEs were available */
    if (count >= maxcount)
        KDEBUG(("Not enough FNODEs for folder %s\n",pn->p_spec));

    dos_sdta(dtasave);          /* restore original DTA for neatness */

    return ((ret==ENMFIL) || (ret==EFILNF)) ? 0 : ret;
}


/*
 *  Clear the selection flag in all FNODES chained from the PNODE in the specified WNODE
 */
void pn_clear(WNODE *pw)
{
    FNODE *pf;

    for (pf = pw->w_pnode.p_flist; pf; pf = pf->f_next)
        pf->f_selected = FALSE;
}
