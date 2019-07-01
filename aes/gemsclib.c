/*      GEMSCLIB.C      07/10/84 - 02/02/85     Lee Lorenzen            */
/*      for 2.0         10/8/85  - 10/15/85     MDF                     */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2019 The EmuTOS development team
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
#include "obdefs.h"
#include "gemlib.h"

#include "gemdos.h"
#include "geminit.h"
#include "gemsclib.h"

#include "string.h"

/************************************************************************/
/*                                                                      */
/* sc_read() -- get the current scrap directory                         */
/*                                                                      */
/*  for compatibility with Atari TOS, no longer checks if a non-empty   */
/*  path already exists, always returns TRUE.                           */
/*                                                                      */
/************************************************************************/

WORD sc_read(char *pscrap)
{
    strcpy(pscrap, D.g_scrap);

    return TRUE;
}


/************************************************************************/
/*                                                                      */
/* sc_write() -- sets the current scrap directory                       */
/*                                                                      */
/*  for compatibility with Atari TOS, no longer validates the           */
/*  supplied directory path, always returns TRUE.                       */
/*                                                                      */
/************************************************************************/

WORD sc_write(const char *pscrap)
{
    strcpy(D.g_scrap, pscrap);

    return TRUE;
}

#if CONF_WITH_PCGEM
/************************************************************************/
/*                                                                      */
/* sc_clear() -- delete scrap files from current scrap directory        */
/*                                                                      */
/*      Assumes D.g_scrap holds a valid directory path.                 */
/*      Returns TRUE on success.                                        */
/*                                                                      */
/************************************************************************/

WORD sc_clear(void)
{
    char    *ptmp;
    DTA     *save_dta;
    WORD    ret;
    const char *scrapmask = "SCRAP.*";

    if (D.g_scrap[0] == '\0')
      return FALSE;

    ptmp = D.g_scrap;
    while(*ptmp)                            /* find null */
      ptmp++;

    strcpy(ptmp, scrapmask);                /* Add mask */

    save_dta = dos_gdta();                  /* save current DTA */
    dos_sdta(&D.g_dta);                     /* make sure dta ok */

    ret = dos_sfirst(D.g_scrap, FA_SUBDIR);
    while(ret == 0)
    {
        strcpy(ptmp + 1, D.g_dta.d_fname);  /* Add file name */
        dos_delete(D.g_scrap);              /* delete scrap.* */
        strcpy(ptmp, scrapmask);            /* Add mask */
        ret = dos_snext();
    }

    *ptmp = '\0';                           /* keep just path name */

    dos_sdta(save_dta);                     /* restore old DTA */

    return(TRUE);
}
#endif
