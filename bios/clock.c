/*
 * clock.c - Realtime clock routines
 *
 * Copyright (c) 2001 Martin Doering
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include	"portab.h"
#include	"bios.h"
#include	"kprint.h"


/*==== Prototypes =========================================================*/

/*==== Defines ============================================================*/
#define CLK_BASE (0xfffffc20L)

#define	CLK	struct clkreg
CLK	/*  needs name for casting	*/
{
	BYTE fill0,  sec_l;     /* seconds elapsed in current minute */
	BYTE fill2,  sec_h;
	BYTE fill4,  min_l;	/* minutes elapsed in current hour   */
	BYTE fill6,  min_h;
	BYTE fill8,  hour_l;	/* hours elapsed in current day	     */
	BYTE fill10, hour_h;
	BYTE fill12, daywk;	/* day of week (1-7); sunday=1       */
	BYTE fill14, day_l;	/* day of month (1-31) */
	BYTE fill16, day_h;
	BYTE fill18, mon_l;	/* month of year (1-12) */
	BYTE fill20, mon_h;
	BYTE fill22, year_l;	/* year of century (0-99) */
	BYTE fill24, year_h;
	BYTE fill26, rega;	/* register A */
	BYTE fill28, regb;	/* register B */
	BYTE fill30, regc;	/* register C */
};

#define clk (*(volatile CLK*)CLK_BASE)


/*==== TIME - structure for passing time and date info ====================*/
/*
**  ti_year is the year in the current century.
**
**  the time indicated in this structure is the system base time.  If the
**	current implementation is a multi-user type system, or one in a
**	widely distributed network, then the base time should be something 
**	based on GMT, with the system converting to local time as needed.  
**	If, however, the implementation is a simple non-networking machine,
**	the base time may be local time.
**
**  the driver does not know anything about normal calendar arithmetic or
**	daylight savings time, or what century it is (that is up to the 
**	system, or anything other than keeping an increment in ms, seconds, 
**	minutes, etc from the base time with which it was
**	initialized.
*/

#define	TIME	struct _TimStruct

TIME
{
	BYTE 	ti_sec;	        /*  seconds      (0-59)			*/
	BYTE 	ti_min;	        /*  minutes      (0-59)			*/
	BYTE 	ti_hour;	/*  hours        (0-23)			*/
	BYTE    ti_daymo;	/*  day of month (1-31)			*/
	BYTE 	ti_daywk;	/*  day of week  (1-7)		Sun = 1	*/
	BYTE 	ti_mon;	        /*  month of year(1-12)			*/
	BYTE 	ti_year; 	/*  yr of century(0-99)			*/
};


/*==== Global variables ===================================================*/



/*==== clk_int - keyboard interrupt service routine =========================*/
/*
 *	clock Interrupt Service Routine for
 *	this routine is invoked upon receipt of an interrupt
 *	from the clock by the system.  it retrieves the
 *	convenient time.
 */
 
VOID	clkint(VOID)
{
    return;
}



/*==== clk_init - initialize the clock ====================================*/
ERROR	clk_init(VOID)
{
    /* no initialization needed - throughpassed to unix clock */
    return(SUCCESS);
}



/*==== clklox - clock as serial device ??? ================================*/

VOID	clklox(LONG flags, LONG ticks)
{
    return;
}



/*==== clk_gettime - get current time from emulator (Mega ST clock) =======*/
/*
**  returns:
**	TRUE if valid time returned
**	FALSE if fail.
*/

BOOLEAN	clk_gettime( REG TIME *t)
{

    t->ti_sec = (clk.sec_h<<4)|clk.sec_l ;
    t->ti_min = (clk.min_h<<4)|clk.min_l ;
    t->ti_hour = (clk.hour_h<<4)|clk.hour_l ;
    t->ti_daymo = (clk.day_h<<4)|clk.day_l ;
    t->ti_daywk = clk.daywk ;
    t->ti_mon = (clk.mon_h<<4)|clk.mon_l ;
    t->ti_year = (clk.year_h<<4)|clk.year_l ;

    return(TRUE);       /* in emulation always ok */
}

/*==== clk_settime - set the current clock ================================*/
BOOLEAN	clk_settime( REG TIME t )
{
    return(TRUE);       /* Emulator allows no write to clock registers */
}	

