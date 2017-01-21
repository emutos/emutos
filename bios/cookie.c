/*
 * cookie.c - initialisation of a cookie jar
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "cookie.h"
#include "processor.h"
#include "tosvars.h"
#include "kprint.h"

/* the default cookie jar, in the bss */

static struct cookie dflt_jar[20];

void cookie_init(void)
{
    dflt_jar[0].tag = 0;
    dflt_jar[0].value = ARRAY_SIZE(dflt_jar);

    p_cookies = (LONG *)dflt_jar;
}

void cookie_add(long tag, long value)
{
    long n, count;
    struct cookie *jar = (struct cookie *)p_cookies;

    assert(jar != NULL);

    count = 0;
    while(jar->tag)
    {
        assert(jar->tag != tag);
        count++;
        jar++;
    }

    n = jar->value;
    assert(n != 0);
    if (count < (n-1))
    {
        jar->tag = tag;
        jar->value = value;
        jar[1].tag = 0;
        jar[1].value = n;
    }
}

#if CONF_WITH_FRB
/*
 * get the current value of the _FRB cookie; we must do this
 * dynamically because it can be changed by the user
 */
UBYTE *get_frb_cookie(void)
{
    struct cookie *jar;

    for (jar = (struct cookie *)p_cookies; jar->tag; jar++)
        if (jar->tag == COOKIE_FRB)
            return (UBYTE *)(jar->value);

    return NULL;
}
#endif
