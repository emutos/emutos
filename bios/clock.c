/*
 * clock.c - BIOS time and date routines
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "portab.h"
#include "bios.h"
#include "kprint.h"
#include "clock.h"
#include "ikbd.h"
#include "tosvars.h"
#include "btools.h"

/* set this to 1 to debug IKBD clock */
#define NO_MEGARTC 0

/* set this to 1 to alwaus use megartc */
#define NO_IKBD_CLOCK 1


/*==== External declarations ==============================================*/


/*==== variables ==========================================================*/

/* one if there is a MegaST real-time clock. */
static int has_megartc;


/*==== Prototypes =========================================================*/

static void megartc_init(void);

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
	BYTE filla,  hour_h;
	BYTE fillc,  daywk;	/* day of week (1-7); sunday=1       */
	BYTE fille,  day_l;	/* day of month (1-31) */
	BYTE fill10, day_h;
	BYTE fill12, mon_l;	/* month of year (1-12) */
	BYTE fill14, mon_h;
	BYTE fill16, year_l;	/* year of century (0-99) */
	BYTE fill18, year_h;
	BYTE fill1a, rega;	/* register A */
	BYTE fill1c, regb;	/* register B */
	BYTE fill1e, regc;	/* register C */
};

#define clk (*(volatile CLK*)CLK_BASE)

struct myclkreg {
	BYTE sec_l;     /* seconds elapsed in current minute */
	BYTE sec_h;
	BYTE min_l;	/* minutes elapsed in current hour   */
	BYTE min_h;
	BYTE hour_l;	/* hours elapsed in current day	     */
	BYTE hour_h;
	BYTE daywk;	/* day of week (1-7); sunday=1       */
	BYTE day_l;	/* day of month (1-31) */
	BYTE day_h;
	BYTE mon_l;	/* month of year (1-12) */
	BYTE mon_h;
	BYTE year_l;	/* year of century (0-99) */
	BYTE year_h;
};

/* buffers to hols the megartc regs */
static struct myclkreg clkregs1, clkregs2;


/*==== MegaRTC section ====================================================*/

static void megartc_init(void)
{
  /* detect megartc. 
   * I do it exactly like TOS 1.2 does, not trying to understand...
   */
#if NO_IKBD_CLOCK
  has_megartc = 1;
#else
#if NO_MEGARTC
  has_megartc = 0;
#else
  clk.rega = 9;
  clk.min_l = 10;
  clk.min_h = 5;
  if((clk.min_l != 10) || (clk.min_h != 5)) {
    has_megartc = 0;
  } else {
    has_megartc = 1;
    clk.sec_l = 1;
    clk.rega = 8;
    clk.regb = 0;
  }
#endif /* NO_MEGARTC */
#endif /* NO_IKBD_CLOCK */
}

/*==== MegaRTC internal functions =========================================*/

/*
 * MegaRTC, TODO:
 * - leap year ?
 */

/* read the 13 non-control clock registers into clkregs1
 * read the reigisters twice, and returns only when the two reads 
 * returned the same value.
 * This is because the MegaRTC clock is a very slow chip (32768 kHz)
 * and presumably the carry is not reported instantly when the
 * time changes!!! (this is LVL interpretation, any other reason 
 * is welcome.)
 */

static void mgetregs(void)
{
  int i;
  BYTE *a, *b, *c;
  do {
    c = (BYTE *) &clk.sec_l;
    a = (BYTE *) &clkregs1.sec_l;
    for (i = 0 ; i < 13 ; i++) {
      *a++ = *c;
      c += 2;
    }
    c = (BYTE *) &clk.sec_l;
    b = (BYTE *) &clkregs2.sec_l;
    a = (BYTE *) &clkregs1.sec_l;
    for (i = 0 ; i < 13 ; i++) {
      *b = *c;
      if(*b++ != *a++) break;
      c += 2;
    }
  } while(i != 13);
}

static void msetregs(void)
{
  int i;
  BYTE *a, *c;
  c = (BYTE *) &clk.sec_l;
  a = (BYTE *) &clkregs1.sec_l;
  for (i = 0 ; i < 13 ; i++) {
    *c = *a++;
    c += 2;
  }
}


static void mdosettime(UWORD time)
{
  clkregs1.sec_l = (time * 2) % 10;
  clkregs1.sec_h = (time & 0x1F) / 5;
  clkregs1.min_l = ((time >> 5) & 0x2F) % 10;
  clkregs1.min_h = ((time >> 5) & 0x2F) / 10;
  clkregs1.hour_l = ((time >> 11) & 0x1F) % 10;
  clkregs1.hour_h = ((time >> 11) & 0x1F) / 10;
}


static UWORD mdogettime(void)
{
  UWORD time;

  time = ((clkregs1.sec_l & 0xf) + 10 * (clkregs1.sec_h & 0xF)) 
    |  (((clkregs1.min_l & 0xf) + 10 * (clkregs1.min_h & 0xf)) << 5)
    |  (((clkregs1.hour_l & 0xf) + 10 * (clkregs1.hour_h & 0xf)) << 11) ;
 
  return time;
}


static void mdosetdate(UWORD date)
{
  clkregs1.day_l = (date & 0x1F) % 10;
  clkregs1.day_h = (date & 0x1F) / 10;
  clkregs1.mon_l = ((date >> 5) & 0xF) % 10;
  clkregs1.mon_h = ((date >> 5) & 0xF) % 10;
  clkregs1.year_l = (date >> 9) % 10;
  clkregs1.year_h = (date >> 9) / 10;  
}

static UWORD mdogetdate(void)
{
  UWORD date;

  date = ((clkregs1.day_l & 0xf) + 10 * (clkregs1.day_h & 0xf))
    |  (((clkregs1.mon_l & 0xf) + 10 * (clkregs1.mon_h & 0xf)) << 5) 
    |  (((clkregs1.year_l & 0xf) + 10 * (clkregs1.year_h & 0xf)) << 9) ;
 
  return date;
}

/*==== MegaRTC high-level functions ========================================*/

static void msettime(UWORD time)
{
  mgetregs();
  mdosettime(time);
  msetregs();
}

static UWORD mgettime(void)
{
  mgetregs();
  return mdogettime();
}

static void msetdate(UWORD date)
{
  mgetregs();
  mdosetdate(date);
  msetregs();
}

static UWORD mgetdate(void)
{
  mgetregs();
  return mdogetdate();
}

static ULONG mgetdt(void)
{
  mgetregs();
  return (((ULONG) mdogetdate()) << 16) | mdogettime();
}

static void msetdt(ULONG dt)
{
  mdosetdate(dt>>16);
  mdosettime(dt);
  msetregs();
}

/*==== IKBD clock section =================================================*/

static struct ikbdregs {
  UBYTE cmd;
  UBYTE year;
  UBYTE month;
  UBYTE day;
  UBYTE hour;
  UBYTE min;
  UBYTE sec;
} iclkbuf;

static volatile WORD iclk_ready;

/* called by the ACIA interrupt */
void clockvec(BYTE *buf)
{
  BYTE *b = 1 + ((BYTE *)&iclkbuf);
  memmove(b, buf, 6);
  iclk_ready = 1;
}

static inline UBYTE int2bcd(UWORD a)
{
  return (a % 10) + ((a / 10) << 4);
}

static inline UWORD bcd2int(UBYTE a)
{
  return (a & 0xF) + ((a >> 4) * 10);
}

/*==== Ikbd clock internal functions ======================================*/


static void igetregs(void)
{
  iclk_ready = 0;
  iclkbuf.cmd = 0x1C;
  ikbdws(0, (LONG) &iclkbuf);
  /* wait until the interrupt receives the full packet */
  while(! iclk_ready) 
    ;

  kprintf("iclkbuf: year = %x, month = %x\n", iclkbuf.year, iclkbuf.month);
}

static void iresetregs(void)
{
  iclkbuf.cmd   = 0x1B;
  iclkbuf.year  = 0xFF;
  iclkbuf.month = 0xFF;
  iclkbuf.day   = 0xFF;
  iclkbuf.hour  = 0xFF;
  iclkbuf.min   = 0xFF;
  iclkbuf.sec   = 0xFF;
}

static void isetregs(void)
{
  iclkbuf.cmd = 0x1B;
  ikbdws(6, (LONG) &iclkbuf);
}

static UWORD idogetdate(void)
{
  UWORD date;

  date = ( bcd2int(iclkbuf.year) << 9) 
    | ( bcd2int(iclkbuf.month) << 5 ) | bcd2int(iclkbuf.day);
  return date;
}

static UWORD idogettime(void)
{
  UWORD time;

  time = ( bcd2int(iclkbuf.sec) >> 1 )
    | ( bcd2int(iclkbuf.min) << 5 ) | ( bcd2int(iclkbuf.hour) << 11 ) ;
  return time;
}

static void idosettime(UWORD time) 
{
  iclkbuf.sec = int2bcd( (time << 1) & 0x3f );
  iclkbuf.min = int2bcd( (time >> 5) & 0x3f );
  iclkbuf.hour = int2bcd( (time >> 11) & 0x1f );
}

static void idosetdate(UWORD date) 
{
  iclkbuf.year = int2bcd( (date >> 9) & 0x7f );
  iclkbuf.month = int2bcd( (date >> 5) & 0xf );
  iclkbuf.day = int2bcd( date & 0x1f );
}


/*==== Ikbd Clock high-level functions ====================================*/

static void isettime(UWORD time)
{
  iresetregs();
  idosettime(time);
  isetregs();
}

static UWORD igettime(void)
{
  igetregs();
  return idogettime();
}

static void isetdate(UWORD date)
{
  iresetregs();
  idosetdate(date);
  isetregs();
}

static UWORD igetdate(void)
{
  igetregs();
  return idogetdate();
}

static ULONG igetdt(void)
{
  igetregs();
  return (((ULONG)idogetdate()) << 16) | idogettime();
}

static void isetdt(ULONG dt)
{
  idosetdate(dt>>16);
  idosettime(dt);
  isetregs();
}


#if 0 /* old stuff */

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


#endif /* old stuff */


void date_time(WORD flag, WORD *dt)
{
  if(has_megartc) {
    switch(flag) {
    case GET_DATE:
      *dt = mgetdate();
      break;
    case SET_DATE:
      msetdate(*dt);
      break;
    case GET_TIME:
      *dt = mgettime();
      break;
    case SET_TIME:
      msettime(*dt);
      break;
    }
  } else {
    switch(flag) {
    case GET_DATE:
      *dt = igetdate();
      break;
    case SET_DATE:
      isetdate(*dt);
      break;
    case GET_TIME:
      *dt = igettime();
      break;
    case SET_TIME:
      isettime(*dt);
      break;
    }
  }
}

/* internal init */

void clock_init(void)
{
  megartc_init();
  if( ! has_megartc ) {
    /* no megartc, the best we can do is set the date to the
     * OS creation date, time 0.
     */
    isetdt(((ULONG) os_dosdate) << 16);
  }
}

/* xbios functions */

void settime(LONG time)
{
  if(has_megartc) {
    msetdt(time);
  } else {
    isetdt(time);
  }
}

LONG gettime(void)
{
  if(has_megartc) {
    return mgetdt();
  } else {
    return igetdt();
  }
}


