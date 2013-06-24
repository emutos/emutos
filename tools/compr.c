/*
 * compr.c - a simple compressor
 *
 * Copyright (C) 2002-2013 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define VERSION "0.2"

/*
 * This is a very naive compressor. It is best described by the
 * algorithm implemented in the decompressor:
 *
 * Conceptually the decompressor will read objects one at a time
 * from a 'file', and always append data to a 'buffer'. When the
 * job is done the 'buffer' contains the uncompressed data.
 * forever {
 *   read number n
 *   append n bytes verbatim from the 'file' into the 'buffer'
 *   read number offset
 *   quit if offset == 0
 *   read number n
 *   append n bytes from (buffer_position - offset) info the 'buffer',
 *     allowing overlapping regions (i.e. offset < n)
 * }
 *
 * Numbers are encoded in a single byte if <= 0x7F, and in a word
 * with msb set if <= 0x7FFF. longer numbers are not representable
 * (so for instance a long verbatim chunk would be cut in parts of
 * size 0x7FFF separated with dummy instructions to copy zero bytes)
 *
 * This scheme was chosen so that the decompressor can be very fast.
 * It achieves a moderate 30% compression ratio on the EmuTOS binary,
 * and about 40% on this source code file. Decompression should be
 * only marginally slower than simply copying the uncompressed data.
 *
 * The compression algorithm is crude too. Conceptually, we scan
 * the data and compare the 4 bytes string at the current position
 * with all possible previous 4 bytes string to find a matching
 * substring, then we take the best possible substring.
 *
 * Todo:
 * - check once and for all which stdio function set errno and which don't
 * - add an option to prepend the decompression code, together
 *   with the length of uncompressed data
 *
 * Possible Improvements
 * - it shouldn't be necessary to read all the file, just keeping
 *   64 kB in memory (32 ahead, and 32 back) should suffice.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#define DEBUG 0

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

static int verbose = 0;

static long total = 0;

static void * xmalloc(size_t s)
{
  void * a = malloc(s);
  if(a == NULL) {
    if(verbose > 1) {
      fatal(0, "out of memory (asking for %ld, total %ld)",
        (long)s, (long)total);
    } else {
      fatal(0, "out of memory");
    }
  }
  if(verbose > 1) total += s;
  return a;
}

/*
 * non-failing, error-reporting stdio FILE stuff
 */

typedef struct xfile {
  FILE *f;
  char fname[1];
} XFILE;

static XFILE *xfopen(const char *fname, const char *mode)
{
  int len = strlen(fname);
  XFILE *f = xmalloc(sizeof(*f) + len);
  f->f = fopen(fname, mode);
  if(f->f == NULL) {
    fatal(1, "cannot open %s", fname);
  }
  memcpy(f->fname, fname, len+1);
  return f;
}

static int xfclose(XFILE *f)
{
  if(fclose(f->f)) {
    fatal(1, "cannot close %s", f->fname);
  }
  free(f);
  return 0;
}

static size_t xfread(void *buf, size_t a, size_t b, XFILE * f)
{
  size_t count = fread(buf, a, b, f->f);
  if(ferror(f->f)) {
    fatal(1, "read error on %s", f->fname);
  }
  return count;
}

static size_t xfwrite(void *buf, size_t a, size_t b, XFILE * f)
{
  size_t count = fwrite(buf, a, b, f->f);
  if(ferror(f->f)) {
    fatal(1, "write error on %s", f->fname);
  }
  return count;
}

static long xftell(XFILE *f)
{
  long t = ftell(f->f);
  if(t < 0) {
    fatal(0, "cannot ftell on %s", f->fname);
  }
  return t;
}

static int xfseek(XFILE *f, long offset, int whence)
{
  if(fseek(f->f, offset, whence)) {
    fatal(1, "cannot fseek on %s", f->fname);
  }
  return 0;
}



#if INT_MAX >= 2147483647
typedef int index_t;
typedef unsigned int uhash_t;
#else
typedef long index_t;
typedef unsigned long uhash_t;
#endif

typedef unsigned char uchar;

/*
 * globals - everything will be indices into the whole data buffer,
 * so let's make this buffer global.
 */

static uchar *buf; /* holds the entire data to compress */
static index_t len;  /* size of data */

static const int z = 4;  /* minimum number of char for a string */

/*
 * lists of indices into the buffer (to each 4-bytes string is
 * associated the list of its occurrences so far)
 */

typedef struct cell {
  index_t x;
  struct cell *next;
} cell;

static cell *freelist = NULL;
static cell *cellpool = NULL;
static int cellpoolsize = 0;

static cell *cell_new(void)
{
  cell *p;
  if(freelist) {
    p = freelist; freelist = p->next;
  } else {
    if(cellpoolsize <= 0) {
      cellpoolsize = 100;
      cellpool = xmalloc(cellpoolsize * sizeof *cellpool);
    }
    p = cellpool + --cellpoolsize;
  }
  return p;
}

static void cell_free(cell *p)
{
  p->next = freelist; freelist = p;
}


/*
 * the hash table: will hash z consecutive bytes and remember
 * them by the index in buf
 */

typedef struct hcell {
  index_t x;
  struct hcell *next;
  struct cell *list;
} hcell;

static hcell *hfreelist = NULL;
static hcell *hcellpool = NULL;
static int hcellpoolsize = 0;

static hcell *hcell_new(void)
{
  hcell *p;
  if(hfreelist) {
    p = hfreelist;
    hfreelist = p->next;
  } else {
    if(hcellpoolsize <= 0) {
      hcellpoolsize = 100;
      hcellpool = xmalloc(hcellpoolsize * sizeof *hcellpool);
    }
    p = hcellpool + --hcellpoolsize;
  }
  return p;
}

static void hcell_free(hcell *p)
{
  p->next = hfreelist;
  hfreelist = p;
}

typedef struct hash {
  uhash_t mod;
  hcell **cells;
} hash;

static hash *hash_new(void)
{
  hash *h = xmalloc(sizeof *h);
  uhash_t i;
  h->mod = 50000;
  h->cells = xmalloc(h->mod * sizeof *h->cells);
  for(i = 0 ; i < h->mod ; i++) {
    h->cells[i] = NULL;
  }
  return h;
}

static uhash_t hash_compute(index_t x)
{
  uhash_t a = 0;
  uchar *c = buf + x;
  int i;
  for(i = 0 ; i < z ; i++) {
    uhash_t b = a >> (32 - 5);
    a <<= 5;
    a ^= *c++;
    a |= b;
  }
  return a;
}

/* add the bucket if the prefix is not here, and in any case
 * return the address of the (maybe new) hash bucket
 */
static hcell *hash_find_add(hash *h, index_t x)
{
  uhash_t m = hash_compute(x) % h->mod;
  hcell *p;
  for(p = h->cells[m]; p != NULL; p = p->next) {
    if(!memcmp(buf+x, buf+p->x, z)) {
      return p;
    }
  }
  p = hcell_new();
  p->x = x;
  p->next = h->cells[m];
  p->list = NULL;
  h->cells[m] = p;
  return p;
}

/* removes old strings from the hash, i.e. whose index x is < min_x */
static void hash_remove_old(hash *h, index_t min_x)
{
  uhash_t i;
  for(i = 0 ; i < h->mod ; i++) {
    hcell **hq = &(h->cells[i]);
    hcell *hp;
    for(hp = *hq; hp != NULL; hp = *hq) {
      cell **q = &hp->list;
      cell *p;
      for(p = *q ; p != NULL; p = *q) {
        if(p->x < min_x) {
          *q = p->next;
          cell_free(p);
        } else {
          q = &(p->next);
        }
      }
      if(hp->list == NULL) {
        *hq = hp->next;
        hcell_free(hp);
      } else {
        hq = &(hp->next);
      }
    }
  }
}

/*
 * output file
 */

static XFILE *ofile;

static void out_byte(uchar c) {
  xfwrite(&c, 1, 1, ofile);
}

static void out_num(int a)
{
#if DEBUG
  if(a < 0 || a >= 0x8000)
    fatal(0, "out_num(0x%x)", a);
#endif

  if(a < 128) {
    out_byte(a);
  } else {
    out_byte((a >> 8) | 0x80);
    out_byte(a & 0xFF);
  }
}

static void out_long(index_t a)
{
  out_byte((a>>24) & 0xFF);
  out_byte((a>>16) & 0xFF);
  out_byte((a>>8) & 0xFF);
  out_byte(a & 0xFF);
}

static void compress(void)
{
  index_t i;
  int remove_timeout = 0x7FFF;

  hash *h = hash_new();

  index_t verb_x = 0;
  int current_n = 0;

  for(i = 0 ; i < len - z ; i++) {
    hcell *hp = hash_find_add(h, i);
    cell *p = hp->list;
    if(p != NULL && current_n == 0) {
      /* if this string was encountered, look for the biggest
       * matching substring. As the most recent strings come first,
       * we can skip the end of the list once one of the string is too
       * far away in the past.
       */
      index_t best_x = 0;
      int best_n = 0;
      for(; p != NULL; p = p->next) {
        uchar *a = buf + p->x;
        uchar *b = buf + i;
        int n = 0;
        if(p->x <= i - 0x8000) {
          /* too old, we should delete it now but I'm lazy here. They will
           * be deleted eventually in hash_remove_old().
           */
          break;
        }
        while(*a++ == *b++ && i+n < len && n < 0x7FFF) n++;
        if(best_n < n) {
          best_n = n;
          best_x = p->x;
        }
      }
      if(best_n >= 4) {
        /* we've found a string. */
        /* emit pending verbatim data */
        while(verb_x + 0x7FFF < i) {
          int j;
          out_num(0x7FFF);
          for(j = 0 ; j < 0x7FFF ; j++) {
            out_byte(buf[verb_x++]);
          }
          /* dummy copy part */
          out_num(1);  /* offset 1 - because offset 0 means eof */
          out_num(0);  /* length 0 */
        }
        out_num(i - verb_x);
        while(verb_x < i) {
          out_byte(buf[verb_x++]);
        }
        /* emit the copy-back sequence */
        out_num(i-best_x);
        out_num(best_n);
        current_n = best_n;
        /* next verbatim part will be after this string */
        verb_x = i + best_n;
      }
    }
    /* in any case, record the current string in the hash. */
    p = cell_new();
    p->x = i;
    p->next = hp->list;
    hp->list = p;
    /* if in a copy, decrement the length */
    if(current_n > 0) {
      current_n --;
    }

    if(--remove_timeout <= 0) {
      if(verbose) fprintf(stderr, ".");
      remove_timeout = 0x7FFF;
      hash_remove_old(h, i-0x7FFF);
    }
  }
  if(verbose) fprintf(stderr, "\n");
  /* emit pending verbatim data */
  i = len;
  while(verb_x + 0x7FFF < i) {
    int j;
    out_num(0x7FFF);
    for(j = 0 ; j < 0x7FFF ; j++) {
      out_byte(buf[verb_x++]);
    }
    /* dummy copy part */
    out_num(1);  /* offset 1 - because offset 0 means eof */
    out_num(0);  /* length 0 */
  }
  out_num(i - verb_x);
  while(verb_x < i) {
    out_byte(buf[verb_x++]);
  }
  /* done */
  out_num(0);
}

static void read_all(const char *fname, uchar **buf, index_t *len)
{
  XFILE *f;
  size_t count;

  f = xfopen(fname, "rb");

  xfseek(f, 0L, SEEK_END);
  count = xftell(f);
  xfseek(f, 0L, SEEK_SET);
  *buf = xmalloc(count);
  *len = count;
  if(count != xfread(*buf, 1, count, f))
    fatal(1, "short read on %s", fname);
  xfclose(f);
}

static int percent(long a, long b)
{
  if(a <= 0 || a <= b) {
    return 0;
  } else if(a < LONG_MAX / 100) {
    return (a-b)*100/a;
  } else if(a < LONG_MAX / 10) {
    return (a-b)*10/(a/10);
  } else {
    return (a-b)/(a/100);
  }
}

static void usage(int exit_value)
{
  fprintf(stderr, "usage: compr -v * [ --rom <loader> ] <in> <out>\n");
  exit(exit_value);
}

static void do_it(char *lfname, char *ifname, char *ofname)
{
  ofile = xfopen(ofname, "wb");

  if(lfname != NULL) {
    /* first copy the loader to the beginning of the output file */
    XFILE *lfile = xfopen(lfname, "rb");
    for(;;) {
      char buf[0x4000];
      size_t len = xfread(buf, 1, sizeof buf, lfile);
      if(len <= 0) break;
      xfwrite(buf, 1, len, ofile);
    }
    xfclose(lfile);
  }

  read_all(ifname, &buf, &len);

  if(lfname != NULL) {
    /* align to even boundary */
    long len = xftell(ofile);
    if(len & 1) out_byte(0);
  }

  /* append "CMPR" */
  out_byte('C'); out_byte('M'); out_byte('P'); out_byte('R');

  if(lfname != NULL) {
    /* destination-address, len */
    uchar *a = buf+8;
    out_byte(*a++); out_byte(*a++); out_byte(*a++); out_byte(*a++);
    out_long(len);
  }

  compress();

  /* print stats just for fun */
  if(verbose)
  {
    long olen = xftell(ofile);
    printf("len = %ld, olen = %ld, saved %ld (%d%%)\n",
        (long)len, (long)olen, (long)len-olen, percent(len, olen));
  }
  if(verbose > 1) {
    printf("total memory allocated %ld\n", total);
  }

  xfclose(ofile);
}

int main(int argc, char **argv)
{
  int i;
  for(i = 1; i < argc ; i++) {
    char *a = argv[i];
    if(!strcmp(a, "--help")) {
      usage(0);
    } else if(!strcmp(a, "--version")) {
      printf("version " VERSION "\n");
      exit(0);
    } else if(!strcmp(a, "-v") || !strcmp(a, "--verbose")) {
      verbose++;
    } else if(!strcmp(a, "--rom")) {
      if(i + 4 != argc) usage(EXIT_FAILURE);
      do_it(argv[i+1], argv[i+2], argv[i+3]);
      break;
    } else if (i + 2 == argc) {
      do_it(NULL, argv[i], argv[i+1]);
      break;
    } else {
      usage(EXIT_FAILURE);
    }
  }
  exit(0);
}
