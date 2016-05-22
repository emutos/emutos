/*
 * clock.c - BIOS time and date routines
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *  THH   Thomas Huth
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "kprint.h"
#include "clock.h"
#include "ikbd.h"
#include "mfp.h"
#include "tosvars.h"
#include "string.h"
#include "vectors.h"
#include "nvram.h"
#include "machine.h"
#include "cookie.h"
#include "asm.h"
#include "dma.h"
#include "delay.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

/* Date/Time to use when the hardware clock is not set.
 * We use the OS creation date at 00:00:00
 */
#define DEFAULT_DATETIME ((ULONG)os_dosdate << 16)

#if CONF_WITH_ICDRTC

/*==== ICD AdSCSI Plus ST RTC section =====================================*/

/* one if there is an ICD real-time clock. */
int has_icdrtc;

#define NUM_ICDRTC_REGS    13  /* number of registers to copy to/from rtc */

/*
 * this is an internal-only structure, used to hold the RTC values
 * before they are transferred to the RTC chip.  it must be organised
 * in the same sequence as the chip registers themselves.
 */
struct icdclkreg
{
    UBYTE sec_l;                /* seconds elapsed in current minute */
    UBYTE sec_h;
    UBYTE min_l;                /* minutes elapsed in current hour   */
    UBYTE min_h;
    UBYTE hour_l;               /* hours elapsed in current day      */
    UBYTE hour_h;
    UBYTE daywk;                /* day of week (0-6); NOT USED */
    UBYTE day_l;                /* day of month (1-31) */
    UBYTE day_h;
    UBYTE mon_l;                /* month of year (1-12) */
    UBYTE mon_h;
    UBYTE year_l;               /* year of century (0-99) */
    UBYTE year_h;
};

/*
 * bits in above fields
 */
#define ICDRTC_24       0x08    /* specifies 24-hour clock; in 'hour_h' */

/*
 * bit masks for RTC chip communication
 */
#define ICDRTC_BEGIN    0xc0
#define ICDRTC_END      0x40
#define ICDRTC_SELECT   0x20
#define ICDRTC_WRITE    0x10
#define ICDRTC_READ     0x8f

/*
 * delay_related
 */
static ULONG loopcount_1_usec;
#define DELAY_1_USEC()  delay_loop(loopcount_1_usec)

/*
 * some prototypes
 */
static WORD icd_clock_begin(void);
static void icd_clock_end(void);

void detect_icdrtc(void)
{
    has_icdrtc = 0;

    if (icd_clock_begin())
        has_icdrtc = 1;

    icd_clock_end();

    loopcount_1_usec = loopcount_1_msec / 1000;
}

/*==== ICD RTC internal functions =========================================*/

/*
 * initialise access to clock
 */
static WORD icd_clock_begin(void)
{
    flock = 1;      /* prevent access */

    DMA->control = DMA_DRQ_FLOPPY | DMA_CS_ACSI;
    DMA->data = ICDRTC_BEGIN;
    DMA->control = DMA_DRQ_FLOPPY | DMA_CS_ACSI | DMA_NOT_NEWCDB;

    DELAY_1_USEC();     /* ICD drivers delay by about 0.5 usec */

    return (MFP_BASE->gpip&0x20) ? 0 : 1;
}

/*
 * terminate access to clock
 */
static void icd_clock_end(void)
{
    DMA->data = ICDRTC_END;
    DMA->control = DMA_DRQ_FLOPPY;

    flock = 0;      /* allow access */
}

/*
 * read the clock registers into clkregs
 */
static void icdgetregs(struct icdclkreg *clkregs)
{
    WORD i;
    UWORD send;
    UBYTE *reg = (UBYTE *)clkregs;

    if (!icd_clock_begin())
        KDEBUG(("icdgetregs(): icd_clock_begin() failed\n"));

    for (i = 0; i < NUM_ICDRTC_REGS; i++)
    {
        send = ICDRTC_BEGIN | i;        /* specify register */
        DMA->data = send;
        send |= ICDRTC_SELECT;          /* toggle select bit */
        DMA->data = send;
        send &= ~ICDRTC_SELECT;
        DMA->data = send;
        send &= ICDRTC_READ;            /* prepare to read */
        DMA->data = send;

        DELAY_1_USEC();

        *reg++ = DMA->data & 0x0f;      /* read */
        DMA->data = ICDRTC_BEGIN;
    }

    icd_clock_end();
}

/*
 * write the ICD registers from clkregs
 */
static void icdsetregs(struct icdclkreg *clkregs)
{
    WORD i;
    UWORD send;
    UBYTE *reg = (UBYTE *)clkregs;

    if (!icd_clock_begin())
        KDEBUG(("icdsetregs(): icd_clock_begin() failed\n"));

    for (i = 0; i < NUM_ICDRTC_REGS; i++)
    {
        send = ICDRTC_BEGIN | i;        /* specify register */
        DMA->data = send;
        send |= ICDRTC_SELECT;
        DMA->data = send;
        send &= ~ICDRTC_SELECT;
        DMA->data = send;

        DELAY_1_USEC();

        send = ICDRTC_BEGIN | *reg++;
        DMA->data = send;
        send |= ICDRTC_WRITE;           /* prepare to write */
        DMA->data = send;
        send &= ~ICDRTC_WRITE;
        DMA->data = send;               /* write */
    }
}

static void icdsettime(struct icdclkreg *clkregs,UWORD time)
{
    UWORD hr, min, sec;

    hr = (time >> 11) & 0x1f;
    min = (time >> 5) & 0x3f;
    sec = (time & 0x1f) << 1;

    /*
     * we must set the '24-hour clock' indicator in the tens-of-hours register
     */
    clkregs->sec_l = sec % 10;
    clkregs->sec_h = sec / 10;
    clkregs->min_l = min % 10;
    clkregs->min_h = min / 10;
    clkregs->hour_l = hr % 10;
    clkregs->hour_h = (hr / 10) | ICDRTC_24;

    KDEBUG(("icdsettime() %x%x:%x%x:%x%x\n", clkregs->hour_h, clkregs->hour_l,
            clkregs->min_h, clkregs->min_l, clkregs->sec_h, clkregs->sec_l));
}

static UWORD icdgettime(struct icdclkreg *clkregs)
{
    UWORD time;

    KDEBUG(("icdgettime() %x%x:%x%x:%x%x\n", clkregs->hour_h, clkregs->hour_l,
            clkregs->min_h, clkregs->min_l, clkregs->sec_h, clkregs->sec_l));

    /*
     * bits 2-3 of the tens-of-hours register must be ignored, since they
     * relate to time format (12-hr vs 24-hr)
     */
    time = ((clkregs->sec_l + 10*clkregs->sec_h) >> 1)
            | ((clkregs->min_l + 10*clkregs->min_h) << 5)
            | ((clkregs->hour_l + 10*(clkregs->hour_h & 0x03)) << 11);

    return time;
}

static void icdsetdate(struct icdclkreg *clkregs,UWORD date)
{
    UWORD year, month, day;

    /*
     * The ICD RTC stores the year as the offset from 1900; thus we need
     * to add 80 to the DOS-format year to get the RTC year.
     *
     * Note: despite the chip documentation, the leap year mechanism does
     * not seem to utilise bits 2-3 of the tens-of-days register.  Instead,
     * it appears to use the year value, i.e. if the year is divisible by
     * 4, then it is assumed to be a leap year.
     */
    year = (date >> 9) + 80;
    month = (date >> 5) & 0x0f;
    day = date & 0x1f;

    clkregs->day_l = day % 10;
    clkregs->day_h = (day / 10);
    clkregs->mon_l = month % 10;
    clkregs->mon_h = month / 10;
    clkregs->year_l = year % 10;
    clkregs->year_h = year / 10;

    KDEBUG(("icdsetdate() %x%x/%x%x/%x%x\n", clkregs->year_h, clkregs->year_l,
            clkregs->mon_h, clkregs->mon_l, clkregs->day_h, clkregs->day_l));
}

static UWORD icdgetdate(struct icdclkreg *clkregs)
{
    UWORD date;

    KDEBUG(("icdgetdate() %x%x/%x%x/%x%x\n", clkregs->year_h, clkregs->year_l,
            clkregs->mon_h, clkregs->mon_l, clkregs->day_h, clkregs->day_l));

    /*
     * The ICD RTC stores the year as the offset from 1900; thus we need
     * to subtract 80 from the RTC year to get the DOS-format year.  Also,
     * we ignore bits 2-3 of the tens-of-days register for safety (they
     * may be set and will retain their setting, even though they do not
     * seem to do anything).
     *
     * Note: despite the chip documentation, the tens-of-years register
     * can have a maximum value of 0x0f, allowing a maximum years value
     * of 15*10+9, which corresponds to year 2059.  As you would expect,
     * 2059/12/31 23:59:59 wraps to 1901/01/01 00:00:00 (tested on real
     * hardware).
     */
    date = (clkregs->day_l + 10*(clkregs->day_h&0x03))
            | ((clkregs->mon_l + 10*clkregs->mon_h) << 5)
            | ((clkregs->year_l + 10*clkregs->year_h - 80) << 9);

    return date;
}

/*==== ICD RTC high-level functions ========================================*/

static ULONG icdgetdt(void)
{
    struct icdclkreg clkregs;

    icdgetregs(&clkregs);

    return (((ULONG) icdgetdate(&clkregs)) << 16) | icdgettime(&clkregs);
}

static void icdsetdt(ULONG dt)
{
    struct icdclkreg clkregs;

    icdsetdate(&clkregs,dt>>16);
    icdsettime(&clkregs,dt);
    icdsetregs(&clkregs);
}

#endif /* CONF_WITH_ICDRTC */

#if CONF_WITH_MEGARTC

/*==== MegaRTC section ====================================================*/

/* one if there is a MegaST real-time clock. */
int has_megartc;

#define CLK_BASE (0xfffffc20L)

#define NUM_MEGARTC_REGS    13  /* number of registers to copy to/from rtc */

/*
 * the structure of the registers in the MegaST RTC (an RP5C15)
 *
 * there are two banks of registers, selectable via a bit in the mode register.
 * bank0 is used for most functions; bank1 contains the alarm registers (not
 * useful in the MegaST since the ALARM output pin is not connected) and some
 * other miscellaneous functions.
 */
struct megartc_bank0
{
    UBYTE fill0,  sec_l;    /* seconds elapsed in current minute */
    UBYTE fill2,  sec_h;
    UBYTE fill4,  min_l;    /* minutes elapsed in current hour   */
    UBYTE fill6,  min_h;
    UBYTE fill8,  hour_l;   /* hours elapsed in current day      */
    UBYTE filla,  hour_h;
    UBYTE fillc,  daywk;    /* day of week (1-7); sunday=1  NOT USED */
    UBYTE fille,  day_l;    /* day of month (1-31) */
    UBYTE fill10, day_h;
    UBYTE fill12, mon_l;    /* month of year (1-12) */
    UBYTE fill14, mon_h;
    UBYTE fill16, year_l;   /* year of century (0-99) */
    UBYTE fill18, year_h;
    UBYTE fill1a, mode;     /* timer/alarm enable, bank select */
    UBYTE fill1c, test;     /* test */
    UBYTE fill1e, misc;     /* clock pulse / reset */
};
struct megartc_bank1
{
    UBYTE fill0,  clkout;   /* clock output select */
    UBYTE fill2,  adjust;   /* adjust, not used by us */
    UBYTE fill4,  alarm_min_l;  /* alarm minutes */
    UBYTE fill6,  alarm_min_h;
    UBYTE fill8[12];        /* other alarm registers / unused */
    UBYTE fill14, sel_24;   /* select 12hr (0x00) or 24hr (0x01) */
    UBYTE fill16, leap_yr;  /* 0-3; 0 => this is a leap year */
    UBYTE fill18[2];        /* unused */
    UBYTE fill1a, mode;     /* timer/alarm enable, bank select */
    UBYTE fill1c, test;     /* test */
    UBYTE fill1e, misc;     /* clock pulse / reset */
};
/*
 * bit masks for above registers
 */
#define SELECT_BANK1    0x01    /* in 'mode' */
#define ENABLE_ALARM    0x04
#define ENABLE_TIMER    0x08
#define SELECT_24HR     0x01    /* in 'sel_24' */

#define CLKOUT_16KHZ    0x01    /* value to store in 'clkout' */

union megartc_clk
{
    struct megartc_bank0 bank0;
    struct megartc_bank1 bank1;
};

#define clk (*(volatile union megartc_clk *)CLK_BASE)

/*
 * this is an internal-only structure, used to hold the RTC values
 * before they are transferred to the RTC chip
 */
struct myclkreg
{
    UBYTE sec_l;            /* seconds elapsed in current minute */
    UBYTE sec_h;
    UBYTE min_l;            /* minutes elapsed in current hour   */
    UBYTE min_h;
    UBYTE hour_l;           /* hours elapsed in current day      */
    UBYTE hour_h;
    UBYTE daywk;            /* day of week (1-7); sunday=1       */
    UBYTE day_l;            /* day of month (1-31) */
    UBYTE day_h;
    UBYTE mon_l;            /* month of year (1-12) */
    UBYTE mon_h;
    UBYTE year_l;           /* year of century (0-99) */
    UBYTE year_h;
};


void detect_megartc(void)
{
    has_megartc = 0;

    /* first check if the address is valid */
    if (check_read_byte(CLK_BASE+1))
    {
        clk.bank1.mode |= SELECT_BANK1; /* verify like TOS does */
        clk.bank1.alarm_min_l = 0x0a;
        clk.bank1.alarm_min_h = 0x05;
        if (((clk.bank1.alarm_min_l&0x0f) == 0x0a)
         && ((clk.bank1.alarm_min_h&0x0f) == 0x05))
        {
            has_megartc = 1;
            /* set some default values in bank1 */
            clk.bank1.clkout = CLKOUT_16KHZ;
            clk.bank1.test = 0;
            clk.bank1.mode &= ~ENABLE_ALARM;
        }
        clk.bank1.mode &= ~SELECT_BANK1;
    }
}

/*==== MegaRTC internal functions =========================================*/

/*
 * MegaRTC
 */

/* read the 13 non-control clock registers into clkregs
 *
 * Note:just like TOS, reads the registers twice, and returns only
 * when the two reads obtain the same values.
 * This is because the MegaRTC clock is a very slow chip (32.768 kHz)
 * and presumably the carry is not reported instantly when the
 * time changes!!! (this is LVL interpretation, any other reason
 * is welcome.)
 */
static void mgetregs(struct myclkreg *clkregs)
{
    WORD i;
    UBYTE *buf1, *buf2, *regs;
    struct myclkreg clkcopy;

    clk.bank0.mode &= ~SELECT_BANK1;    /* ensure bank 0 */

    do
    {
        regs = (UBYTE *)&clk.bank0.sec_l;
        buf1 = (UBYTE *)&clkregs->sec_l;
        for (i = 0; i < NUM_MEGARTC_REGS; i++)
        {
            *buf1++ = *regs;
            regs += 2;
        }

        regs = (UBYTE *)&clk.bank0.sec_l;
        buf1 = (UBYTE *)&clkregs->sec_l;
        buf2 = (UBYTE *)&clkcopy.sec_l;
        for (i = 0; i < NUM_MEGARTC_REGS; i++)
        {
            *buf2 = *regs;
            if (*buf2++ != *buf1++)
                break;
            regs += 2;
        }
    } while(i != NUM_MEGARTC_REGS);
}

static void msetregs(struct myclkreg *clkregs)
{
    WORD i;
    UBYTE *buf, *regs;

    /*
     * set required bank1 register contents
     */
    clk.bank1.mode |= SELECT_BANK1;     /* select bank 1 */
    clk.bank1.sel_24 |= SELECT_24HR;    /* set 24-hour clock */
                                        /* set leap-year counter */
    clk.bank1.leap_yr = (clkregs->year_h*10 + clkregs->year_l) & 0x03;
    clk.bank1.mode &= ~SELECT_BANK1;    /* select bank 0 */

    /*
     * copy values from holding area to bank0
     */
    clk.bank0.mode &= ~ENABLE_TIMER;    /* disable timer */
    regs = (UBYTE *)&clk.bank0.sec_l;
    buf = (UBYTE *)&clkregs->sec_l;
    for (i = 0; i < NUM_MEGARTC_REGS; i++)
    {
        *regs = *buf++;
        regs += 2;
    }
    clk.bank0.mode |= ENABLE_TIMER;     /* enable timer */
}

static void mdosettime(struct myclkreg *clkregs,UWORD time)
{
    clkregs->sec_l = ((time & 0x1f) << 1) % 10;
    clkregs->sec_h = ((time & 0x1f) << 1) / 10;
    clkregs->min_l = ((time >> 5) & 0x3f) % 10;
    clkregs->min_h = ((time >> 5) & 0x3f) / 10;
    clkregs->hour_l = ((time >> 11) & 0x1f) % 10;
    clkregs->hour_h = ((time >> 11) & 0x1f) / 10;

    KDEBUG(("mdosettime() %x%x:%x%x:%x%x\n", clkregs->hour_h, clkregs->hour_l,
            clkregs->min_h, clkregs->min_l, clkregs->sec_h, clkregs->sec_l));
}

static UWORD mdogettime(struct myclkreg *clkregs)
{
    UWORD time;

    KDEBUG(("mdogettime() %x%x:%x%x:%x%x\n", clkregs->hour_h, clkregs->hour_l,
            clkregs->min_h, clkregs->min_l, clkregs->sec_h, clkregs->sec_l));

    time = (((clkregs->sec_l & 0xf) + 10 * (clkregs->sec_h & 0xf)) >> 1)
            | (((clkregs->min_l & 0xf) + 10 * (clkregs->min_h & 0xf)) << 5)
            | (((clkregs->hour_l & 0xf) + 10 * (clkregs->hour_h & 0xf)) << 11);

    return time;
}

static void mdosetdate(struct myclkreg *clkregs,UWORD date)
{
    clkregs->day_l = (date & 0x1F) % 10;
    clkregs->day_h = (date & 0x1F) / 10;
    clkregs->mon_l = ((date >> 5) & 0xF) % 10;
    clkregs->mon_h = ((date >> 5) & 0xF) / 10;
    clkregs->year_l = (date >> 9) % 10;
    clkregs->year_h = (date >> 9) / 10;

    KDEBUG(("mdosetdate() %x%x/%x%x/%x%x\n", clkregs->year_h, clkregs->year_l,
            clkregs->mon_h, clkregs->mon_l, clkregs->day_h, clkregs->day_l));
}

static UWORD mdogetdate(struct myclkreg *clkregs)
{
    UWORD date;

    KDEBUG(("mdogetdate() %x%x/%x%x/%x%x\n", clkregs->year_h, clkregs->year_l,
            clkregs->mon_h, clkregs->mon_l, clkregs->day_h, clkregs->day_l));

    /* The MegaRTC stores the year as the offset from 1980.
     * Fortunately, this is exactly the same as in the BIOS format,
     * and it's helpfully aligned on a leap year.
     */
    date = ((clkregs->day_l & 0xf) + 10 * (clkregs->day_h & 0xf))
            | (((clkregs->mon_l & 0xf) + 10 * (clkregs->mon_h & 0xf)) << 5)
            | (((clkregs->year_l & 0xf) + 10 * (clkregs->year_h & 0xf)) << 9);

    return date;
}

/*==== MegaRTC high-level functions ========================================*/

static ULONG mgetdt(void)
{
    struct myclkreg clkregs;

    mgetregs(&clkregs);

    return (((ULONG) mdogetdate(&clkregs)) << 16) | mdogettime(&clkregs);
}

static void msetdt(ULONG dt)
{
    struct myclkreg clkregs;

    mdosetdate(&clkregs,dt>>16);
    mdosettime(&clkregs,dt);
    msetregs(&clkregs);
}

#endif /* CONF_WITH_MEGARTC */

#if CONF_WITH_NVRAM

/*==== NVRAM RTC internal functions =======================================*/

/*
 * The MC146818 was used as the RTC and NVRAM in TT and Falcon.
 * You can find a header file in /usr/src/linux/include/linux/mc146818rtc.h
 * Proper implementation of RTC functions is in linux/arch/m68k/atari/time.c.
 * The code below is just my quick hack. It works but it could not be used
 * for updating real RTC because it doesn't handle the control registers
 * and also doesn't provide proper timing (32kHz device needs proper timing).
 * Reading of RTC should be OK on real machines.
 * (PES)
 */
#define NVRAM_RTC_SECONDS 0
#define NVRAM_RTC_MINUTES 2
#define NVRAM_RTC_HOURS   4
#define NVRAM_RTC_DAYS    7
#define NVRAM_RTC_MONTHS  8
#define NVRAM_RTC_YEARS   9

/* Offset to be added to the NVRAM RTC year to get the actual year.
 * Beware, this value depends on the ROM OS version.
 * See clock_init() for details. */
static int nvram_rtc_year_offset;

static void ndosettime(UWORD time)
{
    int seconds = (time & 0x1f) << 1;
    int minutes = (time >> 5) & 0x3f;
    int hours = (time >> 11) & 0x1f;

    KDEBUG(("ndosettime() %02d:%02d:%02d\n", hours, minutes, seconds));

    set_nvram_rtc(NVRAM_RTC_SECONDS, seconds);
    set_nvram_rtc(NVRAM_RTC_MINUTES, minutes);
    set_nvram_rtc(NVRAM_RTC_HOURS, hours);
}

static UWORD ndogettime(void)
{
    UWORD seconds = get_nvram_rtc(NVRAM_RTC_SECONDS);
    UWORD minutes = get_nvram_rtc(NVRAM_RTC_MINUTES);
    UWORD hours = get_nvram_rtc(NVRAM_RTC_HOURS);
    UWORD time;

    KDEBUG(("ndogettime() %02d:%02d:%02d\n", hours, minutes, seconds));

    time = (seconds >> 1) | (minutes << 5) | (hours << 11);

    return time;
}

static void ndosetdate(UWORD date)
{
    int days = date & 0x1f;
    int months = (date >> 5) & 0xf;
    int years = (date >> 9) - nvram_rtc_year_offset;

    KDEBUG(("ndosetdate() %02d/%02d/%02d\n", years, months, days));

    set_nvram_rtc(NVRAM_RTC_DAYS, days);
    set_nvram_rtc(NVRAM_RTC_MONTHS, months);
    set_nvram_rtc(NVRAM_RTC_YEARS, years);
}

static UWORD ndogetdate(void)
{
    UWORD days = get_nvram_rtc(NVRAM_RTC_DAYS);
    UWORD months = get_nvram_rtc(NVRAM_RTC_MONTHS);
    UWORD years = get_nvram_rtc(NVRAM_RTC_YEARS);
    UWORD date;

    KDEBUG(("ndogetdate() %02d/%02d/%02d\n", years, months, days));

    date = (days & 0x1f) | ((months & 0xf) << 5) | ((years + nvram_rtc_year_offset) << 9);

    return date;
}

/*==== NVRAM RTC high-level functions ======================================*/

static ULONG ngetdt(void)
{
    return (((ULONG) ndogetdate()) << 16) | ndogettime();
}

static void nsetdt(ULONG dt)
{
    ndosetdate(dt >> 16);
    ndosettime(dt);
}

#endif /* CONF_WITH_NVRAM */

#if CONF_WITH_IKBD_CLOCK

/*==== IKBD clock section =================================================*/

static struct ikbdregs
{
    UBYTE cmd;
    UBYTE year;
    UBYTE month;
    UBYTE day;
    UBYTE hour;
    UBYTE min;
    UBYTE sec;
} iclkbuf;

static volatile WORD iclk_ready;

#define IKBD_CLOCK_TIMEOUT  (2*CLOCKS_PER_SEC)  /* 2 seconds */

/* called by the ACIA interrupt */
/* EmuTOS's ikbdsys also puts the buffer on the stack */
void clockvec(BYTE *buf)
{
    BYTE *b = 1 + ((BYTE *)&iclkbuf);

    memmove(b, buf, 6);
    iclk_ready = 1;
}

static UBYTE int2bcd(UWORD a)
{
    return (a % 10) + ((a / 10) << 4);
}

static UWORD bcd2int(UBYTE a)
{
    return (a & 0xf) + ((a >> 4) * 10);
}

/*==== Ikbd clock internal functions ======================================*/


static void igetregs(void)
{
    LONG timeout;

    iclk_ready = 0;
    iclkbuf.cmd = 0x1C;
    ikbdws(0, (const UBYTE*) &iclkbuf);

    /* wait until the interrupt */
    timeout = hz_200 + IKBD_CLOCK_TIMEOUT;
    while(!iclk_ready && (timeout > hz_200))
        ;
}

static void isetregs(void)
{
    iclkbuf.cmd = 0x1B;
    ikbdws(6, (const UBYTE*) &iclkbuf);
}

static UWORD idogetdate(void)
{
    UWORD year;
    UWORD date;

    KDEBUG(("idogetdate() %02x/%02x/%02x\n", iclkbuf.year, iclkbuf.month, iclkbuf.day));

    /* guess the real year from IKBD data */
    year = bcd2int(iclkbuf.year);
    if (year < 80)
        year += 2000;
    else
        year += 1900;

    date = ((year-1980) << 9) | (bcd2int(iclkbuf.month) << 5) | bcd2int(iclkbuf.day);

    return date;
}

static UWORD idogettime(void)
{
    UWORD time;

    KDEBUG(("idogettime() %02x:%02x:%02x\n", iclkbuf.hour, iclkbuf.min, iclkbuf.sec));

    time = (bcd2int(iclkbuf.sec)>>1) | (bcd2int(iclkbuf.min)<<5) | (bcd2int(iclkbuf.hour)<<11);

    return time;
}

static void idosettime(UWORD time)
{
    iclkbuf.sec = int2bcd((time << 1) & 0x3f);
    iclkbuf.min = int2bcd((time >> 5) & 0x3f);
    iclkbuf.hour = int2bcd((time >> 11) & 0x1f);

    KDEBUG(("idosettime() %02x:%02x:%02x\n", iclkbuf.hour, iclkbuf.min, iclkbuf.sec));
}

static void idosetdate(UWORD date)
{
    UWORD year = 1980 + ((date >> 9) & 0x7f);

    iclkbuf.year = int2bcd(year % 100);
    iclkbuf.month = int2bcd((date >> 5) & 0xf);
    iclkbuf.day = int2bcd(date & 0x1f);

    KDEBUG(("idosetdate() %02x/%02x/%02x\n", iclkbuf.year, iclkbuf.month, iclkbuf.day));
}


/*==== Ikbd Clock high-level functions ====================================*/

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

#endif /* CONF_WITH_IKBD_CLOCK */

/* internal init */

void clock_init(void)
{
    if (FALSE)
    {
        /* Dummy case for conditional compilation */
    }
#ifdef MACHINE_AMIGA
    else if (TRUE)
    {
        return amiga_clock_init();
    }
#endif /* MACHINE_AMIGA */
#if CONF_WITH_NVRAM
    else if (has_nvram)
    {
        /* The base year of the NVRAM RTC varies among TOS versions.
         * On TT TOS <= 3.05, the base year is 1970.
         * On later TT TOS and Falcon TOS, the base year is 1968.
         * This was changed by Atari because the base year must be a
         * leap year for the RTC to handle leap years correctly.
         * By default, we use the new offset in EmuTOS.
         */
        BOOL new_year_offset = TRUE;

#if EMUTOS_LIVES_IN_RAM
        /* We run from RAM, so we must respect the base year of the ROM OS. */
        if (cookie_mch == MCH_TT)
        {
            BOOL rom_is_emutos = *(ULONG*)0xe0002c == 0x45544f53; /* ETOS */

            if (!rom_is_emutos)
            {
                UWORD rom_os_version = *(UWORD*)0xe00002;

                if (rom_os_version <= 0x0305)
                {
                    /* Old TOS in ROM: use old NVRAM offset */
                    new_year_offset = FALSE;
                }
            }
        }
#endif /* EMUTOS_LIVES_IN_RAM */

        nvram_rtc_year_offset = (new_year_offset ? 1968 : 1970) - 1980;
    }
#endif /* CONF_WITH_NVRAM */
    else if (HAS_MEGARTC)
    {
        /* Nothing to initialize */
    }
    else if (HAS_ICDRTC)
    {
        /* Nothing to initialize */
    }
#if CONF_WITH_IKBD_CLOCK
    else
    {
        /* The IKBD clock is lost at power off, and has bogus values at
         * power on.  So initialize it to the default date/time at startup.
         */
        if (first_boot)
            isetdt(DEFAULT_DATETIME);
    }
#endif /* CONF_WITH_IKBD_CLOCK */
}

/* xbios functions */

void settime(LONG time)
{
    if (FALSE)
    {
        /* Dummy case for conditional compilation */
    }
#if CONF_WITH_NVRAM
    else if (has_nvram)
    {
        nsetdt(time);
    }
#endif /* CONF_WITH_NVRAM */
#if CONF_WITH_MEGARTC
    else if (has_megartc)
    {
        msetdt(time);
    }
#endif /* CONF_WITH_MEGARTC */
#if CONF_WITH_ICDRTC
    else if (has_icdrtc)
    {
        icdsetdt(time);
    }
#endif  /* CONF_WITH_ICDRTC */
    else
    {
#if CONF_WITH_IKBD_CLOCK
        isetdt(time);
#endif /* CONF_WITH_IKBD_CLOCK */
    }
}

LONG gettime(void)
{
    if (FALSE)
    {
        /* Dummy case for conditional compilation */
    }
#ifdef MACHINE_AMIGA
    else if (TRUE)
    {
        return amiga_getdt();
    }
#endif /* MACHINE_AMIGA */
#if CONF_WITH_NVRAM
    else if (has_nvram)
    {
        return ngetdt();
    }
#endif /* ! NO_NVRAM */
#if CONF_WITH_MEGARTC
    else if (has_megartc)
    {
        return mgetdt();
    }
#endif /* CONF_WITH_MEGARTC */
#if CONF_WITH_ICDRTC
    else if (has_icdrtc)
    {
        return icdgetdt();
    }
#endif  /* CONF_WITH_ICDRTC */
    else
    {
#if CONF_WITH_IKBD_CLOCK
        return igetdt();
#else
        return DEFAULT_DATETIME;
#endif /* CONF_WITH_IKBD_CLOCK */
    }
}
