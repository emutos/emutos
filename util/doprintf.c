/*
 * doprintf adapted from ACK doprnt.c   1.1 as found in Minix 1.5
 * Unless otherwise stated, the MINIX BSD-ish license applies. 
 *
 * modified Laurent Vogel 2000, 2001
 */

#include "config.h"
#include <stdarg.h>
#include "doprintf.h"


static char *itoa(char *p, unsigned int num, int radix, char first_hex_letter)
{
  int i;
  char *q;

  q = p + 32;
  do {
    i = (int)(num % radix);
    if(i >= 10) {
      i += first_hex_letter - 10;
    } else {
      i += '0';
    }
    *--q = i;
  } while ((num = num / radix) != 0);
  i = (int)(p + 32 - q);
  do {
    *p++ = *q++;
  } while (--i);
  return(p);
}

static char *ltoa(char *p, unsigned long num, int radix, char first_hex_letter)
{
  int i;
  char *q;

  q = p + 32;
  do {
    i = (int)(num % radix);
    if(i >= 10) {
      i += first_hex_letter - 10;
    } else {
      i += '0';
    }
    *--q = i;
  } while ((num = num / radix) != 0);
  i = (int)(p + 32 - q);
  do {
    *p++ = *q++;
  } while (--i);
  return(p);
}

int doprintf(void (*outc)(int), const char *fmt, va_list ap)
{
  char buf[128];
  char *p;
  char *s;
  int c;
  int i;
  short  width;
  short  ndigit;
  int ndfnd;
  int ljust;
  int zfill;
  int len = 0;
  int lflag;
  char first_hex_letter;
  long l;
  for (;;) {
    p = buf;
    s = buf;
    while ((c = *fmt++) && c != '%') {
      (*outc)(c); len++;
    }
    if (c == 0)
      return len;
    ljust = 0;
    if (*fmt == '-') {
      fmt++;
      ljust++;
    }
    zfill = ' ';
    if (*fmt == '0') {
      fmt++;
      zfill = '0';
    }
    for (width = 0;;) {
      c = *fmt++;
      if (c >= '0' && c <= '9') {
        c -= '0';
      } else if (c == '*') {
        c = va_arg(ap, int);
      } else {
        break;
      }
      width *= 10;
      width += c;
    }
    ndfnd = 0;
    ndigit = 0;
    if (c == '.') {
      for (;;) {
        c = *fmt++;
        if (c >= '0' && c <= '9') {
          c -= '0';
        } else if (c == '*') {
          c = va_arg(ap, int);
        } else {
          break;
        }
        ndigit *= 10;
        ndigit += c;
        ndfnd++;
      }
    }
    lflag = 0;
    if (c == 'l' || c == 'L') {
      lflag++;
      if (*fmt)
        c = *fmt++;
    }
    first_hex_letter = 'a';
    switch (c) {
    case 'p':
      lflag++;
      zfill = '0';
      c = 16;
      width = 8;
      (*outc)('0'); len++;
      (*outc)('x'); len++;
      goto oxu;
    case 'X':
      first_hex_letter = 'A';
    case 'x':
      c = 16;
      goto oxu;
    case 'u':
      c = 10;
      goto oxu;
    case 'o':
      c = 8;
    oxu:
      if (lflag) {
        l = va_arg(ap, long);
        p = ltoa(p, l, c, first_hex_letter);
        break;
      }
      i = va_arg(ap, int);
      p = itoa(p, i, c, first_hex_letter);
      break;
    case 'i':
    case 'd':
      if (lflag) {
        l = va_arg(ap, long);
        if (l < 0) {
          *p++ = '-';
          l = -l;
        }
        p = ltoa(p, l, 10, first_hex_letter);
        break;
      }
      i = va_arg(ap, int);
      if (i < 0) {
        *p++ = '-';
        i = -i;
      }
      p = itoa(p, i, 10, first_hex_letter);
      break;
    case 'e':
    case 'f':
    case 'g':
      zfill = ' ';
      *p++ = '?';
      break;
    case 'c':
      zfill = ' ';
      c = va_arg(ap, int);
      if (c)
        *p++ = c;
      break;
    case 's':
      zfill = ' ';
      s = va_arg(ap, char *);
      if (s == 0)
        s = "(null)";
      if (ndigit == 0)
        ndigit = 32767;
      for (p = s; *p && --ndigit >= 0; p++)
        ;
      break;
    default:
      *p++ = c;
      break;
    }
    i = (int)(p - s);
    if ((width -= i) < 0)
      width = 0;
    if (ljust == 0)
      width = -width;
    if (width < 0) {
      if (*s=='-' && zfill=='0') {
        (*outc)(*s++); len++;
        i--;
      }
      do {
        (*outc)(zfill); len++;
      }
      while (++width != 0);
    }
    while (--i>=0) {
      (*outc)(*s++); len++;
    }
    while (width) {
      (*outc)(zfill); len++;
      width--;
    }
  }
}

