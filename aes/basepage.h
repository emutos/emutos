/*      BASEPAGE.H      1/28/84 - 12/15/84      Lee Jay Lorenzen        */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2013 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/



/* Ready List Root - a list of AESPDs linked by the p_link field, terminated
 * by zero [see gempd.c function insert_process]
 */
extern AESPD    *rlr;

extern AESPD    *drl, *nrl;
extern EVB      *eul, *dlr, *zlr;

extern LONG     elinkoff;

/* In Dispatch - a byte whose value is zero when not in function
 * dsptch, and 1 when between dsptch ... switchto function calls
 */
extern BYTE     indisp;

extern WORD     fpt, fph, fpcnt;                /* forkq tail, head,    */
                                                /*   count              */
extern SPB      wind_spb;
extern CDA      *cda;
extern WORD     curpid;
