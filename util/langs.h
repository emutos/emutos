/*
 * langs.h - internal definitions for the nls translations tables
 *
 * Copyright (c) 2001 Laurent Vogel
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _LANGS_H
#define _LANGS_H
 
struct lang_info {
  char *name;
  char ***hash;
};

extern struct lang_info *langs[];
 
#endif _LANGS_H
