/*
 * bug.c - Basic Unencumbering Gettext, a minimal gettext-like tool
 *         (any better name is welcome)
 *
 * Copyright (c) 2001 Laurent Vogel
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Bugs and limitations:
 * - free comments in po files are put to the end when updating
 * - only _() and N_() macros are handled
 * - not 100% compatible with gettext po files
 * - some weird messages
 * - trigraphs are not handled (this is a feature actually !)
 * 
 */
 
/* 
 * Structure of this file
 * (in this order, as each function calls functions above):
 * - library support (errors, memory, ...)
 * - basic data structures (string, dynamic array, hash, ...)
 * - po-file data structure
 * - input file with line counting and ability to go backwards
 * - low level lexical parsing for sh-like and C syntax.
 * - high level parsers
 * - po file internal operations
 * - the three tool commands (xgettext, update, make)
 * - main()
 */

/* 
 * TODO list
 * - better algorithm for merging po files (keep the order of entries)
 * - parse_po_file is a mess
 * - o_free()
 * - use strerror() to report errors
 * - use #~ for old entries
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define VERSION "0.1a"
 
#define TOOLNAME "bug"
#define DOCNAME  "doc/nls.txt"

/* 
 * typedefs
 */

typedef unsigned char uchar;

/* 
 * errors
 */
 
void warn(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "Warning: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

void fatal(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "Fatal: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}

/*
 * memory
 */

void * xmalloc(size_t s)
{
  void * a = calloc(1, s);
  if(a == 0) fatal("memory");
  return a;
}

void * xrealloc(void * b, size_t s)
{
  void * a = realloc(b, s);
  if(a == 0) fatal("memory");
  return a;
}

/*
 * xstrdup
 */

char * xstrdup(char *s)
{
  int len = strlen(s);
  char *a = xmalloc(len+1);
  strcpy(a,s);
  return a;
}

/*
 * now
 */

char * now(void)
{
  time_t t = time(0);
  return ctime(&t);  
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

void da_grow(da *d)
{
  if(d->size == 0) {
    d->size = DA_SIZE;
    d->buf = xmalloc(d->size * sizeof(void*));
  } else {
    d->size *= 4;
    d->buf = xrealloc(d->buf, d->size * sizeof(void*));
  }
}

da * da_new(void)
{
  da *d = xmalloc(sizeof(*d));
  d->size = 0;
  d->len = 0;
  return d;
}

void da_free(da *d)
{
  free(d->buf);
  free(d);
}

int da_len(da *d)
{
  return d->len;
}

void * da_nth(da *d, int n)
{
  return d->buf[n];
}

void da_add(da *d, void *elem)
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

str * s_new(void)
{
  str *s = xmalloc(sizeof(*s));
  s->size = 0;
  s->len = 0;
  return s;
}

void s_grow(str *s)
{
  if(s->size == 0) {
    s->size = STR_SIZE;
    s->buf = xmalloc(s->size);
  } else {
    s->size *= 4;
    s->buf = xrealloc(s->buf, s->size);
  }
}

void s_free(str *s) 
{
  if(s->size) {
    free(s->buf);
  }
  free(s);
}

void s_addch(str *s, char c) 
{
  if(s->len >= s->size) {
    s_grow(s);
  }
  s->buf[s->len++] = c;
}

void s_addstr(str *s, char *t) 
{
  while(*t) {
    s_addch(s, *t++);
  }
}

/* add a trailing 0 if needed and release excess mem */
char * s_close(str *s)
{
  if(s->size == 0) {
    s->buf = xmalloc(1);
    s->buf[0] = 0;
    return s->buf;
  }
  xrealloc(s->buf, s->len+1);
  s->buf[s->len] = 0;
  return s->buf;
}

char * s_detach(str *s)
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

hash * h_new(void)
{
  hash *h = xmalloc(sizeof(*h));
  return h;
}

/* a dumb one */
unsigned compute_hash(char *t)
{
  unsigned m = 0;
  unsigned u; 
  
  while(*t) {
    u = (m >> 15) & 1;
    m += *t++;
    m <<= 1;
  }
  return m;
}

void * h_find(hash *h, char *key)
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

void h_insert(hash *h, void *k)
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

oh * o_new(void)
{
  oh *o = malloc(sizeof(*o));
  o->h = h_new();
  o->d = da_new();
  return o;
}

void o_free(oh *o)
{
  /* TODO */
}

void * o_find(oh *o, char *t)
{
  return h_find(o->h, t);
}

void o_insert(oh *o, void *k)
{
  da_add(o->d, k);
  h_insert(o->h, k);
}

void o_add(oh *o, void *k)
{
  da_add(o->d, k);
}

int o_len(oh *o)
{
  return da_len(o->d);
}

void * o_nth(oh *o, int n)
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

ref * ref_new(char *fname, int lineno)
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
  char *refstr;    /* a char containing the references */
  char *msgstr;    /* the translation */
} poe;

poe * poe_new(char *t)
{
  poe *e = xmalloc(sizeof(*e));
  e->msgid.key = t;
  e->kind = KIND_NORM;
  e->comment = "";
  e->refs = da_new();
  e->msgstr = "";
  e->refstr = "";
  return e;
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

void irefill(IFILE *f)
{
  if(f->size > BACKSIZ) {
    memmove(f->buf, f->buf + f->size - BACKSIZ, BACKSIZ);
    f->size = BACKSIZ;
    f->index = f->size;
  }
  f->size += fread(f->buf + f->size, 1, READSIZ, f->fh);
}
    
IFILE *ifopen(char *fname)
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

void ifclose(IFILE *f)
{
  fclose(f->fh);
  free(f);
}

void iback(IFILE *f)
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

void ibackn(IFILE *f, int n)
{
  f->index -= n;
  if(f->index < 0) {
    fatal("too far backward");
  }
}

int igetc(IFILE *f)
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
int inextsh(IFILE *f)
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
int inextc(IFILE *f)
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


int try_eof(IFILE *f) 
{
  int c = igetc(f);
  if(c == EOF) {
    return 1;
  } else {
    iback(f);
    return 0;
  }
}

int try_c_comment(IFILE *f)
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

int try_white(IFILE *f)
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

int try_c_white(IFILE *f)
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
int get_c_string(IFILE *f, str *s)
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

/* like get, but do not accumulate the result */
int try_c_string(IFILE *f)
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
      } 
    } else if(c == '"') {
      return 1;
    } 
  }
}


/*
 * parse c files
 * put strings surrounded by _("...") or N_("...") into the ordered-hash
 * state means :
 * 0 outside names, 1 after 'N', 2 after '_', 3 after '(', 4 in a name
 */

void parse_c_file(char *fname, oh *o)
{
  int c;
  int state;
  poe *e;
  ref *r;
  str *s;
  char *t;
  int lineno;
        
  IFILE *f = ifopen(fname);
  if(f == NULL) {
    warn("could not open file");
    return;
  }

  state = 0;
  for(;;) {
    c = inextc(f);
    if(c == EOF) {
      break;
    } else if(c == '/') {
      iback(f);
      try_c_comment(f);
      state = 0;
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
          warn("_(\"...\" with no closing )");
          t = s_detach(s);
          warn("the string is %s", t);
          free(t);
          state = 0;
          continue;
        }
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
        da_add(e->refs, r);
      } else {
        iback(f);
        try_c_string(f);
      }
    } else if(c == '(') {
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
  }
  ifclose(f);
}

/*
 * parse po files
 */


void parse_po_file(char *fname, oh *o)
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
        }
        warn("stray ref ignored in %s:%d", fname, f->lineno);
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
    } else {
      e = poe_new(s_detach(msgid));
      e->msgstr = s_detach(msgstr);
      if(refstr) {
        e->refstr = s_detach(refstr);
        refstr = 0;
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

void parse_oipl_file(char *fname, da *d)
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
 * print string in canonical format
 */
 
void print_canon(FILE *f, char *t, char *prefix)
{
  unsigned a;
  
  if(strchr(t, '\n')) {
    fprintf(f, "\"\"\n%s", prefix);
  }
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

char * refs_to_str(da *refs)
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
 
void po_convert_refs(oh *o)
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

void print_po_file(FILE *f, oh *o)
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

void update(char *fname)
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
  parse_po_file("po/messages.pot", o1);

  /* rename the po file (backup) */
  s = s_new();
  s_addstr(s, fname);
  s_addstr(s, ".bak");
  bfname = s_detach(s);
  
  if(rename(fname, bfname)) {
    warn("cannot rename file %s, cancelled", fname);
    return;
  }
  
  /* parse the po file */
  o2 = o_new();
  parse_po_file(bfname, o2);

  /* scan o1 and o2, merging the two */
  n1 = o_len(o1);
  n2 = o_len(o2);
  o = o_new();
  /* TODO, better algorithm to keep the order of entries... */
  
  /* first, update entries in the po file */
  for(i2 = 0 ; i2 < n2 ; i2 ++) {
    e2 = o_nth(o2, i2);
    if(e2->kind == KIND_COMM) {
      o_add(o, e2);
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
      e = o_find(o2, e1->msgid.key);
      if(!e) {
        o_add(o, e1);
        numuntransl++;
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

void xgettext(void)
{
  da *d;
  oh *o;
  int i, n;
  FILE *f;
  char *fname;
  
  d = da_new();
  parse_oipl_file("po/POTFILES.in", d);
  
  o = o_new();
  n = da_len(d);
  for(i = 0 ; i < n ; i++) {
    parse_c_file(da_nth(d,i), o);
  }
  
  po_convert_refs(o);
  
  fname = "po/messages.pot";
  f = fopen(fname, "w");
  if(f == NULL) {
    fatal("couldn't create %s", fname);
  }
  fprintf(f, "\
# \n\
# messages.pot - the reference template for other po files\n\
# \n\
# This file was generated by " TOOLNAME " on %s\
# Do not change this file!\n\
# \n\
# For more info, refer to file " DOCNAME "\n\
# \n\n", now());
  
  print_po_file(f, o);
  fclose(f);
}

/*
 * iso_to_atari : convert in situ iso latin 1 to atari ST encoding
 */

unsigned char i2a[] = {
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

void iso_to_atari(char *s)
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

/*
 * thash - target hash. 
 * Needless to say, any change here must be checked against the
 * code using this hash in the run-time routine
 */

/* 1024 entries, means at least 8 KB, plus 8 bytes per string,
 * plus the lengths of strings 
 */
#define TH_BITS 10
#define TH_SIZE (1 << TH_BITS)
#define TH_MASK (TH_SIZE - 1)
#define TH_BMASK ((1 << (16 - TH_BITS)) - 1)

int compute_th_value(char *t)
{
  uchar *u = (uchar *) t;
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

void make(void)
{
  da *d;
  da *th[TH_SIZE];
  oh *o, *oref;
  int i, n, j, m;
  FILE *f;
  poe *eref;
  char tmp[20];
  char *t;
  int numref = 0;   /* number of entries in the reference */
  int numtransl;    /* number of translated entries */
      
  d = da_new();
  parse_oipl_file("po/LINGUAS", d);
  
  oref = o_new();
  parse_po_file("po/messages.pot", oref);
  
  f = fopen("langs.c", "w");
  if(f == NULL) {
    fatal("cannot open langs.c");
  }
  
  fprintf(f, "\
/*\n\
 * langs.c - tables for all languages\n\
 *\n\
 * This file was generated by " TOOLNAME " on %s\
 * Do not change this file!\n\
 *\n\
 * For more info, refer to file " DOCNAME "\n\
 */\n\n", now());
  
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
      fprintf(f, "static char %s [] = ", tmp);
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
    
    /* read translations */
    o = o_new();
    t = da_nth(d, i);
    if(strlen(t) > 2) {
      warn("bad lang name %s", t);
      continue;
    }
    sprintf(tmp, "po/%s.po", t);
    parse_po_file(tmp, o);
    
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
          /* translate into atari encoding */
          iso_to_atari(e->msgstr);
          da_add(th[a], e->msgstr);
          numtransl++;
        } 
      }
    }
    
    /* print stats if some entries are missing */
    if(numtransl < numref) {
      printf("lang %s: %d untranslated entries\n", t, numref - numtransl);
    }
    
    /* dump the hash table */
    fprintf(f, "/*\n * hash table for lang %s.\n */\n\n", t);
    for(j = 0 ; j < TH_SIZE ; j++) {
      if(th[j] != 0) {
        int ii, nn;
        fprintf(f, "static char *msg_%s_hash_%d[] = {\n", t, j);
        nn = da_len(th[j]);
        for(ii = 0 ; ii < nn ; ii+=2) {
          fprintf(f, "  %s, ", (char *) da_nth(th[j],ii));
          print_canon(f, da_nth(th[j],ii+1), "    ");
          fprintf(f, ",\n");
        }
        fprintf(f, "  0\n};\n\n");
      }
    }
    fprintf(f, "static char **msg_%s[] = {\n", t);
    for(j = 0 ; j < TH_SIZE ; j++) {
      if(th[j]) {
        fprintf(f, "  msg_%s_hash_%d,\n", t, j);
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
  for(i = 0 ; i < n ; i++) {
    t = da_nth(d, i);
    fprintf(f, "\
static struct lang_info lang_%s = { \"%s\", msg_%s };\n", t, t, t);
  }
  fprintf(f, "\n");  
  fprintf(f, "struct lang_info *langs[] = {\n");
  for(i = 0 ; i < n ; i++) {
    t = da_nth(d, i);
    fprintf(f, "  &lang_%s, \n", t);
  }
  fprintf(f, "  0,\n};\n\n");
  fclose(f);
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
  } else if(!strcmp(argv[1], "--version")) {
    printf("version " VERSION "\n");
    exit(0);
  } 
usage:
  fprintf(stderr, "\
Usage: " TOOLNAME " command\n\
Commands are:\n\
  xgettext       scans source files listed in POTFILES.in \n\
                 and (re)creates messages.pot\n\
  update xx.po   compares xx.po to the current messages.pot \n\
                 and creates a new xx.po (new entries added, old \n\
                 entries commented out)\n\
  make           takes all languages listed in file LINGUAS \n\
                 and creates the C file(s) for the project\n\
\n\
Note: " 
TOOLNAME 
" is a very limited gettext clone, with some compatibility \n\
with the original gettext. To have more control on your po files, \n\
please use the original gettext utilities. You will still need this \n\
tool to create the C file(s) at the end, though.\n");
  
  exit(1);
}

