/*
 * console.h - console header
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef CONSOLE_H
#define CONSOLE_H

/******************************************
**
** BDOS level character device file handles
**
*******************************************
*/

#define H_Console       -1
#define H_Aux           -2
#define H_Print         -3


/****************************************
**
** Character device handle conversion
** (BDOS-type handle to BIOS-type handle)
**
*****************************************
*/

#define HXFORM(h)       (3+h)



extern  int     add[3];
extern  int     remove[3];

long xconstat(void);
long xconostat(void);
long xprtostat(void);
long xauxistat(void);
long xauxostat(void);
void xtabout(int ch);
long xauxout(int ch);
long xprtout(int ch);
long x7in(void);
long xconin(void);
long x8in(void);
long xauxin(void);
long rawconio(int parm);
void xprt_line(char *p);
void readline(char *p);
int cgets(int h, int maxlen, char *buf);
long conin(int h); 
void tabout(int h, int ch);



#endif /* CONSOLE_H */
