/*      DESKPRO.C       4/18/84 - 03/19/85      Lee Lorenzen            */
/*      for 3.0         3/11/86 - 01/28/87      MDF                     */
/*      merge source    5/27/87 - 5/28/87       mdf                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985 - 1987                       Digital Research Inc.
*       -------------------------------------------------------------
*/

#include <string.h>

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "dos.h"
#include "infodef.h"
#include "desk_rsc.h"
#include "gembind.h"
#include "deskbind.h"

#include "gemdos.h"
#include "aesbind.h"
#include "deskglob.h"
#include "desksupp.h"



WORD pro_chdir(WORD drv, BYTE *ppath)
{
        WORD            tmpdrv;
                                                /* change to directory  */
                                                /*   that application   */
                                                /*   is in              */
        if (!drv)
          return( (DOS_ERR = TRUE) );

        if ( drv != '@' ) 
        {
          tmpdrv = dos_gdrv();
          dos_sdrv(drv - 'A');
          if (DOS_ERR)
          {
            dos_sdrv(tmpdrv);
            return(FALSE);
          }
          G.g_srcpth[0] = drv;
          G.g_srcpth[1] = ':';
          G.g_srcpth[2] = '\\';
          strcpy(&G.g_srcpth[3], ppath);
          dos_chdir((BYTE *)ADDR(&G.g_srcpth[0]));
        }
        else
          dos_sdrv(gl_stdrv);           /* don't leave closed drive hot */
        return(TRUE);
} /* pro_chdir */


WORD pro_exec(WORD isgraf, WORD isover, LONG pcmd, LONG ptail)
{
        WORD            ret;
#if MULTIAPP
        WORD            chnum;
        LONG            begaddr, csize;

        if (isover != 3)
#endif
        graf_mouse(HGLASS, 0x0L);

#if MULTIAPP
        if ((isover == -1) || (isover == 2) || (isover == 3))
        {
          if (isover == 2)                      /* full step    */
            pro_chcalc((LONG)-1, &begaddr, &csize);
          else
            pro_chcalc((LONG)pr_kbytes << 10, &begaddr, &csize);

          ret = proc_create(begaddr, csize, 1, isgraf, &chnum);
          if (!ret)
          {
            fun_alert(1,STNOROOM,NULLPTR);
            return(FALSE);
          }
          if (isover == 2)
            gl_fmemflg |= (1 << chnum);

          ret = proc_run(chnum, isgraf, isover, pcmd, ptail);
          if (isover==3)
            ret = 0;
        }
        else
#endif
          ret = shel_write(TRUE, isgraf, isover, pcmd, ptail);
        if (!ret)
          graf_mouse(ARROW, 0x0L);
        return( ret );
} /* pro_exec */


WORD pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr)
{
        WORD            ret, len, i;

        G.g_tail[0] = len = strlen(&G.g_tail[1]);
        if ( (len) && (!isgraf) )
        {
          for(i = len; i; i--)
            G.g_tail[i+1] = G.g_tail[i];
          G.g_tail[1] = ' ';
          len++;
        } /* if */
        G.g_tail[0] = len;
        G.g_tail[len+1] = 0x0D;
#if MULTIAPP
        if (isover != 3)                /* keep icon SELECTED during FORMAT */
          do_wopen(FALSE, wh, curr, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk);
#endif
        ret = pro_exec(isgraf, isover, G.a_cmd, G.a_tail);
        if (isover == -1)
          ret = FALSE;
        else
        {
#if MULTIAPP
          if (isover == 3)                      /* for FORMAT           */
#else
          if (wh != -1)
#endif
            do_wopen(FALSE, wh, curr, G.g_xdesk, G.g_ydesk,
                     G.g_wdesk, G.g_hdesk);
        } /* else */
        return(ret);
} /* pro_run */



WORD pro_exit(LONG pcmd, LONG ptail)
{
        WORD            ret;

        ret = shel_write(FALSE, FALSE, 1, pcmd, ptail);
        return( ret );
} /* pro_exit */
