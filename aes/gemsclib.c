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
#include "gem.h"

#include "gemrslib.h"
#include "gemdos.h"
#include "geminit.h"
#include "gemshlib.h"


#define NUM_SCRAPS      6


GLOBAL LONG             ad_scrap;
GLOBAL BYTE     *sc_types[NUM_SCRAPS] =
                         {"CSV", "TXT", "GEM",
                          "IMG", "DCA", "USR"};
GLOBAL WORD     sc_bits[NUM_SCRAPS] =
                         {SC_FTCSV,SC_FTTXT,SC_FTGEM,
                          SC_FTIMG,SC_FTDCA,SC_FTUSR};




WORD sc_clrd(WORD isread)
{
        LONG            ptmp, ptype;
        WORD            bitvect, ii;

        ptmp = ad_scrap;
        while(LBGET(ptmp))                      /* find null            */
          ptmp++;
        rs_gaddr(ad_sysglo, R_STRING, STSCRAP, &ptype);
        LSTCPY(ptmp, ptype); 
        ptype = ptmp + LSTRLEN(ptype);                  /* point just past '.'  */
        bitvect = 0;
        dos_sdta(ad_dta);                       /* make sure dta ok     */
        for (ii = 0; ii < NUM_SCRAPS; ii++)
        {
          LSTCPY(ptype, ADDR(sc_types[ii]));    /* cat on file type     */
          if (dos_sfirst(ad_scrap, F_SUBDIR))
          {
            if (isread)
              bitvect |= sc_bits[ii];           /* set corresponding bit */
            else
              dos_delete((BYTE *)ad_scrap);     /* delete scrap.*       */
          }
        }
        if ( !isread)
          bitvect = TRUE;
        LBSET(ptmp, 0);                         /* keep just path name  */
        return(bitvect);
}


/************************************************************************/
/*                                                                      */
/* sc_read() -- get info about current scrap directory                  */
/*                                                                      */
/*      copies the current scrap directory path to the passed-in        */
/*      address and returns a bit vector with bits set for specific     */
/*      file types present in the directory.  Looks for scrap.* files.  */
/*                                                                      */
/************************************************************************/

WORD sc_read(LONG pscrap)
{
        WORD            len;

        len = LSTCPY(pscrap, ad_scrap);         /* current scrap directory */
        LSTCPY(pscrap+len, ADDR("\\"));         /* cat on backslash     */
        return( sc_clrd(TRUE) );
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
        WORD            len;

        len = LSTCPY(ad_scrap, pscrap);         /* new scrap directory  */
        if (LBGET(ad_scrap + --len) == '\\')    /* remove backslash     */
          LBSET(ad_scrap + len, '\0');
        dos_sdta(ad_dta);                       /* use our dta          */
        return(dos_sfirst(ad_scrap, F_SUBDIR)); /* make sure path ok    */
}


/************************************************************************/
/*                                                                      */
/* sc_clear() -- delete scrap files from current scrap directory        */
/*                                                                      */
/*      Assumes *ad_scrap holds a valid directory path.  Returns TRUE   */
/*                                                                      */
/************************************************************************/

WORD sc_clear()
{
        return( sc_clrd(FALSE) );
}

