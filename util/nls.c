/*
 * nls.c - Native Language Support
 *
 * Copyright (c) 2001 Emutos Development Team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "portab.h"
 
#include "nls.h"
#include "langs.h"
#include "string.h"


static int the_lang;
static int num_of_langs;

/* used by nlsasm.S */

char ***nls_hash;

void gettext_init(void); /* call each time the hash changes */

/* initialisation */

void nls_init(void)
{
  int i;
  
  the_lang = -1;
  nls_hash = 0;
  gettext_init();
  
  for(i = 0 ; langs[i] ; i++) 
    ;
  num_of_langs = i;
}

/* functions to query the lang database and to set the lang */

void nls_set_lang(const char *s)
{
  int i;

  for(i = 0 ; i < num_of_langs ; i++) {
    if(!strcmp(s, langs[i]->name)) {
      the_lang = i;
      nls_hash = langs[i]->hash;
      gettext_init();
      return ;
    }
  }
  the_lang = 0;
}
  
const char *nls_nth_lang(int i)
{
  return langs[i]->name;
}

int nls_num_of_langs(void)
{
  return num_of_langs;
}

