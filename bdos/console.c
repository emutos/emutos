/*
 * console.c - GEMDOS console system
 *
 * Copyright (C) 2001 Lineo, Inc.
 * Copyright (C) 2016 The EmuTOS development team
 *
 * Authors:
 *  JSL   Jason S. Loveman
 *  SCC   Steve C. Cavender
 *  EWF   Eric W. Fleischman
 *  KTB   Karl T. Braun (kral)
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "fs.h"
#include "proc.h"
#include "console.h"
#include "biosbind.h"


/*
 * The following structure is used for the typeahead buffer
 */
typedef struct {
    WORD add;                   /* index of add position */
    WORD remove;                /* index of remove position */
    WORD glbcolumn;             /* current screen column (zero-based) */
    LONG glbkbchar[KBBUFSZ];    /* the actual typeahead buffer */
} TYPEAHEAD;


/*
 * the actual typeahead buffers
 *
 * [0] is used for prn, [1] for aux, and [2] for con
 */
static TYPEAHEAD buffer[3];


/*
 * default standard handles
 */
static const BYTE default_handle[NUMSTD] =
{
    H_Console,      /* stdin  = con: */
    H_Console,      /* stdout = con: */
    H_Aux,          /* stdaux = aux: */
    H_Print,        /* stdprn = prn: */
    H_Console,      /* Atari TOS redirects undefined ... */
    H_Console       /* ... handles 4, 5 to con:          */
};


/*
 * forward declarations (internal prototypes)
 */
static void buflush(TYPEAHEAD *bufptr);
static long constat(int h);
static void conbrk(int h);
static void conout(int h, int ch);
static void cookdout(int h, int ch);
static long getch(int h);
static void prt_line(int h, char *p);
static void newline(int h, int startcol);
static int backsp(int h, char *cbuf, int retlen, int col);


/*
 * miscellaneous local defines
 */
#define ctrlc   0x03
#define ctrlq   0x11
#define ctrlr   0x12
#define ctrls   0x13
#define ctrlu   0x15
#define ctrlx   0x18

#define cr      0x0d
#define lf      0x0a
#define tab     0x09
#define rub     0x7f
#define bs      0x08

#define terminate() xterm(-32)


/*
 * set up system initial standard handles 
 */
void stdhdl_init(void)
{
    TYPEAHEAD *bufptr;
    WORD i;

    for (i = 0; i < NUMSTD; i++)
        run->p_uft[i] = default_handle[i];

    /* initialise typeahead buffer values */
    for (i = 0, bufptr = buffer; i < 3; i++, bufptr++)
        bufptr->add = bufptr->remove = 0;
}


/*
 * return default handle for given standard handle
 */
BYTE get_default_handle(int stdh)
{
    return default_handle[stdh];
}


/*
 * constat - console status
 *
 * @h - device handle
 */
static long constat(int h)
{
    TYPEAHEAD *bufptr;

    if (h > 2)
        return 0;

    bufptr = &buffer[h];

    return (bufptr->add > bufptr->remove) ? -1L : Bconstat(h);
}


/*
 * xconstat - Function 0x0B - Console input status
 */
long xconstat(void)
{
    return constat(HXFORM(run->p_uft[0]));
}


/*
 * xconostat - Function 0x10 - console output status
 */
long xconostat(void)
{
    return Bcostat(HXFORM(run->p_uft[1]));
}


/*
 * xprtostat - Function 0x11 - Printer output status
 */
long xprtostat(void)
{
    return Bcostat(HXFORM(run->p_uft[3]));
}


/*
 * xauxistat - Function 0x12 - Auxiliary input status
 */
long xauxistat(void)
{
    return constat(HXFORM(run->p_uft[2]));
}


/*
 * xauxostat - Function 0x13 - Auxiliary output status
 */
long xauxostat(void)
{
    return Bcostat(HXFORM(run->p_uft[2]));
}


/*
 * conbrk - check for ctrl/s, used internally
 *
 * @h - device handle
 */
static void conbrk(int h)
{
    TYPEAHEAD *bufptr;
    long ch;
    int stop, c;

    stop = 0;
    if (Bconstat(h))
    {
        bufptr = &buffer[h];
        do
        {
            c = (ch = Bconin(h)) & 0xFF;
            if (c == ctrlc)
            {
                /* comments for the following used to say: "flush BDOS
                 * & BIOS buffers", but that wasn't (& isn't) true */
                buflush(bufptr);    /* flush BDOS buffer */
                terminate();
            }

            if (c == ctrls)
                stop = 1;
            else if (c == ctrlq)
                stop = 0;
            else if (c == ctrlx)
            {
                buflush(bufptr);
                bufptr->glbkbchar[bufptr->add++ & KBBUFMASK] = ch;
            }
            else
            {
                if (bufptr->add < bufptr->remove + KBBUFSZ)
                    bufptr->glbkbchar[bufptr->add++ & KBBUFMASK] = ch;
                else
                    Bconout(h, 7);
            }
        } while (stop);
    }
}


/*
 * buflush - flush BDOS type-ahead buffer
 *
 * bufptr - pointer to TYPEAHEAD structure
 */
static void buflush(TYPEAHEAD *bufptr)
{
    bufptr->add = bufptr->remove = 0;
}


/*
 * conout - console output - used internally
 *
 * @h - device handle
 */
static void conout(int h, int ch)
{
    TYPEAHEAD *bufptr = &buffer[h];

    conbrk(h);                  /* check for control-s break */
    Bconout(h,ch);              /* output character to console */
    if (ch >= ' ')
        bufptr->glbcolumn++;    /* keep track of screen column */
    else if (ch == cr)
        bufptr->glbcolumn = 0;
    else if (ch == bs)
        bufptr->glbcolumn--;
}


/*
 * xtabout - Function 0x02 - console output with tab expansion
 */
long xtabout(int ch)
{
    tabout(HXFORM(run->p_uft[1]),ch);
    return 1;
}


/*
 * tabout -
 *
 * @h - device handle
 * @ch - character to output to console
 */
void tabout(int h, int ch)
{
    TYPEAHEAD *bufptr;

    if (ch == tab)
    {
        bufptr = &buffer[h];
        do
        {
            conout(h,' ');
        } while (bufptr->glbcolumn & 7);
    }
    else
        conout(h,ch);
}


/*
 * cookdout - console output with tab and control character expansion
 *
 * @h - device handle
 * @ch - character to output to console
 */
static void cookdout(int h, int ch)
{
    if (ch == tab)
        tabout(h,ch);                       /* if tab, expand it   */
    else
    {
        if (ch < ' ')
        {
            conout(h,'^');                  /* handle control character */
            ch |= 0x40;
        }
        conout(h,ch);                       /* output the character */
    }
}


/*
 * xauxout - Function 0x04 - auxiliary output
 */
long xauxout(int ch)
{
    return Bconout(HXFORM(run->p_uft[2]), ch);
}


/*
 * xprtout - Function 0x05 - printer output
 */
long xprtout(int ch)
{
    return Bconout(HXFORM(run->p_uft[3]), ch);
}


/*
 * getch - get character from device h
 *
 * @h - device handle
 */
static long getch(int h)
{
    TYPEAHEAD *bufptr = &buffer[h];
    long temp;

    if (bufptr->add > bufptr->remove)
    {
        temp = bufptr->glbkbchar[bufptr->remove++ & KBBUFMASK];
        if (bufptr->add == bufptr->remove)
            buflush(bufptr);
        return temp;
    }

    return Bconin(h);
}


/*
 * x7in - Function 0x07 - Direct console input without echo
 */
long x7in(void)
{
    return getch(HXFORM(run->p_uft[0]));
}


/*
 * conin - BDOS console input function
 */
long conin(int h)
{
    long ch;

    conout(h,(unsigned char)(ch = getch(h)));
    return ch;
}


/*
 * xconin - Function 0x01 - console input
 */
long xconin(void)
{
    int h;

    h = HXFORM(run->p_uft[0]);
    conbrk(h);
    return conin(h);
}


/*
 * x8in - Function 0x08 - Console input without echo
 */
long x8in(void)
{
    int h;
    long ch;

    h = HXFORM(run->p_uft[0]);
    conbrk(h);
    ch = getch(h);
    if ((ch & 0xFF) == ctrlc)
        terminate();

    return ch;
}


/*
 * xauxin - Function 0x03 - Auxiliary input
 */
long xauxin(void)
{
    return Bconin(HXFORM(run->p_uft[2]));
}


/*
 * rawconio - Function 0x06 - Raw console I/O
 */
long rawconio(int parm)
{
    int i;

    if (parm == 0xFF)
    {
        i = HXFORM(run->p_uft[0]);
        return constat(i) ? getch(i) : 0L;
    }

    Bconout(HXFORM(run->p_uft[1]), parm);
    return 0; /* dummy */
}


/*
 * xprt_line - Function 0x09 - Print line up to nul with tab expansion
 */
void xprt_line(char *p)
{
    prt_line(HXFORM(run->p_uft[1]),p);
}


/*
 * prt_line - print line to stdout
 */
static void prt_line(int h, char *p)
{
    while(*p)
        tabout(h, (unsigned char)*p++);
}


/*
 * read line with editing and bounds checking
 */

/* Two subroutines first */
static void newline(int h, int startcol)
{
    conout(h,cr);                       /* go to new line */
    conout(h,lf);
    while(startcol)
    {
        conout(h,' ');
        startcol -= 1;          /* start output at starting column */
    }
}


/* backspace one character position */
/* col is the starting console column */
static int backsp(int h, char *cbuf, int retlen, int col)
{
    TYPEAHEAD *bufptr = &buffer[h];
    char ch;                    /* current character */
    int  i;
    char *p;                    /* character pointer */

    if (retlen)                 /* if buffer non-empty, decrease it by 1 */
        --retlen;
    i = retlen;
    p = cbuf;
    while(i--)                  /* calculate column position across entire char buffer */
    {
        ch = *p++;              /* get next char                */
        if (ch == tab)
        {
            col += 8;
            col &= ~7;          /* for tab, go to multiple of 8 */
        }
        else if ((unsigned char)ch < ' ')
            col += 2;           /* control chars put out 2 printable chars */
        else
            col += 1;
    }
    while (bufptr->glbcolumn > col)
    {
        conout(h,bs);           /* backspace until we get to proper column */
        conout(h,' ');
        conout(h,bs);
    }

    return retlen;
}


/*
 * readline - Function 0x0A - Read console string into buffer
 *
 * p - max length, return length, buffer space
 */
void readline(char *p)
{
    p[1] = cgets(HXFORM(run->p_uft[0]),(unsigned char)p[0],&p[2]);
}


/* h is special handle denoting device number */
int cgets(int h, int maxlen, char *buf)
{
    TYPEAHEAD *bufptr = &buffer[h];
    char ch;
    int i, stcol, retlen;

    stcol = bufptr->glbcolumn;      /* set up starting column */
    for (retlen = 0; retlen < maxlen; )
    {
        switch(ch = getch(h))
        {
        case cr:
        case lf:
            conout(h,cr);
            return retlen;
        case bs:
        case rub:
            retlen = backsp(h,buf,retlen,stcol);
            break;
        case ctrlc:
            terminate();
        case ctrlx:
            do
            {
                retlen = backsp(h,buf,retlen,stcol);
            } while(retlen);
            break;
        case ctrlu:
            conout(h,'#');
            newline(h,stcol);
            retlen = 0;
            break;
        case ctrlr:
            conout(h,'#');
            newline(h,stcol);
            for (i = 0; i < retlen; i++)
                cookdout(h,(unsigned char)buf[i]);
            break;
        default:
            cookdout(h,(unsigned char)(buf[retlen++] = ch));
        }
    }
    return retlen;
}
