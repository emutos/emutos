/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2013 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMSUPER_H
#define GEMSUPER_H

extern WORD     gl_mnclick;

typedef struct aespb {
    WORD *control;
    WORD *global;
    WORD *intin;
    WORD *intout;
    LONG *addrin;
    LONG *addrout;
} AESPB;

#endif
