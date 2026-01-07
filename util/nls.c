/*
 * nls.c - Native Language Support
 *
 * Copyright (C) 2001-2019 The EmuTOS Development Team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "nls.h"
#include "langs.h"
#include "string.h"
#include "i18nconf.h"

#if CONF_WITH_NLS

/* used by nlsasm.S */
const nls_key_offset *const *nls_hash;
const char *nls_translations;

void gettext_init(void); /* call each time the hash changes */

/* initialisation */

void nls_init(void)
{
  nls_hash = 0;
  gettext_init();
}

/* functions to query the lang database and to set the lang */

void nls_set_lang(const char *s)
{
  int i;

  for(i = 0 ; langs[i] ; i++) {
    if(!strcmp(s, langs[i]->name)) {
      nls_hash = langs[i]->hash;
      nls_translations = langs[i]->translations;
      gettext_init();
      return ;
    }
  }
}

#if !NLS_USE_ASM_HASH

#if NLS_USE_RAM_HASH
#define RAM_HASH_SIZE 1024
static const char *ram_hash[(RAM_HASH_SIZE + 10) * 2]; /* 1024 entries + 10 additional slots. */
#endif

void gettext_init(void)
{
#if NLS_USE_RAM_HASH
    memset(ram_hash, 0, sizeof(ram_hash));
#endif
}

#define TH_BITS 10
#define TH_SIZE (1 << TH_BITS)
#define TH_MASK (TH_SIZE - 1)
#define TH_BMASK ((1 << (16 - TH_BITS)) - 1)

static unsigned int compute_th_value(const char *t)
{
    const unsigned char *u = (const unsigned char *) t;
    unsigned short a, b;

    a = 0;
    while (*u)
    {
        a = (a << 1) | ((a >> 15) & 1);
        a += *u++;
    }
    b = (a >> TH_BITS) & TH_BMASK;
    a ^= b;
    a &= TH_MASK;
    return a;
}

/* from generated langs.c */
extern char const nls_key_strings[];

const char *gettext(const char *key)
{
    unsigned int hash;
    const nls_key_offset *chain;
    nls_key_offset cmp;
#if NLS_USE_RAM_HASH
    const char *entry;
    int i;
#define compute_ram_hash(key) (((((unsigned long)key << 3) + (((unsigned long)key << 9) >> 16)) & ((RAM_HASH_SIZE - 1) * (2 * sizeof(void *)))) / sizeof(void *))
#endif

/* check for empty string - often used in RSC - must return original address */
    if (nls_hash == NULL || *key == '\0')
        return key;

#if NLS_USE_RAM_HASH
    hash = compute_ram_hash(key);
    for (i = 0; i < 10; i++)
    {
        entry = ram_hash[hash++];
        if (entry == NULL)
            break;
        if (entry == key)
            return ram_hash[hash];
        hash++;
    }
    /* not found in ram hash; do normal lookup */
#endif

    hash = compute_th_value(key);
    if ((chain = nls_hash[hash]) != NULL)
    {
        while ((cmp = *chain++) != 0)
        {
            if (strcmp(&nls_key_strings[cmp], key) == 0)
            {
                /* strings are equal, return next string */
#if NLS_USE_RAM_HASH
                /* store found key in ram hash */
                hash = compute_ram_hash(key);
                for (i = 0; i < 10; i++)
                {
                    if (ram_hash[hash] == NULL)
                    {
                        ram_hash[hash++] = key;
                        ram_hash[hash] = &nls_translations[*chain];
                        break;
                    }
                    hash += 2;
                }
#endif
                key = &nls_translations[*chain];
                break;
            }
            /* the strings differ, next */
            chain++;
        }
    }

    /* not in hash, return original string */
    return key;
}

#endif /* NLS_USE_ASM_HASH */

#endif /* CONF_WITH_NLS */
