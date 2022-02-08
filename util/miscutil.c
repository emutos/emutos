/*
 * miscutil.c - miscellaneous utility functions
 *
 * Copyright (C) 2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "emutos.h"
#include "string.h"
#include "miscutil.h"

/*
 * returns the drive number corresponding to the drive prefix (X:) in
 * the passed string
 *
 * returns  0->25 for drives A thru Z, or
 *          -1 if a valid drive prefix does not exist
 */
WORD extract_drive_number(const char *path)
{
    char c;

    if (path[0] && (path[1] == ':'))
    {
        c = toupper(path[0]);
        if ((c >= 'A') && (c <= 'Z'))
            return c - 'A';
    }

    return -1;
}
