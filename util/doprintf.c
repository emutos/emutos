/*
 * doprintf.c - a simple printf() implementation
 *
 * Copyright (C) 2019-2020 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * Note the following differences from a 'standard' implementation:
 *  1. The only flag supported is '-'
 *  2. The only size supported is 'l' ('L' is treated as a synonym)
 *  3. Only the following types are supported: cdioPpsuXx
 * The limitations are compatible with the previous implementation.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include <stdarg.h>
#include "doprintf.h"

/*
 * max size of buffer for number strings (arbitrary)
 */
#define MAXNUMLEN   100     /* allows a lot of leading zeros ... */

/*
 * definitions for 'flags'
 */
#define FLAG_LJUST  0x0001
#define FLAG_WIDTH  0x0002
#define FLAG_PREC   0x0004
#define FLAG_LONG   0x0008
#define FLAG_SIGN   0x0010
#define FLAG_CAPS   0x0020
#define FLAG_ZERO   0x0040  /* if set, pad with zeros, else spaces */

/*
 * convert the value passed to an ASCII string starting at p
 *
 * returns a pointer to the first character after the converted value
 */
static void *numconv(char *p, unsigned long value, int radix, int precision, unsigned int flags)
{
    char buf[MAXNUMLEN], *q;
    char c;
    long quot, rem;

    /* if displaying an unsigned short item, limit it to the appropriate size */
    if (!(flags & FLAG_LONG) && !(flags & FLAG_SIGN))
        value &= 0xffff;

    /* by default, output 0 as 0, rather than the empty string */
    if (!(flags & FLAG_PREC))
        precision = 1;

    /* create the string in reverse order in 'buf' */
    for (q = buf; value || (precision > 0); precision--)
    {
        quot = value / radix;
        rem = value - (quot * radix);
        if (rem < 10)
            c = rem + '0';
        else
            c = (rem - 10) + ((flags & FLAG_CAPS) ? 'A' : 'a');
        *q++ = c;
        value = quot;
    }

    /* copy to input buffer */
    for ( ; q > buf; )
        *p++ = *--q;

    return p;
}

int doprintf(void (*outc)(int), const char *RESTRICT fmt, va_list ap)
{
    char *p, *bufstart, buf[MAXNUMLEN];
    long longval;
    unsigned int flags;
    int n, type, fill_len, precision, width;
    char c, fill;
    int length = 0; /* returned by us */

    while(1)
    {
        /*
         * look for start of format string
         */
        while(1)
        {
            c = *fmt++;
            if (!c)
                return length;
            if (c == '%')
                break;
            (*outc)(c);
            length++;
        }

        /*
         * check flags
         *
         * note that we currently only support the '-' flag
         */
        flags = 0;
        if (*fmt == '-')
        {
            flags |= FLAG_LJUST;
            fmt++;
        }

        /*
         * get width, checking for fill character
         */
        width = 0;
        if (*fmt == '0')
        {
            flags |= FLAG_ZERO;
            fmt++;
        }
        for ( ; ; fmt++)
        {
            c = *fmt;
            if ((c >= '0') && (c <= '9'))
                n = c - '0';
            else if (c == '*')
                n = va_arg(ap, int);
            else break;
            width = width * 10 + n;
            flags |= FLAG_WIDTH;
        }

        /*
         * check for precision
         */
        precision = 0;
        if (*fmt == '.')
        {
            while(1)
            {
                c = *++fmt;
                if ((c >= '0') && (c <= '9'))
                    n = c - '0';
                else if (c == '*')
                    n = va_arg(ap, int);
                else break;
                precision = precision * 10 + n;
            }
            if (precision > MAXNUMLEN)
                precision = MAXNUMLEN;
            flags |= FLAG_PREC;
        }

        /*
         * check for size
         *
         * note that we currently only support 'l' (or 'L' as a synonym)
         */
        if ((*fmt == 'l') || (*fmt == 'L'))
        {
            fmt++;
            flags |= FLAG_LONG;
        }

        /*
         * handle type: the following types are recognised: cdioPpsuXx
         *
         * we assemble the output into a buffer first
         */
        p = bufstart = buf;
        type = *fmt++;
        switch(type)
        {
        case 'c':
            flags &= ~FLAG_ZERO;    /* precautionary */
            c = va_arg(ap, int);
            *p++ = c;
            break;
        case 'd':
        case 'i':
            flags |= FLAG_SIGN;
            FALLTHROUGH;
        case 'o':
        case 'u':
            if (flags & FLAG_LONG)
                longval = va_arg(ap, long);
            else
                longval = va_arg(ap, int);
            if (flags & FLAG_SIGN)
            {
                if (longval < 0)
                {
                    longval = -longval;
                    *p++ = '-';
                }
            }
            p = numconv(p, longval, (type=='o')?8:10, precision, flags);
            break;
        case 'P':
            flags |= FLAG_CAPS;
            FALLTHROUGH;
        case 'p':
            /* pointers are always long & zero-filled to a width of 8 */
            flags |= FLAG_WIDTH|FLAG_LONG|FLAG_ZERO;
            width = 8;
            outc('0');
            outc((flags&FLAG_CAPS)?'X':'x');
            length += 2;
            longval = va_arg(ap, long);
            p = numconv(p, longval, 16, precision, flags);
            break;
        case 'X':
            flags |= FLAG_CAPS;
            FALLTHROUGH;
        case 'x':
            if (flags & FLAG_LONG)
                longval = va_arg(ap, long);
            else
                longval = va_arg(ap, int);
            p = numconv(p, longval, 16, precision, flags);
            break;
        case 's':
            flags &= ~FLAG_ZERO;
            bufstart = va_arg(ap, char *);
            if (!bufstart)
                bufstart = "(null)";
            for (p = bufstart; *p; p++)
                ;
            if ((flags & FLAG_PREC) && (p-bufstart > precision))
                p = bufstart + precision;
            break;
        default:
            *p++ = type;    /* just copy unrecognised type ... */
            break;
        }

        /*
         * copy the buffer to output, respecting width, fill, and justification
         */
        n = p - bufstart;   /* item size */
        fill_len = 0;
        /* pad the item if necessary */
        if (flags & FLAG_WIDTH)
        {
            if (n < width)
                fill_len = width - n;
        }
        if (flags & FLAG_PREC)
            flags &= ~FLAG_ZERO;

        /*
         * if not left justified, output the fill characters.  note special
         * handling for negative numbers: if we are padding with zeros, the
         * minus sign must appear before the zeros; if we are padding with
         * spaces, there is no special handling.
         */
        fill = (flags & FLAG_ZERO) ? '0' : ' ';
        p = bufstart;
        if (!(flags & FLAG_LJUST))
        {
            if ((*p == '-') && (fill == '0'))
            {
                (*outc)(*p++);
                length++;
                n--;
            }
            for ( ; fill_len; fill_len--, length++)
                (*outc)(fill);
        }
        /* output the item */
        for ( ; n; n--, length++)
            (*outc)(*p++);
        /* output fill characters, if any */
        for ( ; fill_len; fill_len--, length++)
            (*outc)(fill);
    }

    return length;
}
