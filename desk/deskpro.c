/*      DESKPRO.C       4/18/84 - 03/19/85      Lee Lorenzen            */
/*      for 3.0         3/11/86 - 01/28/87      MDF                     */
/*      merge source    5/27/87 - 5/28/87       mdf                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2019 The EmuTOS development team
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

#include "emutos.h"
#include "string.h"

#include "aesdefs.h"
#include "obdefs.h"

#include "deskbind.h"
#include "deskglob.h"
#include "aesbind.h"
#include "desksupp.h"
#include "deskpro.h"


static WORD pro_exec(WORD isgraf, WORD isover, char *pcmd, char *ptail)
{
    WORD ret;

    desk_busy_on();

    ret = shel_write(SHW_EXEC, isgraf, isover, pcmd, ptail);
    if (!ret)
        desk_busy_off();
    return ret;
}


/*
 * run a program via shel_write()
 * optionally, deselect the current icon & zoom to desktop size
 */
WORD pro_run(WORD isgraf, char *cmd, char *tail, WORD wh, WORD curr)
{
    WORD ret, len;

    tail[0] = len = strlen(tail+1);
    tail[len+2] = 0x0D;     /* follows the nul byte, just like Atari TOS */
    ret = pro_exec(isgraf, 1, cmd, tail);

    if (wh != -1)
        do_wopen(FALSE, wh, curr, G.g_xdesk, G.g_ydesk, G.g_wdesk, G.g_hdesk);

    return ret;
}
