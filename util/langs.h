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

struct lang_info {
  const char *name;
  const char * const * const *hash;
};

extern const struct lang_info * const langs[];

#endif /* LANGS_H */
