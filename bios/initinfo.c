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


#if 0   /* unused */
static void set_middle(void)
{
    WORD marl;
    WORD celx;

    marl=v_cel_mx/2 + 3 ;     /* 36 = lenght of Logo */

    cprintf("\r");              /* goto left side */

    /* count for columns */
    for (celx = 0; celx<=marl; celx++)
        cprintf(" ");
}
#endif


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
 * initinfo - Show initial configuration at startup
 */

void initinfo()
{

    /* Clear screen - Esc E */
    cprintf("\eE\r\n");


    /* Now print the EmuTOS Logo */
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


    /* Just a seperator */
    cprintf("\n\r");
    set_line();
    cprintf("\n\r");

    pair_start(_("EmuTOS Ver.:  ")); cprintf(_("Alpha Version")); pair_end();

    pair_start(_("CPU type:     "));
    if (longframe)
        cprintf("m68010-40");
    else
        cprintf("m68000");
    pair_end();

    pair_start(_("MMU avail.:   ")); cprintf(_("No")); pair_end();
    pair_start(_("Free memory:  ")); cprintf(_("%ld bytes"), memtop-membot);
    pair_end();
    pair_start(_("Screen start: ")); cprintf("0x%lx", (long)v_bas_ad);
    pair_end();
    pair_start(_("Boot drive :  ")); cprintf("%c:", bootdev+65); pair_end();

    /* Just a separator */
    cprintf("\n\r");
    set_line();
    cprintf("\n\r");
}

