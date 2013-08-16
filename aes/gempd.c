/*      GEMPD.C         1/27/84 - 03/20/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2013 The EmuTOS development team
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

#include "config.h"
#include "portab.h"
#include "compat.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"

#include "gemdosif.h"
#include "geminit.h"
#include "gemasm.h"
#include "optimize.h"
#include "optimopt.h"
#include "gempd.h"

#include "string.h"

/* returns the AESPD for the given index */
AESPD *pd_index(WORD i)
{
        return( (i<2) ? &D.g_intpd[i] : &D.g_extpd[i-2] );
}

/* returns the AESPD for the given name, or if pname is NULL for the given pid */
AESPD *fpdnm(BYTE *pname, UWORD pid)
{
        WORD            i;
        BYTE            temp[9];
        AESPD           *p;

        temp[8] = 0;
        for(i=0; i<totpds; i++)
        {
          p = pd_index(i);
          if (pname != NULLPTR)
          {
            /* TODO - strncmp */
            memcpy(temp, p->p_name, 8);
            if( strcmp(pname, temp)==0 )
              return(p);
          }
          else
            if (p->p_pid == pid)
              return(p);
        }
        return(0);
}


static AESPD *getpd(void)
{
        register AESPD  *p;

        /* we got all our memory so link it  */
        p = pd_index(curpid);
        p->p_pid = curpid++;

        /* was: setdsss(p->p_uda); */
        p->p_uda->u_insuper = 1;

        /* return the pd we got */
        return(p);
}


/* name an AESPD from the 8 first chars of the given string, stopping at the first
 * '.' (remove the file extension)
 */
void p_nameit(AESPD *p, BYTE *pname)
{
        memset(p->p_name, ' ', 8);
        strscn(pname, p->p_name, '.');
}

/* set the application directory of an AESPD */
void p_setappdir(AESPD *pd, BYTE *pfilespec)
{
        BYTE *p;
        BYTE *plast;
        BYTE *pdest;

        /* find the position *after* the last backslash */
        for (p = plast = pfilespec; *p; )   /* assume no backslash */
        {
          if (*p++ == '\\')
            plast = p;          /* after backslash ... */
        }

        /* copy the directory name including the final backslash */
        for (pdest = pd->p_appdir, p = pfilespec; p < plast; )
          *pdest++ = *p++;
        *pdest = '\0';
}

AESPD *pstart(PFVOID pcode, BYTE *pfilespec, LONG ldaddr)
{
        register AESPD  *px;

        /* create process to execute it */
        px = getpd();
        px->p_ldaddr = ldaddr;
                                                /* copy in name of file */
        p_nameit(px, pfilespec);
        p_setappdir(px, pfilespec);

        /* set pcode to be the return address when this process runs */
        psetup(px, pcode);
                                                /* link him up          */
        /* put it on top of the drl list */
        px->p_stat &= ~WAITIN;
        px->p_link = drl;
        drl = px;

        return(px);
}

                                                /* put pd pi into list  */
                                                /*   *root at the end   */
void insert_process(AESPD *pi, AESPD **root)
{
        register AESPD  *p, *q;
                                                /* find the end         */
        for ( p = (q = (AESPD *) root) -> p_link ; p ; p = (q = p) -> p_link);
                                                /* link him in          */
        pi->p_link = p;
        q->p_link = pi;
}
