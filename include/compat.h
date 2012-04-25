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
*                 2002 The EmuTOS development team
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


                                                /* Use memcpy to copy bytes: */
#define LWCOPY(dest,src,len) memcpy((void *)(dest), (void*)(src), (len)*2)
                                                /* Use memcpy to copy bytes: */
#define LBCOPY(dest,src,len) memcpy((void *)(dest), (void *)(src), (len))

#define LSTRLEN(p) strlen((char *)p)

extern WORD LBWMOV(WORD *pdst, BYTE *psrc);

extern WORD  LSTCPY(LONG pdst, LONG psrc);
                                                /* coerce short ptr to  */
                                                /*   low word  of long  */
#define LW(x) ( (LONG)((UWORD)(x)) )
                                                /* coerce short ptr to  */
                                                /*   high word  of long */
#define HW(x) ((LONG)((UWORD)(x)) << 16)

                                                /* return low word of   */
                                                /*   a long value       */
#define LLOWD(x) ((UWORD)(x))
                                                /* return high word of  */
                                                /*   a long value       */
#define LHIWD(x) ((UWORD)(x >> 16))

                                                /* return low byte of   */
                                                /*   a word value       */
#define LLOBT(x) ((BYTE)(x & 0x00ff))
                                                /* return high byte of  */
                                                /*   a word value       */
#define LHIBT(x) ((BYTE)( (x >> 8) & 0x00ff))


/************************************************************************/


                                                /* return short pointer */
                                                /* from long address    */
#define LPOINTER /**/
                                                /* return a long address*/
                                                /*   of a short pointer */
//#define ADDR (long) /*!!!*/
#define ADDR(x) ((LONG) (x))


                                                /* return a single byte */
                                                /*   pointed at by long */
                                                /*   ptr                */
#define LBGET(x) ( (UBYTE) *((BYTE * )(x)) )
                                                /* set a single byte    */
                                                /*   pointed at by long */
                                                /*   ptr, LBSET(lp, bt) */
#define LBSET(x, y)  ( *((BYTE *)(x)) = y)
                                                /* return a single word */
                                                /*   pointed at by long */
                                                /*   ptr                */
#define LWGET(x) ( (WORD) *((WORD *)(x)) )
                                                /* set a single word    */
                                                /*   pointed at by long */
                                                /*   ptr, LWSET(lp, bt) */
#define LWSET(x, y)  ( *((WORD *)(x)) = y)

                                                /* return a single long */
                                                /*   pointed at by long */
                                                /*   ptr                */
#define LLGET(x) ( *((LONG *)(x)))
                                                /* set a single long    */
                                                /*   pointed at by long */
                                                /*   ptr, LLSET(lp, bt) */
#define LLSET(x, y) ( *((LONG *)(x)) = y)

                                                /* return 0th byte of   */
                                                /*   a long value given */
                                                /*   a short pointer to */
                                                /*   the long value     */
#define LBYTE0(x) ( *((x)+3) )
                                                /* return 1st byte of   */
                                                /*   a long value given */
                                                /*   a short pointer to */
                                                /*   the long value     */
#define LBYTE1(x) ( *((x)+2) )
                                                /* return 2nd byte of   */
                                                /*   a long value given */
                                                /*   a short pointer to */
                                                /*   the long value     */
#define LBYTE2(x) ( *((x)+1) )
                                                /* return 3rd byte of   */
                                                /*   a long value given */
                                                /*   a short pointer to */
                                                /*   the long value     */
#define LBYTE3(x) (*(x))


#define movs(num, ps, pd)  memcpy((char *)pd, (const char *)ps, num)

#endif  /* _COMPAT_H */
