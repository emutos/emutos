/*
 * dumpkbd.c : dump the TOS keyboard tables into a KEYTBL.TBL file format
 *
 * Copyright (c) 2001-2005 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <osbind.h>
#include <stdarg.h>
#include "doprintf.h"
#include "string.h"

/*
 * fake stdio stuff
 */

extern void exit(int status);

struct file { int f; };
#define FILE struct file
FILE stdin[] = { { -1 } };
#define stdout stdin
#define stderr stdout

static int stdio_f;

static void fprintf_outc(int c)
{
    if( c == '\n') {
        Fwrite(stdio_f, 2, "\r\n");
    } else {
        char u = c;
        Fwrite(stdio_f, 1, &u);
    }
}

static int vfprintf(FILE *f, const char *fmt, va_list ap)
{
    stdio_f = f->f;
    return doprintf(fprintf_outc, fmt, ap);
}

int printf(const char *fmt, ...)
{
    int n;
    va_list ap;
    va_start(ap, fmt);
    n = vfprintf(stdout, fmt, ap);
    va_end(ap);
    return n;
}

int fprintf(FILE *f, const char *fmt, ...)
{
    int n;
    va_list ap;
    va_start(ap, fmt);
    n = vfprintf(f, fmt, ap);
    va_end(ap);
    return n;
}


static struct { char *mode; int m ; } fopen_table[] = {
  { "r", 0 }, { "rb", 0 }, { "w", 1 }, { "wb", 1 },
};

int stdio_fileno = 0;
FILE stdio_files[10];

FILE * fopen(const char *fname, const char *mode)
{
    int i;
    int m = -1;
    FILE *f;
    for(i = 0 ; i < sizeof fopen_table / sizeof *fopen_table; i++) {
        if(!strcmp(mode, fopen_table[i].mode)) {
            m = fopen_table[i].m;
            break;
        }
    }
    if(m == -1) return 0;
    if(stdio_fileno > sizeof stdio_files / sizeof *stdio_files) return 0;
    f = &stdio_files[stdio_fileno++];
    f->f = Fopen(fname, m);
    if(m == 1 && f->f == -33) f->f = Fcreate(fname, 0);
    if(f->f < 0) return 0;
    return f;
}

#define fclose(_f) Fclose(_f->f)
#define fread(_b, _n, _s, _f) Fread(_f->f, _n*_s, _b)
#define fwrite(_b, _n, _s, _f) Fwrite(_f->f, _n*_s, _b)
#define getchar() ((int)(0xFF & Cconin()))

/*
 * fake stdlib stuff
 */

#define malloc(_s) Malloc(_s)
#define free(_p) Mfree(_p)
#define EXIT_FAILURE 1
#define NULL ((void *)0)

/*
 *
 */

struct ktable {
    char **norm;
    char **shft;
    char **caps;
};

struct ktbl {
    char norm[128];
    char shft[128];
    char caps[128];
};

void get_tables(struct ktbl *ktbl)
{
    long old;
    struct ktable *ktable = (struct ktable *) Keytbl(-1L, -1L, -1L);

    old = Super(0);

    memmove(ktbl->norm, ktable->norm, 128);
    memmove(ktbl->shft, ktable->shft, 128);
    memmove(ktbl->caps, ktable->caps, 128);

    Super(old);
}

void dump_table(char *table, char *name, FILE *out, char *prefix)
{
    int i;

    fprintf(out,
"const BYTE keytbl_%s_%s[] = {\n",
            prefix, name
            );
    for(i = 0 ; i < 128 ; i++) {
        int c = table[i];
        if((i & 7) == 0) {
            fprintf(out, "    ");
        }
        switch(c) {
        case '\\':
        case '"':
        case '\'':
            fprintf(out, "\'\\%c\', ", c);
            break;
        default:
            if(c >= 0 && c <= 8) {
                fprintf(out, "   %d, ", c);
            } else if(c >= 32 && c <= 126) {
                fprintf(out, " \'%c\', ", c);
            } else {
                fprintf(out, "0x%02x, ", c & 0xFF);
            }
            break;
        }
        if((i & 7) == 7) {
            fprintf(out, "\n");
        }
    }
    fprintf(out, "};\n\n");
}

void dump_tables(struct ktbl *ktbl, char *fname, char *name)
{
    char prefix[3];
    char ucprefix[3];
    FILE *out = fopen(fname, "w");
    if(out == NULL) {
        fprintf(stderr, "cannot open %s\n", fname);
        exit(EXIT_FAILURE);
    }

    /* the code below is pathetic ! */
    prefix[0] = (name[0] & 0x1f) + 'a' - 1;
    prefix[1] = (name[1] & 0x1f) + 'a' - 1;
    prefix[2] = 0;
    ucprefix[0] = prefix[0] - 32;
    ucprefix[1] = prefix[1] - 32;
    ucprefix[2] = 0;

    fprintf(out,
"/*\n"
" * %s - a keyboard layout definition\n"
" *\n"
" * Copyright (c) 2002 EmuTOS development team\n"
" *\n"
" * This file is distributed under the GPL, version 2 or at your\n"
" * option any later version.  See doc/license.txt for details.\n"
" */\n",
          fname);

    /* the code below is even worse ! */

    fprintf(out,
"\n"
"const BYTE keytbl_%s_norm[];\n"
"const BYTE keytbl_%s_shft[];\n"
"const BYTE keytbl_%s_caps[];\n"
"const BYTE keytbl_%s_altnorm[];\n"
"const BYTE keytbl_%s_altshft[];\n"
"const BYTE keytbl_%s_altcaps[];\n"
"\n"
"const struct keytbl keytbl_%s = {\n"
"    keytbl_%s_norm, \n"
"    keytbl_%s_shft, \n"
"    keytbl_%s_caps, \n"
"    keytbl_%s_altnorm, \n"
"    keytbl_%s_altshft, \n"
"    keytbl_%s_altcaps, \n"
"    NULL, \n"
"};\n"
"\n",
            prefix, prefix, prefix, prefix, prefix, prefix,
            prefix,
            prefix, prefix, prefix, prefix, prefix, prefix
            );

    dump_table(ktbl->norm, "norm", out, prefix);
    dump_table(ktbl->shft, "shft", out, prefix);
    dump_table(ktbl->caps, "caps", out, prefix);

    fprintf(out,
"\n"
"/* TODO - the tables below were not filled by the tool.\n"
" * they should contain couples of (scan code, char code), ended by zero.\n"
" */\n"
            );

    fprintf(out,
"\n"
"const BYTE keytbl_%s_altnorm[] = {\n"
"    0,\n"
"};\n",
            prefix
            );
    fprintf(out,
"\n"
"const BYTE keytbl_%s_altshft[] = {\n"
"    0,\n"
"};\n",
            prefix
            );
    fprintf(out,
"\n"
"const BYTE keytbl_%s_altcaps[] = {\n"
"    0,\n"
"};\n",
            prefix
            );

    fclose(out);
}


int main(int argc, char **argv)
{
    struct ktbl ktbl;
    char name[3];
    char fname[10];

    printf("Enter a two-letter name for this keyboard: ");
    name[0] = getchar();
    name[1] = getchar();
    name[2] = 0;
    printf("\n");

    get_tables(&ktbl);

    sprintf(fname, "keyb_%s.h", name);
    printf("Dumping keyboard in file %s\n", fname);
    dump_tables(&ktbl, fname, name);

    printf("Done\n");

    return 0;
}

