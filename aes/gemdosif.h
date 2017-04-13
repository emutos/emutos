/*
 * EmuTOS AES: functions and variables implemened in gemdosif.S
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMDOSIF_H
#define GEMDOSIF_H


extern LONG     drwaddr;

extern void *   tikaddr;
extern void *   tiksav;

extern LONG     NUM_TICK;                       /* number of ticks      */
                                                /*   since last sample  */
                                                /*   while someone was  */
                                                /*   waiting            */
extern LONG     CMP_TICK;                       /* indicates to tick    */
                                                /*   handler how much   */
                                                /*   time to wait before*/
                                                /*   sending the first  */
                                                /*   tchange            */


extern void disable_interrupts(void);
extern void enable_interrupts(void);

extern void far_bcha(void);
extern void far_mcha(void);
extern void aes_wheel(void);
extern void justretf(void);

extern void unset_aestrap(void);
extern void set_aestrap(void);
extern BOOL aestrap_intercepted(void);

extern void takeerr(void);
extern void giveerr(void);
extern void retake(void);

extern void drawrat(WORD newx, WORD newy);


#endif
