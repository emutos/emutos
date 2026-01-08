/*
 * langs.h - internal definitions for the nls translation tables
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef LANGS_H
#define LANGS_H

typedef unsigned short nls_key_offset;

struct lang_info {
  char name[3]; /* LANG_LEN */
  const char *translations;
  const nls_key_offset *offsets;
};

extern struct lang_info const langs[];

#endif /* LANGS_H */
