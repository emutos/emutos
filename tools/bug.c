/*
 * bug.c - Basic Unencumbering Gettext, a minimal gettext-like tool
 *         (any better name is welcome)
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Bugs and limitations:
 * - double quotes within simple quotes are not handled
 * - free comments in po files are put to the end when updating
 * - only _() and N_() macros are handled
 * - no fuzzy or printf-format parameters
 * - some weird messages
 * - trigraphs are not handled (this is a feature actually !)
 * 
 * NOTE: to get warning messages about generation of illegal alert
 * strings, you must #define ALERT_TEXT_WARNINGS.  The generated
 * messages can include Atari versions of non-Latin characters, so
 * may appear as garbage on non-Atari systems.
 */

/*
 * Structure of this file
 * (in this order, as each function calls functions above):
 * - library support (errors, memory, ...)
 * - basic data structures (string, dynamic array, hash, ...)
 * - po-file data structure
 * - po-file administrative data entry
 * - input file with line counting and ability to go backwards
 * - low level lexical parsing for sh-like and C syntax.
 * - high level parsers
 * - charset conversion
 * - po file internal operations
 * - the three tool commands (xgettext, update, make)
 * - main()
 */

/*
 * TODO list
 * - better algorithm for merging po files (keep the order of entries)
 * - parse_po_file is a mess
 * - memory is not freed correctly, esp. after xstrdup
 * - use strerror() to report errors
 * - use #~ for old entries
 * - split this into proper files
 * - finish translate command
 * - LINGUAS, POTFILES.in to be optionally specified on the command line
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define VERSION "0.2b"

#define ALERT_TEXT_WARNINGS 0   /* 1 => generate experimental warning msgs */

#define TOOLNAME "bug"
#define DOCNAME  "doc/nls.txt"
#define LANGS_C  "util/langs.c"

#define HERE fprintf(stderr, "%s:%d\n", __FUNCTION__, __LINE__);
#define UNUSED(x) (void)(x) /* Unused variable */

/*
 * typedefs
 */

typedef unsigned char uchar;

/*
 * errors
 */

static void warn(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "Warning: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

static void fatal(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "Fatal: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(EXIT_FAILURE);
}

/*
 * memory
 */

static void * xmalloc(size_t s)
{
  void * a = calloc(1, s);
  if(a == 0) fatal("memory");
  return a;
}

static void * xrealloc(void * b, size_t s)
{
  void * a = realloc(b, s);
  if(a == 0) fatal("memory");
  return a;
}

/*
 * xstrdup
 */

static char * xstrdup(const char *s)
{
  int len = strlen(s);
  char *a = xmalloc(len+1);
  strcpy(a,s);
  return a;
}

/*
 * now - current date in gettext format (stolen from gettext)
 */

#define TM_YEAR_ORIGIN 1900

/* Yield A - B, measured in seconds.  */
static long difftm (const struct tm *a, const struct tm *b)
{
  int ay = a->tm_year + (TM_YEAR_ORIGIN - 1);
  int by = b->tm_year + (TM_YEAR_ORIGIN - 1);
  /* Some compilers cannot handle this as a single return statement.  */
  long days = (
               /* difference in day of year  */
               a->tm_yday - b->tm_yday
               /* + intervening leap days  */
               + ((ay >> 2) - (by >> 2))
               - (ay / 100 - by / 100)
               + ((ay / 100 >> 2) - (by / 100 >> 2))
               /* + difference in years * 365  */
               + (long) (ay - by) * 365l);

  return 60l * (60l * (24l * days + (a->tm_hour - b->tm_hour))
                + (a->tm_min - b->tm_min))
         + (a->tm_sec - b->tm_sec);
}


static char * now(void)
{
  time_t now;
  struct tm local_time;
  char tz_sign;
  long tz_min;
  char buf[40];

  time (&now);
  local_time = *localtime (&now);
  tz_sign = '+';
  tz_min = difftm (&local_time, gmtime (&now)) / 60;
  if (tz_min < 0) {
    tz_min = -tz_min;
    tz_sign = '-';
  }
  sprintf(buf, "%d-%02d-%02d %02d:%02d%c%02ld%02ld",
    local_time.tm_year + TM_YEAR_ORIGIN, local_time.tm_mon + 1,
          local_time.tm_mday, local_time.tm_hour, local_time.tm_min,
          tz_sign, tz_min / 60, tz_min % 60);
  return xstrdup(buf);
}

/*
 * da - dynamic array
 */

typedef struct da {
  int size;
  void **buf;
  int len;
} da;

#define DA_SIZE 1000

static void da_grow(da *d)
{
  if(d->size == 0) {
    d->size = DA_SIZE;
    d->buf = xmalloc(d->size * sizeof(void*));
  } else {
    d->size *= 4;
    d->buf = xrealloc(d->buf, d->size * sizeof(void*));
  }
}

static da * da_new(void)
{
  da *d = xmalloc(sizeof(*d));
  d->size = 0;
  d->len = 0;
  return d;
}

static void da_free(da *d)
{
  free(d->buf);
  free(d);
}

static int da_len(da *d)
{
  return d->len;
}

static void * da_nth(da *d, int n)
{
  return d->buf[n];
}

static void da_add(da *d, void *elem)
{
  if(d->len >= d->size) {
    da_grow(d);
  }
  d->buf[d->len++] = elem;
}

/*
 * s - string
 */

typedef struct str {
  int size;
  int len;
  char *buf;
} str;

#define STR_SIZE 100

static str * s_new(void)
{
  str *s = xmalloc(sizeof(*s));
  s->size = 0;
  s->len = 0;
  return s;
}

static void s_grow(str *s)
{
  if(s->size == 0) {
    s->size = STR_SIZE;
    s->buf = xmalloc(s->size);
  } else {
    s->size *= 4;
    s->buf = xrealloc(s->buf, s->size);
  }
}

static void s_free(str *s)
{
  if(s->size) {
    free(s->buf);
  }
  free(s);
}

static void s_addch(str *s, char c)
{
  if(s->len >= s->size) {
    s_grow(s);
  }
  s->buf[s->len++] = c;
}

static void s_addstr(str *s, char *t)
{
  while(*t) {
    s_addch(s, *t++);
  }
}

/* add a trailing 0 if needed and release excess mem */
static char * s_close(str *s)
{
  if(s->size == 0) {
    s->buf = xmalloc(1);
    s->buf[0] = 0;
    return s->buf;
  }
  s->buf = xrealloc(s->buf, s->len+1);
  s->buf[s->len] = 0;
  return s->buf;
}

static char * s_detach(str *s)
{
  char *t = s_close(s);
  free(s);
  return t;
}

/*
 * hi - hash item. This is intended to be aggregated by effective
 * hash item structures (a way to implement inheritance in C)
 */

typedef struct hash_item {
  char *key;
} hi;

/*
 * hash - a hash will contain hash-items sorted by their hash
 * value.
 */

#define HASH_SIZ 10000

typedef struct hash {
  da *d[HASH_SIZ];
} hash;

static hash * h_new(void)
{
  hash *h = xmalloc(sizeof(*h));
  return h;
}

/* a dumb one */
static unsigned int compute_hash(char *t)
{
  unsigned int m = 0;

  while(*t) {
    m += *t++;
    m <<= 1;
  }
  return m;
}

static void * h_find(hash *h, char *key)
{
  unsigned m = compute_hash(key) % HASH_SIZ;
  da *d;
  int i, n;
  hi *k;

  d = h->d[m];
  if(d != NULL) {
    n = da_len(d);
    for(i = 0 ; i < n ; i++) {
      k = da_nth(d, i);
      if(!strcmp(key, k->key)) {
        return k;
      }
    }
  }
  return NULL;
}

static void h_insert(hash *h, void *k)
{
  unsigned m = compute_hash(((hi *)k)->key) % HASH_SIZ;
  da *d;

  d = h->d[m];
  if(d == NULL) {
    d = da_new();
    h->d[m] = d;
  }
  da_add(d, k);
}

/*
 * oh - ordered hash
 */

typedef struct oh {
  hash *h;
  da *d;
} oh;

static oh * o_new(void)
{
  oh *o = malloc(sizeof(*o));
  o->h = h_new();
  o->d = da_new();
  return o;
}

static void o_free(oh *o)
{
  (void)o;
  /* TODO */
}

static void * o_find(oh *o, char *t)
{
  return h_find(o->h, t);
}

static void o_insert(oh *o, void *k)
{
  da_add(o->d, k);
  h_insert(o->h, k);
}

static void o_add(oh *o, void *k)
{
  da_add(o->d, k);
}

static int o_len(oh *o)
{
  return da_len(o->d);
}

static void * o_nth(oh *o, int n)
{
  return da_nth(o->d, n);
}

/*
 * ref - reference to locations in source files
 */

typedef struct ref {
  char *fname;
  int lineno;
} ref;

static ref * ref_new(char *fname, int lineno)
{
  ref *r = xmalloc(sizeof(*r));
  r->fname = fname;
  r->lineno = lineno;
  return r;
}

/*
 * poe - po-entries
 * the po structure is an ordered-hash of po-entries,
 * the po-entry being a sub-type of hash-item.
 */

#define KIND_NORM 0
#define KIND_COMM 1
#define KIND_OLD 2

typedef struct poe {
  hi    msgid;     /* the key (super-type) */
  int   kind;      /* kind of entry */
  char *comment;   /* free user comments */
  da   *refs;      /* the references to locations in code */
  char *refstr;    /* a char * representation of the references */
  char *msgstr;    /* the translation */
} poe;

static poe * poe_new(char *t)
{
  poe *e = xmalloc(sizeof(*e));
  e->msgid.key = t;
  e->kind = KIND_NORM;
  e->comment = "";
  e->refs = NULL;
  e->msgstr = "";
  e->refstr = "";
  return e;
}


/*
 * gettext administrative entry, an entry with msgid empty, and
 * msgstr being specially formatted (example in doc/nls.txt)
 */

typedef struct {
  char *pidvers;
  char *potcrdate;
  char *porevdate;
  char *lasttrans;
  char *langteam;
  char *charset;
  char *other;
} ae_t;

static void update_ae(ae_t *a, ae_t *pot_ae)
{
  a->potcrdate = pot_ae->potcrdate;
  a->porevdate = now();
}

static void fill_pot_ae(ae_t *a)
{
  a->pidvers = "PACKAGE VERSION";
  a->potcrdate = now();
  a->porevdate = "YEAR-MO-DA HO:MI+ZONE";
  a->lasttrans = "FULL NAME <EMAIL@ADDRESS>";
  a->langteam = "LANGUAGE <LL@li.org>";
  a->charset = "CHARSET";
  a->other = "";
}

static char * ae_to_string(ae_t *a)
{
  str *s = s_new();
  s_addstr(s, "Project-Id-Version: "); s_addstr(s, a->pidvers);
  s_addstr(s, "\nPOT-Creation-Date: "); s_addstr(s, a->potcrdate);
  s_addstr(s, "\nPO-Revision-Date: "); s_addstr(s, a->porevdate);
  s_addstr(s, "\nLast-Translator: "); s_addstr(s, a->lasttrans);
  s_addstr(s, "\nLanguage-Team: "); s_addstr(s, a->langteam);
  s_addstr(s, "\nMIME-Version: 1.0\nContent-Type: text/plain; charset=");
  s_addstr(s, a->charset);
  s_addstr(s, "\nContent-Transfer-Encoding: 8bit\n");
  s_addstr(s, a->other);
  return s_detach(s);
}

static int ae_check_line(char **cc, char *start, char **end)
{
  char *c, *t;
  int n = strlen(start);
  int m;
  c = *cc;
  if(strncmp(c, start, n)) {
    warn("Expecting \"%s\" in administrative entry", start);
    return 0;
  }
  t = strchr(c+n, '\n');
  if(t == NULL) {
    warn("Fields in administrative entry must end with \\n");
    return 0;
  }
  *cc = t + 1;
  m = t - (c+n);
  t = xmalloc(m+1);
  memmove(t, c+n, m);
  t[m] = 0;
  *end = t;
  return 1;
}

static int parse_ae(char *msgstr, ae_t *a)
{
  char *c = msgstr;
  char *tmp;
  if(!ae_check_line(&c, "Project-Id-Version: ", &a->pidvers)) goto fail;
  if(!ae_check_line(&c, "POT-Creation-Date: ", &a->potcrdate)) goto fail;
  if(!ae_check_line(&c, "PO-Revision-Date: ", &a->porevdate)) goto fail;
  if(!ae_check_line(&c, "Last-Translator: ", &a->lasttrans)) goto fail;
  if(!ae_check_line(&c, "Language-Team: ", &a->langteam)) goto fail;
  if(!ae_check_line(&c, "MIME-Version: ", &tmp)) goto fail;
  if(strcmp(tmp, "1.0")) {
    warn("MIME version must be 1.0");
    goto fail;
  }
  if(!ae_check_line(&c, "Content-Type: text/plain; charset=", &a->charset))
    goto fail;
  if(!ae_check_line(&c, "Content-Transfer-Encoding: ", &tmp)) goto fail;
  if(strcmp(tmp, "8bit")) {
    warn("Content-Transfer-Encoding must be 8bit");
    goto fail;
  }
  a->other = xstrdup(c);
  return 1;
fail:
  warn("Error in administrative entry");
  return 0;
}


/*
 * input files
 */

#define BACKSIZ 10
#define READSIZ 512

typedef struct ifile {
  int lineno;
  char *fname;
  FILE *fh;
  uchar buf[BACKSIZ + READSIZ];
  int size;
  int index;
  int ateof;
} IFILE;

static void irefill(IFILE *f)
{
  if(f->size > BACKSIZ) {
    memmove(f->buf, f->buf + f->size - BACKSIZ, BACKSIZ);
    f->size = BACKSIZ;
    f->index = f->size;
  }
  f->size += fread(f->buf + f->size, 1, READSIZ, f->fh);
}

static IFILE *ifopen(char *fname)
{
  IFILE *f = xmalloc(sizeof(IFILE));

  f->fname = xstrdup(fname);
  f->fh = fopen(fname, "rb");
  if(f->fh == 0) {
    free(f);
    return NULL;
  }
  f->size = 0;
  f->index = 0;
  f->ateof = 0;
  f->lineno = 1;
  return f;
}

static void ifclose(IFILE *f)
{
  fclose(f->fh);
  free(f);
}

static void iback(IFILE *f)
{
  if(f->index == 0) {
    fatal("too far backward");
  } else {
    if (f->buf[f->index] == 012) {
      f->lineno --;
    }
    f->index --;
  }
}

static void ibackn(IFILE *f, int n)
{
  f->index -= n;
  if(f->index < 0) {
    fatal("too far backward");
  }
}

static int igetc(IFILE *f)
{
  if(f->index >= f->size) {
    irefill(f);
    if(f->index >= f->size) {
      f->ateof = 1;
      return EOF;
    }
  }
  return f->buf[f->index++];
}

/* returns the next logical char, in sh syntax */
static int inextsh(IFILE *f)
{
  int ret;

  ret = igetc(f);
  if(ret == 015) {
    ret = igetc(f);
    if(ret == 012) {
      f->lineno++;
      return '\n';
    } else {
      iback(f);
      return 015;
    }
  } else if(ret == 012) {
    f->lineno++;
    return '\n';
  } else {
    return ret;
  }
}

/* returns the next logical char, in C syntax */
static int inextc(IFILE *f)
{
  int ret;

again:
  ret = igetc(f);
  /* look ahead if backslash new-line */
  if(ret == '\\') {
    ret = igetc(f);
    if(ret == 015) {
      ret = igetc(f);
      if(ret == 012) {
        f->lineno++;
        goto again;
      } else {
        ibackn(f,2);
        return '\\';
      }
    } else if(ret == 012) {
      f->lineno++;
      goto again;
    } else {
      iback(f);
      return '\\';
    }
  } else if(ret == 015) {
    ret = igetc(f);
    if(ret == 012) {
      f->lineno++;
      return '\n';
    } else {
      iback(f);
      return 015;
    }
  } else if(ret == 012) {
    f->lineno++;
    return '\n';
  } else {
    return ret;
  }
}

#define is_white(c)  (((c)==' ')||((c)=='\t')||((c)=='\f'))
#define is_letter(c) ((((c)>='a')&&((c)<='z'))||(((c)>='A')&&((c)<='Z')))
#define is_digit(c)  (((c)>='0')&&((c)<='9'))
#define is_octal(c)  (((c)>='0')&&((c)<='7'))
#define is_hexdig(c) ((((c)>='a')&&((c)<='f'))||(((c)>='A')&&((c)<='F')))
#define is_hex(c)    (is_digit(c)||is_hexdig(c))

/*
 * functions swallowing lexical tokens. returns 1 if
 * the token was the one tested for, return 0 else.
 */


static int try_eof(IFILE *f)
{
  int c = igetc(f);
  if(c == EOF) {
    return 1;
  } else {
    iback(f);
    return 0;
  }
}

static int try_c_comment(IFILE *f)
{
  int c;

  c = inextc(f);
  if(c == '/') {
    c = inextc(f);
    if(c == '/') {
      do {
        c = inextc(f);
      } while((c != EOF) && (c != '\n'));
      return 1;
    } else if(c == '*') {
      int state = 0;
      do {
        c = inextc(f);
        if(c == '*') {
          state = 1;
        } else if(c == '/') {
          if(state == 1) return 1;
          else state = 0;
        } else {
          state = 0;
        }
      } while(c != EOF);
      if(c == EOF) {
        warn("EOF reached inside comment");
        return 1;
      }
    }
  }
  iback(f);
  return 0;
}

static int try_white(IFILE *f)
{
  int c;

  c = inextc(f);
  if(is_white(c) || c == '\n') {
    do {
      c = inextc(f);
    } while (is_white(c) || (c == '\n'));
    if(c == EOF) return 1;
    iback(f);
    return 1;
  } else {
    iback(f);
    return 0;
  }
}

static int try_c_white(IFILE *f)
{
  if(try_eof(f)) {
    return 0;
  }
  if(try_c_comment(f) || try_white(f)) {
    while(! try_eof(f) && (try_c_comment(f) || try_white(f)))
      ;
    return 1;
  } else {
    return 0;
  }
}


/* only one "..." string, will be appended onto string s */
static int get_c_string(IFILE *f, str *s)
{
  int c;

  c = inextc(f);
  if(c != '"') {
    iback(f);
    return 0;
  }
  for(;;) {
    c = inextc(f);
    if(c == EOF) {
      warn("EOF reached inside string");
      return 0;
    } else if(c == '\\') {
      c = inextc(f);
      if(c == EOF) {
        warn("EOF reached inside string");
        return 0;
      } else if(is_octal(c)) {
        int i;
        int a = c - '0';
        c = inextc(f);
        for(i = 0 ; i < 3 && is_octal(c) ; i++) {
          a <<= 3;
          a += (c - '0');
          c = inextc(f);
        };
        s_addch(s, a);
        iback(f);
      } else if(c == 'x') {
        int a = 0;
        c = inextc(f);
        while(is_hex(c)) {
          a <<= 4;
          if(c <= '9') {
            a += (c - '0');
          } else if(c <= 'F') {
            a += (c - 'A' + 10);
          } else {
            a += (c - 'a' + 10);
          }
          c = inextc(f);
        }
        s_addch(s, a);
        iback(f);
      } else {
        switch(c) {
        case 'a': c = '\a'; break;
        case 'b': c = '\b'; break;
        case 'v': c = '\v'; break;
        case 'e': c =  033; break;  /* GNU C extension: \e for escape */
        case 'f': c = '\f'; break;
        case 'r': c = '\r'; break;
        case 't': c = '\t'; break;
        case 'n': c = '\n'; break;
        default: break;
        }
        s_addch(s, c);
      }
    } else if(c == '\"') {
      return 1;
    } else {
      s_addch(s,c);
    }
  }
}

/*
 * parse c files
 * put strings surrounded by _("...") or N_("...") into the ordered-hash
 * state means :
 * 0 outside names, 1 after 'N', 2 after '_', 3 after '(', 4 in a name
 * when anything meaningful has been parsed, the corresponding structure of
 * the action structure is called.
 */

typedef struct parse_c_action {
  void (*gstring)(void *this, str *s, char *fname, int lineno);
  void (*string)(void *this, str *s);
  void (*other)(void *this, int c);
} parse_c_action;

static void pca_xgettext_gstring(void *this, str *s, char *fname, int lineno)
{
  oh *o = (oh *) this;
  poe *e;
  ref *r;
  char *t;

  t = s_detach(s);

  /* add the string into the hash */
  e = o_find(o, t);
  if(e) {
    /* the string already exists */
    free(t);
  } else {
    e = poe_new(t);
    o_insert(o, e);
  }
  r = ref_new(fname, lineno);
  if(e->refs == 0) {
    e->refs = da_new();
  }
  da_add(e->refs, r);
}

static void pca_xgettext_string(void *this, str *s)
{
  (void)this;
  s_free(s);
}

static void pca_xgettext_other(void *this, int c)
{
  (void)this;
  (void)c;
}

parse_c_action pca_xgettext[] = { {
  pca_xgettext_gstring,
  pca_xgettext_string,
  pca_xgettext_other,
} };

static void print_canon(FILE *, const char *, const char *);

/* pcati - Parse C Action Translate Info */
typedef struct pcati {
  FILE *f;
  void (*conv)(char *);
  oh *o;
} pcati;


static void pca_translate_gstring(void *this, str *s, char *fname, int lineno)
{
  pcati *p = (pcati *) this;
  char *t;
  poe *e;

  (void)fname;
  (void)lineno;

  t = s_detach(s);
  e = o_find(p->o, t);
  if(e) {  /* if there is a translation, get it instead */
    free(t);
    t = xstrdup(e->msgstr);
  }
  p->conv(t);  /* convert the string, be it a translation or the original */
  print_canon(p->f, t, "");
  free(t);
}

static void pca_translate_string(void *this, str *s)
{
  pcati *p = (pcati *) this;
  char *t;

  t = s_detach(s);
  print_canon(p->f, t, "");
  free(t);
}

static void pca_translate_other(void *this, int c)
{
  pcati *p = (pcati *) this;

  fputc(c, p->f);
}

parse_c_action pca_translate [] = { {
  pca_translate_gstring,
  pca_translate_string,
  pca_translate_other,
} } ;


static void parse_c_file(char *fname, parse_c_action *pca, void *this)
{
  int c;
  int state;
  str *s;
  int lineno;

  IFILE *f = ifopen(fname);
  if(f == NULL) {
    warn("could not open file '%s'", fname);
    return;
  }

/* TODO - merge parse_c_comment into this, rewrite the parser */

  state = 0;
  for(;;) {
    c = inextc(f);
    if(c == EOF) {
      break;
    } else if(c == '/') {
      c = inextc(f);
      if(c == '/') {
        pca->other(this, '\n');
      }
      ibackn(f,2);
      c = '/';
      state = 0;
      if(! try_c_comment(f)) {
        pca->other(this, c);
      } else {
        pca->other(this, ' ');
      }
    } else if(c == '\"') {
      if(state == 3) {
        /* this is a new gettext string */
        s = s_new();
        lineno = f->lineno;
        /* accumulate all consecutive strings (separated by spaces) */
        do {
          iback(f);
          get_c_string(f, s);
          try_c_white(f);
          c = inextc(f);
        } while(c == '\"');
        if(c != ')') {
          char *t = s_detach(s);
          warn("_(\"...\" with no closing )");
          warn("the string is %s", t);
          free(t);
          state = 0;
          continue;
        }
        /* handle the string */
        pca->gstring(this, s, fname, lineno);
        pca->other(this, ')');
      } else {
        iback(f);
        s = s_new();
        get_c_string(f, s);
        pca->string(this, s);
      }
    } else {
      if(c == '(') {
        if(state == 2) {
          state = 3;
        } else {
          state = 0;
        }
      } else if(c == '_') {
        if(state < 2) {
          state = 2;
        } else {
          state = 0;
        }
      } else if(c == 'N') {
        if(state == 0) {
          state = 1;
        } else {
          state = 4;
        }
      } else if(is_white(c)) {
        if((state == 1) || (state == 4)) {
          state = 0;
        }
      } else if(is_letter(c) || is_digit(c)) {
        state = 4;
      } else {
        state = 0;
      }
      pca->other(this, c);
    }
  }
  ifclose(f);
}


/*
 * parse po files
 */


static void parse_po_file(char *fname, oh *o, int ignore_ae)
{
  int c;
  IFILE *f;
  poe *e;
  str *s, *userstr, *refstr, *otherstr, *msgid, *msgstr;

  f = ifopen(fname);
  if(f == NULL) {
    /* TODO: UGLY HACK !!! */
    if(!strcmp("po/messages.pot", fname)) {
      fatal("could not open %s (run 'bug xgettext' to generate it)", fname);
    } else {
      fatal("could not open %s", fname);
    }
  }
  for(;;) {
    c = inextsh(f);
    /* skip any blank line before next entry */
    while ((c == ' ') || (c == '\t')) {
      while ((c == ' ') || (c == '\t')) {
        c = inextsh(f);
      }
      if((c != EOF) && (c != '\n')) {
        warn ("syntax error in %s line %d", fname, f->lineno);
        while ((c != EOF) && (c != '\n')) {
          c = inextsh(f);
        }
      }
      c = inextsh(f);
    }
    if (c == EOF) {
      break;
    }

    /* start an entry */
    userstr = 0;
    refstr = 0;
    otherstr = 0;
    msgid = 0;
    msgstr = 0;
    while (c == '#') {
      c = inextsh(f);
      switch(c) {
      case '\n':
      case ' ':  /* user comment */
        if(! userstr) userstr = s_new();
        s = userstr;
        break;
      case ':':  /* ref comment */
        if(! refstr) refstr = s_new();
        s = refstr;
        break;
      default:  /* other comment */
        if(! otherstr) otherstr = s_new();
        s = otherstr;
        break;
      }
      /* accumulate this comment line to the string */
      s_addch(s, '#');
      if(c == EOF) {
        s_addch(s, '\n');
        break;
      }
      s_addch(s, c);
      if(c != '\n') {
        while((c != EOF) && (c != '\n')) {
          c = inextsh(f);
          s_addch(s, c);
        }
        if(c == EOF) {
          s_addch(s, '\n');
        }
      }
      c = inextsh(f);
    }
    if((c == ' ') || (c == '\t') || (c == '\n') || (c == EOF)) {
      /* the previous entry is a pure comment */
      if(userstr) {
        if(otherstr) {
          s_addstr(userstr,s_close(otherstr));
          s_free(otherstr);
        }
      } else if(otherstr) {
        userstr = otherstr;
      } else {
        if(refstr) {
          s_free(refstr);
          warn("stray ref ignored in %s:%d", fname, f->lineno);
        }
        /* we will reach here when an entry is followed by more than one
         * empty line, at each additional empty line.
         */
        continue;
      }
      e = poe_new("");
      e->comment = s_detach(userstr);
      e->kind = KIND_COMM;
      o_add(o, e);
      continue;
    }
    if(c != 'm') goto err;
    c = inextsh(f);
    if(c != 's') goto err;
    c = inextsh(f);
    if(c != 'g') goto err;
    c = inextsh(f);
    if(c != 'i') goto err;
    c = inextsh(f);
    if(c != 'd') goto err;
    c = inextsh(f);
    if(c != ' ' && c != '\t') goto err;
    while(c == ' ' || c == '\t') {
      c = inextsh(f);
    }
    if(c != '\"') goto err;
    s = msgid = s_new();
    /* accumulate all consecutive strings (separated by spaces) */
    do {
      iback(f);
      get_c_string(f, s);
      c = inextsh(f);
      while((c == ' ') || (c == '\t')) {
        c = inextsh(f);
      }
      if(c == EOF) goto err;
      if(c != '\n') goto err;
      c = inextsh(f);
    } while(c == '\"');
    if(c != 'm') goto err;
    c = inextsh(f);
    if(c != 's') goto err;
    c = inextsh(f);
    if(c != 'g') goto err;
    c = inextsh(f);
    if(c != 's') goto err;
    c = inextsh(f);
    if(c != 't') goto err;
    c = inextsh(f);
    if(c != 'r') goto err;
    c = inextsh(f);
    if(c != ' ' && c != '\t') goto err;
    while((c == ' ') || (c == '\t')) {
      c = inextsh(f);
    }
    if(c != '\"') goto err;
    s = msgstr = s_new();
    /* accumulate all consecutive strings (separated by spaces) */
    do {
      iback(f);
      get_c_string(f, s);
      c = inextsh(f);
      while((c == ' ') || (c == '\t')) {
        c = inextsh(f);
      }
      if(c == EOF) break;
      if(c != '\n') goto err;
      c = inextsh(f);
    } while(c == '\"');
    if(c != '\n' && c != EOF) goto err;
    /* put the comment in userstr */
    if(userstr) {
      if(otherstr) {
        s_addstr(userstr,s_close(otherstr));
        s_free(otherstr);
        otherstr = 0;
      }
    } else if(otherstr) {
      userstr = otherstr;
      otherstr = 0;
    }
    /* now we have the complete entry */
    e = o_find(o, s_close(msgid));
    if(e) {
      warn("double entry %s", s_close(msgid));
      s_free(msgid);
      s_free(msgstr);
    } else if(ignore_ae && msgid->buf[0] == '\0') {
      /* ignore administrative entry */
      s_free(msgid);
      s_free(msgstr);
    } else {
      e = poe_new(s_detach(msgid));
      e->msgstr = s_detach(msgstr);
      if(refstr) {
        e->refstr = s_detach(refstr);
        refstr = 0;
      }
      if(userstr) {
        e->comment = s_detach(userstr);
        userstr = 0;
      }
      o_insert(o, e);
    }
    /* free temp strings */
    if(refstr) {
      s_free(refstr);
    }
    if(otherstr) {
      s_free(otherstr);
    }
    if(userstr) {
      s_free(userstr);
    }
    continue;
err:
    warn("syntax error at %s:%d (c = '%c')", fname, f->lineno, c);
    while(c != '\n' && c != EOF) {
      c = inextsh(f);
    }
  }
  ifclose(f);
}



/*
 * parse OIPL (one item per line) simple files
 */

static void parse_oipl_file(char *fname, da *d)
{
  int c;
  IFILE *f;

  f = ifopen(fname);
  if(f == NULL) {
    fatal("could not open %s", fname);
  }
  for(;;) {
    c = inextsh(f);
    if(c == EOF) {
      break;
    } else if(c == '#') {
      while((c != EOF) && (c != '\n')) {
        c = inextsh(f);
      }
    } else if((c == ' ') || (c == '\t')) {
      while((c == ' ') || (c == '\t')) {
        c = inextsh(f);
      }
      if((c != EOF) && (c != '\n')) {
        warn("syntax error in %s line %d", fname, f->lineno);
        while((c != EOF) && (c != '\n')) {
          c = inextsh(f);
        }
      }
    } else if(c == '\n') {
      continue;
    } else {
      str *s = s_new();
      while((c != EOF) && (c != '\n')) {
        s_addch(s, c);
        c = inextsh(f);
      }
      da_add(d, s_detach(s));
    }
  }
  ifclose(f);
}


/*
 * Check given alert line and button text and complain
 * if they're too long or there are too many lines of
 * text.  For buttons the lines parameter is zero.
 */
static void alert_check(const char *start, const char *end, int lines)
{
#if ALERT_TEXT_WARNINGS
  int len = end - start - 1;
  const char *errstr;
  char *tmpstr;

  if (lines) {
    /* dialog text */
    if (lines > 5) {
      errstr = "with line '%s', dialog has more than 5 lines";
    } else if (len > 32) {
      errstr = "dialog line '%s' longer than 32 chars";
    } else {
      return;
    }
  } else {
    /* dialog button */
    if (len > 10) {
      errstr = "dialog button text '%s' exceeds 10 chars";
    } else {
      return;
    }
  }
  tmpstr = xstrdup(start);
  tmpstr[len] = '\0';
  warn(errstr, tmpstr);
  free(tmpstr);
#else /* !ALERT_TEXT_WARNINGS */
  UNUSED(start);
  UNUSED(end);
  UNUSED(lines);
#endif
}

/*
 * print string in canonical format
 *
 * NOTE: the 'canonical' format is modified for handling of
 * the GEM Alert string specifications: if the string begins with
 * [n][, where n is a digit, then the string will be cut after
 * this initial [n][ and after every |.
 */

#define CANON_GEM_ALERT 1

static void print_canon(FILE *f, const char *t, const char *prefix)
{
  unsigned a;
#if CANON_GEM_ALERT
  int gem_alert = 0, gem_button = 0, alert_lines = 0;
  const char *line_start = NULL;
#endif /* CANON_GEM_ALERT */

  if(strchr(t, '\n')) {
    fprintf(f, "\"\"\n%s", prefix);
  }

#if CANON_GEM_ALERT
  if(t[0] == '[' && t[1] >= '0' && t[1] <= '9' && t[2] == ']' && t[3] == '[') {
    fprintf(f, "\"[%c][\"\n%s", t[1], prefix);
    t += 4;
    line_start = t;
    gem_alert = 1;
  }
#endif /* CANON_GEM_ALERT */

  fprintf(f, "\"");
  while(*t) {
    switch(*t) {
    case '\n':
      if(t[1]) {
        fprintf(f, "\\n\"\n%s\"", prefix);
      } else {
        fprintf(f, "\\n");
      }
      break;
    case '\r': fprintf(f, "\\r"); break;
    case '\t': fprintf(f, "\\t"); break;
    case '\"':
    case '\\': fprintf(f, "\\%c", *t); break;
#if CANON_GEM_ALERT
    case '|':
      if(gem_alert) {
        alert_lines += 1;
        alert_check(line_start, t, alert_lines);
        line_start = t + 1;
        fprintf(f, "%c\"\n%s\"", *t, prefix);
        break;
      } else if (gem_button) {
        alert_check(line_start, t, 0);
        line_start = t + 1;
      }
      /* fallthrough */
    case ']':
      if(gem_alert) {
        gem_alert = 0;
        alert_check(line_start, t, alert_lines + 1);
        if(t[1] == '[') {
          line_start = t + 2;
          gem_button = 1;
        }
      } else if (gem_button && *t != '|') {
        gem_button = 0;
        alert_check(line_start, t, 0);
      }
      /* fallthrough */
#endif /* CANON_GEM_ALERT */
    default:
      a = ((unsigned)(*t))&0xFF;
      if((a < ' ') || (a >= 127 && a < 0xa0)) {
        /* not ISO latin 1 */
        fprintf(f, "\\%03o", a);
      } else {
        fprintf(f, "%c", *t);
      }
    }
    t++;
  }
  fprintf(f, "\"");
}

/*
 * pretty print refs.
 */

static char * refs_to_str(da *refs)
{
  int pos, len;
  int i, n;
  str *s;
  ref *r;
  char line[12];

  s = s_new();
  s_addstr(s, "#:");
  pos = 2;
  n = da_len(refs);
  for(i = 0 ; i < n ; i++) {
    r = da_nth(refs, i);
    sprintf(line, ":%d", r->lineno);
    len = strlen(line)+strlen(r->fname);
    if(pos + len > 78) {
      s_addstr(s, "\n#:");
      pos = 2;
    }
    pos += len + 1;
    s_addch(s, ' ');
    s_addstr(s, r->fname);
    s_addstr(s, line);
    free(r);
  }
  s_addch(s, '\n');
  return s_detach(s);
}

/*
 * convert_refs : transform da * refs into char * refstr in all entries
 * of the po file contained in an oh.
 */

static void po_convert_refs(oh *o)
{
  int i, n;
  poe *e;

  n = o_len(o);
  for(i = 0 ; i < n ; i++) {
    e = o_nth(o, i);
    if(e->refs) {
      e->refstr = refs_to_str(e->refs);
      da_free(e->refs);
      e->refs = 0;
    }
  }
}

/*
 * print po file
 */

static void print_po_file(FILE *f, oh *o)
{
  int i, n;
  char *prefix;

  n = o_len(o);
  for(i = 0 ; i < n ; i++) {
    poe *e = o_nth(o, i);
    fputs(e->comment, f);
    if(e->kind == KIND_COMM) {
      fputs("\n", f);
      continue;
    } else if(e->kind == KIND_OLD) {
      prefix = "#~";
    } else {
      fputs(e->refstr, f);
      prefix = "";
    }
    fprintf(f, "%smsgid ", prefix);
    print_canon(f, e->msgid.key, prefix);
    fprintf(f, "\n%smsgstr ", prefix);
    print_canon(f, e->msgstr, prefix);
    fputs("\n\n", f);
  }
}

/*
 * update po file against messages.pot
 */

static void update(char *fname)
{
  oh *o1, *o2, *o;
  poe *e1, *e2, *e;
  str *s;
  char * bfname;
  FILE *f;
  int i1, i2, n1, n2;
  int numtransl = 0;   /* number of translated entries */
  int numold = 0;      /* number of old entries */
  int numuntransl = 0; /* number of untranslated entries */

  /* get the reference first, before renaming the file */
  o1 = o_new();
  parse_po_file("po/messages.pot", o1, 0);

  /* rename the po file (backup) */
  s = s_new();
  s_addstr(s, fname);
  s_addstr(s, ".bak");
  bfname = s_detach(s);

  if(rename(fname, bfname)) {
    warn("cannot rename file '%s' to '%s', cancelled", fname, bfname);
    return;
  }

  /* parse the po file */
  o2 = o_new();
  parse_po_file(bfname, o2, 0);

  /* scan o1 and o2, merging the two */
  n1 = o_len(o1);
  n2 = o_len(o2);
  o = o_new();
  /* first, put an updated admin entry */
  {
    ae_t a1, a2;
    e1 = o_find(o1, "");
    e2 = o_find(o2, "");
    if(e1 == NULL || !parse_ae(e1->msgstr, &a1)) {
      fill_pot_ae(&a1);
    }
    if(e2 == NULL || !parse_ae(e2->msgstr, &a2)) {
      warn("bad administrative entry, getting back that of the template");
      a2 = a1;
    }
    update_ae(&a2, &a1);
    e = poe_new("");
    if(e2 != NULL) {
      e->comment = e2->comment; /* keep the initial comment */
    }
    e->kind = KIND_NORM;
    e->msgstr = ae_to_string(&a2);
    o_insert(o, e);
  }

  /* TODO, better algorithm to keep the order of entries... */

  /* first, update entries in the po file */
  for(i2 = 0 ; i2 < n2 ; i2 ++) {
    e2 = o_nth(o2, i2);
    if(e2->kind == KIND_COMM) {
      o_add(o, e2);
    } else if(e2->msgid.key[0] == 0) {
      /* the old admin entry - do nothing */
    } else {
      e = o_find(o1, e2->msgid.key);
      if(e) {
        e2->kind = KIND_NORM;
        e2->refstr = e->refstr;
        e2->refs = 0;
        o_add(o, e2);
        if(strcmp("", e2->msgstr)) {
          numtransl++;
        } else {
          numuntransl++;
        }
      } else {
        e2->kind = KIND_OLD;
        o_add(o, e2);
        numold++;
      }
    }
  }

  /* then, add new entries from the template */
  for(i1 = 0 ; i1 < n1 ; i1 ++) {
    e1 = o_nth(o1, i1);
    if(e1->kind == KIND_NORM) {
      if(e1->msgid.key[0] != 0) {
        e = o_find(o2, e1->msgid.key);
        if(!e) {
          o_add(o, e1);
          numuntransl++;
        }
      }
    }
  }

  /* print stats */
  printf("translated %d, untranslated %d, obsolete %d\n",
    numtransl, numuntransl, numold);

  /* dump o into the new file */
  f = fopen(fname, "w");
  if(f == NULL) {
    fatal("could not open %s", fname);
  }
  print_po_file(f, o);
  fclose(f);
}


/*
 * xgettext : parse POTFILES.in, and generate messages.pot
 */

static void xgettext(void)
{
  da *d;
  oh *o;
  int i, n;
  FILE *f;
  char *fname;
  ae_t a;
  poe *e;

  d = da_new();
  parse_oipl_file("po/POTFILES.in", d);

  o = o_new();

  /* create the administrative entry */
  fill_pot_ae(&a);
  e = poe_new("");
  e->kind = KIND_NORM;
  e->msgstr = ae_to_string(&a);
  e->comment = "# SOME DESCRIPTIVE TITLE.\n\
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n";
  o_add(o, e);

  n = da_len(d);
  for(i = 0 ; i < n ; i++) {
    parse_c_file(da_nth(d,i), pca_xgettext, o);
  }

  po_convert_refs(o);

  fname = "po/messages.pot";
  f = fopen(fname, "w");
  if(f == NULL) {
    fatal("couldn't create %s", fname);
  }
  print_po_file(f, o);
  fclose(f);
}

/*
 * charset conversions
 */


/*
 * charsets
 */

struct charset_alias {
  const char *name;
  const char *alias;
};

const struct charset_alias charsets[] = {
  { "latin1", "ISO-8859-1" },
  { "latin2", "ISO-8859-2" },
  { "latin9", "ISO-8859-15" },
};

/* resolve any known alias */
static const char * get_canon_cset_name(const char *name)
{
  int i;
  int n = sizeof(charsets)/sizeof(*charsets);
  for(i = 0 ; i < n ; i++) {
    if(!strcmp(charsets[i].alias, name)) {
      return charsets[i].name;
    }
  }
  return xstrdup(name);
}

/*
 * charset conversion
 */

/*
 * iso_to_atari : convert in situ iso latin 1 to atari ST encoding
 */

static const unsigned char i2a[] = {
  /*     ¡     ¢     £     ¤     ¥     ¦     § */
  ' ' , 0xad, 0x9b, 0x9c,    0, 0x9d, '|', 0xdd,
  /* ¨   ©     ª     «     ¬     ­     ®     ¯ */
  0xb9, 0xbd, 0xa6, 0xae, 0xaa,  '-', 0xbe, 0xff,
  /* °   ±     ²     ³     ´     µ     ¶     · */
  0xf8, 0xf1, 0xfd, 0xfe, 0xba, 0xe6, 0xbc,  0,
  /* ¸   ¹     º     »     ¼     ½     ¾     ¿ */
  '?' , '?',  0xa7, 0xaf, 0xac, 0xab,    0, 0xa8,
  /* À   Á     Â     Ã     Ä     Å     Æ     Ç */
  0xb6, 'A',   'A', 0xB7, 0x8e, 0x8f, 0x92, 0x80,
  /* È   É     Ê     Ë     Ì     Í     Î     Ï */
  'E' , 0x90,  'E',  'E',  'I',  'I',  'I',  'I',
  /* Ð   Ñ     Ò     Ó     Ô     Õ     Ö     × */
  0   , 0xa5,  'O',  'O',  'O', 0xb8, 0x99,  'x',
  /* Ø   Ù     Ú     Û     Ü     Ý     Þ     ß */
  0xb2, 'U' ,  'U',  'U', 0x9a,  'Y',   0,  0x9e,
  /* à   á     â     ã     ä     å     æ     ç */
  0x85, 0xa0, 0x83, 0xb0, 0x84, 0x86, 0x91, 0x87,
  /* è   é     ê     ë     ì     í     î     ï */
  0x8a, 0x82, 0x88, 0x89, 0x8d, 0xa1, 0x8c, 0x8b,
  /* ð   ñ     ò     ó     ô     õ     ö     ÷ */
  0   , 0xa4, 0x95, 0xa2, 0x93, 0xb1, 0x94, 0xf6,
  /* ø   ù     ú     û     ü     ý     þ     ÿ */
  0xb3, 0x97, 0xa3, 0x96, 0x81,  'y',  0,   0x98,
};

static void latin1_to_atarist(char *s)
{
  unsigned c;
  int warned = 0;

  while((c = (((unsigned) (*s)) & 0xFF))){
    if(c >= 0xa0) {
      c = i2a[c - 0xa0];
      if(c == 0) {
        c = '?';
        if(!warned) {
          warn("untranslatable character '%c'", *s);
          warned = 1;
        }
      }
      *s = c;
    }
    s++;
  }
}


static void converter_noop(char *s)
{
  (void)s;
}

typedef void(*converter_t)(char *);


struct converter_info {
  const char * from;
  const char * to;
  converter_t func;
};

static const struct converter_info converters[] = {
  { "latin1", "atarist", latin1_to_atarist },
};

static converter_t get_converter(const char * from, const char * to)
{
  int i;
  int n = sizeof(converters)/sizeof(*converters);

  if(!strcmp(from, to)) {
    return converter_noop;
  }

  for(i = 0 ; i < n ; i++) {
    if(!strcmp(from, converters[i].from) && !strcmp(to,converters[i].to)) {
      return converters[i].func;
    }
  }
  warn("unknown charset conversion %s..%s.", from, to);
  fprintf(stderr, "known conversions are:\n");
  for(i = 0 ; i < n ; i++) {
    fprintf(stderr, "  %s..%s\n", converters[i].from, converters[i].to);
  }
  return converter_noop;
}

/*
 * thash - target hash.
 * Needless to say, any change here must be checked against the
 * code using this hash in the run-time routine
 */

/* 1024 entries, means at least 8 kB, plus 8 bytes per string,
 * plus the lengths of strings
 */
#define TH_BITS 10
#define TH_SIZE (1 << TH_BITS)
#define TH_MASK (TH_SIZE - 1)
#define TH_BMASK ((1 << (16 - TH_BITS)) - 1)

static int compute_th_value(const char *t)
{
  const uchar *u = (const uchar *) t;
  unsigned a, b;

  a = 0;
  while(*u) {
    b = (a>>15) & 1;
    a <<= 1;
    a |= b;
    a += *u++;
  }
  b = (a >> TH_BITS) & TH_BMASK;
  a &= TH_MASK;
  a ^= b;
  return a;
}

/*
 * make a big langs.c file from all supplied lang names.
 */


/* decomposes a string <lang><white><encoding>, returning
 * 0 if s is badly formatted.
 * needs a 3-byte buffer in lang
 */
#define LANG_LEN 3
static int parse_linguas_item(const char *s, char *lang, const char **charset)
{
  if(*s <'a' || *s >= 'z') return 0;
  *lang++ = *s++;
  if(*s <'a' || *s >= 'z') return 0;
  *lang++ = *s++;
  *lang = 0;
  if(*s != ' ' && *s != '\t') return 0;
  while(*s == ' ' || *s == '\t') s++;
  *charset = get_canon_cset_name(s);
  return 1;
}

static void make(void)
{
  da *d;
  da *th[TH_SIZE];
  oh *o, *oref;
  int i, n, j, m;
  FILE *f;
  poe *eref;
  char tmp[20];
  char *t;
  char lang[LANG_LEN];
  da *langs;
  const char *from_charset, *to_charset;
  converter_t converter;
  int numref = 0;   /* number of entries in the reference */
  int numtransl;    /* number of translated entries */

  langs = da_new();

  d = da_new();
  parse_oipl_file("po/LINGUAS", d);

  oref = o_new();
  parse_po_file("po/messages.pot", oref, 1);

  f = fopen(LANGS_C, "w");
  if(f == NULL) {
    fatal("cannot open " LANGS_C);
  }

  fprintf(f, "\
/*\n\
 * " LANGS_C " - tables for all languages\n\
 *\n\
 * This file was generated by " TOOLNAME " version " VERSION " on %s\n\
 * Do not change this file!\n\
 *\n\
 * For more info, refer to file " DOCNAME "\n\
 */\n\n", now());

  fprintf(f, "#include \"config.h\"\n");
  fprintf(f, "#include \"i18nconf.h\"\n\n");
  fprintf(f, "#if CONF_WITH_NLS\n\n");
  fprintf(f, "#include \"langs.h\"\n\n");

  /* generate the default strings table, and store the
   * name of the key string in msgstr
   */
  fprintf(f, "/*\n * The keys for hash tables below.\n */\n\n");

  m = o_len(oref);
  for(j = 0 ; j < m ; j++) {
    eref = o_nth(oref, j);
    if(eref->kind == KIND_NORM) {
      sprintf(tmp, "nls_key_%d", j);
      eref->msgstr = xstrdup(tmp);
      fprintf(f, "static const char %s [] = ", tmp);
      print_canon(f, eref->msgid.key, "  ");
      fprintf(f, ";\n");
      numref++;
    }
  }
  fprintf(f, "\n\n");

  /* for each language, generate a hash table, pointing
   * back to the keys output above
   */
  n = da_len(d);
  for(i = 0 ; i < n ; i++) {

    /* clear target hash */
    for(j = 0 ; j < TH_SIZE ; j++) {
      th[j] = 0;
    }

    /* obtain destination charset from LINGUAS */
    t = da_nth(d, i);
    if(!parse_linguas_item(t, lang, &to_charset)) {
      warn("po/LINGUAS: bad lang/charset specification \"%s\"", t);
      continue;
    }

    /* read translations */
    o = o_new();
    sprintf(tmp, "po/%s.po", lang);
    parse_po_file(tmp, o, 0);

    { /* get the source charset from the po file */
      ae_t a;
      poe *e = o_find(o, "");
      if(e == NULL || !parse_ae(e->msgstr, &a)) {
        warn("%s: bad administrative entry", tmp);
        continue;
      }
      from_charset = get_canon_cset_name(a.charset);
    }
    da_add(langs, xstrdup(lang));

    converter = get_converter(from_charset, to_charset);

    /* compare o to oref */
    numtransl = 0;
    m = o_len(o);
    for(j = 0 ; j < m ; j++) {
      poe *e = o_nth(o, j);
      if((e->kind == KIND_NORM) && strcmp("", e->msgstr)) {
        eref = o_find(oref, e->msgid.key);
        if(eref) {
          int a = compute_th_value(e->msgid.key);
          if(th[a] == 0) {
            th[a] = da_new();
          }
          da_add(th[a], eref->msgstr);
          /* translate into destination encoding */
          converter(e->msgstr);
          da_add(th[a], e->msgstr);
          numtransl++;
        }
      }
    }

    /* print stats if some entries are missing */
    if(numtransl < numref) {
      printf("lang %s: %d untranslated entr%s\n",
          lang, numref - numtransl, (numref - numtransl == 1)? "y" : "ies");
    }

    /* dump the hash table */
    fprintf(f, "/*\n * hash table for lang %s.\n */\n\n", lang);
    for(j = 0 ; j < TH_SIZE ; j++) {
      if(th[j] != 0) {
        int ii, nn;
        fprintf(f, "static const char * const msg_%s_hash_%d[] = {\n", lang, j);
        nn = da_len(th[j]);
        for(ii = 0 ; ii < nn ; ii+=2) {
          fprintf(f, "  %s, ", (char *) da_nth(th[j],ii));
          print_canon(f, da_nth(th[j],ii+1), "    ");
          fprintf(f, ",\n");
        }
        fprintf(f, "  0\n};\n\n");
      }
    }
    fprintf(f, "static const char * const * const msg_%s[] = {\n", lang);
    for(j = 0 ; j < TH_SIZE ; j++) {
      if(th[j]) {
        fprintf(f, "  msg_%s_hash_%d,\n", lang, j);
        da_free(th[j]);
      } else {
        fprintf(f, "  0,\n");
      }
    }
    fprintf(f, "  0\n};\n\n");

    /* free this po */
    o_free(o);
  }

  /* print a lang table */
  fprintf(f, "/*\n * the table of available langs.\n */\n\n");
  n = da_len(langs);
  for(i = 0 ; i < n ; i++) {
    t = da_nth(langs, i);
    fprintf(f, "\
static const struct lang_info lang_%s = { \"%s\", msg_%s };\n", t, t, t);
  }
  fprintf(f, "\n");
  fprintf(f, "const struct lang_info * const langs[] = {\n");
  for(i = 0 ; i < n ; i++) {
    t = da_nth(langs, i);
    fprintf(f, "  &lang_%s, \n", t);
  }
  fprintf(f, "  0,\n};\n\n#endif /* CONF_WITH_NLS */\n");
  fclose(f);
  da_free(langs);
  da_free(d);
}

/*
 * translate
 */

static void translate(char *lang, char * from)
{
  FILE *g;
  pcati p;
  char *to;
  char po[10];
  const char *from_charset, *to_charset;

  { /* build destination filename */
    int len = strlen(from);
    if(len < 2 || from[len-1] != 'c' || from[len-2] != '.') {
      warn("I only translate .c files");
      return;
    }
    to = xmalloc(len+3);
    strcpy(to, from);
    strcpy(to+len-2, ".tr.c");
  }
  g = fopen(to, "w");
  if(g == NULL) {
    warn("cannot create %s\n", to);
    return;
  }
  free(to);
  p.f = g;

  to_charset = NULL;
  { /* obtain destination charset from LINGUAS */
    da *d = da_new();
    int i, n;
    parse_oipl_file("po/LINGUAS", d);

    n = da_len(d);
    for(i = 0 ; i < n ; i++) {
      char *t = da_nth(d, i);
      char l[LANG_LEN];
      if(! parse_linguas_item(t, l, &to_charset)) {
        warn("po/LINGUAS: bad lang/charset specification \"%s\"", t);
      } else if(!strcmp(lang, l)) {
        break;
      }
    }
  }
  if(to_charset == NULL) {
    warn("cannot find destination charset.");
    to_charset = "unknown";
  }

  /* read all translations */
  p.o = o_new();
  sprintf(po, "po/%s.po", lang);
  parse_po_file(po, p.o, 0);

  { /* get the source charset from the po file */
    ae_t a;
    poe *e = o_find(p.o, "");
    if(e == NULL || !parse_ae(e->msgstr, &a)) {
      warn("%s: bad administrative entry", po);
      goto fail;
    }
    from_charset = get_canon_cset_name(a.charset);
  }

  p.conv = get_converter(from_charset, to_charset);

  parse_c_file(from, pca_translate, &p);
fail:
  o_free(p.o);
  fclose(g);
}

/*
 * main
 */

int main(int argc, char **argv)
{
  if(argc < 2) goto usage;
  if(!strcmp(argv[1], "xgettext")) {
    if(argc != 2) goto usage;
    xgettext();
    exit(0);
  } else if(!strcmp(argv[1], "update")) {
    if(argc != 3) goto usage;
    update(argv[2]);
    exit(0);
  } else if(!strcmp(argv[1], "make")) {
    if(argc != 2) goto usage;
    make();
    exit(0);
  } else if(!strcmp(argv[1], "translate")) {
    if(argc != 4) goto usage;
    translate(argv[2], argv[3]);
    exit(0);
  } else if(!strcmp(argv[1], "--version")) {
    printf("version " VERSION "\n");
    exit(0);
  }
usage:
  fprintf(stderr, "\
Usage: " TOOLNAME " command\n");

  fprintf(stderr, "\
Commands are:\n\
  xgettext       scans source files listed in POTFILES.in \n\
                 and (re)creates messages.pot\n\
  update xx.po   compares xx.po to the current messages.pot \n\
                 and creates a new xx.po (new entries added, old \n\
                 entries commented out)\n\
  translate xx from.c\n\
                 translates from.c into from.tr.c for language xx.\n\
  make           takes all languages listed in file LINGUAS \n\
                 and creates the C file(s) for the project\n");

  fprintf(stderr, "\
\n\
Note: "
TOOLNAME
" is a very limited gettext clone, with some compatibility \n\
with the original gettext. To have more control on your po files, \n\
please use the original gettext utilities. You will still need this \n\
tool to create the C file(s) at the end, though.\n");

  exit(EXIT_FAILURE);
}
