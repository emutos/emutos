/*      DESKRSRC.C      05/04/84 - 06/10/85     Lee Lorenzen            */
/*      for 3.0         4/25/86                 MDF                     */
/*      merge source    5/27/87                 mdf                     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2013 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include "portab.h"

#include "aesbind.h"


#define R_STRING 5


BYTE *ini_str(WORD stnum)
{
        LONG            lstr;

        rsrc_gaddr(R_STRING, stnum, &lstr);
        return (BYTE *)lstr;
}
