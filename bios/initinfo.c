/*
 *  initinfo.c - Info screen at startup
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD     Martin Doering
 *  joy     Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
 * Well, this can be made nicer later, if we have much time... :-)
 */

#include "config.h"
#include "portab.h"
#include "kprint.h"
#include "nls.h"
#include "ikbd.h"

#include "lineavars.h"
#include "tosvars.h"
#include "machine.h"
#include "clock.h"    /* for displaying boot date and time */

extern char version[];  /* the version string */

#include "initinfo.h"


#if TOS_VERSION >= 0x200   /* Don't include splashscreen on TOS 1.0x to
                              save some space in the ROM image */


/*==== Defines ============================================================*/

/* allowed values for Mxalloc mode: (defined in mem.h) */
#define MX_STRAM 0
#define MX_TTRAM 1

/*==== External declarations ==============================================*/

/* mxalloc (defined in bdos/mem.h) */
long xmxalloc(long amount, int mode);


/*
 * set_margin - Set
 */

static void set_margin(void)
{
    WORD marl;
    WORD celx;

    marl=(v_cel_mx-34) / 2;     /* 36 = lenght of Logo */

    cprintf("\r");              /* goto left side */

    /* count for columns */
    for (celx = 0; celx<=marl; celx++)
        cprintf(" ");
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


static void set_line(void)
{
    WORD llen;
    WORD celx;

    llen=v_cel_mx - 6 ;     /* line length */

    cprintf("\r   ");          /* goto left side */

    /* count for columns */
    for (celx = 0; celx<=llen; celx++)
        cprintf("-");

    cprintf("\r\n");          /* goto left side */
}


static void pair_start(const char *left)
{
    int n;
    set_margin();
    n = cprintf(left);
    cprintf(": ");
    while(n++ < 14) 
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

static void cprint_asctime()
{
    int years, months, days;
    int hours, minutes, seconds;
    ULONG system_time = gettime();  /* XBIOS directly (shouldn't use TRAP?) */
    seconds = (system_time & 0x1F) * 2;
    system_time >>= 5;
    minutes = system_time & 0x3F;
    system_time >>= 6;
    hours = system_time & 0x1F;
    system_time >>= 5;
    days = system_time & 0x1F;
    system_time >>= 5;
    months = (system_time & 0x0F);
    system_time >>= 4;
    years = (system_time & 0x7F) + 1980;
    cprintf("%04d/%02d/%02d %02d:%02d:%02d", years, months, days, hours, minutes, seconds);
}

#ifdef TIMEOUT_ON_BOOT
/* display help and wait for <TIMEOUT_ON_BOOT> seconds before boot continues */
static void draw_timeout_line()
{
#if TIMEOUT_ON_BOOT > 0
    char bar[]="                                  ";
    int barsize = sizeof(bar);
    int oldidx;
    long start = hz_200;
    long timeout = TIMEOUT_ON_BOOT * 200UL;
    long end = start + timeout;

    set_margin(); cprintf(_("Hold <Control> to skip AUTO/ACC"));
    cprintf("\r\n");
    set_margin(); cprintf(_("Hold <Alternate> to skip HDD"));
    cprintf("\r\n");
    set_margin(); cprintf(_("Press any key to continue booting"));
    cprintf("\r\n");

    oldidx = -1;
    while(hz_200 < end) {
        int idx = (((hz_200 - start) * barsize) / timeout);
        if (idx > oldidx) {
            set_margin();
            cprintf("\033p");
            cprintf(bar + idx);
            cprintf("\033q\033K\r");
            oldidx = idx;
        }
        if (kbshift(-1) || bconstat2())
            break;
    }
    if (bconstat2()) bconin2(); /* eat the keypress */
#endif /* TIMEOUT_ON_BOOT > 0 */
}
#endif /* TIMEOUT_ON_BOOT */

/*
 * initinfo - Show initial configuration at startup
 */

void initinfo()
{

    /* Clear screen - Esc E */
    cprintf("\033E\r\n");

    /* foreground is color 15, background is color 0 */
    cprintf("\033b%c\033c%c", 15 + ' ', 0 + ' ');

    /* Now print the EmuTOS Logo */
    print_art("11111111111 7777777777  777   7777");
    print_art("1                  7   7   7 7    ");
    print_art("1111   1 1  1   1  7   7   7  777 ");
    print_art("1     1 1 1 1   1  7   7   7     7");
    print_art("11111 1   1  111   7    777  7777 ");
    
    /* Just a separator */
    cprintf("\n\r");
    set_line();
    cprintf("\n\r");

    pair_start(_("EmuTOS Version")); cprintf("%s", version); pair_end();
    pair_start(_("CPU type")); cprintf("m680%02ld", mcpu); pair_end();
    pair_start(_("Machine")); cprintf(machine_name()); pair_end();
/*  pair_start(_("MMU available")); cprintf(_("No")); pair_end(); */
    pair_start(_("Free ST-RAM"));
        cprintf(_("%ld kB"), /* memtop-membot */ xmxalloc(-1L, MX_STRAM) >> 10);
    pair_end();

    {
        long fastramsize = xmxalloc(-1L, MX_TTRAM);
        if (fastramsize > 0) {
            pair_start(_("Free FastRAM"));
            cprintf(_("%ld kB"), fastramsize >> 10);
            pair_end();
        }
    }

    pair_start(_("Screen start")); cprintf("%p", v_bas_ad);
    pair_end();
    pair_start(_("GEMDOS drives"));
    {
        int i;
        int mask;
        for(i=0, mask=1; i<26; i++, mask <<=1) {
            if (drvbits & mask)
                cprintf("%c", 'A'+i);
        }
    }
    pair_end();
    /* boot drive is unknown at this moment since blkdev_hdv_boot is now
       executed after the initinfo is printed:
    pair_start(_("Boot drive")); cprintf("%c:", bootdev+65); pair_end();
    */
    pair_start(_("Boot time")); cprint_asctime(); pair_end();

    /* Just a separator */
    cprintf("\n\r");
    set_line();
    cprintf("\n\r");

#ifdef TIMEOUT_ON_BOOT
    draw_timeout_line();
#endif
}


#else    /* TOS_VERSION >= 0x200 */


void initinfo()
{
    /* Clear screen, set foreground to 15, background is color 0 */
    cprintf("\033E\033b%c\033c%c", 15 + ' ', 0 + ' ');

    cprintf("EmuTOS Version %s\r\n", version);
}


#endif   /* TOS_VERSION >= 0x200 */
