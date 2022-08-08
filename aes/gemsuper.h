/*
 * gemsuper.h - header for EmuTOS AES function call handler
 *
 * Copyright (C) 2002-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMSUPER_H
#define GEMSUPER_H

typedef struct aespb {
    WORD *control;
    WORD *global;
    WORD *intin;
    WORD *intout;
    LONG *addrin;
    LONG *addrout;
} AESPB;

#endif
