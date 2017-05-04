/*
 * date.c - setup and display date and time
 *
 * Copyright (C) 2001 by following authors
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <osbind.h>
#include <stdarg.h>
#include "doprintf.h"


void boncout_outc(int c)
{
  if(c == '\n') {
    Bconout(2,'\r');
  }
  Bconout(2,c);
}

int printf(const char *fmt, ...)
{
  int n;
  va_list ap;
  va_start(ap, fmt);
  n = doprintf(boncout_outc, fmt, ap);
  va_end(ap);
  return n;
}

static char *month_name[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  "13?", "14?", "15?"
};

int main(int argc, char **argv)
{
  long time;
  int year;
  int month;
  int day;
  int hour;
  int min;
  int sec;

  /* TODO, if argument, set the date */

  time = Gettime();

  year = 1980 + ((time >> 25) & 0x7F);
  month = (time >> 21) & 0xF;
  day = (time >> 16) & 0xF;
  hour = (time >> 11) & 0x1F;
  min = (time >> 5) & 0x3F;
  sec = (time << 1) & 0x3F;

  printf("%d %s %d, %02d:%02d:%02d\n",
         day, month_name[month-1], year, hour, min, sec);
  return 0;
}
