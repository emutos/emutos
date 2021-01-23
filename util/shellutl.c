/*
 * Shell utility functions.
 * These are factored functions that may be used by both the
 * AES, Desktop and embedded CLI (EmuCON)
 *
 * Copyright (C) 2013-2022 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "shellutl.h"
#include <config.h>
#include "string.h"

/*
 *  Search for a particular string in the DOS environment and return a
 *  value in the pointer pointed to by the first argument.  If the string
 *  is found, the value is a pointer to the first character after the
 *  string; otherwise it is a NULL pointer.
 */
void shellutl_getenv(const char *environment, const char *varname, char **out_value)
{
    char *p;
    WORD len;

    len = strlen(varname);
    *out_value = NULL;

    /*
     * scan environment string until double nul
     */
    for (p = (char*)environment; *p; )
    {
        if (strncmp(p, varname, len) == 0)
        {
            *out_value = p + len;
            break;
        }
        while(*p++) /* skip to end of current env variable */
            ;
    }
}
