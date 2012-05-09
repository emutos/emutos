/*
 * nls.h - Definitions for Native Language Support
 *
 * Copyright (c) 2001 Emutos Development Team
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

#define N_(a) a

#if !CONF_WITH_NLS
# define _(a) (a)
# define gettext(a) (a)    /* Disable NLS / gettext completely */
#else
# define _(a) gettext(a)
const char *gettext(const char *);
#endif

/* initialisation */

void nls_init(void);

/* functions to query the lang database and to set the lang */

void nls_set_lang(const char *);

#endif /* NLS_H */
