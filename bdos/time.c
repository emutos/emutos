/*
 * time.c - GEMDOS time and date functions
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002-2015 The EmuTOS development team
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
 * BIOS interface
 * I didn't put it in a header because only this file is interested.
 */

/* the address of the vector in TOS vars */
extern void (*etv_timer)(int);

/*
 * private declarations
 */

static void tikfrk(int n);

static const BYTE nday[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* static long uptime; */

static int msec;



/*
 * xgetdate - Function 0x2A:  Get date
 */
long    xgetdate(void)
{
    /* call XBIOS and mask out*/
    current_date = (Gettime() >> 16) & 0xffff;
    return current_date;
}


/*
 * xsetdate - Function 0x2B:  Set date
 */
#define DAY_BM          0x001F
#define MTH_BM          0x01E0
#define YRS_BM          0xFE00
long xsetdate(UWORD d)
{
    UWORD curmo, day;

    curmo = ((d >> 5) & 0x0F);
    day = d & DAY_BM;

    if ((d >> 9) > 119)                 /* Warranty expires 12/31/2099 */
        return ERR;

    if (curmo > 12)                     /* 12 months a year */
        return ERR;

    if ((curmo == 2) && !(d & 0x0600))  /* Feb && Leap */
    {
        if (day > 29)
            return ERR;
    }
    else
        if (day > nday[curmo])
            return ERR;

    current_date = d;                   /* ok, assign that value to date */

    /* tell bios about new date */
    Settime((((ULONG)current_date)<<16)|((ULONG)current_time));

    return E_OK;
}



/*
 * xgettime - Function 0x2C:  Get time
 */
long    xgettime(void)
{
    /* call XBIOS and mask out*/
    current_time = Gettime() & 0xffff;
    return current_time;
}



/*
 * xsettime - Function 0x2D:  Set time
 */
/* Bit masks for the various fields in the time variable. */
#define SEC_BM          0x001F
#define MIN_BM          0x07E0
#define HRS_BM          0xF800
long xsettime(UWORD t)
{
    if ((t & SEC_BM) >= 30)
        return ERR;

    if ((t & MIN_BM) >= (60 << 5))      /* 60 max minutes per hour */
        return ERR;

    if ((t & HRS_BM) >= (24 << 11))     /* max of 24 hours in a day */
        return ERR;

    current_time = t;

    /* tell bios about new time */
    Settime((((ULONG)current_date)<<16)|((ULONG)current_time));

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

/*  uptime += n; */

    msec += n;
    if (msec >= 2000)
    {
        /* update time */

        msec -= 2000;
        current_time++;

        if ((current_time & 0x1F) != 30)
            return;

        current_time &= 0xFFE0;
        current_time += 0x0020;

        if ((current_time & 0x7E0) != (60 << 5))
            return;

        current_time &= 0xF81F;
        current_time += 0x0800;

        if ((current_time & 0xF800) != (24 << 11))
            return;

        current_time = 0;

        /* update date */

        if ((current_date & 0x001F) == 31)
            goto datok;

        current_date++;         /* bump day */

        if ((current_date & 0x001F) <= 28)
            return;

        if ((curmo = (current_date >> 5) & 0x0F) == 2)
        {
            /* 2100 is the next non-leap year divisible by 4, so OK */
            if (!(current_date & 0x0600)) {
                if ((current_date & 0x001F) <= 29)
                    return;
                else
                    goto datok;
            }
        }

        if ((current_date & 0x001F) <= nday[curmo])
            return;

    datok:
        current_date &= 0xFFE0; /* bump month */
        current_date += 0x0021;

        if ((current_date & 0x01E0) <= (12 << 5))
            return;

        current_date &= 0xFE00; /* bump year */
        current_date += 0x0221;
    }
}
