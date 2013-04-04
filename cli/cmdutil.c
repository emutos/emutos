/*
 * EmuCON2 utility routines
 *
 * Copyright (c) 2013 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "string.h"
#include "cmd.h"

typedef struct {
    long cookie;
    long value;
} COOKIE;


/*
 *  output a 2-byte escape sequence to console
 */
void escape(char c)
{
    conout(ESC);
    conout(c);
}

/*
 *  output a message to the console
 */
void message(char *msg)
{
char *p;

    for (p = msg; *p; p++) {
        if (*p == '\n')
            conout('\r');
        conout(*p);
    }
}

/*
 *  output a message terminated by a newline to the console
 */
void messagenl(char *msg)
{
    message(msg);
    message("\n");
}

/*
 *  output an error message to the console
 */
void errmsg(LONG rc)
{
char buf[20], *p;

    switch(rc) {
    case 0:                 /* no errors */
        return;
        break;
    case EFILNF:
        p = _("file not found");
        break;
    case EPTHNF:
        p = _("path not found");
        break;
    case ENHNDL:
        p = _("no more file handles");
        break;
    case EACCDN:
        p = _("can't create file");
        break;
    case ENSMEM:
        p = _("not enough memory");
        break;
    case EDRIVE:
        p = _("invalid drive");
        break;
    case USER_BREAK:
        p = _("interrupted");
        break;
    case INVALID_PATH:
        p = _("invalid path");
        break;
    case DISK_FULL:
        p = _("can't write to disk (full?)");
        break;
    case CMDLINE_LENGTH:
        p = _("cmdline too long");
        break;
    default:
        message(_("error code "));
        if (rc < 0) {
            conout('-');
            rc = -rc;
        }
        convulong(buf,rc,10,' ');
        for (p = buf; *p == ' '; p++)
            ;
        break;
    }
    messagenl(p);
}

/*
 *  output a redirectable string
 */
void output(char *s)
{
    if (redir_handle < 0L)
        message(s);
    else Fwrite((WORD)redir_handle,strlen(s),s);
}

/*
 *  output a redirectable string with a newline
 */
void outputnl(char *s)
{
    output(s);
    output("\n");
}

/*
 *  handle control-C
 */
PRIVATE WORD user_break(void)
{
char c;

    c = conin() & 0xff;
    if (c == CTL_C)         /* user wants to interrupt */
        return -1;

    return 0;
}

/*
 *  check for flow control or control-C
 */
PRIVATE WORD user_input(void)
{
char c;

    c = conin() & 0xff;
    if (c == CTL_C)         /* user wants to interrupt */
        return -1;

    if (c == CTL_S) {       /* user wants to pause */
        while(1) {
            c = conin() & 0xff;
            if (c == CTL_C)
                return -1;
            if (c == CTL_Q)
                break;
        }
    }

    return 0;
}

/*
 *  output a redirectable fixed-length buffer
 */
LONG outputbuf(char *s,LONG len)
{
LONG n, rc;

    if (redir_handle < 0L) {
        n = len;
        while(n-- > 0) {
            if (constat())
                if (user_input())
                    return USER_BREAK;
            conout(*s++);
        }
        return len;
    }

    rc = Fwrite((WORD)redir_handle,len,s);
    if (rc >= 0) {
        if (constat())
            if (user_break())
                return USER_BREAK;
    }

    return rc;
}

/*
 *  convert a string to lowercase
 */
char *strlower(char *str)
{
char *p;

    for (p = str; *p; p++)
        if ((*p >= 'A') && (*p <= 'Z'))
            *p |= 0x20;

    return str;
}

/*
 *  convert a string to uppercase
 */
char *strupper(char *str)
{
char *p;

    for (p = str; *p; p++)
        if ((*p >= 'a') && (*p <= 'z'))
            *p &= ~0x20;

    return str;
}

/*
 *  convert an unsigned long to a string
 */
void convulong(char *buf,ULONG n,WORD width,char filler)
{
WORD i;
ULONG quot;
char *p;

    buf[width] = '\0';
    for (i = 0, p = buf+width-1; (i < width) && n; i++, p--, n = quot) {
        quot = n / 10;
        *p = (n - quot*10) + '0';
    }

    for ( ; i < width; i++, p--)
        *p = filler;
}

PRIVATE char *conv2(char *p,WORD n)
{
WORD tens;

    tens = n / 10;
    *p++ = '0' + tens;
    *p++ = '0' + (n - tens * 10);

    return p;
}

PRIVATE char *conv4(char *p,WORD n)
{
WORD hundreds;

    hundreds = n / 100;
    p = conv2(p,hundreds);
    p = conv2(p,n-hundreds*100);

    return p;
}

/*
 *  decode_date_time - generate string with date/time in format derived from _IDT cookie
 *
 *  returns length of formatted string
 */
WORD decode_date_time(char *s,UWORD date,UWORD time)
{
WORD year, month, day, hour, minute, second;
char *p = s;
char date_sep, ampm;

    date_sep = idt_value & 0xff;            /* date separator */
    if ((date_sep < 0x20) || (date_sep > 0x7f))
        date_sep = DEFAULT_DT_SEPARATOR;    /* default if all else fails */

    year = 1980 + (date>>9);
    month = (date>>5) & 0x0f;
    day = date & 0x1f;

    switch((idt_value>>8)&0x03) {
    case _IDT_MDY:
        p = conv2(p,month);
        *p++ = date_sep;
        p = conv2(p,day);
        *p++ = date_sep;
        p = conv4(p,year);
        break;
    case _IDT_DMY:
        p = conv2(p,day);
        *p++ = date_sep;
        p = conv2(p,month);
        *p++ = date_sep;
        p = conv4(p,year);
        break;
    case _IDT_YDM:
        p = conv4(p,year);
        *p++ = date_sep;
        p = conv2(p,day);
        *p++ = date_sep;
        p = conv2(p,month);
        break;
    default:                        /* i.e. _IDT_YMD or some kind of bug ... */
        p = conv4(p,year);
        *p++ = date_sep;
        p = conv2(p,month);
        *p++ = date_sep;
        p = conv2(p,day);
        break;
    }
    *p++ = ' ';
    *p++ = ' ';

    hour = time >> 11;
    minute = (time>>5) & 0x3f;
    second = (time&0x1f) << 1;

    switch((idt_value>>12)&0x01) {
    case _IDT_12H:
        if (hour < 12)              /* figure out am/pm */
            ampm = 'a';
        else ampm = 'p';
        if (hour > 12)              /* figure out noon/midnight */
            hour -= 12;
        else if (hour == 0)
            hour = 12;
        p = conv2(p,hour);
        *p++ = ':';
        p = conv2(p,minute);
        *p++ = ':';
        p = conv2(p,second);
        *p++ = ampm;
        *p++ = 'm';
        break;
    default:                        /* i.e. _IDT_24H or some kind of bug ... */
        p = conv2(p,hour);
        *p++ = ':';
        p = conv2(p,minute);
        *p++ = ':';
        p = conv2(p,second);
        *p++ = ' ';
        *p++ = ' ';
        break;
    }
    *p = '\0';

    return p - s;
}

/*
 *  copies next path component from buffer &
 *  updates buffer pointer
 *
 *  returns:
 *      1   arg is normal
 *      0   no more args
 */
WORD get_path_component(char **pp,char *dest)
{
char *p, *q = dest;

    /*
     *  look for start of next component
     */
    for (p = *pp; *p; p++)
        if (*p != ';')
            break;
    if (!*p) {          /* end of buffer */
        *pp = p;
        return 0;
    }

    while(*p) {
        if (*p == ';')
            break;
        *q++ = *p++;
    }
    *q = '\0';

    *pp = p;
    return 1;
}

WORD has_wildcard(char *name)
{
char *p;

    for (p = name; *p; p++)
        if ((*p == '?') || (*p == '*'))
            return 1;

    return 0;
}

/*
 *  compare strings for equality, ignoring case
 *
 *  returns 1 iff strings equal
 */
WORD strequal(char *s1,char *s2)
{
char *p, *q, c;

    for (p = s1, q = s2; *p; p++, q++) {
        if (*p == *q)
            continue;
        c = *p;
        if ((c >= 'A') && (c <= 'Z'))
            c |= 0x20;
        if (c != *q)
            return 0;
    }

    return 1;
}

PRIVATE LONG getjar(void)
{
    return *(LONG *)0x5a0;
}

/*
 *  getcookie()
 */
WORD getcookie(LONG cookie,LONG *pvalue)
{
COOKIE *jar, *c;

    jar = (COOKIE *)Supexec(getjar);
    if (!jar)
        return 0;

    for (c = jar; c->cookie; c++) {
        if (c->cookie == cookie) {
            if (pvalue)
                *pvalue = c->value;
            return 1;
        }
    }

    return 0;
}

#ifdef STANDALONE_CONSOLE
size_t strlen(const char *s)
{
int n;

    for (n = 0; *s; s++, n++)
        ;

    return n;
}

char *strcpy(char *dest,const char *src)
{
char *p = dest;

    for (p = dest; *src; )
        *p++ = *src++;

    return dest;
}
#endif
