/*
 * fshand.c - file handle routines for the file system
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2014-2016 The EmuTOS development team
 *
 * Authors:
 *  SCC   Steve C. Cavender
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "config.h"
#include "portab.h"
#include "fs.h"
#include "gemerror.h"


/*
 * xforce - 0x46, force a std handle to a non-std handle
 *
 * Arguments:
 *
 *  std - must be a standard handle
 *  h   - must NOT be a standard handle
 *
 * Error returns:
 *     EIHNDL
 */
long xforce(int std, int h)
{
    return ixforce(std,h,run);
}


/*
 * ixforce - force a std handle to a non-std handle
 *
 * Arguments:
 *
 *  std - must be a standard handle
 *  h   - must NOT be a standard handle
 */
long ixforce(int std, int h, PD *p)
{
    long fh;

    if ((std < 0) || (std >= NUMSTD))   /* validate standard handle */
        return EIHNDL;

    if (h < 0)                  /* if the non-std handle is a BIOS handle, */
        p->p_uft[std] = h;      /* just store it as-is in the PD table */
    else
    {
        if (h < NUMSTD)         /* validate the non-std handle */
            return EIHNDL;

        if (!getofd(h))         /* check that the non-std handle exists */
            return EIHNDL;

        /*
         * if the non-std handle is currently mapped to a BIOS handle,
         * store the BIOS handle in the PD table; otherwise store the
         * non-std handle & update the use count
         */
        if ((fh = (long) sft[h-NUMSTD].f_ofd) < 0L)
            p->p_uft[std] = fh;
        else
        {
            p->p_uft[std] = h;
            sft[h-NUMSTD].f_use++;
        }
    }

    return E_OK;
}


/*
 * xdup - 0x45, duplicate a file handle
 *
 * Arguments:
 *
 *  h  - must be standard handle (checked)
 *
 * Error returns:
 *     EIHNDL
 *     ENHNDL
 *
 */
long xdup(int h)
{
    int i;

    if ((h < 0) || (h >= NUMSTD))
        return EIHNDL;          /* only dup standard */

    for (i = 0; i < OPNFILES; i++)      /* find the first free handle */
        if (!sft[i].f_own)
            break;

    if (i == OPNFILES)
        return ENHNDL;          /* no free handles */

    sft[i].f_own = run;

    /*
     * if the standard handle is currently mapped to a non-BIOS
     * handle, copy the OFD pointer from the corresponding sft[]
     * entry to the new entry; otherwise, store the BIOS handle
     * in the OFD pointer variable
     */
    if ((h = run->p_uft[h]) > 0)
        sft[i].f_ofd = sft[h-NUMSTD].f_ofd;
    else
        sft[i].f_ofd = (OFD *)(long)h;

    sft[i].f_use = 1;

    return i+NUMSTD;            /* return the new handle */
}
