/*
 * time.c - GEMDOS time and date functions
 *
 * Copyright (c) 2001 Lineo, Inc.
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

#include "time.h"
#include "portab.h"
#include "gemerror.h"
#include "asm.h"

int date;
int time;

/*
 * BIOS interface  
 * I didn't put it in a header because only this file is interested.
 */

/* the address of the vector in TOS vars */
extern void (*etv_timer)(int);

#define date_time(op,var) trap13(0x11,(int)(op),(int*)(var))

#define GET_TIME        0
#define SET_TIME        1
#define GET_DATE        2
#define SET_DATE        3


/*
 * private declarations
 */

static void tikfrk(int n);

static int nday[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* static long uptime; */

static int msec;

/****************************/
/* Function 0x2A:  Get date */
/****************************/

long    xgetdate(void)
{
    date_time(GET_DATE, &date);         /* allow bios to update date */
    return date;
}


/****************************/
/* Function 0x2B:  Set date */
/****************************/


#define DAY_BM          0x001F
#define MTH_BM          0x01E0
#define YRS_BM          0xFE00


long    xsetdate(int d)
{
int     curmo, day;

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

    date = d;                           /* ok, assign that value to date */
    date_time(SET_DATE, &date);         /* tell bios about new date */

    return E_OK;
}


/****************************/
/* Function 0x2C:  Get time */
/****************************/

long    xgettime(void)
{
    date_time(GET_TIME, &time);         /* bios may update time if it wishes */
    return time;
}


/****************************/
/* Function 0x2D:  Set time */
/****************************/


/* Bit masks for the various fields in the time variable. */
#define SEC_BM          0x001F
#define MIN_BM          0x07E0
#define HRS_BM          0xF800


long    xsettime(int t)
{
    if ((t & SEC_BM) >= 30)
        return ERR;

    if ((t & MIN_BM) >= (60 << 5))      /* 60 max minutes per hour */
        return ERR;

    if ((t & HRS_BM) >= (24 << 11))     /* max of 24 hours in a day */
        return ERR;

    time = t;
    date_time(SET_TIME, &time);         /* tell bios about new time */

    return E_OK;
}


/*
 *  time_init
 */


void time_init(void)
{
    date_time(GET_DATE, &date);
    date_time(GET_TIME, &time);
    
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
        time++;

        if ((time & 0x1F) != 30)
            return;

        time &= 0xFFE0;
        time += 0x0020;

        if ((time & 0x7E0) != (60 << 5))
            return;

        time &= 0xF81F;
        time += 0x0800;

        if ((time & 0xF800) != (24 << 11))
            return;

        time = 0;

        /* update date */

        if ((date & 0x001F) == 31)
            goto datok;

        date++;                 /* bump day */

        if ((date & 0x001F) <= 28)
            return;

        if ((curmo = (date >> 5) & 0x0F) == 2)
        {
            /* 2100 is the next non-leap year divisible by 4, so OK */
            if (!(date & 0x0600)) {
                if ((date & 0x001F) <= 29)
                    return;
                else
                    goto datok;
            }
        }

        if ((date & 0x001F) <= nday[curmo])
            return;

    datok:
        date &= 0xFFE0;         /* bump month */
        date += 0x0021;

        if ((date & 0x01E0) <= (12 << 5))
            return;

        date &= 0xFE00;         /* bump year */
        date += 0x0221;
    }
}


