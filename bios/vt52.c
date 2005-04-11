/*
 * console.c - vt52 like screen handling routines
 *
 *
 * Copyright (c) 2004 by Authors:
 *
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "lineavars.h"
#include "font.h"
#include "tosvars.h"            /* for save_row */
#include "sound.h"              /* for bell() */
#include "string.h"
#include "conout.h"



/*
 * internal prototypes
 */

static void nop();
static void cursor_up();
static void cursor_down();
static void cursor_left();
static void cursor_right();
static void clear_and_home();
static void cursor_home();
static void reverse_linefeed();
static void erase_to_eos();
static void erase_to_eol();
static void insert_line();
static void delete_line();

static void set_fg();
static void set_bg();
static void erase_from_home();
static void cursor_off();
static void cursor_on();
static void cursor_on_cnt();
static void save_cursor_pos();
static void restore_cursor_pos();
static void erase_line();
static void erase_to_eol();
static void erase_from_bol();
static void reverse_video_on();
static void reverse_video_off();
static void line_wrap_on();
static void line_wrap_off();

static void do_bell();
static void do_tab();
static void ascii_lf();
static void ascii_cr();

/* handlers for the console state machine */
static void esc_ch1(WORD);
static void get_row(WORD);
static void get_column(WORD);




void (*con_state)(WORD);        /* state of conout state machine */



BOOL vt52_initialized;  /* checked by kprintf for safety */



/* jumptable for ESC + uppercase character */
void (*am_tab[])() = {
    cursor_up,          /* Cursor Up */
    cursor_down,        /* Cursor Down */
    cursor_right,       /* Cursor Right */
    cursor_left,        /* Cursor Left */
    clear_and_home,     /* Clear and Home */
    nop,                /* <ESC> F not supported */
    nop,                /* <ESC> G not supported */
    cursor_home,        /* Home */
    reverse_linefeed,   /* Reverse Line Feed */
    erase_to_eos,       /* Erase to End of Screen */
    erase_to_eol,       /* Erase to End of Line */
    insert_line,        /* Insert Line */
    delete_line         /* Delete Line */
};


/* jumptable for ESC + lowercase character */
void (*bw_tab[])() = {
    set_fg,             /* Set foreground color (1 more char) */
    set_bg,             /* Set background color (1 more char) */
    erase_from_home,    /* Erase from beginning of page */
    cursor_on,          /* Cursor On */
    cursor_off,         /* Cursor Off */
    nop,                /* <ESC> g not supported */
    nop,                /* <ESC> h not supported */
    nop,                /* <ESC> i not supported */
    save_cursor_pos,    /* Save Cursor Position */
    restore_cursor_pos, /* Restore Cursor position */
    erase_line,         /* Erase line */
    nop,                /* <ESC> m not supported */
    nop,                /* <ESC> n not supported */
    erase_from_bol,     /* Erase from Beginning of Line */
    reverse_video_on,   /* Reverse Video On */
    reverse_video_off,  /* Reverse Video Off */
    nop,                /* <ESC> r not supported */
    nop,                /* <ESC> s not supported */
    nop,                /* <ESC> t not supported */
    nop,                /* <ESC> u not supported */
    line_wrap_on,       /* Wrap at End of Line */
    line_wrap_off       /* No Wrap at End of Line */
};

/* jumptable for ASCII control codes */
void (*cntl_tab[])() = {
    do_bell,            /* 7 = bell */
    cursor_left,        /* 8 = backspace */
    do_tab,             /* 9 = Horizontal tab */
    ascii_lf,           /* 10 = Line feed */
    ascii_lf,           /* 11 = Vertical tab (Treated as line feed) */
    ascii_lf,           /* 12 = Form Feed (Treated as line feed) */
    ascii_cr            /* 13 = Carriage Return */
};



/*
 * cputc - console output
 */

void
cputc(WORD ch)
{
    /* based on our state goto the correct stub routine */
    (*con_state)(ch & 0xff);
}



/*
 * bconout5 - raw console output.
 */

void
bconout5 (WORD ch)
{
    ascii_out(ch);
}


/*
 * normal_ascii - state is normal output
 */

static void
normal_ascii(WORD ch)
{
    /* If the character is printable ascii */
    if ( ch >= 0x20 ) {
        ascii_out(ch);    /* go print it. */
    }


    /* We handle the following control characters as special: */

    /* 7 = bell */
    /* 8 = backspace */
    /* 9 = Horizontal tab */
    /* 10 = Line feed */
    /* 11 = Vertical tab (Treated as line feed) */
    /* 12 = Form Feed (Treated as line feed) */
    /* 13 = Carriage Return */
    /* 27 = Escape (Start Command) */

    /* If escape character alter next state */
    else if ( ch == 0x1b ) {
        /* handle the control characters */
        con_state = esc_ch1;    /* set constate to handle esc codes */
    }

    else if ( ch >= 7 ) {
        /* Range check the character */
        ch -= 7;
        if ( ch <= 6 ) {
            (*cntl_tab[ch])();
        }
    }
    /* All others are thrown away. */
}


static void
nop()
{
    return;
}





/*
 * do_bell - Ring the bell (in sound.c)
 */

static void
do_bell () {
    if (conterm & 4) {
        bell();
    }
}



/*
 * do_tab - calculate the tabulator values
 */

static void
do_tab () {
    move_cursor((v_cur_cx & 0xfff8) + 8, v_cur_cy);
}




/*
 * esc_ch1 - state is: handle first character of an escape sequence
 */

static void
esc_ch1 (WORD ch)
{
    con_state = normal_ascii;   /* Most functions only 2 chars so set */
    ch -= 0x41;                 /* state to normal ascii.  Bias by low */

    if ( ch < 0 )
        return;                 /* char and get out if invalid */

    /* If in the range A-M go handle */
    if ( ch <= 12 ) {
        (*am_tab[ch])();
    }

    /* check for lower case control code */
    else if ( ch != 24 ) {
        ch -= 0x21;       /* see if b to w */
        if ( ch >= 0 && ch <= 21 )
            (*bw_tab[ch])();
    }
    else
        /* direct cursor addressing, take 2 additional chars */
        con_state = get_row;
}


/*
 * get_row - state is: calculate row from character
 */

static void
get_row (WORD ch)
{
    save_row = ch - 0x20;       /* Remove space bias */
    con_state = get_column;
}


/*
 * get_row - state is: calculate column from character
 */

static void
get_column (WORD ch)
{
    int row, col;

    col = ch - 0x20;                    /* Remove space bias */
    row = save_row;
    move_cursor(col,row);
    con_state = normal_ascii;           /* Next char is not special */
}


/*
 * get_fg_col - state is: get foreground color
 */

static void
get_fg_col (WORD ch)
{
    /* set the foreground color, low-order bits only. */
    v_col_fg = (ch - 0x20) & 0x0f;
    con_state = normal_ascii;           /* Next char is not special */
}



/*
 * get_bg_col - state is: get background color
 */

static void
get_bg_col (WORD ch)
{
    /* set the foreground color, low-order bits only. */
    v_col_bg = (ch - 0x20) & 0x0f;
    con_state = normal_ascii;           /* Next char is not special */
}



static void
set_fg ()
{
    con_state = get_fg_col;             /* Next char is the FG color */
}


static void
set_bg ()
{
    con_state = get_bg_col;             /* Next char is the BG color */
}






/*
 * clear_and_home - Clear Screen and Home Cursor.
 */

static void
clear_and_home ()
{
    move_cursor(0, 0);                          /* cursor home */
    blank_out (0, 0, v_cel_mx, v_cel_my);        /* clear screen. */
}



/*
 * cursor_up - Alpha Cursor Up
 */

static void
cursor_up ()
{
    if ( v_cur_cy )
        move_cursor(v_cur_cx, v_cur_cy - 1);
}




/*
 * cursor_down - Alpha Cursor Down
 */

static void
cursor_down ()
{
    if ( v_cur_cy != v_cel_my)
        move_cursor(v_cur_cx, v_cur_cy + 1);
}



/*
 * cursor_right - Alpha Cursor Right
 */

static void
cursor_right ()
{
    if ( v_cur_cx != v_cel_mx)
        move_cursor(v_cur_cx + 1, v_cur_cy);   
}



/*
 * cursor_left - Alpha Cursor Left
 */

static void
cursor_left ()
{
    if ( v_cur_cx )
        move_cursor(v_cur_cx - 1, v_cur_cy);
}



/*
 * cursor_home - Home Alpha Cursor
 */

static void
cursor_home ()
{
    move_cursor(0, 0);
}



/*
 * erase_to_eos - Erase to End of Screen
 */

static void
erase_to_eos ()
{
    erase_to_eol();    /* erase to end of line. */

    /* last line? */
    if ( v_cur_cy == v_cel_my )
        return;    /* yes, done. */

    /* erase from upper left corner to lower right corner. */
    blank_out (0, v_cur_cy + 1, v_cel_mx, v_cel_my);
}



/*
 * erase_line - Erase to End of Line.
 */

static void
erase_to_eol ()
{
    BOOL wrap = v_stat_0 & M_CEOL;      /* save line wrap status */
    v_stat_0 &= ~M_CEOL;    /* clear EOL handling bit. (overwrite) */

    cursor_off();               /* hide cursor. */
    save_cursor_pos();          /* save cursor position. */

    /* is x = x maximum? */
    if ( v_cur_cx == v_cel_mx )
        ascii_out(0x20);        /* output a space, the cell is odd!. */
    else {
        /* test, if x is even or odd */
        if ( v_cur_cx & 0x1 )
            ascii_out(0x20);    /* first output a space. */

        blank_out (v_cur_cx, v_cur_cy, v_cel_mx, v_cur_cy);
    }

    /* restore wrap flag, the result of EOL test. */
    if ( wrap )
        v_stat_0 |= M_CEOL;

    restore_cursor_pos();       /* restore cursor position. */
    cursor_on_cnt();            /* show cursor. */
}


/*
 * reverse_video_on - Reverse Video On
 */

static void
reverse_video_on ()
{
    v_stat_0 |= M_REVID;    /* set the reverse bit. */
}





/*
 * reverse_video_off - Reverse Video Off.
 */

static void
reverse_video_off ()
{
    v_stat_0 &= ~M_REVID;    /* clear the reverse bit. */
}



/*
 * reverse_linefeed - Reverse Index
 */
static void
reverse_linefeed ()
{
    /* if not at top of screen */
    if ( v_cur_cy ) {
        move_cursor(v_cur_cx, v_cur_cy - 1);
    }
    else {
        int savex = v_cur_cx;           /* save current x position */
        insert_line();                  /* Insert a line */
        move_cursor(savex, 0);
    }
}



/*
 * insert_line - Insert Line.
 */

static void
insert_line ()
{
    cursor_off();               /* hide cursor. */
    scroll_down(v_cur_cy);      /* scroll down 1 line & blank current line. */
    move_cursor(0, v_cur_cy);   /* move cursor to beginning of line. */
    cursor_on_cnt();            /* show cursor. */
}



/*
 * delete_line - Delete Line.
 */

static void
delete_line ()
{
    cursor_off();               /* hide cursor. */
    scroll_up(v_cur_cy);        /* scroll up 1 line & blank bottom line. */
    move_cursor(0, v_cur_cy);   /* move cursor to beginning of line. */
    cursor_on_cnt();            /* show cursor. */
}



/*
 * erase_from_home - Erase from Beginning of Page to cursor.
 */

static void
erase_from_home ()
{
    erase_from_bol();    /* erase from beginning of line. */

    /* first line? */
    if ( !v_cur_cy )
        return;    /* yes, done. */

    /* erase rest of screen. */
    blank_out (0, 0, v_cel_mx, v_cur_cy - 1);        /* clear screen. */
}



/*
 * do_cnt_esce - Enable Cursor.
 */

static void
do_cnt_esce ()
{
    /* see if flashing is enabled. */
    if ( v_stat_0 & M_CFLASH ) {
        invert_cell(v_cur_cx, v_cur_cy);        /* complement cursor. */
        v_stat_0 |= M_CSTATE;                   /* set cursor on. */
        v_stat_0 |= M_CVIS;                     /* set visibility bit. */

    }
    else {
        v_stat_0 |= M_CVIS;                     /* set visibility bit. */
        invert_cell(v_cur_cx, v_cur_cy);        /* complement cursor. */
    }
}



/*
 * cursor_on - Enable Cursor forced.
 */

static void
cursor_on()
{
    /* if disable count is zero (cursor still shown) then return */
    if ( !disab_cnt )
        return;

    disab_cnt = 0;                      /* reset the disable counter. */
    do_cnt_esce();
}



/*
 * cursor_on_cnt - Enable Cursor (counted depth).
 */

static void
cursor_on_cnt()
{
    /* if disable count is zero (cursor still shown) then return */
    if ( !disab_cnt )
        return;

    disab_cnt--;                        /* decrement the disable counter. */
    if (!disab_cnt)
        do_cnt_esce();                  /* if 0, do the enable. */
}



/*
 * cursor_off - Disable Cursor.
 */

static void
cursor_off ()
{
    disab_cnt++;                        /* increment the disable counter */

    /* test and clear the visible state bit. */
    if (!(v_stat_0 & M_CVIS) )
        return;                         /* if already invisible, just return */

    v_stat_0 &= ~M_CVIS;                /* make invisible! */

    /* see, if flashing is disabled */
    if ( ! (v_stat_0 & M_CFLASH) ) {
        invert_cell(v_cur_cx, v_cur_cy);
    }
    /* see, if cursor is on or off */
    else if ( v_stat_0 & M_CSTATE ) {
        v_stat_0 &= ~M_CSTATE;    /* cursor off? */
        invert_cell(v_cur_cx, v_cur_cy);
    }
}



/*
 * save_cursor_pos - Save Cursor Position.
 */

static void
save_cursor_pos ()
{
    v_stat_0 |= M_SVPOS;    /* set "position saved" status bit. */

    /* save the x and y coords of cursor. */
    sav_cur_x = v_cur_cx;
    sav_cur_y = v_cur_cy;
}



/*
 * restore_cursor_pos - Restore Cursor Position.
 */

static void
restore_cursor_pos ()
{
    if ( v_stat_0 & M_SVPOS )
        move_cursor(sav_cur_x, sav_cur_y);      /* move to saved position. */
    else
        move_cursor(0, 0);      /* if position was not saved, home cursor. */

    v_stat_0 &= ~M_SVPOS;    /* clear "position saved" status bit. */
}



/*
 * erase_line - Erase Entire Line.
 *
 * upper left coords. (0,y), lower right coords. (max,y)
 */

static void
erase_line ()
{
    cursor_off();               /* hide cursor. */
    blank_out (0, v_cur_cy, v_cel_mx, v_cur_cy);   /* blank whole line. */
    move_cursor(0, v_cur_cy);   /* move cursor to beginning of line. */
    cursor_on_cnt();            /* show cursor. */
}



/*
 * erase_from_bol - Erase from Beginning of Line.
 *
 * upper left coords. (0,y)
 * lower right coords. (x,y)
 */

static void
erase_from_bol ()
{
    cursor_off();               /* hide cursor. */
    save_cursor_pos();          /* save cursor position. */

    /* are we in column 0?*/
    if ( v_cur_cx == 0 )
        ascii_out(0x20);        /* output a space. */
    else {
        /* test, if x is even or odd */
        if ( v_cur_cx & 1 ) {
            ascii_out(0x20);    /* first output a space. */
            blank_out (0, v_cur_cy, v_cur_cx - 2, v_cur_cy);
        }
        else
            blank_out (0, v_cur_cy, v_cur_cx, v_cur_cy);
    }

    restore_cursor_pos();       /* restore cursor position. */
    cursor_on_cnt();            /* show cursor. */
}



/*
 * line_wrap_on() - Wrap at End of Line.
 */
static void
line_wrap_on()
{
    v_stat_0 |= M_CEOL;    /* set the eol handling bit. */
}



/*
 * line_wrap_off - Discard at End of Line.
 */
static void
line_wrap_off()
{
    v_stat_0 &= ~M_CEOL;    /* clear the eol handling bit. */
}



/*
 * ascii_cr - carriage return.
 */
static void
ascii_cr ()
{
    /* beginning of current line. */
    move_cursor(0, v_cur_cy);
}



/*
 * ascii_lf - line feed.
 */
static void
ascii_lf ()
{
    /* at bottom of screen? */
    if ( v_cur_cy != v_cel_my )
        cursor_down();
    else {
        cursor_off();                   /* yes, hide cursor. */
        scroll_up(0);                   /* scroll up 1 line */
        cursor_on_cnt();                /* show cursor. */
    }
}



/*
 * blink - cursor blink interrupt routine.
 *
 * This routine may trash registers, when called from assembler!
 */

void
blink ()
{
    /* test visibility/semaphore bit. */
    if (!(v_stat_0 & M_CVIS) )
        return;    /* if invisible or blocked, return. */

    /* test flash bit. */
    if (!(v_stat_0 & M_CFLASH) )
        return;    /* if not flashing, return. */

    /* decrement cursor flash timer. */
    if ( --v_cur_tim )
        return;    /* if <> 0, return. */

    v_cur_tim = v_period;       /* else reset timer. */

    /* toggle cursor state. */
    if ( v_stat_0 & M_CSTATE )
        v_stat_0 &= ~M_CSTATE;    /* clear bit. (overwrite) */
    else
        v_stat_0 |= M_CSTATE;    /* set bit. (overwrite) */

    /* fetch x and y coords and complement cursor. */
    invert_cell(v_cur_cx, v_cur_cy);
}



/*
 * cursconf - cursor configuration
 *
 * Arguments:
 *
 *   function =
 *   0 - switch off cursor
 *   1 - switch on cursor
 *   2 - blinking cursor
 *   3 - not blinking cursor
 *   4 - set cursor blink rate
 *   5 - get cursor blink rate
 *
 * Bits:
 *   M_CFLASH - cursor flash on
 *   M_CVIS   - cursor visibility on
 */

WORD
cursconf(WORD function, WORD operand)
{
    switch (function) {
    case 0:
        cursor_off();                   /* set cursor unvisible */
        break;
    case 1:
        cursor_on();                    /* set cursor visible */
        break;
    case 2:
        v_stat_0 &= ~M_CFLASH;          /* unset cursor flash bit */
        break;
    case 3:
        v_stat_0 |= M_CFLASH;           /* set cursor flash bit */
        break;
    case 4:
        v_period = operand;             /* set cursor flash interval */
        break;
    case 5:
        return(v_period);               /* set cursor flash interval */
    }
    return 0;
}



/*
 * con_state_init - initialize the conout state machine
 */

void
vt52_init()
{
    /* Initial cursor settings */
    v_cur_cx = 0;                       // cursor to column 0
    v_cur_cy = 0;                       // cursor to line 0
    v_cur_of = 0;                       // line offset is 0
    v_cur_ad = v_bas_ad;                // set cursor to begin of screen

    v_stat_0 = M_CFLASH;                // cursor invisible, flash,
                                        // nowrap, normal video.
    cursconf(4, 30);                    // .5 second blink rate (@60 Hz vblank).
    v_cur_tim = v_period;               // load initial value to blink timer
    disab_cnt = 1;                      // cursor disabled 1 level deep.

    /* set foreground color depending on color depth */
    switch (v_planes) {
    case 1:
        v_col_fg = 1;
        break;
    case 2:
        v_col_fg = 3;
        break;
    default:
        v_col_fg = 15;
    }
    v_col_bg = 0;

    con_state = normal_ascii;           /* Init conout state machine */

    vt52_initialized = TRUE;
}

