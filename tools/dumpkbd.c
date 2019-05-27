/*
 * dumpkbd.c : dump the TOS keyboard tables into a KEYTBL.TBL file format
 *
 * Compile with:
 *      m68k-atari-mint-gcc -o DUMPKBD.TOS -Wall dumpkbd.c
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>
#include <string.h>

#define ARRAY_SIZE(array) ((int)(sizeof(array)/sizeof(array[0])))


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
"const char keytbl_%s_%s[] = {\n",
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
" * Copyright (C) 2002 The EmuTOS development team\n"
" *\n"
" * This file is distributed under the GPL, version 2 or at your\n"
" * option any later version.  See doc/license.txt for details.\n"
" */\n",
          fname);

    /* the code below is even worse ! */

    fprintf(out,
"\n"
"const char keytbl_%s_norm[];\n"
"const char keytbl_%s_shft[];\n"
"const char keytbl_%s_caps[];\n"
"const char keytbl_%s_altnorm[];\n"
"const char keytbl_%s_altshft[];\n"
"const char keytbl_%s_altcaps[];\n"
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
"const char keytbl_%s_altnorm[] = {\n"
"    0,\n"
"};\n",
            prefix
            );
    fprintf(out,
"\n"
"const char keytbl_%s_altshft[] = {\n"
"    0,\n"
"};\n",
            prefix
            );
    fprintf(out,
"\n"
"const char keytbl_%s_altcaps[] = {\n"
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
