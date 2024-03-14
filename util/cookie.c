/*
 * cookie.c - initialisation of a cookie jar
 *
 * Copyright (C) 2001-2024 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "cookie.h"
#include "tosvars.h"

#define DEF_IDT_TIME    (1<<12)     /* 24-hour clock */
#define DEF_IDT_DATE    (2<<8)      /* YY-MM-DD */
#define DEF_IDT_SEP     '/'
#define DEFAULT_IDT     (DEF_IDT_TIME | DEF_IDT_DATE | DEF_IDT_SEP)

/* the default cookie jar, in the bss */

static struct cookie dflt_jar[20];

void cookie_init(void)
{
    dflt_jar[0].tag = 0;
    dflt_jar[0].value = ARRAY_SIZE(dflt_jar);

    p_cookies = dflt_jar;
}

void cookie_add(ULONG tag, ULONG value)
{
    ULONG n, count;
    struct cookie *jar = p_cookies;

    count = 0;
    while(jar->tag)
    {
        count++;
        jar++;
    }

    n = jar->value;
    if (count < (n-1))
    {
        jar->tag = tag;
        jar->value = value;
        jar[1].tag = 0;
        jar[1].value = n;
    }
}

/*
 * get cookie
 *
 * returns TRUE if found, FALSE if not
 *
 * if found, copies the cookie value
 * into the variable pointed to by the second arg
 */
BOOL cookie_get(ULONG tag, ULONG *pvalue)
{
    struct cookie *jar;

    for (jar = p_cookies; jar->tag; jar++)
    {
        if (jar->tag == tag)
        {
            *pvalue = jar->value;
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * get the current value of the _IDT cookie; used by EmuDesk
 */
ULONG get_idt_cookie(void)
{
    ULONG idt;

    if (cookie_get(COOKIE_IDT, &idt))
        return idt;

    return DEFAULT_IDT; /* "can't happen" ... */
}

#if CONF_WITH_FRB
/*
 * get the current value of the _FRB cookie; we must do this
 * dynamically because it can be changed by the user
 */
UBYTE *get_frb_cookie(void)
{
    ULONG frbvalue;

    if (cookie_get(COOKIE_FRB, &frbvalue))
        return (UBYTE *)frbvalue;

    return NULL;
}
#endif

#if CONF_WITH_FDC
/*
 * get the type of floppy drive(s), from the _FDC cookie
 *
 * returns 0 if DD, 1 if HD
 */
WORD get_floppy_type(void)
{
    ULONG value;

    if (cookie_get(COOKIE_FDC, &value))
        return value>>24;

    return 0;
}
#endif

#if CONF_WITH_EXTENDED_MOUSE
/*
 * check if NVDI is installed by querying the cookie
 */
BOOL nvdi_cookie_present(void)
{
    ULONG dummy;
    return cookie_get(COOKIE_NVDI, &dummy);
}
#endif