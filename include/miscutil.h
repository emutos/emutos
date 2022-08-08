/*
 * miscutil.h - header for miscellaneous utility functions
 *
 * Copyright (C) 2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MISCUTIL_H
#define MISCUTIL_H

void build_root_path(char *path, char drive);
WORD extract_drive_number(const char *path);
void set_all_files(char *target);

#endif
