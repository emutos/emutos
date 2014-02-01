/*
 * compat.h - compiler/architecture compatibility settings
 *
 * This file was originally named MACHINE.H, and almost-identical
 * versions existed in the AES and DESK subdirectories.  It was
 * renamed by the EmuTOS development team (to avoid confusion with
 * the MACHINE.H header used within the BIOS subdirectory) and
 * centralised in the INCLUDE subdirectory.
 *
 */
/*      MACHINE.H               09/29/84-02/08/85       Lee Lorenzen    */
/*      GEM20                   12/17/85                Lowell Webster  */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
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
#ifndef _COMPAT_H
#define _COMPAT_H

#include <string.h>


#define DEBUG 0



#define NUM_WIN 8               /* 8 for main app and desk accs       */

#define NUM_ACCS 6              /* maximum number of desk accessory   */
                                /* _files_ (.ACC) that will be loaded */

#define NUM_DESKACC 6           /* maximum number of desk accessory   */
                                /* _slots_ available (one slot per    */
                                /* mn_register() call)                */


extern WORD LBWMOV(WORD *pdst, BYTE *psrc);


/************************************************************************/


                                                /* return a long address*/
                                                /*   of a short pointer */
#define ADDR(x) ((LONG) (x))

                                                /* return a single long */
                                                /*   pointed at by long */
                                                /*   ptr                */
#define LLGET(x) ( *((LONG_ALIAS *)(x)))

#endif  /* _COMPAT_H */
