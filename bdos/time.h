/*
 * time.h - GEMDOS time and date functions
 *
 * Copyright (c) 2002 the EmuTOS Development Team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef TIME_H
#define TIME_H

extern int time, date;

long xgetdate(void);
long xsetdate(int d);
long xgettime(void);
long xsettime(int t);

/* called to initialize the module */
void time_init(void);

#endif /* TIME_H */
