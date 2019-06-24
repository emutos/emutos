/*      DOS.H           4/18/84 - 9/07/84       Lee Lorenzen            */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*       Copyright (C) 2002-2017 The EmuTOS development team
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

/* in bdos/bdosmain.c */
extern void osinit_before_xmaddalt(void);
extern void osinit_after_xmaddalt(void);
extern long osif(short *pw);
