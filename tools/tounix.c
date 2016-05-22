/*
 * tounix.c - utterly stupid program to convert text files containing ^M
 *
 * Copyright (C) 2001 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* Notes
 * - binary files are any file containing a non-printable ISO 8859-1 char.
 * - directories and symbolic links will be opened with fopen(), then
 *   will hopefully be detected as binary files, thus left untouched.
 * - the same temporary file is reused for all files. if a conversion
 *   is needed, the original file is renamed to a backup, then the
 *   content of the tmp file is copied back to the original file.
 * - the backup file will be filename~ in all cases, even when a filename~
 *   already exists (it will then be overriden).
 * - I've tried to restore things in place in case of trouble, but I
 *   cannot test all cases.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>


static void error(const char *fmt, ...)
{
  int err = errno;
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  if(err) {
    fprintf(stderr, ": %s\n", strerror(err));
  } else {
    fprintf(stderr, "\n");
  }
}

/* saves a backup of dst, and copies back the content of tmp to dst.
 * only len bytes are copied.
 * the backup file is renamed back to the original file if problems occur.
 */

#define SIZ 10240
static char buf[SIZ];

static void replace(FILE *tmp, long len, char *dst)
{
  FILE *g;
  int err;
  char *t;
  size_t count, count2;

  t = malloc(strlen(dst)+2);
  if(t == NULL) {
    error("could not allocate memory");
    goto fail1;
  }
  sprintf(t, "%s~", dst);
  err = rename(dst, t);
  if(err) {
    error("could not rename %s to %s", dst, t);
    goto fail1;
  }
  g = fopen(dst, "wb");
  if(g == NULL) {
    error("could not create %s", dst);
    goto fail3;
  }
  while(len > 0) {
    count = len;
    if(count > SIZ) count = SIZ;
    count2 = fread(buf, 1, count, tmp);
    if(count2 != count) {
      error("short read");
      goto fail3;
    }
    count2 = fwrite(buf, 1, count, g);
    if(count2 != count) {
      error("short write");
      goto fail3;
    }
    len -= count;
  }
  err = fclose(g);
  if(err) {
    error("could not close %s", dst);
    goto fail2;
  }
  return;

fail3:
  fclose(g);
fail2:
  err = rename(t, dst);
  if(err)
    error("could not rename %s to %s", t, dst);
fail1:
  return;
}




int main(int argc, char **argv)
{
  int i;
  FILE *f;
  int c;
  FILE *tmp = tmpfile();
  int flag = 0;
  long len;

  for(i = 1 ; i < argc ; i++) {
    if(argv[i][strlen(argv[i])-1] == '~') continue;
    f = fopen(argv[i], "rb");
    if(f == NULL) {
      error( "cannot open %s", argv[i]);
      continue;
    }
    flag = 0;
    rewind(tmp);
    for(;;) {
      c = getc(f);
      if(c == EOF) {
        break;
      } else if(c == 13) {
        flag = 1; /* need to convert */
      } else if( c != 9 && c != 10 && c != 12
          && (c < 32 || (c > 126 && c < 0240 ))) {
        /* printf("binary file %s\n", argv[i]); */
        /* no need to convert */
        flag = 0;
        break;
      } else {
        putc(c, tmp);
      }
    }
    fclose(f);
    len = ftell(tmp);
    rewind(tmp);
    if(flag) {
      printf("%s\n",argv[i]);
      if(ferror(tmp)) {
        error( "problem with the temp file", tmp);
        fclose(tmp);
        exit(1);
      } else {
        replace(tmp, len, argv[i]);
      }
    }
  }

  fclose(tmp);
  exit(0);
}
