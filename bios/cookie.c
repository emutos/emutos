/*
 * cookie.c - initialisation of a cookie jar
 *
 * Copyright (c) 2001 EmuTOS development team.
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

#include "kprint.h"

/* the default cookie jar, in the bss */

static struct cookie dflt_jar[20];

void cookie_init(void)
{
  dflt_jar[0].tag = 0;
  dflt_jar[0].value = sizeof(dflt_jar) / sizeof(*dflt_jar);

  CJAR = dflt_jar;
}

void cookie_add(long tag, long value)
{
  long n, count;
  struct cookie *jar = CJAR;

  assert(jar != NULL);

  count = 0;
  while(jar->tag) {
    assert(jar->tag != tag);
    count++;
    jar++;
  }
  n = jar->value;
  assert(n != 0);
  if (count < (n-1)) {
    jar->tag = tag;
    jar->value = value;
    jar[1].tag = 0;
    jar[1].value = n;
  }
}






