/*
 * fsfat.c - fat mgmt routines for file system           
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002 - 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include        "config.h"
#include        "portab.h"
#include        "asm.h"
#include        "fs.h" 
#include        "gemerror.h"



/*
**  cl2rec -
**      M01.0.1.03
*/

RECNO cl2rec(CLNO cl, DMD *dm)
{
        return((RECNO)(cl-2) * dm->m_clsiz);
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
        int f[1],mask;
        long pos;

        if (dm->m_16)
        {
                swpw(link);
                pos = (long)(cl) << 1;                  /*  M01.01.04   */
                ixlseek(dm->m_fatofd,pos);
                ixwrite(dm->m_fatofd,2L,&link);
                return;
        }

        pos = (cl + (cl >> 1));

        link = link & 0x0fff;

        if (cl & 1)
        {
                link = link << 4;
                mask = 0x000f;
        }
        else
                mask = 0xf000;

        ixlseek(dm->m_fatofd,pos);

        /* pre -read */
        ixread(dm->m_fatofd,2L,f);

        swpw(f[0]);
        f[0] = (f[0] & mask) | link;
        swpw(f[0]);

        ixlseek(dm->m_fatofd,pos);
        ixwrite(dm->m_fatofd,2L,f);
}




/*
**  getrealcl -
**      get the contents of the fat entry indexed by 'cl'.
**
**  returns
**      0xffff if entry contains the end of file marker
**      otherwise, the contents of the entry (16 bit value always returned).
**
**      M01.0.1.03
*/

CLNO getrealcl(CLNO cl, DMD *dm)
{
        unsigned int f[1];

        if (dm->m_16)
        {                               /*  M01.01.04  */
                ixlseek( dm->m_fatofd , (long)( (long)(cl) << 1 ) ) ;
                ixread(dm->m_fatofd,2L,f);
                swpw(f[0]);
                return(f[0]);
        }

        ixlseek(dm->m_fatofd,((long) (cl + (cl >> 1))));
        ixread(dm->m_fatofd,2L,f);
        swpw(f[0]);

        if (cl & 1)
                cl = f[0] >> 4;
        else
                cl = 0x0fff & f[0];

        if (cl == 0x0fff)
                return(0xffff);

        return(cl);
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
        DMD     *dm ;
        CLNO    i ;
        CLNO    rover ;
        CLNO    cl,cl2 ;                                /*  M01.01.03   */

        cl = p->o_curcl;
        dm = p->o_dmd;

        if (cl == 0) {              /* initial value */
            cl2 = (p->o_strtcl ? p->o_strtcl : 0xffff );
        } else if (!p->o_dnode) {   /* if no dir node, must be FAT/root */
            cl2 = cl + 1;
            goto retcl;             /* will be able to omit this when we get rid of negative clusters ... */
        } else {
            cl2 = getrealcl(cl,dm);
        }

        if (wrtflg && (cl2 == 0xffff ))
        { /* end of file, allocate new clusters */
                /* the following code carefully avoids allowing overflow in CLNO variables */
                rover = (cl < 2) ? 2 : cl;  /* start search at first or current cluster */
                for (i=0; i < dm->m_numcl; i++, rover++)    /* look at every cluster once */
                {
                        if (!getrealcl(rover,dm))       /* check for empty cluster */
                                break;
                        if (rover == dm->m_numcl+1)     /* wrap at max cluster num */ 
                                rover = 1;
                }
                cl2 = rover;

                if (i < dm->m_numcl)
                {
                        clfix(cl2,0xffff,dm);
                        if (cl)
                                clfix(cl,cl2,dm);
                        else
                        {
                                p->o_strtcl = cl2;
                                p->o_flag |= O_DIRTY;
                        }
                }
                else
                        return(0xffff);
        }

        if (cl2 == 0xffff)
                return(0xffff);

retcl:  p->o_curcl = cl2;
        p->o_currec = cl2rec(cl2,dm);
        p->o_curbyt = 0;

        return(E_OK);
}




/*      Function 0x36   d_free
                get disk free space data into buffer *
        Error returns
                ERR

        Last modified   SCC     15 May 85
*/

long xgetfree(long *buf, int drv) 
{
        CLNO i, free;
        long n;
        DMD *dm;

        drv = (drv ? drv-1 : run->p_curdrv);

        if ((n = ckdrv(drv)) < 0)
                return(ERR);

        dm = drvtbl[n];
        free = 0;
        for (i = 0; i < dm->m_numcl; i++)
                if (!getrealcl(i+2,dm))     /* cluster numbers start at 2 */
                        free++;
        *buf++ = (long)(free);
        *buf++ = (long)(dm->m_numcl);
        *buf++ = (long)(dm->m_recsiz);
        *buf = (long)(dm->m_clsiz);
        return(E_OK);
}

