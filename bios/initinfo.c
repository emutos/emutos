/*
 *  initinfo.c - Info screen at startup
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
 * Well, this can be made nicer later, if we have much time... :-)
 */

#include "portab.h"
#include "kprint.h"
#include "nls.h"

#include "lineavars.h"
#include "tosvars.h"
#include "machine.h"
#include "clock.h"    /* for displaying current date and time */

#include "initinfo.h"

/*==== Defines ============================================================*/



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
    set_margin();
    cprintf("[ OK ] ");
    cprintf(left);
    cprintf("\033b!");
}

static void pair_end(void)
{
    cprintf("\033b/ \r\n");
}

/*
 * cprint_asctime shows current date and time in YYYY/MM/DD HH:MM:SS format
 */

void cprint_asctime()
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
#if 0
    set_margin();
    cprintf("\ec!           \ec  \ec'          \ec   \ec'   \ec    \ec'    \ec \r\n");
    set_margin();
    cprintf("\ec! \ec                   \ec' \ec    \ec' \ec    \ec' \ec  \ec' \ec     \r\n");
    set_margin();
    cprintf("\ec!    \ec    \ec! \ec  \ec! \ec   \ec! \ec    \ec! \ec   \ec' \ec    \ec' \ec    \ec' \ec   \ec'   \ec  \r\n");
    set_margin();
    cprintf("\ec! \ec      \ec! \ec  \ec! \ec  \ec! \ec  \ec! \ec    \ec! \ec   \ec' \ec    \ec' \ec    \ec' \ec      \ec' \ec     \r\n");
    set_margin();
    cprintf("\ec!     \ec  \ec! \ec    \ec! \ec   \ec!   \ec    \ec' \ec     \ec'   \ec   \ec'    \ec  \r\n");
#else
    print_art("11111111111 7777777777  777   7777");
    print_art("1                  7   7   7 7    ");
    print_art("1111   1 1  1   1  7   7   7  777 ");
    print_art("1     1 1 1 1   1  7   7   7     7");
    print_art("11111 1   1  111   7    777  7777 ");
#endif
    
    /* Just a separator */
    cprintf("\n\r");
    set_line();
    cprintf("\n\r");

    pair_start(_("EmuTOS Ver.:  ")); cprintf(_("Alpha Version")); pair_end();
    pair_start(_("CPU type:     ")); cprintf("m680%02ld", mcpu); pair_end();
    pair_start(_("Machine:      ")); cprintf(machine_name()); pair_end();
    pair_start(_("MMU avail.:   ")); cprintf(_("No")); pair_end();
    pair_start(_("Free memory:  ")); cprintf(_("%ld bytes"), memtop-membot);
    pair_end();
    pair_start(_("Screen start: ")); cprintf("0x%lx", (long)v_bas_ad);
    pair_end();
    pair_start(_("Boot drive :  ")); cprintf("%c:", bootdev+65); pair_end();
    pair_start(_("Curr. time :  ")); cprint_asctime(); pair_end();

    /* Just a separator */
    cprintf("\n\r");
    set_line();
    cprintf("\n\r");
}

