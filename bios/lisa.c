/*
 * lisa.c - Apple Lisa specific functions
 *
 * Copyright (C) 2021 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "lisa.h"
#include "bios.h"
#include "vectors.h"
#include "asm.h"
#include "ikbd.h"
#include "delay.h"
#include "tosvars.h"
#include "screen.h"
#include "../vdi/vdi_defs.h"
#include "disk.h"
#include "gemerror.h"

#ifdef MACHINE_LISA

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/* Processor board control registers.
 * Warning!
 * Any *read* or write to the registers below will trigger the function. */
#define VTIRDIS (*(volatile UWORD*)0x00fce018) /* Disable Vertical Retrace Interrupt (VBL) */
#define VTIRENB (*(volatile UWORD*)0x00fce01a) /* Enable Vertical Retrace Interrupt (VBL) */
/* But the following ones can be safely read */
#define VIDLTCH (*(volatile UBYTE*)0x00fce800) /* Video Address Latch */
#define STATREG (*(volatile UBYTE*)0x00fcf801) /* Error status register */

/* The system features 2 VIA chips for I/O and timers:
 * Synertek SY6522 Versatile Interface Adapter (VIA) */

/* VIA1 port A is connected to COPS (Keyboard/Mouse/Clock/Shutdown) */
#define VIA1BASE 0x00fcdd81
#define ORB1   (*(volatile UBYTE*)(VIA1BASE + 0x00)) /* Port B output register */
#define ORA1   (*(volatile UBYTE*)(VIA1BASE + 0x02)) /* Port A output register */
#define DDRA1  (*(volatile UBYTE*)(VIA1BASE + 0x06)) /* Port A data direction register */
#define IFR1   (*(volatile UBYTE*)(VIA1BASE + 0x1a)) /* Interrupt Flag register */
#define IER1   (*(volatile UBYTE*)(VIA1BASE + 0x1c)) /* Interrupt Enable register */
#define PORTA1 (*(volatile UBYTE*)(VIA1BASE + 0x1e)) /* Port A with no handshake */

/* VIA2 port A is the parallel port (Profile hard disk) */
#define VIA2BASE 0x00fcd901
#define ORA2   (*(volatile UBYTE*)(VIA2BASE + 0x08)) /* Port A output register */
#define T1CL2  (*(volatile UBYTE*)(VIA2BASE + 0x20)) /* Timer 1 counter low byte */
#define T1CH2  (*(volatile UBYTE*)(VIA2BASE + 0x28)) /* Timer 1 counter high byte */
#define ACR2   (*(volatile UBYTE*)(VIA2BASE + 0x58)) /* Auxiliary control register */
#define PCR2   (*(volatile UBYTE*)(VIA2BASE + 0x60)) /* Peripheral control register */
#define IFR2   (*(volatile UBYTE*)(VIA2BASE + 0x68)) /* Interrupt Flag register */
#define IER2   (*(volatile UBYTE*)(VIA2BASE + 0x70)) /* Interrupt Enable register */

/* bit masks unique to SY6522 Interrupt Flag Register */
#define IFR_IRQ     0x80    /* an interrupt has occurred on an active channel */

/* bit masks unique to SY6522 Interrupt Enable Register */
#define IER_ENABLE  0x80    /* enable interrupts corresponding to bit(s) set below */
#define IER_DISABLE 0x00    /* disable interrupts corresponding to bit(s) set below */

/* bit masks for SY6522 Interrupt Flag Register AND Interrupt Enable Register */
#define INT_TIMER1  0x40    /* Timer 1 */
#define INT_TIMER2  0x20    /* Timer 2 */
#define INT_CB1     0x10    /* Peripheral B control line 1 */
#define INT_CB2     0x08    /* Peripheral B control line 2 */
#define INT_SHIFT   0x04    /* Shift register */
#define INT_CA1     0x02    /* Peripheral A control line 1 */
#define INT_CA2     0x01    /* Peripheral A control line 2 */

/* Disk controller shared memory */
#define DISKMEM 0x00fcc001  /* Base address */
#define GOBYTE  (*(volatile UBYTE*)(DISKMEM + 0x00)) /* Command */
#define CMD     (*(volatile UBYTE*)(DISKMEM + 0x02)) /* Function */
#define DRV     (*(volatile UBYTE*)(DISKMEM + 0x04)) /* Drive */

/* Floppy drive IDs */
#define DRV1    0x00 /* Drive 1 of Lisa 1 */
#define DRV2    0x80 /* Drive 2 of Lisa 1, or unique drive of Lisa 2 */

/* Disk controller commands */
#define EXRW 0x81 /* Execute RWTS function */

/* RWTS functions */
#define UNCLAMP 0x02 /* Eject floppy */

/******************************************************************************/
/* Variables                                                                  */
/******************************************************************************/

/* The delays are initialized in lisa_kbd_init() */
static ULONG delay15us;
static ULONG delay40ms;

/******************************************************************************/
/* Screen                                                                     */
/******************************************************************************/

void lisa_screen_init(void)
{
    /* This is not really true, but the closest we can do */
    sshiftmod = ST_HIGH;

    /* Enable the Vertical Retrace Interrupt (VBL) */
    VEC_LEVEL1 = lisa_int_1;
    FORCE_READ(VTIRENB);
}

void lisa_setphys(const UBYTE *addr)
{
    /* Must be a multiple of 32 KB */
    /* FIXME: MMU Logical to Physical address translation.
     * It can work without this implementation, because we are lucky:
     * The boot ROM sets the screen at the end of the RAM,
     * precisely where EmuTOS expects it. */
    /*VIDLTCH = (ULONG)addr >> 15;*/
}

const UBYTE *lisa_physbase(void)
{
    /* FIXME: MMU Physical to Logical address translation */
    /*return (UBYTE*)((ULONG)(VIDLTCH & 0x3f) << 15);*/
    return logbase();
}

/******************************************************************************/
/* COPS: Control Oriented Processor System                                    */
/* Used as Keyboard/Mouse/Clock/Shutdown slave processor                      */
/******************************************************************************/

/* Write a byte to COPS */
static void lisa_write_cops(UBYTE b)
{
    UWORD old_sr;

    /* Ensure we will not be interrupted */
    old_sr = set_sr(0x2700);

    /* Write the byte to port A, no handshake */
    PORTA1 = b;

    /* Wait for a first ready state: CRDY bit = 0 */
    while (ORB1 & 0x40);

    /* Wait for CRDY bit to disappear */
    delay_loop(delay15us);

    /* Wait for a second ready state: CRDY bit = 0 */
    while (ORB1 & 0x40);

    /* Set Port A direction as output to send the data */
    DDRA1 = 0xff;

    /* Wait for not-ready state: CRDY bit = 1 */
    while (!(ORB1 & 0x40));

    /* Hold data long enough so COPS can read it */
    delay_loop(delay40ms);

    /* Reset Port A direction to input */
    DDRA1 = 0x00;

    set_sr(old_sr);
}

/* Return TRUE if there is a pending byte from COPS */
static BOOL lisa_cops_can_read(void)
{
    return !!(IFR1 & INT_CA1);  /* Check CA1 pin: BSY */
}

/* Read a byte from COPS */
static UBYTE lisa_read_cops(void)
{
    while (!lisa_cops_can_read()); /* Wait */

    return ORA1; /* Read byte and clear interrupt */
}

/******************************************************************************/
/* Keyboard and Mouse                                                         */
/******************************************************************************/

void lisa_kbd_init(void)
{
    delay15us = loopcount_1_msec / 65;
    delay40ms = loopcount_1_msec * 40;

    /* Enable mouse interrupts every 20ms */
    lisa_write_cops(0x70 | 0x08 | 0x05);
    VEC_LEVEL2 = lisa_int_2;
}

/* LisaEm hack! Here are locations of Lisa OS variables for mouse position.
 * They are monitored by LisaEm to generate proper mouse events.
 * Fortunately, the trp14ret system variable (same address) is not used */
#define LisaOS_MousX (*(volatile UWORD*)0x486)
#define LisaOS_MousY (*(volatile UWORD*)0x488)

static BOOL lisa_button; /* True if the mouse button is currently down */

/* Send a mouse packet to the IKBD handler */
static void lisa_send_mouse_packet(SBYTE dx, SBYTE dy)
{
    SBYTE packet[3];
    packet[0] = 0xf8; /* IKBD mouse packet header */

    if (lisa_button)
        packet[0] |= 0x02; /* Left button is currently down */

    packet[1] = dx;
    packet[2] = dy;

    KDEBUG(("call_mousevec %d %d\n", dx, dy));
    call_mousevec(packet);

    /* LisaEm expects the mouse position to be in Lisa OS variables.
     * So we mirror the Line-A mouse position there. */
    LisaOS_MousX = GCURX;
    LisaOS_MousY = GCURY;
}

/* Table to translate scancodes from Lisa keyboard to Atari keyboard */
static const UBYTE scancode_atari_from_lisa[128] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x4a,
    0x6a, 0x6b, 0x6c, 0x4e, 0x71, 0x6e, 0x6f, 0x72,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x0d, 0x53, 0x60, 0x19, 0x0e, 0x00, 0x00,
    0x1c, 0x70, 0x00, 0x00, 0x35, 0x6d, 0x00, 0x00,
    0x0a, 0x0b, 0x16, 0x17, 0x24, 0x25, 0x1a, 0x1b,
    0x32, 0x26, 0x27, 0x28, 0x39, 0x33, 0x34, 0x18,
    0x12, 0x07, 0x08, 0x09, 0x06, 0x13, 0x14, 0x15,
    0x01, 0x21, 0x22, 0x23, 0x2f, 0x2e, 0x30, 0x31,
    0x1e, 0x03, 0x04, 0x05, 0x02, 0x10, 0x1f, 0x11,
    0x0f, 0x2c, 0x2d, 0x20, 0x1d, 0x3a, 0x2a, 0x38,
};

/* Called when we received a byte from COPS on interrupt */
static void lisa_process_cops_byte(UBYTE data)
{
    KDEBUG(("COPS byte 0x%02x\n", data));

    if (data == 0x80)
    {
        /* Reset code */
        UBYTE byte1 = lisa_read_cops();
        if ((byte1 & 0xf0) == 0xe0)
        {
            /* Date packet. Ignore. */
            int i;
            for (i = 2; i < 7; i++)
                FORCE_READ(lisa_read_cops());
        }
    }
    else if (data == 0)
    {
        /* Mouse move */
        SBYTE dx = lisa_read_cops();
        SBYTE dy = lisa_read_cops();
        lisa_send_mouse_packet(dx, dy);
    }
    else if (data == 0x86)
    {
        /* Mouse button down */
        lisa_button = TRUE;
        lisa_send_mouse_packet(0, 0);
    }
    else if (data == 0x06)
    {
        /* Mouse button up */
        lisa_button = FALSE;
        lisa_send_mouse_packet(0, 0);
    }
    else
    {
        /* Keyboard */
        BOOL press = !!(data & 0x80);
        UBYTE scancode_lisa = data & 0x7f;
        UBYTE scancode_atari = scancode_atari_from_lisa[scancode_lisa];
        UBYTE ikbdbyte = scancode_atari;

        if (!press)
            ikbdbyte |= 0x80; /* Release key */

        KDEBUG(("call_ikbdraw 0x%02x\n", ikbdbyte));
        call_ikbdraw(ikbdbyte);
    }
}

/******************************************************************************/
/* System timer                                                               */
/******************************************************************************/

#define DOTCK 20375000 /* Main hardware clock, in Hz */
#define CPUCK (DOTCK / 4) /* CPU clock = 5.09375 MHz */
#define LISA1_VIA_CLOCK (CPUCK / 10) /* Slow timers */
#define LISA2_VIA_CLOCK (CPUCK / 4) /* Fast timers */

void lisa_init_system_timer(void)
{
    /* FIXME: Check for slow timers */
    UWORD divisor = LISA2_VIA_CLOCK / CLOCKS_PER_SEC;

    /* Use VIA2 Timer 1 as system timer */
    ACR2 = (ACR2 & ~0xc0) | 0x40; /* Timer 1 in free-running mode */
    T1CL2 = LOBYTE(divisor); /* Must be set *before* T1CH2 */
    T1CH2 = HIBYTE(divisor); /* Start the timer */
    IER2 = IER_ENABLE | INT_TIMER1; /* Enable Timer 1 interrupt */

    /* VEC_LEVEL1 has already been initialized by lisa_screen_init() */
}

/******************************************************************************/
/* Interrupts                                                                 */
/******************************************************************************/

/* VIA1 interrupt */
static void lisa_int_via1(UBYTE ifr1)
{
    if (IER1 & ifr1 & INT_CA1)
    {
        /* COPS input */
        UBYTE data = ORA1;
        lisa_process_cops_byte(data);
    }
}

/* INT2 C handler. Called by assembler lisa_int_2() */
void lisa_int_2_c(void)
{
    UBYTE ifr1 = IFR1;

    if (ifr1 & IFR_IRQ)
    {
        /* VIA1 interrupt */
        lisa_int_via1(ifr1);
    }
}

/* VIA2 interrupt */
static void lisa_int_via2(UBYTE ifr2)
{
    if (IER2 & ifr2 & INT_TIMER1)
    {
        /* Timer 1 */
        lisa_call_5ms();
        FORCE_READ(T1CL2); /* Acknowledge */
    }
}

/* INT1 C handler. Called by assembler lisa_int_1() */
void lisa_int_1_c(void)
{
    UBYTE ifr2 = IFR2;

    if (ifr2 & IFR_IRQ)
    {
        /* VIA2 interrupt */
        lisa_int_via2(ifr2);
    }

    if (!(STATREG & 0x04))
    {
        /* Vertical Retrace Interrupt (VBL) */
        int_vbl();
        FORCE_READ(VTIRDIS); /* Acknowledge ??? */
        FORCE_READ(VTIRENB); /* Re-enable interrupt */
    }
}

/******************************************************************************/
/* Shutdown                                                                   */
/******************************************************************************/

void lisa_shutdown(void)
{
    lisa_write_cops(0x21); /* Power off */
}

/******************************************************************************/
/* Clock                                                                      */
/******************************************************************************/

static int is_leap_year(int year)
{
    if ((year % 400) == 0)
        return 1;

    if ((year % 100) == 0)
        return 1;

    if ((year % 4) == 0)
        return 1;

    return 0;
}

static void lisa_extract_day_month(int year, int day_of_year, int *day, int *month)
{
    int leap = is_leap_year(year);

    if (day_of_year < 31)
        *day = day_of_year + 1, *month = 1;
    else if (day_of_year < 59 + leap)
        *day = day_of_year + 1 - 31, *month = 2;
    else if (day_of_year < 90 + leap)
        *day = day_of_year + 1 - 59 - leap, *month = 3;
    else if (day_of_year < 120 + leap)
        *day = day_of_year + 1 - 90 - leap, *month = 4;
    else if (day_of_year < 151 + leap)
        *day = day_of_year + 1 - 120 - leap, *month = 5;
    else if (day_of_year < 181 + leap)
        *day = day_of_year + 1 - 151 - leap, *month = 6;
    else if (day_of_year < 212 + leap)
        *day = day_of_year + 1 - 181 - leap, *month = 7;
    else if (day_of_year < 243 + leap)
        *day = day_of_year + 1 - 212 - leap, *month = 8;
    else if (day_of_year < 273 + leap)
        *day = day_of_year + 1 - 243 - leap, *month = 9;
    else if (day_of_year < 304 + leap)
        *day = day_of_year + 1 - 273 - leap, *month = 10;
    else if (day_of_year < 334 + leap)
        *day = day_of_year + 1 - 304 - leap, *month = 11;
    else
        *day = day_of_year + 1 - 334 - leap, *month = 12;
}

/* Read Date/Time from hardware */
ULONG lisa_getdt(void)
{
    WORD old_sr;
    UBYTE data[7];
    int i;
    int dig100, dig10, dig1;
    int lisa_year, header_year, year;
    int day_of_year;
    int day;
    int month;
    int hour;
    int minute;
    int second;
    UWORD date, time;

    /* Disable the interrupts now,
     * otherwise the interrupt handler will eat the bytes! */
    old_sr = set_sr(0x2700);

    /* Ask COPS for clock data */
    lisa_write_cops(0x02);

    /* And poll the resulting packet */
    for (i = 0; i < 7; i++)
        data[i] = lisa_read_cops();

    set_sr(old_sr);

    /* Year */
    lisa_year = 1980 + (data[1] & 0x0f);
    /* Ignore the hardware year, because it is only valid between 1980-1995 */
    header_year = 1980 + (os_header.os_dosdate >> 9); /* Best guess */
    year = header_year;

    /* Month and day */
    dig100 = (data[2] & 0xf0) >> 4;
    dig10 = (data[2] & 0x0f);
    dig1 = (data[3] & 0xf0) >> 4;
    day_of_year = dig100*100 + dig10*10 + dig1;
    lisa_extract_day_month(lisa_year, day_of_year, &day, &month);
    KDEBUG(("year=%d day_of_year=%d month=%d day=%d\n", year, day_of_year, month, day));

    /* Hour */
    dig10 = (data[3] & 0x0f);
    dig1 = (data[4] & 0xf0) >> 4;
    hour = dig10*10 + dig1;

    /* Minute */
    dig10 = (data[4] & 0x0f);
    dig1 = (data[5] & 0xf0) >> 4;
    minute = dig10*10 + dig1;

    /* Second */
    dig10 = (data[5] & 0x0f);
    dig1 = (data[6] & 0xf0) >> 4;
    second = dig10*10 + dig1;
    KDEBUG(("time=%02d:%02d:%02d\n", hour, minute, second));

    date = (((year-1980) & 0x7f) << 9) | ((month & 0xf) << 5) | (day & 0x1f);
    time = (hour << 11) | (minute << 5) | (second >> 1);

    return MAKE_ULONG(date, time);
}

void lisa_floppy_init(void)
{

}

BOOL lisa_flop_detect_drive(WORD dev)
{
    return dev == 0; /* Assume that A: is always connected */
}

/* Read or write sectors from floppy.
 * Parameters assume 10 sectors on all tracks,
 * while actual Lisa floppies have a variable number of sectors per track.
 * So we convert the dummy track/sector numbers into an absolute sector number.
 */
WORD lisa_floprw(UBYTE *buf, WORD rw, WORD dev, WORD sect, WORD track, WORD side, WORD count)
{
    ULONG lba; /* Absolute sector number */
    WORD i;

    if (dev != 0 || side != 0)
        return EBADRQ;

    if (rw != 0)
        return EWRPRO; /* Write is not supported */

    lba = track * 10 + (sect - 1);

    for (i = 0; i < count; i++)
    {
        ULONG sector_lba = lba + i;
        UBYTE *sector_buffer = buf + i*512;
        lisa_read_lba_sector(sector_lba, sector_buffer);
        KDEBUG(("lba=%lu data=[%02x %02x %02x %02x ...]\n", sector_lba, sector_buffer[0], sector_buffer[1], sector_buffer[2], sector_buffer[3]));
    }

    return E_OK;
}

LONG lisa_flop_mediach(WORD dev)
{
    /* FIXME: Implement proper floppy eject and media change */
    return MEDIANOCHANGE;
}

void lisa_flop_eject(void)
{
    DRV = DRV2; /* Lisa 2 unique drive */
    CMD = UNCLAMP;
    GOBYTE = EXRW;
    /* FIXME: Wait for interrupt */
}

#endif /* MACHINE_LISA */
