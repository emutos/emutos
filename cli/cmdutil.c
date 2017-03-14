/*
 * EmuCON2 utility routines
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "cmd.h"
#include <string.h>

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
void message(const char *msg)
{
const char *p;

    for (p = msg; *p; p++)
        conout(*p);
}

/*
 *  output a message terminated by a newline to the console
 */
void messagenl(const char *msg)
{
    message(msg);
    message("\r\n");
}

/*
 *  output an error message to the console
 */
void errmsg(LONG rc)
{
char buf[20];
const char *p;

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
    case DIR_NOT_EMPTY:
        p = _("directory not empty");
        break;
    case CANT_DELETE:
        p = _("can't delete file (read-only?)");
        break;
    case INVALID_PARAM:
        p = _("invalid parameter");
        break;
    case WRONG_NUM_ARGS:
        p = _("wrong number of arguments");
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
 *  convert a string to a word value
 *
 *  returns -1 iff error in value
 */
WORD getword(char *buf)
{
LONG n = 0L;
WORD m;
char *p;

    for (p = buf; *p; p++) {
        m = *p - '0';
        if ((m < 0) || (m > 9))
            return -1;
        n = n * 10 + m;
        if (n > 32767)
            return -1;
    }
    return (WORD)n;
}

/*
 *  convert an unsigned long to a string
 */
void convulong(char *buf,ULONG n,WORD width,char filler)
{
ULONG quot;
char *p = buf + width;

    *p-- = '\0';
    while(p >= buf) {
        quot = n / 10;
        *p-- = (n - quot*10) + '0';
        n = quot;
        if (!n)
            break;
    }

    while(p >= buf)
        *p-- = filler;
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
char ampm;
unsigned char date_sep;

    date_sep = LOBYTE(idt_value);           /* date separator */
    if ((date_sep < 0x20) || (date_sep > 0x7f))
        date_sep = DEFAULT_DT_SEPARATOR;    /* default if all else fails */

    year = 1980 + (date>>9);
    month = (date>>5) & 0x0f;
    day = date & 0x1f;

    switch(HIBYTE(idt_value)&0x03) {
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
WORD get_path_component(const char **pp,char *dest)
{
const char *p;
char *q = dest;

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

WORD has_wildcard(const char *name)
{
const char *p;

    for (p = name; *p; p++)
        if ((*p == '?') || (*p == '*'))
            return 1;

    return 0;
}

/*
 *  return pointer to file extension iff a program
 */
const char *program_extension(const DTA *dtaptr)
{
const char *p;

    if (dtaptr->d_attrib & 0x10)    /* a folder */
        return NULL;

    for (p = dtaptr->d_fname; *p; ) {
        if (*p++ == '.') {
            if (strequal(p,"app") || strequal(p,"gtp") || strequal(p,"prg")
             || strequal(p,"tos") || strequal(p,"ttp"))
                return p;
        }
    }

    return NULL;
}

/*
 *  compare strings for equality, ignoring case
 *
 *  returns 1 iff strings equal
 */
WORD strequal(const char *s1,const char *s2)
{
const char *p, *q;
char c1, c2;

    for (p = s1, q = s2; *p; ) {
        c1 = *p++;
        if ((c1 >= 'A') && (c1 <= 'Z'))
            c1 |= 0x20;
        c2 = *q++;
        if ((c2 >= 'A') && (c2 <= 'Z'))
            c2 |= 0x20;
        if (c1 != c2)
            return 0;
    }
    if (*q)
        return 0;

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
    *p = '\0';

    return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
unsigned char *d = (unsigned char *)dest;
const unsigned char *s = (const unsigned char *)src;

    while (n--)
        *d++ = *s++;

    return dest;
}

void *memset(void *dest, int value, size_t n)
{
unsigned char *d = (unsigned char *)dest;

    while (n--)
        *d++ = (unsigned char)value;

    return dest;
}

int toupper(int c)
{
    if(c>='a' && c<='z')
        return(c-'a'+'A');
    else
        return(c);
}

int strncasecmp(const char *a, const char *b, size_t n)
{
    unsigned char s1, s2;

    while(n-- > 0) {
        s1 = toupper((unsigned char)*a++);
        s2 = toupper((unsigned char)*b++);
        if (s1 != s2)
            return s1 - s2;
        if (s1 == '\0')
            break;
    }

    return 0;
}
#endif
