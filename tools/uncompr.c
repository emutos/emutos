/*
 * uncompr.c - the test uncompressor for compr.c
 *
 * Copyright (C) 2002 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * This program is just here to be able to test compr.c
 * A real thing would not need to allocate all memory at
 * once, and would be generally speaking more efficient.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

static void fatal(int with_errno, const char *fmt, ...)
{
  int err = errno;
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  if(with_errno && err) {
    fprintf(stderr, ": %s\n", strerror(err));
  } else {
    fprintf(stderr, "\n");
  }
  exit(EXIT_FAILURE);
}


typedef unsigned char uchar;

/*
 * globals
 */

static FILE *fin;
static FILE *fout;

static uchar buf[0x7FFF];
static int buf_x = 0;

static void put_byte(uchar a)
{
  buf[buf_x] = a;
  fputc(a, fout);
  if(ferror(fout)) fatal(1, "write error");
  buf_x ++;
  if(buf_x >= 0x7FFF) buf_x = 0;
}

static uchar at_offset(int off)
{
  int x = buf_x - off;
  if(x < 0) x += 0x7FFF;
  return buf[x];
}


static uchar get_byte(void)
{
  int c = fgetc(fin);
  if(c == EOF) fatal(0, "eof reached");
  return c;
}

static int get_num(void)
{
  int a = get_byte();
  if(a & 0x80) {
    a &= ~0x80;
    a <<= 8;
    a += get_byte();
  }
  return a;
}

static void uncompress(void)
{
  for(;;) {
    int n, off;

    /* verbatim */
    n = get_num();
    while(n-- > 0) {
      uchar c = get_byte();
      put_byte(c);
    }

    /* copy back */
    off = get_num();
    if(off == 0) return;
    n = get_num();
    while(n-- > 0) {
      uchar c = at_offset(off);
      put_byte(c);
    }
  }
}

int main(int argc, char **argv)
{
  if(argc != 3)
    fatal(0, "usage: %s in out", argv[0]);

  fin = fopen(argv[1], "rb");
  if(fin == NULL)
    fatal(1, "fopen failed on %s", argv[1]);

  if(get_byte() != 'C' ||
     get_byte() != 'M' ||
     get_byte() != 'P' ||
     get_byte() != 'R') fatal(0, "it doesn't start with 'CMPR'");

  fout = fopen(argv[2], "wb");
  if(fout == NULL)
    fatal(1, "fopen failed on %s", argv[2]);

  uncompress();

  if(fclose(fin)) fatal(0, "cannot close %s", argv[1]);
  if(fclose(fout)) fatal(0, "cannot close %s", argv[2]);
  exit(0);
}
