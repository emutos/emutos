/*
 *  clock.h - general header file for clock drivers
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
**  TIME - structure for passing time and date info to and from drivers
*/

#define	TIME	struct _TimStruct

TIME
{
	SHORT	ti_sec ;	/*  seconds      (0-59)			*/
	SHORT	ti_min ;	/*  minutes      (0-59)			*/
	SHORT	ti_hour ;	/*  hours        (0-23)			*/
	SHORT   ti_daymo ;	/*  day of month (1-31)			*/
	SHORT	ti_daywk ;	/*  day of week  (1-7)		Sun = 1	*/
	SHORT	ti_mon ;	/*  month of year(1-12)			*/
	SHORT	ti_year ;	/*  yr of century(0-99)			*/
} ;

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
