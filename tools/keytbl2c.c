/*
 * keytbl2c.c : convert a KEYTBL.TBL file into C source code.
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <stdio.h>
#include <stdlib.h>

struct ktbl {
  char magic[2];
  char norm[128];
  char shft[128];
  char caps[128];
} ktbl;

void dump_table(char *table, char *name, FILE *out, char *prefix);
     
int main(int argc, char **argv)
{
  FILE *in ;
  FILE *out;
  size_t count;
  char prefix[3];
  char ucprefix[3];

  if(argc != 4) {
    fprintf(stderr, "usage: %s <in> <out> <prefix>\n", argv[0]);
    exit(1);
  }
  in = fopen(argv[1], "rb");
  out = fopen(argv[2], "w");

  count = fread(&ktbl, 1, sizeof(ktbl), in);
  if(count < sizeof(ktbl)) {
    fprintf(stderr, "short read\n");
    exit(1);
  }
  
  if((ktbl.magic[0] != 0x27) || (ktbl.magic[1] != 0x71)) {
    fprintf(stderr, "bad magic\n");
    exit(1);
  }

  /* the code below is pathetic ! */
  prefix[0] = (argv[3][0] & 0x1f) + 'a' - 1;
  prefix[1] = (argv[3][1] & 0x1f) + 'a' - 1;
  prefix[2] = 0;
  ucprefix[0] = prefix[0] - 32;
  ucprefix[1] = prefix[1] - 32;
  ucprefix[2] = 0;

  fprintf(out, 
"/*\n"
" * %s - a keyboard layout definition\n" 
" *\n"
" * Copyright (c) 2001 EmuTOS development team\n"
" *\n"
" * Authors:\n"
" *  xxx   xxx\n"
" *\n"
" * This file is distributed under the GPL, version 2 or at your\n"
" * option any later version.  See doc/license.txt for details.\n"
" */\n",
          argv[2]);

  /* the code below is even worse ! */

  fprintf(out, 
"\n"
"BYTE keytbl_%s_norm[];\n"
"BYTE keytbl_%s_shft[];\n"
"BYTE keytbl_%s_caps[];\n"
"BYTE keytbl_%s_altnorm[];\n"
"BYTE keytbl_%s_altshft[];\n"
"BYTE keytbl_%s_altcaps[];\n"
"\n"
"struct keytbl keytbl_%s = {\n"
"    keytbl_%s_norm, \n"
"    keytbl_%s_shft, \n"
"    keytbl_%s_caps, \n"
"    keytbl_%s_altnorm, \n"
"    keytbl_%s_altshft, \n"
"    keytbl_%s_altcaps, \n"
"};\n"
"\n",
          ucprefix,
          prefix, prefix, prefix, prefix, prefix, prefix,
          prefix,
          prefix, prefix, prefix, prefix, prefix, prefix
          );

  dump_table(ktbl.norm, "norm", out, prefix);
  dump_table(ktbl.shft, "shft", out, prefix);
  dump_table(ktbl.caps, "caps", out, prefix);

  fprintf(out, 
"\n"
"/* TODO - the tables below were not filled by the tool */\n"
          );

  fprintf(out, 
"\n"
"BYTE keytbl_%s_altnorm[] = {\n"
"    0,\n"
"};\n",
          prefix
          );
  fprintf(out, 
"\n"
"BYTE keytbl_%s_altshft[] = {\n"
"    0,\n"
"};\n",
          prefix
          );
  fprintf(out, 
"\n"
"BYTE keytbl_%s_altcaps[] = {\n"
"    0,\n"
"};\n",
          prefix
          );

  fclose(out);
  fprintf(stderr, "done.\n");
  exit(0);
}


void dump_table(char *table, char *name, FILE *out, char *prefix)
{
  int i;

  fprintf(out, 
"BYTE keytbl_%s_%s[] = {\n",
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
