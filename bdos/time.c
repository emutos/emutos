/*
 * time.c - GEMDOS time and date functions
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


/*
Originally written by SCC  25 Mar 85.

MODIFICATION HISTORY

        22 Apr 85       SCC     Modified returns of xgetdate() and xgettime() to longs
                                rather than ints.

         9 May 85       EWF     Added in checking for valid date/time setting

        13 May 85       SCC     Modified xsetdate() and xsettime() to return ERR
                                rather than BADDATE or BADTIME.  Also changed some
                                bourdary checks in the tests and optimized some code.

        23 Jul 85       SCC     Modified xsetdate() to allow setting to 29 Feb on leap
                                year (from correction from Landon Dyer @ Atari).

        24 Jul 85       SCC     Modified xsetdate().  Fix of 23 Jul 85 was not correct.

        26 Jun 87       ACH     Introduced new bios function to support
                                hardware real-time clock calendars.

NAMES

        SCC     Steven C. Cavender
        EWF     Eric W. Fleischman
        ACH     Anthony C. Hay (DR UK)
*/

#include "config.h"
#include "time.h"
#include "portab.h"
#include "gemerror.h"
#include "xbiosbind.h"


/*
 * globals: current time and date
 */
UWORD current_time, current_date;

/*
 *  bit masks for the various fields in the time & date variables
 */
#define SEC_BM          0x001F
#define MIN_BM          0x07E0
#define HRS_BM          0xF800
#define DAY_BM          0x001F
#define MTH_BM          0x01E0
#define YRS_BM          0xFE00

/*
 *  shift values for the same
 */
#define MIN_SHIFT       5
#define HRS_SHIFT       11
#define MTH_SHIFT       5
#define YRS_SHIFT       9

/* macro to test standard date format for leap year */
#define IS_A_LEAP_YEAR(a)   (!(a&0x0600))

/*
 * BIOS interface
 * I didn't put it in a header because only this file is interested.
 */

/* the address of the vector in TOS vars */
extern void (*etv_timer)(int);

/*
 * private declarations
 */

static void tikfrk(int n);

static const BYTE nday_norm[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const BYTE nday_leap[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* static long uptime; */

static int msec;



/*
 * xgetdate - Function 0x2A:  Get date
 */
long xgetdate(void)
{
    return current_date;
}


/*
 * xsetdate - Function 0x2B:  Set date
 */
long xsetdate(UWORD d)
{
    UWORD curmo, day;
    const BYTE *nday = IS_A_LEAP_YEAR(d) ? nday_leap : nday_norm;

    curmo = (d & MTH_BM) >> MTH_SHIFT;
    day = d & DAY_BM;

    if (((d >> YRS_SHIFT) > 119)        /* Warranty expires 12/31/2099 */
     || (curmo > 12)                    /* 12 months a year */
     || (day > nday[curmo]))            /* variable days/month */
        return ERR;

    current_date = d;                   /* ok, assign that value to date */

    /* tell bios about new date */
    Settime(MAKE_ULONG(current_date, current_time));

    return E_OK;
}



/*
 * xgettime - Function 0x2C:  Get time
 */
long xgettime(void)
{
    return current_time;
}



/*
 * xsettime - Function 0x2D:  Set time
 */
long xsettime(UWORD t)
{
    if (((t & SEC_BM) >= 30)                /* 30 "double-seconds" per minute */
     || ((t & MIN_BM) >= (60 << MIN_SHIFT)) /* 60 minutes per hour */
     || ((t & HRS_BM) >= (24 << HRS_SHIFT)))/* 24 hours per day */
        return ERR;

    current_time = t;

    /* tell bios about new time */
    Settime(MAKE_ULONG(current_date, current_time));

    return E_OK;
}


/*
 *  time_init
 */

void time_init(void)
{
    ULONG dt = Gettime();

    current_date = (dt >> 16) & 0xffff;
    current_time = dt & 0xffff;

    etv_timer = tikfrk;
}


/*
 *  tikfrk -
 */

static void tikfrk(int n)
{
    int curmo;
    const BYTE *nday;

/*  uptime += n; */

    msec += n;
    if (msec < 2000)
        return;

    /* update seconds */

    msec -= 2000;
    current_time++;
    if ((current_time & SEC_BM) != 30)
        return;

    /* handle minute rollover */

    current_time &= ~SEC_BM;
    current_time += (1 << MIN_SHIFT);
    if ((current_time & MIN_BM) != (60 << MIN_SHIFT))
        return;

    /* handle hour rollover */

    current_time &= ~MIN_BM;
    current_time += (1 << HRS_SHIFT);
    if ((current_time & HRS_BM) != (24 << HRS_SHIFT))
        return;

    /* handle day rollover */

    nday = IS_A_LEAP_YEAR(current_date) ? nday_leap : nday_norm;
    curmo = (current_date & MTH_BM) >> MTH_SHIFT;

    current_time = 0;
    current_date++;
    if ((current_date & DAY_BM) <= nday[curmo])
        return;

    /* handle month rollover */

    current_date &= ~DAY_BM;
    current_date += (1 << MTH_SHIFT) + 1;
    if ((current_date & MTH_BM) <= (12 << MTH_SHIFT))
        return;

    /* handle year rollover */
    current_date &= YRS_BM;
    current_date += (1 << YRS_SHIFT) + (1 << MTH_SHIFT) + 1;
}
