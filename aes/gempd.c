/*      GEMPD.C         1/27/84 - 03/20/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2022 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "emutos.h"
#include "struct.h"
#include "aesvars.h"
#include "obdefs.h"
#include "gemlib.h"

#include "geminit.h"
#include "gemasm.h"
#include "gempd.h"

#include "string.h"

/* returns the AESPD for the given index */
AESPD *pd_index(WORD i)
{
    return (i<2) ? &D.g_int[i].a_pd : &D.g_acc[i-2].a_pd;
}

/* returns the AESPD for the given name, or if pname is NULL for the given pid */
AESPD *fpdnm(char *pname, UWORD pid)
{
    WORD    i;
    AESPD   *p;

    for (i = 0; i < totpds; i++)
    {
        p = pd_index(i);
        if (pname != NULL)
        {
            if (strncmp(pname, p->p_name, AP_NAMELEN) == 0)
                return p;
        }
        else
            if (p->p_pid == pid)
                return p;
    }

    return NULL;
}


static AESPD *getpd(void)
{
    AESPD *p;

    /* we got all our memory so link it  */
    p = pd_index(curpid);
    p->p_pid = curpid++;

    /* return the pd we got */
    return p;
}


/*
 * name an AESPD from the 8 first chars of the given string, stopping at the first
 * '.' (remove the file extension)
 */
void p_nameit(AESPD *p, char *pname)
{
    char *s, *d;
    int i;

    for (i = 0, s = pname, d = p->p_name; (i < AP_NAMELEN) && *s && (*s != '.'); i++)
        *d++ = *s++;
    for ( ; i < 8; i++)
        *d++ = ' ';
}


/* set the application directory of an AESPD */
void p_setappdir(AESPD *pd, char *pfilespec)
{
    char *p;
    char *plast;
    char *pdest;

    /* find the position *after* the last path separator */
    for (p = plast = pfilespec; *p; )   /* assume no path separator */
    {
        if (*p++ == PATHSEP)
            plast = p;          /* after path separator ... */
    }

    /* copy the directory name including the final path separator */
    for (pdest = pd->p_appdir, p = pfilespec; p < plast; )
        *pdest++ = *p++;
    *pdest = '\0';
}


AESPD *pstart(PFVOID pcode, char *pfilespec, LONG ldaddr)
{
    AESPD *px;

    /* create process to execute it */
    px = getpd();
    px->p_ldaddr = ldaddr;

    /* copy in name of file */
    p_nameit(px, pfilespec);
    p_setappdir(px, pfilespec);

    /* set pcode to be the return address when this process runs */
    psetup(px, pcode);

    /* link him up: put it on top of the drl list */
    px->p_stat &= ~WAITIN;
    px->p_link = drl;
    drl = px;

    return px;
}

/* put pd pi into list, *root at the end */
void insert_process(AESPD *pi, AESPD **root)
{
    AESPD *p, *q;

    /* find the end */
    for (p = (q = (AESPD *)root)->p_link; p; p = (q = p)->p_link)
        ;

    /* link him in */
    pi->p_link = p;
    q->p_link = pi;
}
