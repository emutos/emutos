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
extern void charblit(BYTE charcode, UWORD destx, UWORD desty);

/*==== Defines ============================================================*/

#define MAXCOLS		79	/* last column on physical screen */
#define MAXLINES	24	/* last line on physical screen */

#define ESC		0x1b	/* <esc> character		     */
#define TAB		0x09	/* <tab> character		     */
#define CR		0x0d	/* carriage return		     */
#define LF		0x0a	/* line feed      		     */
#define BS		0x08	/* back space			     */


#define INITATTR	0x4400	/*default attributes in character RAM*/
#define UP		0	/*indicates scroll screen up	     */
#define DOWN		1	/*indicates scroll screen down	     */

/*	states in which the screen driver can exist whenever a character
	is received.  Basically indicates possible previous sequences of 
	characters which have been received.				     */

#define STATE_ANY_CHAR	0	/* ready to receive any character    */
#define STATE_ESC		1	/* last character was an <esc>	     */
#define STATE_CURS_LINE	2	/* next char is encoded cursor line pos */
#define STATE_CURS_COL	3	/* next char is encoded cursor column pos */

/*	Logical Screen Control Block - contains information on each logical
	screen (i.e., window) which is available on the physical screen of the
	VME/10  (the first version only supports 1 logical screen and it is
	mapped to all but the last line of the physical screen.		     */

typedef struct
	{
	short	cur_line;	/*current line cursor is on	     */
	short	cur_col;	/*current column cursor is at	     */
	short	cur_cursor;	/*current offset of cursor on the 
				  screen; first position = 0;	     */
	short	base_line;	/*first line of the logical screen   */
	short	base_col;	/*first column of the logical screen */
	short	max_line;	/*last line on screen relative to base*/
	short	max_col;	/*last col on screen relative to base*/
        char	tabstop[MAXCOLS+1];/*the element corresponding to each
				   column on the screen contains the 
				   column where the next tabstop is 
				   located.			     */
	} lscb;

/* chr_map describes the word in the character and attribute RAM which
 * corresponds to a character on the screen.  Note that the chr field
 * in the declaration is 8 bits.  This assumes that software attribute
 * bit 2 is not being used.
 */

/* attributes of character:
 * 
 * bit 7 - software attribute 1
 * bit 6 - don't display character
 * bit 5 - blink character
 * bit 4 - underline character
 * bit 3 - inverse video
 * bit 2 - color or intensity bit 1
 * bit 1 - color or intensity bit 2
 * bit 0 - color or intensity bit 3
 */

typedef struct
{
    char	attr;		/* attributes of character */
    char	chr;		/* 7 - bit ascii character	     */
} chr_map;

/*	attribute and character RAM area 
	maps the VME/10 screen for regular character I/O		     */

//#define SCRN_ADDR	((short (*)[80])0xf17000) /*address of character RAM*/



#define CURHIGH		14
#define CURLOW		15


/*==== global variables ===================================================*/

/* If cons_stat gets a valid read from the driver,
 * the character is stored here, and char_avail is set true.
 */

KBCHAR	_char_save;     /* peek ahead buffer */

WORD	_char_avail;   /* true, if char_save contains a character */



/*==== static variables ===================================================*/

const char		scrver[] = "screen driver 0.1";

const char	tabs[MAXCOLS+1]=     /* tab positions */
{
     8,  8,  8,  8,  8,  8,  8,  8, 16, 16, 16, 16, /*  0-11 */
    16, 16, 16, 16, 24, 24, 24, 24, 24, 24, 24, 24, /* 12-23 */
    32, 32, 32, 32, 32, 32, 32, 32, 40, 40, 40, 40, /* 24-35 */
    40, 40, 40, 40, 48, 48, 48, 48, 48, 48, 48, 48, /* 36-47 */
    56, 56, 56, 56, 56, 56, 56, 56, 64, 64, 64, 64, /* 48-59 */
    64, 64, 64, 64, 72, 72, 72, 72, 72, 72, 72, 72, /* 60-71 */
    79, 79, 79, 79, 79, 79, 79, 79                  /* 72-79 */
};			

static short scrn_ram[MAXLINES+1][MAXCOLS+1];   /* character screen 80x32 */

static int	state;		/* current state of screen driver    */
static lscb	scrn_tbl;	/* Logical Screen Control Table	     */

static short	mcurslin;	/* move cursor - line position */
static short	char_mask;      /*attribute & character mask  */





/*==== con_init - initialize console ======================================*/

VOID con_init()
{
    int pos;            /* one possible position of a tabstop */

    _char_save = 0 ;            /* peek ahead buffer */
    _char_avail = FALSE ;       /* has a character been typed? */


    /* fill Logical Screen Control Table */
    scrn_tbl.cur_line=0;        /* start at first line */
    scrn_tbl.cur_col=0;         /* current column */
    scrn_tbl.cur_cursor=0;      /* current cursor */
    scrn_tbl.base_line=0;
    scrn_tbl.base_col=0;
    scrn_tbl.max_line=MAXLINES; /* max line relative to base */
    scrn_tbl.max_col=MAXCOLS;   /* max column relative to base*/

    /* copy tabstops - this must be done for ROMing */
    for (pos=0; pos<=MAXCOLS+1; pos++)  /* fill in constant tabstops */
        scrn_tbl.tabstop[pos]=tabs[pos];

    char_mask = INITATTR;       /*attribute & character mask  */
    cputs("[    ] Console has been initialized ...\r");
    cstatus(SUCCESS);
}



/**
 * cons_stat - get console input status
 *
 * See if we have a character saved from the last call to cons_stat. If we
 * do, return HAVE_CHAR.  If not, try and read a character from the
 * driver. If we get one, save it and return (HAVE_CHAR). Otherwise return 0.
 *
 *  returns:
 *	-1:	if character available	(HAVE_CHAR)
 *	 0:	if not
 */

LONG cons_stat()
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
 * cons_in - wait for and return a character from the console
 *
 * wait for a cons_stat to indicate that there is a character available
 * then fetch it.
 */

LONG cons_in()	
{
    while( ! cons_stat() );		/* wait for input */

    _char_avail = FALSE ;
    return( _char_save );
}



/*==== Position cursor at screen ==========================================*/

VOID pos_cursor(short incr)
{
    if (incr)		/* bump cursor 1 postion or does screen
    table already have correct position? */
    {			/* bump cursor */
        if (scrn_tbl.cur_col != scrn_tbl.max_col) /* end of line? */
            scrn_tbl.cur_col++; /* no, bump cursor position */
    }
}

/* ==== Scroll the screen ================================================= */
/* direction    direction to scroll the screen */
/* num_chrs 	number of characters to scroll */

/* This performs very badly - charblit is slow!!! */

VOID scroll(int direction, int num_chrs)
{
    register int	i, j, line;
    register short	chr_attr;	/*attribute & character */
    register short	*p1, *p2;	/*address of character */

    if (direction == UP)	/*scroll screen text up */
    {
        line = scrn_tbl.base_line; /*used to step down lines */
        i = scrn_tbl.max_line;	/*decremented for each line */
        do {
            j = scrn_tbl.max_col + 1; /*decremented for each char */
            /*address of 1st charactr*/
            p1 = &(scrn_ram[line++][scrn_tbl.base_col]);
            p2 = &(scrn_ram[line][scrn_tbl.base_col]);
            do {		/*move a line at a time	*/
                *p1++ = *p2++;	/*move character up a line */
            } while (--j);
        } while (--i);
    }
    else /*direction == DOWN*/
    {
        line = scrn_tbl.max_line; /*used to step up lines */
        i = scrn_tbl.max_line;	/*decremented for each line */
        do {
            j = scrn_tbl.max_col + 1; /*decremented for each char */
            /*address of 1st charactr*/
            p1 = &(scrn_ram[line--][scrn_tbl.base_col]);
            p2 = &(scrn_ram[line][scrn_tbl.base_col]);
            do {		/*move a line at a time	*/
                *p1++ = *p2++;	/*move character up a line */
            } while (--j);
        } while (--i);
    }

    /* clear the last/first line */
    chr_attr = char_mask | 0x0020;	/* move blanks to screen RAM*/
    /*address of 1st character */
    p1 = &(scrn_ram[line][scrn_tbl.base_col]);
    j = scrn_tbl.max_col + 1;	/* decremented for each char */
    do {			/* write blanks to a line */
        *p1++ = chr_attr;	/* write blank to a char pos */
    } while (--j);

    /* repaint screen */
    i = scrn_tbl.max_line;	/*decremented for each line */
    do {
        j = scrn_tbl.max_col;	/*decremented for each line */
        do {
            charblit(scrn_ram[i][j], j, i);
        } while (j--);
    } while (i--);
}






/* ==== clear to end of line starting at col ===============================*/
VOID clr_line(int line, int col)
{
    register short	chr_attr;	/* attribute & character */
    register int	j;
    register short	*p;		/* address of character */

    /*set up to move blanks to screen RAM*/
    chr_attr = char_mask | 0x0020;

    j = scrn_tbl.max_col + 1 - col; 
    /*address of 1st charactr*/
    p = &(scrn_ram[line][col]);
    do { /*write blanks to line */
        *p++ = chr_attr;	        /*write blank to a char ram pos*/
        charblit(chr_attr, j, line);      /*write blank to screen pos*/
    } while (--j);                      /*decremented for each char */
}



/*==== cons_out - console output ==========================================*/
/******************************************************************************

Function Name:	cons_out

Description:	Main entry point for the VME/10 screen driver.  This
		VME/10 screen driver is an implementation of the VT52.
		It supports the following functions:

		carriage return - cursor is placed at column 0 on the 
			current line.
			How Recognized : <cr> = 0D(hex)

		line feed - cursor is placed at the same column on the next
			line.
			How Recognized : <lf> = 0A(hex)

		backspace - cursor will move 1 column to the left on the 
			screen.  If the cursor is already at column zero then
			it will be placed at the last column of the previous 
			line.  If the cursor is currently on the top line of
			the display at column zero; it will remain where it is.
			How Recognized : <bs> = 08(hex)

		tab - cursor moves to next tab stop on current line.  Cursor
			moves to end of line if there are no more
			tab stop on the line.  Tab stops are set every eight
			columns.
			How Recognized : <tab> = 09(hex)

		clear to end of screen - blanks are written to the cursor
			and all positions on the screen following the
			cursor.  The cursor will remain in its current
			position.
			How Recognized : <esc>J = 1B 4A (hex)

		clear to end of line - blanks are written to the cursor
			and all positions on the line following the cursor.
			The cursor will remain in its current position.
			How Recognized : <esc>K = 1B 4B (hex)

		home cursor - cursor will be placed on the first line at the
			first column on the screen.
			How Recognized : <esc>H = 1B 48 (hex)

		move cursor right - cursor will be moved one character position
			right without affecting the character at that position.
			The cursor will not move past the right margin.
			How Recognized : <esc>C = 1B 43 (hex)

		move cursor up - cursor will be moved one up one line.  If the
			cursor is on the top line then it will stay there.
			How Recognized : <esc>A = 1B 41 (hex)

		reverse line feed - move the cursor up one line.  If the cursor
			was already on the top line the screen will be scrolled
			down one line.  The line at the top of the screen will
			be cleared.  The cursor will remain in the same column.
			How Recognized : <esc>I = 1B 49 (hex)

		position cursor - cursor will be moved to the specified line
			and column.  If either value is beyond the edge of the
			screen then the maximum value for that argument will
			be used.  The line position is sent before the column
			position.  Both values are added to the ASCII space
			character value so that the values received are
			printable characters.
			How Recognized : <esc>Y<line+' '><col+' '> = 1B 59 (hex)

		place character - will place any printable character (i.e.
			20 - 7E (hex)) at the present cursor position on the
			screen.
			How Recognized : any printable character (which is not
					part of a control function sequence).

Inputs:		char - a 7-bit ascii character.

Outputs:	None.

External variables: None

******************************************************************************/


void cons_out(BYTE chr)
{

    chr &= 0x7f;		/* strip the high order bit */


    switch ((int)state)	/* perform action based on the current */
    {			/* state of the screen handler.	*/
    case STATE_ANY_CHAR:	/* waiting to accept any character (i.e. */
                        /* are not processing control sequence */
        switch (chr)
        {
        case ESC:	/* received an <esc> character */
            state = STATE_ESC;
            break;

        case TAB:	/* received a <tab> character */
            scrn_tbl.cur_col = scrn_tbl.tabstop[scrn_tbl.cur_col];
            pos_cursor(FALSE);
            break;

        case CR:	/*received a carriage return	     */
            scrn_tbl.cur_col = 0;
            pos_cursor(FALSE); /*position cursor per scrn_tbl*/
            break;

        case LF:	/*received a line feed		     */
            /*at end of screen?*/
            if (scrn_tbl.cur_line == scrn_tbl.max_line) {
                scroll(UP, MAXCOLS); 	/* scroll up 1 line */
            }
            else
                scrn_tbl.cur_line++;	/* cursor down 1 line */
            pos_cursor(FALSE);	/*position cursor per scrn_tbl*/
            break;

        case BS:	/*received a back space character    */
            /*at beginning of a line? */
			if (scrn_tbl.cur_col == 0)
                        {
                            if (scrn_tbl.cur_line != 0)
                            {	/* not at first line of display */
                                /*place cursor at last column of
                                 previous line */
                                scrn_tbl.cur_line--;
                                scrn_tbl.cur_col = scrn_tbl.max_col;
                            }
                            else
                                break;	/* at first line of display */
			}
                        else	/* not at beginning of a line */
                            scrn_tbl.cur_col--;
                        pos_cursor(FALSE); /*position cursor per scrn_tbl */
                        break;

        default:
            {
                /* character and attributes for RAM */
                register short	chr_attr;

                if (chr >= ' ')	        /* printable character? */
                {
                    /* chr in 16-bit template */
                    chr_attr = char_mask | (short)chr;
                    /*place character on the screen    */
                    (scrn_ram[scrn_tbl.base_line+scrn_tbl.cur_line]
                     [scrn_tbl.base_col+scrn_tbl.cur_col])=chr_attr;
                    charblit(chr, scrn_tbl.cur_col, scrn_tbl.cur_line);
                    pos_cursor(TRUE); /* put cursor at next pos */
                }
            }
        } /* end of switch(chr) */
        break;

    case STATE_ESC:		/* process a control function sequence */
        state = STATE_ANY_CHAR; /* except position cursor is special */
        switch (chr)
        {
		case 'A':	/* cursor up a line */
                    if ( scrn_tbl.cur_line != scrn_tbl.base_line )
                        scrn_tbl.cur_line--;
                    pos_cursor(FALSE);
                    break;

        case 'C':	/* nondestructive space */
            pos_cursor(TRUE);
			break;

        case 'H':	/* home cursor */
            scrn_tbl.cur_col  = scrn_tbl.base_col;
            scrn_tbl.cur_line = scrn_tbl.base_line;
            pos_cursor(FALSE);	/* position the cursor */
            break;

        case 'I':	/* reverse line feed */
            if ( scrn_tbl.cur_line != scrn_tbl.base_line )
                scrn_tbl.cur_line--;
            else
                scroll(DOWN, MAXCOLS);
            pos_cursor(FALSE);
            break;

        case 'J':	/* clear display from cursor */
            {
                int line;

                line = scrn_tbl.cur_line;
                clr_line(line++,scrn_tbl.cur_col);
                while ( line <= scrn_tbl.max_line )
                    clr_line(line++,scrn_tbl.base_col);
            }
            break;

        case 'K':	/* clear line from cursor */
            clr_line(scrn_tbl.cur_line,scrn_tbl.cur_col);
            break;

        case 'Y':	/* position cursor */
            state = STATE_CURS_LINE;	/* next comes cursor line */
            break;
		}
        break;

    case STATE_CURS_LINE:
        mcurslin = chr - ' ';		/* save new line position */
		if ( mcurslin > MAXLINES ) mcurslin = MAXLINES; /* limit it */
                state = STATE_CURS_COL;		/* next comes cursor column */
                break;

    case STATE_CURS_COL:
        scrn_tbl.cur_line = mcurslin;	/* update cursor position */
		chr = chr - ' ';
                if ( chr > MAXCOLS ) chr = MAXCOLS; /* limit position */
                scrn_tbl.cur_col = chr;
                pos_cursor(FALSE);		/* move it */
                state = STATE_ANY_CHAR;
                break;

    } /* end of switch(state) */
}



/*==== printstr - print string to console =================================*/
VOID printstr(REG BYTE *s)     /* used by bioserr */
{
    while (*s)
        cons_out(*s++);
}



/*==== error procedure for BIOS ===========================================*/
VOID bioserr(REG BYTE *errmsg)
{
    printstr("\n\rBIOS ERROR -- ");
    printstr(errmsg);
    printstr(".\n\r");
}
