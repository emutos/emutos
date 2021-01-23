/*
 * Shell utility functions.
 * These are factored functions that may be used by both the
 * AES, Desktop and embedded CLI (EmuCON)
 *
 * Copyright (C) 2013-2022 The EmuTOS development team
 *
 * Authors:
 *  VB    Vincent Barrilliot
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _SHELLUTL_H
#define _SHELLUTL_H

#include <portab.h>

void  shellutl_getenv(const char *environment, const char *varname, char **out_value);
char *shellutl_find_next_path_component(const char *paths, char *dest);
WORD  shellutl_get_drive_number(char drive_letter);

#endif
