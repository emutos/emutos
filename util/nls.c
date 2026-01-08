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

static const nls_key_offset *nls_offsets;
static const char *nls_translations;
/* from generated langs.c */
extern char const nls_key_strings[];
extern nls_key_offset const msg_en_offsets[];

/* initialisation */

void nls_init(void)
{
  nls_offsets = msg_en_offsets;
  nls_translations = nls_key_strings;
}

/* functions to query the lang database and to set the lang */

void nls_set_lang(const char *s)
{
  const struct lang_info *lang;

  for(lang = langs; lang->name[0]; lang++) {
    if(!strcmp(s, lang->name)) {
      nls_offsets = lang->offsets;
      nls_translations = lang->translations;
      return ;
    }
  }
}

const char *etos_gettext(unsigned long msgid)
{
  nls_key_offset offset;

/* check for empty string - often used in RSC - must return original address */
  if (msgid == 0)
    return nls_key_strings;

  /*
   * This function is also called on "real" strings, when they are compiled
   * into a resource file, and are not marked with N_() and thus are not
   * translated by "bug translate".
   */
  if (!(msgid & NLS_MSGID_FLAG))
    return (const char *)msgid;
  msgid &= ~NLS_MSGID_FLAG;

  /*
   * If we can ensure that nls_offsets is always set before this function is called,
   * the first check can be omitted
   * (currently this is not the case; in bios_init(), it may be called
   * before the current language has been set)
   */
  if (nls_offsets == NULL || (offset = nls_offsets[msgid - 1]) == 0)
    /* no translation, return original string */
    return nls_key_strings + msg_en_offsets[msgid - 1];
  return nls_translations + offset;
}

#endif /* CONF_WITH_NLS */
