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

#include "lineavars.h"
#include "tosvars.h"



/*==== Defines ============================================================*/



/*
 * set_margin - Set
 */

void set_margin(void)
{
    WORD marl;
    WORD celx;

    marl=(v_cel_mx-34) / 2;     /* 36 = lenght of Logo */

    cprintf("\r");              /* goto left side */

    /* count for columns */
    for (celx = 0; celx<=marl; celx++)
        cprintf(" ");
}



void set_middle(void)
{
    WORD marl;
    WORD celx;

    marl=v_cel_mx/2 + 3 ;     /* 36 = lenght of Logo */

    cprintf("\r");              /* goto left side */

    /* count for columns */
    for (celx = 0; celx<=marl; celx++)
        cprintf(" ");
}



void set_line(void)
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

    set_margin(); cprintf("[ OK ] EmuTOS Ver.:  \eb!Alpha Version\eb/ \r\n");
    set_margin(); cprintf("[ OK ] CPU type:     \eb!m68000\eb/ \r\n");
    set_margin(); cprintf("[ OK ] MMU avail.:   \eb!No\eb/ \r\n");
    set_margin(); cprintf("[ OK ] Free memory:  \eb!%ld bytes\eb/ \r\n", memtop-membot);
    set_margin(); cprintf("[ OK ] Screen start: \eb!0x%lx\eb/ \r\n", (long)v_bas_ad);
    set_margin(); cprintf("[ OK ] Boot drive :  \eb!%c:\eb/ \r\n", bootdev+65);

    /* switch on cursor - Esc E */
    cprintf("\ee");

    /* Just a seperator */
    cprintf("\n\r");
    set_line();
    cprintf("\n\r");
}

