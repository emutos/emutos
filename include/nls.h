/*
 * nls.h - Definitions for Native Language Support
 *
 * Copyright (C) 2001 The Emutos Development Team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef NLS_H
#define NLS_H

#include "i18nconf.h"

/* the gettext-like macros */

#if CONF_WITH_NLS

#define NLS_MSGID_FLAG 0x80000000UL

# define _(a) etos_gettext(a)
# define N_(a) ((char *)((unsigned long)(a) | NLS_MSGID_FLAG))
# define gettext(a) etos_gettext((unsigned long)(a))
const char *etos_gettext(unsigned long msgid);

/* initialisation */

void nls_init(void);

/* functions to query the lang database and to set the lang */

void nls_set_lang(const char *);

#else

/* Disable NLS / gettext completely */

# define _(a) (a)
# define N_(a) a
# define gettext(a) (a)

#endif

#endif /* NLS_H */
