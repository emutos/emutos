/*
 *  initinfo.c - Info screen at startup
 *
 * Copyright (c) 2001-2008 by Authors:
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

#include "config.h"
#include "portab.h"
#include "kprint.h"
#include "nls.h"
#include "ikbd.h"
#include "asm.h"
#include "blkdev.h"     /* for BLKDEVNUM */
//#include "lineavars.h"
#include "font.h"
#include "tosvars.h"
#include "machine.h"
#include "processor.h"
#include "xbiosbind.h"
#include "version.h"

#include "initinfo.h"

int early_cli;

#if FULL_INITINFO


/*==== Defines ============================================================*/

/* allowed values for Mxalloc mode: (defined in mem.h) */
#define MX_STRAM 0
#define MX_TTRAM 1

/*==== External declarations ==============================================*/

/* mxalloc (defined in bdos/mem.h) */
extern long xmxalloc(long amount, int mode);


/*
 * set_margin - Set
 */

static void set_margin(void)
{
    WORD marl;
    WORD celx;

    marl=(v_cel_mx-34) / 2;     /* 36 = length of Logo */

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
        cprintf("_");

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

static void cprint_asctime(void)
{
    int years, months, days;
    int hours, minutes, seconds;
    ULONG system_time = Gettime(); /* Use the trap interface as a workaround
                                      to wrong Steem IKBD date after 16:00 */
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

/*
 * initinfo - Show initial configuration at startup
 */

void initinfo(void)
{
#if CONF_WITH_AROS
    int initinfo_height = 24;
#else
    int initinfo_height = 22;
#endif
    int i;

    /* Center the initinfo screen vertically */
    for (i = 0; i < (v_cel_my - initinfo_height) / 2; i++)
        cprintf("\r\n");

    /* Now print the EmuTOS Logo */
    print_art("11111111111 7777777777  777   7777");
    print_art("1                  7   7   7 7    ");
    print_art("1111   1 1  1   1  7   7   7  777 ");
    print_art("1     1 1 1 1   1  7   7   7     7");
    print_art("11111 1   1  111   7    777  7777 ");

    /* Just a separator */
    set_line();
    cprintf("\n\r");

    pair_start(_("EmuTOS Version")); cprintf("%s", version); pair_end();

    pair_start(_("CPU type"));
#ifdef __mcoldfire__
    cprintf("ColdFire V4e");
#else
    cprintf("m680%02ld", mcpu);
#endif
    pair_end();

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
        LONG mask;
        for(i=0, mask=1L; i<BLKDEVNUM; i++, mask <<=1) {
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
    set_line();
    cprintf("\n\r");

    set_margin(); cprintf(_("Hold <Control> to skip AUTO/ACC"));
    cprintf("\r\n");
    set_margin(); cprintf(_("Hold <Alternate> to skip HDD boot"));
    cprintf("\r\n");
#if WITH_CLI
    set_margin(); cprintf(_("Press 'C' to run an early console"));
    cprintf("\r\n");
#endif
    cprintf("\r\n");
#if CONF_WITH_AROS
    set_margin(); cprintf("\033pWarning: This binary mixes GPL and AROS \033q\r\n");
    set_margin(); cprintf("\033psources, redistribution is forbidden.   \033q\r\n");
#endif
    cprintf("\r\n");
    set_margin();
    cprintf("\033p");
    cprintf(_(" Hold <Shift> to pause this screen "));
    cprintf("\033q");

    /* pause for a short while (or longer if a Shift key is held down) */
    {
        /* Wait until timeout or keypress */
        long end = hz_200 + INITINFO_DURATION * 200UL;
        LONG shiftbits;
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

        /* Wait while Shift is pressed */
        while (shiftbits & MODE_SHIFT)
        {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
            stop_until_interrupt();
#endif
            shiftbits = kbshift(-1);
        }
    }
    if (bconstat2()) {  /* examine the keypress */
        int c = 0xFF & bconin2();
        MAYBE_UNUSED(c);
#if WITH_CLI
        if (c == 'c' || c == 'C') {
            early_cli = 1;
        }
#endif
    }
    cprintf("\r\033K"); /* remove the "Hold Shift" message */
}


#else    /* FULL_INITINFO */


void initinfo(void)
{
    cprintf("EmuTOS Version %s\r\n", version);
}


#endif   /* FULL_INITINFO */
