/*      GEMSCLIB.C      07/10/84 - 02/02/85     Lee Lorenzen            */
/*      for 2.0         10/8/85  - 10/15/85     MDF                     */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */ 

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
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

#include "portab.h"
#include "machine.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"
#include "crysbind.h"
#include "dos.h"
#include "gem_rsc.h"

#include "gemrslib.h"
#include "gemdos.h"
#include "geminit.h"
#include "gemshlib.h"

#include "string.h"


GLOBAL LONG             ad_scrap;



/************************************************************************/
/*                                                                      */
/* sc_read() -- get info about current scrap directory                  */
/*                                                                      */
/*      copies the current scrap directory path to the passed-in        */
/*      address and returns TRUE if a valid path has already been set.  */
/*                                                                      */
/************************************************************************/

WORD sc_read(LONG pscrap)
{
    WORD    len;

    /* current scrap directory */
    len = strlencpy((char *) pscrap, (char *) ad_scrap);      
    strcpy((char *) pscrap+len, "\\");      /* cat on backslash  */
    return( len != 0 );
}


/************************************************************************/
/*                                                                      */
/* sc_write() -- sets the current scrap directory                       */
/*                                                                      */
/*      pscrap must be the long address of a valid path.  Returns       */
/*      TRUE if no error occurs in validating the path name.            */
/*                                                                      */
/************************************************************************/

WORD sc_write(LONG pscrap)
{
    WORD    len;

    len = strlencpy((char *) ad_scrap, (char *) pscrap);      /* new scrap directory  */
    if (LBGET(ad_scrap + --len) == '\\')    /* remove backslash     */
      LBSET(ad_scrap + len, '\0');
    dos_sdta(ad_dta);                       /* use our dta          */
    return(dos_sfirst(ad_scrap, F_SUBDIR)); /* make sure path ok    */
}


/************************************************************************/
/*                                                                      */
/* sc_clear() -- delete scrap files from current scrap directory        */
/*                                                                      */
/*      Assumes *ad_scrap holds a valid directory path.                 */
/*      Returns TRUE on success.                                        */
/*                                                                      */
/************************************************************************/

WORD sc_clear()
{
    LONG    ptmp;
    WORD    found;
    static char scrapmask[] = "\\SCRAP.*";

    if(ad_scrap == NULL || LBGET(ad_scrap) == 0)
      return FALSE;

    ptmp = ad_scrap;
    while(LBGET(ptmp))                      /* find null */
      ptmp++;

    strcpy((char *) ptmp, scrapmask);       /* Add mask */

    dos_sdta(ad_dta);                       /* make sure dta ok */

    found = dos_sfirst(ad_scrap, F_SUBDIR);
    while(found)
    {
        strcpy((char *)ptmp + 1, ((char *)ad_dta + 30));  /* Add file name */
        dos_delete((char *)ad_scrap);       /* delete scrap.* */
        strcpy((char *) ptmp, scrapmask);   /* Add mask */
        found = dos_snext();
    }

    LBSET(ptmp, 0);                         /* keep just path name */

    return(TRUE);
}

