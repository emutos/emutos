/*
 * tounix.c - utterly stupid program to convert text files containing ^M
 *
 * Copyright (c) 2001 Laurent Vogel
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
 * - as the old file is removed and the new one moved to the new name,
 *   the tool will also modify read-only files.
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

int main(int argc, char **argv)
{
  int i;
  FILE *f, *g;
  int c;
  char *tmp = tmpnam(NULL);
  int flag = 0;
  
  for(i = 1 ; i < argc ; i++) {
    f = fopen(argv[i], "rb");
    if(f == NULL) {
      error( "cannot open %s", argv[i]);
      continue;
    }
    g = fopen(tmp, "wb");
    if(g == NULL) {
      fprintf(stderr, "fatal: ");
      error("cannot open %s", tmp);
      fclose(f);
      exit(0);
    }
    flag = 0;
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
        putc(c, g);
      } 
    }
    fclose(f);
    fclose(g);
    if(flag) {
      printf("%s\n",argv[i]);
      if(ferror(g)) {
        error( "problem with %s", tmp);
      } else {
        /* replace f by g */
        int err = remove(argv[i]);
        if(err) {
          error( "cannot remove %s", argv[i]);
        } else {
          err = rename(tmp, argv[i]);
          if(err) {
            error( "cannot update %s", argv[i]);
          } 
        }
      }
    }
  }
  exit(0);
}

        
