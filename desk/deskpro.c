/*      DESKPRO.C       4/18/84 - 03/19/85      Lee Lorenzen            */
/*      for 3.0         3/11/86 - 01/28/87      MDF                     */
/*      merge source    5/27/87 - 5/28/87       mdf                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
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

/* #define ENABLE_KDEBUG */

#include "config.h"
#include <string.h>

#include "portab.h"
#include "obdefs.h"
#include "gemdos.h"

#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskbind.h"

#include "aesbind.h"
#include "deskglob.h"
#include "desksupp.h"
#include "deskpro.h"
#include "kprint.h"


static WORD pro_exec(WORD isgraf, WORD isover, BYTE *pcmd, BYTE *ptail)
{
    WORD ret;

    graf_mouse(HGLASS, NULL);

    ret = shel_write(TRUE, isgraf, isover, pcmd, ptail);
    if (!ret)
        graf_mouse(ARROW, NULL);
    return ret;
}


/*
 * run a program via shel_write()
 * optionally, deselect the current icon & zoom to desktop size
 */
WORD pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr)
{
    WORD ret, len;

    G.g_tail[0] = len = strlen(&G.g_tail[1]);
    G.g_tail[len+1] = 0x0D;
    ret = pro_exec(isgraf, isover, G.g_cmd, G.g_tail);

    if (wh != -1)
        do_wopen(FALSE, wh, curr, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk);

    return ret;
}


WORD pro_exit(BYTE *pcmd, BYTE *ptail)
{
    WORD ret;

    ret = shel_write(FALSE, FALSE, 1, pcmd, ptail);
    return ret;
}
