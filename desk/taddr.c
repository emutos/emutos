/*      DESKIF.C        12/03/84 - 02/09/85     Lee Lorenzen            */
/*      merge source    5/27/87                 mdf                     */

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
*       Copyright (C) 1985                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "gembind.h"



LONG obaddr(LONG tree, WORD obj, WORD fld_off)
{
        return( (tree + ((obj) * sizeof(OBJECT) + fld_off)) );
}
