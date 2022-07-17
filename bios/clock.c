/*
 * clock.c - BIOS time and date routines
 *
 * Copyright (C) 2001-2022 The EmuTOS development team
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

#include "emutos.h"
#include "clock.h"
#include "ikbd.h"
#include "mfp.h"
#include "tosvars.h"
#include "string.h"
#include "vectors.h"
#include "nvram.h"
#include "machine.h"
#include "has.h"
#include "cookie.h"
#include "asm.h"
#include "dma.h"
#include "delay.h"
#include "bios.h"
#include "../bdos/bdosstub.h"
#include "amiga.h"
#include "lisa.h"
#include "disk.h"
#include "acsi.h"

#if (CONF_WITH_MONSTER || CONF_WITH_IKBD_CLOCK)
static UBYTE int2bcd(UWORD a)
{
    return (a % 10) + ((a / 10) << 4);
}

static UWORD bcd2int(UBYTE a)
{
    return (a & 0xf) + ((a >> 4) * 10);
}
#endif

#if (CONF_WITH_ICDRTC || CONF_WITH_MONSTER || CONF_WITH_MEGARTC || CONF_WITH_NVRAM || CONF_WITH_IKBD_CLOCK || CONF_WITH_ULTRASATAN_CLOCK)
/*
 * structures used by extract_date(), extract_time()
 */
struct ymd
{
    UWORD year;
    UWORD month;
    UWORD day;
};

struct hms
{
    UWORD hour;
    UWORD minute;
    UWORD second;
};

/*
 * extract year/month/day from GEMDOS-style date
 *
 * note: extracted year is raw, i.e. relative to 1980
 */
static void extract_date(struct ymd *out, UWORD date)
{
    out->year = date >> 9;
    out->month = (date >> 5) & 0x0f;
    out->day = date & 0x1f;
}

/*
 * extract hour/minute/second from GEMDOS-style time
 */
static void extract_time(struct hms *out, UWORD time)
{
    out->hour = time >> 11;
    out->minute = (time >> 5) & 0x3f;
    out->second = (time << 1) & 0x3f;
}
#endif

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

    loopcount_1_usec = loopcount_1_msec / 1000;

    if (icd_clock_begin())
        has_icdrtc = 1;

    icd_clock_end();
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
    struct hms tm;

    extract_time(&tm, time);

    /*
     * we must set the '24-hour clock' indicator in the tens-of-hours register
     */
    clkregs->sec_l = tm.second % 10;
    clkregs->sec_h = tm.second / 10;
    clkregs->min_l = tm.minute % 10;
    clkregs->min_h = tm.minute / 10;
    clkregs->hour_l = tm.hour % 10;
    clkregs->hour_h = (tm.hour / 10) | ICDRTC_24;

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
    struct ymd dt;

    /*
     * The ICD RTC stores the year as the offset from 1900; thus we need
     * to add 80 to the DOS-format year to get the RTC year.
     *
     * Note: despite the chip documentation, the leap year mechanism does
     * not seem to utilise bits 2-3 of the tens-of-days register.  Instead,
     * it appears to use the year value, i.e. if the year is divisible by
     * 4, then it is assumed to be a leap year.
     */
    extract_date(&dt, date);

    dt.year += 80;

    clkregs->day_l = dt.day % 10;
    clkregs->day_h = dt.day / 10;
    clkregs->mon_l = dt.month % 10;
    clkregs->mon_h = dt.month / 10;
    clkregs->year_l = dt.year % 10;
    clkregs->year_h = dt.year / 10;

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

    return MAKE_ULONG(icdgetdate(&clkregs), icdgettime(&clkregs));
}

static void icdsetdt(ULONG dt)
{
    struct icdclkreg clkregs;

    icdsetdate(&clkregs,HIWORD(dt));
    icdsettime(&clkregs,LOWORD(dt));
    icdsetregs(&clkregs);
}

#endif /* CONF_WITH_ICDRTC */

#if CONF_WITH_MONSTER

/*==== MonSTer RTC section ================================================*/

/*
 * Supports Dallas DS1307 and compatible (13072, 1337, 1338...) RTC
 * chips connected to the MonSTer I2C bus.
 * I2C bitbanging routines based on code by Alan Hourihane.
 */

#define MONSTER_I2C_DIR *(volatile unsigned short *)0xfffffe02L
#define MONSTER_I2C_SCL *(volatile unsigned short *)0xfffffe04L
#define MONSTER_I2C_SDA *(volatile unsigned short *)0xfffffe06L

#define I2C_HIGH 1
#define I2C_LOW 0

static ULONG delay5us;
#define DELAY5US delay_loop(delay5us)

static void i2c_start (void)
{
    MONSTER_I2C_SDA = I2C_HIGH;
    DELAY5US;
    MONSTER_I2C_SCL = I2C_HIGH;
    DELAY5US;
    MONSTER_I2C_SDA = I2C_LOW;
    DELAY5US;
    MONSTER_I2C_SCL = I2C_LOW;
}

static void i2c_stop (void)
{
    MONSTER_I2C_SCL = I2C_LOW;
    MONSTER_I2C_SDA = I2C_LOW;
    MONSTER_I2C_SCL = I2C_HIGH;
    DELAY5US;
    MONSTER_I2C_SDA = I2C_HIGH;

    MONSTER_I2C_DIR = 3; /* Set pins to float */
}

static void i2c_write (UBYTE data)
{
    UBYTE i;

    for(i = 0; i < 8; i++)
    {
        MONSTER_I2C_SDA = (data & 0x80) ? 1 : 0;
        data <<= 1;
        MONSTER_I2C_SCL = I2C_HIGH;
        DELAY5US;
        MONSTER_I2C_SCL = I2C_LOW;
    }

    MONSTER_I2C_DIR = 2; /* SDA as input */
    MONSTER_I2C_SCL = I2C_HIGH;
    DELAY5US;
    MONSTER_I2C_SCL = I2C_LOW;
}

static UBYTE i2c_read (void)
{
    UBYTE i, data = 0;

    MONSTER_I2C_DIR = 2; /* SDA as input */

    for(i = 0; i < 8; i++)
    {
        data <<= 1;
        MONSTER_I2C_SCL = I2C_HIGH;
        data |= MONSTER_I2C_SDA;
        DELAY5US;
        MONSTER_I2C_SCL = I2C_LOW;
    }

    return data;
}

static void write_ds1307(UBYTE address, UBYTE data)
{
    i2c_start();
    i2c_write(0xd0);
    i2c_write(address);
    i2c_write(data);
    i2c_stop();
}

static UBYTE read_ds1307(UBYTE address)
{
    UBYTE data;

    i2c_start();
    i2c_write(0xd0);
    i2c_write(address);
    i2c_start();
    i2c_write(0xd1);
    data = i2c_read();
    i2c_stop();

    return data;
}

/*==== MonSTer RTC high-level functions ====================================*/

static ULONG monstergetdt(void)
{
    ULONG t = 0;
    t  = (ULONG)(bcd2int((read_ds1307(0) & 0x7f)/2));   /* Seconds */
    t |= (ULONG)(bcd2int(read_ds1307(1))) << 5;         /* Minute */
    t |= (ULONG)(bcd2int(read_ds1307(2) & 0x3f)) << 11; /* Hour */

    t |= (ULONG)(bcd2int(read_ds1307(4))) << 16;        /* Day of month */
    t |= (ULONG)(bcd2int(read_ds1307(5))) << 21;        /* Month */
    t |= (ULONG)(bcd2int(read_ds1307(6)) + 20) << 25;   /* Year */

    KDEBUG(("monstergetdt = 0x%lx\n", t));

    return t;
}

static void monstersetdt(ULONG time)
{
    struct hms tm;
    struct ymd dt;

    extract_time(&tm, LOWORD(time));
    extract_date(&dt, HIWORD(time));
    dt.year = (dt.year + 1980) % 100;

    write_ds1307(0, int2bcd(tm.second));    /* Seconds */
    write_ds1307(1, int2bcd(tm.minute));    /* Minute */
    write_ds1307(2, int2bcd(tm.hour));      /* Hour */

    write_ds1307(4, int2bcd(dt.day));       /* Day of month */
    write_ds1307(5, int2bcd(dt.month));     /* Month */
    write_ds1307(6, int2bcd(dt.year));      /* Year */

    KDEBUG(("monstersetdt(0x%lx)\n", time));
}

int has_monster_rtc;

void detect_monster_rtc(void)
{
    /*
     * Check if there's a DS1307-compatible RTC connected.
     * If there isn't, any attempts to read from it will
     * return either all zeros or all ones depending on the
     * exact HW setup. So we try to read the DAY register.
     * If it's 0 or 0xff, there's either no RTC or it's not
     * initialized. So we try to write to the DAY register
     * and read back its value. If it is still 0 or 0xff,
     * then no RTC is present.
     */

    UBYTE dayreg;

    /* Initialize I2C delay. */
    delay5us = loopcount_1_msec / 200;

    /* Detect presence of RTC. */
    has_monster_rtc = TRUE;

    dayreg = read_ds1307(4);
    if ((dayreg == 0) || (dayreg == 0xff))
    {
        write_ds1307(4, 1);

        dayreg = read_ds1307(4);
        if ((dayreg == 0) || (dayreg == 0xff))
            has_monster_rtc = FALSE;
        else
            /* RTC present, but not initialized. */
            monstersetdt(DEFAULT_DATETIME);
    }
}

#endif /* CONF_WITH_MONSTER */

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
    UBYTE *buf1, *buf2;
    volatile UBYTE *regs;
    struct myclkreg clkcopy;

    clk.bank0.mode &= ~SELECT_BANK1;    /* ensure bank 0 */

    do
    {
        regs = &clk.bank0.sec_l;
        buf1 = &clkregs->sec_l;
        for (i = 0; i < NUM_MEGARTC_REGS; i++)
        {
            *buf1++ = *regs;
            regs += 2;
        }

        regs = &clk.bank0.sec_l;
        buf1 = &clkregs->sec_l;
        buf2 = &clkcopy.sec_l;
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
    UBYTE *buf;
    volatile UBYTE *regs;

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
    regs = &clk.bank0.sec_l;
    buf = &clkregs->sec_l;
    for (i = 0; i < NUM_MEGARTC_REGS; i++)
    {
        *regs = *buf++;
        regs += 2;
    }
    clk.bank0.mode |= ENABLE_TIMER;     /* enable timer */
}

static void mdosettime(struct myclkreg *clkregs,UWORD time)
{
    struct hms tm;

    extract_time(&tm, time);

    clkregs->sec_l = tm.second % 10;
    clkregs->sec_h = tm.second / 10;
    clkregs->min_l = tm.minute % 10;
    clkregs->min_h = tm.minute / 10;
    clkregs->hour_l = tm.hour % 10;
    clkregs->hour_h = tm.hour / 10;

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
    struct ymd dt;

    extract_date(&dt, date);

    clkregs->day_l = dt.day % 10;
    clkregs->day_h = dt.day / 10;
    clkregs->mon_l = dt.month % 10;
    clkregs->mon_h = dt.month / 10;
    clkregs->year_l = dt.year % 10;
    clkregs->year_h = dt.year / 10;

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

    return MAKE_ULONG(mdogetdate(&clkregs), mdogettime(&clkregs));
}

static void msetdt(ULONG dt)
{
    struct myclkreg clkregs;

    mdosetdate(&clkregs,HIWORD(dt));
    mdosettime(&clkregs,LOWORD(dt));
    msetregs(&clkregs);
}

#undef clk

#endif /* CONF_WITH_MEGARTC */

#if CONF_WITH_NVRAM

/*==== NVRAM RTC internal functions =======================================*/

/*
 * The MC146818 was used as the RTC and NVRAM in TT and Falcon.
 * You can find a header file in /usr/src/linux/include/linux/mc146818rtc.h
 * Proper implementation of RTC functions is in linux/arch/m68k/atari/time.c.
 *
 * The following code is based on a review of the above sources together
 * with disassemblies of TT and Falcon TOS.
 */
#define NVRAM_RTC_SECONDS 0
#define NVRAM_RTC_MINUTES 2
#define NVRAM_RTC_HOURS   4
#define NVRAM_RTC_DAYS    7
#define NVRAM_RTC_MONTHS  8
#define NVRAM_RTC_YEARS   9
#define NVRAM_RTC_REG_A   10
#define NVRAM_RTC_REG_B   11
#define NVRAM_RTC_REG_C   12
#define NVRAM_RTC_REG_D   13

/*
 * Internal structure for holding values from RTC.
 */
struct clkreg {
    UBYTE years, months, days;
    UBYTE hours, minutes, seconds;
};

/* Offset to be added to the NVRAM RTC year to get the actual year.
 * Beware, this value depends on the ROM OS version.
 * See clock_init() for details. */
static int nvram_rtc_year_offset;

static WORD nvram_getregs(struct clkreg *clk)
{
    WORD old_sr;

    if ((get_nvram_rtc(NVRAM_RTC_REG_D) & 0x80) == 0)   /* VRT==0 => invalid date/time */
        return -1;

    old_sr = set_sr(0x2700);                        /* prevent interrupts */

    while(get_nvram_rtc(NVRAM_RTC_REG_A) & 0x80)    /* wait for UIP == 0 */
        ;

    clk->seconds = get_nvram_rtc(NVRAM_RTC_SECONDS);
    clk->minutes = get_nvram_rtc(NVRAM_RTC_MINUTES);
    clk->hours = get_nvram_rtc(NVRAM_RTC_HOURS);
    clk->days = get_nvram_rtc(NVRAM_RTC_DAYS);
    clk->months = get_nvram_rtc(NVRAM_RTC_MONTHS);
    clk->years = get_nvram_rtc(NVRAM_RTC_YEARS);

    set_sr(old_sr);

    KDEBUG(("nvram_getregs(): %02d/%02d/%02d  %02d:%02d:%02d\n",
            clk->years, clk->months, clk->days, clk->hours, clk->minutes, clk->seconds));

    return 0;
}

static void nvram_setregs(struct clkreg *clk)
{
    KDEBUG(("nvram_setregs(): %02d/%02d/%02d  %02d:%02d:%02d\n",
            clk->years, clk->months, clk->days, clk->hours, clk->minutes, clk->seconds));

    set_nvram_rtc(NVRAM_RTC_REG_B, 0x80);   /* prevent updates, abort any in progress */
    set_nvram_rtc(NVRAM_RTC_REG_A, 0x2A);   /* select 32768Hz frequency */
    set_nvram_rtc(NVRAM_RTC_REG_B, 0x86);   /* select binary encoding, 24-hour clock */
    set_nvram_rtc(NVRAM_RTC_YEARS, clk->years);
    set_nvram_rtc(NVRAM_RTC_MONTHS, clk->months);
    set_nvram_rtc(NVRAM_RTC_DAYS, clk->days);
    set_nvram_rtc(NVRAM_RTC_HOURS, clk->hours);
    set_nvram_rtc(NVRAM_RTC_MINUTES, clk->minutes);
    set_nvram_rtc(NVRAM_RTC_SECONDS, clk->seconds);
    set_nvram_rtc(NVRAM_RTC_REG_B, 0x06);   /* allow updates */
}

/*==== NVRAM RTC high-level functions ======================================*/

static ULONG ngetdt(void)
{
    struct clkreg clk;
    UWORD date, time;

    if (nvram_getregs(&clk) < 0)
        return 0UL;

    date = (((clk.years+nvram_rtc_year_offset) & 0x7f) << 9)
            | ((clk.months & 0xf) << 5) | (clk.days & 0x1f);
    time = (clk.hours << 11) | (clk.minutes << 5) | (clk.seconds >> 1);

    return MAKE_ULONG(date, time);
}

static void nsetdt(ULONG dt)
{
    struct clkreg clk;
    struct ymd date;
    struct hms time;

    extract_date(&date, HIWORD(dt));
    clk.years = date.year - nvram_rtc_year_offset;
    clk.months = date.month;
    clk.days = date.day;

    extract_time(&time, LOWORD(dt));
    clk.hours = time.hour;
    clk.minutes = time.minute;
    clk.seconds = time.second;

    nvram_setregs(&clk);
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
void clockvec(char *buf)
{
    char *b = 1 + ((char *)&iclkbuf);

    memmove(b, buf, 6);
    iclk_ready = 1;
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
    struct hms tm;

    extract_time(&tm, time);

    iclkbuf.sec = int2bcd(tm.second);
    iclkbuf.min = int2bcd(tm.minute);
    iclkbuf.hour = int2bcd(tm.hour);

    KDEBUG(("idosettime() %02x:%02x:%02x\n", iclkbuf.hour, iclkbuf.min, iclkbuf.sec));
}

static void idosetdate(UWORD date)
{
    struct ymd dt;

    extract_date(&dt, date);
    dt.year = (dt.year + 1980) % 100;

    iclkbuf.year = int2bcd(dt.year);
    iclkbuf.month = int2bcd(dt.month);
    iclkbuf.day = int2bcd(dt.day);

    KDEBUG(("idosetdate() %02x/%02x/%02x\n", iclkbuf.year, iclkbuf.month, iclkbuf.day));
}


/*==== Ikbd Clock high-level functions ====================================*/

static ULONG igetdt(void)
{
    igetregs();

    return MAKE_ULONG(idogetdate(), idogettime());
}

static void isetdt(ULONG dt)
{
    idosetdate(HIWORD(dt));
    idosettime(LOWORD(dt));
    isetregs();
}

#endif /* CONF_WITH_IKBD_CLOCK */

#if CONF_WITH_ULTRASATAN_CLOCK /* CONF_WITH_ULTRASATAN_CLOCK */

static ULONG ultrasatan_getdt(void)
{
    UBYTE hour, minute, second, day, month;
    UWORD year, date, time;

    int ret;
    ret = acsi_ioctl(ultrasatan_id,ULTRASATAN_GET_CLOCK,NULL);

    /* check return status and format */
    if (ret != 0 || memcmp(dskbufp,"RTC",3) != 0)
        return 0;

    year = (UWORD)dskbufp[3];
    month = dskbufp[4];
    day = dskbufp[5];
    hour = dskbufp[6];
    minute = dskbufp[7];
    second = dskbufp[8];

    KDEBUG(("ultrasatan_getdt(): read clock value %02d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second));

    date = (year + 20) << 9 | (month & 0xf) << 5 | (day & 0x1f);
    time = (hour << 11) | (minute << 5) | (second >> 1);

    return MAKE_ULONG(date, time);
}

static ULONG ultrasatan_setdt(ULONG dt)
{
    struct ymd date;
    struct hms time;
    int ret;

    extract_date(&date, HIWORD(dt));
    extract_time(&time, LOWORD(dt));

    KDEBUG(("ultrasatan_setdt(): new date/time %02d-%02d-%02d %02d:%02d:%02d\n", date.year - 20, date.month, date.day, time.hour, time.minute, time.second));

    dskbufp[0] = 'R';
    dskbufp[1] = 'T';
    dskbufp[2] = 'C';

    dskbufp[3] = (UBYTE)(date.year - 20);
    dskbufp[4] = date.month;
    dskbufp[5] = date.day;
    dskbufp[6] = time.hour;
    dskbufp[7] = time.minute;
    dskbufp[8] = time.second;

    KDEBUG(("ultrasatan_setdt(): setting clock\n"));

    ret = acsi_ioctl(ultrasatan_id,ULTRASATAN_SET_CLOCK,NULL);

    return ret;
}

#endif /* CONF_WITH_ULTRASATAN_CLOCK */

/* internal init */

void clock_init(void)
{
    /*
     * the following bypasses a bug in Hatari 2.0, which causes the ICD RTC
     * to be detected incorrectly when detect_icdrtc() is called early on
     * in initialisation
     */
#if CONF_WITH_ICDRTC
    if (has_icdrtc)
        detect_icdrtc();
#endif

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
        /*
         * The IKBD clock powers up with zeros in the month & day
         * fields.  If we find that, we initialize it to the default
         * date/time, just like Atari TOS does.
         */
        if ((igetdt() & 0x01ff0000L) == 0L)
            isetdt(DEFAULT_DATETIME);
    }
#endif /* CONF_WITH_IKBD_CLOCK */
}

/* xbios functions */

void settime(LONG time)
{
    /* Update GEMDOS time and date */
    current_time = LOWORD(time);
    current_date = HIWORD(time);

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
#if CONF_WITH_MONSTER
    else if (has_monster_rtc)
    {
        monstersetdt(time);
    }
#endif /* CONF_WITH_MONSTER */
#if CONF_WITH_ICDRTC
    else if (has_icdrtc)
    {
        icdsetdt(time);
    }
#endif  /* CONF_WITH_ICDRTC */
#if CONF_WITH_ULTRASATAN_CLOCK
    else if (has_ultrasatan_clock)
    {
        ultrasatan_setdt(time);
    }
#endif /* CONF_WITH_ULTRASATAN_CLOCK */
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
#ifdef MACHINE_LISA
    else if (TRUE)
    {
        return lisa_getdt();
    }
#endif /* MACHINE_LISA */
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
#if CONF_WITH_MONSTER
    else if (has_monster_rtc)
    {
        return monstergetdt();
    }
#endif /* CONF_WITH_MONSTER */
#if CONF_WITH_ICDRTC
    else if (has_icdrtc)
    {
        return icdgetdt();
    }
#endif  /* CONF_WITH_ICDRTC */
#if CONF_WITH_ULTRASATAN_CLOCK
    else if (has_ultrasatan_clock)
    {
        return ultrasatan_getdt();
    }
#endif /* CONF_WITH_ULTRASATAN_CLOCK */
    else
    {
#if CONF_WITH_IKBD_CLOCK
        return igetdt();
#else
        return DEFAULT_DATETIME;
#endif /* CONF_WITH_IKBD_CLOCK */
    }
}
