/*      BASEPAGE.H      1/28/84 - 12/15/84      Lee Jay Lorenzen        */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
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

                                                /* in BASE88.C          */
extern PD       *rlr, *drl, *nrl;
extern EVB      *eul, *dlr, *zlr;

#if I8086
extern UWORD    elinkoff;
#else
extern LONG     elinkoff;
#endif
extern BYTE     indisp;
extern WORD     fpt, fph, fpcnt;                /* forkq tail, head,    */
                                                /*   count              */
extern SPB      wind_spb;
extern CDA      *cda;
extern WORD     curpid;

