/*
 *  kbd.h - header file for keyboard/console routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
**  KBCHAR - type for characters comming in from logical keyboard
**	these are longs because we pass back a UWORD full of info  
**	(scan code + char code), and have -1 mean error.
*/

typedef	LONG	KBCHAR ;

typedef	BYTE	KBSCN ; /* type for kbd scan code */


/*
 *  status registers
 */

#define NOCHAR		0		/* no character was returned	     */
#define BREAK		1		/* break key was struck		     */
#define KBD_LOCKED	2		/* key on front panel in lock pos.   */
#define IN_CHAR		3		/* a real live character available   */



/*
**  external declarations
*/

EXTERN VOID kbd_init(VOID) ;
EXTERN ERROR kbd_select(VOID) ;
EXTERN ERROR kbd_disab(VOID) ;
EXTERN VOID mode_chg(VOID) ;
EXTERN VOID	kbd_int(VOID) ;
EXTERN KBCHAR kbd_read(VOID) ;

