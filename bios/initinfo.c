/*
 *  initinfo.c - Info screen at startup
 *
 * Copyright (C) 2001-2020 by Authors:
 *
 * Authors:
 *  MAD     Martin Doering
 *  PES     Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
 * Well, this can be made nicer later, if we have much time... :-)
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "nls.h"
#include "ikbd.h"
#include "asm.h"
#include "string.h"
#include "blkdev.h"     /* for BLKDEVNUM */
#include "font.h"
#include "tosvars.h"
#include "machine.h"
#include "processor.h"
#include "xbiosbind.h"
#include "biosext.h"
#include "version.h"
#include "bios.h"

#include "initinfo.h"
#include "conout.h"
#include "../bdos/bdosstub.h"
#include "lineavars.h"

/* Screen width, in characters, as signed value */
#define SCREEN_WIDTH ((WORD)v_cel_mx + 1)

#if FULL_INITINFO

#define INFO_LENGTH 40      /* width of info lines (must fit in low-rez) */
#define LOGO_LENGTH 34      /* must equal length of strings in EmuTOS logo */

static const char logo[][LOGO_LENGTH+1] =
    { "11111111111 7777777777  777   7777",
      "1                  7   7   7 7    ",
      "1111   1 1  1   1  7   7   7  777 ",
      "1     1 1 1 1   1  7   7   7     7",
      "11111 1   1  111   7    777  7777 " };

/* Print n spaces */
static void print_spaces(WORD n)
{
    WORD i;

    for (i = 0; i < n; i++)
        cprintf(" ");
}

static WORD left_margin;

/*
 * set_margin - Set
 */

static void set_margin(void)
{
    cprintf("\r");              /* goto left side */
    print_spaces(left_margin);
}

/* print a line in which each char stands for the background color */
static void print_art(const char *s)
{
    int old = -1;
    int color;

    set_margin();
    while(*s) {
        color = (*s++) & 15;
        if(color != old) {
            cprintf("\033c%c", color + 32);
            old = color;
        }
        cprintf(" ");
    }
    if(old != 0) {
        cprintf("\033c ");
    }
    cprintf("\r\n");
}


/*
 * display a separator line
 */
static void set_line(void)
{
    WORD celx;

    set_margin();

    /* count for columns */
    for (celx = 0; celx < INFO_LENGTH; celx++)
        cprintf("_");

    cprintf("\r\n\r\n");    /* followed by blank line */
}


/*
 * display a message followed by cr/lf
 */
static void display_message(const char *s)
{
    set_margin();
    cprintf(s);
    cprintf("\r\n");
}


/*
 * display a message in inverse video, with optional cr/lf
 */
static void display_inverse(const char *s,BOOL crlf)
{
    WORD len = strlen(s);
    WORD left = (INFO_LENGTH - len) / 2;
    WORD right = INFO_LENGTH - (left + len);

    set_margin();

    cprintf("\033p");
    print_spaces(left);
    cprintf(s);
    print_spaces(right);
    cprintf("\033q");

    if (crlf)
        cprintf("\r\n");
}


static void pair_start(const char *left)
{
    int n;
    set_margin();
    n = cprintf(left) + 2;      /* allow for following cprintf() */
    cprintf(": ");
    while(n++ < INFO_LENGTH/2)
        cprintf(" ");
    cprintf("\033b!");
}

static void pair_end(void)
{
    cprintf("\033b/ \r\n");
}

/*
 * cprint_asctime shows boot date and time in YYYY/MM/DD HH:MM:SS format
 */

static void cprint_asctime(void)
{
    int years, months, days;
    int hours, minutes, seconds;
    BOOL bad_clock = FALSE;
    ULONG system_time = Gettime();

    seconds = (system_time & 0x1F) * 2;
    system_time >>= 5;

    /*
     * if the system time is not later than the default date/time, it is
     * invalid (the default date/time is 00:00 on the build date).
     *
     * note: if we do not have an RTC and are therefore using the IKBD
     * clock, and if we found it to be invalid (see clock_init() in
     * clock.c), we will have set the clock to the default date/time.
     * it could now be a second or two later, so if we included seconds
     * when comparing the clock to the default date/time, we would not
     * detect a bad clock.  therefore we ignore the seconds.
     */
    if (system_time <= (DEFAULT_DATETIME>>5))
        bad_clock = TRUE;

    minutes = system_time & 0x3F;
    system_time >>= 6;
    hours = system_time & 0x1F;
    system_time >>= 5;
    days = system_time & 0x1F;
    system_time >>= 5;
    months = (system_time & 0x0F);
    system_time >>= 4;
    years = (system_time & 0x7F) + 1980;

    /*
     * if the date/time is invalid, show it in inverse video
     */
    if (bad_clock)
        cprintf("\033p");
    cprintf("%04d/%02d/%02d %02d:%02d:%02d", years, months, days, hours, minutes, seconds);
    if (bad_clock)
        cprintf("\033q");
}

/*
 * display the available devices, with the 'dev' device highlighted
 */
static void cprint_devices(WORD dev)
{
    int i;
    LONG mask;

    cprintf("\033k\033j");  /* restore, then resave, current cursor posn */

    pair_start(_("GEMDOS drives"));

    for (i = 0, mask = 1L; i < BLKDEVNUM; i++, mask <<= 1) {
        if (drvbits & mask) {
            if (i == dev)
                cprintf("\033p");
            cprintf("%c",'A'+i);
            if (i == dev)
                cprintf("\033q");
        }
    }

    pair_end();
}

/*
 * display a size in bytes with best multiple unit
 */
static void cprintf_bytesize(ULONG bytes)
{
    const char *unit;
    ULONG value;

    if (bytes % (1024UL * 1024UL) == 0) {
        unit = _("MB");
        value = bytes / (1024UL * 1024UL);
    }
    else {
        unit = _("KB");
        value = bytes / 1024UL;
    }

    cprintf("%lu %s", value, unit);
}

/*
 * initinfo - Show initial configuration at startup
 *
 * returns the selected boot device
 */
WORD initinfo(ULONG *pshiftbits)
{
    int screen_height = v_cel_my + 1;
    int initinfo_height = 19; /* Define ENABLE_KDEBUG to guess correct value */
    int top_margin;
#ifdef ENABLE_KDEBUG
    int actual_initinfo_height;
#endif
    int i;
    WORD olddev, dev = bootdev;
    long stramsize = (long)phystop;
#if CONF_WITH_ALT_RAM
    long altramsize = total_alt_ram();
#endif
    LONG hdd_available = blkdev_avail(HARDDISK_BOOTDEV);
    ULONG shiftbits;

    /*
     * If additional info lines are going to be printed in specific cases,
     * then initinfo_height must be adjusted in the same way here.
     */
#if WITH_CLI
    initinfo_height += 1;
#endif
#if CONF_WITH_ALT_RAM
    if (altramsize > 0)
        initinfo_height += 1;
#endif
    if (hdd_available)
        initinfo_height += 1;

    /* Center the initinfo screen vertically */
    top_margin = (screen_height - initinfo_height) / 2;
    KDEBUG(("screen_height = %d, initinfo_height = %d, top_margin = %d\n",
        screen_height, initinfo_height, top_margin));
    for (i = 0; i < top_margin; i++)
        cprintf("\r\n");

    /* Centre the logo horizontally */
    left_margin = (SCREEN_WIDTH-LOGO_LENGTH) / 2;

    /* Now print the EmuTOS Logo */
    for (i = 0; i < ARRAY_SIZE(logo); i++)
        print_art(logo[i]);

    /* adjust margins for remaining messages to allow more space for translations */
    left_margin = (SCREEN_WIDTH-INFO_LENGTH) / 2;

    /* Print separator followed by blank line */
    set_line();

    pair_start(_("EmuTOS Version")); cprintf("%s", version); pair_end();

    pair_start(_("CPU type"));
#ifdef __mcoldfire__
    cprintf("ColdFire V4e");
#else
# if CONF_WITH_APOLLO_68080
    if (is_apollo_68080)
        cprintf("Apollo 68080");
    else
# endif
        cprintf("M680%02ld", mcpu);
#endif
    pair_end();

    pair_start(_("Machine")); cprintf(machine_name()); pair_end();
    pair_start("ST-RAM"); cprintf_bytesize(stramsize); pair_end();

#if CONF_WITH_ALT_RAM
    if (altramsize > 0) {
        pair_start("Alt-RAM"); cprintf_bytesize(altramsize); pair_end();
    }
#endif

    cprintf("\033j");       /* save current cursor position */
    cprint_devices(dev);

    pair_start(_("Boot time")); cprint_asctime(); pair_end();

    /* Print separator followed by blank line */
    set_line();

    display_message(_("Hold <Control> to skip AUTO/ACC"));
    if (hdd_available) {
        display_message(_("Hold <Alternate> to skip HDD boot"));
    }
    display_message(_("Press key 'X' to boot from X:"));
#if WITH_CLI
    display_message(_("Press <Esc> to run an early console"));
#endif
    cprintf("\r\n");

    /* centre 'hold shift' message in all languages */
    display_inverse(_("Hold <Shift> to pause this screen"),0);

#ifdef ENABLE_KDEBUG
    /* We need +1 because the previous line is not ended with CRLF */
    actual_initinfo_height = v_cur_cy + 1 - top_margin;
    if (actual_initinfo_height == initinfo_height)
        KDEBUG(("initinfo_height is correct\n"));
    else
        KDEBUG(("Warning: initinfo_height = %d, should be %d.\n",
            initinfo_height, actual_initinfo_height));
#endif

    /*
     * pause for a short while, or longer if:
     *  . a Shift key is held down, or
     *  . the user selects an alternate boot drive
     */
    while (1)
    {
        /* Wait until timeout or keypress */
        long end = hz_200 + INITINFO_DURATION * 200UL;

        olddev = dev;

        do
        {
            shiftbits = kbshift(-1);

            /* If Shift, Control, Alt or normal key is pressed, stop waiting */
            if ((shiftbits & MODE_SCA) || bconstat2())
                break;

#if USE_STOP_INSN_TO_FREE_HOST_CPU
            stop_until_interrupt();
#endif
        }
        while (hz_200 < end);

        /* Wait while Shift is pressed, and normal key is not pressed */
        while ((shiftbits & MODE_SHIFT) && !bconstat2())
        {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
            stop_until_interrupt();
#endif
            shiftbits = kbshift(-1);
        }

        /* if a non-modifier key was pressed, examine it */
        if (bconstat2()) {
            int c = LOBYTE(bconin2());

            c = toupper(c);
#if WITH_CLI
            if (c == 0x1b) {
                bootflags |= BOOTFLAG_EARLY_CLI;
            } else
#endif
            {
                c -= 'A';
                if ((c >= 0) && (c < BLKDEVNUM))
                    if (blkdev_avail(c))
                        dev = c;
            }
        }
        if (dev == olddev)
            break;
        /*
         * user has changed boot device, so we display it and go round again
         */
        cprint_devices(dev);
    }

    /*
     * on exit, restore (pop) cursor position (neatness), then
     * clear screen so that subsequent text displays are clean
     */
#if CONF_SERIAL_CONSOLE_ANSI
    /* FIXME: Quick fix until save/restore cursor works with CONF_SERIAL_CONSOLE_ANSI */
    cprintf("\r\n\r\n");
#else
    cprintf("\033k\033E");
#endif

    *pshiftbits = shiftbits;
    return dev;
}


#else    /* FULL_INITINFO */


WORD initinfo(ULONG *pshiftbits)
{
    cprintf("EmuTOS Version %s\r\n", version);

    *pshiftbits = kbshift(-1);
    return bootdev;
}


#endif   /* FULL_INITINFO */
