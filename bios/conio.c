/*
 * conio.c - Console/keyboard I/O routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  SCC     Steve C. Cavender
 *  KTB     Karl T. Braun (kral)
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "bios.h"
#include "kbd.h"
#include "kprint.h"
#include "fontdef.h"



/*==== external declarations for graphics =================================*/
extern void cputc(WORD);
extern void linea_init();                /* Init linea variables */



/*==== Defines ============================================================*/



/*==== global variables ===================================================*/

/* If con_stat gets a valid read from the driver, the
 * character is stored here, and char_avail is set true.
 */

KBCHAR	_char_save;     /* peek ahead buffer */
WORD	_char_avail;   /* true, if char_save contains a character */



/**
 * con_init - initialize console
 */

VOID con_init()
{
    _char_save = 0 ;            /* peek ahead buffer */
    _char_avail = FALSE ;       /* has a character been typed? */
#if NEEDED
    linea_init();                /* Init linea variables - now in startup.s*/
#endif
}



/**
 * con_stat - get console input status
 *
 * See if we have a character saved from the last call to con_stat. If we
 * do, return HAVE_CHAR.  If not, try and read a character from the
 * driver. If we get one, save it and return (HAVE_CHAR). Otherwise return 0.
 *
 *  returns:
 *	-1:	if character available	(HAVE_CHAR)
 *	 0:	if not
 */

LONG con_stat()
{
    KBCHAR kbd_read() ;

    if( ! _char_avail  )
    {
        if( (_char_save=kbd_read()) == -1L )
            return( 0 ) ;
        _char_avail = TRUE ;
    }
    return( -1L ) ;
}
 

/**
 * con_in - wait for and return a character from the console
 *
 * wait for a con_stat to indicate that there is a character available
 * then fetch it.
 */

LONG con_in()
{
    while( ! con_stat() );		/* wait for input */

    _char_avail = FALSE ;
    return( _char_save );
}



/**
 * con_out - output one character to the console
 */

void con_out(BYTE chr)
{
    WORD what;

    what=(WORD)chr;
    cputc(what);
}
