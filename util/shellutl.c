/*
 * Shell utility functions.
 * These are factored functions that may be used by both the
 * AES, Desktop and embedded CLI (EmuCON2)
 *
 * Copyright (C) 2013-2022 The EmuTOS development team
 *
 * Authors:
 *  VB     Vincent Barrilliot
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <config.h>

#include "shellutl.h"
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


/*
 *  "Search next"-style routine to pick up each path component in a
 *  list of paths separated by ";" or ",".
 *  Returns a pointer to the start of the following component path
 *  there are no more paths to find.
 */
char *shellutl_find_next_path_component(const char *paths, char *dest)
{
    char last = 0;
    char *p;

    if (!paths)           /* precautionary */
        return NULL;

    /* check for end of PATH= env var */
    if (!*paths)
        return NULL;

    /* copy over path */
    for (p = (char*)paths; *p; )
    {
        if ((*p == ';') || (*p == ','))
            break;
        last = *p;
        *dest++ = *p++;
    }

    /* see if extra slash is needed */
    if ((last != '\\') && (last != ':'))
        *dest++ = '\\';

	*dest = '\0';

    /* point past terminating separator or nul */
    return *p ? p + 1 : p;
}

/*
 *  Return a drive number from a drive letter (lower or upper case)
 */
WORD shellutl_get_drive_number(char drive_letter) {
	return (drive_letter | 0x20) - 'a';
}
